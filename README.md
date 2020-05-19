# apex-m800-cfg

Configure SteelSeries Apex M800 Keyboard on Linux/Windows/Mac without SteelSeries Engine (Maybe other keyboards too)

## Current Status

As of now, this utility lets you "upload" configuration sniffed from the original [SteelSeries Engine](https://steelseries.com/engine).
This means that you still need a Windows/Mac computer or VM in order to create a configuration. (More below)

However, once you have created the desired configurations from Windows/Mac and cloned them
into some binary format (more below), you can apply the configuration directly with this software whenever you want, even on Linux.

## Build

First of all, you need to compile the uploader program. You need to install gcc and the libusb library, both the library and the headers.

On Ubuntu:

```bash
sudo apt install libusb-1.0-0-dev gcc
```

Then, you can compile main.c under `src/uploader`:

```bash
gcc src/uploader/main.c -lusb-1.0 -o uploader
```


## How to apply a configuration

Once builded, you can launch the uploader and pass the path of the configuration file as first argument:

```bash
./uploader configurations/configuration1
```

You can find 2 pre-made configuration on this repo.

## How to create your configuration

### Prerequisites

1. SteelSeries Engine installed on Windows/Mac (You can also install it in a VM)
2. Wireshark
3. Python

### What to do

1. Prepare to upload a configuration from SteelSeries Engine ( you need to select another one first )
2. Start the USB capture from Wireshark - More info [here](https://wiki.wireshark.org/CaptureSetup/USB)
3. Select the desired configuration from SteelSeries Engine
4. End the capture from Wireshark
5. Filter the packets from Wireshark using `usb.setup.wLength == 514`
6. On Wireshark File > Export Packet Dissections > As JSON
7. This file should look like `configurations/configuration1.json`
8. In this repo you will find `src/json_to_binary/json_to_binary.py`
9. Run that script specifying the JSON file as input and the output configuration file, for example:

```
python src/json_to_binary/json_to_binary.py /path/to/json /path/to/configuration
```

10. Compile `src/uploader/main.c`
11. Run the new executable as root specifying the configuration file, for example:

```
sudo ./upload /path/to/configuration
``` 

12. You are done! Repeat 10 every time you want to change configuration
