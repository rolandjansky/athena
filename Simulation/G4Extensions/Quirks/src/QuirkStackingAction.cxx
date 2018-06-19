/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "QuirkStackingAction.h"

#include "Quirk.h"

#include "G4Track.hh"

QuirkStackingAction::QuirkStackingAction(G4UserStackingAction* defaultStackingAction):
    m_firstTrack(true),
    m_defaultStackingAction(defaultStackingAction)
{}

QuirkStackingAction::~QuirkStackingAction() {
    delete m_defaultStackingAction;
}

G4ClassificationOfNewTrack QuirkStackingAction::ClassifyNewTrack(const G4Track* aTrack) {
    G4ClassificationOfNewTrack defaultClassification = fUrgent;
    if (m_defaultStackingAction) {
        m_defaultStackingAction->SetStackManager(stackManager);
        defaultClassification = m_defaultStackingAction->ClassifyNewTrack(aTrack);
    }
    const Quirk* quirkDef = dynamic_cast<const Quirk*>(aTrack->GetParticleDefinition());
    if (quirkDef == 0 || (defaultClassification != fUrgent && defaultClassification != fWaiting)) {
        return defaultClassification;
    } else if (m_firstTrack) {
        m_firstTrack = false;
        return fWaiting;
    } else {
        return fWaiting_1;
    }
}

void QuirkStackingAction::NewStage() {
    if (m_defaultStackingAction) {
        m_defaultStackingAction->SetStackManager(stackManager);
        m_defaultStackingAction->NewStage();
    }
}

void QuirkStackingAction::PrepareNewEvent() {
    if (m_defaultStackingAction) {
        m_defaultStackingAction->SetStackManager(stackManager);
        m_defaultStackingAction->PrepareNewEvent();
    }
    m_firstTrack = true;
}
