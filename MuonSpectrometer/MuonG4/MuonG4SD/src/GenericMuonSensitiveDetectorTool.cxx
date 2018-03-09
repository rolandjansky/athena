/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GenericMuonSensitiveDetectorTool.h"
#include "GenericMuonSensitiveDetector.h"

GenericMuonSensitiveDetectorTool::GenericMuonSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{
}

G4VSensitiveDetector* GenericMuonSensitiveDetectorTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );
  return new GenericMuonSensitiveDetector(name(), m_outputCollectionNames[0]);
}
