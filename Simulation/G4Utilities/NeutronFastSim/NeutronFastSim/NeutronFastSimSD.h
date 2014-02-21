/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NeutronFastSimSD_H
#define NeutronFastSimSD_H

#include "TrackRecord/TrackRecordCollection.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"
#include "FadsSensitiveDetector/FadsSensitiveDetector.h"

class G4Track;

class NeutronFastSimSD : public FADS::FadsSensitiveDetector
{
  public:
   NeutronFastSimSD(std::string name) : FADS::FadsSensitiveDetector(name) , trackRecordCollection(0) {}
   ~NeutronFastSimSD() {}

   void   Initialize(G4HCofThisEvent*);
   G4bool ProcessHits(G4Step*, G4TouchableHistory*) { return false; }
   void   EndOfEvent(G4HCofThisEvent*E); 
   void   WriteNeutron(const G4Track*);

  private:
   TrackRecordCollection *trackRecordCollection;
   AthenaHitsCollectionHelper theHelper;
};

#endif
