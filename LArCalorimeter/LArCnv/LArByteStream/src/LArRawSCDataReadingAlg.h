//Dear emacs, this is -*-c++-*- 
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_LARRAWSCDATAREADINDINGALG_H
#define LARBYTESTREAM_LARRAWSCDATAREADINDINGALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadCondHandle.h"
#include "LArCabling/LArLATOMEMapping.h"
#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/ReadHandleKey.h"
#include "GaudiKernel/ToolHandle.h"

#include "LArByteStream/LArLATOMEDecoder.h"

//Event classes
class LArDigitContainer;
class LArRawSCContainer;
class LArLATOMEHeaderContainer;
class LArOnlineID;
class IROBDataProviderSvc;

class LArRawSCDataReadingAlg : public  AthReentrantAlgorithm {

public:
  LArRawSCDataReadingAlg(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;

private:
  // Mapping input
  SG::ReadCondHandleKey<LArLATOMEMapping> m_mapKey {this,"MappingKey","LArLATOMEMap"};
  //Event output:
  SG::WriteHandleKey<LArDigitContainer> m_adcCollKey{this, "adcCollKey", "SC"};
  SG::WriteHandleKey<LArDigitContainer> m_adcBasCollKey{this, "adcBasCollKey", "SC_ADC_BAS"};
  SG::WriteHandleKey<LArRawSCContainer> m_etCollKey{this, "etCollKey", "SC_ET", "SG key of the LArRawSCContainer"};
  SG::WriteHandleKey<LArRawSCContainer> m_etIdCollKey{this, "etIdCollKey", "SC_ET_ID", "SG key of the LArRawSCContainer"};
  SG::WriteHandleKey<LArLATOMEHeaderContainer> m_latomeHeaderCollKey{this, "LArLATOMEHeaderKey", "SC_LATOME_HEADER", "SG key of the LArLATOMEHEaderContainer"};

  //Service providing the input data
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc{this, "ROBDataProviderSvc", "ROBDataProviderSvc"};
  
  //Identifier helper
  const LArOnlineID* m_onlineId = nullptr;

  // The LATOME Decoder tool
  ToolHandle<LArLATOMEDecoder> m_latomeDecoder;

  //Switches set in initialize() based of SG keys of output object
  bool m_doAdc = true;
  bool m_doAdcBas = true;
  bool m_doEt = true;
  bool m_doEtId = true;
  bool m_doLATOMEHeader = true;
 
};

#endif
