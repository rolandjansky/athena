/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/JetBTaggerAlg.h"

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
#include "StoreGate/WriteDecorHandle.h"

#include <iostream>
#include <string>

namespace Analysis {


  JetBTaggerAlg::JetBTaggerAlg(const std::string& n, ISvcLocator *p) : 
    AthAlgorithm(n,p),
    m_JetName(""),
    m_BTagLink(".btaggingLink"),
    m_bTagTool("Analysis::BTagTool",this),
    m_BTagTrackAssocTool("Analysis::BTagTrackAssociation",this),
    m_bTagSecVtxTool("Analysis::BTagSecVertexing",this)
  {
    declareProperty("JetCalibrationName", m_JetName);
    declareProperty("BTaggingLink", m_BTagLink);
    declareProperty("BTagTool", m_bTagTool);
    declareProperty("BTagTrackAssocTool", m_BTagTrackAssocTool);
    declareProperty("BTagSecVertexing", m_bTagSecVtxTool);
  }

  JetBTaggerAlg::~JetBTaggerAlg()
  {
    /////....
  }


  StatusCode JetBTaggerAlg::initialize() {

    // This will check that the properties were initialized properly
    // by job configuration.
    ATH_CHECK( m_JetCollectionName.initialize() );
    ATH_CHECK( m_BTaggingCollectionName.initialize() );
    m_jetBTaggingLinkName = m_JetCollectionName.key() + m_BTagLink;
    ATH_CHECK( m_jetBTaggingLinkName.initialize() );
    m_bTagJetDecorLinkName = m_BTaggingCollectionName.key() + ".jetLink";
    ATH_CHECK( m_bTagJetDecorLinkName.initialize() );
   
    /// retrieve the main BTagTool
    if ( m_bTagTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_bTagTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_bTagTool);
    }

    /// retrieve the track association tool
    if ( !m_BTagTrackAssocTool.empty() ) {
      if ( m_BTagTrackAssocTool.retrieve().isFailure() ) {
        ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_BTagTrackAssocTool);
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_BTagTrackAssocTool);
      }
    }
    else {
      ATH_MSG_DEBUG("#BTAG# No track association tool to retrieve");
    }

    /// retrieve the bTagSecVtxTool
    if ( m_bTagSecVtxTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAGVTX# Failed to retrieve tool " << m_bTagSecVtxTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("#BTAGVTX# Retrieved tool " << m_bTagSecVtxTool);
    }

    /// handle to the magnetic field cache
    ATH_CHECK( m_fieldCacheCondObjInputKey.initialize() );

    return StatusCode::SUCCESS;
  }


  StatusCode JetBTaggerAlg::execute() {
      
    EventContext ctx = Gaudi::Hive::currentContext();

    //retrieve the Jet container
    SG::ReadHandle<xAOD::JetContainer> h_JetCollectionName (m_JetCollectionName, ctx);
    if (!h_JetCollectionName.isValid()) {
      ATH_MSG_ERROR( " cannot retrieve jet container with key " << m_JetCollectionName.key()  );
      return StatusCode::FAILURE;
    }

    if (h_JetCollectionName->size() == 0) {
      ATH_MSG_DEBUG("#BTAG# Empty JetContainer !!");
    }
    else {
      ATH_MSG_DEBUG("#BTAG#  Nb jets in JetContainer: "<< h_JetCollectionName->size());
    }

    //Decor Jet with element link to the BTagging
    SG::WriteDecorHandle<xAOD::JetContainer,ElementLink< xAOD::BTaggingContainer > > h_jetBTaggingLinkName(m_jetBTaggingLinkName, ctx);
    //Decor BTagging with element link to the Jet
    SG::WriteDecorHandle<xAOD::BTaggingContainer, ElementLink< xAOD::JetContainer > > h_bTagJetLinkName(m_bTagJetDecorLinkName, ctx);

    //Create a xAOD::BTaggingContainer in any case (must be done)
    std::string bTaggingContName = m_BTaggingCollectionName.key();
    ATH_MSG_DEBUG("#BTAG#  Container name: "<< bTaggingContName);

    /* Record the BTagging  output container */
    SG::WriteHandle<xAOD::BTaggingContainer> h_BTaggingCollectionName (m_BTaggingCollectionName, ctx);
    ATH_CHECK( h_BTaggingCollectionName.record(std::make_unique<xAOD::BTaggingContainer>(),
                    std::make_unique<xAOD::BTaggingAuxContainer>()) );

    MagField::AtlasFieldCache    fieldCache;
    // Get field cache object
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
    const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
   
    if (fieldCondObj == nullptr) {
      ATH_MSG_ERROR("execute: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCacheCondObjInputKey.key());
      return StatusCode::FAILURE;
    }
    fieldCondObj->getInitializedCache (fieldCache);

    if (!fieldCache.solenoidOn()) {
      for (size_t jetIndex=0; jetIndex < h_JetCollectionName->size() ; ++jetIndex) {
        const xAOD::Jet * jet = h_JetCollectionName->at(jetIndex);
        ElementLink< xAOD::BTaggingContainer> linkBTagger;
        h_jetBTaggingLinkName(*jet) = linkBTagger;
      }
      return StatusCode::SUCCESS;
    }
    else { //Solenoid ON
      for (unsigned int i = 0; i < h_JetCollectionName->size(); i++) {
        xAOD::BTagging * newBTagMT  = new xAOD::BTagging();
        h_BTaggingCollectionName->push_back(newBTagMT);
      }
    }


    StatusCode jetIsAssociated;
    if (!m_BTagTrackAssocTool.empty()) {
      ATH_MSG_VERBOSE("#BTAG# Track association tool is not empty");
      jetIsAssociated = m_BTagTrackAssocTool->BTagTrackAssociation_exec(h_JetCollectionName.ptr(), h_BTaggingCollectionName.ptr());
      if ( jetIsAssociated.isFailure() ) {
        ATH_MSG_ERROR("#BTAG# Failed to associate tracks to jet ");
        return StatusCode::FAILURE;
      }
    }
    else {
      ATH_MSG_WARNING("#BTAG# Empty track association tool ");
    }


    // Secondary vertex reconstruction.
    StatusCode SV = m_bTagSecVtxTool->BTagSecVtx_exec(h_JetCollectionName.ptr(), h_BTaggingCollectionName.ptr());
    if (SV.isFailure()) {
      ATH_MSG_WARNING("#BTAG# Failed to reconstruct sec vtx");
    }

    //Tag the jets
    SV = m_bTagTool->tagJet( h_JetCollectionName.ptr(), h_BTaggingCollectionName.ptr(), m_JetName);
    if (SV.isFailure()) {
      ATH_MSG_WARNING("#BTAG# Failed in taggers call");
    }

    //Create the element link from the jet to the btagging and reverse link
    for (size_t jetIndex=0; jetIndex < h_JetCollectionName->size() ; ++jetIndex) {
      const xAOD::Jet * jetToTag = h_JetCollectionName->at(jetIndex);
      xAOD::BTagging * itBTag = h_BTaggingCollectionName->at(jetIndex);
      ElementLink< xAOD::BTaggingContainer> linkBTagger;
      linkBTagger.toContainedElement(*h_BTaggingCollectionName.ptr(), itBTag);
      h_jetBTaggingLinkName(*jetToTag) = linkBTagger;
      ElementLink< xAOD::JetContainer> linkJet;
      linkJet.toContainedElement(*h_JetCollectionName.ptr(), jetToTag);
      h_bTagJetLinkName(*itBTag) = linkJet;
    }

    return StatusCode::SUCCESS;

  }

} //// namespace analysis
