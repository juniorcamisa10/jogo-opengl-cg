#include "tiro.h"
#include <math.h>
#define DISTANCIA_MAX 500

void Tiro::DesenhaCirc(GLint radius, GLfloat R, GLfloat G, GLfloat B)
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

void Tiro::DesenhaTiro(GLfloat x, GLfloat y)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    DesenhaCirc(raioTiro, 255, 255, 255);
    glPopMatrix();
}

void Tiro::Move(GLfloat timeDiference)
{
    GLfloat angulo = (gDirectionAng * 3.14159f) / 180.0f;
    gX += timeDiference * gVel * cosf(angulo);
    gY += timeDiference * gVel * sinf(angulo);
}

bool Tiro::Valido()
{
    GLfloat distancia = sqrtf(powf(gX - gXInit, 2) + powf(gY - gYInit, 2));
    if(distancia > DISTANCIA_MAX){
        return false;
    }

    return true;
}