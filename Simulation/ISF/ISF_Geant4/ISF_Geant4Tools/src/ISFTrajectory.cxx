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

  bool debug=false;

  // only use truth service if there are new any secondaries
  int numSecondaries = m_sHelper.NrOfNewSecondaries();

  if (numSecondaries) {

    G4Track* track=aStep->GetTrack();
    if (debug) {
      std::cout << "A new track "
                << " (trackID " << track->GetTrackID()
                << "), track pos: "<<track->GetPosition()
                << ", mom: "<<track->GetMomentum()
                << ", parentID " << track->GetParentID()
                << ", numSec="<<numSecondaries<<" is in AppendStep." << std::endl;
    }

    // OK, there was an interaction. look at the track, if it
    // is not a secondary (i.e. we have a connected tree) we
    // apply the MC truth machinery...
    TrackHelper tHelper(track);
    if (tHelper.IsSecondary()) { if (debug) std::cout<<"is secondary, returning"<<std::endl; return; }

    AtlasDetDescr::AtlasRegion geoID = AtlasDetDescr::fUndefinedAtlasRegion;

    // get parent particle
    TrackInformation* trackInfo = static_cast<TrackInformation*>(aStep->GetTrack()->GetUserInformation());
    if (!trackInfo) {  std::cerr<<"ISFTrajectory::AppendStep ERROR NULL TrackInformation pointer!"<<std::endl; }

    const ISF::ISFParticle* parent = (trackInfo) ? trackInfo->GetISFParticle() : 0;
    assert(parent); //FIXME: assert only works in dbg builds of Athena

    geoID=parent->nextGeoID();

    ISF::Geant4TruthIncident truth( aStep, geoID, numSecondaries, m_sHelper);

    if (m_truthRecordSvcQuick) {
      m_truthRecordSvcQuick->registerTruthIncident(truth);
    }
    else {
      std::cerr<<"no truth record svc!!!"<<std::endl;
      exit(3);
    }
  }

  G4Trajectory::AppendStep(aStep);
}
