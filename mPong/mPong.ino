#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include <MeMCore.h>
	

MeIR ir;
MeLightSensor lightsensor_6(6);
MeRGBLed rgbled_7(7, 7==7?2:4);
MeLEDMatrix ledMtx_4(4);
MeLineFollower linefollower_2(2);
unsigned char drawBuffer[16];
unsigned char *drawTemp;
int player1_in,player2_in,player1_pos,player2_pos,ball_x,ball_y;
int ball_xdir,ball_ydir;
long pInTick,pGTick,pRTick;
void inputTaking();
void updateLoop();
void render();
void playerCollision(unsigned char);




void _delay(float seconds){
    long endTime = millis() + seconds * 1000;
    while(millis() < endTime)_loop();
}

void inputTaking(){
    ir.loop();
    if(pInTick+150 < millis()){//input taking speed
      if(player1_in==0)player1_in=linefollower_2.readSensors();
      if(player2_in==0){
        player2_in= ir.getCode();
        if(player2_in==64){
          player2_in=2;
        }else if(player2_in==25){
          player2_in=1;
        }else{
          player2_in=0;
        }    
      }
      pInTick = millis();
    }
}

void updateLoop(){
    
    if(pGTick+150 < millis()){//game speed
      if(ball_x==0||ball_x==13){//end of game
        rgbled_7.setColor(2,(ball_x>3?255:0),(ball_x>3?0:255),0);        
        rgbled_7.setColor(1,(ball_x>3?0:255),(ball_x>3?255:0),0);
        rgbled_7.show();
        init(ball_x>3?-1:1);
        delay(3000);
        rgbled_7.setColor(0,0,0,0);
        rgbled_7.show();
      }else{
        //player1 move
        if(player1_in==1&&player1_pos>0){
          player1_pos--;
        }else if(player1_in==2&&player1_pos<6){
          player1_pos++;
        }
        //player2 move
        if(player2_in==1&&player2_pos>0){
          player2_pos--;
        }else if(player2_in==2&&player2_pos<6){
          player2_pos++;
        }
        //ball collision top/bottom
        if((ball_y==0 && ball_ydir<0)||(ball_y==6 && ball_ydir>0)){
          ball_ydir *= -1;
        }
        //player colision
        playerColision(player1_pos,-1);
        playerColision(player2_pos,1);
        //colide(); //uncomment this line and coment playerColision ones to set demo mode
        //ball collision top/bottom if not here you can go out of bounds(in case you colided with wall and player in a corner)
        if((ball_y==0 && ball_ydir<0)||(ball_y==6 && ball_ydir>0)){
          ball_ydir *= -1;
        }
        //ball move
        ball_x += ball_xdir;
        ball_y += ball_ydir;
        player1_in = 0;
        player2_in = 0;
      }
      pGTick = millis();
    }
}

void colide(){
  if(((ball_x*2)-13)==(ball_xdir*11))ball_xdir*=-1;
  
}

void render(){
  
      drawTemp = new unsigned char[16]{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
      drawTemp[0]=3<<player1_pos;//print players
      drawTemp[14]=3<<player2_pos;
      drawTemp[ball_x]|=3<<ball_y;//print ball
      drawTemp[ball_x + 1]|=3<<ball_y;
      memcpy(drawBuffer,drawTemp,16);
      free(drawTemp);
      ledMtx_4.drawBitmap(0,0,16,drawBuffer);
}

void playerColision(int player_pos,int side){
    int ball_hit=ball_y-player_pos;
    if(((ball_x*2)-13)==(ball_xdir*11) && abs(ball_hit)<3 && (side==ball_xdir)){
      if(abs(ball_hit)==0){
        ball_xdir *= -1;
      }else if(abs(ball_hit)==1){
        ball_ydir = (abs(ball_ydir + ball_hit ) & 1) * ball_hit;
        ball_xdir*=-1;
      }else if((ball_hit>0 && ball_ydir < 0) ||(ball_hit<0 && ball_ydir > 0)){
        ball_ydir*=-1;
        ball_xdir*=-1;
      }
    }
}

void _loop(){
    ir.loop();
}

void setup(){
    ir.begin();
    ledMtx_4.setColorIndex(1);
    ledMtx_4.setBrightness(6); 
    drawTemp = new unsigned char[16]{        };   
    randomSeed(lightsensor_6.read());
    
    player1_pos = 4;
    player2_pos  = 4;
    init(random(2)*2-1);
}

void init(int dir){
    player1_in=0;
    player2_in=0;
    ball_x = dir==1?6:7;
    ball_y = random(3,6);
    ball_xdir = dir;
    ball_ydir = ball_y>4?1:-1;
    pGTick = millis();
    pInTick = millis();
    render();
  
}

void loop(){
    
    inputTaking();
    updateLoop();
    render();
}
