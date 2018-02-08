/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H6SD_LARGEOH62004EVENTACTIONTOOL_H
#define LARG4H6SD_LARGEOH62004EVENTACTIONTOOL_H

// System includes
#include <string>

// Infrastructure includes
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"

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
  class LArGeoH62004EventActionTool : public ActionToolBase<LArGeoH62004EventAction>,
                                      public IG4EventActionTool
  {

  public:

    /// Standard constructor
    LArGeoH62004EventActionTool(const std::string& type, const std::string& name,
                                const IInterface* parent);

    /// Framework intialize method
    StatusCode initialize() override final;

    /// Retrieve the event action interface
    virtual G4UserEventAction* getEventAction() override final
    { return static_cast<G4UserEventAction*>( getAction() ); }

    /// Query interface for gaudi
    virtual StatusCode
    queryInterface(const InterfaceID& riid, void** ppvInterface) override;

  protected:

    /// Create action for this thread
    virtual std::unique_ptr<LArGeoH62004EventAction> makeAction() override final;

  private:
    /// Configuration parameters
    LArGeoH62004EventAction::Config m_config;

  }; // class LArGeoH62004EventActionTool

} // namespace G4UA

#endif
