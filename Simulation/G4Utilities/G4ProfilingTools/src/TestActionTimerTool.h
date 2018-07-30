/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4PROFILINGTOOLS_G4UA__TESTACTIONTIMERTOOL_H
#define G4PROFILINGTOOLS_G4UA__TESTACTIONTIMERTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "TestActionTimer.h"
#include "GaudiKernel/ITHistSvc.h"


namespace G4UA
{

  /// @class TestActionTimerTool
  /// @brief Tool which manages the TestActionTimer action.
  ///
  /// Create the TestActionTimer  for each worker thread
  ///
  /// @author Andrea Di Simone
  ///
  class TestActionTimerTool: public UserActionToolBase<TestActionTimer>
  {

  public:
    /// Standard constructor
    TestActionTimerTool(const std::string& type, const std::string& name,const IInterface* parent);

    /// Initialize the tool
    virtual StatusCode initialize() override;
    /// Finalize the tool
    virtual StatusCode finalize() override;

  protected:
    /// Create action for this thread
    virtual std::unique_ptr<TestActionTimer>
    makeAndFillAction(G4AtlasUserActions&) override final;

  private:
    /// HistSvc to record timing histograms
    ServiceHandle<ITHistSvc> m_histSvc;

  }; // class TestActionTimerTool

} // namespace G4UA

#endif
