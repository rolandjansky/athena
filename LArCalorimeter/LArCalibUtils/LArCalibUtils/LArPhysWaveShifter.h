//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARPHYSWAVESHIFTER_H
#define LARPHYSWAVESHIFTER_H

#include "AthenaBaseComps/AthAlgorithm.h"


#include "LArRawConditions/LArPhysWave.h"
#include "LArRawConditions/LArPhysWaveContainer.h"
#include "LArRawConditions/LArFEBTimeOffset.h"
#include "LArCalibUtils/LArPhysWaveTool.h" 

#include <vector>
#include <string>

class LArPhysWaveShifter : public AthAlgorithm
{
 public:
 
  LArPhysWaveShifter(const std::string & name, ISvcLocator * pSvcLocator);

  ~LArPhysWaveShifter();

  //standard algorithm methods
  StatusCode initialize() {return StatusCode::SUCCESS;} 
  StatusCode execute() {return StatusCode::SUCCESS;}
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
 
 private:

  const LArOnlineID* m_onlineHelper;
  
  std::vector<std::string> m_keylist;
  std::string m_keyout;
  
  bool        m_compTimeShiftByFEB;
  unsigned    m_modeTimeShiftByFEB;
  std::string m_fileTimeShiftByFEB;
  bool        m_dumpTimeShiftByFEB;
  StatusCode  ComputeTimeShiftByFEB(unsigned mode);
  LArFEBTimeOffset* m_larFEBTstart;

  int  m_timeShiftByIndex;
  bool m_timeShiftByHelper;
  std::vector<unsigned int> m_TshiftLayer;
  bool m_timeShiftFromPeak;
  int  m_nIndexFromPeak, m_nDelays, m_nSamplings ;
  bool m_timeShiftByFEB;
  int  m_timeShiftGuardRegion;
  bool m_usePhysCaliTdiff;

  bool m_timeShiftOffset;
  float m_timeShiftOffsetValue;

  // Grouping type
  std::string m_groupingType;
  std::string m_totalShiftsKey;
  std::string  m_cellByCellShiftsKey;
};

#endif
