/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCSensitiveDetectorTool.h"
#include "TGCSensitiveDetector.h"

TGCSensitiveDetectorTool::TGCSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{
  declareInterface<ISensitiveDetector>(this);
}

G4VSensitiveDetector* TGCSensitiveDetectorTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );
  return new TGCSensitiveDetector(name(), m_outputCollectionNames[0]);
}
