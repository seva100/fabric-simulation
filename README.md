# fabric-simulation

This project is a realistic simulation of fabric with texture completed as a part of MSU Programming course. Language: C++, compiler: Microsoft Visual C++.

<img src="example pics/Clipboard15.jpg" style="width: 50px"/>
<img src="example pics/Clipboard16.jpg" style="width: 50px"/>
<img src="example pics/Clipboard17.jpg" style="width: 50px"/>

## How to compile and execute

To compile the program, open it from Microsoft Visual Studio. (code makes use of OpenGL and can be rewritten to work in Linux environment). **src/** folder contains Microsoft Visual Studio 2015 project file.

To execute, double-click file **bin/viewer.exe**.

## Control

Control camera movement by mouse, press **w / s** to move camera closer/farther, press **p** to add wind. By pressing **1** you obtain vertex mode (you can see what vertices is fabric model based on), **2** - normals mode (coordinates of normal to the corresponding point of fabric surface are translated to RGB color space: [-1, 1]x[-1, 1]x[-1, 1] values cube translates linearly to [0, 255]x[0, 255]x[0, 255] values cube), **3** - classic mode.
