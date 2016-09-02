#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Random.h>
#include <ngl/ShaderLib.h>
#include <time.h>
#include <NGLScene.h>
#include "DLASim.h"

//This class simulates the algorithm of DLA

float DLASim::m_radius;
std::vector <ngl::Vec3> DLASim::m_data;
std::vector <std::vector <ngl::Vec3>> DLASim::m_final;

DLASim::DLASim()
{
}

DLASim::~DLASim()
{
  std::cout<<"DLASim destructor called\n";
}

void DLASim::drawDLA()
{
  m_frameCount++;
  m_radius = 0.6;
  m_totalNumber = 20000;
  float radius = m_radius*m_radius;
  //std::cout<<"Radius: "<<radius<<"\n";

  if (m_frameCount % 1 == 0)
  {
    int currentSize = m_data.size();
    for(int i=currentSize-1; i<currentSize; i++)
    {
    ngl::Vec3 direction;
    float offset = 0.02; //how far the new particle will be adhere to the previous one
    int x = rand() % 8;   //random number between 0-7 to go throw the switch

    //Set the direction of the initial growth and number of step taken
    for(int step=0; step<10; step++)
    {
      switch(x)
    {
      case 0: { direction = ngl::Vec3(1,1,1)*offset; break; }
      case 1: { direction = ngl::Vec3(-1,-1,-1)*offset; break; }
      case 2: { direction = ngl::Vec3(-1,1,1)*offset; break; }
      case 3: { direction = ngl::Vec3(1,-1,1)*offset; break; }
      case 4: { direction = ngl::Vec3(1,1,-1)*offset; break; }
      case 5: { direction = ngl::Vec3(-1,-1,1)*offset; break; }
      case 6: { direction = ngl::Vec3(-1,1,-1)*offset; break; }
      case 7: { direction = ngl::Vec3(1,-1,-1)*offset; break; }
    }

    //Set the position of the particles to be added to the previous one
    int y = rand() % 3; //random number between 0-2 to go throw the switch
    switch (y)
    {
    case 0: { m_data[i].m_x = m_data[i-1].m_x+direction[0]; m_data[i].m_y = m_data[i-1].m_y; m_data[i].m_z = m_data[i-1].m_z; break; }
    case 1: { m_data[i].m_x = m_data[i-1].m_x; m_data[i].m_y = m_data[i-1].m_y+direction[1]; m_data[i].m_z = m_data[i-1].m_z; break; }
    case 2: { m_data[i].m_x = m_data[i-1].m_x; m_data[i].m_y = m_data[i-1].m_y; m_data[i].m_z = m_data[i-1].m_z+direction[2]; break; }
    }
    //kill the walker if its moving out of the boundary
    if(radius<=m_data[i].m_x*m_data[i].m_x+m_data[i].m_y*m_data[i].m_y+m_data[i].m_z*m_data[i].m_z)
    {
      m_data[i].m_x = m_data[0].m_x;
      m_data[i].m_y = m_data[0].m_y;
      m_data[i].m_z = m_data[0].m_z;

    }
    if(m_data.size()<m_totalNumber)
    {
      //adding rng will an extra random point causing more density between the points
      m_data.push_back(ngl::Vec3(m_data[i].m_x,m_data[i].m_y,m_data[i].m_z));
      m_final.push_back(m_data);

/*    //Testing
      std::cout<<"x is: "<<m_data[i].m_x<<"\n";
      std::cout<<"y is: "<<m_data[i].m_y<<"\n";
      std::cout<<"z is: "<<m_data[i].m_z<<"\n";
*/
    }
    }
    }
    }
}
