#ifndef BARRIL_H
#define	BARRIL_H
#include <GL/gl.h>
#include <GL/glu.h>
#include "player.h"
#include "tiro.h"

class Barril {
    GLfloat gX; 
    GLfloat gY;
    GLfloat gVel;
    GLfloat larguraBarril;
    GLfloat alturaBarril;
    GLfloat chanceInimigo;
    bool temInimigo;
    int gVida;
private:
    void DesenhaDetalhes(GLint height, GLint width, GLfloat R, GLfloat G, GLfloat B);
    void DesenhaLinhaFechada(GLint height, GLint width, GLfloat R, GLfloat G, GLfloat B);
    void DesenhaRect(GLint height, GLint width, GLfloat R, GLfloat G, GLfloat B);
    void DesenhaCirc(GLint radius, GLfloat R, GLfloat G, GLfloat B);
    void DesenhaBarril(GLfloat x, GLfloat y);
    void ImprimeVida(GLfloat x, GLfloat y);
public:
    Barril(GLfloat x, GLfloat y){
        gX = x; 
        gY = y;
        gVida = 5;
        gVel = 0.1;
        larguraBarril = 0;
        alturaBarril = 0;
        chanceInimigo = 0;
        temInimigo = false;
    };
    void Desenha(){ 
        DesenhaBarril(gX, gY);
    };
    void Recria(GLfloat x, GLfloat y);
    bool Atingido(Tiro *tiro);
    bool Morreu();
    void Move(GLfloat timeDiference);
    GLfloat ObtemX(){
        return gX;
    }
    GLfloat ObtemY(){
        return gY;
    }
    void SetElementos(GLfloat vel, GLfloat largura, GLfloat altura, GLfloat resistencia, GLfloat chance);
    void SetTemInimigo(bool x);
    bool TemInimigo(){
        return temInimigo;
    }
};

#endif	/* BARRIL_H */

