/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H6SD_LARGEOH62004EVENTACTIONTOOL_H
#define LARG4H6SD_LARGEOH62004EVENTACTIONTOOL_H

// System includes
#include <string>

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "LArGeoH62004EventAction.h"

namespace G4UA
{

  /// @class LArGeoH62004EventActionTool
  /// @brief Tool which manages the LArGeoH62004EventAction action.
  ///
  /// Create the LArGeoH62004EventAction for each worker thread
  ///
  /// @author Andrea Di Simone
  ///
  class LArGeoH62004EventActionTool :
    public UserActionToolBase<LArGeoH62004EventAction>
  {

  public:

    /// Standard constructor
    LArGeoH62004EventActionTool(const std::string& type, const std::string& name,
                                const IInterface* parent);

    /// Framework intialize method
    StatusCode initialize() override final;

  protected:

    /// Create action for this thread
    virtual std::unique_ptr<LArGeoH62004EventAction>
    makeAndFillAction(G4AtlasUserActions&) override final;

  private:
    /// Configuration parameters
    LArGeoH62004EventAction::Config m_config;

  }; // class LArGeoH62004EventActionTool

} // namespace G4UA

#endif
