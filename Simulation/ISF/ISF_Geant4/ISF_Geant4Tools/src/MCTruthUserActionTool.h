/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_G4UA__MCTRUTHUSERACTIONTOOL_H 
#define ISF_GEANT4TOOLS_G4UA__MCTRUTHUSERACTIONTOOL_H 
#include "G4AtlasInterfaces/IPreTrackingActionTool.h"
#include "G4AtlasInterfaces/IPostTrackingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "MCTruthUserAction.h"

namespace G4UA{ 
  
  namespace iGeant4{
  

    /// @class MCTruthUserActionTool
    /// @brief Tool which manages the MCTruthUserAction
    ///
    /// @author Andrea Di Simone
    ///
  
    class MCTruthUserActionTool: 
    public ActionToolBase<MCTruthUserAction>,
      public IPreTrackingActionTool,  public IPostTrackingActionTool
    {
      
    public:
      /// Standard constructor
      MCTruthUserActionTool(const std::string& type, const std::string& name,const IInterface* parent);
      /// Retrieve the preTracking action
      virtual IPreTrackingAction* getPreTrackingAction() override final 
      { return static_cast<IPreTrackingAction*>( getAction() ); }
      /// Retrieve the postTracking action
      virtual IPostTrackingAction* getPostTrackingAction() override final 
      { return static_cast<IPostTrackingAction*>( getAction() ); }

      /// Query interface for gaudi
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
    protected:
      /// Create an action for this thread
      virtual std::unique_ptr<MCTruthUserAction> makeAction() override final;
    private:
      /// Configuration parameters
      MCTruthUserAction::Config m_config;
    }; // class MCTruthUserActionTool
    
  }// iGeant4
} // namespace G4UA 
#endif
  
