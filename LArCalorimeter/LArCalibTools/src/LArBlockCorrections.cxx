/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArBlockCorrections.h"
#include "LArRawConditions/LArConditionsContainerBase.h"

LArBlockCorrections::LArBlockCorrections(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name,pSvcLocator) {}

StatusCode LArBlockCorrections::initialize() {
  bool setFlag =   LArConditionsContainerBase::applyCorrectionsAtInit(true, false);

  ATH_MSG_INFO ( "LArConditionsContainerBase::applyCorrectionsAtInit set to " << setFlag );

  return StatusCode::SUCCESS;
}

