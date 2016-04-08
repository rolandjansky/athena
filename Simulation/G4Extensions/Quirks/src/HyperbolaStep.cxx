/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HyperbolaStep.h"

#include "HyperbolaStepper.h"
#include "InfracolorForce.h"

#include "G4ElectroMagneticField.hh"
#include "G4Track.hh"
#include "G4DynamicParticle.hh"

HyperbolaStep::HyperbolaStep(const HyperbolaStepper* stepper, InfracolorForce& string, const G4Track& track) :
    m_stepper(stepper),
    m_stringPtr(string.GetStringVectors().begin()),
    m_stringEnd(string.GetStringVectors().end()),
    m_fracLeft(1.0),
    m_position(track.GetPosition()),
    m_time(track.GetGlobalTime()),
    m_momentum(track.GetDynamicParticle()->Get4Momentum()),
    m_stringIn(0,0,0,0),
    m_maxFracTaken(1.0),
    m_stringOut(0,0,0,0),
    m_maxLength(0.0),
    m_length(0.0)
{}

HyperbolaStep::~HyperbolaStep() {}

void HyperbolaStep::PrepareNextStep() {
    if (m_stringPtr == m_stringEnd) {
        // End of string vectors
        m_stringIn.set(0,0,0,0);
        m_stringOut.set(0,0,0,0);
        m_maxLength = m_length;
    } else {
        G4double force = m_stepper->GetForce();
        G4double mass = m_stepper->GetMass();

        // Get incoming string vector
        m_stringIn = (*m_stringPtr) * m_fracLeft;
        m_maxFracTaken = 1.0;

        // Find maximum remaining boost allowed for this step
        G4double gamma0 = std::max(m_momentum * m_stepper->GetStartMomentum() / sqr(mass), 1.0);
        G4double expRapidity0 = gamma0 + std::sqrt(sqr(gamma0) - 1);
        G4double maxExpRapidity = std::max(m_stepper->GetMaxExpRapidity() / expRapidity0, 1.0);

        // Reduce string vector if boost too large
        G4double tmp = 2 * force * m_stringIn.lv() * m_momentum / sqr(mass);
        if (tmp > maxExpRapidity - 1) {
            m_maxFracTaken = (maxExpRapidity - 1) / tmp;
            m_stringIn *= m_maxFracTaken;
        }

        // Reflect string vector and compute maximum step length
        m_stringOut = m_stringIn.reflect(m_momentum + force * m_stringIn.lv());
        m_maxLength = m_length + (m_stringIn.lv() + m_stringOut.lv()).rho();
    }
}

void HyperbolaStep::Step(G4double length) {
    //G4cout << "steplength/maxlength: " << length << " / " << m_maxLength << G4endl;
    G4double force = m_stepper->GetForce();
    G4double mass = m_stepper->GetMass();

    // Cut step to proper length
    G4double fracTaken = m_maxFracTaken;
    if (length < m_maxLength) {
        if (length > m_length) {
            // Calculate fraction of string vector to next-to-leading order in step length
            G4double currMaxLength = m_maxLength - m_length;
            for (G4int i = 0; i < 2 && currMaxLength > 0; i++) {
                G4double r = (length - m_length) / currMaxLength;
                m_stringIn *= r;
                m_stringOut = m_stringIn.reflect(m_momentum + force * m_stringIn.lv());
                fracTaken *= r;
                currMaxLength = (m_stringIn.lv() + m_stringOut.lv()).rho();
            }
        } else {
            m_stringIn.set(0,0,0,0);
            m_stringOut.set(0,0,0,0);
            fracTaken = 0.0;
        }
    }
    m_length = length;

    // Advance string pointer
    m_fracLeft *= (1 - fracTaken);
    if (m_fracLeft <= 0.0) {
        if (m_stringPtr != m_stringEnd) m_stringPtr++;
        m_fracLeft = 1.0;
    }

    // Update position, momentum
    G4LorentzVector dPosition = m_stringIn.lv() + m_stringOut.lv();
    m_position += dPosition.vect();
    G4double dTime = dPosition.t() / CLHEP::c_light;
    m_time += dTime;
    m_momentum += force * (m_stringIn.lv() - m_stringOut.lv());

    // Field
    if (m_stepper->GetField() != 0) {
        G4ThreeVector p = m_position - dPosition.vect()/2;
        G4double point[4] = {p.x(), p.y(), p.z(), m_time - dTime/2};
        G4double field[6] = {0};
        m_stepper->GetField()->GetFieldValue(point, field);
        G4ThreeVector B(field[0], field[1], field[2]);
        G4ThreeVector E(field[3], field[4], field[5]);
        G4double charge = m_stepper->GetCharge();
        G4ThreeVector dMomField = E * dTime + dPosition.vect().cross(B);
        m_momentum += charge * G4LorentzVector(CLHEP::c_light * dMomField, E * dPosition.vect());
    }

    // Renormalize 4-momentum (TODO: address possible instability)
    m_momentum *= mass / m_momentum.m();
}

// Set track variables
void HyperbolaStep::Dump(G4double y[]) {
    y[0] = m_position.x();
    y[1] = m_position.y();
    y[2] = m_position.z();
    y[3] = m_momentum.x();
    y[4] = m_momentum.y();
    y[5] = m_momentum.z();
    y[6] = m_momentum.t() - m_stepper->GetMass();
    y[7] = m_time;
    y[8] = m_length;
}
