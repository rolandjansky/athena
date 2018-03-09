/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Class header
#include "TrackWriteFastSim/TrackFastSimSD.h"

// Athena headers
#include "CxxUtils/make_unique.h" // For make unique
#include "MCTruth/TrackHelper.h"

// Geant4 headers
#include "G4ChargedGeantino.hh"
#include "G4DynamicParticle.hh"
#include "G4Geantino.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSolid.hh"

// STL headers
#include <cmath>

TrackFastSimSD::TrackFastSimSD(const std::string& name, const std::string& outputCollectionName, const int SD_type)
  : G4VSensitiveDetector( name )
  , m_trackRecordCollection( outputCollectionName )
  , m_SD_type( SD_type )
{
}

// Initialize from G4 - necessary to new the write handle for now
void TrackFastSimSD::Initialize(G4HCofThisEvent *)
{
  if (!m_trackRecordCollection.isValid()) m_trackRecordCollection = CxxUtils::make_unique<TrackRecordCollection>(m_trackRecordCollection.name());
}

G4bool TrackFastSimSD::ProcessHits(G4Step* aStep,G4TouchableHistory* )
{
  if (!m_SD_type) { return false; }
  const G4StepPoint *preStep=aStep->GetPreStepPoint();
  const G4VPhysicalVolume *preVol=preStep->GetPhysicalVolume();
  const G4StepPoint *postStep=aStep->GetPostStepPoint();
  const G4VPhysicalVolume *postVol=postStep->GetPhysicalVolume();

  if (preVol==postVol) { return false; }

  const G4Track *track(aStep->GetTrack());

  // PDG Code
  int pdgcode = (track->GetDefinition())?track->GetDefinition()->GetPDGEncoding():0;
  if (track->GetDefinition() == G4Geantino::Definition() ) pdgcode=999;
  if (track->GetDefinition() == G4ChargedGeantino::Definition() ) pdgcode=998;

  // Position and Momentum
  G4ThreeVector pos=postStep->GetPosition();
  G4ThreeVector mom=postStep->GetMomentum();

  if(m_SD_type==2)
    {
      // need to get the local position
      const G4TouchableHistory* touchHist = (G4TouchableHistory*)preStep->GetTouchable();
      const G4AffineTransform trans = track->GetTouchable()->GetHistory()->GetTopTransform(); // trans from global to local
      G4ThreeVector localPos=trans.TransformPoint(pos);
      G4ThreeVector localMom=mom;
      trans.ApplyAxisTransform(localMom); // rotate momentum as needed
      const G4VSolid *shape= touchHist->GetSolid();
      const G4ThreeVector normal=shape->SurfaceNormal(localPos);
      // check if particle is entering. if it is extiting don't record it.
      if(normal.dot(localPos)>=0.) return false;
    }

  // Energy
  const double ener=postStep->GetTotalEnergy();

  // Time
  const double time=postStep->GetGlobalTime();

  // Barcode
  TrackHelper trHelp(track);
  const int barcode = trHelp.GetBarcode();

  //create the TimedTrackRecord
  m_trackRecordCollection->Emplace(pdgcode,ener,mom,pos,time,barcode,preVol->GetName());

  return true;
}

void TrackFastSimSD::WriteTrack(const G4Track* track, const bool originPos, const bool originMom)
{
  if (!track) { G4cout << "ERROR: the track pointer was zero" << G4endl; return; }

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
  m_trackRecordCollection->Emplace(pdgcode,ener,mom,pos,time,barcode,preVol?preVol->GetName():"Unknown");
}

