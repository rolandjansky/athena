/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGCSensitiveDetector_H
#define sTGCSensitiveDetector_H

#include "G4VSensitiveDetector.hh"
#include "StoreGate/WriteHandle.h"
#include "MuonSimEvent/GenericMuonSimHitCollection.h"

class sTgcHitIdHelper;

class sTGCSensitiveDetector : public G4VSensitiveDetector {
    
public:
    /** construction/destruction */
    sTGCSensitiveDetector(const std::string& name, const std::string& hitCollectionName);
    ~sTGCSensitiveDetector() {}
    
    /** member functions */
    void   Initialize(G4HCofThisEvent* HCE) override final;
    G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist) override final;
    
private:

    SG::WriteHandle<GenericMuonSimHitCollection> m_GenericMuonHitCollection;
    sTgcHitIdHelper* m_muonHelper;

};

#endif
