/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ScoringVolumeTrackKiller_H
#define ScoringVolumeTrackKiller_H

#include "G4AtlasTools/UserActionBase.h"


#include <string>

class ScoringVolumeTrackKiller final: public UserActionBase {

  public:
   ScoringVolumeTrackKiller(const std::string& type, const std::string& name, const IInterface* parent):
  UserActionBase(type,name,parent),killCount(0) {;}

   virtual void EndOfEvent(const G4Event*) override;
   virtual void Step(const G4Step*) override;

   virtual StatusCode queryInterface(const InterfaceID&, void**) override;


  private:
   unsigned long killCount;

}; 

#endif

