/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HYPERBOLASTEP_H
#define HYPERBOLASTEP_H

#include <deque>
#include "G4ThreeVector.hh"
#include "G4LorentzVector.hh"
#include "StringVector.h"

class G4Track;
class HyperbolaStepper;
class InfracolorForce;

class HyperbolaStep {
public:
    HyperbolaStep(const HyperbolaStepper* stepper, InfracolorForce& string, const G4Track& track);
    ~HyperbolaStep();

    void PrepareNextStep();
    void Step(G4double length);
    void Dump(G4double y[]);

    HyperbolaStep* GetPrev() const;
    std::deque<StringVector>::const_iterator GetStringPtr() const;
    G4double GetFracLeft() const;
    G4bool IsBoostLimited() const;
    G4double GetMaxLength() const;
    G4double GetLength() const;
    const StringVector& GetStringOut() const;
    const G4LorentzVector& GetMomentum() const;

private:
    const HyperbolaStepper* m_stepper;

    std::deque<StringVector>::const_iterator m_stringPtr;
    std::deque<StringVector>::const_iterator m_stringEnd;
    G4double m_fracLeft;
    G4ThreeVector m_position;
    G4double m_time;
    G4LorentzVector m_momentum;

    StringVector m_stringIn;
    G4double m_maxFracTaken;
    StringVector m_stringOut;
    G4double m_maxLength;
    G4double m_length;
};

inline std::deque<StringVector>::const_iterator HyperbolaStep::GetStringPtr() const {return m_stringPtr;}
inline G4double HyperbolaStep::GetFracLeft() const {return m_fracLeft;}
inline G4bool HyperbolaStep::IsBoostLimited() const {return m_maxFracTaken != 1.0;}
inline G4double HyperbolaStep::GetMaxLength() const {return m_maxLength;}
inline G4double HyperbolaStep::GetLength() const {return m_length;}
inline const StringVector& HyperbolaStep::GetStringOut() const {return m_stringOut;}
inline const G4LorentzVector& HyperbolaStep::GetMomentum() const {return m_momentum;}

#endif
