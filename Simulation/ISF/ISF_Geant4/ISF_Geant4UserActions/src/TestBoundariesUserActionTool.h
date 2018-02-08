/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4USERACTIONS_G4UA_IGEANT4_TESTBOUNDARIESUSERACTIONTOOL_H
#define ISF_GEANT4USERACTIONS_G4UA_IGEANT4_TESTBOUNDARIESUSERACTIONTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "TestBoundariesUserAction.h"

namespace G4UA
{

  namespace iGeant4
  {

    /// @class TestBoundariesUserActionTool
    /// @brief A class to manage the TestBoundariesUserAction
    ///
    /// creates one action instance per thread
    ///
    /// @author Andrea Di Simone
    ///
    class TestBoundariesUserActionTool:
      public UserActionToolBase<TestBoundariesUserAction>
    {

    public:

      /// standard tool ctor
      TestBoundariesUserActionTool(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent);

    protected:

      /// creates action for this thread
      virtual std::unique_ptr<TestBoundariesUserAction>
      makeAndFillAction(G4AtlasUserActions&) override final;

    }; // class TestBoundariesUserActionTool

  } // namespace iGeant4

} // namespace G4UA

#endif
