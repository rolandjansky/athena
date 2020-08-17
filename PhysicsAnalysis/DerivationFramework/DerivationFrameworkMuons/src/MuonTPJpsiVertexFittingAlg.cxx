/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "DerivationFrameworkMuons/MuonTPJpsiVertexFittingAlg.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/ITHistSvc.h"
#include "xAODEventInfo/EventInfo.h"

# if !defined(XAOD_ANALYSIS)    
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#endif

using std::string;

//**********************************************************************

MuonTPJpsiVertexFittingAlg::MuonTPJpsiVertexFittingAlg(const std::string& name, ISvcLocator* pSvcLocator) :
            AthAlgorithm(name, pSvcLocator) {
# if !defined(XAOD_ANALYSIS)
    declareProperty("AugmentationTools", m_augmentation_tools);
#endif
}

//**********************************************************************

MuonTPJpsiVertexFittingAlg::~MuonTPJpsiVertexFittingAlg() {
}

//**********************************************************************

StatusCode MuonTPJpsiVertexFittingAlg::initialize() {

    ATH_MSG_DEBUG("in initialize - retrieving tools");

# if !defined(XAOD_ANALYSIS)    
    ATH_CHECK(m_augmentation_tools.retrieve());
#endif

    return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode MuonTPJpsiVertexFittingAlg::finalize() {

    return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode MuonTPJpsiVertexFittingAlg::execute() {

    ATH_MSG_DEBUG("in execute");

# if !defined(XAOD_ANALYSIS)    
    for (auto tool : m_augmentation_tools) {
        ATH_MSG_DEBUG("now calling addBranches for " << tool->name());
        ATH_CHECK(tool->addBranches());
    }
#endif
    return StatusCode::SUCCESS;
}

//**********************************************************************
