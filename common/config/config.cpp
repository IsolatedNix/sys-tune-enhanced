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

auto get_title_blacklist(u64 tid) -> bool {
    return ini_getbool(BLACKLIST_CATEGORY, get_tid_str(tid), false, BLACKLIST_PATH);
}

void set_title_blacklist(u64 tid, bool value) {
    create_config_dir();
    ini_putl(BLACKLIST_CATEGORY, get_tid_str(tid), value, BLACKLIST_PATH);
}

auto get_playlist() -> std::vector<std::string> {
    std::vector<std::string> playlist;
    long count = ini_getl(PLAYLIST_COUNT_CONFIG[0], PLAYLIST_COUNT_CONFIG[1], 0, CONFIG_PATH);
    
     for (long i = 0; i < count; i++) {
        char key[32];  // Increased buffer size
        snprintf(key, sizeof(key), "track%d", static_cast<int>(i));  // Using %d and casting to int
        char path[1024] = {0};
        if (ini_gets(PLAYLIST_COUNT_CONFIG[0], key, "", path, sizeof(path), CONFIG_PATH) > 0) {
            playlist.push_back(path);
        }
    }
    return playlist;
}

void save_playlist(const std::vector<std::string>& playlist) {
    create_config_dir();
    
    ini_putl(PLAYLIST_COUNT_CONFIG[0], PLAYLIST_COUNT_CONFIG[1], playlist.size(), CONFIG_PATH);
    
    for (size_t i = 0; i < playlist.size(); i++) {
        char key[32];  // Increased buffer size
        snprintf(key, sizeof(key), "track%d", static_cast<int>(i));  // Using %d and casting to int
        ini_puts(PLAYLIST_COUNT_CONFIG[0], key, playlist[i].c_str(), CONFIG_PATH);
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

