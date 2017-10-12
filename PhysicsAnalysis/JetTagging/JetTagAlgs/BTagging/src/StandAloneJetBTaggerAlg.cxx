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
  //xAOD::JetContainer* jets = h_JetCollectionName
  //auto rec = xAOD::shallowCopyContainer (*jetsAOD);
  auto rec = xAOD::shallowCopyContainer (*h_JetCollectionName);
  int ret = m_JetBTaggerTool->modify(*rec.first);
  delete rec.first;
  delete rec.second;
  if (!ret) {
    ATH_MSG_DEBUG("#BTAG# Failed to call JetBTaggerTool");
  }

  //std::string BTagName("BTagging_");
  //BTagName += m_JetCollectionName.key();
  //BTagName.erase(BTagName.length()-4);
  //BTagName += m_suffix;
  //m_BTaggingCollectionName.assign(BTagName);
  //ATH_MSG_INFO( " BTaggingCollectionName " << m_BTaggingCollectionName.key()  );
  //m_BTaggingCollectionName.key() = std::string("BTagging_") + m_JetCollectionName.key();
  //no PV0 in BTagging Collection name
  /*std::size_t start_position_PV0 = m_BTaggingCollectionName.key().find("PV0");
  if (start_position_PV0!=std::string::npos) {
    m_BTaggingCollectionName.key().erase(start_position_PV0, 3);
  }*/
  // Try to get the BTaggingContainer and write it on SG
  //SG::WriteHandle<xAOD::BTaggingContainer> h_BTaggingCollectionName (m_BTaggingCollectionName);
  //xAOD::BTaggingContainer * bTaggingContainer = std::make_unique<xAOD::BTaggingContainer>(0);
  //xAOD::BTagging* tagInfo = const_cast<xAOD::BTagging*>(rec.first->btagging());
  //xAOD::BTaggingContainer * bTaggingContainer = std::make_unique<xAOD::BTaggingContainer>(tagInfo);

  //ATH_CHECK( h_BTaggingCollectionName.record(std::move(bTaggingContainer)) );



/*  // Check if there are Jet collection to tag
  if (evtStore()->contains<xAOD::JetContainer > ( m_JetCollectionName )) {
      std::string BTaggingCollectionName = std::string("BTagging_") + m_JetCollectionName;
      BTaggingCollectionName.erase(BTaggingCollectionName.length()-4);
      BTaggingCollectionName += m_suffix;
      //no PV0 in BTagging Collection name
      std::size_t start_position_PV0 = BTaggingCollectionName.find("PV0");
      if (start_position_PV0!=std::string::npos) {
 	BTaggingCollectionName.erase(start_position_PV0, 3);	
      }
      //Check if Jet collection already not tagged
      if (!evtStore()->contains<xAOD::BTaggingContainer>(BTaggingCollectionName)) {
        ATH_MSG_DEBUG("#BTAG# Deep copy of Jet container:" << m_JetCollectionName );
	if (evtStore()->contains<xAOD::JetAuxContainer>(m_JetCollectionName+"Aux.")) {
	  if (overwrite<xAOD::JetContainer, xAOD::JetAuxContainer>(m_JetCollectionName).isFailure()) {
	    ATH_MSG_FATAL( "Couldn't call overwrite with concrete auxiliary store" );
	    return StatusCode::FAILURE;
	  }
	} else if (evtStore()->template contains< xAOD::AuxContainerBase >(m_JetCollectionName+"Aux.")) {
	  if (overwrite<xAOD::JetContainer, xAOD::AuxContainerBase>(m_JetCollectionName).isFailure()) {
	    ATH_MSG_FATAL( "Couldn't call overwrite with generic auxiliary store" );
	    return StatusCode::FAILURE;
	  }
	}
	xAOD::JetContainer* jets(0);
	CHECK( evtStore()->retrieve(jets, m_JetCollectionName) );
        int ret = m_JetBTaggerTool->modify(*jets);
        if (!ret) {
          ATH_MSG_DEBUG("#BTAG# Failed to call JetBTaggerTool");
        }
      }
      else { //Shallow copy for re-tagging already tagged jet
        const xAOD::JetContainer * jetsAOD = 0;
        CHECK( evtStore()->retrieve(jetsAOD, m_JetCollectionName));
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
  else {
    ATH_MSG_VERBOSE("#BTAG# No Jet container " << m_JetCollectionName << " in store, no re-tagging");
  }
*/  
  return 1;

}

template< class CONTAINER, class AUXSTORE >
StatusCode
StandAloneJetBTaggerAlg::overwrite(const std::string& key) const {
 
  // Retrieve the const container:
  const CONTAINER* c = 0;
  ATH_CHECK( evtStore()->retrieve(c, key) );
 
  // Create the new container:
  CONTAINER* copy = new CONTAINER();
  AUXSTORE* copyAux = new AUXSTORE();
  copy->setStore(copyAux);

  // Create the new objects, doing a deep copy if requested
  for (auto oldObj : *c ) {
    auto newObj = new typename CONTAINER::base_value_type();
    copy->push_back(newObj);
    *newObj = *oldObj;
  }
 
  // Do the overwrite:
  ATH_CHECK( evtStore()->overwrite( copy, key, true, false ) );
  ATH_CHECK( evtStore()->overwrite( copyAux, key + "Aux.", true, false ) );
 
  // Return gracefully:
  return StatusCode::SUCCESS;
}

} //// namespace analysis
