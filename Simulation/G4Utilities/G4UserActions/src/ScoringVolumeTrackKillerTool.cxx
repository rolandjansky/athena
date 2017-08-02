/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4UserActions/ScoringVolumeTrackKillerTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  ScoringVolumeTrackKillerTool::ScoringVolumeTrackKillerTool(const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent)
    : ActionToolBase<ScoringVolumeTrackKiller>(type, name, parent)
  {}

  //---------------------------------------------------------------------------
  std::unique_ptr<ScoringVolumeTrackKiller>
  ScoringVolumeTrackKillerTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    return CxxUtils::make_unique<ScoringVolumeTrackKiller>();
  }

  //---------------------------------------------------------------------------
  StatusCode ScoringVolumeTrackKillerTool::queryInterface(const InterfaceID& riid,
                                                          void** ppvIf)
  {
    if(riid == IEndEventActionTool::interfaceID()) {
      *ppvIf = (IEndEventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    } if(riid == ISteppingActionTool::interfaceID()) {
      *ppvIf = (ISteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    } return ActionToolBase<ScoringVolumeTrackKiller>::queryInterface(riid, ppvIf);
  }

} // namespace G4UA
