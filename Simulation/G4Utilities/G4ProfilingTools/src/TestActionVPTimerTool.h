/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4PROFILINGTOOLS_G4UA__TESTACTIONVPTIMERTOOL_H 
#define G4PROFILINGTOOLS_G4UA__TESTACTIONVPTIMERTOOL_H 
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "TestActionVPTimer.h"


namespace G4UA{ 

  /// @class TestActionVPTimerTool
  /// @brief Tool which manages the TestActionVPTimer action.
  ///
  /// Create the TestActionVPTimer for each worker thread and merges timing results in
  /// finalization.
  ///
  /// @author Andrea Di Simone
  
class TestActionVPTimerTool:
  
  public ActionToolBaseReport<TestActionVPTimer>,
    public IG4EventActionTool, public IG4RunActionTool,  public IG4SteppingActionTool
  {
    
  public:
    /// constructor
    TestActionVPTimerTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// Retrieve the event action interface
    virtual G4UserEventAction* getEventAction() override final 
    { return static_cast<G4UserEventAction*>( getAction() ); }
    /// Retrieve the run action interface
    virtual G4UserRunAction* getRunAction() override final 
    { return static_cast<G4UserRunAction*>( getAction() ); }
    /// Retrieve the stepping action interface
    virtual G4UserSteppingAction* getSteppingAction() override final 
    { return static_cast<G4UserSteppingAction*>( getAction() ); }
    /// gaudi interface query
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
    /// triggers report merging from threads
    virtual StatusCode finalize() override;

  protected:
    /// Create action for this thread
    virtual std::unique_ptr<TestActionVPTimer> makeAction() override final;

  private:
    /// holds the runtime configuration
    TestActionVPTimer::Config m_config;
    /// holds data to be reported at end of run
    //TestActionVPTimer::Report m_report;

    void TimerPrint(std::pair<VolTree, TestActionVPTimer::volumeData>, 
		    const double tTotal,
		    const int depth = 0) const;
    void TreeOut(VolTree, const double tAtlas, int depth = 0);
    
    

  }; // class TestActionVPTimerTool
  
} // namespace G4UA 
#endif
