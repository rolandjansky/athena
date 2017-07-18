//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARCALIWAVEBUILDER_H
#define LARCALIWAVEBUILDER_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"


#include <vector>
#include <string>
#include <map>

#include "LArRawConditions/LArCaliWaveContainer.h"


class ILArPedestal;
class LArCablingBase;

class LArCaliWaveBuilder : public AthAlgorithm
{
 public:
  LArCaliWaveBuilder(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArCaliWaveBuilder();

  StatusCode initialize();
  StatusCode execute();
  virtual StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
    
 private:
  
  bool       m_useAccumulatedDigits;
  StatusCode executeWithAccumulatedDigits();
  StatusCode executeWithStandardDigits();
  
  std::vector<std::string> m_keylist;
  std::vector<std::string> m_keylistproperty;
  std::string m_keyoutput ;
  std::string m_groupingType;

  // Intermediate caching objects
  typedef std::map<int, LArCaliWave>      WaveMap;
  typedef LArConditionsContainer<WaveMap> WaveContainer;
  WaveContainer m_waves;

  // Pedestal subtraction
  const DataHandle<ILArPedestal> m_larPedestal;
  bool                m_pedSub;
  //const ILArPedestal* m_larPedestal;
  unsigned            m_baseline;

  // Reco also unpulsed and saturated cells ?
  bool            m_recAll;
  
  // Running on cells or supercells?
  bool            m_isSC;
  
  //used to store different waves for different HEC calib lines
  bool m_useDacAndIsPulsedIndex;

  // Saturation check
  int              m_ADCsatur;

  // Empty phases (missing files) check
  bool            m_checkEmptyPhases;

  const LArOnlineID_Base*   m_onlineID;
  LArCablingBase*     m_cablingSvc;
  
  unsigned m_event_counter; 
  int      m_NStep;
  float    m_SamplingPeriod;
  float    m_dt;

  uint16_t m_fatalFebErrorPattern;

  int      m_usePatt;
  unsigned m_numPatt; 
};  

#endif
