#include "window.h"

#include <QDebug>
#include <QString>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>

#include "vertex.h"
#include "input.h"

// 创建一个彩色三角形
//static const Vertex sg_vertexes[] = {
//    //Red -> Green -> Blue
//  Vertex( QVector3D( 0.00f,  0.75f, 1.0f), QVector3D(1.0f, 0.0f, 0.0f) ),
//  Vertex( QVector3D( 0.75f, -0.75f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f) ),
//  Vertex( QVector3D(-0.75f, -0.75f, 1.0f), QVector3D(0.0f, 0.0f, 1.0f) )
//};


// 彩色立方体
// Front Verticies
#define VERTEX_FTR Vertex( QVector3D( 0.5f,  0.5f,  0.5f), QVector3D( 1.0f, 0.0f, 0.0f ) )
#define VERTEX_FTL Vertex( QVector3D(-0.5f,  0.5f,  0.5f), QVector3D( 0.0f, 1.0f, 0.0f ) )
#define VERTEX_FBL Vertex( QVector3D(-0.5f, -0.5f,  0.5f), QVector3D( 0.0f, 0.0f, 1.0f ) )
#define VERTEX_FBR Vertex( QVector3D( 0.5f, -0.5f,  0.5f), QVector3D( 1.0f, 0.0f, 0.0f ) )

// Back Verticies
#define VERTEX_BTR Vertex( QVector3D( 0.5f,  0.5f, -0.5f), QVector3D( 1.0f, 1.0f, 0.0f ) )
#define VERTEX_BTL Vertex( QVector3D(-0.5f,  0.5f, -0.5f), QVector3D( 0.0f, 1.0f, 1.0f ) )
#define VERTEX_BBL Vertex( QVector3D(-0.5f, -0.5f, -0.5f), QVector3D( 1.0f, 0.0f, 1.0f ) )
#define VERTEX_BBR Vertex( QVector3D( 0.5f, -0.5f, -0.5f), QVector3D( 1.0f, 1.0f, 1.0f ) )

// Create a colored cube
//十二个三角形
static const Vertex sg_vertexes[] = {
  // Face 1 (Front)
    VERTEX_FTR, VERTEX_FTL, VERTEX_FBL,
    VERTEX_FBL, VERTEX_FBR, VERTEX_FTR,
  // Face 2 (Back)
    VERTEX_BBR, VERTEX_BTL, VERTEX_BTR,
    VERTEX_BTL, VERTEX_BBR, VERTEX_BBL,
  // Face 3 (Top)
    VERTEX_FTR, VERTEX_BTR, VERTEX_BTL,
    VERTEX_BTL, VERTEX_FTL, VERTEX_FTR,
  // Face 4 (Bottom)
    VERTEX_FBR, VERTEX_FBL, VERTEX_BBL,
    VERTEX_BBL, VERTEX_BBR, VERTEX_FBR,
  // Face 5 (Left)
    VERTEX_FBL, VERTEX_FTL, VERTEX_BTL,
    VERTEX_FBL, VERTEX_BTL, VERTEX_BBL,
  // Face 6 (Right)
    VERTEX_FTR, VERTEX_FBR, VERTEX_BBR,
    VERTEX_BBR, VERTEX_BTR, VERTEX_FTR
};

#undef VERTEX_BBR
#undef VERTEX_BBL
#undef VERTEX_BTL
#undef VERTEX_BTR

#undef VERTEX_FBR
#undef VERTEX_FBL
#undef VERTEX_FTL
#undef VERTEX_FTR


Window::Window()
{
    m_transform.translate(0.0f, 0.0f, -5.0f);
}

Window::~Window()
{
    makeCurrent();
    teardownGL();
}

//初始化OpenGL窗口部件
void Window::initializeGL()
{
    // 初始化 OpenGL 后端
    initializeOpenGLFunctions();
    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    printContextInformation();

    glEnable(GL_CULL_FACE);
    //设置清除屏幕时所用的颜色
    //色彩值的范围从0.0到1.0。0.0代表最黑的情况，1.0就是最亮的情况。
    //第一个参数是红色,第二个是绿色，第三个是蓝色。最大值也是1.0，代表特定颜色分量的最亮情况。最后一个参数是Alpha值。
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // 应用程序相关初始化
    {
        // 创建Shader(着色器,VAO创建之前不要释放)
        m_program = new QOpenGLShaderProgram();
        // 顶点
        m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
        // 片段
        m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");
        m_program->link();
        m_program->bind();

        // Cache Uniform Locations
        //u_modelToWorld = m_program->uniformLocation("modelToWorld");
        //u_worldToView = m_program->uniformLocation("worldToView");
        // Cache Uniform Locations
        u_modelToWorld = m_program->uniformLocation("modelToWorld");
        u_worldToCamera = m_program->uniformLocation("worldToCamera");
        u_cameraToView = m_program->uniformLocation("cameraToView");


        // 创建Buffer（VAO创建之前不要释放）
        m_vertex.create();
        m_vertex.bind();
        m_vertex.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_vertex.allocate(sg_vertexes, sizeof(sg_vertexes));

        // 创建VAO（Vertex Array Object）
        m_object.create();
        m_object.bind();
        m_program->enableAttributeArray(0);
        m_program->enableAttributeArray(1);
        m_program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
        m_program->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());

        // 释放（解绑定）所有对象
        m_object.release();
        m_vertex.release();
        m_program->release();
    }


}

//处理窗口大小变化事件
//resizeGL()在处理完后会自动刷新屏幕
void Window::resizeGL(int width, int height)
{
    m_projection.setToIdentity();
    m_projection.perspective(45.0f, width / float(height), 0.0f, 1000.0f);
}


//绘制OpenGL的窗口，只要有更新发生，这个函数就会被调用
//这个函数中包括所有的绘图代码
void Window::paintGL()
{
    // Clear
    glClear(GL_COLOR_BUFFER_BIT);

    // 渲染Shader
    m_program->bind();
    m_program->setUniformValue(u_worldToCamera, m_camera.toMatrix());
    m_program->setUniformValue(u_cameraToView, m_projection);
    //m_program->setUniformValue(u_worldToView, m_projection);
    {
        m_object.bind();
        m_program->setUniformValue(u_modelToWorld, m_transform.toMatrix());
//        qDebug() << sizeof(sg_vertexes) / sizeof(sg_vertexes[0]);
        //按三角形绘制
        glDrawArrays(GL_TRIANGLES, 0, sizeof(sg_vertexes) / sizeof(sg_vertexes[0]));
        //glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(sg_vertexes) / sizeof(sg_vertexes[0]));
        m_object.release();
    }
    m_program->release();
}

// 回收资源
void Window::teardownGL()
{
    // Actually destroy our OpenGL information
    m_object.destroy();
    m_vertex.destroy();
    delete m_program;
}

void Window::update()
{
    // Update input
    Input::update();

    // Camera Transformation
//    if (Input::buttonPressed(Qt::LeftButton))
    {
      static const float transSpeed = 0.5f;
      static const float rotSpeed   = 0.5f;
      if (Input::buttonPressed(Qt::LeftButton))
      {
      // Handle rotations
          m_camera.rotate(-rotSpeed * Input::mouseDelta().x(), Camera3D::LocalUp);
          m_camera.rotate(-rotSpeed * Input::mouseDelta().y(), m_camera.right());
      }

      // Handle translations
      QVector3D translation;
      if (Input::keyPressed(Qt::Key_W))
      {
        translation += m_camera.forward();
      }
      if (Input::keyPressed(Qt::Key_S))
      {
        translation -= m_camera.forward();
      }
      if (Input::keyPressed(Qt::Key_A))
      {
        translation -= m_camera.right();
      }
      if (Input::keyPressed(Qt::Key_D))
      {
        translation += m_camera.right();
      }
      if (Input::keyPressed(Qt::Key_Q))
      {
        translation -= m_camera.up();
      }
      if (Input::keyPressed(Qt::Key_E))
      {
        translation += m_camera.up();
      }
      m_camera.translate(transSpeed * translation);
    }


    // Update instance information
    m_transform.rotate(1.0f, QVector3D(0.4f, 0.3f, 0.3f));

    // Schedule a redraw
    QOpenGLWindow::update();
}

void Window::keyPressEvent(QKeyEvent *event)
{
  if (event->isAutoRepeat())
  {
    event->ignore();
  }
  else
  {
    Input::registerKeyPress(event->key());
  }
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
  if (event->isAutoRepeat())
  {
    event->ignore();
  }
  else
  {
    Input::registerKeyRelease(event->key());
  }
}

void Window::mousePressEvent(QMouseEvent *event)
{
  Input::registerMousePress(event->button());
}

void Window::mouseReleaseEvent(QMouseEvent *event)
{
  Input::registerMouseRelease(event->button());
}

// 打印相关信息，调试用
void Window::printContextInformation()
{
  QString glType;
  QString glVersion;
  QString glProfile;

  // Get Version Information
  glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
  glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

  // Get Profile Information
#define CASE(c) case QSurfaceFormat::c: glProfile = #c; break
  switch (format().profile())
  {
    CASE(NoProfile);
    CASE(CoreProfile);
    CASE(CompatibilityProfile);
  }
#undef CASE

  // qPrintable() will print our QString w/o quotes around it.
  qDebug() << qPrintable(glType) << qPrintable(glVersion) << "(" << qPrintable(glProfile) << ")";
}




