/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARCONDFLATBASE_H
#define LARCONDFLATBASE_H

#include <string>
#include "GaudiKernel/StatusCode.h"
#include "LArIdentifier/LArOnlineID.h"
#include "AthenaBaseComps/AthMessaging.h"

// Forward delcarations
class MsgStream;


class LArCondFlatBase
  : public AthMessaging
{

 public:
  LArCondFlatBase(const std::string& name);
  ~LArCondFlatBase();
  StatusCode initializeBase();
  
 protected:
  bool 	m_isInitialized;
  const LArOnlineID*          m_onlineHelper;
};

#endif
