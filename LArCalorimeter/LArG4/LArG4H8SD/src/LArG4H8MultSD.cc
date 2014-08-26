/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H8SD/LArG4H8MultSD.h"
#include "LArG4H8SD/LArG4H8CalibSD.h"
#include "LArG4SD/LArG4SD.h"

#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"


#include "G4Step.hh"
#include "G4TouchableHistory.hh"

using namespace FADS;

static SensitiveDetectorEntryT<LArG4H8MultSD> TBstacMultSD("TBSTACMult");
static SensitiveDetectorEntryT<LArG4H8MultSD> TBpresampModuleMultSD("TBBarrelPresamplerMult");


LArG4H8MultSD::LArG4H8MultSD(G4String a_name)
  : LArG4MultSD(a_name)
{

  LArG4SD* larSD = 0;
  LArG4H8CalibSD* larCalibSD = 0;

  if(a_name=="TBSTACMult")
  {
    larSD = new LArG4SD("EMB::STAC");
    larCalibSD = new LArG4H8CalibSD("TB::EMB::STAC::Calibration");
  }
  else if(a_name=="TBBarrelPresamplerMult")
  {
    larSD = new LArG4SD("Barrel::Presampler::Module");
    larCalibSD = new LArG4H8CalibSD("TB::Barrel::Presampler::Module::Calibration");
  }

  if(larSD)
  {
    larSD->Register();
    addSD(larSD);
  }

  if(larCalibSD)
  {
    larCalibSD->Register();
    addSD(larCalibSD);
  }
}


LArG4H8MultSD::~LArG4H8MultSD() 
{
}
