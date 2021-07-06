/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "DerivationFrameworkMuons/MuonTPJpsiVertexFittingAlg.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/ITHistSvc.h"
#include "xAODEventInfo/EventInfo.h"


MuonTPJpsiVertexFittingAlg::MuonTPJpsiVertexFittingAlg(const std::string& name, ISvcLocator* pSvcLocator) :
            AthAlgorithm(name, pSvcLocator) {
    declareProperty("AugmentationTools", m_augmentation_tools);
}
StatusCode MuonTPJpsiVertexFittingAlg::initialize() {
    ATH_MSG_DEBUG("in initialize - retrieving tools");
    ATH_CHECK(m_augmentation_tools.retrieve());
    return StatusCode::SUCCESS;
}

StatusCode MuonTPJpsiVertexFittingAlg::execute() {

    ATH_MSG_DEBUG("in execute");
    for (auto tool : m_augmentation_tools) {
        ATH_MSG_DEBUG("now calling addBranches for " << tool->name());
        ATH_CHECK(tool->addBranches());
    }
    return StatusCode::SUCCESS;
}
