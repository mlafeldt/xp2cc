Xploder2 Code Compiler
Copyright (C) 2004 misfire


Welcome to Xploder2 Code Compiler!

THIS PROGRAM IS NOT LICENSED, ENDORSED, NOR SPONSORED BY FIRE INTERNATIONAL LTD.
ALL TRADEMARKS ARE PROPERTY OF THEIR RESPECTIVE OWNERS.


About XP2CC
-----------

I didn't like the fact, that you cannot use FCD's XLink to upload your own
Xploder PS2 cheat codes to memory card. It really sucks to enter codes by hand,
so I decided to write XP2CC.

Basically, it translates a text file storing your codes into a P2M file, the
proprietary save file format of FCD, which can then be uploaded to MC via XLink.
Of course, you are not bound to P2M, you can use the excellent PS2 Save Builder
by Vector in order to convert into other formats like NPO.

Besides compiling, XP2CC also allows you to extract cheat codes from P2M files.


Support
-------

As of this point in time, XP2CC only supports the "old" Xploder2 V1 code save
files. With XP2 V2+, there's a lot more nasty stuff to deal with (encryption/
compression/CRC), and I will stick to V1 files for simplicity.

To determine whether or not a code save file is supported by XP2CC, open the
save with PS2 Save Builder: Root/ID must be "FCD", and there has to be a binary
file called "cheats".

However, newer XP2 versions are able to import V1 code save files. Therefore, it
is actually possible to use compiled code save files with every XP2 version. ;)

Btw, XP2CC does support Code Breaker PS2 up to version 6!


How it works
------------

Usage:  xp2cc.exe <input file> <output file> [-absx]

Options description:

        -x      Extract cheat codes from P2M/BIN file to text file
                If this option is not chosen, XP2CC will compile a P2M/BIN file,
                cf. "Text file format".

        -b      Use binary file instead of P2M
                Actually, this is the file "cheats" mentioned under "Support",
                where all the cheat codes are stored in a special format.

        -a      Set system-default ANSI code page to use extended ASCII
                character set (affects compiling only)
                E.g., if you want to use German umlauts in a game title.

        -s      Sort by game title (affects compiling only)

Without any options set, XP2CC will compile a P2M file using ASCII.


Examples
--------

Compile P2M file using ASCII:
xp2cc.exe cheats.txt cheats.p2m

Compile BIN file using ASCII:
xp2cc.exe cheats.txt cheats -b

Compile P2M file using ANSI code page, sort by game title:
xp2cc.exe cheats.txt cheats.p2m -as

Extract cheat codes from BIN file:
xp2cc.exe cheats cheats.txt -x -b

Extract cheat codes from P2M file:
xp2cc.exe cheats.p2m cheats.txt -x


Text file format
----------------

Here's how the format goes:

//*** Start of format sample ***

"GTC Africa PAL"             // game title
Mastercode                   // cheat description
F0100098 0000000E            // cheat code
Unlock All Challenges        // cheat description
1027A588 00000001            // cheat code

"Silent Hill 2 PAL"           ; game title
Max Reverse Joker Pad 2       ; cheat description
D103AC02 0000????             ; cheat description
Mastercode                    ; cheat description
F01A2D70 0000000E             ; cheat code
PAL2NTSC                      ; cheat description
001A3218 00000002             ; cheat code
XY-Fix                        ; cheat description
200F0000 3C010001             ; cheat code
200F0004 34216000             ; "
200F0008 0061182F             ; "
200F000C FCA30000             ; "
200F0010 03E00008             ; "
201F7430 0C03C000             ; "

//*** End of format sample ***

Notes:
- The maximum length of a string (game title or cheat description) is 38,
  the rest is cut off!
- You can add single-line comments by putting "//" or ";"

If you follow these rules, you should not get any parse errors.

IMPORTANT:
Before uploading the compiled P2M file, make sure that there are NO other code
save files on your memory card!
Also, too many compiled cheat codes might cause the XP2 to freeze!

Finally, your cheats are displayed by the Xploder2 and ready to be used...
Let's get cheating! :)


Version history
---------------

v1.31 (2004/09/06)
* Initial public release


Warranty
--------

Xploder2 Code Compiler comes with no warranty. Use it at your own risk.


Special thanks
--------------

First, I want to thank those people whose work helped me to develop XP2CC:
Vector for PS2 Save Builder.
loser for various file format specs: xps, PS2 icon, etc.
Sjeep for bmp2icon.
SlicStik for ExecFTPs.

Also, thanks to Nachbrenner for beta-testing and sharing countless hacking
ideas.

Last but not least, greets to Parasyte & CMX. I respect you for your skills.


Contact
-------

misfire@xploderfreax.de
http://www.xploderfreax.de/


HACKING NEEDS TO BE FREE!
