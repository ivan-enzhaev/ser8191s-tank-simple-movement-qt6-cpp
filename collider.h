#ifndef COLLIDER_H
#define COLLIDER_H

#include <QtGui/QQuaternion>
#include <QtGui/QVector3D>
#include <btBulletDynamicsCommon.h>

class Collider
{
public:
    Collider(btDynamicsWorld *pWorld, btCollisionShape *pShape, float mass,
        const btVector3 &initialPosition = btVector3(0, 0, 0),
        const btQuaternion &initialRotation = btQuaternion(0, 0, 0, 1));
    ~Collider();

    QVector3D getPosition();
    QQuaternion getRotation();

    btRigidBody *pBody;

private:
    btDynamicsWorld *m_pWorld;
    btCollisionShape *m_pShape;
    btDefaultMotionState *m_pMotionState;
    btTransform m_tempTransform;
};

#endif // COLLIDER_H
