#pragma once

#include<QOpenGLWidget>
#include<glm/glm/glm.hpp>
#include<glm/glm/gtc/matrix_transform.hpp>
#include<glm/glm/gtc/type_ptr.hpp>
#include<glm/glm/gtx/transform.hpp>
#include<glm/glm/mat4x4.hpp>
#include<glm/glm/gtx/rotate_vector.hpp>

class canvas:public QOpenGLWidget
{
    Q_OBJECT
public:
    canvas(QWidget* parent = nullptr);
    ~canvas();

protected:
    void initializeGL() ;
    void paintGL() ;
    void resizeGL(int width, int height);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent* event);

protected:
    GLfloat m_fRotateX;
    GLfloat m_fRotateY;

    GLfloat m_fScale;

    glm::mat4 m_matModelView;

    float IniX;
    float IniY;
    float dx;
    float dy;

    std::vector<float> vec_pos;
    std::vector<float> vec_norm;
    std::vector<int> vec_vertex_index;
    std::vector<float> vec_texcoord;
};

