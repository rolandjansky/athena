/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "PhysicsValidationUserActionTool.h"

namespace G4UA{

  namespace iGeant4{

    PhysicsValidationUserActionTool::PhysicsValidationUserActionTool(const std::string& type, const std::string& name,const IInterface* parent):
      ActionToolBase<PhysicsValidationUserAction>(type, name, parent), m_config(){
    }

    std::unique_ptr<PhysicsValidationUserAction>  PhysicsValidationUserActionTool::makeAction(){
      ATH_MSG_DEBUG("makeAction");
      if(msgLvl(MSG::VERBOSE))    { m_config.verboseLevel = 10; }
      else if(msgLvl(MSG::DEBUG)) { m_config.verboseLevel = 5;  }
      auto action = CxxUtils::make_unique<PhysicsValidationUserAction>(m_config);
      return std::move(action);
    }

    StatusCode PhysicsValidationUserActionTool::queryInterface(const InterfaceID& riid, void** ppvIf){

      if(riid == IBeginEventActionTool::interfaceID()) {
        *ppvIf = (IBeginEventActionTool*) this;
        addRef();
        return StatusCode::SUCCESS;
      }
      if(riid == IEndEventActionTool::interfaceID()) {
        *ppvIf = (IEndEventActionTool*) this;
        addRef();
        return StatusCode::SUCCESS;
      }
      if(riid == IBeginRunActionTool::interfaceID()) {
        *ppvIf = (IBeginRunActionTool*) this;
        addRef();
        return StatusCode::SUCCESS;
      }
      if(riid == ISteppingActionTool::interfaceID()) {
        *ppvIf = (ISteppingActionTool*) this;
        addRef();
        return StatusCode::SUCCESS;
      }
      if(riid == IPreTrackingActionTool::interfaceID()) {
        *ppvIf = (IPreTrackingActionTool*) this;
        addRef();
        return StatusCode::SUCCESS;
      }
      return ActionToolBase<PhysicsValidationUserAction>::queryInterface(riid, ppvIf);
    }

  } // iGeant4

} // namespace G4UA
