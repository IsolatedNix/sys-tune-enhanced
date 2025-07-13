#pragma once

#include <switch.h>
#include <string>
#include <vector>

namespace config {

// tune autoplay
auto get_autoplay_enabled() -> bool;
void set_autoplay_enabled(bool value);

// tune shuffle
auto get_shuffle() -> bool;
void set_shuffle(bool value);

// tune repeat
auto get_repeat() -> int;
void set_repeat(int value);

// tune volume
auto get_volume() -> float;
void set_volume(float value);

// per title tune enable
auto has_title_enabled(u64 tid) -> bool;
auto get_title_enabled(u64 tid) -> bool;
void set_title_enabled(u64 tid, bool value);

// default for tune for every title
auto get_title_enabled_default() -> bool;
void set_title_enabled_default(bool value);

// per title volume
auto has_title_volume(u64 tid) -> bool;
auto get_title_volume(u64 tid) -> float;
void set_title_volume(u64 tid, float value);

// default volume for every title
auto get_default_title_volume() -> float;
void set_default_title_volume(float value);

// returns true if title causes a fatal on launch
auto get_title_blacklist(u64 tid) -> bool;
void set_title_blacklist(u64 tid, bool value);

// returns true if toggled as enabled
auto get_title_playlist_mode() -> bool;
void set_title_playlist_mode(bool value);

// New playlist functions
auto get_playlist(u64 tid) -> std::vector<std::string>;
void save_playlist(const std::vector<std::string>& playlist, u64 tid);

// returns true if title is toggled as enabled (does not override blacklist) 
auto get_whitelist_mode() -> bool;
void set_whitelist_mode(bool value);

auto get_title_whitelist(u64 tid) -> bool;
void set_title_whitelist(u64 tid, bool value);

} // namespace 'config'

