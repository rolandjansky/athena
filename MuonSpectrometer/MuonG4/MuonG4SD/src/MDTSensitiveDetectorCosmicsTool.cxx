/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MDTSensitiveDetectorCosmicsTool.h"
#include "MDTSensitiveDetectorCosmics.h"

MDTSensitiveDetectorCosmicsTool::MDTSensitiveDetectorCosmicsTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{
  declareInterface<ISensitiveDetector>(this);
}

G4VSensitiveDetector* MDTSensitiveDetectorCosmicsTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );
  return new MDTSensitiveDetectorCosmics(name(), m_outputCollectionNames[0]);
}
