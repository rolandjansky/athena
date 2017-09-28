/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CSCSensitiveDetectorTool.h"
#include "CSCSensitiveDetector.h"

CSCSensitiveDetectorTool::CSCSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{
}

G4VSensitiveDetector* CSCSensitiveDetectorTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );
  return new CSCSensitiveDetector(name(), m_outputCollectionNames[0]);
}
