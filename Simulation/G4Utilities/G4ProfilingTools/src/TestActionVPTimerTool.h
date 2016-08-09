/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4PROFILINGTOOLS_G4UA__TESTACTIONVPTIMERTOOL_H 
#define G4PROFILINGTOOLS_G4UA__TESTACTIONVPTIMERTOOL_H 
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/IBeginRunActionTool.h"
#include "G4AtlasInterfaces/IEndRunActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
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
    public IBeginEventActionTool,  public IEndEventActionTool,  public IBeginRunActionTool,  public IEndRunActionTool,  public ISteppingActionTool
  {
    
  public:
    /// constructor
    TestActionVPTimerTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// Retrieve the begin-event action interface
    virtual IBeginEventAction* getBeginEventAction() override final 
    { return static_cast<IBeginEventAction*>( getAction() ); }
    /// Retrieve the EoE action interface
    virtual IEndEventAction* getEndEventAction() override final 
    { return static_cast<IEndEventAction*>( getAction() ); }
    /// Retrieve the BoR action interface
    virtual IBeginRunAction* getBeginRunAction() override final 
    { return static_cast<IBeginRunAction*>( getAction() ); }
    /// Retrieve the EoR action interface 
    virtual IEndRunAction* getEndRunAction() override final 
    { return static_cast<IEndRunAction*>( getAction() ); }
    /// Retrieve the stepping action interface
    virtual ISteppingAction* getSteppingAction() override final 
    { return static_cast<ISteppingAction*>( getAction() ); }
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
    //  void TimerPrint(std::ofstream&,
    //		  std::pair<VolTree, volData>, 
    //		  const int depth = 0) const;		//!< (Modifiable) print function for a map element
    void TreeOut(VolTree, const double tAtlas, int depth = 0);
    
    

  }; // class TestActionVPTimerTool
  
} // namespace G4UA 
#endif
