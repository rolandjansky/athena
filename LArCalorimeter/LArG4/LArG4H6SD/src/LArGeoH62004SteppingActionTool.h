/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H6SD_LARGEOH62004STEPPINGACTIONTOOL_H
#define LARG4H6SD_LARGEOH62004STEPPINGACTIONTOOL_H

// System includes
#include <string>

// Infrastructure includes
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"

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
  class LArGeoH62004SteppingActionTool : public ActionToolBase<LArGeoH62004SteppingAction>,
                                         public IG4SteppingActionTool
  {

  public:

    /// Standard constructor
    LArGeoH62004SteppingActionTool(const std::string& type, const std::string& name,
                                   const IInterface* parent);

    /// Framework intialize method
    StatusCode initialize() override final;

    /// Retrieve the stepping action interface
    virtual G4UserSteppingAction* getSteppingAction() override final
    { return static_cast<G4UserSteppingAction*>( getAction() ); }

    /// Query interface for gaudi
    virtual StatusCode
    queryInterface(const InterfaceID& riid, void** ppvInterface) override;

  protected:

    /// Create action for this thread
    virtual std::unique_ptr<LArGeoH62004SteppingAction> makeAction() override final;

  private:
    /// Configuration parameters
    LArGeoH62004SteppingAction::Config m_config;

  }; // class LArGeoH62004SteppingActionTool

} // namespace G4UA

#endif
