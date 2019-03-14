/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilder
 * @author Kin Yip Kin.Yip@cern.ch
 * @author Hong Ma hma@bnl.gov
 * @author Walter Lampl Walter.Lampl@cern.ch
 * @author Sandrine Laplace laplace@lapp.in2p3.fr
 * @author Many others
 * @brief CTB LArRawChannelBuilder: Convert ADC samples into energy
 * 
 * This code is used in CTB. In ATLAS, it is superseeded by LArRawChannelBuilderDriver.
 */

#ifndef LARRAWCHANNELBUILDER
#define LARRAWCHANNELBUILDER

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "LArElecCalib/ILArOFCTool.h"
#include "LArElecCalib/ILArADC2MeVTool.h"
#include "LArElecCalib/ILArHVCorrTool.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "TBEvent/TBPhase.h"
#include "LArRawEvent/LArRawChannelContainer.h"

#include "LArIdentifier/LArOnlineID.h"

#include "CaloIdentifier/CaloCell_ID.h"

class LArRawChannelBuilder : public AthAlgorithm
{

public:
  LArRawChannelBuilder (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

private:
  //Services & Tools 
  ToolHandle<ILArOFCTool> m_OFCTool;
  ToolHandle<ILArADC2MeVTool> m_adc2mevTool;
  ToolHandle<ILArHVCorrTool> m_hvCorrTool;
  const LArOnlineID* m_onlineHelper;
  //LArRoI_Map* m_roiMap;
  //LArRawOrdering m_larRawOrdering; 


  //Algo-properties
  SG::ReadHandleKey<LArDigitContainer> m_dataLocation
    { this, "DataLocation", "FREE", "" };
  SG::ReadHandleKey<TBPhase> m_tbPhaseLocation
    { this, "TBPhaseLocation", "TBPhase", "" };
  SG::WriteHandleKey<LArRawChannelContainer> m_ChannelContainerName
    { this, "LArRawChannelContainerName", "LArRawChannels", "" };
  bool m_useTDC, m_useOFCTool;
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
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  int m_firstSample;  // shift to apply to Shape from DB to match digitized samples
  std::string m_pedestalKey;
  std::string m_shapesKey;

};

#endif
