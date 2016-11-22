#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent)
{
  setFocusPolicy(Qt::ClickFocus);  // per rebre events de teclat
}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();
  
  glClearColor (0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  sx=0.5;
  sy=0.5;
  tx = 0;
  ty = 0;
  scl = 0.5;
  grades = float(M_PI/4);
  carregaShaders();
  load();
  createBuffers();
  setMouseTracking(true);
}

void MyGLWidget::paintGL ()
{
  glClear (GL_COLOR_BUFFER_BIT);  // Esborrem el frame-buffer

  // Activem l'Array a pintar 
  glBindVertexArray(VAO);
 
  // Pintem l'escena
  glDrawArrays(GL_TRIANGLES, 0, 3);
  
  // Desactivem el VAO
  glBindVertexArray(0);
}

void MyGLWidget::resizeGL (int w, int h)
{
  glViewport (0, 0, w, h);
}

void MyGLWidget::createBuffers ()
{
  glm::vec3 Vertices[3];  // Tres vèrtexs amb X, Y i Z
  Vertices[0] = glm::vec3(-1.0, -1.0, 0.0);
  Vertices[1] = glm::vec3(1.0, -1.0, 0.0);
  Vertices[2] = glm::vec3(0.0, 1.0, 0.0);
  
  // Creació del Vertex Array Object (VAO) que usarem per pintar
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Creació del buffer amb les dades dels vèrtexs
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
  // Activem l'atribut que farem servir per vèrtex	
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glm::vec3 Colors[3];  // Tres vèrtexs amb X, Y i Z
  Colors[0] = glm::vec3(1.0, 0.0, 0.0);
  Colors[1] = glm::vec3(0.0, 1.0, 0.0);
  Colors[2] = glm::vec3(0.0, 0.0, 1.0);

  glGenBuffers(1, &VBOC);
  glBindBuffer(GL_ARRAY_BUFFER, VBOC);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
  // Activem l'atribut que farem servir per vèrtex
  glVertexAttribPointer(ColorF, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(ColorF);

  // Desactivem el VAO
  glBindVertexArray(0);
}

void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/fragshad.frag");
  vs.compileSourceFile("shaders/vertshad.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  ColorF = glGetAttribLocation (program->programId(), "color");
  transLoc = glGetUniformLocation (program->programId(), "TG");
  varLoc = glGetUniformLocation (program->programId (), "val");
  modelTransform();
  glUniform1f (varLoc, scl);

}

void MyGLWidget::modelTransform () {
    TG = glm::mat4(1.0); // Matriu de transformació, inicialment identitat
    TG = glm::translate (TG, glm::vec3 (tx, ty, 0.0));
    TG = glm::rotate(TG,grades,glm::vec3(0.0,0.0,1.0));
    TG = glm::scale(TG,glm::vec3(sx,sy,1.0));
    glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::keyPressEvent (QKeyEvent *e) {
  makeCurrent ();
  switch ( e->key() ) {
    case Qt::Key_Left :
        tx -= 0.1;
        grades += float(M_PI/4);
        modelTransform();
        break;
    case Qt::Key_Right :
        tx += 0.1;
        grades += float(M_PI/4);
        modelTransform();
        break;
    case Qt::Key_Down :
        ty -= 0.1;
        grades += float(M_PI/4);
        modelTransform();
        break;
    case Qt::Key_Up :
        ty += 0.1;
        grades += float(M_PI/4);
        modelTransform();
        break;
    default: e->ignore (); // propagar al pare
  }
  update ();
}


void MyGLWidget::mouseMoveEvent(QMouseEvent *e)		// AQUESTA ES LA FUNCIO QUE FA QUE S'ESCALI DIFERENT SEGONS MOGUIS EL MOUSE
{
  makeCurrent ();
    int xnew=e->x();
    int ynew=e->y();

    if(rx<xnew){
        sx+=0.01;
    }
    else if(rx>xnew){
        sx-=0.01;
    }

    if(ry<ynew){
        sy+=0.01;
    }
    else if(ry>ynew){
        sy-=0.01;
    }
    modelTransform();
    update();
    rx=xnew;
    ry=ynew;
}
