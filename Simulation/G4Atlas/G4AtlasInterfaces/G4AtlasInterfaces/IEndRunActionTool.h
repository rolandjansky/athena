/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_IENDRUNACTIONTOOL_H
#define G4ATLASINTERFACES__G4UA_IENDRUNACTIONTOOL_H

// Framework includes
#include "GaudiKernel/IAlgTool.h"

// Local includes
#include "G4AtlasInterfaces/IEndRunAction.h"

namespace G4UA
{

  /// @class IEndRunActionTool
  /// @brief Abstract interface for tools that manage ATLAS end-of-run
  /// custom actions.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class IEndRunActionTool : virtual public IAlgTool
  {

    public:

      /// Return the action for current thread.
      virtual IEndRunAction* getEndRunAction() = 0;

      /// Interface declaration
      static const InterfaceID& interfaceID() {
        static const InterfaceID iid_IEndRunTool("G4UA::IEndRunActionTool", 1, 0);
        return iid_IEndRunTool;
      }

  }; // class IEndRunActionTool

} // namespace G4UA

#endif
