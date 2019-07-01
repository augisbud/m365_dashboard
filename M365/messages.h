#ifndef MESSAGES_h
#define MESSAGES_h

#define MAX_MESSAGES  15
#define MAX_BROADCAST  2

#include "Arduino.h"

#define DROP  0
#define NEW   1
#define READY 2


class MessagesClass
{
  private:
    uint8_t messages[MAX_MESSAGES];
    uint8_t broadcast[MAX_BROADCAST];

  public:
    MessagesClass();
    void Post(uint8_t);
    uint8_t Get(uint8_t);
    uint8_t Peek(uint8_t); //for sniffer
    void Process();
    void PostBroadcast(uint8_t);
    uint8_t GetBroadcast(uint8_t);
    void ProcessBroadcast();
};

#endif
