/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "NeutronFastSim/NeutronFastSimSD.h"

#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"

#include "MCTruth/TrackHelper.h"

#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Neutron.hh"

static  FADS::SensitiveDetectorEntryT<NeutronFastSimSD> cavfastsd("NeutronFastSimSD");

void NeutronFastSimSD::Initialize(G4HCofThisEvent*) 
{
  trackRecordCollection = new TrackRecordCollection("NeutronBG");
}

void NeutronFastSimSD::WriteNeutron(const G4Track* neutron)
{
  if (!neutron) { msg(MSG::ERROR) << "the neutron pointer was zero" << endreq; return; }

  G4VPhysicalVolume *preVol=neutron->GetVolume();

  int pdgcode = (neutron->GetDefinition())?neutron->GetDefinition()->GetPDGEncoding():0; // Had damn well better be a neutron...

  G4ThreeVector pos=neutron->GetPosition();
  G4ThreeVector mom=neutron->GetMomentum();

  double ener=neutron->GetTotalEnergy();
  double time=neutron->GetGlobalTime();

  TrackHelper trHelp(neutron);
  int barcode = trHelp.GetBarcode();

  //create the TimedTrackRecord
  TrackRecord *rec=new TrackRecord(pdgcode,ener,mom,pos,time,barcode,preVol?preVol->GetName():"Unknown");

  if (!trackRecordCollection){
    msg(MSG::ERROR) << "No collection" << endreq;
    delete rec;
  } else trackRecordCollection->Insert(rec);
}

void NeutronFastSimSD::EndOfEvent(G4HCofThisEvent* ) 
{
  if (trackRecordCollection) theHelper.ExportCollection<TrackRecordCollection>(trackRecordCollection);
}

