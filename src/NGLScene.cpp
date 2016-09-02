#include <QMouseEvent>
#include <QGuiApplication>
#include "NGLScene.h"
#include <ngl/Camera.h>
#include <ngl/Transformation.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Random.h>
#include <ngl/ShaderLib.h>
#include <time.h>
#include <QTimerEvent>
#include <ngl/Text.h>

//The following class follows the fashion of the NGL Demos we have used in the class and more specifically VertexArrayObjects.
//Part of it was wrritten by Jon Macey but major changes were made in order to adapt it accordingly to the project specifics.

/// @brief VertexArrayObject/PerFrameVAO
/// Modified for: -
/// Jon Macey (January 21, 2016). GitHub/NCCA/VertexArrayObject/PerFrameVAO[online]
/// [Accessed January 29, 2016]. Available from https://github.com/NCCA/VertexArrayObject/tree/master/PerFrameVAO


//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for x/y translation with mouse movement
//----------------------------------------------------------------------------------------------------------------------
const static float INCREMENT=0.01;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for the wheel zoom
//----------------------------------------------------------------------------------------------------------------------
const static float ZOOM=0.1;

NGLScene::NGLScene()
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  m_rotate=false;
  // mouse rotation values set to 0
  m_spinXFace=0;
  m_spinYFace=0;
  setTitle("Diffusion Limited Aggregation Fractal Growth");

}

NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::resizeGL(QResizeEvent *_event)
{
  m_width=_event->size().width()*devicePixelRatio();
  m_height=_event->size().height()*devicePixelRatio();
  // now set the camera size values as the screen size has changed
  m_cam.setShape(45.0f,(float)width()/height(),0.05f,350.0f);
}

void NGLScene::resizeGL(int _w , int _h)
{
  m_cam.setShape(45.0f,(float)_w/_h,0.05f,350.0f);
  m_width=_w*devicePixelRatio();
  m_height=_h*devicePixelRatio();
}

void NGLScene::initializeGL()
{
  // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
  // be done once we have a valid GL context but before we call any GL commands. If we dont do
  // this everything will crash
  ngl::NGLInit::instance();
  m_text.reset(new  ngl::Text(QFont("Arial",18)));
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  // Now we will create a basic Camera from the graphics library
  // This is a static camera so it only needs to be set once
  // First create Values for the camera position
  ngl::Vec3 from(0,1,2);
  ngl::Vec3 to(0,0,0);
  ngl::Vec3 up(0,1,0);
  m_cam.set(from,to,up);

  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planes of 0.5 and 10
  m_cam.setShape(45,(float)720.0/576.0,0.001,150);

  glViewport(0,0,width(),height());

  //Start timers
  startTimer(15);
  glPointSize(10);
  srand (time(NULL));
  wireSphere = GL_LINE;

  //Setting the first element to the origin.
  ngl::Vec3 seed = (0,0,0);
  DLASim::m_data.push_back(seed);
}
void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // Rotation based on the mouse position for our global transform
  ngl::Transformation trans;
  // Rotation based on the mouse position for our global
  // transform
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  // create the rotation matrices
  rotX.rotateX(m_spinXFace);
  rotY.rotateY(m_spinYFace);
  // multiply the rotations
  m_mouseGlobalTX=rotY*rotX;
  // add the translations
  m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;

  glPolygonMode(GL_FRONT_AND_BACK,wireSphere);

  ngl::Mat4 MVP;
  MVP=m_mouseGlobalTX*m_cam.getVPMatrix();
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)["nglColourShader"]->use();
  shader->setShaderParamFromMat4("MVP",MVP);

  //Setting the data of tha vao
  ngl::VertexArrayObject *vao =ngl::VertexArrayObject::createVOA(GL_LINE_LOOP);
  vao->bind();
  vao->setData(DLASim::m_data.size()*sizeof(ngl::Vec3),DLASim::m_data[0].m_x);
  vao->setVertexAttributePointer(0,3,GL_FLOAT,0,0);
  shader->setRegisteredUniform4f("Colour",0,0,0,1);
  vao->setNumIndices(DLASim::m_final.size());
  vao->draw();
  vao->unbind();
  vao->removeVOA();

  //Sphere acting as the boundary of the growth
  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  prim->createSphere("sphere",DLASim::m_radius,30);
  shader->setRegisteredUniform4f("Colour",0,0.5,1,1);
  prim->draw("sphere");

  //TExt printing out the growing number of data
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  m_text->setScreenSize(width(),height());
  m_text->setColour(ngl::Colour(1,1,1));
  QString text=QString("Data Size %1 ").arg(DLASim::m_data.size());
  m_text->renderText(10,18,text);

}
void NGLScene::timerEvent(QTimerEvent *_event)
{
  m_dlaSim.drawDLA();
  update();
}

//This following four functions were originally wrriten by Jon Macey
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent (QMouseEvent * _event)
{
  // note the method buttons() is the button state when event was called
  // this is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if(m_rotate && _event->buttons() == Qt::LeftButton)
  {
    int diffx=_event->x()-m_origX;
    int diffy=_event->y()-m_origY;
    m_spinXFace += (float) 0.5f * diffy;
    m_spinYFace += (float) 0.5f * diffx;
    m_origX = _event->x();
    m_origY = _event->y();
    update();

  }
  // right mouse translate code
  else if(m_translate && _event->buttons() == Qt::RightButton)
  {
    int diffX = (int)(_event->x() - m_origXPos);
    int diffY = (int)(_event->y() - m_origYPos);
    m_origXPos=_event->x();
    m_origYPos=_event->y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    update();
   }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent ( QMouseEvent * _event)
{
  // this method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if(_event->button() == Qt::LeftButton)
  {
    m_origX = _event->x();
    m_origY = _event->y();
    m_rotate =true;
  }
  // right mouse translate mode
  else if(_event->button() == Qt::RightButton)
  {
    m_origXPos = _event->x();
    m_origYPos = _event->y();
    m_translate=true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_rotate=false;
  }
        // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_translate=false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{

  // check the diff of the wheel position (0 means no change)
  if(_event->delta() > 0)
  {
    m_modelPos.m_z+=ZOOM;
  }
  else if(_event->delta() <0 )
  {
    m_modelPos.m_z-=ZOOM;
  }
  update();
}
//----------------------------------------------------------------------------------------------------------------------
//end of citation

//The following function was written by Jon Macey but a few modifications were made to serve the purpose of my project
void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window receives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quit
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  // turn on wireframe rendering
  case Qt::Key_W : wireSphere = GL_LINE; break;
  // turn off wireframe
  case Qt::Key_S : wireSphere = GL_FILL; break;
  // show full screen
  case Qt::Key_F : showFullScreen(); break;
  // show windowed
  case Qt::Key_N : showNormal(); break;
  default : break;

  }
  // finally update the GLWindow and re-draw
  //if (isExposed())
    update();
}
//end of citation
