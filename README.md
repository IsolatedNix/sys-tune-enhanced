# sys-tune-enhanced
Based on the orginal [sys-tune by HookedBehemoth](https://github.com/HookedBehemoth/sys-tune) with some added enhancements pulled from [TheReconJacob's fork](https://github.com/TheReconJacob/sys-tune) along with some enhancements of my own to improve the experience.

In addition to the original behavior, the following features were added:
- Playlists are now saved when restarting the switch (TheReconJacob)
- A new option appears in the menu, allowing your playlist to automatically start playing when you turn your switch on (TheReconJacob). 
  
  **Note: You need to turn on autostart in [ovl-sysmodules](https://github.com/WerWolv/ovl-sysmodules)**

- Adds a whitelist mode, which does the opposite of blacklist, where a song ONLY plays when at a specific title id (TheReconJacob)
- Added a Title ID Playlist mode which expands on the above playlist saving feature and now allows every title id to load their own unique playlist.
- Fixed the shuffle mode behavior which was broken in TheReconJacob's fork
- Changed the hardcoded blacklist behavior so it no longer needs to be written to file as it was causing an error to be thrown by Tesla overlay when written to on startup. 

  **Note: the blacklist.ini file will still work as it originally did for anything added manually to it, just that it will no longer populate hardcoded entries at startup**

- Updated the Sys-tune Tesla error menu to allow the toggling of some settings to make settings (i.e Whitelist Mode) a bit more user friendly to use. 

  **Note: Unfortunately you can't toggle these when the Sys-tune module is running / suspended as I couldn't get this to work without causing sys-tune to crash so please use [ovl-sysmodules](https://github.com/WerWolv/ovl-sysmodules) to close sys-tune first!**

## Installation
1. To be added...

Leaving the orginal readme notes from HookedBehemoth below...

# sys-tune
## Background audio player for the Nintendo switch + Tesla overlay

## Installation
1. Download the release zip from [here](https://github.com/HookedBehemoth/sys-tune/releases/latest)
2. Extract the zip to the root of your sd card.
3. Put mp3, flac, wav or wave files to your sd card.

You can manage playback via the Tesla overlay in the release.

## Screenshots
![Main](/sample/libtesla_1586882452.jpg)
![Main](/sample/libtesla_1586882672.jpg)
![Main](/sample/libtesla_1586882735.jpg)
(Alpha values are wrong in these screenshots. The overlay will be less transparent.)

## Special thanks to:
- [mackron](http://mackron.github.io/) who made the awesome [audio decoders used here.](https://github.com/mackron/dr_libs/)
- [WerWolv](https://werwolv.net/) for making libtesla, the UI library used for the control overlay.
- [TotalJustice](https://github.com/ITotalJustice) for bug fixes, adding some features and bad code.

## Info for developers
I implemented an IPC interface accessible via service wrappers [here](/ipc/).

My [Tesla overlay](/overlay/source/) uses these bindings.
