/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4HITFILTER_H
#define G4HITFILTER_H

#include "FadsActions/UserAction.h"
#include "StoreGate/StoreGateSvc.h"

#include <string>
#include <vector>
#include <map>

class G4HitFilter: public FADS::UserAction {
 public:
  G4HitFilter(std::string s);
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);
  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);
  void SteppingAction(const G4Step*);

private:
  void ParseProperties();

  enum hitCntainerTypes {
    CALOCALIB,
    CSC,
    LAR,
    LUCID,
    MDT,
    RPC,
    SI,
    TGC,
    TILE,
    TRT };

  StoreGateSvc* m_storeGate;
  std::string m_collectionName;
  int m_ntot,m_npass;
  bool m_init;
  std::vector<std::pair<int,std::string> > m_hitContainers;
};


#endif
