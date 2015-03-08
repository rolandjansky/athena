/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HyperspaceCatcher_H
#define HyperspaceCatcher_H

#include "FadsActions/ActionsBase.h"
#include "FadsActions/UserAction.h"
#include <string>

class G4VSolid;
class G4Track;

class HyperspaceCatcher: public FADS::ActionsBase , public FADS::UserAction {

  public:
   HyperspaceCatcher(std::string s): FADS::ActionsBase(s), FADS::UserAction(s), m_killCount(0),m_world(0),m_treatmentLevel(0),m_killAfter(0) {}
   void SteppingAction(const G4Step*);

   // This method is past a const G4Track, so it can't really kill things
   // void PreUserTrackingAction(const G4Track* aTrack);

  private:
   void initAndSetup();
   int m_killCount;
   G4VSolid * m_world;
   int m_treatmentLevel;
   int m_killAfter;
};

#endif
