#include "messages.h"

MessagesClass::MessagesClass(){
  memset((void*)&messages, DROP, sizeof(messages));
  memset((void*)&broadcast, DROP, sizeof(broadcast));
}

void MessagesClass::Post(unsigned char num){
  if(messages[num] == DROP){
    messages[num] = NEW;
  }
}

unsigned char MessagesClass::Get(unsigned char num){
  if(messages[num] == READY){
    messages[num] = DROP;
    return 1;
  }
  return 0;
}

void MessagesClass::Process(){
  for(unsigned char i = 0; i < MAX_MESSAGES; i++){
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

void MessagesClass::PostBroadcast(unsigned char num){
  broadcast[num] = NEW;
}

unsigned char MessagesClass::GetBroadcast(unsigned char num){
  if(broadcast[num] == READY){
    return 1;
  }
  return 0;
}

void MessagesClass::ProcessBroadcast(){
  for(unsigned char i = 0; i < MAX_BROADCAST; i++){
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
unsigned char MessagesClass::Peek(unsigned char num){
  if(messages[num] == READY){
    return 1;
  }
  return 0;
}

