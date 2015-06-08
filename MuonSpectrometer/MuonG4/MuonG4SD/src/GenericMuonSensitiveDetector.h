/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GenericMuonSensitiveDetector_H
#define GenericMuonSensitiveDetector_H

#include "G4VSensitiveDetector.hh"
#include "StoreGate/WriteHandle.h"
#include "MuonSimEvent/GenericMuonSimHitCollection.h"

class GenericMuonSensitiveDetector : public G4VSensitiveDetector {
    
public:
    /** construction/destruction */
    GenericMuonSensitiveDetector(const std::string& name, const std::string& hitCollectionName);
    ~GenericMuonSensitiveDetector() {}
    
    /** member functions */
    void   Initialize(G4HCofThisEvent* HCE) override final;
    G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist) override final;
    
private:

    SG::WriteHandle<GenericMuonSimHitCollection> m_GenericMuonHitCollection;

};

#endif
