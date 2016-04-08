/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4HITFILTER_H
#define G4HITFILTER_H


#include "G4AtlasTools/UserActionBase.h"

#include <string>
#include <vector>
#include <map>

class G4HitFilter final: public UserActionBase {

 public:
  G4HitFilter(const std::string& type, const std::string& name, const IInterface* parent);

  virtual void EndOfEvent(const G4Event*) override;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  
  virtual StatusCode queryInterface(const InterfaceID&, void**) override;

private:

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

  // property on python side
  std::vector<std::string> m_volumenames;

  // internals
  int m_ntot,m_npass;
  std::vector<std::pair<int,std::string> > m_hitContainers;
};


#endif
