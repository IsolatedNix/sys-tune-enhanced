#include "gui_error.hpp"
#include "config/config.hpp"
#include "pm/pm.hpp"
#include "tune.h"

#include "elm_overlayframe.hpp"

namespace {

    char result_buffer[10];

}

ErrorGui::ErrorGui(const char *msg, Result rc) : m_msg(msg) {
    if (rc) {
        std::snprintf(result_buffer, 10, "2%03d-%04d", R_MODULE(rc), R_DESCRIPTION(rc));
        m_result = result_buffer;
    }
}

tsl::elm::Element *ErrorGui::createUI() {
    auto rootFrame = new SysTuneOverlayFrame();

    auto list = new tsl::elm::List();

    u64 pid{}, tid{};
    pm::getCurrentPidTid(&pid, &tid);

    list->addItem(new tsl::elm::CategoryHeader("Info"));
    auto systune_status = new tsl::elm::ListItem("Sys-tune Status",pm::systuneIsRunning() ? "Running" : "Not Running");
    list->addItem(systune_status);

    list->addItem(new tsl::elm::CategoryHeader("Error Message"));
    auto custom = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *drawer, u16 x, u16 y, u16 w, u16 h) {
        drawer->drawString("\uE150", false, x + (w / 2) - (90 / 2), y + 80, 90, 0xffff);
        auto [width, height] = drawer->drawString(this->m_msg, false, x + (w / 2) - (this->msgW / 2), y + 120, 25, 0xffff);
        if (msgW == 0) {
            msgW = width;
            msgH = height;
        }
        if (this->m_result)
            drawer->drawString(this->m_result, false, 120, y + 170, 25, 0xffff);
    });
    list->addItem(custom,170);

    list->addItem(new tsl::elm::CategoryHeader("Settings"));
    if(pm::systuneIsRunning()) {
        auto exit_msg = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *drawer, u16 x, u16 y, u16 w, u16 h) {
             auto [width, height] = drawer->drawString("Cannot change settings while \n"
                                                       "sys-tune-enhanced player is running. \n"
                                                       "Please exit sys-tune first!"
                                                       , false, x + (w / 2) - (this->msgW / 2), y + 30 , 25, 0xffff);
            if (msgW == 0) {
                msgW = width;
                msgH = height;
            }
        });
        list->addItem(exit_msg,90);
    }

    // Whitelist options are not available for blacklisted games
    if (!config::get_title_blacklist(tid)) {
        /* Whitelist mode toggle */
        if(!pm::systuneIsRunning()) {
            auto whitelist_mode = new tsl::elm::ToggleListItem("Whitelist Mode", config::get_whitelist_mode());
            // take action when the state is changed
            whitelist_mode->setStateChangedListener([](bool state) {
            if(!pm::systuneIsRunning()) {
                config::set_whitelist_mode(state);
            }
            });

            // also take action when it is clicked
            whitelist_mode->setClickListener([](u64 keys) {
                if (keys & HidNpadButton_A) {
                    if (!config::get_whitelist_mode()) {
                        tsl::goBack();
                    }
                    return true;
                }
                return false;
            });
            list->addItem(whitelist_mode);
        } else {
            auto whitelist_mode = new tsl::elm::ListItem("Whitelist Mode", config::get_whitelist_mode() ? "On" : "Off");
            list->addItem(whitelist_mode);
        }

        /* Whitelist current game toggle */
        if (tid != 0) {
            if(!pm::systuneIsRunning()) {
                auto whitelist_game = new tsl::elm::ToggleListItem("Whitelist Current Game", config::get_title_whitelist(tid));
                // take action when the state is changed
                whitelist_game->setStateChangedListener([tid](bool state) {
                    if(!pm::systuneIsRunning()) {
                        config::set_title_whitelist(tid, state);
                    }
                });

                // also take action when it is clicked
                whitelist_game->setClickListener([tid](u64 keys) {
                    if (keys & HidNpadButton_A) {
                        if (config::get_title_whitelist(tid) && config::get_whitelist_mode()) {
                            tsl::goBack();
                        }
                        return true;
                    }
                    return false;
                });
                list->addItem(whitelist_game);
            }
            else {
                auto whitelist_game = new tsl::elm::ListItem("Whitelist Current Game", config::get_title_whitelist(tid) ? "On" : "Off");
                list->addItem(whitelist_game);
            }
        }
    }

    /* Title ID Playlist mode toggle */
    if(!pm::systuneIsRunning()) {
        auto title_playlist_mode = new tsl::elm::ToggleListItem("Title ID Playlist Mode", config::get_title_playlist_mode());
        title_playlist_mode->setStateChangedListener([](bool state) {
            if(!pm::systuneIsRunning()) {
                config::set_title_playlist_mode(state);
            }
        });
        list->addItem(title_playlist_mode);
    } else {
        auto title_playlist_mode = new tsl::elm::ListItem("Title ID Playlist Mode", config::get_title_playlist_mode() ? "On" : "Off");
        list->addItem(title_playlist_mode);
    }

    /* Autoplay on boot toggle */
    if(!pm::systuneIsRunning()) {
        auto tune_autoplay = new tsl::elm::ToggleListItem("Autoplay on boot", config::get_autoplay_enabled(), "On", "Off");
        tune_autoplay->setStateChangedListener([](bool new_value) {
            if(!pm::systuneIsRunning()) {
                config::set_autoplay_enabled(new_value);
            }
        });
        list->addItem(tune_autoplay);
    } else {
        auto tune_autoplay = new tsl::elm::ListItem("Autoplay on boot", config::get_autoplay_enabled() ? "On" : "Off");
        list->addItem(tune_autoplay);
    }

    rootFrame->setContent(list);

    return rootFrame;
}
