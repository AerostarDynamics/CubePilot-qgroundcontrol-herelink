
<p align="center">
  <img src="https://raw.githubusercontent.com/Dronecode/UX-Design/35d8148a8a0559cd4bcf50bfa2c94614983cce91/QGC/Branding/Deliverables/QGC_RGB_Logo_Horizontal_Positive_PREFERRED/QGC_RGB_Logo_Horizontal_Positive_PREFERRED.svg" alt="QGroundControl Logo" width="500">
</p>

<p align="center">
  <a href="https://github.com/CubePilot/qgroundcontrol-herelink/releases">
    <img src="https://img.shields.io/github/release/CubePilot/qgroundcontrol-herelink.svg" alt="Latest Release">
  </a>
</p>

# QGroundControl - Herelink Fork

This is the [CubePilot](https://www.cubepilot.com/) Herelink fork of [QGroundControl](https://github.com/mavlink/qgroundcontrol).

Also see:

- [General QGroundControl Manual](https://docs.qgroundcontrol.com/en/)
- [Herelink documentation](https://docs.cubepilot.org/user-guides/herelink/herelink-overview)

## Sideloading via ADB

You can install the Herelink QGroundControl-v5 APK manually on your Herelink controller using ADB over USB. Follow the steps below if you're not familiar with sideloading.

### Install ADB

Install `adb` (Android Debug Bridge) if you don't have it already:

- **Linux (Debian/Ubuntu):** `sudo apt install android-tools-adb`
- **macOS:** `brew install android-platform-tools`
- **Windows:** Download [Android SDK Platform-Tools](https://developer.android.com/tools/releases/platform-tools) and extract it. Add the folder to your PATH or run `adb` from there directly.

### Enable USB debugging

1. Enable developer mode on the Herelink controller: `Settings` -> `About phone` -> Tap `Build number` 7 times.
2. Then, enable USB debugging in `Settings` > `Developer options` -> `USB debugging`.
3. Connect the Herelink controller to your computer via USB.

### Install the APK

1. Download the latest APK from the [Releases](https://github.com/CubePilot/qgroundcontrol-herelink/releases) page.
2. Install the APK:
   ```
   adb install QGroundControl-v5.0.8.1-herelink.apk
   ```
   If a previous (sideloaded) version is already installed, use the `-r` flag to replace it:
   ```
   adb install -r QGroundControl-v5.0.8.1-herelink.apk
   ```

