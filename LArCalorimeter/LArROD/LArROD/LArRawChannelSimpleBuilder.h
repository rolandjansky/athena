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

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "LArElecCalib/ILArADC2MeVTool.h"
#include "LArRecUtils/LArParabolaPeakRecoTool.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class LArRawChannelContainer;


class LArRawChannelSimpleBuilder : public AthReentrantAlgorithm
{

public:

  LArRawChannelSimpleBuilder (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;


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
  SG::ReadHandleKey<LArDigitContainer> m_DataLocation
    { this, "DataLocation", "FREE", "" };
  SG::WriteHandleKey<LArRawChannelContainer> m_ChannelContainerName
    { this, "LArRawChannelContainerName", "LArRawChannels", "" };
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
  
  unsigned int m_iPedestal;
};

#endif
