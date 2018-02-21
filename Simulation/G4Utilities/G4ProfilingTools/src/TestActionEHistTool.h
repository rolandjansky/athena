/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4PROFILINGTOOLS_G4UA__TESTACTIONEHISTTOOL_H
#define G4PROFILINGTOOLS_G4UA__TESTACTIONEHISTTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "TestActionEHist.h"

namespace G4UA
{

  /// @class TestActionEHistTool
  /// @brief A Tool to manage TestActionEHist
  /// Create the TestActionEHist  for each worker thread. Note that since this is using ROOT access
  /// DIRECTLY, it is aonly tested and working with 1 thread. Operation with more than 1 thread
  /// will be disabled from the python side
  ///
  /// @author Andrea Di Simone
  ///
  class TestActionEHistTool:
  public UserActionToolBase<TestActionEHist>
  {

  public:

    /// Standard constructor
    TestActionEHistTool(const std::string& type, const std::string& name,
                        const IInterface* parent);

  protected:

    /// Create action for this thread
    virtual std::unique_ptr<TestActionEHist>
    makeAndFillAction(G4AtlasUserActions&) override final;

  private:

    /// holds the configuration details received from python
    TestActionEHist::Config m_config;

  }; // class TestActionEHistTool

} // namespace G4UA

#endif
