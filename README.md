# P5InH
**An experimental and fun to use asset which mimics a VEX environment similar to the creative coding framework [Processing](https://processing.org/). Using the object "p5" and setup/draw fields, one can call Processing's familiar functions to create geometry in Houdini.**

The asset supports familiar fill() and stroke() functions plus all the primitive geometry drawing functions of Processing. It can add thickness (using wireframe/polywire SOPs) to the strokes and supports strokeJoin() function. It has the ability to attach Houdini Material (there's a version for RedShift on github) in alpha and additive modes. The asset also supports "PixelCoords" where you can write coordinates like Processing's native system based on a camera's view rectangle.
Also since it's all VEX you can import attributes from other inputs and use them in your P5InH context (sample included)

Behind the scenes, the asset uses the geometry's "detail" level to store drawing "states" for the main context. Each drawing call adds a point with all the necessary attributes to interpret it correctly (color/shapes/coordination etc.). Afterwards a subnet analyzes the point and makes Houdini geometry based on it's data.

Please check the samples hip file to get to know the asset better and discover some of the ways you can use it. One of the samples uses my [mStructManager](https://github.com/kino-dome/Houdini.mTools/tree/master/mStructManager) tool to draw a movement trail made of two "Ball" type objects. The "Ball" class has basic physics functions like bounding the ball to the camera's view rectangle, it calls its "update" function in the asset's "draw" field much like you would do in Processing (The StructManager.h VEX code needs to be in the project's "vex" folder so the sample can include it)
