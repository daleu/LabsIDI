#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();
    
  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ();

    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ();
 
    // resize - Es cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);  

  private:
    void createBuffers ();
    void carregaShaders ();
    void modelTransform();
    void keyPressEvent (QKeyEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    
    // attribute locations
    GLuint vertexLoc,varLoc,transLoc,ColorF;
    
    // Program
    QOpenGLShaderProgram *program;

    GLuint VAO, VBO, VBOC;
    float scl,tx,ty,grades,sx,sy,rx,ry;
    int b;

    glm::mat4 TG;
};
