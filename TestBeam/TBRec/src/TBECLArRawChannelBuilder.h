/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBECLARRAWCHANNELBUILDER_H
#define TBREC_TBECLARRAWCHANNELBUILDER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArElecCalib/ILArOFCTool.h"
#include "LArElecCalib/ILArADC2MeVTool.h"
#include "LArElecCalib/ILArHVCorrTool.h"

// sandrine
//#include "CaloIdentifier/LArEM_ID.h"

//#include "LArRawUtils/LArRoI_Map.h"
//#include "LArRawUtils/LArRawOrdering.h"
#include "LArRawEvent/LArRawChannelContainer.h"

#include "LArIdentifier/LArOnlineID.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "StoreGate/ReadCondHandle.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArElecCalib/ILArHVScaleCorr.h"

class CaloCell_ID;
class CaloDetDescrManager;

class TBECLArRawChannelBuilder : public AthAlgorithm
{
public:
  TBECLArRawChannelBuilder (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  //Services & Tools 
  ToolHandle<ILArOFCTool> m_OFCTool;
  ToolHandle<ILArADC2MeVTool> m_adc2mevTool;
  const LArOnlineID* m_onlineHelper;
  const CaloCell_ID* m_calo_id;
  const CaloDetDescrManager* m_calo_dd_man; 

  SG::ReadCondHandleKey<LArOnOffIdMapping>  m_cablingKey {this,"keyCabling", "LArOnOffIdMap", "Input key for Id mapping"} ;
  SG::ReadCondHandleKey<ILArHVScaleCorr> m_offlineHVScaleCorrKey{this, "keyOfflineHVCorr", "LArHVScaleCorrRecomputed","Key for LArHVScaleCorr"};

  //Algo-properties 
  std::string m_DataLocation, m_ChannelContainerName;
  bool m_useTDC, m_useRamp, m_useShape, m_useOFCTool;
  bool m_ConvertADCToHighGain;
  float m_Ecut;
  int m_initialTimeSampleShift;
  int m_NOFCPhases;
  int m_NOFCTimeBins;
  bool m_useIntercept[4];
  bool m_useOFCPhase; 
  bool m_phaseInv ;
  float m_ramp_max[3];

  double m_SamplingPeriode;
  float m_OFCTimeBin;
  bool m_binHalfOffset;
  bool m_allowTimeJump;
  int m_pedestalFallbackMode;
  unsigned int m_iPedestal;
  unsigned int m_shapeMode;

  unsigned m_skipSaturCells; // 0 = no skip ; 1 = skip saturating cells ; 2 = skipp all event
  short m_AdcMax;
  
  //Counters for errors & warnings per job:
  int m_noEnergy;   // Number of events with at least completly failed channel
  int m_noTime;     // Number of events with at least one channel without time info
  int m_noShape;    // Number of events with at least one channel without Shape (=with not quality factor);
  int m_noShapeDer; // Number of events with at least one channel without ShapeDerivative (=not taken into accout for quality factor);
  int m_saturation; // Number of events with at least one saturating channel
  
  //bool m_errorPrint;
  int m_lastNoEnergy;    // Number of completly failed channels in previous event
  int m_lastNoTime;      // Number of channels without time info in previous event
  int m_lastNoShape;     // Number of channels without Shape (=with not quality factor) in previous event
  int m_lastNoShapeDer;  // Number of channels without ShapeDerivative in previous event
  //int m_lastSaturCells;  // Number of saturating channels without in previous event (not used)
  
  float m_aveNoEnergy;   // Average number of completly failed channels per event
  float m_aveNoTime;     // Average number of channels without time info per event
  float m_aveNoShape;    // Average number of channels without Shape (=with not quality factor) per event
  float m_aveNoShapeDer; // Average number of channels without ShapeDerivative per event
  float m_aveSaturCells; // Average number of saturating channels per event
  
  int   m_nEvents;       // Total number of processed events ;
  float m_aveChannels;   // Average number of readout channels per event
  bool  m_hvcorr;
  
  //Time ranges for OFC application
  double m_SamplingPeriodeUpperLimit,m_SamplingPeriodeLowerLimit;
  
  //Private Member functions:
  //void sortChannels(LArRawChannelContainer* container) ;
 
  // to be used for detailed DEBUG output only
  const LArEM_ID* m_emId;

  // For useRamp = False
  float m_adc2mev[30];
};

#endif
