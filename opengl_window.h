#ifndef OPENGL_WINDOW_H
#define OPENGL_WINDOW_H

#include <QtCore/QElapsedTimer>
#include <QtGui/QCloseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLFunctions>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGL/QOpenGLWindow>
#include <btBulletDynamicsCommon.h>
#include <map>

#include "collider.h"
#include "debug_drawer.h"
#include "lightless.h"
#include "model.h"
#include "orbit_controls.h"

class OpenGLWindow : public QOpenGLWindow, private QOpenGLFunctions
{
public:
    OpenGLWindow();
    ~OpenGLWindow();

private slots:
    void onCameraUpdate();

private:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void closeEvent(QCloseEvent *event) override;
    void keyboardHandler(float deltaTime);
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QOpenGLShaderProgram *m_pProgram;
    QMatrix4x4 m_projViewMatrix;
    QMatrix4x4 m_projMatrix;
    QMatrix4x4 m_viewMatrix;
    DebugDrawer *m_pDebugDrawer;
    QElapsedTimer m_elapsedTimer;
    std::map<int, bool> m_keys;
    OrbitControls *m_pOrbitControls;
    bool m_showColliders = false;

    Model *m_pPlayer;
    Model *m_pGround;
    Model *m_pWall;

    Collider *m_pGroundCollider;
    Collider *m_pPlayerCollider;
    Collider *m_pWallCollider;
    QOpenGLShaderProgram *m_pLightlessProgram;
    Lightless *m_pLightless;

    float m_linearSpeed = 200.f;
    float m_angularSpeed = 200.f;
    QVector3D m_forward = QVector3D(0.f, 0.f, 1.f);
    QVector3D m_zUnit = QVector3D(0.f, 0.f, 1.f);
    QVector3D m_direction = m_forward;

    btBroadphaseInterface *m_pOverlappingPairCache;
    btCollisionDispatcher *m_pDispatcher;
    btCollisionConfiguration *m_pCollisionConfiguration;
    btConstraintSolver *m_pSolver;
    btDynamicsWorld *m_pWorld;
};

#endif // OPENGL_WINDOW_H
