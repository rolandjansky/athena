/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4UserActions/FastIDKillerTool.h"

namespace G4UA
{

  FastIDKillerTool::FastIDKillerTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
    : ActionToolBaseReport<FastIDKiller>(type, name, parent)
  {
    declareProperty("R", m_config.R);
    declareProperty("Z", m_config.Z);
  }

  std::unique_ptr<FastIDKiller> FastIDKillerTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    return CxxUtils::make_unique<FastIDKiller>(m_config);
  }

  StatusCode FastIDKillerTool::queryInterface(const InterfaceID& riid, void** ppvIf)
  {
    if(riid == IBeginRunActionTool::interfaceID())
      {
        *ppvIf = (IBeginRunActionTool*) this;
        addRef();
        return StatusCode::SUCCESS;
      }
    if(riid == ISteppingActionTool::interfaceID())
      {
        *ppvIf = (ISteppingActionTool*) this;
        addRef();
        return StatusCode::SUCCESS;
      }
    return ActionToolBase<FastIDKiller>::queryInterface(riid, ppvIf);
  }

  StatusCode FastIDKillerTool::finalize()
  {
    mergeReports();
    ATH_MSG_INFO( "Fast Inner Detector Killer killed " << m_report.killCount <<
                  " particles during this run." );
    return StatusCode::SUCCESS;
  }

} // namespace G4UA
