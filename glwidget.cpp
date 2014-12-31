#include "glwidget.h"
#include <QDebug>
#include <QMatrix4x4>
#include <math.h>

#define MAX_NUM_PARTICLES 1500
#define MAX_AGE 200
#define MAX_BOUNCING_COUNT 5
#define MAX_NOISE_WIDTH 256
#define MAX_NOISE_HEIGHT 256
#define PI 3.14159
#define PERSIST 1
#define OCTAVE 4

//Particle Struct for holding all information on each particle
struct PARTICLE{
    QVector3D position;
    QVector3D speed;
    QVector3D targetSpeed;
    QVector3D color;
    bool active;
    int age;
    int maxAge;
    int bounceCount;
}
//The particles
Particles[MAX_NUM_PARTICLES];

//Constructor
GLWidget::GLWidget( QWidget *parent ) : QGLWidget( parent ) {
    size = 0.1f;
    sim = new QTimer(this);
    connect(sim, SIGNAL(timeout()), this, SLOT(calc()));
    sim->start(10);

    soundEffect = new QSound("/Users/mghinton/Desktop/fire/dsflame.wav", this);
    if(QSound::isAvailable())
        qDebug() << "Sound Effect Enabled";
    else
        qDebug() << "Sound Effect Not Enabled";
    soundEffect->setLoops(300);
    soundEffect->play();
}

GLWidget::~GLWidget() {

}
//====================Mouse Camera=====================
void GLWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        lastMousePoint = e->pos();
        Rotating = true;
    }

    if (e->button() == Qt::RightButton)
    {
        lastMousePoint = e->pos();
        Scaling = true;
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton && Rotating)
        {
            DoRotate(e->pos(), lastMousePoint);
            Rotating = false;
        }

        if (e->button() == Qt::RightButton && Scaling)
        {
            DoScale(e->pos(), lastMousePoint);
            Scaling = false;
        }
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
    if ((e->buttons() & Qt::LeftButton) && Rotating)
    {
        DoRotate(e->pos(), lastMousePoint);
        lastMousePoint = e->pos();
    }

    if ((e->buttons() & Qt::RightButton) && Scaling)
    {
        DoScale(e->pos(), lastMousePoint);
        lastMousePoint = e->pos();
    }
}

void GLWidget::DoRotate(QPoint desc, QPoint orig)
{
    //A version using the classes provided by Qt, instead of our demo/simple martix class
    qreal YRot = (desc.x() - orig.x()) * (-1);
    QMatrix4x4 rotation;
    rotation.setToIdentity();;
    rotation.rotate(YRot, 0, 1, 0);
    qreal ZRot = (desc.y() - orig.y()) * (-1);
    rotation.rotate(ZRot, 0, 0, 1);
    CameraPos = rotation * CameraPos;
    //perspCameraDirty = true;
}

void GLWidget::DoScale(QPoint desc, QPoint orig)
{
    qreal newLength = (desc.y() - orig.y()) * (-0.1) + CameraPos.length();
    if (newLength > 0.5)
    {
        CameraPos = CameraPos.normalized() * newLength;
        //perspCameraDirty = true;
    }
}
//====================================================

//Sets up the initial scene.
void GLWidget::initializeGL() {
    static const GLfloat lightPos[4] = { 2.0f, 2.0f, 2.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    CameraPos = QVector3D(0,0,5);

    //Initial setup of all the particles
    loadTextures();
    initializeParticles();
    activateParticles();

}

//Paint
void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 60.0, 1.0, 0.5, 15);
    gluLookAt(CameraPos.x(), CameraPos.y(), CameraPos.z(), 0, 2, 0, 0, 1, 0);

    //Drawing the particles
    renderParticles();

}

//Resize
void GLWidget::resizeGL( int width, int height ) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 60.0, 1.0, 0.5, 10.0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    updateGL();
}

//Initalizing all particles in the array
void GLWidget::initializeParticles()
{
    srand((int)time(NULL));

    for(int i = 0; i < MAX_NUM_PARTICLES; i++)
    {
        Particles[i].targetSpeed = QVector3D(0.005*sin(rand()%360*PI/180), fabs(0.025*cos(rand()%360*PI/180)), 0);
        Particles[i].active = false;
    }
}

//Activating all the particles in the array
void GLWidget::activateParticles()
{
    for(int i = 0; i < MAX_NUM_PARTICLES; i++)
    {
        if(!Particles[i].active)
        {
            Particles[i].active = true;             //Sets the particle active
            Particles[i].age = (rand()%MAX_AGE)+1;  //Gives it a random age
            Particles[i].maxAge = MAX_AGE;          //Sets the max age of the particle
            Particles[i].bounceCount = 0;           //Bounce count for particles with negative velocity

            float ranposx = (((float)((rand()%85)))/80.0f)-0.5f;
            center = QVector3D(ranposx, 0, 0);
            Particles[i].position = center;

            //Creates random speeds for all three axis, sets that as the particles initial speed
            float ranx = (((float)((rand()%100)+1))/1000.0f)-0.05f;
            float rany = (((float)((rand()%200)+50))/5000.0f);
            float ranz = (((float)((rand()%100)+1))/1000.0f)-0.05f;
            Particles[i].speed = QVector3D(ranx, rany, ranz);
        }
    }
}

//Adjusting the particles each step of rendering
void GLWidget::adjustParticles()
{
    for(int i = 0; i < MAX_NUM_PARTICLES; i++)
    {
        if(Particles[i].active)
        {
            //Adjusts the particle speed towards the target speed by 5%
            QVector3D t = Particles[i].targetSpeed;
            QVector3D s = Particles[i].speed;
            Particles[i].speed = QVector3D(s.x() + ((t.x()-s.x())/20), s.y() + ((t.y()-s.y())/20), s.z() + (t.z()-s.z())/20);
            s = Particles[i].speed;

            //Adjusts the particles new position based on it's speed
            QVector3D p = Particles[i].position;
            Particles[i].position = QVector3D(p.x()+s.x(), p.y()+s.y(), p.z()+s.z());

            //Checks if the particle has a negative position, if it does, inverts it's speed and increase the bounce count
            if(Particles[i].position.y() < 0)
            {
                Particles[i].position.setY(0);
                Particles[i].speed.setY(-1*Particles[i].speed.y());
                Particles[i].bounceCount++;
                if(Particles[i].bounceCount > MAX_BOUNCING_COUNT)
                    Particles[i].active = false;
            }

            //Randomly assigns noise to 25% of the particles
            if(rand()%100<25)
            {
                float noise1;
                noise1 = perlin(Particles[i].position.x(), Particles[i].position.y());
                Particles[i].speed.setX(Particles[i].speed.x() + noise1/4500);
            }

            //Changes the particles color based on how old it is
            Particles[i].color = getColour(Particles[i].age);
            Particles[i].age++;

            //If the particle reaches it's max age, it deactivates it.
            if(Particles[i].age > Particles[i].maxAge)
                Particles[i].active = false;
        }
    }
}

//Rendering the particle
void GLWidget::renderParticles()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_COLOR, GL_ONE);

    for(int i = 0; i < MAX_NUM_PARTICLES; i++)
    {
        if(Particles[i].active)
        {
            QVector3D color = Particles[i].color;
            glColor4f(color.x(),color.y(),color.z(), 1);
            glPushMatrix();
            glTranslatef(Particles[i].position.x(), Particles[i].position.y(), Particles[i].position.z());
            glBegin(GL_TRIANGLE_FAN);
                glNormal3f(0,0,1);

                //glTexCoord2f(0,0);
                glVertex3f(-size, -size, 0);

                //glTexCoord2f(1,0);
                glVertex3f(size, -size, 0);

                //glTexCoord2f(1,1);
                glVertex3f(size, size, 0);

                //glTexCoord2f(0,1);
                glVertex3f(-size, size, 0);
            glEnd();

            glPopMatrix();
        }
    }
    glEnable(GL_DEPTH_TEST);
}

//Slot to adjust, the particles, update the scene, and then activate any deactivated particles
void GLWidget::calc()
{
    adjustParticles();
    updateGL();
    activateParticles();
}

//Returns the color based on the age of the particle
QVector3D GLWidget::getColour(int age)
{
    age*=100;
    return QVector3D(0.75 + (pow(age,2)*.2), 0.40 + pow(age,2)*.2, 0.05 + pow(age,2)*.1);
    /*
    int c = age/MAX_AGE;
    QColor color;
    color.setHsv(c*60, 150, 50);
    //qDebug() << color.red() << " " << color.green() << " " << color.blue();
    return QVector3D(color.red(), color.green(), color.blue());

    if(age < MAX_AGE/3)
    {
        return QVector3D(1,1,1);
    }
    else if(age > MAX_AGE/3 && age < MAX_AGE - (MAX_AGE/3))
    {
        return QVector3D(1,0.69,0.4);
    }
    else
    {
        return QVector3D(1,0.2,0.2);
    }*/
}

//Loading the texture for each particle
//PATH MUST BE CHANGED IF WORKING ON DIFFERENT MACHINE
void GLWidget::loadTextures()
{
    glEnable(GL_TEXTURE_2D);
    QImage text[1], buffer[1];
    if(!buffer[0].load("/Users/mghinton/Desktop/fire/fire2.png"))
        qDebug() << "Loading fire texture failed.";

    text[0] = QGLWidget::convertToGLFormat(buffer[0]);
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, text[0].width(), text[0].height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, text[0].bits());

}

//Interpolation function for perlin noise
float GLWidget::cosInterp(float a, float b, float x)
{
    float f_t = x*PI;
    float f = (1-cos(f_t))*0.5;
    //return a*(1-x) + b*x;
    return a*(1-f) + b*f;
}

//Generating "noise" based on primes.
float GLWidget::noise(int x, int y)
{
    int n = x + y * 57;
    n = pow((n<<13),n);
    return ( 1.0 - ( (x * (x * x * 101 + 211) + 491) & 7) / 1279.0);
}

//Calculating smooth noise
float GLWidget::smoothNoise(float x, float y)
{
    float center = noise(x, y)/4;
    float side = (noise(x-1, y) + noise(x+1, y) + noise(x, y-1) + noise(x, y+1))/8;
    float corner= (noise(x-1, y-1) + noise(x+1, y-1) + noise(x-1, y+1) + noise(x+1, y+1))/16;
    return center + side + corner;
}

//Interpolation of the perlin noise
float GLWidget::interpolateNoise(float x, float y)
{
    int intX = (int)x;
    float fractX = x - intX;

    int intY = (int)y;
    float fractY = y - intY;

    float x0 = smoothNoise(intX, intY);
    float x1 = smoothNoise(intX+1, intY);
    float x2 = smoothNoise(intX, intY+1);
    float x3 = smoothNoise(intX+1, intY+1);

    float interp0 = cosInterp(x0, x1, fractX);
    float interp1 = cosInterp(x2, x3, fractX);

    return cosInterp(interp0, interp1, fractY);
}

//Starting function of the perlin noise
float GLWidget::perlin(float x, float y)
{
    float total = 0;
    float p = PERSIST;
    int n = OCTAVE - 1;

    for(int i = 0; i < n; i++)
    {
        float freq = pow(2, i);
        float amp = pow(p, i);
        total += interpolateNoise(x * freq, y * freq) * amp;
    }
    return total/3;
}
