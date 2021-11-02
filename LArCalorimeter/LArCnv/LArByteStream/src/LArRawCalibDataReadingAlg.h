/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_LARRAWCALIBDATAREADINDINGALG_H
#define LARBYTESTREAM_LARRAWCALIBDATAREADINDINGALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadCondHandle.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "LArRecConditions/LArCalibLineMapping.h"


//Event classes
class LArDigitContainer;
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
  SG::ReadCondHandleKey<LArCalibLineMapping>  m_CLKey{this, "CalibLineKey", "LArCalibLineMap", "SG calib line key"};

  //Event output:
  SG::WriteHandleKey<LArDigitContainer> m_DigitKey{this,"LArDigitKey",""};
  SG::WriteHandleKey<LArCalibDigitContainer> m_calibDigitKey{this,"LArCalibDigitKey",""};
  SG::WriteHandleKey<LArAccumulatedDigitContainer> m_accDigitKey{this,"LArAccDigitKey",""};
  SG::WriteHandleKey<LArAccumulatedCalibDigitContainer> m_accCalibDigitKey{this,"LArAccCalibDigitKey",""};
  SG::WriteHandleKey<LArFebHeaderContainer> m_febHeaderKey{this,"LArFebHeaderKey",""};
    
  //Service providing the input data
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc{this,"ROBDataProviderSvc","ROBDataProviderSvc"};
  
  //Other properties:
  BooleanProperty m_verifyChecksum{this,"VerifyChecksum",true,"Calculate and compare checksums to detect data transmission errors"}; 
  BooleanProperty m_failOnCorruption{this,"FailOnCorruption",true,"Return FAILURE if data corruption is found"};

  Gaudi::Property<std::string> m_subCaloPreselection{this,"SubCaloPreselection","","One of 'EM', 'HEC' or 'FCAL'"};

  Gaudi::Property<std::vector<unsigned> > m_vBEPreselection{this,"BEPreselection",{},"For channel-selection: Barrel=0, Endcap=1"};
  Gaudi::Property<std::vector<unsigned> > m_vPosNegPreselection{this,"PosNegPreselection",{}, "For channel-selection: C-Side:0, A-Side: 1"};
  Gaudi::Property<std::vector<unsigned> > m_vFTPreselection{this,"FTNumPreselection",{}, "For channel-selection: Feedthrough numbers (e.g. 0 - 31 for barrel)"};
  DoubleProperty m_delayScale{this,"DelayScale",(25./240.)*Gaudi::Units::nanosecond,"One calibration step in time"};

  std::set<HWIdentifier> m_vFinalPreselection;

  //Identifier helper
  const LArOnlineID* m_onlineId=nullptr;

  //Switches set in initialize() based of SG keys of output object
  bool m_doDigits=false;
  bool m_doCalibDigits=false;
  bool m_doAccDigits=false;
  bool m_doAccCalibDigits=false;
  bool m_doFebHeaders=false;
 
};

#endif
