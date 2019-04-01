
/**Variables y librerias para la lectura del g-code**/
#include <SD.h>
const char* codop[50]; //tamaño preasignado del vector de operaciones 
float xpos[50];
float ypos[50];
/**Variables y librerias para el control de motores**/
#include <Servo.h>
Servo myservo;
#include <AccelStepper.h>
AccelStepper Xax(1, 6, 7);
AccelStepper Yax(1, 10, 8);

int serv=1;  
int s_speed=200; //este rango de velocidades va bien(comprobar con carga)
int acl=600;
int numop=0; //contador de operacion
void setup() {
  delay(5000);
  Serial.begin(9600);
  SD.begin(4);
  File gcode = SD.open("cuad.ngc");
  delay(1000);
  if(!gcode){Serial.println("NOPE");}
  char curnt;
  char* code;
  int cnt =0;

  
  
  while(gcode.available()){
    curnt=gcode.read();
    if(curnt=='G'){ //empieza una nueva linea de gcode
      gcode.read(); //lee el primer numero de cod op--descartado
     char cod0=gcode.read();
      if(cod0=='0'){code="G00";}
      else{code="G01";}//"transforma" los G2 G3 en G1
      gcode.read(); //leer el espacio
      curnt=gcode.read(); //lee la X o la Z o el espacio 
      if(curnt=='X'){ //el caso de la linea que vale 
          String charx;
          String chary;
          curnt=gcode.read();
          while(curnt!=' '){
          charx+=curnt;
          curnt=gcode.read();} //ya ha guardado toda la X
          gcode.read(); //lee la Y
          curnt=gcode.read(); //lee el primer numero de y
          while(curnt!=' '){
          chary+=curnt;
          curnt=gcode.read();} //ya ha guardado toda la Y
          codop[cnt]=code;
          xpos[cnt]=charx.toFloat();
          ypos[cnt]=chary.toFloat();
          cnt++;}}} //numero de operaciones(-1)
        gcode.close();

    
for(int i=0; i<cnt; i++){
     
   Serial.print(codop[i]);
   Serial.print(" ");
   Serial.print(xpos[i]);
   Serial.print(" ");
   Serial.print(ypos[i]);
   Serial.print("\n");
   }

myservo.attach(9);
Xax.setMaxSpeed(5000);
Yax.setMaxSpeed(5000);

while(numop<cnt){
  if(codop[numop]=="G00"){
    G00(map(xpos[numop],0,39,0,250), map(ypos[numop],0,39,0,250));}
  else if (codop[numop]=="G01"){
    G01(map(xpos[numop],0,39,0,250), map(ypos[numop],0,39,0,250));}
  numop++;
  }
}




int up(){ //sube el puntero si no lo esta ya
  if (serv==1){return serv;} 
  myservo.write(180);
  delay(170);
  myservo.write(90);
  serv=1;
  delay(1000);
  return(serv);
}

int down(){
  if(serv==0){return serv;}
  myservo.write(0);
  delay(125);
  myservo.write(90);
  serv=0;
  delay(1000);
  return(serv);
  }

  

void G00(int corx, int cory){ //desplazamiento libre
  serv=up();
  Xax.setSpeed(s_speed);//ajustar la velocidad 
  Yax.setSpeed(s_speed);
  Xax.setAcceleration(acl);
  Yax.setAcceleration(acl);
  Xax.moveTo(corx);
  Yax.moveTo(cory);
  while(Xax.distanceToGo()!=0 || Yax.distanceToGo()!=0){
  if(Xax.distanceToGo()!=0){Xax.run();}
  if(Yax.distanceToGo()!=0){Yax.run();}
  }
}
  void G01(int corx, int cory){
    serv=down(); 
    float spdf=abs((ypos[numop]-ypos[numop-1])/(xpos[numop]-xpos[numop-1]));
    if (spdf>1){ // eje y tiene que ir mas rapido
      Xax.setAcceleration((int)(acl/spdf));
      Yax.setAcceleration(acl);
      Xax.setSpeed((int)(s_speed/spdf));
      Yax.setSpeed(s_speed);
      }
    else{
      Xax.setAcceleration(acl);
      Yax.setAcceleration(acl*spdf);
      Xax.setSpeed(s_speed); // ejeX tiene que ir mas rapido
      Yax.setSpeed((int)s_speed*spdf);
      }
  Xax.moveTo(corx);
  Yax.moveTo(cory);
  
  while(Xax.distanceToGo()!=0 || Yax.distanceToGo()!=0){
  if(Xax.distanceToGo()!=0){Xax.run();}
  if(Yax.distanceToGo()!=0){Yax.run();}
  }}

  void loop(){}
