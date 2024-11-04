#include "collider.h"

Collider::Collider(btDynamicsWorld *pWorld, btCollisionShape *pShape,
    float mass, const btVector3 &initialPosition,
    const btQuaternion &initialRotation)
    : m_pWorld(pWorld)
    , m_pShape(pShape)
{
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(initialPosition);
    transform.setRotation(initialRotation);

    m_pMotionState = new btDefaultMotionState(transform);
    btVector3 localInertia(0, 0, 0);

    if (mass > 0.001f)
    {
        m_pShape->calculateLocalInertia(mass, localInertia);
    }

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, m_pMotionState, pShape, localInertia);
    pBody = new btRigidBody(rbInfo);
    pBody->setActivationState(DISABLE_DEACTIVATION);
    pBody->setFriction(1);
    pBody->setRollingFriction(0.1);
    m_pWorld->addRigidBody(pBody);
}

Collider::~Collider()
{
    delete pBody;
    delete m_pMotionState;
    delete m_pShape;
}

QVector3D Collider::getPosition()
{
    btMotionState *motionState = pBody->getMotionState();
    motionState->getWorldTransform(m_tempTransform);
    btVector3 btPosition = m_tempTransform.getOrigin();
    QVector3D position(btPosition.x(), btPosition.y(), btPosition.z());
    return position;
}

QQuaternion Collider::getRotation()
{
    btMotionState *motionState = pBody->getMotionState();
    motionState->getWorldTransform(m_tempTransform);
    btQuaternion btQuat = m_tempTransform.getRotation();
    QQuaternion rotation(btQuat.w(), btQuat.x(), btQuat.y(), btQuat.z());
    return rotation;
}
