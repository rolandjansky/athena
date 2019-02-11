/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARCONDFLATBASE_H
#define LARCONDFLATBASE_H

#include <string>
#include "GaudiKernel/StatusCode.h"
#include "LArIdentifier/LArOnlineID.h"

// Forward delcarations
class MsgStream;


class LArCondFlatBase {

 public:
  LArCondFlatBase();
  ~LArCondFlatBase();
  StatusCode initializeBase(const char* context);
  
 protected:
  bool 	m_isInitialized;
  const LArOnlineID*          m_onlineHelper;
  MsgStream*                  m_log;

};

#endif
