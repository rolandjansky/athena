//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARDELTARESPPREDICTOR_H
#define LARDELTARESPPREDICTOR_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "LArRawConditions/LArWFParams.h"
#include "LArRawConditions/LArCaliWave.h"

#include "LArCalibUtils/LArWFParamTool.h"
#include "LArCalibUtils/LArDeltaRespTool.h" 
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

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

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this, "OnOffMap", "LArOnOffIdMap", "SG key for mapping object"};

  std::vector<std::string> m_keylist;

};

#endif
