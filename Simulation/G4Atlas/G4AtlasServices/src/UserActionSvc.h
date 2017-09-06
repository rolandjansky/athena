/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef G4ATLASSERVICES__G4UA_USERACTIONSVC_H
#define G4ATLASSERVICES__G4UA_USERACTIONSVC_H


// System includes
#include <thread>

// Framework includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"

// Local includes
#include "G4AtlasRunAction.h"
#include "G4AtlasEventAction.h"
#include "G4AtlasStackingAction.h"
#include "G4AtlasTrackingAction.h"
#include "G4AtlasSteppingAction.h"
#include "G4AtlasInterfaces/IBeginRunActionTool.h"
#include "G4AtlasInterfaces/IEndRunActionTool.h"
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/IStackingActionTool.h"
#include "G4AtlasInterfaces/IPreTrackingActionTool.h"
#include "G4AtlasInterfaces/IPostTrackingActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"

#include "G4AtlasInterfaces/IUserActionSvc.h"
#include "G4AtlasTools/ThreadActionHolder.h"


namespace G4UA
{

  /// @class UserActionSvc
  /// @brief A service which manages the user actions for G4 simulation
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class UserActionSvc : public AthService,
                        public virtual IUserActionSvc
  {

    public:

      /// Standard constructor
      UserActionSvc(const std::string& name, ISvcLocator* pSvcLocator);

      /// Initialize the service
      StatusCode initialize() override;

      /// Initialize the user actions for the current thread
      StatusCode initializeActions() override final;

      /// Gaudi interface query
      virtual StatusCode queryInterface(const InterfaceID& riid,
                                        void** ppvInterface) override;

    private:

      /// @name Handles to ATLAS action tools
      /// @{

      /// Begin-run action tools
      ToolHandleArray<IBeginRunActionTool> m_beginRunActionTools;
      /// End-run action tools
      ToolHandleArray<IEndRunActionTool> m_endRunActionTools;
      /// Begin-event action tools
      ToolHandleArray<IBeginEventActionTool> m_beginEventActionTools;
      /// End-event action tools
      ToolHandleArray<IEndEventActionTool> m_endEventActionTools;
      /// Stacking action tools
      ToolHandleArray<IStackingActionTool> m_stackingActionTools;
      /// Pre-tracking action tools
      ToolHandleArray<IPreTrackingActionTool> m_preTrackingActionTools;
      /// Post-tracking action tools
      ToolHandleArray<IPostTrackingActionTool> m_postTrackingActionTools;
      /// Stepping action tools
      ToolHandleArray<ISteppingActionTool> m_steppingActionTools;

      /// @}

      /// @name ATLAS Geant4 user actions
      /// @{

      /// Thread-local run action
      ThreadActionHolder<G4AtlasRunAction> m_runActions;
      /// Thread-local event action
      ThreadActionHolder<G4AtlasEventAction> m_eventActions;
      /// Thread-local stacking action
      ThreadActionHolder<G4AtlasStackingAction> m_stackingActions;
      /// Thread-local tracking action
      ThreadActionHolder<G4AtlasTrackingAction> m_trackingActions;
      /// Thread-local stepping action
      ThreadActionHolder<G4AtlasSteppingAction> m_steppingActions;

      /// @}

  }; // class UserActionSvc

}

#endif
