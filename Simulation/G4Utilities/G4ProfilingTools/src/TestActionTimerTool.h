/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4PROFILINGTOOLS_G4UA__TESTACTIONTIMERTOOL_H 
#define G4PROFILINGTOOLS_G4UA__TESTACTIONTIMERTOOL_H 
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "TestActionTimer.h"

class ITHistSvc;

namespace G4UA{ 

  /// @class TestActionTimerTool
  /// @brief Tool which manages the TestActionTimer action.
  ///
  /// Create the TestActionTimer  for each worker thread
  ///
  /// @author Andrea Di Simone
  ///
  
  class TestActionTimerTool: public ActionToolBaseReport<TestActionTimer>,
    public IG4EventActionTool, public IG4RunActionTool,  public IG4SteppingActionTool
  {
    
  public:
    /// Standard constructor
    TestActionTimerTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// retrieve event action
    virtual G4UserEventAction* getEventAction() override final 
    { return static_cast<G4UserEventAction*>( getAction() ); }
    /// retrieve run action
    virtual G4UserRunAction* getRunAction() override final 
    { return static_cast<G4UserRunAction*>( getAction() ); }
    /// retrieve stepping action
    virtual G4UserSteppingAction* getSteppingAction() override final 
    { return static_cast<G4UserSteppingAction*>( getAction() ); }

    /// Query interface for gaudi
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

    virtual StatusCode finalize() override;

  protected:
    /// Create action for this thread
    virtual std::unique_ptr<TestActionTimer> makeAction() override final;
  private:
    /// data from individual threads to be merged at the end of run
    //TestActionTimer::Report m_report;
    /// HistSvc to record timing histograms
    ServiceHandle<ITHistSvc> m_histSvc;
    


}; // class TestActionTimerTool
  
  
} // namespace G4UA 
#endif
