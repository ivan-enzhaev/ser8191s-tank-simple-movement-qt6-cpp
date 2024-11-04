#ifndef MODEL_H
#define MODEL_H

#include <QtGui/QMatrix4x4>
#include <QtGui/QQuaternion>
#include <QtGui/QVector3D>
#include <QtOpenGL/QOpenGLBuffer>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGL/QOpenGLTexture>

#include "vertex_buffers.h"

class Model
{
public:
    Model(QOpenGLShaderProgram *program,
        const VertexBuffersData &vertexBuffers,
        const QString &texturePath,
        const QVector3D &lightPosiiton);

    void draw(const QMatrix4x4 &projViewMatrix);
    void cleanUp();

    QVector3D position = QVector3D(0.f, 0.f, 0.f);
    QQuaternion rotation = QQuaternion(1.f, 0.f, 0.f, 0.f);
    QVector3D scale = QVector3D(1.f, 1.f, 1.f);

private:
    QOpenGLShaderProgram *m_pProgram;
    QOpenGLBuffer m_vertPosBuffer;
    QOpenGLBuffer m_normalBuffer;
    QOpenGLBuffer m_texCoordBuffer;
    QOpenGLTexture m_texture;
    int m_amountOfVertices;
    int m_aPositionLocation;
    int m_aNormalLocation;
    int m_aTexCoordLocation;
    int m_mvpMatrixLocation;
    int m_modelMatrixLocation;
    int m_normalMatrixLocation;
    QMatrix4x4 m_mvpMatrix;
    QMatrix4x4 m_modelMatrix;
    QMatrix4x4 m_normalMatrix;
};

#endif // MODEL_H
