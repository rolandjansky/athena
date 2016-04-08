/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARSUPERCELLCONDBASE_H
#define LARSUPERCELLCONDBASE_H

#include <string>
#include "GaudiKernel/StatusCode.h"
#include "LArCabling/LArSuperCellCablingTool.h"
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
  LArSuperCellCablingTool*     m_scCablingTool;
  MsgStream*                   m_log;

};

#endif
