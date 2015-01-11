//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARCALIWAVEMERGER_H
#define LARCALIWAVEMERGER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArRawConditions/LArCaliWaveContainer.h"
#include <string>

class LArCaliWaveMerger : public AthAlgorithm
{

 public:

  LArCaliWaveMerger(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArCaliWaveMerger();

  //standard algorithm methods
  StatusCode initialize() { return StatusCode::SUCCESS ; } 
  StatusCode execute()    { return StatusCode::SUCCESS ; } 
  StatusCode stop();
  StatusCode finalize()   { return StatusCode::SUCCESS ; }
  
 private:
    
  std::vector<std::string> m_keylist ; 
  std::string m_keyout ;
  std::string m_groupingType;

};

#endif
