/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4Track.hh"
#include "G4DynamicParticle.hh"
#include "G4ElectroMagneticField.hh"

#include "DummyEquation.h"
#include "HyperbolaStepper.h"

HyperbolaStepper::HyperbolaStepper(
    InfracolorForce& string,
    const G4Track& track,
    const G4Field* field
) :
    // 9 "integration" variables:
    // x, y, z, px, py, pz, KE, t_lab, steplength (in place of t_proper)
    G4MagIntegratorStepper(new DummyEquation(), m_NUM_VARS),
    m_string(string),
    m_field(dynamic_cast<const G4ElectroMagneticField*>(field)),
    m_mass(track.GetDynamicParticle()->GetMass()),
    m_charge(track.GetDynamicParticle()->GetCharge()),
    m_startMomentum(track.GetDynamicParticle()->Get4Momentum()),
    m_maxExpRapidity(string.GetMaxExpRapidity()),
    m_steps(),
    m_currStep(this, string, track),
    m_nPrevSteps(0),
    m_debug(false)
{
    if (field != 0 && m_field == 0) {
        G4Exception("HyperbolaStepper::HyperbolaStepper", "QuirkNonEM", FatalErrorInArgument, "Non-EM fields not supported");
    }

    // Precompute steps
    m_currStep.PrepareNextStep();
    m_steps.push_back(m_currStep);
    m_currStep.Step(m_currStep.GetMaxLength());
    while (m_currStep.GetStringPtr() != string.GetStringVectors().end()) {
        HyperbolaStep next = m_currStep;
        next.PrepareNextStep();
        if (next.IsBoostLimited()) break; // don't allow 2nd+ step if it would be cut by boost limit
        m_steps.push_back(next);
        m_currStep = next;
        m_currStep.Step(m_currStep.GetMaxLength());
    }
    m_nPrevSteps = m_steps.size() - 1;
}

HyperbolaStepper::~HyperbolaStepper() {
    delete GetEquationOfMotion();
}

void HyperbolaStepper::SetCurrStep(G4double length) {
    if (m_currStep.GetLength() == length) return;
    std::vector<HyperbolaStep>::size_type i = 0;
    while (i+1 < m_steps.size() && m_steps[i+1].GetLength() < length) i++;
    m_currStep = m_steps[i];
    m_currStep.Step(length);
    m_nPrevSteps = i;
}

void HyperbolaStepper::Stepper(
    const G4double y[],
    const G4double[], //dydx
    G4double h,
    G4double yout[],
    G4double yerr[]
) {
    G4int maxvar = GetNumberOfStateVariables();
    for (G4int i = 0; i < maxvar; i++) yout[i] = y[i];

    if (m_debug) G4cout << "HyperbolaStepper: asked to move " << h << G4endl;
    if (m_debug) G4cout << "HyperbolaStepper: start = " << y[0] << ", " << y[1] << ", " << y[2] << " [" << y[8] << "]" << G4endl;

    SetCurrStep(y[8] + h);
    m_currStep.Dump(yout);

    if (m_debug) G4cout << "HyperbolaStepper: end   = " << yout[0] << ", " << yout[1] << ", " << yout[2] << " [" << yout[8] << "]" << G4endl;

    // TODO: Errors
    for (G4int i = 0; i < m_NUM_VARS; i++) yerr[i] = 0;
}

G4double HyperbolaStepper::DistChord() const {
    // TODO
    return 0;
}

void HyperbolaStepper::Update(G4FieldTrack& fieldTrack, G4bool forceStep) {
    if (forceStep) {
        // Propagator refused to take step through detector geometry because length too short;
        // take step ourselves and update string state and quirk 4-momentum only (don't change position)
        m_currStep = m_steps.back();
        m_currStep.Step(m_currStep.GetMaxLength());
        m_nPrevSteps = m_steps.size() - 1;
        G4LorentzVector p = m_currStep.GetMomentum();
        fieldTrack.UpdateFourMomentum(p.t() - m_mass, p.vect().unit());
    } else {
        SetCurrStep(fieldTrack.GetProperTimeOfFlight());
    }

    // Update string vectors
    m_string.PopTo(m_currStep.GetStringPtr(), m_currStep.GetFracLeft());
    for (std::vector<HyperbolaStep>::size_type i = 0; i < m_nPrevSteps; i++) {
        if (m_debug) G4cout << "HyperbolaStepper: pushing vector " << m_steps[i].GetStringOut().lv() << G4endl;
        m_string.GetReactionForce()->PushStringVector(m_steps[i].GetStringOut());
    }
    if (m_debug) G4cout << "HyperbolaStepper: pushing vector " << m_currStep.GetStringOut().lv() << G4endl;
    m_string.GetReactionForce()->PushStringVector(m_currStep.GetStringOut());
}
