#include "opengl_window.h"
#include "vertex_buffers.h"
#include <QtGui/QSurfaceFormat>

#ifdef _WIN32
#include <windows.h>
#endif

OpenGLWindow::OpenGLWindow()
{
    setTitle("Bullet Physics, OpenGL ES 2.0, Qt6, C++");
    resize(450, 450);

#ifdef _WIN32
    PostMessage(GetForegroundWindow(), WM_INPUTLANGCHANGEREQUEST, 1, 0x04090409);
#endif

    QSurfaceFormat surfaceFormat;
    surfaceFormat.setDepthBufferSize(24);
    surfaceFormat.setSwapInterval(1);
    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    setFormat(surfaceFormat);

    m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
    m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);
    m_pOverlappingPairCache = new btDbvtBroadphase();
    m_pSolver = new btSequentialImpulseConstraintSolver();
    m_pWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pOverlappingPairCache,
        m_pSolver, m_pCollisionConfiguration);
    m_pWorld->setGravity(btVector3(0, -9.8, 0));

    m_pOrbitControls = new OrbitControls(10.f, QVector2D(40.f, 0.f), QVector2D(0.f, 0.f));
    connect(m_pOrbitControls, &OrbitControls::update, this, &OpenGLWindow::onCameraUpdate);
}

OpenGLWindow::~OpenGLWindow()
{
    delete m_pWorld;
    delete m_pSolver;
    delete m_pOverlappingPairCache;
    delete m_pDispatcher;
    delete m_pCollisionConfiguration;

    delete m_pPlayer;
    delete m_pGround;
    delete m_pProgram;

    delete m_pPlayerCollider;
    delete m_pGroundCollider;
    delete m_pWallCollider;

    delete m_pLightlessProgram;
    delete m_pLightless;
    delete m_pDebugDrawer;
    delete m_pOrbitControls;
}

void OpenGLWindow::onCameraUpdate()
{
    m_viewMatrix = m_pOrbitControls->getViewMatrix();
    update();
}

void OpenGLWindow::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // qDebug() << "OpenGL version:" << (const char*) glGetString(GL_VERSION) << "\n";
    // qDebug() << "GLSL version: " << (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
    // qDebug() << "Vendor: " << (const char*) glGetString(GL_VENDOR) << "\n";

    m_pProgram = new QOpenGLShaderProgram();
    m_pProgram->create();
    m_pProgram->addShaderFromSourceFile(QOpenGLShader::ShaderTypeBit::Vertex,
        ":/assets/shaders/texture.vert");
    m_pProgram->addShaderFromSourceFile(QOpenGLShader::ShaderTypeBit::Fragment,
        ":/assets/shaders/texture.frag");
    m_pProgram->link();

    const QVector3D lightPosition(-20.f, 30.f, 35.f);

    const VertexBuffersData groundBuffers = VertexBuffers::init(":/assets/models/ground.dae");
    m_pGround = new Model(m_pProgram, groundBuffers, ":/assets/models/colors.png",
        lightPosition);
    btBoxShape *groundShape = new btBoxShape(btVector3(20.f / 2.f, 0.2f / 2.f, 20.f / 2.f));
    m_pGroundCollider = new Collider(m_pWorld, groundShape, 0);

    const VertexBuffersData playerBuffers = VertexBuffers::init(":/assets/models/tank.dae");
    m_pPlayer = new Model(m_pProgram, playerBuffers, ":/assets/models/colors.png",
        lightPosition);
    btSphereShape *playerSphereShape = new btSphereShape(1);
    btBoxShape *playerBoxShape = new btBoxShape(btVector3(1.47f / 2.f, 1.05f / 2.f, 3.175f / 2.f));

    btCompoundShape *playerShape = new btCompoundShape();
    // btScalar masses[] = { 1.f, 1.f };
    // btTransform principal;
    // btVector3 inertia;
    // playerShape->calculatePrincipalAxisTransform(masses, principal, inertia);

    btTransform transform;
    transform.setIdentity();
    playerShape->addChildShape(transform, playerSphereShape);
    transform.setIdentity();
    playerShape->addChildShape(transform, playerBoxShape);
    m_pPlayerCollider = new Collider(m_pWorld, playerShape, 500.f, btVector3(0.f, 5.f, 0.f));
    m_pPlayerCollider->pBody->setAngularFactor(btVector3(0.f, 1.f, 0.f));
    m_pPlayerCollider->pBody->setDamping(0.9, 0.99);

    const VertexBuffersData wallBuffers = VertexBuffers::init(":/assets/models/wall.dae");
    m_pWall = new Model(m_pProgram, wallBuffers, ":/assets/models/colors.png",
        lightPosition);
    btBoxShape *wallShape = new btBoxShape(btVector3(1.f / 4.f, 4.329f / 4.f, 8.515f / 4.f));
    m_pWallCollider = new Collider(m_pWorld, wallShape, 0, btVector3(-3.8f, 1.f, 0.f));
    m_pWallCollider->pBody->setFriction(0.f);
    m_pWall->position = m_pWallCollider->getPosition();

    m_pLightlessProgram = new QOpenGLShaderProgram();
    m_pLightlessProgram->create();
    m_pLightlessProgram->addShaderFromSourceFile(QOpenGLShader::ShaderTypeBit::Vertex,
        ":/assets/shaders/lightless.vert");
    m_pLightlessProgram->addShaderFromSourceFile(QOpenGLShader::ShaderTypeBit::Fragment,
        ":/assets/shaders/lightless.frag");
    m_pLightlessProgram->link();

    const VertexBuffersData edgeBuffers = VertexBuffers::init(":/assets/models/cube.dae");
    m_pLightless = new Lightless(m_pLightlessProgram, edgeBuffers);
    m_pDebugDrawer = new DebugDrawer(m_pWorld, m_pLightless);
    m_pDebugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
    m_pWorld->setDebugDrawer(m_pDebugDrawer);

    m_viewMatrix = m_pOrbitControls->getViewMatrix();
    m_elapsedTimer.start();
}

void OpenGLWindow::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    m_projMatrix.setToIdentity();
    m_projMatrix.perspective(50.f, w / (float)h, 0.1f, 500.f);
    m_pOrbitControls->resize(w, h);
}

void OpenGLWindow::paintGL()
{
    float deltaTime = m_elapsedTimer.elapsed() / 1000.f;
    m_elapsedTimer.restart();
    keyboardHandler(deltaTime);
    m_pWorld->stepSimulation(deltaTime, 8);

    glClearColor(0.34f, 0.73f, 0.73f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_projViewMatrix = m_projMatrix * m_viewMatrix;

    if (m_showColliders)
    {
        m_pDebugDrawer->updateProjViewMatrix(m_projViewMatrix);
        m_pWorld->debugDrawWorld();
    }

    m_pPlayer->position = m_pPlayerCollider->getPosition();
    m_pPlayer->position.setY(m_pPlayer->position.y() - 0.5f);
    m_pPlayer->rotation = m_pPlayerCollider->getRotation();
    m_pPlayer->draw(m_projViewMatrix);
    m_pGround->draw(m_projViewMatrix);
    m_pWall->draw(m_projViewMatrix);
}

void OpenGLWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    m_pPlayer->cleanUp();
    m_pGround->cleanUp();
    m_pWall->cleanUp();
}

void OpenGLWindow::keyboardHandler(float dt)
{
    if (m_keys[Qt::Key::Key_W] || m_keys[Qt::Key::Key_Up])
    {
        float vy = m_pPlayerCollider->pBody->getLinearVelocity().y();
        btVector3 impulse = btVector3(m_direction.x() * m_linearSpeed * dt, vy,
            m_direction.z() * m_linearSpeed * dt);
        m_pPlayerCollider->pBody->setLinearVelocity(impulse);
    }

    if (m_keys[Qt::Key::Key_S] || m_keys[Qt::Key::Key_Down])
    {
        float vy = m_pPlayerCollider->pBody->getLinearVelocity().y();
        btVector3 impulse = btVector3(-m_direction.x() * m_linearSpeed * dt, vy,
            -m_direction.z() * m_linearSpeed * dt);
        m_pPlayerCollider->pBody->setLinearVelocity(impulse);
    }

    if (m_keys[Qt::Key::Key_A] || m_keys[Qt::Key::Key_Left])
    {
        btVector3 impulse = btVector3(0.f, m_angularSpeed * dt, 0.f);
        m_pPlayerCollider->pBody->setAngularVelocity(impulse);
    }

    if (m_keys[Qt::Key::Key_D] || m_keys[Qt::Key::Key_Right])
    {
        btVector3 impulse = btVector3(0.f, -m_angularSpeed * dt, 0.f);
        m_pPlayerCollider->pBody->setAngularVelocity(impulse);
    }

    m_direction = m_pPlayer->rotation.rotatedVector(m_forward);
}

void OpenGLWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key::Key_W:
        case Qt::Key::Key_Up:
        {
            m_keys[Qt::Key::Key_W] = true;
            m_keys[Qt::Key::Key_Up] = true;
            break;
        }
        case Qt::Key::Key_A:
        case Qt::Key::Key_Left:
        {
            m_keys[Qt::Key::Key_A] = true;
            m_keys[Qt::Key::Key_Left] = true;
            break;
        }
        case Qt::Key::Key_S:
        case Qt::Key::Key_Down:
        {
            m_keys[Qt::Key::Key_S] = true;
            m_keys[Qt::Key::Key_Down] = true;
            break;
        }
        case Qt::Key::Key_D:
        case Qt::Key::Key_Right:
        {
            m_keys[Qt::Key::Key_D] = true;
            m_keys[Qt::Key::Key_Right] = true;
            break;
        }
    }
}

void OpenGLWindow::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key::Key_W:
        case Qt::Key::Key_Up:
        {
            m_keys[Qt::Key::Key_W] = false;
            m_keys[Qt::Key::Key_Up] = false;
            break;
        }
        case Qt::Key::Key_A:
        case Qt::Key::Key_Left:
        {
            m_keys[Qt::Key::Key_A] = false;
            m_keys[Qt::Key::Key_Left] = false;
            break;
        }
        case Qt::Key::Key_S:
        case Qt::Key::Key_Down:
        {
            m_keys[Qt::Key::Key_S] = false;
            m_keys[Qt::Key::Key_Down] = false;
            break;
        }
        case Qt::Key::Key_D:
        case Qt::Key::Key_Right:
        {
            m_keys[Qt::Key::Key_D] = false;
            m_keys[Qt::Key::Key_Right] = false;
            break;
        }
    }
}

void OpenGLWindow::mousePressEvent(QMouseEvent *event)
{
    switch (event->button())
    {
        case Qt::MouseButton::LeftButton:
        {
            int x = event->pos().x();
            int y = event->pos().y();
            m_pOrbitControls->startCameraRotation(x, y);
            break;
        }
        case Qt::MouseButton::RightButton:
        {
            int x = event->pos().x();
            int y = event->pos().y();
            m_pOrbitControls->startCameraPanning(x, y);
            break;
        }
        default:
            break;
    }
}

void OpenGLWindow::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->pos().x();
    int y = event->pos().y();
    m_pOrbitControls->mouseMove(x, y);
}

void OpenGLWindow::mouseReleaseEvent(QMouseEvent *event)
{
    switch (event->button()) {
        case Qt::MouseButton::LeftButton:
        {
            m_pOrbitControls->finishCameraRotation();
            break;
        }
        case Qt::MouseButton::RightButton:
        {
            m_pOrbitControls->finishCameraPanning();
            break;
        }
        default:
            break;
    }
}

void OpenGLWindow::wheelEvent(QWheelEvent *event)
{
    m_pOrbitControls->zoomInZoomOut(event->angleDelta().y());
}
