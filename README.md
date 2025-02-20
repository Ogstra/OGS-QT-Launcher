# OGS QT Launcher

![Screenshot_20250220_075918](https://github.com/user-attachments/assets/c35b4b3d-96a6-4004-965e-7bb04302c078)

![Screenshot_20250220_075939](https://github.com/user-attachments/assets/d0444a43-51e6-497f-b28e-2bce25f90c5c)

![Screenshot_20250220_075956](https://github.com/user-attachments/assets/dcdb11fa-f334-4b11-8a5f-50ca2ed93aed)


## Dependencies

- [QHotkey](https://github.com/Skycoder42/QHotkey/tree/bb630252684d3556b79ac7a521616692f348fcf7)

### Debian 12

```sudo apt installqt5-qmake qtbase5-dev libqt5x11extras5-dev cmake```

## Build

### Test
```
mkdir -p build
cd build
qmake ../OGS-Launcher.pro CONFIG+=debug
make
```
### Install
```
mkdir -p build
cd build
qmake ../OGS-Launcher.pro
sudo make
```

## Uninstall
```
sudo rm /usr/share/applications/ogs.launcher.desktop
sudo rm /usr/local/bin/OGS-Launcher
```
