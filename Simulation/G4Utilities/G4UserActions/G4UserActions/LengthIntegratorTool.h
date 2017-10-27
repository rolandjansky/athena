/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA_LENGTHINTEGRATOR_H
#define G4USERACTIONS_G4UA_LENGTHINTEGRATOR_H

// Framework includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

// User action infrastructure includes
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
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
                               public IBeginEventActionTool,
                               public IEndEventActionTool,
                               public ISteppingActionTool
  {

    public:

      /// Standard constructor
      LengthIntegratorTool(const std::string& type, const std::string& name,
                           const IInterface* parent);

      /// Initialize the tool
      virtual StatusCode initialize() override;

      /// Retrieve the begin-event action interface
      virtual IBeginEventAction* getBeginEventAction() override final
      { return static_cast<IBeginEventAction*>( getAction() ); }

      /// Retrieve the end-event action interface
      virtual IEndEventAction* getEndEventAction() override final
      { return static_cast<IEndEventAction*>( getAction() ); }

      /// Retrieve the stepping action interface
      virtual ISteppingAction* getSteppingAction() override final
      { return static_cast<ISteppingAction*>( getAction() ); }

    protected:

      /// Create aciton for this thread
      virtual std::unique_ptr<LengthIntegrator> makeAction() override final;

    private:

      /// Handle to the histogram service
      ServiceHandle<ITHistSvc> m_hSvc;

  }; // class LengthIntegratorTool

} // namespace G4UA

#endif
