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
    return ini_getbool("tune", "autoplay_enabled", false, CONFIG_PATH);
}

void set_autoplay_enabled(bool value) {
    create_config_dir();
    ini_putl("tune", "autoplay_enabled", value, CONFIG_PATH);
}


auto get_shuffle() -> bool {
    return ini_getbool("config", "shuffle", false, CONFIG_PATH);
}

void set_shuffle(bool value) {
    create_config_dir();
    ini_putl("config", "shuffle", value, CONFIG_PATH);
}

auto get_repeat() -> int {
    return ini_getl("config", "repeat", 1, CONFIG_PATH);
}

void set_repeat(int value) {
    create_config_dir();
    ini_putl("config", "repeat", value, CONFIG_PATH);
}

auto get_volume() -> float {
    return ini_getf("config", "volume", 1.f, CONFIG_PATH);
}

void set_volume(float value) {
    create_config_dir();
    ini_putf("config", "volume", value, CONFIG_PATH);
}

auto has_title_enabled(u64 tid) -> bool {
    return ini_haskey("title", get_tid_str(tid), CONFIG_PATH);
}

auto get_title_enabled(u64 tid) -> bool {
    return ini_getbool("title", get_tid_str(tid), true, CONFIG_PATH);
}

void set_title_enabled(u64 tid, bool value) {
    create_config_dir();
    ini_putl("title", get_tid_str(tid), value, CONFIG_PATH);
}

auto get_title_enabled_default() -> bool {
    return ini_getbool("title", "default", true, CONFIG_PATH);
}

void set_title_enabled_default(bool value) {
    create_config_dir();
    ini_putl("title", "default", value, CONFIG_PATH);
}

auto has_title_volume(u64 tid) -> bool {
    return ini_haskey("volume", get_tid_str(tid), CONFIG_PATH);
}

auto get_title_volume(u64 tid) -> float {
    return ini_getf("volume", get_tid_str(tid), 1.f, CONFIG_PATH);
}

void set_title_volume(u64 tid, float value) {
    create_config_dir();
    ini_putf("volume", get_tid_str(tid), value, CONFIG_PATH);
}

auto get_default_title_volume() -> float {
    return ini_getf("config", "global_volume", 1.f, CONFIG_PATH);
}

void set_default_title_volume(float value) {
    create_config_dir();
    ini_putf("config", "global_volume", value, CONFIG_PATH);
}

auto get_title_blacklist(u64 tid) -> bool {
    return ini_getbool("blacklist", get_tid_str(tid), false, BLACKLIST_PATH);
}

void set_title_blacklist(u64 tid, bool value) {
    create_config_dir();
    ini_putl("blacklist", get_tid_str(tid), value, BLACKLIST_PATH);
}

auto get_playlist() -> std::vector<std::string> {
    std::vector<std::string> playlist;
    long count = ini_getl("playlist", "count", 0, CONFIG_PATH);
    
     for (long i = 0; i < count; i++) {
        char key[32];  // Increased buffer size
        snprintf(key, sizeof(key), "track%d", static_cast<int>(i));  // Using %d and casting to int
        char path[1024] = {0};
        if (ini_gets("playlist", key, "", path, sizeof(path), CONFIG_PATH) > 0) {
            playlist.push_back(path);
        }
    }
    return playlist;
}

void save_playlist(const std::vector<std::string>& playlist) {
    create_config_dir();
    
    ini_putl("playlist", "count", playlist.size(), CONFIG_PATH);
    
    for (size_t i = 0; i < playlist.size(); i++) {
        char key[32];  // Increased buffer size
        snprintf(key, sizeof(key), "track%d", static_cast<int>(i));  // Using %d and casting to int
        ini_puts("playlist", key, playlist[i].c_str(), CONFIG_PATH);
    }
}

auto get_whitelist_mode() -> bool {
    return ini_getbool("custom", "whitelist_mode", false, CONFIG_PATH);
}

void set_whitelist_mode(bool value) {
    create_config_dir();
    ini_putl("custom", "whitelist_mode", value, CONFIG_PATH);
}

auto get_title_whitelist(u64 tid) -> bool {
    if (tid == 0) return false;
    return ini_getbool("whitelist", get_tid_str(tid), false, WHITELIST_PATH);
}

void set_title_whitelist(u64 tid, bool value) {
    if (tid == 0) return;
    create_config_dir();
    ini_putl("whitelist", get_tid_str(tid), value, WHITELIST_PATH);
}

} // namespace 'config'

