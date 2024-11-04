#ifndef VERTEX_BUFFERS_H
#define VERTEX_BUFFERS_H

#include <QtCore/QString>
#include <QtOpenGL/QOpenGLBuffer>

struct VertexBuffersData
{
    int amountOfVertices;
    QOpenGLBuffer vertPosBuffer;
    QOpenGLBuffer normalBuffer;
    QOpenGLBuffer texCoordBuffer;
};

class VertexBuffers
{
public:
    VertexBuffers();
    static VertexBuffersData init(const QString &path);
};

#endif // VERTEX_BUFFERS_H
