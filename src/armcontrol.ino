#include <Servo.h>

const int dataPin = 12;

Servo sr; // Servo Rotate
Servo sh; // Servo Height
Servo sp; // Servo Plane
Servo sg; // Servo Grab

int timer = 0;
int cr = 65;
int ch = 150;
int cp = 40;
int cg = 40;

void setup() {
  pinMode(dataPin, INPUT);

  sr.attach(11);
  sh.attach(9);
  sp.attach(10);
  sg.attach(8);
  
  sr.write(180); // Servo Rotate -> 0-180
  sh.write(150); // Servo Height -> 100-155
  sp.write(80); // Servo Plane -> 20-100
  sg.write(40); // Servo Grab -> 0-40
}

void LinearMovement(int nr, int nh, int np, int ng, int jumps, int timer) {
  int diffr = nr - cr;
  int diffh = nh - ch;
  int diffp = np - cp;
  int diffg = ng - cg;

  for (int i = 0; i < jumps; i += 1) {
    sr.write(cr + (diffr*i/jumps));
    sh.write(ch + (diffh*i/jumps));
    sp.write(cp + (diffp*i/jumps));
    sg.write(cg + (diffg*i/jumps));
    delay(timer/jumps);
  }
  sr.write(nr);
  sh.write(nh);
  sp.write(np);
  sg.write(ng);
  cr = nr;
  ch = nh;
  cp = np;
  cg = ng;
}

// Servo Rotate ->  0-180     (right-left)
// Servo Height ->  100-155   (low-high)
// Servo Plane ->   20-100    (close-far)
// Servo Grab ->    0-40      (open-close)

// int cr = 180; ch = 100; cp = 80;cg = 0;

int prevx = 0;
int dataState = 0;
int whatplant = 0;
int newx = 0;

void loop() {
  dataState = digitalRead(dataPin);
  if (dataState == HIGH) {
    whatplant += 1;
    delay(600);
  } else if (whatplant != 0 && dataState == LOW) {
      newx = (whatplant-1)*90;
      LinearMovement(prevx, 150, 40, 40, 30, 500);
      LinearMovement(newx, 150, 40, 40, 30, 1000);
      LinearMovement(newx, 160, 90, 40, 30, 500);
      prevx = newx;
      whatplant = 0;
  } else {delay(1);}
}
