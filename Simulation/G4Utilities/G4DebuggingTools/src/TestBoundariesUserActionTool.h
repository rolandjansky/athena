/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA_IGEANT4_TESTBOUNDARIESUSERACTIONTOOL_H
#define G4DEBUGGINGTOOLS_G4UA_IGEANT4_TESTBOUNDARIESUSERACTIONTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "TestBoundariesUserAction.h"

namespace G4UA
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

} // namespace G4UA

#endif
