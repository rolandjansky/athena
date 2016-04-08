/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// an action to query the SD responsible for the storing of the
// TrackRecords *at the entrance of the ID* if no track it aborts
// the event - jamie boyd 15 nov 06


#ifndef G4CosmicFilter_H
#define G4CosmicFilter_H

#include "G4AtlasTools/UserActionBase.h"

#include <string>

class G4CosmicFilter final: public UserActionBase
{
 private:

  int m_ntot,m_npass;
  int m_PDGId;
  std::string m_collectionName,m_volumeName;

  double m_ptMin , m_ptMax;

public:
  G4CosmicFilter(const std::string& type, const std::string& name, const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual StatusCode queryInterface(const InterfaceID&, void**) override;

  virtual void EndOfEvent(const G4Event*) override;
  virtual void EndOfRun(const G4Run*) override;

};


#endif
