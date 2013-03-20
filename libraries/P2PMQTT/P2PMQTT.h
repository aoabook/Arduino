/*
 * P2PMQTT protocol implementation for Arduino
 *
 * (c) D. Cuartielles & A. Goransson, Malmo University, K3 School of Arts
 */

#ifndef P2PMQTT_h
#define P2PMQTT_h

#include <AndroidAccessory.h>
#include <Arduino.h>
//#include "Stream.h"
 

 // Message types
 // note that 0x00 and 0x0F are reserved
 // the ones implemented are marked with *
 #define CONNECT       0x01  // client request to connect to server *
 #define CONNACK       0x02  // connect acknowledgment *
 #define PUBLISH       0x03  // publish message *
 #define PUBACK        0x04  // publish acknowledgment
 #define PUBREC        0x05  // publish received
 #define PUBREL        0x06  // publish release
 #define PUBCOMP       0x07  // publish complete
 #define SUBSCRIBE     0x08  // client subscribe request *
 #define SUBACK        0x09  // subscribe acknowledgment *
 #define UNSUBSCRIBE   0x0A  // client unsubscribe request *
 #define UNSUBACK      0x0B  // unsubscribe acknowledgment *
 #define PINGREQ       0x0C  // PING request *
 #define PINGRESP      0x0D  // PING response *
 #define DISCONNECT    0x0E  // client is disconnecting *
 
 // DUP (Duplicate delivery) is a 1 bit flag on bit 3
 #define DUP  8
 
 // QoS (Quality of Service) is a 2 bits flag on bits 2-1
 #define QoS1  4
 #define QoS0  2
 
 // RETAIN is a 1 bit flag on bit 0
 #define RETAIN  1

// CONNECT Flags (used in the variable header of a CONNECT message)
#define USER_NAME     128  // always 0, we will not use it
#define PASSWORD      64   // always 0, we will not use it
#define WILL_RETAIN   32   // always 0, we will not use it
#define WILL_QoS1     16   // always 0, we will not use it
#define WILL_QoS0     8    // always 0, we will not use it
#define WILL_FLAG     4    // always 0, we will not publish upon error
#define CLEAN_SESSION 2    // always 1, we won't store anything upon disconnect
#define RESERVED      1    // who knows, it is reserved

// CONNECT RETURN CODES for CONNACK
#define ACK                      0
#define NACK_BAD_PROTOCOL        1
#define NACK_REJECTED_ID         2
#define NACK_SERVER_UNAVAILABLE  3
#define NACK_BAD_USER_PASS       4
#define NACK_NOT_AUTHORIZED      5

// PROTOCOL IMPLEMENTATION
#define PROTOCOL_NAME             "P2PP2PMQTT"
#define PROTOCOL_NAME_LENGTH_MSB  0
#define PROTOCOL_NAME_LENGTH_LSB  7
#define PROTOCOL_VERSION          1

// CLIENT ID
#define CLIENT_ID             "Arduino"
#define CLIENT_ID_LENGTH_MSB  0
#define CLIENT_ID_LENGTH_LSB  7

//XXX this will probably be removed later
// MSG PARTS
#define PAYLOAD   0
#define TOPIC     1

  // structures needed to compose each type of message
  // P2PMQTT CONNECT package
  // we send no Will Topic, nor Will Message, nor User Name, nor Password
  // our Connect will only use the clientID and in our case will always be
  // "Arduino"
  typedef struct {
    // Fixed header
    byte fixedHeader;
    byte length;
    // Variable Header
    byte lengthProtocolNameMSB;
    byte lengthProtocolNameLSB;
    byte * protocolName;
    byte protocolVersion;
    byte connectFlags;
    byte keepAliveMSB;
    byte keepAliveLSB;
    byte clientIdMSB;
    byte clientIdLSB;
    byte * clientId;
  } P2PMQTTconnect;

  // P2PMQTT CONNACK package
  typedef struct {
    // Fixed header
    byte fixedHeader;
    byte length;
    // Variable Header
    byte topicNameCompression;  // this is not used, will be zero
    byte returnCode;
  } P2PMQTTconnack;

  // P2PMQTT PUBLISH package
  typedef struct {
    // Fixed header
    byte fixedHeader;
    byte length; // this has to change to be a byte array
    // Variable Header
    byte lengthTopicMSB;
    byte lengthTopicLSB;
    byte * topic;
    byte * payload;
  } P2PMQTTpublish;

  // P2PMQTT SUBSCRIBE package
  typedef struct {
    // Fixed header
    byte fixedHeader;
    byte length;
    // Variable Header
    byte msgIdMSB;
    byte msgIdLSB;
    byte lengthTopicMSB;
    byte lengthTopicLSB;
    byte * topic;
    byte topicQoS;
  } P2PMQTTsubscribe;

  // P2PMQTT SUBACK package
  typedef struct {
    // Fixed header
    byte fixedHeader;
    byte length;
    // Variable Header
    byte msgIdMSB;
    byte msgIdLSB;
    byte topicQoS;
  } P2PMQTTsuback;

  // P2PMQTT UNSUBSCRIBE package
  typedef struct {
    // Fixed header
    byte fixedHeader;
    byte length;
    // Variable Header
    byte msgIdMSB;
    byte msgIdLSB;
    byte lengthTopicMSB;
    byte lengthTopicLSB;
    byte * topic;
  } P2PMQTTunsubscribe;

  // P2PMQTT UNSUBACK package
  typedef struct {
    // Fixed header
    byte fixedHeader;
    byte length;
    // Variable Header
    byte msgIdMSB;
    byte msgIdLSB;
  } P2PMQTTunsuback;

  // P2PMQTT PINGREQ package
  typedef struct {
    // Fixed header
    byte fixedHeader;
    byte length;
  } P2PMQTTpingreq;

  // P2PMQTT PINGRESP package
  typedef struct {
    // Fixed header
    byte fixedHeader;
    byte length;
  } P2PMQTTpingresp;

  // P2PMQTT DISCONNECT package
  typedef struct {
    // Fixed header
    byte fixedHeader;
    byte length;
  } P2PMQTTdisconnect;

/*
 *  Class definition
 */
class P2PMQTT : public Stream {

  public:
  
  // Constructor
  P2PMQTT(bool debug = false);

  // methods
  bool begin(const char* model = "default");
  bool begin(   const char *manufacturer, const char *model, const char *description,
                const char *version, const char *uri, const char *serial );
  int connect(byte, long);
  int subscribe(P2PMQTTsubscribe);
  int publish(P2PMQTTpublish pub);
    bool isConnected(void);
    virtual size_t write(uint8_t *buff, size_t len);

    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);

    virtual void flush();
    virtual size_t write(uint8_t);

    using Print::write; // pull in write(str) and write(buf, size) from Print

    int getType(byte* buffer);
    byte* getMsgPublishField(byte* buffer, int field);
    byte* getPayload(byte* buffer, int type);
    byte* getTopic(byte* buffer, int type);
    bool checkTopic(byte* buffer, int type, char* topic);
    bool cmpStr(byte* str1, char* str2, int length);
    
    // KEEP ALIVE TIMER
    // it is a 16 bits number containing two bytes (MSB, LSB) counting
    // the amount of time the client should be alive, waiting for PINGREQ, etc
    // we could use a define for this, but it is better practice to have a
    // variable inside the library
    int keepAliveTimer;

    // TOPIC NAME
    // string containing the topic name of this board on the P2PP2PMQTT protocol
    // on the Arduino side, topic names will ONLY be 10 chars long
    String topic;

    // MESSAGE IDENTIFIER
    // this is needed for PUBLISH, PUBACK, PUBREC, PUBREL, PUBCOMP, SUBSCRIBE,
    // SUBACK, UNSUBSCRIBE, UNSUBACK ... it is a 16 bit unsigned int, typically
    // increases from one message to the other, a client has to maintain two 
    // different indentifiers, one for the sending stream and one for the receiving one
    int msgIdOut;
    int msgIdIn;
    
    // buffer to get the structs by reference as taken from the communication
    // as we work with a reduced version of MQTT we will make all our packages
    // max length 128 bytes, which means 2 bytes for the fixed header and 126 for the rest
    byte buffer[128];

    private:

    // shall we print out debug information?
    bool debug;

    // is there a connected object?
    boolean isConnectedObject;

    // USB buffer utils
    byte* getTopicUSB(int length);

};

#endif

