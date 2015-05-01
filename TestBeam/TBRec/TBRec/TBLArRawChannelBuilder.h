/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBLARRAWCHANNELBUILDER
#define TBLARRAWCHANNELBUILDER

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IToolSvc.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "LArRawUtils/LArRoI_Map.h"
#include "LArRawUtils/LArRawOrdering.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArTools/LArCablingService.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"

#include "CLHEP/Units/SystemOfUnits.h"

class LArOnlineID;

class TBLArRawChannelBuilder : public AthAlgorithm
{

public:

  TBLArRawChannelBuilder (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  //Services & Tools 
  LArRoI_Map* m_roiMap;
  LArCablingService *m_larCablingSvc;
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
  
  //Private Member functions:
  //void sortChannels(LArRawChannelContainer* container) ;

  unsigned int m_iPedestal;
};

#endif
