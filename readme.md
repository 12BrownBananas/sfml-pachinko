A simple pachinko-themed physics simulation, written with the help of SFML.

To see the code in action, you can simply run the executable (build/main.exe).

**Known Issues**
(These may get resolved at a later time, but for now, they're documented below for posterity)

- Sometimes, respawning balls will get stuck in the top-left corner of the screen. This has been compensated for with brute force means, but a "real" solution may, in fact, be more desirable.
- Occasionally (and seemingly only at the edges of collision sectors) two balls will stick together and stay "stuck" in midair, despite the constant force of gravity. This may have been resolved by applying an arbitrary pad value to sector width and height.