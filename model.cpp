#include "model.h"

Model::Model(QOpenGLShaderProgram *program,
    const VertexBuffersData &vertexBuffers,
    const QString &texturePath,
    const QVector3D &lightPosition)
    : m_pProgram(program)
    , m_texture(QOpenGLTexture::Target::Target2D)
{
    m_vertPosBuffer = vertexBuffers.vertPosBuffer;
    m_normalBuffer = vertexBuffers.normalBuffer;
    m_texCoordBuffer = vertexBuffers.texCoordBuffer;
    m_amountOfVertices = vertexBuffers.amountOfVertices;

    m_pProgram->bind();

    m_aPositionLocation = m_pProgram->attributeLocation("aPosition");
    m_aNormalLocation = m_pProgram->attributeLocation("aNormal");
    m_aTexCoordLocation = m_pProgram->attributeLocation("aTexCoord");
    m_mvpMatrixLocation = m_pProgram->uniformLocation("uMvpMatrix");
    m_modelMatrixLocation = m_pProgram->uniformLocation("uModelMatrix");
    m_normalMatrixLocation = m_pProgram->uniformLocation("uNormalMatrix");
    const int lightPositionLocation = m_pProgram->uniformLocation("uLightPosition");
    m_pProgram->setUniformValue(lightPositionLocation, lightPosition);

    m_texture.create();
    m_texture.setData(QImage(texturePath).mirrored());
    m_texture.setMinMagFilters(QOpenGLTexture::Filter::Linear,
        QOpenGLTexture::Filter::Linear);
    m_texture.setWrapMode(QOpenGLTexture::WrapMode::ClampToEdge);
}

void Model::draw(const QMatrix4x4 &projViewMatrix)
{
    m_pProgram->bind();

    m_vertPosBuffer.bind();
    m_pProgram->setAttributeBuffer(m_aPositionLocation, GL_FLOAT, 0, 3);
    m_pProgram->enableAttributeArray(m_aPositionLocation);

    m_normalBuffer.bind();
    m_pProgram->setAttributeBuffer(m_aNormalLocation, GL_FLOAT, 0, 3);
    m_pProgram->enableAttributeArray(m_aNormalLocation);

    m_texCoordBuffer.bind();
    m_pProgram->setAttributeBuffer(m_aTexCoordLocation, GL_FLOAT, 0, 2);
    m_pProgram->enableAttributeArray(m_aTexCoordLocation);

    m_modelMatrix.setToIdentity();
    m_modelMatrix.translate(position);
    m_modelMatrix.rotate(rotation);
    m_modelMatrix.scale(scale);
    m_mvpMatrix = projViewMatrix * m_modelMatrix;

    m_normalMatrix = m_modelMatrix.inverted();
    m_normalMatrix = m_normalMatrix.transposed();

    m_pProgram->bind();
    m_pProgram->setUniformValue(m_mvpMatrixLocation, m_mvpMatrix);
    m_pProgram->setUniformValue(m_modelMatrixLocation, m_modelMatrix);
    m_pProgram->setUniformValue(m_normalMatrixLocation, m_normalMatrix);

    m_texture.bind();
    glDrawArrays(GL_TRIANGLES, 0, m_amountOfVertices);
}

void Model::cleanUp()
{
    m_texture.destroy();
}
