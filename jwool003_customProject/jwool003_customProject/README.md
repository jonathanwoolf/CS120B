CubeRunner

Project source can be downloaded at https://github.com/jonathanwoolf/CS120B/tree/master/jwool003_customProject/jwool003_customProject

Author and contribution list: Jonathan Woolf 

Any bugs may be reported to jwool003@ucr.edu

How to run the program on a properly wired breadboard:

* Press the two centered buttons at the same time to start and reset the game
* These buttons can also be used to move your character left and right
* The button on the right activates overDrive and speeds up the rate in which you encounter obstacles

Summary:

* The goal of CubeRunner is to avoid incoming obstacles
* You earn 1 point per second or 3 points per second depending on whether you choose to enable overdrive
* Points will be displayed on the LCD and a high score will be saved to the device

Complexity:

* One LED matrix that displays your character and obstacles
* Two shift registers enabling additional colors and freeing up space on the microprocessor 
* EEPROM stores the high score to the microprocessor even when powered off
* Special characters are output to the LCD and are created by writing to individual pixels

Bugs:

* All known bugs have been removed

Demo Video:

https://www.youtube.com/watch?v=tt2nkv3_csM
