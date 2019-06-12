Moon In My Room Conversion
==========================

The goal of this project was to convert a Moon In My Room to show the current moon phase.
The solution involved both hardware and software changes.

How To Use
----------

Once you have the Moon converted and plugged in, there are several settings you can adjust.
You are able to adjust the brightness, speed, offset, and direction.

To change a setting, press the button once, then keep pressing to cycle through the different settings.
When you first switch to an option, a short animation will play that represents what type of setting will be effected.
When you've gotten to the setting you want, wait 2 seconds, then you can make any adjustments.
Once you are done adjusting, wait 2 seconds, then the moon will revert back to displaying.
While adjusting, you can either keep pressing to adjust, or hold the button down to have a change trigger every .75 seconds.

The brightness controls the brightness of the display.
The current brightness will be shown while you are adjusting.
It is possible to set the brightness to 0 so it appears off.
The brightness does not affect the brightness while you are changing settings.

The speed controls how long a period is, from full moon to full moon.
There are 6 options: 12 seconds, 12 minutes, 1 hour, 1 day, 12 days, 1 full moon cycle (29 days, 12 hours, 44 minutes, and 3 seconds).
Your current speed is shown by which LED is lit up.
The smallest period is on the left and the largest is on the right.

The offset controls how far off the base state to be from when it was first plugged in.
There are 2 modes for this.
The first is for the full moon period.
An offset of 0 is a full moon.
Each time you trigger an offset, it is like you move forward 1 day.
For all other periods, triggering an offset moves forward one panel.

The direction controls what direction the panels light up.
By default, it lights up right to left, like the northern hemisphere.
With this setting you can choose to have it light up left to right also.

How It's Made
-------------

Below, I've documented the steps I went through in order to convert the Moon In My Room to use an arduino.

#### Materials

 * 1 Moon In My Room
 * 1 Arduino micro
 * 1 micro USB cable
 * 2 feet thin gauge wire
 * 1 protoboard
 * 1 push button
 * 1 10kOhm resistor

#### Tools

 * Phillips screwdriver
 * Soldering iron
 * Dremel with cutting wheel
 * Wire stripper/cutter
