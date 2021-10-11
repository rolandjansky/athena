/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_HitHelper.h"

#include "LUCID_GeoModel/LUCID_Constants.h"

int LUCID_HitHelper::GetVolNumber(const G4String& volName) {

  int volNumber = 100;

  if      (!strcmp(volName.substr(9, 9).c_str(), "VesselGas")) volNumber = GASVES;
  else if (!strcmp(volName.substr(9, 7).c_str(), "TubeGas"  )) volNumber = GASTUB;
  else if (!strcmp(volName.substr(9, 3).c_str(), "Pmt"      )) volNumber = QUARTZ;

  return volNumber;
}

int LUCID_HitHelper::GetTubNumber(G4Step* aStep) const {

  G4String volName = aStep->GetTrack()->GetVolume()->GetName();

  int tubNumber = 100;

  if      (!strcmp(volName.substr(5, 9).c_str(), "VesselGas")) tubNumber = -1;
  else if (!strcmp(volName.substr(5, 7).c_str(), "TubeGas"  )) tubNumber = atoi(volName.substr(12, 2).c_str());
  else if (!strcmp(volName.substr(5, 3).c_str(), "Pmt"      )) tubNumber = atoi(volName.substr( 8, 2).c_str());

  if (GetPreStepPoint(aStep).z() < 0) tubNumber += 20;

  return tubNumber;
}
