#ifndef WINDOW_H
#define WINDOW_H

#include <QKeyEvent>

#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include "transform3d.h"
#include "camera3d.h"

class QOpenGLShaderProgram;

class Window : public QOpenGLWindow,
               protected QOpenGLFunctions
{
    Q_OBJECT
public:
    Window();
    ~Window();

    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void teardownGL();

protected slots:
    void update();

protected:
  void keyPressEvent(QKeyEvent *event);
  void keyReleaseEvent(QKeyEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);

private:
    // OpenGL缓冲区的辅助类
    // 如果调用了destroy()，缓冲区就变为了无效
    QOpenGLBuffer m_vertex;

    //    顶点数组对象QOpenGLVertexArrayObject
    //    典型用法是：对于每个可视化对象
    //    绑定顶点数组
    //    设置顶点状态、属性等
    //    解绑定顶点数组
    QOpenGLVertexArrayObject m_object;

    // 着色器QOpenGLShaderProgram
    QOpenGLShaderProgram *m_program;

    // Shader Information
    int u_modelToWorld;
    //int u_worldToView;
    int u_worldToCamera;
    int u_cameraToView;

    QMatrix4x4 m_projection;
    Camera3D m_camera;
    Transform3D m_transform;

private:
  // Private Helpers
  void printContextInformation();
};

#endif // WINDOW_H
