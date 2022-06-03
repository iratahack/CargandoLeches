# CargandoLeches

Turbo loader for the ZX Spectrum. This repository was forked from [Antonio Villena](https://github.com/antoniovillena/CargandoLeches.git), all credit goes to him.

The turbo loader is in two parts, native command line tools used to create the turbo tape images and updated ZX Spectrum ROM images containing the turbo loader. The ROM images include the turbo loader in addition to the standard loader allowing both regular and turbo images to be loaded.

## Building

To build the command line tools for converting .tap files to turbo loading .wav or .tzx files and to build the updated ROM images which include the turbo loader type `make`.

Z88DK must be installed and included in the path to build the ROM images. GCC is used to build the native tools and must also be included in the path.

## ROM Images

Pre-build ROM images can be found in the [*binaries*](binaries) folder.

## Usage

When using emulators like [FUSE](http://fuse-emulator.sourceforge.net/) the fastest loading speeds can be used. But, when using real hardware a lower speed may be required. I have found the following command line options work well when loading via iPhone to a Harlequin 128.

```bash
./CgLeches <input.tap> <output.wav> 6
```
