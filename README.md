# Media Center Application
Media center application final projected completed for COE 718 (Embedded Systems Design).

## Abstract
The media center that was developed for this project is comprised of several subsystems (written in C) which utilize the MCB1700 boards instruments. The center's features are a photo gallery, an MP3 player that plays audio from the PC, and a game center. Each of these features use the LCD for the user interface and joystick for navigation. This media center was designed and implemented using the concepts that were studied and practiced regarding embedded system design. 

## Design
### Main Menu
The main menu is used to display the main features of the media center: photo gallery, MP3 player, and game center. The user is able to scroll through each option by moving the joystick up and down and using the select button to select an option. 


### Photo Gallery
The photo gallery is designed to display images to the user in a carousel format. To allow this slideshow format to be scrollable, the function reads in the joystick movement and displays the image accordingly.The image is drawn on the LCD using the GLCD_Bitmap() function.


### Games
The first game, called Grid Hunter is a fast paced memory based game played on a 3x3 grid rendered in text characters on the LCD. The player must memorize the positions of briefly flashed “X” targets randomly on the grid and then move the cursor to select one of those positions. The second game, called Highway Dodge is a lane changing game where the player controls a car that must dodge oncoming traffic. The road consists of multiple lanes, and obstacles that the player needs to avoid by switching through the lanes.

### MP3 Player
The MP3 player module connects the USB audio driver to the PC to be an audio output device. It initializes the hardware for the audio system. It sets up the ADC for reading the potentiometer for controlling the volume and DAC for the audio output. There is also a timer configured to generate periodic interrupts for audio processing with the TIMER0_IRQHandler() interrupt service routine used for processing audio data, adjusting volume, and managing audio playback.

### Conclusion
Through the diverse features that were implemented and designed for this media center, the project provided solid experience in embedded system design.