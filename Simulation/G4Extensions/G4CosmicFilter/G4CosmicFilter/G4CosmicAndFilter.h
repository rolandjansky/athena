/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// an action to query the SD responsible for the storing of the
// TrackRecords *at the entrance of the ID* if no track it aborts
// the event - jamie boyd 15 nov 06


#ifndef G4CosmicAndFilter_H
#define G4CosmicAndFilter_H

#include "G4AtlasTools/UserActionBase.h"

#include <string>

class G4CosmicAndFilter final: public UserActionBase {

private:

  int ntracks;

  int m_ntot,m_npass;

  std::string m_collectionName;
  std::string m_collectionName2;
  std::string m_volumeName;
  std::string m_volumeName2;

public:

  G4CosmicAndFilter(const std::string& type, const std::string& name, const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual StatusCode queryInterface(const InterfaceID&, void**) override;

  virtual void EndOfEvent(const G4Event*) override;
  virtual void EndOfRun(const G4Run*) override;




};


#endif
