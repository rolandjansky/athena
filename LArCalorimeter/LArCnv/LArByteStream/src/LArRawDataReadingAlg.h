//Dear emacs, this is -*-c++-*- 
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_LARRAWDATAREADINDINGALG_H
#define LARBYTESTREAM_LARRAWDATAREADINDINGALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadCondHandle.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

//Event classes
class LArRawChannelContainer;
class LArDigitContainer;
class LArFebHeaderContainer;
class LArOnlineID;
class IROBDataProviderSvc;

class LArRawDataReadingAlg : public  AthReentrantAlgorithm {
 public:
  LArRawDataReadingAlg(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;

 private:
  //Event output:
  SG::WriteHandleKey<LArRawChannelContainer> m_rawChannelKey{this,"LArRawChannelKey","LArRawChannels",
      "SG key of the LArRawChannelContainer"};
  SG::WriteHandleKey<LArDigitContainer> m_digitKey{this,"LArDigitKey","FREE"};
  SG::WriteHandleKey<LArFebHeaderContainer> m_febHeaderKey{this,"LArFebHeaderKey","LArFebHeader"};
    
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "eventInfoKey", "EventInfo", "Key for EventInfo object"};

  //Service providing the input data
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc{this,"ROBDataProviderSvc","ROBDataProviderSvc"};
  
  //Other properties:
  BooleanProperty m_verifyChecksum{this,"VerifyChecksum",true,"Calculate and compare checksums to detect data transmission errors"}; 
  BooleanProperty m_failOnCorruption{this,"FailOnCorruption",true,"Return FAILURE if data corruption is found"};

  //Identifier helper
  const LArOnlineID* m_onlineId=nullptr;

  //Switches set in initialize() based of SG keys of output object
  bool m_doRawChannels=true;
  bool m_doDigits=true;
  bool m_doFebHeaders=true;
 
};

#endif
