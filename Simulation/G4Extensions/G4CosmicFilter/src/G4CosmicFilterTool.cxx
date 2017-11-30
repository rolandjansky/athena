/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4CosmicFilter/G4CosmicFilterTool.h"

namespace G4UA
{

  G4CosmicFilterTool::G4CosmicFilterTool(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent)
    : ActionToolBaseReport<G4CosmicFilter>(type, name, parent),
      m_config()
  {
    declareProperty("CollectionName", m_config.collectionName);
    declareProperty("PDGId", m_config.PDGId);
    declareProperty("PtMin", m_config.ptMin);
    declareProperty("PtMax", m_config.ptMax);
  }

  std::unique_ptr<G4CosmicFilter> G4CosmicFilterTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<G4CosmicFilter>(m_config);
    return std::move(action);
  }

  StatusCode G4CosmicFilterTool::queryInterface(const InterfaceID& riid, void** ppvIf)
  {
    if(riid == IG4EventActionTool::interfaceID()) {
      *ppvIf = (IG4EventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    } return ActionToolBase<G4CosmicFilter>::queryInterface(riid, ppvIf);
  }

  StatusCode G4CosmicFilterTool::finalize()
  {
    mergeReports();

    ATH_MSG_INFO( "processed "<< m_report.ntot <<" events, "<< m_report.npass<<" events passed filter" );

    return StatusCode::SUCCESS;
  }

} // namespace G4UA
