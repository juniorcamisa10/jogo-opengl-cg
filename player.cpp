#include "player.h"
#include <math.h>

void Player::DesenhaRect(GLint height, GLint width, GLfloat R, GLfloat G, GLfloat B)
{
    glBegin(GL_QUADS);
        glColor3f(R, G, B);
        glVertex3d(-width/2.0, -height/2, 0);
        glVertex3d(-width/2.0, height/2, 0);
        glVertex3d(width/2.0, height/2, 0);
        glVertex3d(width/2.0, -height/2, 0);
    glEnd();
}

void Player::DesenhaCirc(GLint radius, GLfloat R, GLfloat G, GLfloat B)
{
    glColor3f(R, G, B);
    glPointSize(2);
    glBegin(GL_POLYGON);
        
        for (int i = 0; i <= 360; i+=20){
            float angulo = (i * 3.14159f) / 180.0f;
            float x = radius * cosf(angulo);
            float y = radius * sinf(angulo);
            glVertex3f(x, y, 0);
        }
        
    glEnd();
}

void Player::DesenhaRoda(GLfloat x, GLfloat y, GLfloat thetaWheel, GLfloat R, GLfloat G, GLfloat B)
{
    //glPushMatrix();
    glTranslatef(x, y, 0);
    glRotatef(thetaWheel, 0, 0, 1);
    DesenhaCirc(gRaio, R, G, B);
    //glPopMatrix();
}

void Player::DesenhaCanhao(GLfloat x, GLfloat y, GLfloat theta2){
    glPushMatrix();
    glTranslatef(x, y, 0);
    glRotatef(theta2, 0, 0, 1);
    glTranslatef(0, gRaio*0.50f, 0);
    DesenhaRect(alturaBraco, larguraBraco, 0, 0, 0);
    DesenhaRect(alturaBraco-2, larguraBraco-2, 255, 0, 0);
    glPopMatrix();
}

void Player::DesenhaPerna(GLfloat x, GLfloat y){
    glPushMatrix();
    glTranslatef(x, y, 0);
    //glRotatef(theta2, 0, 0, 1);
    DesenhaRect(alturaBraco, larguraBraco, 0, 0, 0);
    glPopMatrix();
}

void Player::DesenhaPlayer(GLfloat x, GLfloat y, GLfloat thetaWheel, GLfloat theta1, GLfloat theta2)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    glRotatef(theta1, 0, 0, 1);
    
    DesenhaPerna((-3.0f*gRaio)/4.0f, 0);
    DesenhaPerna((3.0f*gRaio)/4.0f, 0);
    DesenhaCirc(gRaio+2, 0, 0, 0);
    DesenhaCirc(gRaio, R, G, B);
    //DesenhaCanhao(gRaio + larguraBraco/2, -gRaio, theta2);
    DesenhaCanhao(gRaio + larguraBraco/2, 0, theta2);
    
    glPopMatrix();
}

void Player::RodaBraco1(GLfloat inc)
{
    gTheta1 += inc;
}

void Player::RodaBraco2(GLfloat inc)
{
    gTheta2 += inc;
}

void Player::MoveEmX(GLfloat dx)
{
    GLfloat angleRad = gTheta1 * (3.14159f/180);
    GLfloat dirCorpoX = sinf(angleRad);
    gX += dirCorpoX * dx;
}

void Player::MoveEmY(GLfloat dy){
    GLfloat angleRad = gTheta1 * (3.14159f/180);
    GLfloat dirCorpoY = -cosf(angleRad);
    gY += dirCorpoY * dy;
}

void Player::MudaX(GLfloat gx){
    gX = gx;
}
void Player::MudaY(GLfloat gy){
    gY = gy;
}

void Player::MudaTheta1(GLfloat theta1){
    gTheta1 = theta1;
}
void Player::MudaTheta2(GLfloat theta2){
    gTheta2 = theta2;
}

void Player::MoveEmXY(GLfloat dx, GLfloat dy){
    GLfloat angleRad = gTheta1 * (3.14159f/180);
    GLfloat dirCorpoX = sinf(angleRad);
    GLfloat dirCorpoY = -cosf(angleRad);

}

//Funcao auxiliar de rotacao
void RotatePoint(GLfloat x, GLfloat y, GLfloat angle, GLfloat &xOut, GLfloat &yOut){

    GLfloat angleRad = angle * (3.14159f/180);
    GLfloat xOutTemp = x * cosf(angleRad) - y * sinf(angleRad);
    GLfloat yOutTemp = x * sinf(angleRad) + y * cosf(angleRad);

    xOut = xOutTemp;
    yOut = yOutTemp;
}

Tiro* Player::Atira(GLfloat gVel)
{
    GLfloat xBase = 0, yBase = gRaio*0.50f, xPonta = 0, yPonta = (gRaio*0.50f) + (alturaBraco/2), angulo;
    RotatePoint(xPonta, yPonta, gTheta2, xPonta, yPonta);
    RotatePoint(xBase, yBase, gTheta2, xBase, yBase);

    xBase += (larguraBraco/2) + gRaio;
    xPonta += (larguraBraco/2) + gRaio;
    yBase += 0;
    yPonta += 0;

    RotatePoint(xPonta, yPonta, gTheta1, xPonta, yPonta);
    RotatePoint(xBase, yBase, gTheta1, xBase, yBase);

    xBase += gX;
    xPonta += gX;
    yBase += gY;
    yPonta += gY;

    angulo = atan2f(yPonta - yBase, xPonta - xBase);
    angulo = (angulo * 180) / 3.14159f;

    Tiro *t = new Tiro(xPonta, yPonta, angulo, gVel);

    return t;
}

void Player::defineRaio(GLfloat raio){
    gRaio = raio;
    larguraBraco = raio * 0.42f;
    alturaBraco = raio * 2.0f;
}

void Player::andaComBarril(GLfloat timeDiference, GLfloat vel){
    gY -= vel * timeDiference;
}

void Player::defineAnguloInicial(GLfloat angulo){
    gTheta1 = angulo;
}

void Player::mudaCor(GLfloat red, GLfloat green, GLfloat blue){
    R = red;
    G = green;
    B = blue;
}