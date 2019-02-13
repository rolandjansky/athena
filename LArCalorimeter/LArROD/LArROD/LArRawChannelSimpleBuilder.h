/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelSimpleBuilder
 * @author Walter Lampl
 * @author Rob McPherson
 * @author Sandrine Laplace
 * @brief CTB simplified LArRawChannelBuilder using hardcoded calib. constants
 * 
 */

#ifndef LARRAWCHANNELSIMPLEBUILDER
#define LARRAWCHANNELSIMPLEBUILDER

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "LArElecCalib/ILArADC2MeVTool.h"
#include "LArRecUtils/LArParabolaPeakRecoTool.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

class LArRawChannelContainer;


class LArRawChannelSimpleBuilder : public AthAlgorithm
{

public:

  LArRawChannelSimpleBuilder (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  //Services & Tools 
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this, "OnOffMap", "LArOnOffIdMap", "SG key for mapping object"};
  const LArEM_ID*   m_emId;
  const LArFCAL_ID* m_fcalId;
  const LArHEC_ID*  m_hecId;
  const LArOnlineID* m_onlineHelper;
  ToolHandle<ILArADC2MeVTool> m_adc2mevTool;

  ToolHandle<LArParabolaPeakRecoTool> m_peakParabolaTool;
  
  //Algo-properties
  std::string m_DataLocation, m_ChannelContainerName;
  int m_imaxSamp;
  std::string m_mode;
  std::string m_FCALmodeTime;
  float m_cubicAdcCut;
  bool m_useRampDB, m_usePedestalDB;
  int m_averageSamplesEM,m_averageSamplesHEC,m_averageSamplesFCAL;
  float m_averageScaleEM,m_averageScaleHEC,m_averageScaleFCAL;
  
  float m_ADCtoMeVFCAL[3];
  float m_ADCtoMeVHEC[2];
  float m_ADCtoMeVEMECInner[2];
  float m_ADCtoMeVEMECOuter[4];
  float m_ADCtoMeVEMB[4];
  
  //Private Member functions:
  void sortChannels(LArRawChannelContainer* container) ;
 
  unsigned int m_iPedestal;
  LArRawChannelContainer* m_larRawChannelContainer;

};

#endif
