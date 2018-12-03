/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA_LENGTHINTEGRATOR_H
#define G4USERACTIONS_G4UA_LENGTHINTEGRATOR_H

// Framework includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

// User action infrastructure includes
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"

// Local includes
#include "G4UserActions/LengthIntegrator.h"

namespace G4UA
{

  /// @class LengthIntegratorTool
  /// @brief Tool which manages the LengthIntegrator action.
  ///
  /// Creates the LengthIntegrator for each worker thread.
  ///
  class LengthIntegratorTool : public ActionToolBase<LengthIntegrator>,
                               public IG4EventActionTool,
                               public IG4SteppingActionTool
  {

    public:

      /// Standard constructor
      LengthIntegratorTool(const std::string& type, const std::string& name,
                           const IInterface* parent);

      /// Initialize the tool
      virtual StatusCode initialize() override;

      /// Retrieve the event action interface
      virtual G4UserEventAction* getEventAction() override final
      { return static_cast<G4UserEventAction*>( getAction() ); }

      /// Retrieve the stepping action interface
      virtual G4UserSteppingAction* getSteppingAction() override final
      { return static_cast<G4UserSteppingAction*>( getAction() ); }

    protected:

      /// Create aciton for this thread
      virtual std::unique_ptr<LengthIntegrator> makeAction() override final;

    private:

      /// Handle to the histogram service
      ServiceHandle<ITHistSvc> m_hSvc;

  }; // class LengthIntegratorTool

} // namespace G4UA

#endif
