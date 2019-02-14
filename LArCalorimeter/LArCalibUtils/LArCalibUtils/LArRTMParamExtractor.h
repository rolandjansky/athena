//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/*
 * LArRTMParamExtractor
 *
 * This algorithm reads LArCaliWaveContainers and extract from each of the
 * LArCaliWaves the LArCaliPulseParams and LArDetCellParams parameters 
 * according to the corresponding RTM algorithms.
 *
 * Author:      Marco.Delmastro@cen.ch
 * 
 */

#ifndef LArRTMPARAMEXTRACTOR_H
#define LArRTMPARAMEXTRACTOR_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArCabling/LArOnOffIdMapping.h"

class LArRTMParamExtractor : public AthAlgorithm
{

 public:

  LArRTMParamExtractor(const std::string & name, ISvcLocator * pSvcLocator);
  
  ~LArRTMParamExtractor();

  StatusCode initialize();
  StatusCode execute()    {return StatusCode::SUCCESS;}
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
  
 private:

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  std::vector<std::string> m_keylist;
  bool m_testmode, m_dumpOmegaScan, m_dumpResOscill;
  std::string m_omegaScanKey, m_resOscillKeyBefore, m_resOscillKeyAfter;
  
  bool m_ignoreDACselection;
  std::vector<int> m_DAC;
  
  bool  m_extractTcal, m_extractFstep, m_extractOmega0, m_extractTaur ;
  float m_defaultTcal, m_defaultFstep, m_defaultOmega0, m_defaultTaur ;
  
  bool m_recoverEmptyDB;
  
  std::string m_suffixRetrievedCaliPulse, m_suffixRetrievedDetCell;
  std::string m_keyRetrievedCaliPulse, m_keyRetrievedDetCell ;
  
  std::string m_suffixExtractedCaliPulse, m_suffixExtractedDetCell;
  std::string m_keyExtractedCaliPulse, m_keyExtractedDetCell ;
  
  // Grouping type
  std::string m_groupingType;

  // FT selection
  bool m_FTselection;
  std::vector<int> m_FT;
  int m_PosNeg;
  // Slot selection
  bool m_Slotselection;
  std::vector<int> m_Slot;
  // Calib line selection
  bool m_Calibselection;
  int m_Cline;

};

#endif
