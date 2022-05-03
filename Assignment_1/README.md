Write a program to blend two images together.
The image files should be 24Bit per pixel (standard) BMP files. You will find several ones in the
adjunkt zip to this assigment. You can use your own ones – save them as 24Bit BMP in e.g.
photoshop.

The program should read several parameters from the comand line:
[programname] [imagefile1] [imagefile2] [ratio] [outputfile]
e.g.
blendimages face.bmp butterfly.bmp 0.3 merged.bmp
Catch wrong or missing parameters and print a manual page.
The ratio should determine how much of imagefile1 and imagefile2 will be in the result. A ratio
of 0.5 means the resultimage will be a 50:50 mixture of both. 0.3 means 30% imagefile1 and
70% imagefile2. Got it?
So what gets blended? The pixels.
If both images have the same resolution, then the result pixel on the same x/y coordinate will
be:
red_result = red_image1 * ratio + red_image2 * (1-ratio);
and so for green and blue.
The resolution of both files might be different though. In this case, the resultimage should have
the same resolution as the bigger image (width). In this case, to blend the colors precicely, you
need a function which returns the pixelcolor on a floatingpoint position:
unsigned char get_red(unsigned char *imagedata,float x,float y,int
imagewidth, int imageheight);
And use bilinear interpolation between 4 pixel, depending on the coordinates.
