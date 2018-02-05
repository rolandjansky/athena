/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKG4USERACTIONS_G4UA__GEANTFOLLOWERTOOL_H
#define TRKG4USERACTIONS_G4UA__GEANTFOLLOWERTOOL_H

#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/UserActionToolBase.h"
#include "TrkG4UserActions/GeantFollower.h"

namespace G4UA{

  /// @class GeantFollowerTool
  /// @brief Tool which manages the GeantFollower action
  ///
  /// @author Andrea Di Simone
  ///
  class GeantFollowerTool : public UserActionToolBase<GeantFollower>
  {

  public:
    /// Standard constructor
    GeantFollowerTool(const std::string& type, const std::string& name,const IInterface* parent);
  protected:
    /// Create an action for this thread
    virtual std::unique_ptr<GeantFollower>
    makeAndFillAction(G4AtlasUserActions&) override final;
  private:
    /// Configuration parameters
    GeantFollower::Config m_config;
  }; // class GeantFollowerTool

} // namespace G4UA

#endif
