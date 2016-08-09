/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4PROFILINGTOOLS_G4UA__TESTACTIONTIMERTOOL_H 
#define G4PROFILINGTOOLS_G4UA__TESTACTIONTIMERTOOL_H 
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/IBeginRunActionTool.h"
#include "G4AtlasInterfaces/IEndRunActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
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
    public IBeginEventActionTool,  public IEndEventActionTool,  public IBeginRunActionTool,  public IEndRunActionTool,  public ISteppingActionTool
  {
    
  public:
    /// Standard constructor
    TestActionTimerTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// retrieve BoE action
    virtual IBeginEventAction* getBeginEventAction() override final 
    { return static_cast<IBeginEventAction*>( getAction() ); }
    /// retrieve EoE action
    virtual IEndEventAction* getEndEventAction() override final 
    { return static_cast<IEndEventAction*>( getAction() ); }
    /// retrieve BoR action
    virtual IBeginRunAction* getBeginRunAction() override final 
    { return static_cast<IBeginRunAction*>( getAction() ); }
    /// retrieve EoR action
    virtual IEndRunAction* getEndRunAction() override final 
    { return static_cast<IEndRunAction*>( getAction() ); }
    /// retrieve stepping action
    virtual ISteppingAction* getSteppingAction() override final 
    { return static_cast<ISteppingAction*>( getAction() ); }

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
