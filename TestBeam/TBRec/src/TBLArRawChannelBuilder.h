/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBLARRAWCHANNELBUILDER_H
#define TBREC_TBLARRAWCHANNELBUILDER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IToolSvc.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "LArRawUtils/LArRawOrdering.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"

#include "CLHEP/Units/SystemOfUnits.h"

class LArOnlineID;

class TBLArRawChannelBuilder : public AthAlgorithm
{

public:

  TBLArRawChannelBuilder (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

private:
  //Services & Tools 
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  const LArEM_ID*   m_emId;
  const LArFCAL_ID* m_fcalId;
  const LArHEC_ID*  m_hecId;
  const LArOnlineID* m_onlineHelper;
  
  LArRawOrdering m_larRawOrdering;
  
  //Algo-properties
  std::string m_DataLocation, m_ChannelContainerName;
  int m_imaxSamp;
  std::string m_mode;
  float m_cubicAdcCut;
  
  float m_ADCtoMeVFCAL[3];
  float m_ADCtoMeVHEC[2];
  float m_ADCtoMeVEMECInner[2];
  float m_ADCtoMeVEMECOuter[2];
  float m_ADCtoMeVEMB[2];
  
  unsigned int m_iPedestal;
};

#endif
