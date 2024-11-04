#ifndef LIGHTLESS_H
#define LIGHTLESS_H

#include "vertex_buffers.h"
#include <QtGui/QMatrix4x4>
#include <QtGui/QQuaternion>
#include <QtGui/QVector3D>
#include <QtOpenGL/QOpenGLShaderProgram>

class Lightless
{
public:
    Lightless(QOpenGLShaderProgram *program, const VertexBuffersData &vertexBuffers);

    void draw(const QMatrix4x4 &projViewMatrix);

    QVector3D position = QVector3D(0.f, 0.f, 0.f);
    QQuaternion rotation = QQuaternion(1, 0, 0, 0);
    QVector3D scale = QVector3D(1.f, 1.f, 1.f);
    QVector3D color;

private:
    QOpenGLShaderProgram *m_pProgram;
    QOpenGLBuffer m_vertPosBuffer;
    int m_amountOfVertices;
    int m_aPositionLocation;
    int m_uColorLocation;
    int m_uMvpMatrixLocation;
    QMatrix4x4 m_mvpMatrix;
    QMatrix4x4 m_modelMatrix;
};

#endif // LIGHTLESS_H
