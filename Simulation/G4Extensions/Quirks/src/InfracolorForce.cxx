/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InfracolorForce.h"

#include "G4Track.hh"
#include "G4DynamicParticle.hh"

InfracolorForce::InfracolorForce() :
    m_stringForce(0.0),
    m_reactionForce(0),
    m_initU(),
    m_initialized(false),
    m_killed(false),
    m_firstStep(false),
    m_stringVectors(),
    m_firstString(),
    m_borrowedString(),
    m_firstStringLength(1*CLHEP::nm),
    m_maxExpRapidity(1e-3),
    m_maxMergeT(0.0),
    m_maxMergeMag(0.0)
{}

InfracolorForce::~InfracolorForce() {}

void InfracolorForce::StartTracking(const G4Track* dest) {
    if (m_reactionForce == 0) {
        G4Exception("InfracolorForce::StartTracking", "NoAntiQuirk", FatalErrorInArgument, "InfracolorForce: No antiquirk defined");
    }
    if (dest->GetCurrentStepNumber() != 0) return;

    // Clear old stuff
    if (m_initialized && m_reactionForce->m_initialized) {
        Clear();
        m_reactionForce->Clear();
    }

    // Set momentum
    m_initialized = true;
    m_initU = dest->GetDynamicParticle()->Get4Momentum();
    m_initU /= m_initU.m();

    // Compute first string vector
    if (m_reactionForce->m_initialized) {
        m_firstStep = true;
        G4double dot = m_initU * m_reactionForce->m_initU;
        G4LorentzVector firstString = m_reactionForce->m_initU - m_initU * (dot - std::sqrt(dot*dot - 1));
        firstString *= m_firstStringLength / firstString.t();
        m_firstString.set(firstString.vect(), 0);
        m_stringVectors.push_back(m_firstString);
        // TODO: Check oscillation period
    }
}

void InfracolorForce::Clear() {
    m_initU.set(0,0,0,0);
    m_initialized = false;
    m_killed = false;
    m_firstStep = false;
    m_stringVectors.clear();
    m_firstString.set(0,0,0,0);
    m_borrowedString.set(0,0,0,0);
}

void InfracolorForce::TrackKilled() {
    //G4cout << "killed, and other killed is " << m_reactionForce->m_killed << G4endl;
    m_killed = true;
}

void InfracolorForce::PopTo(std::deque<StringVector>::const_iterator stringPtr, G4double fracLeft) {
    if (fracLeft > 1.0 || fracLeft < 0.0) {
        Clear();
        m_reactionForce->Clear();
        G4Exception("InfracolorForce::PopTo", "QuirkStringBadFraction", EventMustBeAborted, "invalid fraction of string vector");
    }
    if (m_firstStep) {
        if (stringPtr == m_stringVectors.begin()) {
            m_borrowedString = (1 - fracLeft) * m_firstString;
        } else {
            m_borrowedString = m_firstString;
        }
        m_firstStep = false;
        m_stringVectors.clear();
    } else {
        std::deque<StringVector>::iterator stringPtr2 = m_stringVectors.begin() + (stringPtr - m_stringVectors.begin());
        m_stringVectors.erase(m_stringVectors.begin(), stringPtr2);
        if (fracLeft != 1.0 && !m_stringVectors.empty()) {
            m_stringVectors[0] *= fracLeft;
        }
    }
}

void InfracolorForce::PushStringVector(const StringVector& v) {
    if (v.t() == 0) return;
    if (m_borrowedString.t() == 0) {
        CombineStringVector(v);
    } else if (m_borrowedString.t() <= v.t()) {
        G4double r = m_borrowedString.t() / v.t();
        CombineStringVector((1-r) * v);
        m_borrowedString.set(0,0,0,0);
    } else {
        G4double r = v.t() / m_borrowedString.t();
        m_borrowedString *= 1-r;
        G4Exception("InfracolorForce::PushStringVector()",
                    "BorrowedStringSplit", JustWarning,
                    "Initial step too long.");
    }
}

void InfracolorForce::CombineStringVector(const StringVector& v) {
    if (m_stringVectors.empty()) {
        m_stringVectors.push_back(v);
    } else {
        G4LorentzVector sum = m_stringVectors.back().lv() + v.lv();
        if (sum.t() < m_maxMergeT && sum.m2() < sqr(m_maxMergeMag)) {
            m_stringVectors.back().set(sum.vect(), sum.m());
        } else {
            m_stringVectors.push_back(v);
        }
    }
}

G4LorentzVector InfracolorForce::GetSumStrings() const {
    G4LorentzVector x(0,0,0,0);
    std::deque<StringVector>::const_iterator it = m_stringVectors.begin();
    for (; it != m_stringVectors.end(); it++) {
        x += it->lv();
    }
    return x - m_borrowedString.lv();
}

G4ThreeVector InfracolorForce::GetAngMomentum() const {
    std::deque<StringVector>::const_iterator it = m_stringVectors.begin();
    G4LorentzVector x(0,0,0,0);
    G4ThreeVector L(0,0,0);
    for (; it != m_stringVectors.end(); it++) {
        G4LorentzVector dx = it->lv();
        L += m_stringForce * x.vect().cross(dx.vect());
        x += dx;
    }
    return L;
}

G4ThreeVector InfracolorForce::GetMomentOfE() const {
    std::deque<StringVector>::const_iterator it = m_stringVectors.begin();
    G4LorentzVector x(0,0,0,0);
    G4ThreeVector Excm(0,0,0);
    for (; it != m_stringVectors.end(); it++) {
        G4LorentzVector dx = it->lv();
        Excm += m_stringForce * (dx.t() * x.vect() - dx.vect() * x.t());
        x += dx;
    }
    return Excm;
}
