/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMONMSG_H
#define TRIGMONMSG_H

/*
  @author Doug Schaefer
  @date March 2012

  @brief Wrapper for Athena Message service
*/

#include "AthenaKernel/MsgStreamMember.h"

//using namespace Athena;

class TrigMonMSG
{

 public:
  TrigMonMSG(const std::string name);
  ~TrigMonMSG() {}

  void Log(const std::string message, MSG::Level lvl = MSG::DEBUG) {
    m_stream << lvl << " - " << message << endmsg;
  }

 private:

  Athena::MsgStreamMember m_stream;  
};

#endif
