/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_ISTACKINGACTIONTOOL_H
#define G4ATLASINTERFACES__G4UA_ISTACKINGACTIONTOOL_H

// Framework includes
#include "GaudiKernel/IAlgTool.h"

// Local includes
#include "G4AtlasInterfaces/IStackingAction.h"

namespace G4UA
{

  /// @class IStackingActionTool
  /// @brief Abstract interface for tools that manage ATLAS stacking actions.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class IStackingActionTool : virtual public IAlgTool
  {
    
    public:

      /// @brief Return the action for current thread.
      virtual IStackingAction* getStackingAction() = 0;

      /// Interface declaration
      static const InterfaceID& interfaceID() {
        static const InterfaceID iid_IStepTool("G4UA::IStackingActionTool", 1, 0);
        return iid_IStepTool;
      }

  }; // class IStackingActionTool

} // namespace G4UA

#endif
