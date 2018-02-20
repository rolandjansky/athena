/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCSensitiveDetectorCosmicsTool.h"
#include "TGCSensitiveDetectorCosmics.h"

TGCSensitiveDetectorCosmicsTool::TGCSensitiveDetectorCosmicsTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{
}

G4VSensitiveDetector* TGCSensitiveDetectorCosmicsTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );
  return new TGCSensitiveDetectorCosmics(name(), m_outputCollectionNames[0]);
}
