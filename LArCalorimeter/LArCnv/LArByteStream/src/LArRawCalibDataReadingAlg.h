/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_LARRAWCALIBDATAREADINDINGALG_H
#define LARBYTESTREAM_LARRAWCALIBDATAREADINDINGALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadCondHandle.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "GaudiKernel/ServiceHandle.h"


//Event classes
class LArCalibDigitContainer;
class LArAccumulatedDigitContainer;
class LArAccumulatedCalibDigitContainer;
class LArFebHeaderContainer;
class LArOnlineID;
class IROBDataProviderSvc;

class LArRawCalibDataReadingAlg : public  AthReentrantAlgorithm {
 public:
  LArRawCalibDataReadingAlg(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;

 private:
  //Event output:
  SG::WriteHandleKey<LArCalibDigitContainer> m_calibDigitKey{this,"LArCalibDigitKey",""};
  SG::WriteHandleKey<LArAccumulatedDigitContainer> m_accDigitKey{this,"LArAccDigitKey",""};
  SG::WriteHandleKey<LArAccumulatedCalibDigitContainer> m_accCalibDigitKey{this,"LArAccCalibDigitKey",""};
  SG::WriteHandleKey<LArFebHeaderContainer> m_febHeaderKey{this,"LArFebHeaderKey",""};
    
  //Service providing the input data
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc{this,"ROBDataProviderSvc","ROBDataProviderSvc"};
  
  //Other properties:
  BooleanProperty m_verifyChecksum{this,"VerifyChecksum",true,"Calculate and compare checksums to detect data transmission errors"}; 
  BooleanProperty m_failOnCorruption{this,"FailOnCorruption",true,"Return FAILURE if data corruption is found"};

  Gaudi::Property<std::vector<unsigned> > m_vBEPreselection{this,"BEPreselection",{},"For channel-selection: Barrel=0, Endcap=1"};
  Gaudi::Property<std::vector<unsigned> > m_vPosNegPreselection{this,"PosNegPreselection",{}, "For channel-selection: C-Side:0, A-Side: 1"};
  Gaudi::Property<std::vector<unsigned> > m_vFTPreselection{this,"FTNumPreselection",{}, "For channel-selection: Feedthrough numbers (e.g. 0 - 31 for barrel)"};

  std::set<unsigned> m_vFinalPreselection;

  //Identifier helper
  const LArOnlineID* m_onlineId=nullptr;

  //Switches set in initialize() based of SG keys of output object
  bool m_doCalibDigits;
  bool m_doAccDigits;
  bool m_doAccCalibDigits;
  bool m_doFebHeaders;
 
};

#endif
