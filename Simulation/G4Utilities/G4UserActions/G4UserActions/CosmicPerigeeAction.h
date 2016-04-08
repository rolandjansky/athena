/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CosmicPerigeeAction_H
#define CosmicPerigeeAction_H

#include "G4AtlasTools/UserActionBase.h"

#include "StoreGate/WriteHandle.h"
#include "TrackRecord/TrackRecordCollection.h" // Can't be forward declared - it's a type def

#include <string>

class CosmicPerigeeAction final: public UserActionBase {

 public:
  CosmicPerigeeAction(const std::string& type, const std::string& name, const IInterface* parent);

  virtual void BeginOfEvent(const G4Event*) override;
  virtual void EndOfEvent(const G4Event*) override;
  virtual void BeginOfRun(const G4Run*) override;
  virtual void Step(const G4Step*) override;

  StatusCode initialize() override;
  virtual StatusCode queryInterface(const InterfaceID&, void**);

 private:
  SG::WriteHandle<TrackRecordCollection> m_trackRecordCollection;
  double m_idZ, m_idR;
  bool m_allowMods;
};

#endif
