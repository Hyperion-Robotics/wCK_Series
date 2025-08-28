#include "wCK_Series.h"

Response_packet time_out_response = {
    .load = -1,
    .position = -1    
};


wCK::wCK(HardwareSerial *SerialPort){
    wck_Ser = SerialPort; // Initialize the serial port
}

void wCK::begin(unsigned int BaudRate, uint8_t wck_config = Null, uint8_t wck_rx = -1, uint8_t wck_tx = -1){
    this->wck_config = wck_config;
    this->wck_RX = wck_rx;
    this->wck_TX = wck_tx;

    if(wck_config == Null && (wck_RX == -1 || wck_TX == -1)){
        wck_Ser->begin(BaudRate);
    }else if(wck_config == Null){
        wck_Ser->begin(BaudRate, wck_config); // Start the serial communication with the specified baud rate
    }else{
        wck_Ser->begin(BaudRate, wck_config, wck_RX, wck_TX);
    }
}   

void wCK::SendOperCommand(char Data1, char Data2) {
    uint8_t command_packet[4]; // This aligns with the "Command Packet" structure in the image.

    command_packet[0] = WCK_HEADER_BYTE;                                    // Header byte
    command_packet[1] = Data1;                                              // Data1
    command_packet[2] = Data2;                                              // Data2
    command_packet[3] = (command_packet[1] ^ command_packet[2]) & 0x7F;     // Checksum

    wck_Ser->write(command_packet, sizeof(command_packet)); // Send the command packet
}


void wCK::SendSetCommand(char Data1, char Data2, char Data3, char Data4){
    uint8_t command_packet[6]; // This aligns with the "Command Packet" structure in the image.

    command_packet[0] = WCK_HEADER_BYTE;                            // Header byte
    command_packet[1] = Data1;                                      // Data1
    command_packet[2] = Data2;                                      // Data2
    command_packet[3] = Data3;                                      // Data3
    command_packet[4] = Data4;                                      // Data4
    command_packet[5] = (Data1 ^ Data2 ^ Data3 ^ Data4) & 0x7F;     // Checksum

    wck_Ser->write(command_packet, sizeof(command_packet)); // Send the command packet
}

void wCK::SendOper10BitCommand(char Data1, char Data2, char Data3, char Data4, char Data5, char Data6){
    uint8_t command_packet[8]; // This aligns with the "Command Packet" structure in the image.

    command_packet[0] = WCK_HEADER_BYTE;                            // Header byte
    command_packet[1] = Data1;                                      // Data1
    command_packet[2] = Data2;                                      // Data2
    command_packet[3] = Data3;                                      // Data3
    command_packet[4] = Data4;                                      // Data4
    command_packet[5] = Data5;                                      // Data5
    command_packet[6] = Data6;                                      // Data6
    command_packet[7] = (Data1 ^ Data2 ^ Data3 ^ Data4 ^ Data5 ^ Data6) & 0x7F; // Checksum

    wck_Ser->write(command_packet, sizeof(command_packet)); // Send the command packet
}
/************************************************ACTIONS************************************************/
Response_packet wCK::PosSend(char ServoID, char TorqueLevel, char Position){
    uint8_t data1 = (TorqueLevel << 5) | ServoID;
    uint8_t data2 = Position;
    
    SendOperCommand(data1, data2);

    unsigned long start_time = millis();

    while(wck_Ser->available() < 2) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return time_out_response; // Return timeout response if no data received
        }
    }
    
    Response_packet response;

    response.load = wck_Ser->read(); // Read load value
    response.position = wck_Ser->read(); // Read position value

    return response; // Return the response packet
}

bool wCK::PosSendH(char ServoID, char TorqueLevel, int Position){


    uint8_t pos_upper = (Position >> 7) & 0x07;  // bits 9-7
    uint8_t pos_lower = Position & 0x7F;         // bits 6-0


    Serial.printf("\n\nU HEX: %02X, L HEX: %02X\n\n", pos_upper, pos_lower);

    SendOper10BitCommand((7<<5)|0, 0xC8, ServoID, TorqueLevel, pos_upper, pos_lower);

    unsigned long start_time = millis();
    while(wck_Ser->available() < 2) {
        if(millis() - start_time > WCK_TIMEOUT) {
            return false;
        }
    }

    uint8_t response1 = wck_Ser->read();
    uint8_t response2 = wck_Ser->read();

    return (response1 == pos_upper && response2 == pos_lower);
}



void wCK::SyncPosSend(char LastID, char TorqueLevel, char *TargetArray){
    int i;
    char CheckSum;
    LastID=LastID+1;
    i = 0;
    CheckSum = 0;
    Serial.write(0xFF);
    Serial.write((TorqueLevel<<5)|0x1f);
    Serial.write(LastID+1);
    for(int i=0; i<=LastID; i++){
        Serial.write(TargetArray[i]);
        CheckSum = CheckSum ^ TargetArray[i];
    }
    CheckSum = CheckSum & 0x7f;
    Serial.write(CheckSum);
} 

Response_packet wCK::Rotation360(char ServoID, char TorqueLevel, char RotationDir){
    uint8_t data1 = (6 << 5) | ServoID; // Command to rotate 360 degrees
    uint8_t data2 = (RotationDir << 4) | TorqueLevel; // Set rotation direction and speed level

    SendOperCommand(data1, data2); // Send the command

    unsigned long start_time = millis();
    while(wck_Ser->available() < 1) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return time_out_response; // Return -1 on timeout
        }
    }

    Response_packet response;

    response.load = wck_Ser->read(); // Read load value
    response.position = wck_Ser->read(); // Read position value

    return response; // Return the response packet
}

char wCK::PassiveCK(char ServoID){
    uint8_t data1 = 0xC0 | ServoID; // Command to pass over the servo

    SendOperCommand(data1, 0x10); // Send the command

    unsigned long start_time = millis();
    while(wck_Ser->available() < 2) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return -1; // Return -1 on timeout
        }
    }   

    if(wck_Ser->read() != 0x10){
        return -1; // If the first byte is not 0x10, return -1 error code
    }

    return wck_Ser->read(); // Read and return the response
}

char wCK::BreakWCK(void){
    SendOperCommand(0xDF, 0x20); // Send the command

    unsigned long start_time = millis();
    while(wck_Ser->available() < 2) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return -1; // Return -1 on timeout
        }
    }

    char ServoID = wck_Ser->read(); // Read ServoID
    wck_Ser->read(); // Read and discard the second byte

    if(ServoID < 31) {
        return ServoID; // Return ServoID if valid
    }
    
    return 0xFF; // Return 0xFF on error
}

/************************************************ACTIONS************************************************/

/************************************************SETTERS************************************************/
bool wCK::setBaudrate(char ServoID, char NewBaud){    
    SendSetCommand((7<<5)|ServoID, 0x08, NewBaud, NewBaud); 


    unsigned long start_time = millis();
    while(wck_Ser->available() < 2) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return false; // Return -1 on timeout
        }
    }

    char response = wck_Ser->read(); // Read the response
    
    if(response ==  wck_Ser->read() && response == NewBaud){
        return true;
    }


    return false; // Return false if the response does not match the expected value
}

bool wCK::setPDGain(char ServoID, char *NewPgain, char *NewDgain){
    SendSetCommand((7<<5)|ServoID, 0x09, *NewPgain, *NewDgain); // Send the command

    unsigned long start_time = millis();
    while(wck_Ser->available() < 2) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return false; // Return false on timeout
        }
    }

    char response1 = wck_Ser->read(); // Read the first response byte
    char response2 = wck_Ser->read(); // Read the second response byte

    if(response1 == *NewPgain && response2 == *NewDgain){
        return true; // Return true if both responses match the expected values
    }

    return false; // Return false if the responses do not match
}

bool wCK::setIGain(char ServoID, char *NewIgain){
    SendSetCommand((7<<5)|ServoID, 0x15, *NewIgain, *NewIgain); // Send the command

    unsigned long start_time = millis();
    while(wck_Ser->available() < 2) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return false; // Return false on timeout
        }
    }

    char response1 = wck_Ser->read(); // Read the first response byte
    char response2 = wck_Ser->read(); // Read the second response byte

    if(response1 == *NewIgain && response2 == *NewIgain){
        return true; // Return true if both responses match the expected values
    }

    return false; // Return false if the responses do not match
}

bool wCK::setRuntimePDGain(char ServoID, char *NewPgain, char *NewDgain){
    SendSetCommand((7<<5)|ServoID, 0x0B, *NewPgain, *NewDgain); // Send the command

    unsigned long start_time = millis();
    while(wck_Ser->available() < 2) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return false; // Return false on timeout
        }
    }

    char response1 = wck_Ser->read(); // Read the first response byte
    char response2 = wck_Ser->read(); // Read the second response byte

    if(response1 == *NewPgain && response2 == *NewDgain){
        return true; // Return true if both responses match the expected values
    }

    return false; // Return false if the responses do not match
}

bool wCK::setRuntimeIGain(char ServoID, char *NewIgain){
    SendSetCommand((7<<5)|ServoID, 0x18, *NewIgain, *NewIgain); // Send the command

    unsigned long start_time = millis();
    while(wck_Ser->available() < 2) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return false; // Return false on timeout
        }
    }

    char response1 = wck_Ser->read(); // Read the first response byte
    char response2 = wck_Ser->read(); // Read the second response byte

    if(response1 == *NewIgain && response2 == *NewIgain){
        return true; // Return true if both responses match the expected values
    }

    return false; // Return false if the responses do not match
}

bool wCK::setId(char ServoID, char NewId){
    SendSetCommand((7<<5)|ServoID, 0x0C, NewId, NewId); // Send the command

    unsigned long start_time = millis();
    while(wck_Ser->available() < 2) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return false; // Return false on timeout
        }
    }

    char response1 = wck_Ser->read(); // Read the first response byte
    char response2 = wck_Ser->read(); // Read the second response byte

    if(response1 == NewId && response2 == NewId){
        return true; // Return true if both responses match the expected values
    }

    return false; // Return false if the responses do not match
}

bool wCK::setSpeed(char ServoID, char NewSpeed, char NewAccel){
    SendSetCommand((7<<5)|ServoID, 0x0D, NewSpeed, NewAccel); // Send the command

    unsigned long start_time = millis();
    while(wck_Ser->available() < 2) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return false; // Return false on timeout
        }
    }

    char response1 = wck_Ser->read(); // Read the first response byte
    char response2 = wck_Ser->read(); // Read the second response byte

    if(response1 == NewSpeed && response2 == NewAccel){
        return true; // Return true if both responses match the expected values
    }

    return false; // Return false if the responses do not match
}

void wCK::setRuntimeSpeed(char ServoID, char NewSpeed, char NewAccel){
    SendSetCommand((7<<5)|ServoID, 0x17, NewSpeed, NewAccel); // Send the command
}

bool wCK::setOverLoad(char ServoID, char NewOverLoad){
    SendSetCommand((7<<5)|ServoID, 0x0F, NewOverLoad, NewOverLoad); // Send the command

    unsigned long start_time = millis();
    while(wck_Ser->available() < 2) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return false; // Return false on timeout
        }
    }

    char response1 = wck_Ser->read(); // Read the first response byte
    char response2 = wck_Ser->read(); // Read the second response byte

    if(response1 == NewOverLoad && response2 == NewOverLoad){
        return true; // Return true if both responses match the expected values
    }

    return false; // Return false if the responses do not match
}

bool wCK::setBoundary(char ServoID, char *NewLBound, char *NewUBound){
    SendSetCommand((7<<5)|ServoID, 0x11, *NewLBound, *NewUBound); // Send the command

    unsigned long start_time = millis();
    while(wck_Ser->available() < 2) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return false; // Return false on timeout
        }
    }

    char response1 = wck_Ser->read(); // Read the first response byte
    char response2 = wck_Ser->read(); // Read the second response byte

    if(response1 == *NewLBound && response2 == *NewUBound){
        return true; // Return true if both responses match the expected values
    }

    return false; // Return false if the responses do not match
}
/************************************************SETTERS************************************************/



/************************************************GETTERS************************************************/
Response_packet wCK::getPos(char ServoID){
    uint8_t data1 = 0xA0 | ServoID; // Command to read position

    SendOperCommand(data1, 0); // Send the command

    unsigned long start_time = millis();
    while(wck_Ser->available() < 1) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return time_out_response; // Return -1 on timeout
        }
    }

    Response_packet response;
    response.load = wck_Ser->read(); // Read load value
    response.position = wck_Ser->read(); // Read position value

    return response; // Return the response packet
}

int wCK::getPosH(char ServoID){
    while(wck_Ser->available()){
        wck_Ser->read(); // Clear the serial buffer
    }

    SendSetCommand((7<<5)|0, 0xC9, ServoID, ServoID);

    unsigned long start_time = millis();
    while(wck_Ser->available() < 2) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return -1; // Return -1 on timeout
        }
    }
    
    char pos_upper = wck_Ser->read(); // Read upper 3 bits of position
    char pos_lower = wck_Ser->read(); // Read lower 7 bits of position

    
    int position = (pos_upper << 7) | (pos_lower); // Combine upper and lower bits
    
    Serial.printf("\n\nU HEX: %02X, L HEX: %02X ", pos_upper, pos_lower);
    Serial.printf("Position: %d\n\n", position);

    return position; // Return the combined position value
}

char wCK::getPDGain(char ServoID, char *Pgain, char *Dgain){
    SendSetCommand((7<<5)|ServoID, 0x0C, 0, 0); // Send the command to read gains

    unsigned long start_time = millis();
    while(wck_Ser->available() < 2) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return -1; // Return -1 on timeout
        }
    }

    *Pgain = wck_Ser->read(); // Read P gain
    *Dgain = wck_Ser->read(); // Read D gain

    if((*Pgain > 0 && *Pgain < 51) && (*Dgain < 101)) {
        return 1; // Return 1 if gains are valid
    }
    
    return 0; // Return 0 if gains are invalid
}

char wCK::getIGain(char ServoID, char *Igain){
    SendSetCommand((7<<5)|ServoID, 0x16, 0, 0); // Send the command to read gains

    unsigned long start_time = millis();
    while(wck_Ser->available() < 2) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return -1; // Return -1 on timeout
        }
    }

    *Igain = wck_Ser->read(); // Read P gain

    if(wck_Ser->read() == *Igain) {
        return 1; // Return 1 if gains are valid
    }

    return 0; // Return 0 if gains are invalid
}

char wCK::getSpeed(char ServoID, char* NewSpeed, char *NewAccel){
    SendSetCommand((7<<5)|ServoID, 0x0E, 0, 0); // Send the command to read speed and acceleration

    unsigned long start_time = millis();
    while(wck_Ser->available() < 2) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return -1; // Return -1 on timeout
        }
    }

    *NewSpeed = wck_Ser->read(); // Read speed
    *NewAccel = wck_Ser->read(); // Read acceleration

    if((*NewSpeed >= 31 && *NewAccel < 20 && *NewAccel > 100) || (*NewSpeed < 31 && *NewAccel < 20)) {
        return 0; // Return 0 if speed and acceleration are invalid
    }

    return 1; // Return 1 if speed and acceleration are read successfully
}

char wCK::getOverLD(char ServoID){
    SendSetCommand((7<<5)|ServoID, 0x10, 0, 0); // Send the command to read overload current

    unsigned long start_time = millis();
    while(wck_Ser->available() < 1) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return -1; // Return -1 on timeout
        }
    }

    char OverCT = wck_Ser->read(); // Read overload current

    if(OverCT >= 0 && OverCT <= WCK_CURRENT_LIMIT_1800MA) {
        return OverCT; // Return the overload current if valid
    }

    return -1; // Return -1 if the overload current is invalid
}

char wCK::getBound(char ServoID, char *LBound, char *UBound){
    SendSetCommand((7<<5)|ServoID, 0x12, 0, 0); // Send the command to read boundaries

    unsigned long start_time = millis();
    while(wck_Ser->available() < 2) {
        // Wait for response or timeout
        if(millis() - start_time > WCK_TIMEOUT) {
            return -1; // Return -1 on timeout
        }
    }

    *LBound = wck_Ser->read(); // Read lower boundary
    *UBound = wck_Ser->read(); // Read upper boundary

    return 1; // Return 1 if boundaries are valid
}
/************************************************GETTERS************************************************/