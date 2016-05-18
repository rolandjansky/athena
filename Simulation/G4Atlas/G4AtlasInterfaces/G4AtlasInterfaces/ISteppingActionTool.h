/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_ISTEPPINGACTIONTOOL_H
#define G4ATLASINTERFACES__G4UA_ISTEPPINGACTIONTOOL_H

// Framework includes
#include "GaudiKernel/IAlgTool.h"

// Local includes
#include "G4AtlasInterfaces/ISteppingAction.h"

namespace G4UA
{

  /// @class ISteppingActionTool
  /// @brief Abstract interface for tools that manage ATLAS stepping actions.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class ISteppingActionTool : virtual public IAlgTool
  {
    
    public:

      /// @brief Return the action for current thread.
      virtual ISteppingAction* getSteppingAction() = 0;

      /// Interface declaration
      static const InterfaceID& interfaceID() {
        static const InterfaceID iid_IStepTool("G4UA::ISteppingActionTool", 1, 0);
        return iid_IStepTool;
      }

  }; // class ISteppingActionTool

} // namespace G4UA

#endif
