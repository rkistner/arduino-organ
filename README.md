# Arduino Organ

MIDI Organ project built with an Arduino Leonardo. This connects to an
Yamaha Electone FS-70 with 3x MCP23017 I/O expanders.

The code in this project is very specific to the wiring of this project.
However, it serves as a practical example of how input matrix scanning
can be used to generate MIDI messages over USB.

This project uses [arcore][2] for MIDI-USB on an Arduino Leonardo.

For a very good tutorial on different input matrix scanning methods, see
 [this post][1] on Open Music Labs.

For technical details on the Yamaha FS-70 (and other similar models), see
[this site][3] by Tim Singer.

 [1]: http://www.openmusiclabs.com/learning/digital/input-matrix-scanning/
 [2]: https://github.com/rkistner/arcore
 [3]: http://www.tsstech.org/electronics/fxtechinfo.html

