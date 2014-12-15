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
      //const xAOD::BTagging * BTag = 0;
      //if (jetsAOD->size() != 0) {
	// BTag = jetsAOD->front()->btagging();
        // sc = evtStore()->retrieve(jetsAOD, m_JetCollectionName);
      //}
      //if (BTag == 0) { //Deep copy of jet container even if JetContainer empty
      std::string BTaggingCollectionName = std::string("BTagging_") + m_JetCollectionName;
      BTaggingCollectionName.erase(BTaggingCollectionName.length()-4);
      //no PV0 in BTagging Colection Name
      std::size_t start_position_PV0 = BTaggingCollectionName.find("PV0");
      if (start_position_PV0!=std::string::npos) {
 	BTaggingCollectionName.erase(start_position_PV0, 3);	
      }
      if (!evtStore()->contains<xAOD::BTaggingContainer>(BTaggingCollectionName)) {
        ATH_MSG_DEBUG("#BTAG# Deep copy of Jet container:" << m_JetCollectionName << " with " << jetsAOD->size() << " jets");
        xAOD::JetContainer * jets = new xAOD::JetContainer();
        xAOD::JetAuxContainer * jetsAux = new xAOD::JetAuxContainer();
        jets->setStore(jetsAux);

        xAOD::JetContainer::const_iterator itB = jetsAOD->begin();
        xAOD::JetContainer::const_iterator itE = jetsAOD->end();
        for(xAOD::JetContainer::const_iterator it = itB ; it != itE; ++it) {
          xAOD::Jet * jetToTag = new xAOD::Jet( **it );
          jets->push_back(jetToTag);
        }
        int ret = m_JetBTaggerTool->modify(*jets);
        if (!ret) {
          ATH_MSG_DEBUG("#BTAG# Failed to call JetBTaggerTool");
        }
        else {
          static const bool allowOverwrite = true;
	  static const bool resetOnly = false;
          CHECK( evtStore()->overwrite(jetsAux, m_JetCollectionName+"Aux.", allowOverwrite, resetOnly));
          CHECK( evtStore()->overwrite(jets, m_JetCollectionName, allowOverwrite, resetOnly));
        }
      }
      else { //Shallow copy for re-tagging already tagged jet
        ATH_MSG_DEBUG("#BTAG# Shallow copy of Jet container:" << m_JetCollectionName << " with " << jetsAOD->size() << " jets");
        auto rec = xAOD::shallowCopyContainer (*jetsAOD);
        int ret = m_JetBTaggerTool->modify(*rec.first);
        delete rec.first;
        delete rec.second;
        if (!ret) {
          ATH_MSG_DEBUG("#BTAG# Failed to call JetBTaggerTool");
        }
      }
    }
  }
  else {
    ATH_MSG_VERBOSE("#BTAG# No Jet container " << m_JetCollectionName << " in store, no re-tagging");
  }
  
  return 1;

}

} //// namespace analysis
