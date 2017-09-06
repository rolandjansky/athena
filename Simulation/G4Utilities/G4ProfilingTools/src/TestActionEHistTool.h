/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef G4PROFILINGTOOLS_G4UA__TESTACTIONEHISTTOOL_H 
#define G4PROFILINGTOOLS_G4UA__TESTACTIONEHISTTOOL_H 

#include "G4AtlasInterfaces/IG4TrackingActionTool.h"
#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "TestActionEHist.h"

namespace G4UA{ 
  /// @class TestActionEHistTool
  /// @brief A Tool to manage TestActionEHist
  /// Create the TestActionEHist  for each worker thread. Note that since this is using ROOT access
  /// DIRECTLY, it is aonly tested and working with 1 thread. Operation with more than 1 thread
  /// will be disabled from the python side
  ///
  /// @author Andrea Di Simone

  
  class TestActionEHistTool: 
  public ActionToolBase<TestActionEHist>,
    public IG4TrackingActionTool, public IG4RunActionTool,  public IG4SteppingActionTool
  {
    
  public:
    /// Standard constructor
    TestActionEHistTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// Retrieve the tracking action interface
    virtual G4UserTrackingAction* getTrackingAction() override final 
    { return static_cast<G4UserTrackingAction*>( getAction() ); }
    /// Retrieve the run action interface
    virtual G4UserRunAction* getRunAction() override final 
    { return static_cast<G4UserRunAction*>( getAction() ); }
    /// Retrieve the stepping action interface
    virtual G4UserSteppingAction* getSteppingAction() override final 
    { return static_cast<G4UserSteppingAction*>( getAction() ); }
    /// Gaudis query interface
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

  protected:
    /// Create aciton for this thread
    virtual std::unique_ptr<TestActionEHist> makeAction() override final;

  private:
    /// holds the configuration details received from python
    TestActionEHist::Config m_config;
  }; // class TestActionEHistTool
  
  
} // namespace G4UA 
#endif
