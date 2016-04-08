/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4ParticleTable.hh"
#include "InfracolorForce.h"
#include "Quirk.h"

Quirk::Quirk(
    const G4String&     aName,        G4double            mass,
    G4double            width,        G4double            charge,   
    G4int               iSpin,        G4int               iParity,   
    G4int               iConjugation, G4int               iIsospin,   
    G4int               iIsospin3,    G4int               gParity,
    const G4String&     pType,        G4int               lepton,     
    G4int               baryon,       G4int               encoding,
    G4bool              stable,       G4double            lifetime,
    G4DecayTable        *decaytable,  G4bool              shortlived,
    const G4String&     subType,
    G4int               anti_encoding,
    G4double            magneticMoment
) :
    G4ParticleDefinition(
	aName,mass,width,charge,iSpin,iParity,
        iConjugation,iIsospin,iIsospin3,gParity,pType,
        lepton,baryon,encoding,stable,lifetime,decaytable,
        shortlived,subType,anti_encoding,magneticMoment
    ),
    m_stringIn(new InfracolorForce())
{
    G4ParticleTable* particleTable = GetParticleTable();
    G4ParticleDefinition* partner = particleTable->FindParticle(GetAntiPDGEncoding());
    if (partner != 0) {
        Quirk* qpartner = dynamic_cast<Quirk*>(partner);
        if (qpartner == 0) G4Exception("Quirk::Quirk", "NonQuirkAntiparticle", FatalErrorInArgument, "Antiparticle of quirk must be a quirk");
        m_stringIn->SetReactionForce(qpartner->m_stringIn);
        qpartner->m_stringIn->SetReactionForce(m_stringIn);
    }
}

Quirk::~Quirk() {
    delete m_stringIn;
}
