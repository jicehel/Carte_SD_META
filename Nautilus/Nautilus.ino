// ###########################################################################
//    Include
// ###########################################################################  

#include <Gamebuino-Meta.h>

// ###########################################################################
//    Déclaration des variables
// ###########################################################################  
extern Image screentitle;
extern Image back;
extern Image screenpause;
extern Image matelot;

/////////////////////

byte lives = 3 ;
short score = 0;

//déplacement du matelot (accélération)
byte acc = 0;
//son orientation
bool orient = true;
bool walk = false;

//----------------
// Définie les types d'obstacles (voir exemple physics "define de Box structure for obstacles")
//----------------
typedef struct {
  byte w;
  byte x;
  byte h;
  byte y;
} Box;

/////////////////////

//define de MovingBox structure for Matelot
typedef struct {
  byte w;
  float x;
  float xv;
  byte h;
  float y;
  float yv;
} MovingBox;

/////////////////////

// Divers variables
byte mode; // mode de jeu, change l'aléatoire pour plus de variété dans les parties 
int pause = 75; //temps de pause pour nouvelle vie
byte gamestate = 1; //0 running, 1 mainmenu, 2 gameover menu, 3 nvle vie, 4 pause
boolean presstart = false; //a appuyé sur A ?
int framecount = 0; // ça me sert à compter les frame dans plusieurs trucs
int timerinit = 0; //pareil, je compte des trucs avec

#define NUM_BORDERS 4           //4 "boîtes" fixes qui symbolisent les bords de la zone de jeu
Box borders[NUM_BORDERS];       //

//--------------------------
// Concerne les obstacles
//--------------------------
#define NUM_OBSTACLES 10 //define a constant named "NUM_OBSTACLES" equal to 8
Box obstacles[NUM_OBSTACLES]; //create the obstacles array

// ###########################################################################
//    Génération initiale des bordures
// ###########################################################################   
void initBorders(){
  //haut 
  borders[0].w = 80;
  borders[0].h = 1;
  borders[0].x = 0;
  borders[0].y = 0;
  //bas
  borders[1].w = 80;
  borders[1].h = 2;
  borders[1].x = 0;
  borders[1].y = 62;
  //gauche
  borders[2].w = 2;
  borders[2].h = 64;
  borders[2].x = 0;
  borders[2].y = 0;
  //droite
  borders[3].w = 2;
  borders[3].h = 64;
  borders[3].x = 78;
  borders[3].y = 0; 
}

// ###########################################################################
//    Création des obstacles
// ###########################################################################  
void initObstacles(){
  //randomize the other obstacles
  for(byte i=4; i<NUM_OBSTACLES; i++){
    obstacles[i].w = random(5,16);
    obstacles[i].h = 2;
    obstacles[i].x = random(3, gb.display.width() - obstacles[i].w);
    obstacles[i].y = random(15, gb.display.height() - 15);
  }
}

void updateObstacles(){
  
}

void drawObstacles(){
  for(byte i=0; i<NUM_OBSTACLES; i++){
    gb.display.setColor(BROWN);
    gb.display.drawRect(obstacles[i].x, obstacles[i].y, obstacles[i].w, obstacles[i].h);
  }
}

// ###########################################################################
//    Création du player (le matelot)
// ###########################################################################


MovingBox Matelot;

void initMatelot(){
  Matelot.w = 6;
  Matelot.h = 11;
  while(MatelotCollision()){ //pick a random player position until there is no collision
    Matelot.x = random(0, gb.display.width() - Matelot.w);
    Matelot.y = random(0, gb.display.height() - Matelot.h);
  }
}

void updateMatelot(){
/*  ///// crouch
  if(gb.buttons.pressed(BUTTON_DOWN)){
    Matelot.h = 8;
    Matelot.y += 4;
  }
  if(! gb.buttons.timeHeld(BUTTON_DOWN)){ //if BUTTON_B is not held down
    if(Matelot.h ==4){ //and the player is crouched
      //stand up
      Matelot.h = 8;
      Matelot.y -= 4; //move the player uppward to avoid being stuck in the floor
      if(MatelotCollision()){ //if it creates a collision to stand up, crouch again
        Matelot.h = 4;
        Matelot.y += 4;
      }
    }
  }
*/  
  ///// horizontal movement
  Matelot.xv *= 0.9; //friction
 if (gb.buttons.repeat(BUTTON_LEFT, 0)) {
  orient = false;
  if(Matelot.x > 0) {
    acc++;
    if(acc > 5) {
        Matelot.x -= 2;
      }else{
        Matelot.x--;
   }
 }
}

if (gb.buttons.repeat(BUTTON_RIGHT, 0)) {
  orient = true;
  if(Matelot.x < gb.display.width()-8) {
    acc++;
    if (acc > 5) {
        Matelot.x += 2;
      }else{
        Matelot.x++;
   }
 }
}

Matelot.x += Matelot.xv; //move forward of 1 step
  //check for collisions on x axis
  if(MatelotCollision()){
    Matelot.x -= Matelot.xv; //move back of 1 step
    Matelot.xv *= 0; //change the velocity direction
  }
  
/*
//--------------
//test du saut
//--------------
 int getWidth() {
      return 48;
    }
    int getHeight() {
      return 72;
    }
if (gb.buttons.pressed(BUTTON_B)) {
          if (f(Matelot.x, Matelot.y + 1, getWidth(), getHeight())) {
            vy = -32;
            jumping = true;
            //gb.sound.playPattern(jump_sound, 1);
            gb.sound.playTick();
          }
          else {
            if (doubleJumped == false) {
              vy = -32;
              doubleJumped = true;
              jumping = true;
              //gb.sound.playPattern(jump_sound, 1);
              gb.sound.playTick();
            }
          }
        }
        if ((gb.buttons.timeHeld(BUTTON_B) > 0) && (gb.buttons.timeHeld(BUTTON_B) < 5) && (vy < 0) && jumping) {
          if (doubleJumped) {
            vy -= 6;
          }
          else {
            vy -= 12;
          }
        }
        if (vy > 0) {
          jumping = false;
        }

*/
//---------------
// saut
//---------------  
  ///// vertical movement
  Matelot.yv += 0.3; //gravity
  Matelot.yv *= 0.95; //friction
  if(gb.buttons.timeHeld(BUTTON_A) > 0 & gb.buttons.timeHeld(BUTTON_A) < 5 ){ //higher jump by holding A down
    Matelot.yv -= 1; //jump
  }
  if(gb.buttons.pressed(BUTTON_A)){
    gb.sound.playOK();
  }


//---------------
// Collision du player sur un obstacle
//---------------
 
  Matelot.y += Matelot.yv; //move forward of 1 step
  //check for collisions on y axis
  if(MatelotCollision()){
    Matelot.y -= Matelot.yv; //move back of 1 step
    Matelot.yv *= 0; //change the velocity direction si on fait -0.5 on fait un rebond à l'arrivée du saut. 0 = pas de rebond
  }
}

void drawMatelot(){
   gb.display.drawImage(Matelot.x, Matelot.y, matelot, (orient == 0)?(matelot.width()*-1):matelot.width(), matelot.height());
}

boolean MatelotCollision(){
  for(byte i=0; i<NUM_BORDERS; i++){
    if(gb.collideRectRect((int)Matelot.x, (int)Matelot.y, Matelot.w, Matelot.h, borders[1].x, borders[1].y, borders[1].w, borders[1].h)){
      return true;
    }
  }
  for(byte i=0; i<NUM_OBSTACLES; i++){
    if(gb.collideRectRect((int)Matelot.x, (int)Matelot.y, Matelot.w, Matelot.h, obstacles[i].x, obstacles[i].y, obstacles[i].w, obstacles[i].h)){
      return true;
    }
  }
  return false;

}


// ###########################################################################
//    Setup et Loop
// ###########################################################################  
void setup() {
  // put your setup code here, to run once:
  gb.begin();
}

/////////////////////

void loop() {
//gestion de la console avec la fonction while
  while(!gb.update());

  gb.display.clear();
  gb.lights.clear();

  switch (gamestate) {
    case 0: //jeu en cours
      gb.display.drawImage(0, 0, back);
      initBorders();
      initMatelot();
      updateObstacles();
      drawObstacles();
      updateMatelot();
      drawMatelot();
      if (gb.buttons.pressed(BUTTON_B)) {
         initObstacles();
      }
      if (gb.buttons.pressed(BUTTON_MENU)) {
         gamestate = 3; //->pause
      }
      break;
    case 1: //menu principal
      gb.display.drawImage(0, 0, screentitle);
      if (gb.buttons.pressed(BUTTON_A)) {
        gamestate = 0; // Jeu en cours
      }

      break;
    case 2: //menu gameover
      break;
    case 3: //pause
    presstart = false;
    gb.display.drawImage(0, 0, back);
    gb.display.drawImage(0, 0, screenpause);
    drawMatelot();
    if (gb.buttons.pressed(BUTTON_A)||gb.buttons.pressed(BUTTON_MENU)) {
    gamestate = 0;
    }
    if (gb.buttons.pressed(BUTTON_A)&&gb.buttons.pressed(BUTTON_DOWN)) {
    gamestate = 1;
    }
      break;
  }
}
