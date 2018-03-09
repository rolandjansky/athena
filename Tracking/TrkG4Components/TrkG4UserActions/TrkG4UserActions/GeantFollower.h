/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeantFollower.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef GeantFollower_H
#define GeantFollower_H

#include "GaudiKernel/ToolHandle.h"
#include "TrkG4UserActions/IGeantFollowerHelper.h"
#include <string>
#include <vector>

/** @class GeantFollower

    @author Andreas.Salzburger@cern.ch
*/

namespace Trk {
  class IGeantFollowerHelper;
}

#include "G4UserEventAction.hh"
#include "G4UserRunAction.hh"
#include "G4UserSteppingAction.hh"
namespace G4UA{


  class GeantFollower: public G4UserEventAction, public G4UserRunAction,  public G4UserSteppingAction
  {

  public:

    struct Config
    {
      ToolHandle<Trk::IGeantFollowerHelper>  helper=ToolHandle<Trk::IGeantFollowerHelper>("Trk::GeantFollowerHelper/GeantFollowerHelper");
    };

    GeantFollower(const Config& config);
    virtual void BeginOfEventAction(const G4Event*) override;
    virtual void EndOfEventAction(const G4Event*) override;
    virtual void BeginOfRunAction(const G4Run*) override;
    virtual void UserSteppingAction(const G4Step*) override;

  private:

    Config m_config;
    mutable const Trk::IGeantFollowerHelper*  m_helperPointer;

  }; // class GeantFollower


} // namespace G4UA

#endif
