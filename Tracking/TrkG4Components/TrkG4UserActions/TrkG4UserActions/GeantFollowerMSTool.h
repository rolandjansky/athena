/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKG4USERACTIONS_G4UA__GEANTFOLLOWERMSTOOL_H
#define TRKG4USERACTIONS_G4UA__GEANTFOLLOWERMSTOOL_H

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "TrkG4UserActions/GeantFollowerMS.h"

namespace G4UA
{

  /// @class GeantFollowerMSTool
  /// @brief Tool which manages the GeantFollowerMS action
  ///
  /// @author Andrea Di Simone
  ///

  class GeantFollowerMSTool : public UserActionToolBase<GeantFollowerMS>
  {
  public:
    /// Standard constructor
    GeantFollowerMSTool(const std::string& type, const std::string& name,const IInterface* parent);
  protected:
    /// Create an action for this thread
    virtual std::unique_ptr<GeantFollowerMS>
    makeAndFillAction(G4AtlasUserActions&) override final;
  private:
    /// Configuration parameters
    GeantFollowerMS::Config m_config;
  }; // class GeantFollowerMSTool

} // namespace G4UA

#endif
