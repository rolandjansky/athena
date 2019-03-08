//Dear emacs, this is  -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARCALIBUTILS_LARTIMETUNING_H
#define LARCALIBUTILS_LARTIMETUNING_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArElecCalib/ILArOFCTool.h"
#include "LArRawUtils/LArRoI_Map.h"
#include "LArRawUtils/LArRawOrdering.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "GaudiKernel/Bootstrap.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "TBEvent/TBPhase.h"
#include "LArRawConditions/LArGlobalTimeOffset.h"
#include "LArRawConditions/LArFEBTimeOffset.h"
#include "LArRawConditions/LArCellTimeOffset.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class LArTimeTuning : public AthAlgorithm
{

public:

  LArTimeTuning (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~LArTimeTuning();
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode stop() override;

private:
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  const LArOnlineID* m_onlineHelper;
  SG::ReadHandleKey<LArDigitContainer> m_DataLocation
    { this, "DataLocation", "FREE", "" };
  SG::ReadHandleKey<TBPhase> m_tbPhaseReadKey
    { this, "TBPhaseReadKey", "TBPhase", "" };
  SG::WriteHandleKey<LArGlobalTimeOffset>  m_globalTimeOffsetOut
    { this, "GlobalTimeOffsetOutKey", "GlobalTimeOffset", "" };
  SG::WriteHandleKey<LArFEBTimeOffset>  m_febTimeOffsetOut
    { this, "FebTimeOffsetOutKey", "FebTimeOffset", "" };
  SG::WriteHandleKey<LArCellTimeOffset>  m_cellTimeOffsetOut
    { this, "CellTimeOffsetOutKey", "CellTimeOffset", "" };
  SG::WriteHandleKey<TBPhase>  m_tbPhaseWriteKey
    { this, "TBPhaseWriteKey", "TBPhase", "" };

  short m_AdcCut;
  short m_AdcMax;
  unsigned int m_Nevents;
  unsigned int m_NaboveThreshold;
  unsigned int m_Nconv;
  int  m_maxIterations;
  float  m_OFCTimePrecision;
  int m_NOFCPhases;
  int m_NOFCTimeBins;
  int m_initialTimeSampleShift;

  //Time ranges for OFC application
  //static const double m_SamplingPeriode;
  double m_SamplingPeriode;
  double m_SamplingPeriodeUpperLimit,m_SamplingPeriodeLowerLimit;

  float m_OFCTimeBin;
  
  bool m_phaseInv ;
  bool m_saveRemErr ;
  bool m_useOFCphase ;
  bool m_binHalfOffset;
  bool m_allowTimeJump;
  bool m_skipSaturCell;
  
  int m_corrSign ;
  unsigned int m_nIterAverage ;
  
  std::string m_scopeStr;
  
  // layer selection (use only cells from one layer)
  int m_layerSel;
  const LArEM_ID* m_emId;
  
  // gain selection
  std::string m_gainSel; 
  CaloGain::CaloGain m_gain ;

  enum SCOPE {
    GLOBAL=0,
    FEB,
    CELL,
    PHASE
  };
  SCOPE m_scope;

  class WeightedAverage { 
  public:
    WeightedAverage() : timePeak(0),peak(0) {};
    double timePeak;
    double peak;
    inline double get() const {return (peak==0. ? 0 : timePeak/peak);}
  };
  
  /*
  class maxmin {
  public:
    inline maxmin() : max(-9999999),min(99999999) {};
    inline float middle() const 
    {return ((max+min)/2.0);}
    float max;
    float min;
  };
  */

  typedef std::map<HWIdentifier, WeightedAverage> WeightedAverageMAP;

  WeightedAverageMAP m_CellTimeAverage;
  //std::map<HWIdentifier, WeightedAverage> m_timePerFeb;
  //std::map<HWIdentifier, WeightedAverage> m_timePerCell;
 

};

#endif
