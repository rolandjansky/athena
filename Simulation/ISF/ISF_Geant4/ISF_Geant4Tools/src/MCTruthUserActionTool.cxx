/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "MCTruthUserActionTool.h"

namespace G4UA{ 
  
  namespace iGeant4{
    
    MCTruthUserActionTool::MCTruthUserActionTool(const std::string& type, const std::string& name,const IInterface* parent):
      ActionToolBase<MCTruthUserAction>(type, name, parent), m_config(){

      declareProperty("TruthRecordSvc", m_config.truthRecordSvc, "ISF Particle Truth Svc");
      declareProperty("SecondarySavingLevel", m_config.ilevel=2);

    }

    std::unique_ptr<MCTruthUserAction>  MCTruthUserActionTool::makeAction(){
      ATH_MSG_DEBUG("makeAction");
      if(msgLvl(MSG::VERBOSE))    { m_config.verboseLevel = 10; }
      else if(msgLvl(MSG::DEBUG)) { m_config.verboseLevel = 5;  }
      auto action = CxxUtils::make_unique<MCTruthUserAction>(m_config);
      return std::move(action);
    }
    StatusCode MCTruthUserActionTool::queryInterface(const InterfaceID& riid, void** ppvIf){
      
      if(riid == IG4TrackingActionTool::interfaceID()) {
	*ppvIf = (IG4TrackingActionTool*) this;
	addRef();
	return StatusCode::SUCCESS;
      }
      return ActionToolBase<MCTruthUserAction>::queryInterface(riid, ppvIf);
    }

  } // iGeant4
    
} // namespace G4UA 
  
