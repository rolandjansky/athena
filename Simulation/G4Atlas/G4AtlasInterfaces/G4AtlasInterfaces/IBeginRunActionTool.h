/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_IBEGINRUNACTIONTOOL_H
#define G4ATLASINTERFACES__G4UA_IBEGINRUNACTIONTOOL_H

// Framework includes
#include "GaudiKernel/IAlgTool.h"

// Local includes
#include "G4AtlasInterfaces/IBeginRunAction.h"

namespace G4UA
{

  /// @class IBeginRunActionTool
  /// @brief Abstract interface for tools that manage ATLAS begin-of-run
  /// custom actions.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class IBeginRunActionTool : virtual public IAlgTool
  {

    public:

      /// Return the action for current thread.
      virtual IBeginRunAction* getBeginRunAction() = 0;

      /// Interface declaration
      static const InterfaceID& interfaceID() {
        static const InterfaceID iid_IBeginRunTool("G4UA::IBeginRunActionTool", 1, 0);
        return iid_IBeginRunTool;
      }

  }; // class IBeginRunActionTool

} // namespace G4UA

#endif
