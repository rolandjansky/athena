//Dear emacs, this is  -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARTIMETUNING
#define LARTIMETUNING

#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArElecCalib/ILArOFCTool.h"
#include "LArRawUtils/LArRoI_Map.h"
#include "LArRawUtils/LArRawOrdering.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "GaudiKernel/Bootstrap.h"

#include "LArIdentifier/LArOnlineID.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"

class LArTimeTuning : public AthAlgorithm
{

public:

  LArTimeTuning (const std::string& name, ISvcLocator* pSvcLocator);
  ~LArTimeTuning();
  StatusCode initialize();
  StatusCode execute();
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}

private:
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  const LArOnlineID* m_onlineHelper;
  std::string m_DataLocation;
  std::string m_globalTimeOffsetOut, m_febTimeOffsetOut, m_cellTimeOffsetOut;

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
