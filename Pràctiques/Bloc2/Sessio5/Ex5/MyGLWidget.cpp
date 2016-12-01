#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent)
{
  setFocusPolicy(Qt::ClickFocus);  // per rebre events de teclat
  scale = 1.0f;
  x = y = 0.0f;
  grades = float(M_PI/4);
  angle_prespectiva = float(M_PI/4.0f);
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
  
  m.load("./Model/Patricio.obj");
  esferaContenedoraHomer();
  
  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  
  carregaShaders();
  createBuffers();
  
  projectTransform();
  viewTransform();
  
  setMouseTracking(true);
}

void MyGLWidget::paintGL () 
{
  // Esborrem el frame-buffer
  glClear (GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

  // Activem el VAO per a pintar la caseta 
  //HOMMER
  modelTransform ();
  glBindVertexArray (VAO_Casa);
  glDrawArrays(GL_TRIANGLES, 0, m.faces().size()*3);

  //TERRA
  //modelTransform2();
  //glBindVertexArray (VAO_Terra);  
  //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindVertexArray (0);
}

void MyGLWidget::modelTransform () 
{
  //Matriu de transformació de model
  glm::mat4 transform (1.0f);
  transform = glm::translate(transform, glm::vec3(0,0,0));
  transform = glm::scale(transform, glm::vec3(scale));
  transform = glm::rotate(transform,grades,glm::vec3(0.0,1.0,0.0));
  transform = glm::translate(transform, glm::vec3(-x_mid,-y_mid,-z_mid));
  
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::modelTransform2 () 
{
  // Matriu de transformació de terra
  glm::mat4 transform (1.0f);
  //transform = glm::translate(transform, glm::vec3(0,0,0));
  transform = glm::scale(transform, glm::vec3(scale));
  //transform = glm::rotate(transform,grades,glm::vec3(0.0,1.0,0.0));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::projectTransform () 
{
  // glm::perspective (FOV en radians, ra window, znear, zfar)
  float ra= double(width())/double(height());
  if(ra < 1) angle_prespectiva= atan((y_max-y_min));
  angle_prespectiva= asin(radiCapsa/(2*radiCapsa));
  glm::mat4 Proj = glm::perspective (2*angle_prespectiva, ra, z_min,z_max);
  glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform () 
{
  
  glm::mat4 View(1.0);
  View = glm::translate(View,glm::vec3(0.0f,0.0f,float(-2*radiCapsa))); 
  View = glm::rotate(View, x, glm::vec3(1.0f,0.0f,0.0f)); 
  View = glm::rotate(View, y, glm::vec3(0.0f,1.0f,0.0f)); 
  //View = glm::translate(View, glm::vec3(-x_mid,-y_mid,-z_mid));   
  
  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
  
  
  /*// glm::lookAt (OBS, VRP, UP)
  glm::mat4 VM(1.0f);
  VM = glm::translate(VM,glm::vec3(0,0,-2*radiCapsa));
  //VM=VM*Rotate(-φ,0,0,1)
  VM= glm::rotate(VM,x,glm::vec3(1.0,0.0,0.0));
  VM= glm::rotate(VM,-y,glm::vec3(0.0,1.0,0.0));
  VM= glm::translate(VM,glm::vec3(-x_mid,-y_mid,-z_mid));
  
  std::cout << "Matriu: " << x_mid << std::endl;
  
  
  //glm::mat4 View = glm::lookAt (glm::vec3(0,0,-2*radiCapsa), glm::vec3(0,0,0), glm::vec3(0,1,0));
  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &VM[0][0]);*/
  
  /*glm::mat4 View = glm::lookAt (glm::vec3(0,0,-2*radiCapsa), glm::vec3(0,0,0), glm::vec3(0,1,0));
  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);*/
}

void MyGLWidget::resizeGL (int w, int h) 
{
  glViewport(0, 0, w, h);
  projectTransform();
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  switch (event->key()) {
    makeCurrent();
    case Qt::Key_S: { // escalar a més gran
      scale += 0.05;
      modelTransform();
      break;
    }
    case Qt::Key_D: { // escalar a més petit
      scale -= 0.05;
      modelTransform();
      break;
    }
    case Qt::Key_R:{
      grades += float(M_PI/4);
    }
    default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::createBuffers () 
{

  
  //CARREGA HOMER///////////////////////////////////////////////////////////////////////////////////////
  // Creació del Vertex Array Object per pintar
  glGenVertexArrays(1, &VAO_Casa);
  glBindVertexArray(VAO_Casa);

  glGenBuffers(1, &VBO_CasaPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_CasaPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.faces().size()*3*3, m.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glGenBuffers(1, &VBO_CasaCol);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_CasaCol);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.faces().size()*3*3, m.VBO_matdiff(), GL_STATIC_DRAW);

  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);
  
  //CARREGA TERRA///////////////////////////////////////////////////////////////////////////////////////
  
  glm::vec3 posterra[4] = {
	glm::vec3(-1.0, -1.0, -1.0),
	glm::vec3(-1.0, -1.0, 1.0),
	glm::vec3(1.0, -1.0, -1.0),
	glm::vec3(1.0, -1.0, 1.0)
  }; 
  glm::vec3 colterra[4] = {
	glm::vec3(0,0,1),
	glm::vec3(0,0,1),
	glm::vec3(0,0,1),
	glm::vec3(0,0,1)
  };
  
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  glGenBuffers(1, &VBO_TerraPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posterra), posterra, GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glGenBuffers(1, &VBO_TerraCol);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraCol);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colterra), colterra, GL_STATIC_DRAW);

  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);
  
  glBindVertexArray (0);
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

  // Obtenim identificador per a l'atribut “vertex” del vertex shader
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  // Obtenim identificador per a l'atribut “color” del vertex shader
  colorLoc = glGetAttribLocation (program->programId(), "color");
  // Uniform locations
  transLoc = glGetUniformLocation(program->programId(), "TG");
  projLoc = glGetUniformLocation (program->programId(), "proj");
  viewLoc = glGetUniformLocation (program->programId(), "view");

  glEnable(GL_DEPTH_TEST);
}

void MyGLWidget::esferaContenedoraHomer()
{
    //Agafar x_max, x_min, y_max, y_min, z_max, z_min
    for(double i = 0; i < m.vertices().size(); i = i+3){
	if(i==0){
	  x_max = x_min = m.vertices()[i];
	  y_max = y_min = m.vertices()[i+1];
	  z_max = z_min = m.vertices()[i+2];
	}
        else{
	  if(m.vertices()[i] > x_max) x_max = m.vertices()[i];
	  if(m.vertices()[i] < x_min) x_min = m.vertices()[i];

	  if(m.vertices()[i+1] > y_max) y_max = m.vertices()[i+1];
	  if(m.vertices()[i+1] < y_min) y_min = m.vertices()[i+1];

	  if(m.vertices()[i+2] > z_max) z_max = m.vertices()[i+2];
	  if(m.vertices()[i+2] < z_min) z_min = m.vertices()[i+2];
	}
    }

    //Calcular centre base capsa contenidora
    x_mid = (x_max+x_min)/2;
    y_mid = (y_max+y_min)/2;
    z_mid = (z_max+z_min)/2;
    
    std::cout << "Aixó es la x del centre la capsa: " << x_mid << std::endl;
    std::cout << "Aixó es la y del centre la capsa: " << y_mid << std::endl;
    std::cout << "Aixó es la z del centre la capsa: " << z_mid << std::endl;

    radiCapsa = sqrt((pow(x_max-x_min,2))+(pow(y_max-y_min,2))+(pow(z_max-z_min,2)))/2;
    
    std::cout << "Aixó es el radi la capsa: " << radiCapsa << std::endl;
}


void MyGLWidget::mouseMoveEvent(QMouseEvent *e)		// AQUESTA ES LA FUNCIO QUE FA QUE S'ESCALI DIFERENT SEGONS MOGUIS EL MOUSE
{
  makeCurrent ();
    int xnew=e->x();
    int ynew=e->y();

    if(rx<xnew){
        x+=0.00000001;
    }
    else if(rx>xnew){
        x-=0.00000001;
    }

    if(ry<ynew){
        y+=0.00000001;
    }
    else if(ry>ynew){
        y-=0.00000001;
    }
    viewTransform();
    update();
    x=xnew;
    y=ynew;
}
