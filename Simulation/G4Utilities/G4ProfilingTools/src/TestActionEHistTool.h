/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef G4PROFILINGTOOLS_G4UA__TESTACTIONEHISTTOOL_H 
#define G4PROFILINGTOOLS_G4UA__TESTACTIONEHISTTOOL_H 

#include "G4AtlasInterfaces/IPreTrackingActionTool.h"
#include "G4AtlasInterfaces/IPostTrackingActionTool.h"
#include "G4AtlasInterfaces/IBeginRunActionTool.h"
#include "G4AtlasInterfaces/IEndRunActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
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
    public IPreTrackingActionTool,  public IPostTrackingActionTool,  public IBeginRunActionTool,  public IEndRunActionTool,  public ISteppingActionTool
  {
    
  public:
    /// Standard constructor
    TestActionEHistTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// Retrieve the begin of tracking action interface
    virtual IPreTrackingAction* getPreTrackingAction() override final 
    { return static_cast<IPreTrackingAction*>( getAction() ); }
    /// Retrieve the end of tracking action interface
    virtual IPostTrackingAction* getPostTrackingAction() override final 
    { return static_cast<IPostTrackingAction*>( getAction() ); }
    /// Retrieve the BoR action interface
    virtual IBeginRunAction* getBeginRunAction() override final 
    { return static_cast<IBeginRunAction*>( getAction() ); }
    /// Retrieve the EoR action interface
    virtual IEndRunAction* getEndRunAction() override final 
    { return static_cast<IEndRunAction*>( getAction() ); }
    /// Retrieve the stepping action interface
    virtual ISteppingAction* getSteppingAction() override final 
    { return static_cast<ISteppingAction*>( getAction() ); }
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
