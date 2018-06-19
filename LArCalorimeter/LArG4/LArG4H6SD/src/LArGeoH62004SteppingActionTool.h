/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H6SD_LARGEOH62004STEPPINGACTIONTOOL_H
#define LARG4H6SD_LARGEOH62004STEPPINGACTIONTOOL_H

// System includes
#include <string>

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "LArGeoH62004SteppingAction.h"

namespace G4UA
{

  /// @class LArGeoH62004SteppingActionTool
  /// @brief Tool which manages the LArGeoH62004SteppingAction action.
  ///
  /// Create the LArGeoH62004SteppingAction for each worker thread
  ///
  /// @author Andrea Di Simone
  ///
  class LArGeoH62004SteppingActionTool :
    public UserActionToolBase<LArGeoH62004SteppingAction>
  {

  public:

    /// Standard constructor
    LArGeoH62004SteppingActionTool(const std::string& type, const std::string& name,
                                   const IInterface* parent);

    /// Framework intialize method
    StatusCode initialize() override final;

  protected:

    /// Create action for this thread
    virtual std::unique_ptr<LArGeoH62004SteppingAction>
    makeAndFillAction(G4AtlasUserActions&) override final;

  private:

    /// Configuration parameters
    LArGeoH62004SteppingAction::Config m_config;

  }; // class LArGeoH62004SteppingActionTool

} // namespace G4UA

#endif
