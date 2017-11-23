/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CSCSensitiveDetectorCosmicsTool.h"
#include "CSCSensitiveDetectorCosmics.h"

CSCSensitiveDetectorCosmicsTool::CSCSensitiveDetectorCosmicsTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{
}

G4VSensitiveDetector* CSCSensitiveDetectorCosmicsTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );
  return new CSCSensitiveDetectorCosmics(name(), m_outputCollectionNames[0]);
}
