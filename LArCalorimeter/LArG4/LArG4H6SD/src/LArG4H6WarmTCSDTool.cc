/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6WarmTCSDTool.h"
#include "LArG4H6WarmTCSD.h"

#include "G4String.hh"

LArG4H6WarmTCSDTool::LArG4H6WarmTCSDTool(const std::string& type, const std::string& name, const IInterface *parent)
  : SensitiveDetectorBase(type,name,parent)
{
}

G4VSensitiveDetector* LArG4H6WarmTCSDTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );
  //FIXME use configurable properties here

  // be smart, and take only last part of the name
  G4String colname = "";
  if(name().find("::Abs")!=std::string::npos){
    colname = "WarmTCAbsCollection";
  } else {
    if(name().find("::X")!=std::string::npos){
      colname = "WarmTCSciXCollection";
    } else {
      colname = "WarmTCSciYCollection";
    }
  }
  return new LArG4H6WarmTCSD(name(),colname);
}

StatusCode LArG4H6WarmTCSDTool::Gather()
{
  LArG4H6WarmTCSD* anSD = dynamic_cast<LArG4H6WarmTCSD*>(getSD());
  if (anSD) anSD->EndOfAthenaEvent();
  return StatusCode::SUCCESS;
}
