#ifndef DLASim_H__
#define DLASim_H__

#include <ngl/Camera.h>
#include <QOpenGLWindow>
#include <memory>

class DLASim// : public QOpenGLWindow
{
public:
  DLASim();
  ~DLASim();
  void drawDLA();
  // Data to plot each frame
  static std::vector <ngl::Vec3> m_data;
  static std::vector <std::vector <ngl::Vec3>> m_final;
  static float m_radius;
  ngl::Vec3 m_seed;
  int m_frameCount;
  int m_totalNumber;
};

#endif // DLASIM

