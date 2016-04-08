/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HYPERBOLASTEPPER_H
#define HYPERBOLASTEPPER_H

#include <vector>
#include "G4LorentzVector.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4FieldTrack.hh"
#include "InfracolorForce.h"
#include "HyperbolaStep.h"

class G4Track;
class G4Field;
class G4ElectroMagneticField;

class HyperbolaStepper : public G4MagIntegratorStepper {
public:
    HyperbolaStepper(
        InfracolorForce& string,
        const G4Track& track,
        const G4Field* field = 0
    );
    virtual ~HyperbolaStepper();

    virtual void Stepper(
        const G4double y[],
        const G4double[], //dydx
        G4double h,
        G4double yout[],
        G4double yerr[]
    );
    virtual G4double DistChord() const;
    virtual void ComputeRightHandSide(const G4double[], G4double[]) {}
    virtual G4int IntegratorOrder() const {return 1;}

    G4double GetForce() const;
    const G4ElectroMagneticField* GetField() const;
    G4double GetMass() const;
    G4double GetCharge() const;
    const G4LorentzVector& GetStartMomentum() const;
    G4double GetMaxExpRapidity() const;

    G4double GetMaxLength() const;
    void Update(G4FieldTrack& fieldTrack, G4bool forceStep);

    void SetDebug(G4bool debug);

private:
    static const G4int m_NUM_VARS = 9;

    InfracolorForce& m_string;
    const G4ElectroMagneticField* const m_field;
    const G4double m_mass;
    const G4double m_charge;
    const G4LorentzVector m_startMomentum;
    const G4double m_maxExpRapidity;

    std::vector<HyperbolaStep> m_steps;
    HyperbolaStep m_currStep;
    std::vector<HyperbolaStep>::size_type m_nPrevSteps;

    G4bool m_debug;

    void SetCurrStep(G4double length);
};

inline G4double HyperbolaStepper::GetForce() const {return m_string.GetStringForce();}
inline const G4ElectroMagneticField* HyperbolaStepper::GetField() const {return m_field;}
inline G4double HyperbolaStepper::GetMass() const {return m_mass;}
inline G4double HyperbolaStepper::GetCharge() const {return m_charge;}
inline const G4LorentzVector& HyperbolaStepper::GetStartMomentum() const {return m_startMomentum;}
inline G4double HyperbolaStepper::GetMaxExpRapidity() const {return m_maxExpRapidity;}

inline G4double HyperbolaStepper::GetMaxLength() const {return m_steps.back().GetMaxLength();}

inline void HyperbolaStepper::SetDebug(G4bool debug) {m_debug = debug;}

#endif
