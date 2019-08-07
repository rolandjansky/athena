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
class LArDigitContainer;
class LArFebHeaderContainer;
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

  SG::WriteHandleKey<LArDigitContainer> m_digitKey{this,"LArDigitKey","FREE"};

  SG::WriteHandleKey<LArFebHeaderContainer> m_febHeaderKey{this,"LArFebHeaderKey","LArFebHeaders"};
    
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc{this,"ROBDataProviderSvc","ROBDataProviderSvc"};
  
  //Other properties:
  BooleanProperty m_verifyChecksum{this,"VerifyChecksum",true,"Calculate and compare checksums to detect data transmission errors"};
  
  BooleanProperty m_failOnCorruption{this,"FailOnCorruption",true,"Return FAILURE if data corruption is found"};

  //Identifier helper
  const LArOnlineID* m_onlineId;

  //Switches set in initialize based of SG keys 
  bool m_doRawChannels=true;
  bool m_doDigits=true;
  bool m_doFebHeaders=true;
 
};

#endif
