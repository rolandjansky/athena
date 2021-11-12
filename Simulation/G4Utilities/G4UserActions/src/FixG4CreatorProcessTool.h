/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__FIXG4CREATORPROCESSTOOL_H
#define G4USERACTIONS_G4UA__FIXG4CREATORPROCESSTOOL_H

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "FixG4CreatorProcess.h"

namespace G4UA
{

  /// @brief Tool which manages the FixG4CreatorProcess user action.
  ///
  class FixG4CreatorProcessTool : public UserActionToolBase<FixG4CreatorProcess>
  {

    public:

      /// Standard constructor
      FixG4CreatorProcessTool(const std::string& type, const std::string& name,
                       const IInterface* parent);

    protected:

      /// Create the action for the current thread
      virtual std::unique_ptr<FixG4CreatorProcess>
      makeAndFillAction(G4AtlasUserActions&) override final;

  }; // class FixG4CreatorProcessTool

} // namespace G4UA

#endif
