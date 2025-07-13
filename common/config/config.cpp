#include "config.hpp"
#include "sdmc/sdmc.hpp"
#include "minIni/minIni.h"
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>

namespace config {

namespace {

const char CONFIG_PATH[]{"/config/sys-tune/config.ini"};
// blacklist uses it's own config file because eventually a database
// may be setup and users can easily update their blacklist by downloading
// an updated blacklist.ini.
// Also, the blacklist lookup needs to be as fast as possible
// (literally a race until the title opens audren), so a seperate, smaller file is ideal.
const char BLACKLIST_PATH[]{"/config/sys-tune/blacklist.ini"};
// Custom whitelist implementation
const char WHITELIST_PATH[]{"/config/sys-tune/whitelist.ini"};
// config directory path
const char CONFIG_DIRECTORY_PATH[]{"/config/sys-tune/"};
// config directory extension
const char CONFIG_FILE_EXTENSION[]{".ini"};

// .ini catergories
const char DEFAULT_CATEGORY[]   =  {"config"};
const char TUNE_CATEGORY[]      =  {"tune"};
const char TITLE_CATEGORY[]     =  {"title"};
const char VOLUME_CATEGORY[]    =  {"volume"};
const char BLACKLIST_CATEGORY[] =  {"blacklist"};
const char WHITELIST_CATEGORY[] =  {"whitelist"};
const char PLAYLIST_CATEGORY[]  =  {"playlist"};
const char ENHANCED_CATEGORY[]  =  {"enhanced"};

// .ini config names - These must be unique per setting
const char* SHUFFLE_CONFIG[]               = {DEFAULT_CATEGORY  , "shuffle"};
const char* REPEAT_MODE_CONFIG[]           = {DEFAULT_CATEGORY  , "repeat"};
const char* VOLUME_CONFIG[]                = {DEFAULT_CATEGORY  , "volume"};
const char* GLOBAL_VOLUME_CONFIG[]         = {DEFAULT_CATEGORY  , "global_volume"};
const char* AUTOPLAY_CONFIG[]              = {TUNE_CATEGORY     , "autoplay_enabled"};
const char* TITLE_ENABLED_DEFAULT_CONFIG[] = {TITLE_CATEGORY    , "default"};
const char* PLAYLIST_COUNT_CONFIG[]        = {PLAYLIST_CATEGORY , "count"};
const char* TITLE_PLAYLIST_MODE_CONFIG[]   = {ENHANCED_CATEGORY , "title_playlist_mode"};
const char* WHITELIST_MODE_CONFIG[]        = {ENHANCED_CATEGORY , "whitelist_mode"};

void create_config_dir() {
    /* Creating directory on every set call looks sus, but the user may delete the dir */
    /* whilst the sys-mod is running and then any changes made via the overlay */
    /* is lost, which sucks. */
    sdmc::CreateFolder("/config");
    sdmc::CreateFolder("/config/sys-tune");
}

auto get_tid_str(u64 tid) -> const char* {
    static char buf[21]{};
    std::sprintf(buf, "%016lX", tid);
    return buf;
}

} // nested anonymous 'namespace'

auto get_autoplay_enabled() -> bool {
    return ini_getbool(AUTOPLAY_CONFIG[0], AUTOPLAY_CONFIG[1], false, CONFIG_PATH);
}

void set_autoplay_enabled(bool value) {
    create_config_dir();
    ini_putl(AUTOPLAY_CONFIG[0], AUTOPLAY_CONFIG[1], value, CONFIG_PATH);
}


auto get_shuffle() -> bool {
    return ini_getbool(SHUFFLE_CONFIG[0], SHUFFLE_CONFIG[1], false, CONFIG_PATH);
}

void set_shuffle(bool value) {
    create_config_dir();
    ini_putl(SHUFFLE_CONFIG[0], SHUFFLE_CONFIG[1], value, CONFIG_PATH);
}

auto get_repeat() -> int {
    return ini_getl(REPEAT_MODE_CONFIG[0], REPEAT_MODE_CONFIG[1], 1, CONFIG_PATH);
}

void set_repeat(int value) {
    create_config_dir();
    ini_putl(REPEAT_MODE_CONFIG[0], REPEAT_MODE_CONFIG[1], value, CONFIG_PATH);
}

auto get_volume() -> float {
    return ini_getf(VOLUME_CONFIG[0], VOLUME_CONFIG[1], 1.f, CONFIG_PATH);
}

void set_volume(float value) {
    create_config_dir();
    ini_putf(VOLUME_CONFIG[0], VOLUME_CONFIG[1], value, CONFIG_PATH);
}

auto has_title_enabled(u64 tid) -> bool {
    return ini_haskey(TITLE_CATEGORY, get_tid_str(tid), CONFIG_PATH);
}

auto get_title_enabled(u64 tid) -> bool {
    return ini_getbool(TITLE_CATEGORY, get_tid_str(tid), true, CONFIG_PATH);
}

void set_title_enabled(u64 tid, bool value) {
    create_config_dir();
    ini_putl(TITLE_CATEGORY, get_tid_str(tid), value, CONFIG_PATH);
}

auto get_title_enabled_default() -> bool {
    return ini_getbool(TITLE_ENABLED_DEFAULT_CONFIG[0], TITLE_ENABLED_DEFAULT_CONFIG[1], true, CONFIG_PATH);
}

void set_title_enabled_default(bool value) {
    create_config_dir();
    ini_putl(TITLE_ENABLED_DEFAULT_CONFIG[0], TITLE_ENABLED_DEFAULT_CONFIG[1], value, CONFIG_PATH);
}

auto has_title_volume(u64 tid) -> bool {
    return ini_haskey(VOLUME_CATEGORY, get_tid_str(tid), CONFIG_PATH);
}

auto get_title_volume(u64 tid) -> float {
    return ini_getf(VOLUME_CATEGORY, get_tid_str(tid), 1.f, CONFIG_PATH);
}

void set_title_volume(u64 tid, float value) {
    create_config_dir();
    ini_putf(VOLUME_CATEGORY, get_tid_str(tid), value, CONFIG_PATH);
}

auto get_default_title_volume() -> float {
    return ini_getf(GLOBAL_VOLUME_CONFIG[0], GLOBAL_VOLUME_CONFIG[1], 1.f, CONFIG_PATH);
}

void set_default_title_volume(float value) {
    create_config_dir();
    ini_putf(GLOBAL_VOLUME_CONFIG[0], GLOBAL_VOLUME_CONFIG[1], value, CONFIG_PATH);
}

// get_title_blacklist - hardcoded values below...
// tldr, most fancy things made by N will fatal
// https://github.com/HookedBehemoth/sys-tune/issues/10
/*
0x010077B00E046000, // spyro reignited trilogy
0x0100AD9012510000, // pac man 99
0x01006C100EC08000, // minecraft dugeons
0x01000A10041EA000, // skyrim
0x0100F9F00C696000, // crash team racing nitro fueled
0x01001E9003502000, // labo 03
0x0100165003504000, // labo 04
0x0100F2300D4BA000, // darksiders genesis
0x0100E1400BA96000, // darksiders warmastered edition
0x010071800BA98000, // darksiders 2
0x0100F8F014190000, // darksiders 3
0x0100D870045B6000, // NES NSO
0x01008D300C50C000, // SNES NSO
0x0100C62011050000, // GB NSO
0x010012F017576000, // GBA NSO
0x0100C9A00ECE6000, // N64 NSO

// https://github.com/tallbl0nde/TriPlayer/issues/31
0x0100E5600D446000, // Ni No Kuni: Wrath of the White Witch
0x0100A3900C3E2000, // Paper Mario™: The Origami King
0x0100626011656000, // The Outer Worlds
0x010090F012916000, // Ghostrunner
0x0100F15012D36000, // IMMERSE LAND
0x01005950022EC000, // Blade Strangers
0x0100423009358000, // Death Road to Canada
0x010044500C182000, // Sid Meier's Civilization VI

// anything made by PROTOTYPE
0x0100A3A00CC7E000, // CLANNAD
0x01007B501372C000, // CLANNAD Side Stories
0x01003B300E4AA000, // THE GRISAIA TRILOGY
0x0100F06013710000, // ISLAND
0x0100BD100C752000, // planetarian
0x01002330123BC000, // GRISAIA PHANTOM TRIGGER 05
0x0100240013AE8000, // GRISAIA PHANTOM TRIGGER 06
0x01002EF014DA2000, // GRISAIA PHANTOM TRIGGER 07
0x0100398010314000, // Tomoyo After -It's a Wonderful Life- CS Edition
0x01004AB0133E8000, // GRISAIA PHANTOM TRIGGER 01 to 05
0x01005250123B8000, // GRISAIA PHANTOM TRIGGER 03
0x010054101370E000, // FATAL TWELVE
0x010062A0178A8000, // LOOPERS
0x0100806017562000, // OshiRabu: Waifus Over Husbandos + Love･or･die
0x0100943010310000, // Little Busters! Converted Edition
0x010096000CA38000, // TAISHO x ALICE ALL IN ONE
0x0100A1200CA3C000, // Butterfly's Poison; Blood Chains
0x0100C38019CE4000, // GRISAIA PHANTOM TRIGGER 08
0x0100C9C0178A6000, // Harmonia
0x0100CAF013AE6000, // GRISAIA PHANTOM TRIGGER 5.5
0x0100D970123BA000, // GRISAIA PHANTOM TRIGGER 04
*/

auto get_title_blacklist(u64 tid) -> bool {
    // Ideally this hardcoded list will be removed in the future when a proper means 
    // of keeping the blacklist up to date is implemented.
    // Time is of the essense here so using a sorted version in the code and then conducting a binary search on it...
    // See above comments for the original grouped listings
    u64 sorted_hardcoded_blacklist[] = {
        0x01000A10041EA000,  
        0x010012F017576000,  
        0x0100165003504000,  
        0x01001E9003502000,  
        0x01002330123BC000,  
        0x0100240013AE8000,  
        0x01002EF014DA2000,  
        0x0100398010314000,  
        0x01003B300E4AA000,  
        0x0100423009358000,  
        0x010044500C182000,  
        0x01004AB0133E8000,  
        0x01005250123B8000,  
        0x010054101370E000,  
        0x01005950022EC000,  
        0x0100626011656000,  
        0x010062A0178A8000,  
        0x01006C100EC08000,  
        0x010071800BA98000,  
        0x010077B00E046000,  
        0x01007B501372C000,  
        0x0100806017562000,  
        0x01008D300C50C000,  
        0x010090F012916000,  
        0x0100943010310000,  
        0x010096000CA38000,  
        0x0100A1200CA3C000,  
        0x0100A3900C3E2000,  
        0x0100A3A00CC7E000,  
        0x0100AD9012510000,  
        0x0100BD100C752000,  
        0x0100C38019CE4000,  
        0x0100C62011050000,  
        0x0100C9A00ECE6000,  
        0x0100C9C0178A6000,  
        0x0100CAF013AE6000,  
        0x0100D870045B6000,  
        0x0100D970123BA000,  
        0x0100E1400BA96000,  
        0x0100E5600D446000,  
        0x0100F06013710000,  
        0x0100F15012D36000,  
        0x0100F2300D4BA000,  
        0x0100F8F014190000,  
        0x0100F9F00C696000
    };

    if (std::binary_search(sorted_hardcoded_blacklist
                         , sorted_hardcoded_blacklist + (sizeof(sorted_hardcoded_blacklist) / sizeof(sorted_hardcoded_blacklist[0]))
                         , tid)) 
    {
        return true;
    }
    else {
        return ini_getbool(BLACKLIST_CATEGORY, get_tid_str(tid), false, BLACKLIST_PATH);
    }
}

void set_title_blacklist(u64 tid, bool value) {
    create_config_dir();
    ini_putl(BLACKLIST_CATEGORY, get_tid_str(tid), value, BLACKLIST_PATH);
}

auto get_title_playlist_mode() -> bool {
    return ini_getbool(TITLE_PLAYLIST_MODE_CONFIG[0], TITLE_PLAYLIST_MODE_CONFIG[1], false, CONFIG_PATH);
}

void set_title_playlist_mode(bool value) {
    create_config_dir();
    ini_putl(TITLE_PLAYLIST_MODE_CONFIG[0], TITLE_PLAYLIST_MODE_CONFIG[1], value, CONFIG_PATH);
}

auto get_playlist(u64 tid) -> std::vector<std::string> {
    std::vector<std::string> playlist;

    char targetConfigPath[49]{};

    if(get_title_playlist_mode()) {
        std::strcpy(targetConfigPath,CONFIG_DIRECTORY_PATH);
        std::strcat(targetConfigPath,get_tid_str(tid));
        std::strcat(targetConfigPath,CONFIG_FILE_EXTENSION);
    }
    else {
        std::strcpy(targetConfigPath,CONFIG_PATH);
    }

    long count = ini_getl(PLAYLIST_COUNT_CONFIG[0], PLAYLIST_COUNT_CONFIG[1], 0, targetConfigPath);
    
    for (long i = 0; i < count; i++) {
        char key[32];  // Increased buffer size
        snprintf(key, sizeof(key), "track%d", static_cast<int>(i));  // Using %d and casting to int
        char path[1024] = {0};
        if (ini_gets(PLAYLIST_COUNT_CONFIG[0], key, "", path, sizeof(path), targetConfigPath) > 0) {
            playlist.push_back(path);
        }
    }

    return playlist;
}

void save_playlist(const std::vector<std::string>& playlist, u64 tid) {
    create_config_dir();
    
    char targetConfigPath[49]{};

    if(get_title_playlist_mode()) {
        std::strcpy(targetConfigPath,CONFIG_DIRECTORY_PATH);
        std::strcat(targetConfigPath,get_tid_str(tid));
        std::strcat(targetConfigPath,CONFIG_FILE_EXTENSION);
    }
    else {
        std::strcpy(targetConfigPath,CONFIG_PATH);
    }

    ini_putl(PLAYLIST_COUNT_CONFIG[0], PLAYLIST_COUNT_CONFIG[1], playlist.size(), targetConfigPath);
    
    for (size_t i = 0; i < playlist.size(); i++) {
        char key[32];  // Increased buffer size
        snprintf(key, sizeof(key), "track%d", static_cast<int>(i));  // Using %d and casting to int
        ini_puts(PLAYLIST_COUNT_CONFIG[0], key, playlist[i].c_str(), targetConfigPath);
    }
}

auto get_whitelist_mode() -> bool {
    return ini_getbool(WHITELIST_MODE_CONFIG[0], WHITELIST_MODE_CONFIG[1], false, CONFIG_PATH);
}

void set_whitelist_mode(bool value) {
    create_config_dir();
    ini_putl(WHITELIST_MODE_CONFIG[0], WHITELIST_MODE_CONFIG[1], value, CONFIG_PATH);
}

auto get_title_whitelist(u64 tid) -> bool {
    if (tid == 0) return false;
    return ini_getbool(WHITELIST_CATEGORY, get_tid_str(tid), false, WHITELIST_PATH);
}

void set_title_whitelist(u64 tid, bool value) {
    if (tid == 0) return;
    create_config_dir();
    ini_putl(WHITELIST_CATEGORY, get_tid_str(tid), value, WHITELIST_PATH);
}

} // namespace 'config'

