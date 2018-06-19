/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISFTrajectory.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISFTrajectory.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/TruthBinding.h"
#include "ISF_Interfaces/ITruthSvc.h"

// ISF Geant4 includes
#include "ISF_Geant4Event/ISFG4Helper.h"
#include "ISF_Geant4Event/Geant4TruthIncident.h"

// Athena includes
//#include "FadsActions/TrackingAction.h"

// MCTruth includes
#include "MCTruth/EventInformation.h"
#include "MCTruth/TrackInformation.h"
#include "MCTruth/TrackHelper.h"

//HepMC includes
#include "HepMC/GenParticle.h"

#undef _ISFTRAJECTORY_DEBUG_

iGeant4::ISFTrajectory::ISFTrajectory()
  : G4Trajectory()
  , m_truthRecordSvcQuick(nullptr)
{
}

iGeant4::ISFTrajectory::ISFTrajectory(const G4Track* aTrack,
                                      ISF::ITruthSvc* truthSvc)
  : G4Trajectory(aTrack)
  , m_truthRecordSvcQuick(truthSvc)
{
}

iGeant4::ISFTrajectory::~ISFTrajectory()
{

}

void iGeant4::ISFTrajectory::AppendStep(const G4Step* aStep)
{

  // only use truth service if there are new any secondaries
  const int numSecondaries = aStep->GetSecondaryInCurrentStep()->size();
  //const int numSecondaries = aStep->GetNumberOfSecondariesInCurrentStep(); //Once we switch to G4 10.2 or later

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

    // get base ISFParticle
    auto* trackInfo = ISFG4Helper::getISFTrackInfo(*track);
    if (!trackInfo) {
      G4ExceptionDescription description;
      description << G4String("AppendStep: ") + "No VTrackInformation associated with G4Track (trackID: "
                  << track->GetTrackID() << ", track pos: "<<track->GetPosition() << ", mom: "<<track->GetMomentum()
                  << ", parentID " << track->GetParentID() << ")";
      G4Exception("iGeant4::ISFTrajectory", "NoVTrackInformation", FatalException, description);
      return; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
    }

    ISF::ISFParticle* baseIsp = const_cast<ISF::ISFParticle*>( trackInfo->GetBaseISFParticle() );
    if (!baseIsp) {
      G4ExceptionDescription description;
      description << G4String("AppendStep: ") + "NULL ISFParticle pointer for current G4Step (trackID "
                  << track->GetTrackID() << ", track pos: "<<track->GetPosition() << ", mom: "<<track->GetMomentum()
                  << ", parentID " << track->GetParentID() << ", numSec="<<numSecondaries << ")";
      G4Exception("iGeant4::ISFTrajectory", "NoISFParticle", FatalException, description);
      return; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
    }

    AtlasDetDescr::AtlasRegion geoID = baseIsp->nextGeoID();

    auto* eventInfo = ISFG4Helper::getEventInformation();
    iGeant4::Geant4TruthIncident truth(aStep, *baseIsp, geoID, eventInfo);

    if (m_truthRecordSvcQuick) {
      m_truthRecordSvcQuick->registerTruthIncident(truth);

      // read the TrackInformation to determine whether the G4Track was
      // returned to the ISF in this step
      if ( trackInfo->GetReturnedToISF()==true ) {
        // make sure that the TruthBinding of the ISFParticle points to the newest
        // HepMC::GenParticle instance in case it got updated by the
        // ITruthSvc::registerTruthIncident call above
        auto* currentGenPart = eventInfo->GetCurrentlyTraced();
        baseIsp->getTruthBinding()->setTruthParticle( currentGenPart );
        Barcode::ParticleBarcode newBarcode = currentGenPart->barcode();
        baseIsp->setBarcode( newBarcode );
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
