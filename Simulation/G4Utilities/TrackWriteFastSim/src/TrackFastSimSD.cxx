/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackWriteFastSim/TrackFastSimSD.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "MCTruth/TrackHelper.h"

#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4DynamicParticle.hh"

#include <cmath>

static  FADS::SensitiveDetectorEntryT<TrackFastSimSD> trackfastsd("TrackFastSimSD");

void TrackFastSimSD::Initialize(G4HCofThisEvent*) 
{
  trackRecordCollection = new TrackRecordCollection(m_colName);
}

void TrackFastSimSD::WriteTrack(const G4Track* track, const bool originPos, const bool originMom)
{
  if (!track) { msg(MSG::ERROR) << "the track pointer was zero" << endreq; return; }

  G4VPhysicalVolume *preVol=track->GetVolume();

  int pdgcode = (track->GetDefinition())?track->GetDefinition()->GetPDGEncoding():0;

  G4ThreeVector pos = originPos?track->GetVertexPosition():track->GetPosition();
  double ener=originMom?(track->GetVertexKineticEnergy()+track->GetDynamicParticle()->GetMass()):track->GetTotalEnergy();
  G4ThreeVector mom = track->GetMomentum();
  if (originMom){
    double mommag = std::sqrt(std::pow(ener,2)-std::pow(track->GetDynamicParticle()->GetMass(),2));
    mom = track->GetVertexMomentumDirection()*mommag;
  }

  double time=track->GetGlobalTime();
  TrackHelper trHelp(track);
  int barcode = trHelp.GetBarcode();

  //create the TimedTrackRecord
  TrackRecord *rec=new TrackRecord(pdgcode,ener,mom,pos,time,barcode,preVol?preVol->GetName():"Unknown");

  if (!trackRecordCollection){
    msg(MSG::ERROR) << "No collection" << endreq;
    delete rec;
  } else trackRecordCollection->Insert(rec);
}

void TrackFastSimSD::EndOfEvent(G4HCofThisEvent* ) 
{
  if (trackRecordCollection) theHelper.ExportCollection<TrackRecordCollection>(trackRecordCollection);
}

void TrackFastSimSD::SetCollectionName(const std::string s) 
{ 
  m_colName = s; 
  if (trackRecordCollection) trackRecordCollection->setName(s);
}

