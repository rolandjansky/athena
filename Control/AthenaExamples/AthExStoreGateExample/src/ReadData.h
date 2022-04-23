// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXSTOREGATEEXAMPLE_READDATA_H
#define ATHEXSTOREGATEEXAMPLE_READDATA_H 1

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthExStoreGateExample/MyDataObj.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
class StoreGateSvc;

/////////////////////////////////////////////////////////////////////////////

class ReadData:public AthAlgorithm {
public:
  ReadData (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  //Properties
  std::string m_DataProducer;
  SG::ReadHandle<MyDataObj> m_dobj3;
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this, "EvtInfo", "EventInfo", "EventInfo name"};
};

#endif // not ATHEXSTOREGATEEXAMPLE_READDATA_H








