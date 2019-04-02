#include "messages.h"

MessagesClass::MessagesClass(){
  memset((void*)&messages, DROP, sizeof(messages));
  memset((void*)&broadcast, DROP, sizeof(broadcast));
}

void MessagesClass::Post(uint8_t num){
  if(messages[num] == DROP){
    messages[num] = NEW;
  }
}

uint8_t MessagesClass::Get(uint8_t num){
  if(messages[num] == READY){
    messages[num] = DROP;
    return 1;
  }
  return 0;
}

void MessagesClass::Process(){
  for(uint8_t i = 0; i < MAX_MESSAGES; i++){
    switch(messages[i]){
      case NEW:
        messages[i] = READY;
        break;
      default:
        messages[i] = DROP;
        break;
    }
  }
}

void MessagesClass::PostBroadcast(uint8_t num){
  broadcast[num] = NEW;
}

uint8_t MessagesClass::GetBroadcast(uint8_t num){
  if(broadcast[num] == READY){
    return 1;
  }
  return 0;
}

void MessagesClass::ProcessBroadcast(){
  for(uint8_t i = 0; i < MAX_BROADCAST; i++){
    switch(broadcast[i]){
      case NEW:
        broadcast[i] = READY;
        break;
      default:
        broadcast[i] = DROP;
        break;
    }
  }
}
uint8_t MessagesClass::Peek(uint8_t num){
  if(messages[num] == READY){
    return 1;
  }
  return 0;
}
