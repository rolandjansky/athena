/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EventInformation_H
#define EventInformation_H

#include "HepMC/GenEvent.h"
#include "G4ThreeVector.hh"
#include "G4VUserEventInformation.hh"

class EventInformation: public G4VUserEventInformation {
public:
        EventInformation(): G4VUserEventInformation(),m_nrOfPrimaryParticles(0),
                        m_nrOfPrimaryVertices(0),m_secondaryParticleBarCode(200000),
                        m_secondaryVertexBarCode(-200000),m_theEvent(0),
                        m_currentPrimary(0),m_currentlyTraced(0) {}
        HepMC::GenEvent* GetHepMCEvent() ;
        void SetHepMCEvent(HepMC::GenEvent*);
        int GetNrOfPrimaryParticles() const;
        void SetNrOfPrimaryParticles(int nr);
        int GetNrOfPrimaryVertices() const;
        void SetNrOfPrimaryVertices(int nr);
        void SetVertexPosition(const G4ThreeVector);
        const G4ThreeVector GetVertexPosition() const;
        void Print() const {}

        void SetCurrentPrimary(HepMC::GenParticle *p) {m_currentPrimary=p;}

        void SetCurrentlyTraced(HepMC::GenParticle *p) {m_currentlyTraced=p;}

        HepMC::GenParticle *GetCurrentPrimary() const {return m_currentPrimary;}

        HepMC::GenParticle *GetCurrentlyTraced() const {return m_currentlyTraced;}
        int SecondaryParticleBarCode() {m_secondaryParticleBarCode++;
                                        return m_secondaryParticleBarCode;}
        int SecondaryVertexBarCode() {m_secondaryVertexBarCode--;
                                      return m_secondaryVertexBarCode;}

private:
        G4ThreeVector m_vertexPosition;
        int m_nrOfPrimaryParticles;
        int m_nrOfPrimaryVertices;
        int m_secondaryParticleBarCode;
        int m_secondaryVertexBarCode;
        HepMC::GenEvent *m_theEvent;
        HepMC::GenParticle *m_currentPrimary;
        HepMC::GenParticle *m_currentlyTraced;
};

#endif
