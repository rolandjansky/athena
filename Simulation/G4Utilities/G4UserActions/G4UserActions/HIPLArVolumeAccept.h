/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIPLArVolumeAccept_H
#define HIPLArVolumeAccept_H

#include "G4AtlasTools/UserActionBase.h"


#include <string>

class HIPLArVolumeAccept final: public UserActionBase {

  public:
 HIPLArVolumeAccept(const std::string& type, const std::string& name, const IInterface* parent): UserActionBase(type,name,parent),HIPacc(0),HIPevts(0),HIPevts_failed(0) {}
   virtual void BeginOfEvent(const G4Event*) override;
   virtual void EndOfEvent(const G4Event*) override;
   virtual void BeginOfRun(const G4Run*) override;
   virtual void EndOfRun(const G4Run*) override;
   virtual void Step(const G4Step*) override;

   virtual StatusCode queryInterface(const InterfaceID&, void**) override;

  private:
   bool HIPacc;
   int HIPevts;
   int HIPevts_failed;

};

#endif
