#ifndef WCK_SERIES_H
#define WCK_SERIES_H

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

    /**
     * @brief Constructor for the wCK class.
     * @param SerialPort Pointer to the HardwareSerial object for communication.
     */
    wCK(HardwareSerial *SerialPort);

    /**
     * @brief Initializes the wCK Serial.
     * @param BaudRate The baud rate for communication.
     */
    void begin(unsigned int BaudRate, uint8_t config = Null, uint8_t rx = -1, uint8_t tx = -1);

    /***********************ACTIONS***********************/

    /**
     * @brief Sends a position command to the servo.
     * @param ServoID The ID of the servo (value: 0-30).
     * @param TorqueLevel The torque level for the servo (value:0-4 (0(max), 4(minimum))).
     * @param Position The target position for the servo (value: 0-253).
     * 
     * @return Response_packet containing the load and position after the command is sent.
     */
    Response_packet PosSend(char ServoID, char TorqueLevel, char Position);

    /**
     * @brief Sends a position command to the servo with high precision.
     * @param ServoID The ID of the servo (value: 0-253).
     * @param TorqueLevel The torque level for the servo (value: 0-254).
     * @param Position The target position for the servo (value: 0-1023).
     * 
     * @return true if the command was sent successfully, false otherwise.
     */
    bool PosSendH(char ServoID, char TorqueLevel, int Position);

    /**
     * @brief move multiple wCK modules to a specified target position from id:0 until the wanted last id.
     * @param LastID The last ID of the last servo that will be moved from the sequence (value: 0-30).
     * @param TorqueLevel The torque level for the servo (value: 0-4)
     * @param TargetArray An array of target positions for each servo (size: LastID + 1).
     */
    void SyncPosSend(char LastID, char TorqueLevel, char *TargetArray);
    
    /**
     * @brief Rotates the servo (wheel mode) in a specified direction with a specified torque level.
     * @param ServoID The ID of the servo (value: 0-30).
     * @param SpeedLevel The speed the servo rotates (value: 0-15).
     * @param RotationDir The direction of rotation (value: ROTATE_CCW or ROTATE_CW).
     * 
     * @return Response_packet containing the load and position after the command is sent.
     */
    Response_packet Rotation360(char ServoID, char SpeedLevel, char RotationDir);

    /**
     * @brief Sends a command to the servo to move freely without position control.
     * @param ServoID The ID of the servo (value: 0-30).
     * 
     * @return char The position of the servo after sending the command. 
     */
    char PassiveCK(char ServoID);
    
    /**
     * @brief  command make all wCK modules connected on one channel to turn into Break Mode. Break Mode stops the motion of
     * a wCK module and thus help hold still a system’s mechanical structure without consuming electric power by using dynamic break. In
     * this mode, the rotation axis become stiff and hard to move with a hand.
     */
    char BreakWCK(void);
    /***********************ACTIONS***********************/
    
    
    /***********************SETTERS***********************/
    /**
     * @brief Sets the baud rate for the servo.
     * @param ServoID The ID of the servo (value: 0-30).
     * @param NewBaud The new baud rate to set (value: 0-7).
     * 
     * @return true if the baud rate was set successfully, false otherwise.
     */
    bool setBaudrate(char ServoID, char NewBaud);
    
    /**
     * @brief Sets the position of the servo.
     * @param ServoID The ID of the servo (value: 0-30).
     * @param NewPgain The new proportional gain (P) to set (value: 1-254).
     * @param NewDgain The new derivative gain (D) to set (value: 0-254).
     * 
     * @return true if the position was set successfully, false otherwise.
     */
    bool setPDGain(char ServoID, char *NewPgain, char *NewDgain);
    
    /**
     * @brief Sets the integral gain of the servo.
     * @param ServoID The ID of the servo (value: 0-30).
     * @param NewIgain The new integral gain to set (value: 0-10).
     * 
     * @return true if the integral gain was set successfully, false otherwise.
     */
    bool setIGain(char ServoID, char *NewIgain);
    
    /**
     * @brief Sets the ID of the servo.
     * @param ServoID The ID of the servo to change (value: 0-30).
     * @param NewPgain The new ID to set (value: 0-30).
     * 
     * @return true if the ID was set successfully, false otherwise.
     */
    bool setRuntimePDGain(char ServoID, char *NewPgain, char *NewDgain);
    
    /**
     * @brief Sets the integral gain of the servo at runtime.
     * @param ServoID The ID of the servo (value: 0-30).
     * @param NewIgain The new integral gain to set (value: 0-10).
     * 
     * @return true if the integral gain was set successfully, false otherwise.
     */
    bool setRuntimeIGain(char ServoID, char *NewIgain);
    
    /**
     * @brief Sets the ID of the servo.
     * @param ServoID The ID of the servo to change (value: 0-30).
     * @param NewId The new ID to set (value: 0-30).
     * 
     * @return true if the ID was set successfully, false otherwise.
     */
    bool setId(char ServoID, char NewId);
    
    /**
     * @brief Sets the speed and acceleration of the servo
     * @param ServoID The ID of the servo (value: 0-30).
     * @param NewSpeed The new speed to set (value: 0-30).
     * @param NewAccel The new acceleration to set (value: 20-100).
     * 
     * @return true if the speed and acceleration were set successfully, false otherwise.
     */
    bool setSpeed(char ServoID, char NewSpeed, char NewAccel);
    
    /**
     * @brief sets the runtime speed and acceleration range of a wCK module. 
     * @param ServoID The ID of the servo (value: 0-30).
     * @param NewSpeed The new speed to set (value: 0-30).
     * @param NewAccel The new acceleration to set (value: 20-100).     
     * 
     * @return true if the speed and acceleration were set successfully, false otherwise.
     */
    void setRuntimeSpeed(char ServoID, char NewSpeed, char NewAccel);
    
    /**
     * @brief Sets the overload current limit for the servo.
     * @param ServoID The ID of the servo (value: 0-30).
     * @param NewOverLoad The new overload current limit to set (value: 33-199).
     * 
     * @return true if the overload current limit was set successfully, false otherwise.
     */
    bool setOverLoad(char ServoID, char NewOverLoad);
    
    /**
     * @brief Sets the boundary limits for the servo.
     * @param ServoID The ID of the servo (value: 0-30).
     * @param NewLBound The new lower boundary limit to set (value: 0-254).
     * @param NewUBound The new upper boundary limit to set (value: 0-254).
     * 
     * @return true if the boundaries were set successfully, false otherwise.
     */
    bool setBoundary(char ServoID, char *NewLBound, char *NewUBound);
    /***********************SETTERS***********************/
    
    /***********************GETTERS***********************/
    /**
     * @brief Gets the position and load of the servo.
     * @param ServoID The ID of the servo (value: 0-30).
     * 
     * @return Response_packet containing the load and position of the servo.
     */
    Response_packet getPos(char ServoID);
    
    /**
     * @brief Gets the position with high precision.
     * @param ServoID The ID of the servo (value: 0-253).
     * 
     * @return value: 0-1023.
     */
    int getPosH(char ServoID);

    /**
     * @brief Gets the P and D gains of the servo.
     * @param ServoID The ID of the servo (value: 0-30).
     * @param Pgain Pointer to store the proportional gain (P) (value: 1-50).
     * @param Dgain Pointer to store the derivative gain (D) (value: 0-50).
     * 
     * @return 1 if gains are valid, 0 if invalid, -1 on error.
     */
    char getPDGain(char ServoID, char *Pgain, char *Dgain);

    /**
     * @brief Gets the integral gain of the servo.
     * @param ServoID The ID of the servo (value: 0-30).
     * @param Igain Pointer to store the integral gain (I) (value: 0-10).
     * 
     * @return 1 if gain is valid, 0 if invalid, -1 on error.
     */
    char getIGain(char ServoID, char *Igain);
    
    /**
     * @brief Gets the speed and acceleration of the servo.
     * @param ServoID The ID of the servo (value: 0-30).
     * @param NewSpeed Pointer to store the new speed (value: 0-30).
     * @param NewAccel Pointer to store the new acceleration (value: 20-100).
     * 
     * @return 1 if speed and acceleration are valid, 0 if invalid, -1 on error.
     */
    char getSpeed(char ServoID, char* NewSpeed, char *NewAccel);
    
    /**
     * @brief Gets the current overload limit of the servo.
     * @param ServoID The ID of the servo (value: 0-30).
     * 
     * @return value: 33-199.
     */
    char getOverLD(char ServoID);

    /**
     * @brief Gets the current boundary of the servo.
     * @param ServoID The ID of the servo (value: 0-30).
     * @param LBound Pointer to store the lower boundary limit (value: 0-254).
     * @param UBound Pointer to store the upper boundary limit (value: 0-254).
     * 
     * @return value: 0-7 representing different baud rates.
     */
    char getBound(char ServoID, char *LBound, char *UBound);
    /***********************GETTERS***********************/

private:
    HardwareSerial* wck_Ser;

    uint8_t wck_Config = NULL;
    uint8_t wck_RX = -1;
    uint8_t wck_TX = -1;

    void SendOperCommand(char Data1, char Data2);
    void SendOper10BitCommand(char Data1, char Data2, char Data3 = 0, char Data4 = 0, char Data5 = 0, char Data6 = 0);
    void SendSetCommand(char Data1, char Data2, char Data3, char Data4);
};
#endif