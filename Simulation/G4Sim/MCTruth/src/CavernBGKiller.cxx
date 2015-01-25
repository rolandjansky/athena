/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/CavernBGKiller.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"

#include "G4VSolid.hh"
#include "MCTruth/TrackHelper.h"

#include "G4Neutron.hh"

static  FADS::SensitiveDetectorEntryT<CavernBGKiller> ttrksd("CavernBGKiller");

CavernBGKiller::CavernBGKiller(std::string name)
 : FADS::FadsSensitiveDetector(name)
 , trackRecordCollection(0)
{;}

CavernBGKiller::~CavernBGKiller()
{;}

void CavernBGKiller::Initialize(G4HCofThisEvent*) 
{
  trackRecordCollection = new TrackRecordCollection("CavernBG");
}

G4bool CavernBGKiller::ProcessHits(G4Step* aStep,G4TouchableHistory* )
{
  // Only score neutrons
  if (aStep->GetTrack()->GetDefinition()!=G4Neutron::Neutron() ) return false;

  G4StepPoint *preStep=aStep->GetPreStepPoint();
  G4StepPoint *postStep=aStep->GetPostStepPoint();
  G4VPhysicalVolume *preVol=preStep?preStep->GetPhysicalVolume():0;
  G4VPhysicalVolume *postVol=postStep?postStep->GetPhysicalVolume():0;

  // Only if we're changing volumes
  if (preVol == postVol) return false; 

  int volume=0;

  // Dirty trick because G4 string comparisons were buggy... 
  static const G4String volNames[15] = 
      { "MA::MA" , "MB::MB" , "MC::MC" ,
        "ME1::ME1" , "ME2::ME2" , "FI1::FI1" , "FI2::FI2" ,
        "FE1::FE1" , "FE2::FE2" , "FM1::FM1" , "FM2::FM2" ,
        "FO1::FO1" , "FO2::FO2" , "CS1::CS1" , "CS2::CS2" };

  if (postVol &&
      postVol->GetName().length()==6 ){
    if     (postVol->GetName()==volNames[0] ) volume=110;
    else if(postVol->GetName()==volNames[1] ) volume=120;
    else if(postVol->GetName()==volNames[2] ) volume=130;
  } else if (postVol &&
             postVol->GetName().length()==8 ){
    if     (postVol->GetName()==volNames[3] ) volume=141;
    else if(postVol->GetName()==volNames[4] ) volume=142;
    else if(postVol->GetName()==volNames[5] ) volume=211;
    else if(postVol->GetName()==volNames[6] ) volume=212;
    else if(postVol->GetName()==volNames[7] ) volume=221;
    else if(postVol->GetName()==volNames[8] ) volume=222;
    else if(postVol->GetName()==volNames[9] ) volume=231;
    else if(postVol->GetName()==volNames[10]) volume=232;
    else if(postVol->GetName()==volNames[11]) volume=241;
    else if(postVol->GetName()==volNames[12]) volume=242;
    else if(postVol->GetName()==volNames[13]) volume=301;
    else if(postVol->GetName()==volNames[14]) volume=302;
  }

  if (volume == 0) return false; // Require entering an interesting volume

  int pdgcode = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();

  G4ThreeVector pos=postStep->GetPosition();
  G4ThreeVector mom=postStep->GetMomentum();

  double ener=postStep->GetTotalEnergy();
  double time=postStep->GetGlobalTime();

  TrackHelper trHelp(aStep->GetTrack());
  int barcode = trHelp.GetBarcode();

  //create the TimedTrackRecord
  trackRecordCollection->Emplace(pdgcode,ener,mom,pos,time,barcode,preVol->GetName());

  aStep->GetTrack()->SetTrackStatus(fStopAndKill); // Remove the neutron

  return true;
}
void CavernBGKiller::EndOfEvent(G4HCofThisEvent* ) 
{
  theHelper.ExportCollection<TrackRecordCollection>(trackRecordCollection);
}

