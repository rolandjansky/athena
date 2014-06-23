/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#ifndef MicromegasSensitiveDetector_H
#define MicromegasSensitiveDetector_H

#include "globals.hh"

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"
#include "MuonSimEvent/GenericMuonSimHitCollection.h"

class GenericMuonSimHit;
class MicromegasHitIdHelper;

class MicromegasSensitiveDetector : public FADS::FadsSensitiveDetector {
    
public:
    /** construction/destruction */
     MicromegasSensitiveDetector(std::string name);
    ~MicromegasSensitiveDetector() {}
    
    /** member functions */
    void   Initialize(G4HCofThisEvent* HCE);
    G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist);
    void   EndOfEvent(G4HCofThisEvent* HCE); 
    
private:

    GenericMuonSimHitCollection* m_GenericMuonHitCollection;
    AthenaHitsCollectionHelper m_hitCollHelp;
    MicromegasHitIdHelper* m_muonHelper;

};

#endif
