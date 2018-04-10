/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "HbbTaggingAlgorithm.h"

#include "xAODJet/JetContainer.h"
#include "JetCalibTools/JetCalibrationTool.h"
#include "BoostedJetTaggers/HbbTaggerDNN.h"


HbbTaggingAlgorithm::HbbTaggingAlgorithm(
  const std::string& name, ISvcLocator* loc):
  AthAlgorithm(name, loc)
{
}

StatusCode HbbTaggingAlgorithm::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode HbbTaggingAlgorithm::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode HbbTaggingAlgorithm::execute() {
  return StatusCode::SUCCESS;
}
