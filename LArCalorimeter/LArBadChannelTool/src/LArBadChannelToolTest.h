/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadChannelToolTest_H
#define LArBadChannelToolTest_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"
#include "LArIdentifier/LArOnlineID.h"

class ILArBadChanTool;
class StoreGateSvc;

class LArBadChannelToolTest : public Algorithm {
public:
  LArBadChannelToolTest( const std::string& name, ISvcLocator* pSvcLocator );
  StatusCode initialize();
  StatusCode execute();
  //StatusCode finalize();

private:
  ToolHandle< ILArBadChanTool > m_BadChanTool;
  StoreGateSvc*                 m_detStore;
  const DataHandle<LArOnlineID> m_onlineID;

  int m_Check1;
  int m_Check2;
};

#endif
