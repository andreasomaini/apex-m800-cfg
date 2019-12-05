# apex-m800-cfg

Configure SteelSeries Apex M800 Keyboard on Linux (Maybe other keyboards too)

## Current Status

As of now, this utility lets you "upload" configuration sniffed from the original [SteelSeries Engine](https://steelseries.com/engine).
This means that you still need a Windows/Mac computer in order to create a configuration. (More below)

However, once you have created the desired configurationsfrom Windows/Mac and cloned them
into some binary format (more below), you can apply the configuration directly from Linux whenever you want.

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

## How to create your configuration

WIP
