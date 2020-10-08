/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "ISF_Geant4Event/ISFG4Helper.h"

// Geant4 includes
#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"

#include "AtlasHepMC/GenParticle.h"

// G4Atlas includes
#include "MCTruth/AtlasG4EventUserInfo.h"
#include "MCTruth/TrackBarcodeInfo.h"
#include "MCTruth/TrackInformation.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"

// ATLAS GeoPrimitves (Amg::Vector3D etc)
#include "GeoPrimitives/GeoPrimitives.h"


/** convert the given G4Track into an ISFParticle */
ISF::ISFParticle*
iGeant4::ISFG4Helper::convertG4TrackToISFParticle(const G4Track& aTrack,
                                                   const ISF::ISFParticle& parent,
                                                   ISF::TruthBinding* truth)
{
  const G4ThreeVector& g4pos = aTrack.GetPosition();
  const double         gTime = aTrack.GetGlobalTime();
  const Amg::Vector3D  position(g4pos.x(),g4pos.y(),g4pos.z());
  
  const G4ThreeVector& g4mom = aTrack.GetMomentum();
  const Amg::Vector3D  momentum(g4mom.x(),g4mom.y(),g4mom.z());
  
  const G4ParticleDefinition &particleDefinition = *aTrack.GetDefinition();
  double mass    = particleDefinition.GetPDGMass();
  double charge  = particleDefinition.GetPDGCharge();
  int    pdgID   = particleDefinition.GetPDGEncoding();

#ifdef HEPMC3
  HepMC::GenParticlePtr genParticle = (truth) ? truth->getTruthParticle(): nullptr;
#else  
  auto* genParticle = (truth) ? truth->getTruthParticle(): nullptr;
#endif
  Barcode::ParticleBarcode barcode = (genParticle) ? HepMC::barcode(genParticle) : Barcode::fUndefinedBarcode;

  ISF::ISFParticle *isp = new ISF::ISFParticle( position,
                                                momentum,
                                                mass,
                                                charge,
                                                pdgID,
                                                gTime,
                                                parent,
                                                barcode,
                                                truth
                                               );

  return isp;
}


/** return a valid UserInformation object of the G4Track for use within the ISF */
VTrackInformation *
iGeant4::ISFG4Helper::getISFTrackInfo(const G4Track& aTrack) 
{
  VTrackInformation* trackInfo = static_cast<VTrackInformation*>(aTrack.GetUserInformation());
  return trackInfo;
}


/** link the given G4Track to the given ISFParticle */
TrackInformation*
iGeant4::ISFG4Helper::attachTrackInfoToNewG4Track( G4Track& aTrack,
                                                    const ISF::ISFParticle& baseIsp,
                                                    TrackClassification classification,
                                                    HepMC::GenParticlePtr nonRegeneratedTruthParticle)
{
  if ( aTrack.GetUserInformation() ) {
    G4ExceptionDescription description;
    description << G4String("ISFG4Helper::attachTrackInfoToNewG4Track: ")
                << "Trying to attach new TrackInformation object to G4Track which already has a TrackUserInformation attached (trackID: "
                << aTrack.GetTrackID() << ", track pos: "<<aTrack.GetPosition() << ", mom: "<<aTrack.GetMomentum()
                << ", parentID " << aTrack.GetParentID() << ")";
    G4Exception("ISFG4Helper::attachTrackInfoToNewG4Track", "TrackUserInformationAlreadyExists", FatalException, description);
    return nullptr;
  }

  auto* truthBinding = baseIsp.getTruthBinding();
  if ( !truthBinding ) {
    G4ExceptionDescription description;
    description << G4String("ISFG4Helper::attachTrackInfoToNewG4Track: ")
                << "No TruthBinding present in base ISFParticle (trackID: "
                << aTrack.GetTrackID() << ", track pos: "<<aTrack.GetPosition() << ", mom: "<<aTrack.GetMomentum()
                << ", parentID " << aTrack.GetParentID() << ", ISFParticle: "<<baseIsp<<")";
    G4Exception("ISFG4Helper::attachTrackInfoToNewG4Track", "NoISFTruthBinding", FatalException, description);
    return nullptr;
  }

  TrackInformation *trackInfo = new TrackInformation( nonRegeneratedTruthParticle, &baseIsp );
  auto primaryTruthParticle   = truthBinding->getPrimaryTruthParticle();

  trackInfo->SetPrimaryHepMCParticle( primaryTruthParticle );
  trackInfo->SetClassification( classification );
  aTrack.SetUserInformation( trackInfo );

  return trackInfo;
}

/** return pointer to current AtlasG4EventUserInfo */
AtlasG4EventUserInfo*
iGeant4::ISFG4Helper::getAtlasG4EventUserInfo()
{
  return ( static_cast<AtlasG4EventUserInfo*> (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation()) );
}

