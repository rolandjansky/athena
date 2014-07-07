/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArBlockCorrections.h"
#include "LArRawConditions/LArConditionsContainerBase.h"

LArBlockCorrections::LArBlockCorrections(const std::string& name, ISvcLocator* pSvcLocator) : 
  Algorithm(name,pSvcLocator) {}

StatusCode LArBlockCorrections::initialize() {
  MsgStream log(msgSvc(),name());

  bool setFlag =   LArConditionsContainerBase::applyCorrectionsAtInit(true, false);

  log << MSG::INFO << "LArConditionsContainerBase::applyCorrectionsAtInit set to " << setFlag << endreq;

  return StatusCode::SUCCESS;
}

