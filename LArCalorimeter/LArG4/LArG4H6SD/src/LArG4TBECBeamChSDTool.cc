/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4TBECBeamChSDTool.h"
#include "LArG4TBECBeamChSensitiveDetector.h"


LArG4TBECBeamChSDTool::LArG4TBECBeamChSDTool(const std::string& type, const std::string& name, const IInterface *parent)
  : SensitiveDetectorBase(type,name,parent)
{
}

G4VSensitiveDetector* LArG4TBECBeamChSDTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );
  return new LArG4TBECBeamChSensitiveDetector(name());
}
