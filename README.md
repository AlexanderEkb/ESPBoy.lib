# ESPboy Library

The ESPBoy library is maintained in a git repository hosted on [GitHub](https://github.com/) at:

https://github.com/AlexanderEkb/ESPBoy.lib

The **ESPBoy** library is a fork of the [Arduboy2 library](https://github.com/MLXXXp/Arduboy2), which provides a standard *application programming interface* (API) to the display, buttons and other hardware of the ESP8266 based miniature game system. Its hardware description may be found in /doc folder.

For notes on the differences between the *ESPBoy* library and all its predecessors, and for information on migrating a sketch currently using the *Arduboy* or *Arduboy2* libraries, see the sections at the end of this document.

## Library documentation

Comments in the library header files are formatted for the [Doxygen](http://www.doxygen.org) document generation system. 

## Start up features

The *begin()* function, used to initialize the library, includes features that are intended to be available to all sketches using the library (unless the sketch developer has chosen to disable one or more of them to free up some code space):

### The boot logo

At the start of the sketch, the **ARDUBOY** logo scrolls down from the top of the screen to the center.
I kept it unchanged, because my game system is a direct successor of the Adruboy.

Once the logo display sequence completes, the sketch continues.

For developers who wish to quickly begin testing, or impatient users who want to go strait to playing their game, the boot logo sequence can be bypassed by holding the *RIGHT* button while powering up, and then releasing it. Alternatively, the *RIGHT* button can be pressed while the logo is scrolling down.

For users who wish to always disable the displaying of the boot logo sequence on boot up, a flag in system EEPROM is available for this. The included *SetSystemEEPROM* example sketch can be used to set this flag.

## Using the library in a sketch

As with most libraries, to use ESPBoy in your sketch you must include its header file at the start:

```cpp
#include <ESPBoy.h>
```

You must then create an ESPBoy class object:

```cpp
ESPBoy arduboy;
```
Keeping the old object name "arduboy" simplifies the porting process, but you can use a different name if you wish.

To initialize the library, you must call its *begin()* function. This is usually done at the start of the sketch's *setup()* function:

```cpp
void setup()
{
  arduboy.begin();
  // more setup code follows, if required
}
```

The rest of the ESPBoy functions will now be available for use.

If you wish to use the Sprites class functions you must create a Sprites object:

```cpp
Sprites sprites;
```

More information on writing sketches for the Arduboy can be found in the [Arduboy Community Forum](http://community.arduboy.com/).

### Using EEPROM in a sketch

The ESPBoy library implements a *Storage* class, which replaces an old *EEPROM* one. It utilizes the SPIFFS to store the game data and allows multiple games on a cartridge saving their data without mutual corruption.

#### Sound effects and music

Sound is still unsupported in the library. I hope that once I'll add the support...

#### Remove the text functions

If your sketch doesn't use any of the functions for displaying text, such as *setCursor()* and *print()*, you can remove them. You could do this if your sketch generates whatever text it requires by some other means. Removing the text functions frees up code by not including the font table and some code that is always pulled in by inheriting the [Arduino *Print* class](http://playground.arduino.cc/Code/Printclass).

To eliminate text capability in your sketch, when creating the library object simply use the *ESPBoyBase* class instead of *ESPBoy*:

For example, if the object will be named *arduboy*:

Replace

```cpp
ESPBoy arduboy;
```

with

```cpp
ESPBoyBase arduboy;
```

### Use the SpritesB class instead of Sprites

The *SpritesB* class has functions identical to the *Sprites* class. The difference is that *SpritesB* is optimized for small code size rather than execution speed. If you want to use the sprites functions, and the slower speed of *SpritesB* doesn't affect your sketch, you may be able to use it to gain some code space.

Even if the speed is acceptable when using *SpritesB*, you should still try using *Sprites*. In some cases *Sprites* will produce less code than *SpritesB*, notably when only one of the functions is used.

You can easily switch between using *Sprites* or *SpritesB* by using one or the other to create an object instance:

```cpp
Sprites sprites;  // Use this to optimize for execution speed
SpritesB sprites; // Use this to (likely) optimize for code size
```

## Migrating a sketch from Arduboy library V1.1 to ESPBoy

TBD.

----------

