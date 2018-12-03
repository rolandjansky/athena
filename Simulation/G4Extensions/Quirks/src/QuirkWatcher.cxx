/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4TrackStatus.hh"
#include "G4LorentzVector.hh"

#include "Quirk.h"
#include "InfracolorForce.h"
#include "QuirkWatcher.h"

QuirkWatcher::QuirkWatcher() : G4VProcess(G4String("QuirkWatcher")) {
    enableAtRestDoIt = false;
    enableAlongStepDoIt = false;
}

QuirkWatcher::~QuirkWatcher() {}

G4double QuirkWatcher::PostStepGetPhysicalInteractionLength(
    const G4Track&, //track
    G4double, //previousStepSize
    G4ForceCondition* condition
) {
    *condition = StronglyForced;
    return DBL_MAX;
}

G4VParticleChange* QuirkWatcher::PostStepDoIt(
    const G4Track& track,
    const G4Step& //stepData
) {
    // Get infracolor string
    const Quirk* quirkDef = dynamic_cast<const Quirk*>(track.GetParticleDefinition());
    if (quirkDef == 0) {
        G4Exception("QuirkWatcher::PostStepDoIt", "NonQuirk", FatalErrorInArgument, "QuirkWatcher run on non-quirk particle");
    }
    InfracolorForce& string = quirkDef->GetStringIn();

    if (track.GetCurrentStepNumber() > 1 && !string.IsSourceInitialized()) {
        string.Clear();
        string.GetReactionForce()->Clear();
        G4Exception(
            "QuirkWatcher::PostStepDoIt",
            "QuirkMissingPartner",
            EventMustBeAborted,
            "QuirkWatcher: missing partner for quirk"
        );
    }

    // Update track status
    G4TrackStatus stat = track.GetTrackStatus();
    if (stat == fStopButAlive) {
        stat = fAlive;
    }
    if (stat == fAlive || stat == fSuspend) {
        G4bool passControl = !string.HasNextStringVector();
        //G4bool passControl = !string.HasNextStringVector() || (string.GetSumStrings().t() < string.GetReactionForce()->GetSumStrings().t());
        if (passControl) {
            if (string.IsSourceAlive()) {
                stat = fSuspend;
            } else {
                stat = fStopAndKill;
            }
        }
    }
    if (stat == fStopAndKill || stat == fKillTrackAndSecondaries) {
        string.TrackKilled();
    }

    m_particleChange.Initialize(track);
    m_particleChange.ProposeTrackStatus(stat);
    return &m_particleChange;
}
