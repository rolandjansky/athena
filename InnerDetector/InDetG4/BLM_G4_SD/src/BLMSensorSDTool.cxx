/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//###############################################
//   BLM Sensitive Detector tool
//
//###############################################

// Class header
#include "BLMSensorSDTool.h"

// Package headers
#include "BLMSensorSD.h"


BLMSensorSDTool::BLMSensorSDTool(const std::string& type, const std::string& name, const IInterface *parent)
  : SensitiveDetectorBase(type,name,parent)
{
  declareInterface<ISensitiveDetector>(this);
}

G4VSensitiveDetector* BLMSensorSDTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );

  // Create a fresh SD
  return new BLMSensorSD(name(), m_outputCollectionNames[0]);
}
