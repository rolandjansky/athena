/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/StandAloneJetBTaggerAlg.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
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
  m_JetCollectionName(""),
  m_JetBTaggerTool("Analysis::JetBTaggerTool")
{
  declareProperty( "JetBTaggerTool", m_JetBTaggerTool);
  declareProperty( "JetCollectionName", m_JetCollectionName );
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
 
  return StatusCode::SUCCESS;
}


StatusCode StandAloneJetBTaggerAlg::execute() {

  const xAOD::JetContainer * jetsAOD = 0;
  //Retrieve the Jet container
  if (evtStore()->contains<xAOD::JetContainer > ( m_JetCollectionName )) {
    StatusCode sc = evtStore()->retrieve(jetsAOD, m_JetCollectionName);
    if (sc.isFailure()) {
      ATH_MSG_DEBUG("#BTAG# Failed to retrieve JetContainer in AOD" << m_JetCollectionName);
      return StatusCode::FAILURE;
    }
    else {
      auto rec = xAOD::shallowCopyContainer (*jetsAOD);
      int ret = m_JetBTaggerTool->modify(*rec.first);
      if (!ret) {
        ATH_MSG_DEBUG("#BTAG# Failed to call JetBTaggerTool");
      }
    }
  }
  else {
    ATH_MSG_VERBOSE("#BTAG# No Jet container " << m_JetCollectionName << " in store, no re-tagging");
  }
  
  return 1;

}

} //// namespace analysis
