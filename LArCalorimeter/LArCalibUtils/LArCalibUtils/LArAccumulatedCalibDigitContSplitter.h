/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ----------------------------------------------------------------------------
// Splitting AccumulatedCalibDigits container according pulsed calibration line
//
// Author: F. Tomasz
// May 2007
// ----------------------------------------------------------------------------

#ifndef LARACCUMULATEDCALIBDIGITCONTSPLITTER_H
#define LARACCUMULATEDCALIBDIGITCONTSPLITTER_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include <vector>
#include <string>
#include <map>

class LArAccumulatedCalibDigitContSplitter : public AthAlgorithm
{
 public:
  LArAccumulatedCalibDigitContSplitter(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArAccumulatedCalibDigitContSplitter();

  //standard algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode stop();
  StatusCode finalize(){ return StatusCode::SUCCESS;}
    
 private:
  
  StatusCode executeWithAccumulatedDigits();
  
  std::vector<std::string> m_keylist;
  std::vector<std::string> m_OutputList;

  bool  m_recAll;
  unsigned   m_numLine;

  bool m_useDacAndIsPulsedIndex;//used to store different waves for different HEC calib lines

  unsigned m_event_counter; 
};  

#endif
