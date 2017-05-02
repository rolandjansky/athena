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

#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/IBeginRunAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"
namespace G4UA{


  class GeantFollower: public IBeginEventAction,  public IEndEventAction,  public IBeginRunAction,  public ISteppingAction
  {

  public:

    struct Config
    {
      ToolHandle<Trk::IGeantFollowerHelper>  helper=ToolHandle<Trk::IGeantFollowerHelper>("Trk::GeantFollowerHelper/GeantFollowerHelper");
    };

    GeantFollower(const Config& config);
    virtual void beginOfEvent(const G4Event*) override;
    virtual void endOfEvent(const G4Event*) override;
    virtual void beginOfRun(const G4Run*) override;
    virtual void processStep(const G4Step*) override;

  private:

    Config m_config;
    mutable const Trk::IGeantFollowerHelper*  m_helperPointer;

  }; // class GeantFollower


} // namespace G4UA

#endif
