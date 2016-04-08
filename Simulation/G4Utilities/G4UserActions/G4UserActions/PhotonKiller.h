/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhotonKiller_H
#define PhotonKiller_H

#include "G4AtlasTools/UserActionBase.h"

#include <string>

class PhotonKiller final: public UserActionBase {
  
 public:
 PhotonKiller(const std::string& type, const std::string& name, const IInterface* parent): UserActionBase(type,name,parent){};

  virtual void Step(const G4Step*) override;
  
  virtual StatusCode initialize() override;
  virtual StatusCode queryInterface(const InterfaceID&, void**) override;
  
 private:
  
};

#endif
