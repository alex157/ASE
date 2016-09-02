#Fractal Growth Using the Diffusion Limited Aggregation Model

The submitted program represents a fractal growth model using the idea of Diffusion Limited Aggregation.

Following is a documentation of the main operations of the program.

#How to use the program

#DLASim.cpp:
This class contains the implementation of the DLA algorithm written in C++ and OpenGL as well as utilising the NCCA Graphics Library(ngl::).

In this class you can change a various things for testing purposes, in order to modify the way the growth is been made:

    1. m_radius: connects to the VAOPrimitive created on NGLScene.cpp. By changing the radius value the sphere becomes larger or smaller accordingly
    and consequently the maximum boundary where the growth is suppose to be happening changes. A smaller radius, gives more chances for the lines of the
    growing model to hit the sphere and create a new line.

    2. m_totalNumber: represents the number of "particles"(or lines) to be created. This is also displayed on the Data Size text on the top left of the
    window. By increasing this number, the lines have a bigger chance to hit the radius boundary while undertaking the random walk and start with a new
    line from the origin (0,0,0).

    3.offset: represents how far the new line will be added from the previous one.

    4. step: this variable inside the for loop represents how many lines are created each time. You can minimize the growth by decreasing this number to 1,
    or make it larger and slower(depending also on the m_totalNumber) by setting it to 1 and having a more square effect(mainly influence from the directions
    specified).


    5. direction: by changing the number of int x into 2 you isolate the switch operator to go through only the first 2 options. This causes for the DLA
    to grow only towards(1,1,1) and (-1,-1,-1) directions. Similarly, the same can be done with int y, were by omitting the change on the z value of the
    vector, one can achieve a 2D effect only on the x and y values.


#NGLScene.cpp
This class creates the window where the growth is to be visualised as well as the VAO, shaders to add colour, the camera and the mouse and key events.

The couple of things you could possible change minimum parameters for testing reasons mainly regarding Colour. For example:

    1.   glClearColor(0.4f, 0.4f, 0.4f, 1.0f); in intializeGL() controls the colour of the window's background
    2.   shader->setRegisteredUniform4f("Colour",0,0,0,1); in paintGL changes the colour of the VAO to be painted(fractal model)
    3.   shader->setRegisteredUniform4f("Colour",0,0.5,1,1); further down in paintGL changes the colour of the sphere surrounding the model


