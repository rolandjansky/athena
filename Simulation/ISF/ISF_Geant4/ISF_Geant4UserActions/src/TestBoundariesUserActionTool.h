/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4USERACTIONS_G4UA_IGEANT4_TESTBOUNDARIESUSERACTIONTOOL_H 
#define ISF_GEANT4USERACTIONS_G4UA_IGEANT4_TESTBOUNDARIESUSERACTIONTOOL_H 
#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "TestBoundariesUserAction.h"


namespace G4UA{ 

  namespace iGeant4{
    
    /// @class TestBoundariesUserActionTool
    /// @brief A class to manage the TestBoundariesUserAction
    ///
    /// creates one action instance per thread
    ///
    /// @author Andrea Di Simone

    class TestBoundariesUserActionTool: 
    public ActionToolBase<TestBoundariesUserAction>,
      public IG4RunActionTool, public IG4SteppingActionTool
    {
      
    public:
      /// standard tool ctor
      TestBoundariesUserActionTool(const std::string& type, const std::string& name,const IInterface* parent);
      /// gets the run action
      virtual G4UserRunAction* getRunAction() override final 
      { return static_cast<G4UserRunAction*>( getAction() ); }
      /// gets the stepping action
      virtual G4UserSteppingAction* getSteppingAction() override final 
      { return static_cast<G4UserSteppingAction*>( getAction() ); }
      /// Gaudi interface handling
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
    protected:
      /// creates action for this thread
      virtual std::unique_ptr<TestBoundariesUserAction> makeAction() override final;
    private:
    }; // class TestBoundariesUserActionTool
    
  } // namespace iGeant4

} // namespace G4UA 
#endif
