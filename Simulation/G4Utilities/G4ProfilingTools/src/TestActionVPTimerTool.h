/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4PROFILINGTOOLS_G4UA__TESTACTIONVPTIMERTOOL_H
#define G4PROFILINGTOOLS_G4UA__TESTACTIONVPTIMERTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "TestActionVPTimer.h"


namespace G4UA
{

  /// @class TestActionVPTimerTool
  /// @brief Tool which manages the TestActionVPTimer action.
  ///
  /// Create the TestActionVPTimer for each worker thread and merges timing results in
  /// finalization.
  ///
  /// @author Andrea Di Simone
  ///
  class TestActionVPTimerTool : public UserActionToolBase<TestActionVPTimer>
  {

  public:
    /// Standard constructor
    TestActionVPTimerTool(const std::string& type, const std::string& name,
                          const IInterface* parent);

    /// Triggers report merging from threads
    virtual StatusCode finalize() override;

  protected:
    /// Create action for this thread
    virtual std::unique_ptr<TestActionVPTimer>
    makeAndFillAction(G4AtlasUserActions&) override final;

  private:

    /// holds the runtime configuration
    TestActionVPTimer::Config m_config;
    /// holds data to be reported at end of run
    TestActionVPTimer::Report m_report;

    void TimerPrint(std::pair<VolTree, TestActionVPTimer::volumeData>,
                    const double tTotal,
                    const int depth = 0) const;
    //  void TimerPrint(std::ofstream&,
    //            std::pair<VolTree, volData>,
    //            const int depth = 0) const;           //!< (Modifiable) print function for a map element
    void TreeOut(const VolTree&, const double tAtlas, int depth = 0);

  }; // class TestActionVPTimerTool

} // namespace G4UA

#endif
