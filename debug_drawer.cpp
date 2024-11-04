#include "debug_drawer.h"
#include <QtGlobal>
#include <QtMath>

DebugDrawer::DebugDrawer(btDynamicsWorld *pWorld, Lightless *pLightless)
    : m_pWorld(pWorld)
    , m_pLightless(pLightless)
{
}

void DebugDrawer::updateProjViewMatrix(const QMatrix4x4 &projViewMatrix)
{
    m_projViewMatrix = projViewMatrix;
}

void DebugDrawer::setDebugMode(int debugMode)
{
    m_debugMode = debugMode;
}

int DebugDrawer::getDebugMode() const
{
    return m_debugMode;
}

void DebugDrawer::drawContactPoint(const btVector3 &pointOnB, const btVector3 &normalOnB,
    btScalar distance, int lifeTime, const btVector3 &color)
{
    Q_UNUSED(pointOnB);
    Q_UNUSED(normalOnB);
    Q_UNUSED(distance);
    Q_UNUSED(lifeTime);
    Q_UNUSED(color);
}

void DebugDrawer::drawLine(const btVector3 &from, const btVector3 &to,
    const btVector3 &color)
{
    m_pLightless->color.setX(color.x());
    m_pLightless->color.setY(color.y());
    m_pLightless->color.setZ(color.z());

    float centerX, centerY, centerZ;

    if (from.x() > to.x())
    {
        centerX = to.x() + qFabs(from.x() - to.x()) / 2.f;
    }
    else
    {
        centerX = from.x() + qFabs(to.x() - from.x()) / 2.f;
    }

    if (from.y() > to.y())
    {
        centerY = to.y() + qFabs(from.y() - to.y()) / 2.f;
    }
    else
    {
        centerY = from.y() + qFabs(to.y() - from.y()) / 2.f;
    }

    if (from.z() > to.z())
    {
        centerZ = to.z() + qFabs(from.z() - to.z()) / 2.f;
    }
    else
    {
        centerZ = from.z() + qFabs(to.z() - from.z()) / 2.f;
    }

    m_tempVec.setX(to.x() - from.x());
    m_tempVec.setY(to.y() - from.y());
    m_tempVec.setZ(to.z() - from.z());
    float length = m_tempVec.length();
    bool isAxis = false;
    if (length < 0.2f)
    {
        length = 2.f;
        isAxis = true;
    }
    m_tempVec.normalize();

    m_pLightless->rotation = QQuaternion::rotationTo(QVector3D(1.f, 0.f, 0.f), m_tempVec);
    m_pLightless->scale.setX(length);
    m_pLightless->scale.setY(0.03f);
    m_pLightless->scale.setZ(0.03f);
    if (!isAxis)
    {
        m_pLightless->position.setX(centerX);
        m_pLightless->position.setY(centerY);
        m_pLightless->position.setZ(centerZ);
    }
    else
    {
        m_pLightless->position = QVector3D(centerX, centerY, centerZ) + m_tempVec;
    }

    m_pLightless->draw(m_projViewMatrix);
}

void DebugDrawer::draw3dText(const btVector3 &location, const char *textString)
{
    Q_UNUSED(location);
    Q_UNUSED(textString);
}

void DebugDrawer::reportErrorWarning(const char *warningString)
{
    Q_UNUSED(warningString);
}
