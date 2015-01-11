//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARDELTARESPPREDICTOR_H
#define LARDELTARESPPREDICTOR_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "LArRawConditions/LArWFParams.h"
#include "LArRawConditions/LArCaliWave.h"

#include "LArCalibUtils/LArWFParamTool.h"
#include "LArCalibUtils/LArDeltaRespTool.h" 

#include <vector>
#include <string>

class LArDeltaRespPredictor : public AthAlgorithm
{
 public:
 
  LArDeltaRespPredictor(const std::string & name, ISvcLocator * pSvcLocator);

  ~LArDeltaRespPredictor();

  //standard algorithm methods
  StatusCode initialize() ; 
  StatusCode execute() {return StatusCode::SUCCESS;} //empty method
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
 
 private:

  std::vector<std::string> m_keylist;

};

#endif
