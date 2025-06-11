#include <Arduino.h>

#define WCK_HEADER_BYTE 0xff
#define WCK_TIMEOUT 500
#define ROTATE_CCW 3
#define ROTATE_CW 4

#define WCK_921600_BAUD_RATE 0
#define WCK_460800_BAUD_RATE 1
#define WCK_230400_BAUD_RATE 3
#define WCK_115200_BAUD_RATE 7
#define WCK_57600_BAUD_RATE 15
#define WCK_38400_BAUD_RATE 23 
#define WCK_9600_BAUD_RATE 95
#define WCK_4800_BAUD_RATE 191 

#define WCK_CURRENT_LIMIT_400MA 33
#define WCK_CURRENT_LIMIT_500MA 44
#define WCK_CURRENT_LIMIT_600MA 56
#define WCK_CURRENT_LIMIT_700MA 68
#define WCK_CURRENT_LIMIT_800MA 80
#define WCK_CURRENT_LIMIT_900MA 92
#define WCK_CURRENT_LIMIT_1000MA 104
#define WCK_CURRENT_LIMIT_1100MA 116
#define WCK_CURRENT_LIMIT_1200MA 128
#define WCK_CURRENT_LIMIT_1800MA 199


typedef struct{
    int load;
    int position;
} Response_packet;

class wCK {
public:
    wCK(HardwareSerial *SerialPort);

    void begin(unsigned int BaudRate);

    /***********************ACTIONS***********************/
    Response_packet PosSend(char ServoID, char SpeedLevel, char Position);   
    bool PosSendH(char ServoID, char SpeedLevel, char Position);   
    void SyncPosSend(char LastID, char SpeedLevel, char *TargetArray, char Index);
    
    Response_packet Rotation360(char ServoID, char SpeedLevel, char RotationDir);

    char PassiveCK(char ServoID);
    
    char BreakWCK(void);
    /***********************ACTIONS***********************/
    
    
    /***********************SETTERS***********************/
    bool setBaudrate(char ServoID, char NewBaud);
    bool setPDGain(char ServoID, char *NewPgain, char *NewDgain);
    bool setIGain(char ServoID, char *NewIgain);
    bool setRuntimePDGain(char ServoID, char *NewPgain, char *NewDgain);
    bool setRuntimeIGain(char ServoID, char *NewIgain);
    bool setId(char ServoID, char NewId);
    bool setSpeed(char ServoID, char NewSpeed, char NewAccel);
    void setRuntimeSpeed(char ServoID, char NewSpeed, char NewAccel);
    bool setOverLoad(char ServoID, char NewOverLoad);
    bool setBoundary(char ServoID, char *NewLBound, char *NewUBound);
    /***********************SETTERS***********************/
    
    /***********************GETTERS***********************/
    Response_packet getPos(char ServoID);
    char getPosH(char ServoID);
    char getPDGain(char ServoID, char *Pgain, char *Dgain);
    char getIGain(char ServoID, char *Igain);
    char getSpeed(char ServoID, char* NewSpeed, char *NewAccel);
    char getOverCT(char ServoID);
    char getBound(char ServoID, char *LBound, char *UBound);
    /***********************GETTERS***********************/

private:
    HardwareSerial* wck_Ser;

    void SendOperCommand(char Data1, char Data2);
    void SendOper10BitCommand(char Data1, char Data2, char Data3 = 0, char Data4 = 0, char Data5 = 0, char Data6 = 0);
    void SendSetCommand(char Data1, char Data2, char Data3, char Data4);
};
