#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <list>
#include "player.h"
#include "tiro.h"
#include "barril.h"
#include "tinyxml2.h"

int alturaArena;
int larguraArena;
float raioCabecaJogador;
float velocidadeJogador;
float raioCabecaInimigo;
int tirosPorSegundo;
float velocidadeTiro;
float alturaBarril;
float larguraBarril;
int resistenciaBarril;
int barrisParaGanhar;
float velocidadeBarril;
float velocidadeGiro;
float limiteGiroArma;
float chanceInimigo;

//Key status
int keyStatus[256];
int opcaoMenu = 0;

//Ultima posição do mouse
int ultimaPosMouse = -1;
int mouseX;

//Componentes do mundo virtual sendo modelado
Player player;
std::list<Tiro*> tiros;
std::list<Tiro*> tirosInimigos;
float tempoEntreTiros = 0.0f;
Barril alvo(0, 250);
Player *inimigo = new Player(0, 250+20);

enum EstadoJogo { JOGANDO, VITORIA, DERROTA, MENU };
EstadoJogo estado = MENU;
int atingido = 0;
int barrisDestruidos = 0;
static char str[1000];
void * font = GLUT_BITMAP_9_BY_15;

bool carregarConfiguracoesXML(){
    tinyxml2::XMLDocument doc;
    doc.LoadFile("configuracoes.xml");

    tinyxml2::XMLElement* root = doc.FirstChildElement("jogo");
    if (!root) return false;

    tinyxml2::XMLElement* arena = root->FirstChildElement("arena");
    if (arena) {
        arena->QueryIntAttribute("largura", &larguraArena);
        arena->QueryIntAttribute("altura", &alturaArena);
        //player.MudaY(-alturaArena/2);
    }

    tinyxml2::XMLElement* jogador = root->FirstChildElement("jogador");
    if (jogador) {
        jogador->QueryFloatAttribute("raioCabeca", &raioCabecaJogador);
        player.defineRaio(raioCabecaJogador);
        jogador->QueryFloatAttribute("raioCabeca", &raioCabecaInimigo);
        jogador->QueryFloatAttribute("velocidade", &velocidadeJogador);
        jogador->QueryFloatAttribute("velocidadeGiro", &velocidadeGiro);
        jogador->QueryFloatAttribute("limiteGiroArma", &limiteGiroArma);
    }

    tinyxml2::XMLElement* barril = root->FirstChildElement("barril");
    if (barril) {
        barril->QueryFloatAttribute("largura", &larguraBarril);
        barril->QueryFloatAttribute("altura", &alturaBarril);
        barril->QueryFloatAttribute("velocidade", &velocidadeBarril);
        barril->QueryIntAttribute("resistencia", &resistenciaBarril);
        barril->QueryFloatAttribute("chanceInimigo", &chanceInimigo);
        alvo.SetElementos(velocidadeBarril, larguraBarril, alturaBarril, resistenciaBarril, chanceInimigo);
        if(((rand() % 100) / 100.0f) < chanceInimigo){
            alvo.SetTemInimigo(true);
        }
        inimigo->defineRaio(raioCabecaInimigo);
        inimigo->defineAnguloInicial(180.0f);
        inimigo->mudaCor(0.65f, 0.25f, 0.85f);
    }

    tinyxml2::XMLElement* partida = root->FirstChildElement("partida");
    if (partida) {
        partida->QueryIntAttribute("barrisParaGanhar", &barrisParaGanhar);
    }

    tinyxml2::XMLElement* tiro = root->FirstChildElement("tiro");
    if (tiro) {
        tiro->QueryIntAttribute("cadencia", &tirosPorSegundo);
        tiro->QueryFloatAttribute("velocidade", &velocidadeTiro);
    }
    return true;
}

//Imprime o placar com a quantidade de inimigos destruidos e quanto falta para vencer
void ImprimePlacar(GLfloat x, GLfloat y)
{
    glColor3f(0.0f, 0.0f, 0.0f);
    //Cria a string a ser impressa
    char *tmpStr;
    sprintf(str, "Pontuacao: %d Para Vencer: %d", barrisDestruidos, barrisParaGanhar-barrisDestruidos);
    //Define a posicao onde vai comecar a imprimir
    glRasterPos2f(x, y);
    //Imprime um caractere por vez
    tmpStr = str;
    while( *tmpStr ){
        glutBitmapCharacter(font, *tmpStr);
        tmpStr++;
    }
}


float calculaLarguraTexto(void *fonte, const char *texto){
    float largura = 0;
    int tam = strlen(texto);

    for(int i=0;i<tam;i++){
        largura += glutStrokeWidth(fonte, texto[i]);
    }

    return largura;
}

void desenharTextoStroke(void *fonte, GLfloat x, GLfloat y, GLfloat escala, const char *texto){
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(escala, escala, 1.0f);
    
    int tam = strlen(texto);
    for(int i=0;i<tam;i++){
        glutStrokeCharacter(fonte, texto[i]);
    }

    glPopMatrix();

}

void TelaFinal(){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glLineWidth(0.1f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glDisable(GL_LINE_STIPPLE);
    glColor3f(1.0f, 1.0f, 1.0f);

    const char *titulo = "Game Over - Voce Morreu!";
    const char *opcao1 = "Parabens - Voce Venceu!";
    const char *opcao2 = "Aperte [R] para voltar ao menu ou [ESC] para sair";
    GLfloat escala = 0.1f;
    GLfloat x;

    if(estado == VITORIA){
        x = (-(calculaLarguraTexto(GLUT_STROKE_ROMAN, opcao1)*escala))/2.0f;
        desenharTextoStroke(GLUT_STROKE_ROMAN, x, 0.0f, escala, opcao1);
        x = (-(calculaLarguraTexto(GLUT_STROKE_ROMAN, opcao2)*escala))/2.0f;
        desenharTextoStroke(GLUT_STROKE_ROMAN, x, -30.0f, escala, opcao2);
    }
    else{
        x = (-(calculaLarguraTexto(GLUT_STROKE_ROMAN, titulo)*escala))/2.0f;
        desenharTextoStroke(GLUT_STROKE_ROMAN, x, 0.0f, escala, titulo);
        x = (-(calculaLarguraTexto(GLUT_STROKE_ROMAN, opcao2)*escala))/2.0f;
        desenharTextoStroke(GLUT_STROKE_ROMAN, x, -30.0f, escala, opcao2);
    }

    glutSwapBuffers();
}

void desenhaMenu(){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glLineWidth(1.5f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glDisable(GL_LINE_STIPPLE);

    glColor3f(1.0f, 1.0f, 0.0f);
    const char *titulo = "JOGO DO BARRIL";
    const char *opcao1 = "INICIAR";
    const char *opcao2 = "SAIR";
    GLfloat escala = 0.4f;
    GLfloat escala1 = 0.1f;
    GLfloat escala2 = 0.2f;

    GLfloat x = (-(calculaLarguraTexto(GLUT_STROKE_ROMAN, titulo)*escala))/2.0f;

    desenharTextoStroke(GLUT_STROKE_ROMAN, x, 0.0f, escala, titulo);

    glColor3f(1.0f, 1.0f, 1.0f);
    if(opcaoMenu){
        x = (-(calculaLarguraTexto(GLUT_STROKE_ROMAN, opcao1)*escala1))/2.0f;
        desenharTextoStroke(GLUT_STROKE_ROMAN, x, -alturaArena/4.0f, escala1, opcao1);
        x = (-(calculaLarguraTexto(GLUT_STROKE_ROMAN, opcao2)*escala2))/2.0f;
        desenharTextoStroke(GLUT_STROKE_ROMAN, x, (-alturaArena/4.0f)-30, escala2, opcao2);
    }
    else{
        x = (-(calculaLarguraTexto(GLUT_STROKE_ROMAN, opcao1)*escala2))/2.0f;
        desenharTextoStroke(GLUT_STROKE_ROMAN, x, -alturaArena/4.0f, escala2, opcao1);
        x = (-(calculaLarguraTexto(GLUT_STROKE_ROMAN, opcao2)*escala1))/2.0f;
        desenharTextoStroke(GLUT_STROKE_ROMAN, x, (-alturaArena/4.0f)-30, escala1, opcao2);
    }

    glutSwapBuffers();
}

void resetaJogo(){

    opcaoMenu = 0;
    ultimaPosMouse = -1;

    tempoEntreTiros = 0.0f;
    atingido = 0;
    barrisDestruidos = 0;
}

void renderScene(void)
{
    // Clear the screen.
    glClear(GL_COLOR_BUFFER_BIT);

    if(estado == MENU){
        desenhaMenu();
    }
    else if(estado == JOGANDO){

        //Aqui desenha um retângulo com as dimensões da janela de visualização paara usar como fundo
        glColor3f(0.68f, 0.83f, 0.96f);
        glBegin(GL_QUADS);
        glVertex2f(-larguraArena/2, -alturaArena/2);
        glVertex2f(-larguraArena/2, alturaArena/2);
        glVertex2f(larguraArena/2, alturaArena/2);
        glVertex2f(larguraArena/2, -alturaArena/2);
        glEnd();

        player.Desenha();
        
        // Anda pela lista de tiros e desenha um por um
        for (Tiro* t : tiros) {
            t->Desenha();
        }

        // Anda pela lista de tiros e desenha um por um
        for (Tiro* t : tirosInimigos) {
            t->Desenha();
        }
        
        alvo.Desenha();
        if(alvo.TemInimigo()){
            inimigo->Desenha();
        }

        ImprimePlacar((-larguraArena/2)+1, (-alturaArena/2)+1);
    }

    if(estado == DERROTA || estado == VITORIA){
        TelaFinal();
    }

    glutSwapBuffers(); // Desenha o novo frame do game.
}

void keyPress(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'a':
        case 'A':
             keyStatus[(int)('a')] = 1; //Using keyStatus trick
             //player.RodaBraco1(+INC_KEY);
             break;
        case 'd':
        case 'D':
             keyStatus[(int)('d')] = 1; //Using keyStatus trick
             //player.RodaBraco1(-INC_KEY);
             break;
        case 'w':
        case 'W':
             if(estado == JOGANDO){
                keyStatus[(int)('w')] = 1;
             }
             else{
                if(opcaoMenu){
                    opcaoMenu = 0;
                }
                else{
                    opcaoMenu = 1;
                }
             }
             break;
        case 's':
        case 'S':
             if(estado == JOGANDO){
                keyStatus[(int)('s')] = 1;
             }
             else{
                if(opcaoMenu){
                    opcaoMenu = 0;
                }
                else{
                    opcaoMenu = 1;
                }
             }
             break;
        case 13:
             keyStatus[(int)('\r')] = 1;
             break;
        case 'r':
        case 'R':
             if(estado == VITORIA || estado == DERROTA){
                resetaJogo();
                estado = MENU;
             }
             break;
        case 'o':
        case 'O':
             if(estado == JOGANDO){
                if(keyStatus[(int)('o')] == 0){
                    keyStatus[(int)('o')] = 1;
                }
                else{
                    keyStatus[(int)('o')] = 0;
                }
             }
             break;
        case 27 :
            //Deleta todos os tiros alocados, caso o usuário aperte ESC para sair do jogo
             for (Tiro* t : tiros) {
             delete t;
             }
             tiros.clear();
             if(inimigo != NULL){
                delete inimigo;
             }
             exit(0);
    }
    glutPostRedisplay();
}

void keyup(unsigned char key, int x, int y)
{
    if(key != 'o'){
        keyStatus[(int)(key)] = 0;
    }
    glutPostRedisplay();
}

// Função para resetar o status de todas as teclas no vetor
void ResetKeyStatus()
{
    int i;
    //Initialize keyStatus
    for(i = 0; i < 256; i++)
       keyStatus[i] = 0; 
}

void init(void)
{
    srand(time(NULL)); // Garante aleatoriedade
    ResetKeyStatus();
    keyStatus[(int)('o')] = 1;

    player.mudaCor(0.35f, 0.78f, 0.32f);

    // The color the windows will redraw. Its done to erase the previous frame.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black, no opacity(alpha).
 
    glMatrixMode(GL_PROJECTION); // Select the projection matrix    
    glOrtho(-(larguraArena/2),     // X coordinate of left edge             
            (larguraArena/2),     // X coordinate of right edge            
            -(alturaArena/2),     // Y coordinate of bottom edge             
            (alturaArena/2),     // Y coordinate of top edge             
            -100,     // Z coordinate of the “near” plane            
            100);    // Z coordinate of the “far” plane
    glMatrixMode(GL_MODELVIEW); // Select the projection matrix    
    glLoadIdentity();
      
}

// Função principal onde o jogo acontece, 
// é sempre chamada enquanto não ocorre nenhum evento
void idle(void)
{
    if(estado == JOGANDO){

        static GLdouble previousTime = glutGet(GLUT_ELAPSED_TIME);
        GLdouble currentTime, timeDiference;
        //Pega o tempo que passou do inicio da aplicacao
        currentTime = glutGet(GLUT_ELAPSED_TIME);
        // Calcula o tempo decorrido desde de a ultima frame.
        timeDiference = currentTime - previousTime;
        //Atualiza o tempo do ultimo frame ocorrido
        previousTime = currentTime;

        //Treat keyPress
        if(keyStatus[(int)('s')])
        {
            player.MoveEmX(timeDiference*velocidadeJogador);
            player.MoveEmY(timeDiference*velocidadeJogador);
        }
        if(keyStatus[(int)('w')])
        {
            player.MoveEmX(-timeDiference*velocidadeJogador);
            player.MoveEmY(-timeDiference*velocidadeJogador);
        }
        if(keyStatus[(int)('a')])
        {
            player.RodaBraco1(timeDiference*velocidadeGiro);
        }
        if(keyStatus[(int)('d')])
        {
            player.RodaBraco1(-timeDiference*velocidadeGiro);
        }
        
        //Trata colisão com as paredes da janela
        //Colisão em X
        if (player.ObtemX() - raioCabecaJogador < -(larguraArena/2)){
            player.MudaX(-(larguraArena/2)+raioCabecaJogador);
        }
        else if (player.ObtemX() + raioCabecaJogador > (larguraArena/2)){
            player.MudaX((larguraArena/2)-raioCabecaJogador);
        }
        //Colisão em Y
        if (player.ObtemY() - raioCabecaJogador < -(alturaArena/2)){
            player.MudaY(-(alturaArena/2)+raioCabecaJogador);
        }
        else if (player.ObtemY() + raioCabecaJogador > (alturaArena/2)){
            player.MudaY((alturaArena/2)-raioCabecaJogador);
        }
        
        //Mira da Arma
        int deltaX = mouseX - ultimaPosMouse;
        float sensibilidade = 0.5f;

        player.RodaBraco2(-deltaX * sensibilidade);

        GLfloat theta2 = player.ObtemTheta2();
        if(theta2 > limiteGiroArma/2){
            player.MudaTheta2(limiteGiroArma/2);
        }
        else if(theta2 < -limiteGiroArma/2){
            player.MudaTheta2(-limiteGiroArma/2);
        }
        ultimaPosMouse = mouseX;

        //Trata o movimento do barril
        alvo.Move(timeDiference);
        inimigo->andaComBarril(timeDiference, velocidadeBarril);
        if(alvo.ObtemY() + alturaBarril < - (alturaArena/2)){
            float xMin = -(larguraArena / 2.0f) + (larguraBarril / 2.0f);
            float xMax = (larguraArena / 2.0f) - (larguraBarril / 2.0f);
            float xSpawn = xMin + (rand() % (int)(xMax - xMin + 1));
            delete inimigo;
            alvo.Recria(xSpawn, 250);
            alvo.SetElementos(velocidadeBarril, larguraBarril, alturaBarril, resistenciaBarril, chanceInimigo);
            if(((rand() % 100) / 100.0f) < chanceInimigo){
                alvo.SetTemInimigo(true);
            }
            inimigo = new Player(xSpawn, 250+(alturaBarril/2));
            inimigo->defineRaio(raioCabecaInimigo);
            inimigo->defineAnguloInicial(180.0f);
            inimigo->mudaCor(0.65f, 0.25f, 0.85f);
        }

        //Trata colisão do barril com a cabeça do player

        if(keyStatus[(int)('o')]){

            GLfloat playerX, playerY;
            player.GetPos(playerX, playerY);

            GLfloat barrilEsquerda = alvo.ObtemX() - (larguraBarril/2.0f);
            GLfloat barrilDireita = alvo.ObtemX() + (larguraBarril/2.0f);
            GLfloat barrilBaixo = alvo.ObtemY();
            GLfloat barrilCima = alvo.ObtemY() + alturaBarril;

            // Encontra o ponto mais próximo do centro do player dentro do retângulo do barril
            GLfloat pontoProximoX = playerX;
            if (pontoProximoX < barrilEsquerda){
                pontoProximoX = barrilEsquerda;
            }
            if (pontoProximoX > barrilDireita){
                pontoProximoX = barrilDireita;
            }

            GLfloat pontoProximoY = playerY;
            if (pontoProximoY < barrilBaixo){
                pontoProximoY = barrilBaixo;
            } 
            if (pontoProximoY > barrilCima){
                pontoProximoY = barrilCima;
            }

            // Calcula a distância estrita entre o centro do player e esse ponto mais próximo
            GLfloat distX = playerX - pontoProximoX;
            GLfloat distY = playerY - pontoProximoY;
            GLfloat distanciaQuadrada = (distX * distX) + (distY * distY);

            // Se a distância for menor que o raio do círculo ao quadrado, colidiu!
            if (distanciaQuadrada <= (raioCabecaJogador * raioCabecaJogador)) {
                estado = DERROTA;
            }
        }

        //Trata o tiro (soh permite um tiro por vez)
        //Poderia usar uma lista para tratar varios tiros
        if(!tiros.empty()){
            std::list<Tiro*>::iterator it = tiros.begin();
            while (it != tiros.end()){
                Tiro *atual = *it;
                atual->Move(timeDiference);
                if (alvo.Atingido(atual)){
                    atingido += 1;
                    if(alvo.Morreu()){
                        barrisDestruidos += 1;
                        if(barrisDestruidos == barrisParaGanhar){
                            estado = VITORIA;
                        }
                        float xMin = -(larguraArena / 2.0f) + (larguraBarril / 2.0f);
                        float xMax = (larguraArena / 2.0f) - (larguraBarril / 2.0f);
                        float xSpawn = xMin + (rand() % (int)(xMax - xMin + 1));
                        delete inimigo;
                        alvo.Recria(xSpawn, 250);
                        alvo.SetElementos(velocidadeBarril, larguraBarril, alturaBarril, resistenciaBarril, chanceInimigo);
                        if(((rand() % 100) / 100.0f) < chanceInimigo){
                            alvo.SetTemInimigo(true);
                        }
                        inimigo = new Player(xSpawn, 250+(alturaBarril/2));
                        inimigo->defineRaio(raioCabecaInimigo);
                        inimigo->defineAnguloInicial(180.0f);
                        inimigo->mudaCor(0.65f, 0.25f, 0.85f);
                    }
                    delete atual;
                    it = tiros.erase(it);
                }
                else if(!atual->Valido()){
                    delete atual;
                    it = tiros.erase(it);
                }
                else{
                    ++it;
                }
            }
        }

        //Trata da IA dos inimigos
        if(keyStatus[(int)('o')]){
            if (inimigo != NULL && alvo.TemInimigo()) {
                GLfloat pX, pY, inimigoX, inimigoY;
                player.GetPos(pX, pY);
                inimigo->GetPos(inimigoX, inimigoY);

                //Calcula o vetor direção do inimigo até o jogador
                GLfloat vetorX = pX - inimigoX;
                GLfloat vetorY = pY - inimigoY;
                GLfloat anguloAlvo = atan2f(vetorY, vetorX) * (180.0f / 3.14159f);

                //Compensa o fato do inimigo ter nascido rotacionado em 180 graus
                GLfloat anguloDesejadoArma = anguloAlvo - inimigo->ObtemTheta1() - 90.0f;

                //Mantém o ângulo dentro de intervalos limpos
                while (anguloDesejadoArma > 180.0f)  anguloDesejadoArma -= 360.0f;
                while (anguloDesejadoArma < -180.0f) anguloDesejadoArma += 360.0f;

                //Limita o giro da arma com base nas regras do XML
                if (anguloDesejadoArma > limiteGiroArma / 2) anguloDesejadoArma = limiteGiroArma / 2;
                if (anguloDesejadoArma < -limiteGiroArma / 2) anguloDesejadoArma = -limiteGiroArma / 2;

                //Aplica a mira automática instantânea
                inimigo->MudaTheta2(anguloDesejadoArma);

                //Controle de Cadência e Disparo automático
                if (tempoEntreTiros > 0.0f) {
                    tempoEntreTiros -= (timeDiference / 1000.0f);
                }

                if (tempoEntreTiros <= 0.0f) {
                    tirosInimigos.push_back(inimigo->Atira(velocidadeTiro));
                    tempoEntreTiros = 1.0f / (float)tirosPorSegundo; //reinicia temporizador
                }
            }
        }
        //Trata os tiros inimigos
        if(keyStatus[(int)('o')]){
            
            if(!tirosInimigos.empty()){
                std::list<Tiro*>::iterator it = tirosInimigos.begin();
                while (it != tirosInimigos.end()){
                    Tiro *atual = *it;
                    atual->Move(timeDiference);
                    GLfloat xPlayer, yPlayer, xTiro, yTiro;
                    atual->GetPos(xTiro, yTiro);
                    player.GetPos(xPlayer, yPlayer);
                    float distanciaEntreCentros = sqrtf(powf(xPlayer - xTiro, 2) + powf(yPlayer - yTiro, 2));

                    if(distanciaEntreCentros < raioTiro+raioCabecaJogador){
                        estado = DERROTA;
                        //return;
                    }
                    ++it;
                }
            }
        }
    }
    if(estado == MENU){
        if(keyStatus[(int)('\r')])
        {
            if(opcaoMenu == 0){
                estado = JOGANDO;
            }
            else{
                //Deleta todos os tiros alocados
                for (Tiro* t : tiros) {
                    delete t;
                }
                tiros.clear();
                if(inimigo != NULL){
                    delete inimigo;
                }
                exit(0);
            }
        }
    }
    glutPostRedisplay();
}

void movimentoMouse(int x, int y){

    if(ultimaPosMouse == -1){
        ultimaPosMouse = x;
        return;
    }

    mouseX = x;
}

void mouseClick(int button, int state, int x, int y){
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        tiros.push_back(player.Atira(velocidadeTiro));
    }
}
 
int main(int argc, char *argv[])
{

    carregarConfiguracoesXML();
    // Initialize openGL with Double buffer and RGB color without transparency.
    // Its interesting to try GLUT_SINGLE instead of GLUT_DOUBLE.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
 
    // Create the window.
    glutInitWindowSize(larguraArena, alturaArena);
    glutInitWindowPosition(150,50);
    glutCreateWindow("Jogo do Barril");
 
    // Define callbacks.
    glutDisplayFunc(renderScene);
    glutKeyboardFunc(keyPress);
    glutIdleFunc(idle);
    glutKeyboardUpFunc(keyup);
    glutPassiveMotionFunc(movimentoMouse);
    glutMouseFunc(mouseClick);
    
    init();
 
    glutMainLoop();
 
    return 0;
}
