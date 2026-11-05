#include "barril.h"
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

void Barril::DesenhaDetalhes(GLint height, GLint width, GLfloat R, GLfloat G, GLfloat B){
    
    glBegin(GL_LINES);
        glLineWidth(2.0f);
        glColor3f(R, G, B);
        glVertex2f(-width/2.0f, height/4.0f);
        glVertex2f(width/2.0f, height/4.0f);
        glVertex2f(-width/2.0f, height/2.0f);
        glVertex2f(width/2.0f, height/2.0f);
        glVertex2f(-width/2.0f, 3.0f*height/4.0f);
        glVertex2f(width/2.0f, 3.0f*height/4.0f);
    glEnd();
}

void Barril::DesenhaLinhaFechada(GLint height, GLint width, GLfloat R, GLfloat G, GLfloat B){
    
    glBegin(GL_LINE_LOOP);
        glLineWidth(2.0f);
        glColor3f(R, G, B);
        glVertex2f(-width/2.0f, 0.0f);
        glVertex2f(-width/2.0f, height);
        glVertex2f(width/2.0f, height);
        glVertex2f(width/2.0f, 0.0f);
    glEnd();
}

void Barril::DesenhaRect(GLint height, GLint width, GLfloat R, GLfloat G, GLfloat B)
{
    glBegin(GL_QUADS);
        glColor3f(R, G, B);
        glVertex3d(-width/2.0, 0, 0);
        glVertex3d(-width/2.0, height, 0);
        glVertex3d(width/2.0, height, 0);
        glVertex3d(width/2.0, 0, 0);
    glEnd();
}

void Barril::DesenhaCirc(GLint radius, GLfloat R, GLfloat G, GLfloat B)
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

void Barril::ImprimeVida(GLfloat x, GLfloat y)
{
    static char str[2];
    void * font = GLUT_BITMAP_9_BY_15;
    glColor3f(0.0f, 0.0f, 0.0f);
    //Cria a string a ser impressa
    char *tmpStr;
    sprintf(str, "%d", gVida);
    //Define a posicao onde vai comecar a imprimir
    glRasterPos2f(x, y);
    //Imprime um caractere por vez
    tmpStr = str;
    while( *tmpStr ){
        glutBitmapCharacter(font, *tmpStr);
        tmpStr++;
    }
}

void Barril::DesenhaBarril(GLfloat x, GLfloat y)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    //glRotatef(gDirectionAng, 0, 0, 1);
    DesenhaLinhaFechada(alturaBarril+1, larguraBarril+1, 0.40f, 0.18f, 0.04f);
    DesenhaRect(alturaBarril, larguraBarril, 0.96f, 0.52f, 0.18f);
    DesenhaDetalhes(alturaBarril+1, larguraBarril+1, 0.78f, 0.38f, 0.08f);
    ImprimeVida((-larguraBarril/2)+2, 2);
    glPopMatrix();
}

void Barril::Recria(GLfloat x, GLfloat y)
{
    gX = x;
    gY = y;
    temInimigo = false;
}

bool Barril::Atingido(Tiro *tiro)
{
    GLfloat tiroX, tiroY;
    tiro->GetPos(tiroX, tiroY);

    GLfloat barrilEsquerda = gX - (larguraBarril/2.0f);
    GLfloat barrilDireita = gX + (larguraBarril/2.0f);
    GLfloat barrilBaixo = gY;
    GLfloat barrilCima = gY + larguraBarril;

    if(tiroX >= (barrilEsquerda-raioTiro) && tiroX <= (barrilDireita-raioTiro) && tiroY >= (barrilBaixo-raioTiro) && tiroY <= (barrilCima+raioTiro)){
        gVida -= 1;
        return true;
    }
    else{
        return false;
    }
}

bool Barril::Morreu(){
    if(gVida == 0){
        return true;
    }
    else{
        return false;
    }
}

void Barril::Move(GLfloat timeDiference){
    gY -= gVel * timeDiference;
}

void Barril::SetElementos(GLfloat vel, GLfloat largura, GLfloat altura, GLfloat resistencia, GLfloat chance){
    gVel = vel;
    larguraBarril = largura;
    alturaBarril = altura;
    gVida = resistencia;
    chanceInimigo = chance;
}

void Barril::SetTemInimigo(bool x){
    temInimigo = x;
}