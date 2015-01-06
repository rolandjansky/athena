/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ISF includes
#include "ISF_Geant4Tools/ISFTrajectory.h"
#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Event/ISFParticle.h"
#include "ISF_Geant4Tools/Geant4TruthIncident.h"

#include "FadsActions/TrackingAction.h"


// G4 includes

#include "MCTruth/TrackInformation.h"
#include "MCTruth/TrackHelper.h"

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

    AtlasDetDescr::AtlasRegion geoID = AtlasDetDescr::fUndefinedAtlasRegion;

    // get parent particle
    TrackInformation* trackInfo = static_cast<TrackInformation*>(aStep->GetTrack()->GetUserInformation());
    if (!trackInfo) {  std::cerr<<"ISFTrajectory::AppendStep ERROR NULL TrackInformation pointer!"<<std::endl; }

    const ISF::ISFParticle* parent = (trackInfo) ? trackInfo->GetISFParticle() : 0;
    if (!parent) {
      G4ExceptionDescription description;
      description << G4String("AppendStep: ") + "NULL ISFParticle pointer for current G4Step (trackID "
                  << track->GetTrackID() << ", track pos: "<<track->GetPosition() << ", mom: "<<track->GetMomentum()
                  << ", parentID " << track->GetParentID() << ", numSec="<<numSecondaries << ")";
      G4Exception("iGeant4::ISFTrajectory", "NoISFParticle", FatalException, description);
      return; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
    }

    geoID=parent->nextGeoID();

    ISF::Geant4TruthIncident truth( aStep, geoID, numSecondaries, m_sHelper);

    if (m_truthRecordSvcQuick) {
      m_truthRecordSvcQuick->registerTruthIncident(truth);
    }
    else {
      G4ExceptionDescription description;
      description << G4String("AppendStep: ") + "m_truthRecordSvcQuick is NULL!";
      G4Exception("iGeant4::ISFTrajectory", "NoTruthRecordSvc", FatalException, description);
    }
  }

  G4Trajectory::AppendStep(aStep);
}
