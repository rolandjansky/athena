/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARSUPERCELLCONDBASE_H
#define LARSUPERCELLCONDBASE_H

#include <string>
#include "GaudiKernel/StatusCode.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "AthenaBaseComps/AthMessaging.h"

// Forward delcarations
class MsgStream;

class LArCondSuperCellBase
  : public AthMessaging
{
 public:
  LArCondSuperCellBase(const std::string& name);
  ~LArCondSuperCellBase();
  StatusCode initializeBase();
  
 protected:
  bool 	                       m_isInitialized;
  const LArOnline_SuperCellID* m_scOnlineID;
};

#endif
