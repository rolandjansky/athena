/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4CosmicFilter/G4CosmicOrFilterTool.h"

namespace G4UA
{

  G4CosmicOrFilterTool::G4CosmicOrFilterTool(const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
    : ActionToolBaseReport<G4CosmicOrFilter>(type, name, parent),
      m_config()
  {
    declareProperty("CollectionName", m_config.collectionName);
    declareProperty("CollectionName2",m_config.collectionName2);
    declareProperty("CollectionName3",m_config.collectionName3);
  }

  std::unique_ptr<G4CosmicOrFilter> G4CosmicOrFilterTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<G4CosmicOrFilter>(m_config);
    return std::move(action);
  }

  StatusCode G4CosmicOrFilterTool::queryInterface(const InterfaceID& riid, void** ppvIf)
  {
    if(riid == IG4EventActionTool::interfaceID()) {
      *ppvIf = (IG4EventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    } return ActionToolBase<G4CosmicOrFilter>::queryInterface(riid, ppvIf);
  }

  StatusCode G4CosmicOrFilterTool::finalize()
  {
    mergeReports();
    ATH_MSG_INFO( "processed "<< m_report.ntot <<" events, "<< m_report.npass<<" events passed filter" );

    return StatusCode::SUCCESS;
  }

} // namespace G4UA
