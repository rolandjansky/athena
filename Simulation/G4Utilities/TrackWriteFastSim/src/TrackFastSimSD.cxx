/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Class header
#include "TrackWriteFastSim/TrackFastSimSD.h"

// Athena headers
#include "CxxUtils/make_unique.h" // For make unique
#include "MCTruth/TrackHelper.h"

// Geant4 headers
#include "G4DynamicParticle.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"

// STL headers
#include <cmath>

TrackFastSimSD::TrackFastSimSD(const std::string& name, const std::string& outputCollectionName)
  : G4VSensitiveDetector( name )
  , m_trackRecordCollection( outputCollectionName )
{
}

// Initialize from G4 - necessary to new the write handle for now
void TrackFastSimSD::Initialize(G4HCofThisEvent *)
{
  if (!m_trackRecordCollection.isValid()) m_trackRecordCollection = CxxUtils::make_unique<TrackRecordCollection>(m_trackRecordCollection.name());
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

