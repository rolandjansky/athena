/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// an action to query the SD responsible for the storing of the
// TrackRecords *at the entrance of the ID* if no track it aborts
// the event - jamie boyd 15 nov 06


#ifndef G4CosmicFilter_H
#define G4CosmicFilter_H

#include "FadsActions/UserAction.h"
#include <string>
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"

class G4CosmicFilter: public FADS::UserAction {
private:

  StoreGateSvc*   m_storeGate;
  int m_ntot,m_npass;
  int m_magicID;
  std::string m_collectionName;

  double m_ptMin , m_ptMax;

  void ParseProperties();
  bool m_init;

public:
  G4CosmicFilter(std::string s):FADS::UserAction(s),m_storeGate(0),m_ntot(0),m_npass(0),m_magicID(0),m_ptMin(-1),m_ptMax(-1),m_init(false){};
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);
  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);
  void SteppingAction(const G4Step*);

};


#endif
