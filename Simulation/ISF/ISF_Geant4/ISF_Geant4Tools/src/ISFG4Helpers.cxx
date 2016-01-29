/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////
// ISFG4Helpers.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////

// class header
#include "ISFG4Helpers.h"

// Geant4 includes
#include "G4Track.hh"
#include "G4ThreeVector.hh"

// G4Atlas includes
#include "MCTruth/VTrackInformation.h"
#include "MCTruth/TrackBarcodeInfo.h"
#include "MCTruth/TrackInformation.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"

// ATLAS GeoPrimitves (Amg::Vector3D etc)
#include "GeoPrimitives/GeoPrimitives.h"


/** empty dummy constructor */
iGeant4::ISFG4Helpers::ISFG4Helpers()
{

}

/** empty dummy destructor */
iGeant4::ISFG4Helpers::~ISFG4Helpers()
{

}

/** convert the given G4Track into an ISFParticle */
ISF::ISFParticle*
iGeant4::ISFG4Helpers::convertG4TrackToISFParticle(const G4Track& aTrack,
                                                   const ISF::ISFParticle& parent,
                                                   ISF::ITruthBinding* truth)
{
  const ISF::DetRegionSvcIDPair regionSimSvcPair( parent.nextGeoID(), parent.nextSimID() );
  return ISFG4Helpers::convertG4TrackToISFParticle( aTrack,
                                                    regionSimSvcPair,
                                                    parent.getPrimaryTruthParticle(),
                                                    truth );
}

/** convert the given G4Track into an ISFParticle */
ISF::ISFParticle*
iGeant4::ISFG4Helpers::convertG4TrackToISFParticle(const G4Track& aTrack,
                                                   const ISF::DetRegionSvcIDPair& regionSimSvcPair,
                                                   const HepMC::GenParticle* primaryTruthParticle,
                                                   ISF::ITruthBinding* truth)
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

  Barcode::ParticleBarcode barcode = ISFG4Helpers::getParticleBarcode(aTrack);

  ISF::ISFParticle *isp = new ISF::ISFParticle( position,
                                                momentum,
                                                mass,
                                                charge,
                                                pdgID,
                                                gTime,
                                                regionSimSvcPair,
                                                primaryTruthParticle,
                                                barcode,
                                                truth
                                               );

  return isp;
}


/** get the ParticleBarcode corresponding to the given G4Track */
Barcode::ParticleBarcode
iGeant4::ISFG4Helpers::getParticleBarcode(const G4Track& aTrack)
{
  VTrackInformation* trackInfo = getISFTrackInfo(aTrack);

  Barcode::ParticleBarcode barcode = (trackInfo) ? trackInfo->GetParticleBarcode() : Barcode::fUndefinedBarcode;

  return barcode;
}


/** return a valid UserInformation object of the G4Track for use within the ISF */
VTrackInformation *
iGeant4::ISFG4Helpers::getISFTrackInfo(const G4Track& aTrack) 
{
  VTrackInformation* trackInfo = static_cast<VTrackInformation*>(aTrack.GetUserInformation());
  bool isValidInISF = trackInfo && (trackInfo->GetPrimaryHepMCParticle() || dynamic_cast<TrackBarcodeInfo*>(trackInfo));

  return ( (isValidInISF) ? trackInfo : 0 );
}

/** link the given G4Track to the given ISFParticle */
void
iGeant4::ISFG4Helpers::setG4TrackInfoFromBaseISFParticle( G4Track& aTrack,
                                                          const ISF::ISFParticle& baseIsp,
                                                          bool setReturnToISF )
{
  G4VUserTrackInformation* g4TrackInfo = aTrack.GetUserInformation();
  VTrackInformation*       trackInfo   = dynamic_cast<VTrackInformation*>( g4TrackInfo );


  if (!trackInfo) {
    // create and attach a new TrackInformation object to the G4Track

    // NB: using a lightweight TrackBarcodeInfo instead of TrackInformation
    //     possibly leads to an output change in the SiHitCollection_p2_BCMHits.m_id
    HepMC::GenParticle* hepParticle = nullptr;
    trackInfo = new TrackInformation( hepParticle, &baseIsp );
    trackInfo->SetClassification( Secondary );
    aTrack.SetUserInformation( trackInfo );
  }

  auto hepPrimary = baseIsp.getPrimaryTruthParticle();
  trackInfo->SetBaseISFParticle( &baseIsp );
  trackInfo->SetPrimaryHepMCParticle( hepPrimary );

  bool returnToISF = setReturnToISF | trackInfo->GetReturnedToISF();
  trackInfo->SetReturnedToISF( returnToISF );

  return;
}
