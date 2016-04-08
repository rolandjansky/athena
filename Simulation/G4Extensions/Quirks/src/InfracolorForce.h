/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INFRACOLORFORCE_H
#define INFRACOLORFORCE_H

#include <deque>
#include "G4LorentzVector.hh"
#include "StringVector.h"

class G4Track;

class InfracolorForce {
public:
    InfracolorForce();
    ~InfracolorForce();
    void SetReactionForce(InfracolorForce* reactionForce);
    InfracolorForce* GetReactionForce() const;
    void SetFirstStringLength(G4double firstStringLength);
    void StartTracking(const G4Track* dest);
    void TrackKilled();
    G4bool IsSourceAlive() const;
    G4bool IsSourceInitialized() const;
    G4bool HasNextStringVector() const;
    const std::deque<StringVector>& GetStringVectors() const;
    void PopTo(std::deque<StringVector>::const_iterator stringPtr, G4double fracLeft);
    void PushStringVector(const StringVector& v);
    void SetStringForce(G4double stringForce);
    G4double GetStringForce() const;
    void SetMaxBoost(G4double maxBoost);
    G4double GetMaxExpRapidity() const;
    void SetMaxMergeT(G4double maxMergeT);
    void SetMaxMergeMag(G4double maxMergeMag);
    G4int GetNStrings() const;
    G4LorentzVector GetSumStrings() const;
    G4ThreeVector GetAngMomentum() const;
    G4ThreeVector GetMomentOfE() const;
    void Clear();

private:
    void CombineStringVector(const StringVector& v);

    G4double m_stringForce;             // strength of string force
    InfracolorForce* m_reactionForce;   // string vectors moving the other direction, creating reaction force on the other quirk
    G4LorentzVector m_initU;            // initial 4-velocity of quirk
    G4bool m_initialized;               // this track has been set up with m_initU
    G4bool m_killed;                    // this track has been killed
    G4bool m_firstStep;                 // this is the first step and m_firstString must be used
    std::deque<StringVector> m_stringVectors;  // string vectors moving towards tracked quirk, each carrying momentum of F_string * value
    StringVector m_firstString;         // string vector used to take first step
    StringVector m_borrowedString;      // portion of m_firstString used, to be subtracted from next vector pushed
    G4double m_firstStringLength;       // length of string vector to be used for first step
    G4double m_maxExpRapidity;          // maximum exp(rapidity) of boost to be allowed in a single step
    G4double m_maxMergeT;
    G4double m_maxMergeMag;
};

inline void InfracolorForce::SetReactionForce(InfracolorForce* reactionForce) {
    m_reactionForce = reactionForce;
}

inline InfracolorForce* InfracolorForce::GetReactionForce() const {
    return m_reactionForce;
}

inline void InfracolorForce::SetFirstStringLength(G4double firstStringLength) {
    m_firstStringLength = firstStringLength;
    m_reactionForce->m_firstStringLength = firstStringLength;
}

inline G4bool InfracolorForce::IsSourceAlive() const {
    return !m_reactionForce->m_killed;
}

inline G4bool InfracolorForce::IsSourceInitialized() const {
    return m_reactionForce->m_initialized;
}

inline G4bool InfracolorForce::HasNextStringVector() const {
    return !m_stringVectors.empty();
}

inline const std::deque<StringVector>& InfracolorForce::GetStringVectors() const {
    return m_stringVectors;
}

inline void InfracolorForce::SetStringForce(G4double stringForce) {
    m_stringForce = stringForce;
    m_reactionForce->m_stringForce = stringForce;
}

inline G4double InfracolorForce::GetStringForce() const {
    return m_stringForce;
}

inline void InfracolorForce::SetMaxBoost(G4double maxBoost) {
    m_maxExpRapidity = std::sqrt((1.0 + maxBoost) / (1.0 - maxBoost));
    m_reactionForce->m_maxExpRapidity = m_maxExpRapidity;
}

inline G4double InfracolorForce::GetMaxExpRapidity() const {
    return m_maxExpRapidity;
}

inline void InfracolorForce::SetMaxMergeT(G4double maxMergeT) {
    m_maxMergeT = maxMergeT;
    m_reactionForce->m_maxMergeT = maxMergeT;
}

inline void InfracolorForce::SetMaxMergeMag(G4double maxMergeMag) {
    m_maxMergeMag = maxMergeMag;
    m_reactionForce->m_maxMergeMag = maxMergeMag;
}

inline G4int InfracolorForce::GetNStrings() const {
    return m_stringVectors.size();
}

#endif
