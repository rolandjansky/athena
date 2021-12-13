/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// This is based on https://gitlab.cern.ch/atlas/athena/-/blob/master/Tracking/TrkG4Components/TrkG4UserActions/TrkG4UserActions/GeantFollower.h

#ifndef ActsGeantFollower_H
#define ActsGeantFollower_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "IActsGeantFollowerHelper.h"
#include <string>
#include <vector>

#include "G4UserEventAction.hh"
#include "G4UserRunAction.hh"
#include "G4UserSteppingAction.hh"


class ActsGeantFollower: public G4UserEventAction, public G4UserRunAction, public G4UserSteppingAction
{

public:

  ActsGeantFollower();
  virtual void BeginOfEventAction(const G4Event*) override;
  virtual void EndOfEventAction(const G4Event*) override;
  virtual void BeginOfRunAction(const G4Run*) override;
  virtual void UserSteppingAction(const G4Step*) override;

private:

    ToolHandle<IActsGeantFollowerHelper> m_helper =
      ToolHandle<IActsGeantFollowerHelper>("ActsGeantFollowerHelper");
}; // class GeantFollower

#endif