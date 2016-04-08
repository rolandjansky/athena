/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIPKiller_H
#define HIPKiller_H

#include "G4AtlasTools/UserActionBase.h"
#include <string>

class HIPKiller final: public UserActionBase {

  public:
 HIPKiller(const std::string& type, const std::string& name, const IInterface* parent): UserActionBase(type,name,parent) {}
  
  virtual void Step(const G4Step*) override;
  
  virtual StatusCode queryInterface(const InterfaceID&, void**) override;
  

};

#endif
