#include <wCK_Series.h>

wCK myServo(&Serial);// Create an instance of the wCK class with Serial communication

void setup() {
  myServo.begin(115200);// Initialize the servo communication at 115200 baud rate
}

void loop() {
    myServo.PosSendH(7, 0, 0);//id:7, torque level: 0, position: 0
    delay(2000);
    myServo.PosSendH(7, 4, 1023);// id:7, torque level: 4, position: 254
    delay(2500);
    myServo.PosSendH(7, 02, 500);//id:7, torque level: 0, position: 0
    delay(2000);
}