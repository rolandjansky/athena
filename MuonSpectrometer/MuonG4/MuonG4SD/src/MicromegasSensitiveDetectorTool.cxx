/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MicromegasSensitiveDetectorTool.h"
#include "MicromegasSensitiveDetector.h"

MicromegasSensitiveDetectorTool::MicromegasSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{
  declareInterface<ISensitiveDetector>(this);
}

G4VSensitiveDetector* MicromegasSensitiveDetectorTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );
  return new MicromegasSensitiveDetector(name(), m_outputCollectionNames[0]);
}
