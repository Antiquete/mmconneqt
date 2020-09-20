<!-- @format -->

# MMConneqt

## Description

A qt based wrapper gui for Modem Manager.

![alt text](docs/screenshots/mmconneqt_scr_1.png)
![alt text](docs/screenshots/mmconneqt_scr_2.png)

### Features

- Live Notifications for incoming SMS
- Live Notifications for modem plugged in status
- Modem Viewer
  - View Modem information (Device, Name, Manufacturer, IMEI, IMSI, etc..)
- Message Viewer
  - Browse SMSs
  - Save, Send or Resend SMSs
  - Delete SMSs in bulk or singly
  - Block certain numbers by setting AutoDeletes
- Integration with DE's(KDE, Gnome, etc.) system tray

## Dependencies

- Qt
- qmake (For compiling)

## Build

Compile

---

```sh
git clone https://gitlab.com/Antiquete/mmconneqt
cd mmconneqt
mkdir build && cd build
qmake ../MMConneqt.pro
make
```

Run

---

```sh
./MMConneqt
```
