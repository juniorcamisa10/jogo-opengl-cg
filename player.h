#ifndef PLAYER_H
#define	PLAYER_H
#include <GL/gl.h>
#include <GL/glu.h>
#include "tiro.h"

class Player {
    GLfloat gX; 
    GLfloat gY; 
    GLfloat gTheta1; 
    GLfloat gTheta2;  
    GLfloat gThetaWheel; 
    GLfloat direction;
    GLfloat gRaio;
    GLfloat alturaBraco;
    GLfloat larguraBraco;
    GLfloat R;
    GLfloat G;
    GLfloat B;
private:
    void DesenhaRect(  GLint height, GLint width, 
                    GLfloat R, GLfloat G, GLfloat B);
    void DesenhaCirc(  GLint radius, GLfloat R, 
                    GLfloat G, GLfloat B);
    void DesenhaRoda(GLfloat x, GLfloat y, GLfloat thetaWheel, 
                    GLfloat R, GLfloat G, GLfloat B);
    void DesenhaCanhao(GLfloat x, GLfloat y, GLfloat theta2);
    void DesenhaPerna(GLfloat x, GLfloat y);
    void DesenhaPlayer( GLfloat x, GLfloat y, GLfloat thetaWheel, 
                    GLfloat theta1, GLfloat theta2);

public:
    Player(){
        gX = 0; 
        gY = -200; 
        gTheta1 = 0; 
        gTheta2 = 0;  
        gThetaWheel = 0;
        direction = 0;
        gRaio = 0;
        alturaBraco = 0;
        larguraBraco = 0;
        R = 0;
        G = 0;
        B = 0;
    };
    Player(GLfloat x, GLfloat y){
        gX = x; 
        gY = y; 
        gTheta1 = 0; 
        gTheta2 = 0; 
        gThetaWheel = 0;
        direction = 0;
        gRaio = 0;
        alturaBraco = 0;
        larguraBraco = 0;
        R = 0;
        G = 0;
        B = 0;
    }
    void Desenha(){ 
        DesenhaPlayer(gX, gY, gThetaWheel, gTheta1, 
                gTheta2);
    };
    void RodaBraco1(GLfloat inc);
    void RodaBraco2(GLfloat inc);
    void MoveEmX(GLfloat dx);
    void MoveEmY(GLfloat dy);
    void MudaX(GLfloat gx);
    void MudaY(GLfloat gy);
    void MudaTheta1(GLfloat theta1);
    void MudaTheta2(GLfloat theta2);
    void MoveEmXY(GLfloat dx, GLfloat dy);
    Tiro* Atira(GLfloat gVel);
    GLfloat ObtemX(){
        return gX;
    };
    GLfloat ObtemY(){
        return gY;
    };
    GLfloat ObtemTheta1(){
        return gTheta1;
    };
    GLfloat ObtemTheta2(){
        return gTheta2;
    };
    void GetPos(GLfloat &xOut, GLfloat &yOut){
        xOut = gX;
        yOut = gY;
    };
    void defineRaio(GLfloat raio);
    void andaComBarril(GLfloat timeDiference, GLfloat vel);
    void defineAnguloInicial(GLfloat angulo);
    void mudaCor(GLfloat red, GLfloat green, GLfloat blue);
};

#endif	/* PLAYER_H */

