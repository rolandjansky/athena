/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkMuons/mcpDecorator.h"
#include "xAODMuon/MuonContainer.h"

#include "AthenaKernel/errorcheck.h"

namespace DerivationFramework {
  mcpDecorator::mcpDecorator(const std::string& t, const std::string& n, const IInterface* p):
    ExpressionParserUser<AthAlgTool>(t, n, p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("TargetContainer",    m_containerName = "InDetTrackParticles");
    declareProperty("SelectionString",    m_selectionString = "");
    declareProperty("Prefix",             m_prefix="MuonEntry_");
  }

  StatusCode mcpDecorator::initialize() {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    // Set up the text-parsing machinery for thinning the tracks directly according to user cuts
    if (!m_selectionString.empty()) {
       ATH_CHECK( initializeParser(m_selectionString) );
    }

    return StatusCode::SUCCESS;
  }

  StatusCode DerivationFramework::mcpDecorator::addBranches() const {
    // retrieve tag (muon) container
    const xAOD::MuonContainer* toDecorate = nullptr;   
     ATH_CHECK(evtStore()->retrieve(toDecorate, m_containerName));

    // Execute the text parser and update the mask
    std::vector<int> entries(toDecorate->size(), 1);
    if (m_parser) {
        entries =  m_parser->evaluateAsVector();
        if (entries.size() != toDecorate->size()) {
            ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used ID TrackParticles?");
            return StatusCode::FAILURE;
        }
    }
    return StatusCode::SUCCESS;
  }
}
