/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUIRK_H
#define QUIRK_H

#include "G4ParticleDefinition.hh"

class InfracolorForce;

class Quirk : public G4ParticleDefinition {
public:
    Quirk(
        const G4String&     aName,        G4double            mass,
        G4double            width,        G4double            charge,   
        G4int               iSpin,        G4int               iParity,   
        G4int               iConjugation, G4int               iIsospin,   
        G4int               iIsospin3,    G4int               gParity,
        const G4String&     pType,        G4int               lepton,     
        G4int               baryon,       G4int               encoding,
        G4bool              stable,       G4double            lifetime,
        G4DecayTable        *decaytable,  G4bool              shortlived,
        const G4String&     subType = "",
        G4int               anti_encoding = 0,
        G4double            magneticMoment = 0.0
    );
    ~Quirk();
    InfracolorForce& GetStringIn() const;

private:
    InfracolorForce* m_stringIn;
};

/** Force on this quirk from its partner */
inline InfracolorForce& Quirk::GetStringIn() const {
    return *m_stringIn;
}

#endif
