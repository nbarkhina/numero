# Numero

![screenshot](preview.gif)

Numero is a libretro core for emulating the TI-83 family of graphing calculators. It is based on the awesome Wabbitemu emulator. It allows you to play your TI-83 games like never before in fullscreen and using a gamepad! You can also control it with a mouse or keyboard, or you can just use the virtual mouse for pressing the calculator buttons.

# Using the core

Here is some useful information to help you run the emulator

- You will need to put the BIOS into your RetroArch System Directory
  - ti83se.rom is recommended since you will get the largest storage capacity
- The Emulator will save your progress every 10 seconds
  - this is done in the background since the calculator doesn't really have any "long term storage"
  - everything was always just saved in RAM
  - or if you just select "Close Content" in RetroArch that will also immediately save your progress
- You can run the core without any content by just selecting "Start Core"
- There are two control schemes
  - Joypad which is the default
  - And "Gaming Buttons" which is more suited towards gaming
  - You can move the virtual mouse with the left stick
    - and by Pressing R2 to click
  - You can also use the D-Pad and A Button if using the Joypad scheme
  - Change the virtual mouse speed in the core Options
- You can toggle between the calculator view and "Big Mode" with L2
- Hitting "Restart" in the RetroArch menu will clear the entire memory
  - So be careful when doing this because you will lose all your data
  - This is useful however sometimes when the emulator hangs for one reason or another 
- Installing Apps
  - You will probably need to install some "loaders" for most of the demanding apps
  - Such as "Ion" or "Mirage"
  - You can install "Ion.8xg" by going to Retroarch => Load Content
  - After starting you will see a message in the bottom left corner if it imported successfully
  - Then go to "Close Content"
  - Then go again to "Load Content" and install the game you want to run inside of Ion such as "Ztetris.8xp"
  - Then keep repeating the process to load all the apps you want (careful not to fill up the entire space)
  - Essentially you have to load each piece one on top of another since there is a single common "hard drive" for the calculator
- Every time you load the emulator you will start from the last place you left off
  - For the same reason mentioned above where it saves in the background
  - Also keep this in mind for save states
- Save States
  - You can create as many save states as you want using the different slots
  - However they will be named internally based on how you started the emulator
    - From a rom or just the core itself
  - There will be a common "In Progress" storage mechanism when loading different states
    - Save States will restore whatever the state was of the entire calculator
    - Including all installed apps at the time of saving
  - Take lots of save states since you may run into problems where you will need to wipe the memory and try again
- If you have trouble installing one game or another
  - Try moving things around between Memory and Archive
  - Sometimes different launchers require things not be in Archive
  - You can watch this awesome video by LGR where he talks about the phenomenon
    - https://www.youtube.com/watch?v=nduMTX86Zl0

# References
- wxWabbitEmu - https://github.com/alberthdev/wxwabbitemu
- Wabbitemu - https://github.com/sputt/wabbitemu