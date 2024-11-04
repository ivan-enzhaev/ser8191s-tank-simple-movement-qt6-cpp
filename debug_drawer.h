#ifndef DEBUG_DRAWER_H
#define DEBUG_DRAWER_H

#include "lightless.h"
#include <btBulletDynamicsCommon.h>

class DebugDrawer : public btIDebugDraw
{
public:
    DebugDrawer(btDynamicsWorld *pWorld, Lightless *pLightless);

    void updateProjViewMatrix(const QMatrix4x4 &projViewMatrix);

    // Debug mode methods
    virtual void setDebugMode(int debugMode) override;
    virtual int getDebugMode() const override;

    // Drawing methods
    virtual void drawContactPoint(const btVector3 &pointOnB, const btVector3 &normalOnB,
        btScalar distance, int lifeTime, const btVector3 &color) override;
    virtual void drawLine(const btVector3 &from, const btVector3 &to,
        const btVector3 &color) override;

    // Unused
    virtual void draw3dText(const btVector3 &location, const char *textString) override;
    virtual void reportErrorWarning(const char *warningString) override;

private:
    int m_debugMode = 1;
    btDynamicsWorld *m_pWorld;
    Lightless *m_pLightless;
    QVector3D m_tempVec;
    QMatrix4x4 m_projViewMatrix;
};

#endif // DEBUG_DRAWER_H
