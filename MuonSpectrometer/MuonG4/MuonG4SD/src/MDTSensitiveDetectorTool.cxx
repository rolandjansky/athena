/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MDTSensitiveDetectorTool.h"
#include "MDTSensitiveDetector.h"

MDTSensitiveDetectorTool::MDTSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{
  declareInterface<ISensitiveDetector>(this);
}

G4VSensitiveDetector* MDTSensitiveDetectorTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );
  return new MDTSensitiveDetector(name(), m_outputCollectionNames[0]);
}
