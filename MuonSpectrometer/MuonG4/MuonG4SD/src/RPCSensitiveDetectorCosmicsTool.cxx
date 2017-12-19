/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RPCSensitiveDetectorCosmicsTool.h"
#include "RPCSensitiveDetectorCosmics.h"

RPCSensitiveDetectorCosmicsTool::RPCSensitiveDetectorCosmicsTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{
}

G4VSensitiveDetector* RPCSensitiveDetectorCosmicsTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );
  return new RPCSensitiveDetectorCosmics(name(), m_outputCollectionNames[0]);
}
