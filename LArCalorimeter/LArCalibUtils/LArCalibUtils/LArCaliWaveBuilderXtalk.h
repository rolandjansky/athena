/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/************************************************************

 This algorithm produces a LArCaliWaveContainer
 out of a LArCalibDigitContainer. They key of the
 containers that are to be processed  is defined 
 by the jobOption 'KeyList'. The resulting 
 containers are put in the DetectorStore with the 
 same keys. 

 This algorithm was largely inspired from the LArCaliWaveBuilder
 algorithm; it however allows to reconstruct channels
 that were not pulsed,in order to study cross talk
 The definition of the desired pattern is given in the job
 options

 Different calibration pattern are available : 
  - EM Barrel : StripSingle[1-4] / MiddleSingle[1-8] / BackSingle[1-4]
  - EM Endcap : StripSingle[1-4] / MiddleSingle[1-4] / BackSingle[1-4]
    and MiddleSingle[1-4] / BackSingle[1-4] for inner wheel
 For EM endcap special crates studies, set SpecialCrate to true and
 SpecialRegion to InnerWheel or OuterWheel.

 FeedthroughPul is used to find the relevant calibration pattern. Empty
 ntuples/histograms may be produced with badly set FeedthroughPul, or if
 there are bad channels or disconnected FEBs in FeedthroughPul.


 NB.
 Currently the calibration patterns are tested for one particular channel,
 hard-coded in initializeCabling(), and taken in the feed-through FeedthroughPul.
 The algorithm should be runnned for each calibration pattern and each
 subdetectors.
 This could, for example, be improved by:
  (1) Using directly calibration line channels to detect the pattern. This 
      would avoid to specify, and look at, a particular feed-through.
      One could also set dynamicaly the different patterns, with job options
      (for example in the same way that they are set for calibration runs: 
      four words of 32 bits).
  (2) Create one LArCaliWaveContainer per pattern (gathered inside a vector 
      for exemple). This should permit to reconstruct all patterns in one pass.


************************************************************/

#ifndef LARCALIWAVEBUILDERXTALK_H
#define LARCALIWAVEBUILDERXTALK_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"
#include "LArRawConditions/LArCaliWaveContainer.h"
#include "LArIdentifier/LArOnlineID.h"

#include "LArCabling/LArCablingService.h"
#include "LArElecCalib/ILArPedestal.h"
#include "CaloIdentifier/LArEM_ID.h"

#include <vector>
#include <string>
#include <map>

class LArCaliWaveBuilderXtalk : public AthAlgorithm
{
 public:
  LArCaliWaveBuilderXtalk(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArCaliWaveBuilderXtalk();

  //standard algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode stop();
  StatusCode finalize(){ return StatusCode::SUCCESS;}

 private:

  StatusCode initializeCabling();
  
  std::vector<std::string> m_keylist;
  std::vector<std::string> m_keylistproperty;
  std::string m_keyoutput;
  std::string m_groupingType;

  // Intermediate caching objects
  typedef std::map<int, LArCaliWave>      WaveMap;
  typedef LArConditionsContainer<WaveMap> WaveContainer;
  WaveContainer   m_waves;

  const LArOnlineID*   m_onlineHelper;
  ToolHandle<LArCablingService> m_larCablingSvc;

  std::vector<HWIdentifier> m_CalibLineHW;

  int 	   m_feedthroughNumber;	// FT which is used to check the calibration pattern
  int 	   m_posOrNeg;		// A/C side
  int 	   m_barrelEndcap;	// Barrel / Endcap

  std::string m_calibPattern;  
  std::string m_partition;

  bool m_isSpecialCrate;		// for special crates
  std::string m_emecSpecialRegion;	// OuterWheel / InnerWheel
  bool m_isInnerWheel;			// for EMEC inner wheel

  unsigned m_event_counter; 
  int	   m_NStep; 
  float    m_SamplingPeriod;
  float    m_dt;
  int	   m_ADCsatur;		// Saturation check

  int	   m_cutOnSample;	// Cut waves (0 = no cut)
};  

#endif

