/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadChannelToolTest_H
#define LArBadChannelToolTest_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"
#include "LArIdentifier/LArOnlineID.h"

class ILArBadChanTool;

class LArBadChannelToolTest : public AthAlgorithm {
public:
  LArBadChannelToolTest( const std::string& name, ISvcLocator* pSvcLocator );
  StatusCode initialize();
  StatusCode execute();
  //StatusCode finalize();

private:
  ToolHandle< ILArBadChanTool > m_BadChanTool;
  const DataHandle<LArOnlineID> m_onlineID;

  int m_Check1;
  int m_Check2;
};

#endif
