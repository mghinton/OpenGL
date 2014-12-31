#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL/QGLWidget>
#include <glu.h>
#include <QVector3D>
#include <time.h>
#include <QTimer>
#include <glu.h>
#include <QMouseEvent>
#include <QSound>

class GLWidget : public QGLWidget {
    Q_OBJECT

public:
    /// CONSTRUCTOR
    GLWidget(QWidget *parent = 0);

    /// DESTRUCTOR
    ~GLWidget();

    //QTimer *drawing;
    QTimer *sim;
    bool debug;
    GLuint texture;
    float size;
    QVector3D center;
    QSound * soundEffect;

private:

    //Particle Engine function
    void initializeParticles();
    void activateParticles();
    void renderParticles();
    void adjustParticles();

    QVector3D getColour(int);

    //Perlin Noise functions
    float cosInterp(float, float, float);
    float noise(int, int);
    float smoothNoise(float, float);
    float interpolateNoise(float, float);
    float perlin(float, float);

    //Camera functions
    QVector3D CameraPos;
    QPoint lastMousePoint;
    bool Rotating;
    bool Scaling;
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void DoRotate(QPoint, QPoint);
    void DoScale(QPoint, QPoint);

protected:
    /// OPENGL
    void initializeGL();
    void paintGL();
    void resizeGL( int width, int height );
    void loadTextures();

private slots:
    void calc();
};

#endif // GLWIDGET_H
