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
        EventInformation(): G4VUserEventInformation(),nrOfPrimaryParticles(0),
                        nrOfPrimaryVertices(0),secondaryParticleBarCode(200000),
                        secondaryVertexBarCode(-200000),theEvent(0),
                        currentPrimary(0),currentlyTraced(0) {}
        HepMC::GenEvent* GetHepMCEvent() ;
        void SetHepMCEvent(HepMC::GenEvent*);
        int GetNrOfPrimaryParticles() const;
        void SetNrOfPrimaryParticles(int nr);
        int GetNrOfPrimaryVertices() const;
        void SetNrOfPrimaryVertices(int nr);
        void SetVertexPosition(const G4ThreeVector);
        const G4ThreeVector GetVertexPosition() const;
        void Print() const {}

        void SetCurrentPrimary(HepMC::GenParticle *p) {currentPrimary=p;}

        void SetCurrentlyTraced(HepMC::GenParticle *p) {currentlyTraced=p;}

        HepMC::GenParticle *GetCurrentPrimary() const {return currentPrimary;}

        HepMC::GenParticle *GetCurrentlyTraced() const {return currentlyTraced;}
        int SecondaryParticleBarCode() {secondaryParticleBarCode++;
                                        return secondaryParticleBarCode;}
        int SecondaryVertexBarCode() {secondaryVertexBarCode--;
                                      return secondaryVertexBarCode;}

private:
        G4ThreeVector vertexPosition;
        int nrOfPrimaryParticles;
        int nrOfPrimaryVertices;
        int secondaryParticleBarCode;
        int secondaryVertexBarCode;
        HepMC::GenEvent *theEvent;
        HepMC::GenParticle *currentPrimary;
        HepMC::GenParticle *currentlyTraced;
};

#endif
