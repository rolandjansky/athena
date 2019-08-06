//Dear emacs, this is -*-c++-*- 
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYSTREAM_LARRAWCHANNELBSREADALG_H
#define LARBYSTREAM_LARRAWCHANNELBSREADALG_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
//#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadCondHandle.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "GaudiKernel/ServiceHandle.h"


//Event classes
class LArRawChannelContainer;
class LArOnlineID;
class IROBDataProviderSvc;

class LArRawChannelBSReadAlg : public  AthReentrantAlgorithm {
 public:
  LArRawChannelBSReadAlg(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;


 private:
  //Event output:
  SG::WriteHandleKey<LArRawChannelContainer> m_rawChannelKey{this,"LArRawChannelKey","LArRawChannels",
      "SG key of the LArRawChannelContainer"};
  
  
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc{this,"ROBDataProviderSvc","ROBDataProviderSvc"};
  
  //Identifier helper
  const LArOnlineID* m_onlineId;
};

#endif
