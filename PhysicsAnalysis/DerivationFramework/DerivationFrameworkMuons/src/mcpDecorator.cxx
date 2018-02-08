/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkMuons/mcpDecorator.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "xAODMuon/MuonContainer.h"

#include "AthenaKernel/errorcheck.h"

namespace DerivationFramework {
  mcpDecorator::mcpDecorator(const std::string& t, const std::string& n, const IInterface* p):
    AthAlgTool(t, n, p),
    m_parser(nullptr)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("TargetContainer",    m_containerName = "InDetTrackParticles");
    declareProperty("SelectionString",    m_selectionString = "");
    declareProperty("Prefix",             m_prefix="MuonEntry_");
  }

  mcpDecorator::~mcpDecorator() {}

  StatusCode mcpDecorator::initialize() {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    // Set up the text-parsing machinery for thinning the tracks directly according to user cuts
    if (m_selectionString!="") {
            ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader(); // not deleted
            proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore())); // not deleted
            m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
            m_parser->loadExpression(m_selectionString);
    }

    return StatusCode::SUCCESS;
  }

  StatusCode mcpDecorator::finalize() {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

    /// clear parser
    if (m_parser) {
        delete m_parser;
        m_parser = 0;
    }


    return StatusCode::SUCCESS;
  }

  StatusCode DerivationFramework::mcpDecorator::addBranches() const
  {
    // retrieve tag (muon) container
    const xAOD::MuonContainer* toDecorate = 0;
    if(evtStore()->retrieve(toDecorate, m_containerName).isFailure()) {
      ATH_MSG_FATAL( "Unable to retrieve " << m_containerName );
      return StatusCode::FAILURE;
    }

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
