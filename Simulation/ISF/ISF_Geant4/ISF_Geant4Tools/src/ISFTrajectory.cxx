/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISFTrajectory.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISFTrajectory.h"

// package includes
#include "Geant4TruthIncident.h"

// ISF includes
#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Event/ISFParticle.h"
#include "ISF_HepMC_Event/HepMC_TruthBinding.h"


// Athena includes
//#include "FadsActions/TrackingAction.h"

#include "MCTruth/TrackInformation.h"
#include "MCTruth/TrackHelper.h"

//HepMC includes
#include "HepMC/GenParticle.h"

#undef _ISFTRAJECTORY_DEBUG_

iGeant4::ISFTrajectory::ISFTrajectory()
  : G4Trajectory()
  , m_truthRecordSvcQuick(0)
{
}

iGeant4::ISFTrajectory::ISFTrajectory(const G4Track* aTrack,
                                      ISF::ITruthSvc* truthSvc)
  : G4Trajectory(aTrack)
    //, m_sHelper(FADS::FadsTrackingAction::GetTrackingAction()->GetTrackingManager())
  , m_truthRecordSvcQuick(truthSvc)
{
}

iGeant4::ISFTrajectory::~ISFTrajectory()
{

}

void iGeant4::ISFTrajectory::AppendStep(const G4Step* aStep)
{

  // only use truth service if there are new any secondaries
  int numSecondaries = m_sHelper.NrOfNewSecondaries();

  if (numSecondaries) {

    G4Track* track=aStep->GetTrack();
#ifdef _ISFTRAJECTORY_DEBUG_
    std::cout << "A new track "
              << " (trackID " << track->GetTrackID()
              << "), track pos: "<<track->GetPosition()
              << ", mom: "<<track->GetMomentum()
              << ", parentID " << track->GetParentID()
              << ", numSec="<<numSecondaries<<" is in AppendStep." << std::endl;
#endif
    // OK, there was an interaction. look at the track, if it
    // is not a secondary (i.e. we have a connected tree) we
    // apply the MC truth machinery...
    TrackHelper tHelper(track);
    if (tHelper.IsSecondary()) {
#ifdef _ISFTRAJECTORY_DEBUG_
      std::cout<<"is secondary, returning"<<std::endl;
#endif
      return;
    }

    // get parent particle
    TrackInformation* trackInfo = static_cast<TrackInformation*>(track->GetUserInformation());
    if (!trackInfo) {  std::cerr<<"ISFTrajectory::AppendStep ERROR NULL TrackInformation pointer!"<<std::endl; }

    ISF::ISFParticle* parent = const_cast<ISF::ISFParticle*>( (trackInfo) ? trackInfo->GetISFParticle() : 0 );
    if (!parent) {
      G4ExceptionDescription description;
      description << G4String("AppendStep: ") + "NULL ISFParticle pointer for current G4Step (trackID "
                  << track->GetTrackID() << ", track pos: "<<track->GetPosition() << ", mom: "<<track->GetMomentum()
                  << ", parentID " << track->GetParentID() << ", numSec="<<numSecondaries << ")";
      G4Exception("iGeant4::ISFTrajectory", "NoISFParticle", FatalException, description);
      return; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
    }

    AtlasDetDescr::AtlasRegion geoID = parent->nextGeoID();

    ISF::Geant4TruthIncident truth( aStep, geoID, numSecondaries, m_sHelper);

    if (m_truthRecordSvcQuick) {
      m_truthRecordSvcQuick->registerTruthIncident(truth);

      // read the TrackInformation to determine whether the G4Track was
      // returned to the ISF in this step
      if ( trackInfo && trackInfo->GetReturnedToISF()==true ) {
        // make sure that the TruthBinding of the ISFParticle points to the newest
        // HepMC::GenParticle instance
        HepMC::GenParticle *newGenPart = truth.parentParticleAfterIncident(Barcode::fUndefinedBarcode, false);
        ISF::ITruthBinding *newTruthBinding = new ISF::HepMC_TruthBinding(*newGenPart);
        parent->setTruthBinding( newTruthBinding );
        Barcode::ParticleBarcode newBarcode = newGenPart->barcode();
        parent->setBarcode( newBarcode );
      }
    }
    else {
      G4ExceptionDescription description;
      description << G4String("AppendStep: ") + "m_truthRecordSvcQuick is NULL!";
      G4Exception("iGeant4::ISFTrajectory", "NoTruthRecordSvc", FatalException, description);
    }
  }

  G4Trajectory::AppendStep(aStep);
}
