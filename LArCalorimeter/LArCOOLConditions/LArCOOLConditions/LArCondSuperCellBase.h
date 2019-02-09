/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARSUPERCELLCONDBASE_H
#define LARSUPERCELLCONDBASE_H

#include <string>
#include "GaudiKernel/StatusCode.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"

// Forward delcarations
class MsgStream;

class LArCondSuperCellBase {

 public:
  LArCondSuperCellBase();
  ~LArCondSuperCellBase();
  StatusCode initializeBase(const char* context);
  
 protected:
  bool 	                       m_isInitialized;
  const LArOnline_SuperCellID* m_scOnlineID;
  MsgStream*                   m_log;

};

#endif
