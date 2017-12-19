/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/StandAloneJetBTaggerAlg.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODBTagging/BTagVertexContainer.h"
#include "xAODBTagging/BTagVertexAuxContainer.h"

#include "BTagging/JetBTaggerTool.h"

#include <iostream>
#include <string>

namespace Analysis {


StandAloneJetBTaggerAlg::StandAloneJetBTaggerAlg(const std::string& n, ISvcLocator *p) : 
  AthAlgorithm(n,p),
  //m_BTagName(""),
  m_suffix(""),
  m_JetBTaggerTool("Analysis::JetBTaggerTool")
{
  declareProperty( "JetBTaggerTool", m_JetBTaggerTool);
  declareProperty( "outputCollectionSuffix", m_suffix );
}

StandAloneJetBTaggerAlg::~StandAloneJetBTaggerAlg()
{
  /////....
}


StatusCode StandAloneJetBTaggerAlg::initialize() {

  /// retrieve the main JetBTaggerTool
  if ( m_JetBTaggerTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_JetBTaggerTool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_JetBTaggerTool);
  }

  // This will check that the properties were initialized properly
  // by job configuration.
  ATH_CHECK( m_JetCollectionName.initialize() );
  ATH_CHECK( m_BTaggingCollectionName.initialize() );
 
  return StatusCode::SUCCESS;
}


StatusCode StandAloneJetBTaggerAlg::execute() {
  SG::ReadHandle<xAOD::JetContainer> h_JetCollectionName (m_JetCollectionName);
  if (!h_JetCollectionName.isValid()) {
        ATH_MSG_ERROR( " cannot retrieve jet container with key " << m_JetCollectionName.key()  );
        return StatusCode::FAILURE;
    }
  // Should work only with AntiKtEMTopoJets collection, only this collection is b-tagged during reconstruction
  auto rec = xAOD::shallowCopyContainer (*h_JetCollectionName);
  int ret = m_JetBTaggerTool->modify(*rec.first);
  delete rec.first;
  delete rec.second;
  if (!ret) {
    ATH_MSG_DEBUG("#BTAG# Failed to call JetBTaggerTool");
  }

  // Return gracefully:
  return StatusCode::SUCCESS;

}

} //// namespace analysis
