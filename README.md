# TTGO-BitmapFontServer
## A font bitmap server for IOT MCUs
This is an implementation of BitmapFontServer for TTGO T-display: https://github.com/ghostyguo/BitmapFontServer
Test command by Serial:
- size=NN (font size, default 16)
- color=XXXX (cont color, 4 bytes HEX)

The server is implemented in Python. It used the opencv to draw text, then return the result bitmap. The data protocol:
- Client Side: 
size=NN\n
text=XXXXXXXX\n
- Server Side: 
width\n
height\n
line_1_bits\n
line_2_bits\n
line_height_bits\n
...

The debug version use an global vairiable img to show the bitmap. Only one client can be serverd each time. The multi-client services is possible to create img for each connected client.
See video: https://www.youtube.com/watch?v=oIsvLyBjq_4