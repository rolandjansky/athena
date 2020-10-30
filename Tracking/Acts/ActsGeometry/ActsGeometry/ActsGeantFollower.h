/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// This is based on https://gitlab.cern.ch/atlas/athena/-/blob/master/Tracking/TrkG4Components/TrkG4UserActions/TrkG4UserActions/GeantFollower.h

#ifndef ActsGeantFollower_H
#define ActsGeantFollower_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "ActsGeometry/IActsGeantFollowerHelper.h"
#include <string>
#include <vector>

#include "G4UserEventAction.hh"
#include "G4UserRunAction.hh"
#include "G4UserSteppingAction.hh"


class ActsGeantFollower: public G4UserEventAction, public G4UserRunAction, public G4UserSteppingAction
{

public:

  struct Config
  {
    /// FIXME: Public tool handle. See ATLASSIM-3561.
    ToolHandle<IActsGeantFollowerHelper> helper =
      ToolHandle<IActsGeantFollowerHelper>("ActsGeantFollowerHelper");
  };

  ActsGeantFollower(const Config& config);
  virtual void BeginOfEventAction(const G4Event*) override;
  virtual void EndOfEventAction(const G4Event*) override;
  virtual void BeginOfRunAction(const G4Run*) override;
  virtual void UserSteppingAction(const G4Step*) override;

private:

  Config m_config;

  /// FIXME: mutable helper! See ATLASSIM-3561.
  mutable const IActsGeantFollowerHelper* m_helperPointer;

}; // class GeantFollower

#endif