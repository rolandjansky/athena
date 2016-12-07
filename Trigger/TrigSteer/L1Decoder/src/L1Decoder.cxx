/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./L1Decoder.h"


L1Decoder::L1Decoder(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator) {}
StatusCode L1Decoder::initialize() {
  return StatusCode::SUCCESS;
}
StatusCode L1Decoder::execute() {
  return StatusCode::SUCCESS;
}
StatusCode L1Decoder::finalize() {
  return StatusCode::SUCCESS;
}


