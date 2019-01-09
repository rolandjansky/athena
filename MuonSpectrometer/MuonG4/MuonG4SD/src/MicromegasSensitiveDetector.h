/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MicromegasSensitiveDetector_H
#define MicromegasSensitiveDetector_H

#include "G4VSensitiveDetector.hh"
#include "StoreGate/WriteHandle.h"
#include "MuonSimEvent/GenericMuonSimHitCollection.h"
#include "MuonSimEvent/MMSimHitCollection.h"

class MicromegasHitIdHelper;

class MicromegasSensitiveDetector : public G4VSensitiveDetector {

public:
    /** construction/destruction */
    MicromegasSensitiveDetector(const std::string& name, const std::string& hitCollectionName);
    ~MicromegasSensitiveDetector() {}
    
    /** member functions */
    void   Initialize(G4HCofThisEvent* HCE) override final;
    G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist) override final;
    
private:

    SG::WriteHandle<MMSimHitCollection> m_MMSimHitCollection;
    SG::WriteHandle<GenericMuonSimHitCollection> m_GenericMuonHitCollection;
    MicromegasHitIdHelper* m_muonHelper;

};

#endif
