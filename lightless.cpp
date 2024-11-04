#include "lightless.h"

Lightless::Lightless(QOpenGLShaderProgram *program, const VertexBuffersData &vertexBuffers)
    : m_pProgram(program)
{
    m_vertPosBuffer = vertexBuffers.vertPosBuffer;
    m_amountOfVertices = vertexBuffers.amountOfVertices;

    m_pProgram->bind();
    m_aPositionLocation = m_pProgram->attributeLocation("aPosition");
    m_uColorLocation = m_pProgram->uniformLocation("uColor");
    m_uMvpMatrixLocation = m_pProgram->uniformLocation("uMvpMatrix");
}

void Lightless::draw(const QMatrix4x4 &projViewMatrix)
{
    m_pProgram->bind();

    m_vertPosBuffer.bind();
    m_pProgram->setAttributeBuffer(m_aPositionLocation, GL_FLOAT, 0, 3);
    m_pProgram->enableAttributeArray(m_aPositionLocation);

    m_modelMatrix.setToIdentity();
    m_modelMatrix.translate(position);
    m_modelMatrix.rotate(rotation);
    m_modelMatrix.scale(scale);
    m_mvpMatrix = projViewMatrix * m_modelMatrix;

    m_pProgram->bind();
    m_pProgram->setUniformValue(m_uColorLocation, color);
    m_pProgram->setUniformValue(m_uMvpMatrixLocation, m_mvpMatrix);

    glDrawArrays(GL_TRIANGLES, 0, m_amountOfVertices);
}
