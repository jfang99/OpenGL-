#include "canvas.h"
#include <gl/GLU.h>
#include<QMouseEvent>
#include<cmath>
#include<math.h>
#include <fstream>
#include <iostream>
#include "tinyply.h"




canvas::canvas(QWidget* parent):QOpenGLWidget(parent)
{
    setGeometry(400, 400, 640, 480);
    m_fRotateX = 0.0f;
    m_fRotateY = 0.0f;
    m_fScale = 1000;
    dx = 0.0f;
    dy = 0.0f;

    glm::mat4 m_matModelView = glm::mat4(1.0);

}

canvas::~canvas(){}

void canvas::initializeGL()
{
    glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
    glEnable(GL_DEPTH_TEST);

    float AmbientLight[4]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    float DiffuseLight[4]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    float SpecularLight[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float LightPosition[4] = { 30.0f, 30.0f, 30.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    using namespace tinyply;
    std::shared_ptr<PlyData> vertices, normals, faces, vertex_indice, texcoords;
    std::ifstream ss("1.ply", std::ios::binary);
    PlyFile file;
    file.parse_header(ss);

    std::cout << "........................................................................\n";
    for (auto c : file.get_comments()) std::cout << "Comment: " << c << std::endl;
    for (auto e : file.get_elements())
    {
        std::cout << "element - " << e.name << " (" << e.size << ")" << std::endl;
        for (auto p : e.properties) std::cout << "\tproperty - " << p.name << " (" << tinyply::PropertyTable[p.propertyType].str << ")" << std::endl;
    }

    std::cout << "........................................................................\n";
    vertices = file.request_properties_from_element("vertex", { "x", "y", "z" });
    normals = file.request_properties_from_element("vertex", { "nx", "ny", "nz" });
    vertex_indice = file.request_properties_from_element("face", { "vertex_indices" });
    texcoords = file.request_properties_from_element("face", { "texcoord" });

    file.read(ss);

    vec_pos.resize(vertices->count * 3);
    vec_norm.resize(normals->count * 3);
    //vec_vertex_index.resize(vertex_indice->count * 1);
    //vec_texcoord.resize(texcoords->count * 3);

    memcpy(vec_pos.data(), vertices->buffer.get(), vertices->buffer.size_bytes());
    memcpy(vec_norm.data(), normals->buffer.get(), normals->buffer.size_bytes());
    //memcpy(vec_vertex_index.data(), vertex_indice->buffer.get(), vertex_indice->buffer.size_bytes());
    //memcpy(vec_texcoord.data(), texcoords->buffer.get(), texcoords->buffer.size_bytes());

    float fx = 0;
    float fy = 0;
    float fz = 0;

    for (int i = 0; i < vec_pos.size(); i = i + 3)
    {
        fx += vec_pos[i];
        fy += vec_pos[i + 1];
        fz += vec_pos[i + 2];
    }

    fx /= vec_pos.size() / 3;
    fy /= vec_pos.size() / 3;
    fz /= vec_pos.size() / 3;

    for (int i = 0; i < vec_pos.size(); i = i + 3)
    {
        vec_pos[i] -= fx;
        vec_pos[i + 1] -= fy;
        vec_pos[i + 2] -= fz;
    }
}



void canvas::paintGL()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glm::vec4 vecX = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) * m_matModelView;
    glm::vec4 vecY = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) * m_matModelView;
    m_matModelView = glm::rotate(m_matModelView, m_fRotateX * 0.1f, glm::vec3(vecX.x, vecX.y, vecX.z));
    m_matModelView = glm::rotate(m_matModelView, m_fRotateY * 0.1f, glm::vec3(vecY.x, vecY.y, vecY.z));
    glm::vec4 view(0, 0, 1000, 1.0);
    view = view * m_matModelView;
    glm::vec4 up(0, 1, 0, 1.0);
    up = up * m_matModelView;
    gluLookAt(view.x, view.y, view.z, 0, 0, 0, up.x, up.y, up.z);

    m_fRotateX = 0.0f;
    m_fRotateY = 0.0f;

    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < vec_pos.size(); i = i + 3)
    {
        glNormal3f(vec_norm[i], vec_norm[i + 1], vec_norm[i + 2]);
        glVertex3f(vec_pos[i], vec_pos[i + 1], vec_pos[i + 2]);
    }
    glEnd();
}

void canvas::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(100.0f, (float)width / (float)height, 1, 2000);
}

void canvas::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        IniX = event->globalX();
        IniY = event->globalY();
        dx = 0.0f;
        dy = 0.0f;
    }
}


void canvas::mouseMoveEvent(QMouseEvent *event) {
    dx = event->globalX() - IniX;
    dy = event->globalY() - IniY;
    if (event->buttons() == Qt::LeftButton) {
        m_fRotateX = 0.2 * dy;
        m_fRotateY = 0.2 * dx;
        update();
        IniX = event->globalX();
        IniY = event->globalY();
    }
}
