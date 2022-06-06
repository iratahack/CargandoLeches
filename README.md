# CargandoLeches

Turbo loader for the ZX Spectrum. This repository was forked from [Antonio Villena](https://github.com/antoniovillena/CargandoLeches.git), all credit goes to him.

The turbo loader is in two parts, native command line tools used to create the turbo tape images and updated ZX Spectrum ROM images containing the turbo loader. The ROM images include the turbo loader in addition to the standard loader allowing both regular and turbo images to be loaded.

## Building

To build the command line tools for converting .tap files to turbo loading .wav or .tzx files and to build the updated ROM images which include the turbo loader type `make`.

Z88DK must be installed and included in the path to build the ROM images. GCC is used to build the native tools and must also be included in the path.

## ROM Images

Pre-built ROM images can be found in the [*binaries*](binaries) folder. For 48K systems use the [leches.rom](binaries/leches.rom) image. For 128K systems use either [Spectrum128_ROM0.rom](binaries/Spectrum128_ROM0.rom) or [Spectrum128_ROM0_BugFixed.rom](binaries/Spectrum128_ROM0_BugFixed.rom) for ROM-0 and [leches.rom](binaries/leches.rom) for ROM-1.

## Usage

When using emulators like [FUSE](http://fuse-emulator.sourceforge.net/) the fastest loading speeds can be used. But, when using real hardware a lower speed may be required. I have found the following command line options work well when loading via iPhone to a Harlequin 128. Valid speed values are 0-7, the lower the number the faster the load.

```bash
./CgLeches <input.tap> <output.wav> 6
```

Type `./CgLeches` to see the built-in help.

## pokemon

*pokemon* is a ROM based memory peek/poke utility. This version of *pokemon* has been refactored to remove dependencies on system variables and enabling of interrupts within the *pokemon* code.

*pokemon* is activated by pressing the NMI button. Once activated, *pokemon* will display 5 dashes (-) in the upper-right corner of the screen (screen 0 on 128K systems). To read (peek) memory, enter a decimal address followed by *&lt;enter&gt;*. The contents of the memory location will be displayed. To change (poke) the memory contents overwrite the value shown and press *&lt;enter&gt;*. Another address may now be entered. If *&lt;enter&gt;* is pressed without entering an address, *pokemon* will exit.
