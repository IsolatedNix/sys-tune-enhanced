#include "gui_main.hpp"

#include "tune.h"
#include "elm_overlayframe.hpp"
#include "elm_volume.hpp"
#include "gui_browser.hpp"
#include "gui_playlist.hpp"
#include "pm/pm.hpp"
#include "config/config.hpp"

namespace {
    constexpr const size_t num_steps = 20;
}

MainGui::MainGui() {
    m_status_bar    = new StatusBar();
}

tsl::elm::Element *MainGui::createUI() {
    auto frame = new SysTuneOverlayFrame();
    auto list  = new tsl::elm::List();

    u64 pid{}, tid{};
    pm::getCurrentPidTid(&pid, &tid);

    /* Current track. */
    list->addItem(this->m_status_bar, tsl::style::ListItemDefaultHeight * 2);

    /* Playlist. */
    auto queue_button = new tsl::elm::ListItem("Playlist");
    queue_button->setClickListener([](u64 keys) {
        if (keys & HidNpadButton_A) {
            tsl::changeTo<PlaylistGui>();
            return true;
        }
        return false;
    });
    list->addItem(queue_button);

    /* Browser. */
    auto browser_button = new tsl::elm::ListItem("Music browser");
    browser_button->setClickListener([](u64 keys) {
        if (keys & HidNpadButton_A) {
            tsl::changeTo<BrowserGui>();
            return true;
        }
        return false;
    });
    list->addItem(browser_button);

    list->addItem(new tsl::elm::CategoryHeader("Settings"));

    /* Whitelist mode toggle */
    auto whitelist_mode = new tsl::elm::ToggleListItem("Whitelist Mode", config::get_whitelist_mode());
    // take action when the state is changed
    whitelist_mode->setStateChangedListener([tid](bool state) {
        config::set_whitelist_mode(state);
    });

    // also take action when it is clicked
    whitelist_mode->setClickListener([tid](u64 keys) {
        if (keys & HidNpadButton_A) {
            if (config::get_whitelist_mode() && config::get_title_whitelist(tid) && config::get_autoplay_enabled()) {
                tunePlay();
            } else if (config::get_whitelist_mode() && !config::get_title_whitelist(tid)) {
                tuneQuit();
                tsl::goBack();
            }
            return true;
        }
        return false;
    });
    list->addItem(whitelist_mode);

    /* Whitelist current game toggle */
    if (tid != 0) {
        auto whitelist_game = new tsl::elm::ToggleListItem("Whitelist Current Game", config::get_title_whitelist(tid));
        // take action when the state is changed
        whitelist_game->setStateChangedListener([tid](bool state) {
            config::set_title_whitelist(tid, state);
        });

        // also take action when it is clicked
        whitelist_game->setClickListener([tid](u64 keys) {
            if (keys & HidNpadButton_A) {
                if (config::get_title_whitelist(tid) && config::get_whitelist_mode() && config::get_autoplay_enabled()) {
                    tunePlay();
                } else if (config::get_whitelist_mode() && !config::get_title_whitelist(tid)) {
                    tuneQuit();
                    tsl::goBack();
                }
                return true;
            }
            return false;
        });
        list->addItem(whitelist_game);
    }

    /* Title ID Playlist mode toggle */
    auto title_playlist_mode = new tsl::elm::ToggleListItem("Title ID Playlist Mode", config::get_title_playlist_mode());
    title_playlist_mode->setStateChangedListener([](bool state) {
        config::set_title_playlist_mode(state);
    });
    list->addItem(title_playlist_mode);


    /* Volume indicator */
    list->addItem(new tsl::elm::CategoryHeader("Volume Control"));

    /* Get initial volume. */
    float tune_volume = 1.f;
    float title_volume = 1.f;
    float default_title_volume = 1.f;

    tuneGetVolume(&tune_volume);
    tuneGetTitleVolume(&title_volume);
    tuneGetDefaultTitleVolume(&default_title_volume);

    auto tune_volume_slider = new ElmVolume("\uE13C", "Tune Volume", num_steps);
    tune_volume_slider->setProgress(tune_volume * num_steps);
    tune_volume_slider->setValueChangedListener([](u8 value){
        const float volume = float(value) / float(num_steps);
        tuneSetVolume(volume);
    });
    list->addItem(tune_volume_slider);

    // empty pid means we are qlaunch :)
    if (tid && pid) {
        auto title_volume_slider = new ElmVolume("\uE13C", "Game Volume", num_steps);
        title_volume_slider->setProgress(title_volume * num_steps);
        title_volume_slider->setValueChangedListener([tid](u8 value){
            const float volume = float(value) / float(num_steps);
            tuneSetTitleVolume(volume);
            config::set_title_volume(tid, volume);
        });
        list->addItem(title_volume_slider);
    }

    auto default_title_volume_slider = new ElmVolume("\uE13C", "Game Volume (default)", num_steps);
    default_title_volume_slider->setProgress(default_title_volume * num_steps);
    default_title_volume_slider->setValueChangedListener([](u8 value){
        const float volume = float(value) / float(num_steps);
        tuneSetDefaultTitleVolume(volume);
    });
    list->addItem(default_title_volume_slider);

    list->addItem(new tsl::elm::CategoryHeader("Play / Pause"));

    /* Autoplay on boot toggle */
    auto tune_autoplay = new tsl::elm::ToggleListItem("Autoplay on boot", config::get_autoplay_enabled(), "On", "Off");
    tune_autoplay->setStateChangedListener([](bool new_value) {
        config::set_autoplay_enabled(new_value);
        if (new_value) {
            tunePlay();
        }
    });
    list->addItem(tune_autoplay);

    /* Per title tune toggle. */
    auto tune_play = new tsl::elm::ToggleListItem("Tune", config::get_title_enabled(tid), "Play", "Pause");
    tune_play->setStateChangedListener([tid](bool new_value) {
        config::set_title_enabled(tid, new_value);
        if (new_value) {
            tunePlay();
        } else {
            tunePause();
        }
    });
    list->addItem(tune_play);

    /* Default title tune toggle. */
    auto tune_default_play = new tsl::elm::ToggleListItem("Tune (default)", config::get_title_enabled_default(), "Play", "Pause");
    tune_default_play->setStateChangedListener([](bool new_value) {
        config::set_title_enabled_default(new_value);
        if (new_value) {
            tunePlay();
        } else {
            tunePause();
        }
    });
    list->addItem(tune_default_play);

    list->addItem(new tsl::elm::CategoryHeader("Misc"));

    auto exit_button = new tsl::elm::ListItem("Close sys-tune-enhanced");
    exit_button->setClickListener([](u64 keys) {
        if (keys & HidNpadButton_A) {
            tuneQuit();
            tsl::goBack();
            return true;
        }
        return false;
    });
    list->addItem(exit_button);

    frame->setContent(list);

    return frame;
}

void MainGui::update() {
    static u8 tick = 0;
    /* Update status 4 times per second. */
    if ((tick % 15) == 0)
        this->m_status_bar->update();
    tick++;
}
