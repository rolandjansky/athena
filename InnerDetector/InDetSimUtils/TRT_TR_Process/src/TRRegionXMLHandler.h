/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRREGIONXMLHANDLER_H
#define TRREGIONXMLHANDLER_H

#include <string>
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"

class TRTTransitionRadiation;

class TRRegionXMLHandler {
public:
  TRRegionXMLHandler(TRTTransitionRadiation*);
  void Process(const std::string& name);
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  bool msgLevel (MSG::Level lvl)    { return m_msg.get().level() <= lvl; }

private:
  TRTTransitionRadiation * m_theProcess;
  StoreGateSvc* m_storeGate;
  bool m_initialLayoutIdDict;
  mutable Athena::MsgStreamMember m_msg ATLAS_THREAD_SAFE;
};

#endif
