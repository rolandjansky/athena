/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4UserActions/PhotonKillerTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  PhotonKillerTool::PhotonKillerTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
    : ActionToolBase<PhotonKiller>(type, name, parent)
  {}

  //---------------------------------------------------------------------------
  std::unique_ptr<PhotonKiller> PhotonKillerTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    return CxxUtils::make_unique<PhotonKiller>();
  }

  //---------------------------------------------------------------------------
  StatusCode PhotonKillerTool::queryInterface(const InterfaceID& riid, void** ppvIf){

    if(riid == ISteppingActionTool::interfaceID()) {
      *ppvIf = (ISteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    } if(riid == IPreTrackingActionTool::interfaceID()) {
      *ppvIf = (IPreTrackingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    } return ActionToolBase<PhotonKiller>::queryInterface(riid, ppvIf);
  }

} // namespace G4UA
