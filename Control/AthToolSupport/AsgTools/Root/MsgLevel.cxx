// MsgLevel.cxx

#include "AsgTools/MsgLevel.h"

std::string MSG::name(MSG::Level lvl) {
  if ( lvl == MSG::NIL ) return "NIL";
  if ( lvl == MSG::VERBOSE ) return "VERBOSE";
  if ( lvl == MSG::DEBUG ) return "DEBUG";
  if ( lvl == MSG::INFO ) return "INFO";
  if ( lvl == MSG::WARNING ) return "WARNING";
  if ( lvl == MSG::ERROR ) return "ERROR";
  if ( lvl == MSG::FATAL ) return "FATAL";
  if ( lvl == MSG::ALWAYS ) return "ALWAYS";
  return "";
}


