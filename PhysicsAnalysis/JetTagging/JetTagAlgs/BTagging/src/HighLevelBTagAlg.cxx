/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/HighLevelBTagAlg.h"

#include "xAODJet/JetContainer.h"
//general interface for secondary vertex finders
#include "InDetRecToolInterfaces/ISecVertexInJetFinder.h"
#include "VxSecVertex/VxSecVKalVertexInfo.h"
#include "VxSecVertex/VxJetFitterVertexInfo.h"
 
#include "StoreGate/ReadDecorHandle.h"
#include "StoreGate/ReadHandle.h"

namespace Analysis {

  HighLevelBTagAlg::HighLevelBTagAlg(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator),
    m_jetDecorator(this)
    //m_vxPrimaryName("PrimaryVertices")
  {
    declareProperty("JetDecorator", m_jetDecorator);
  }

  HighLevelBTagAlg::~HighLevelBTagAlg() {
  }

  StatusCode HighLevelBTagAlg::initialize()
  {
    // This will check that the properties were initialized properly
    // by job configuration.
    ATH_CHECK( m_BTagCollectionName.initialize() );
    m_btagJetLinkName = m_BTagCollectionName.key() + ".jetLink";
    ATH_CHECK( m_btagJetLinkName.initialize() );

    /* ----------------------------------------------------------------------------------- */
    /*                        RETRIEVE SERVICES FROM STOREGATE                             */
    /* ----------------------------------------------------------------------------------- */

    if ( m_jetDecorator.retrieve().isFailure() ) {
      ATH_MSG_ERROR("#BTAG# Failed to retrieve " << m_jetDecorator);
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved " << m_jetDecorator);
    }

    // Prepare decorators
    m_dec_d0       = m_TrackContainerKey.key() + "." + m_prefix.value() + m_dec_d0.key();
    m_dec_z0       = m_TrackContainerKey.key() + "." + m_prefix.value() + m_dec_z0.key();
    m_dec_d0_sigma = m_TrackContainerKey.key() + "." + m_prefix.value() + m_dec_d0_sigma.key();
    m_dec_z0_sigma = m_TrackContainerKey.key() + "." + m_prefix.value() + m_dec_z0_sigma.key();

    m_dec_track_pos = m_TrackContainerKey.key() + "." + m_prefix.value() + m_dec_track_pos.key();
    m_dec_track_mom = m_TrackContainerKey.key() + "." + m_prefix.value() + m_dec_track_mom.key();

    // Initialize decorators
    CHECK( m_dec_d0.initialize() );
    CHECK( m_dec_z0.initialize() );
    CHECK( m_dec_d0_sigma.initialize() );
    CHECK( m_dec_z0_sigma.initialize() );
    CHECK( m_dec_track_pos.initialize() );
    CHECK( m_dec_track_mom.initialize() );

    return StatusCode::SUCCESS;
  }


  StatusCode HighLevelBTagAlg::execute() {
    //retrieve the BTagging container
    SG::ReadHandle< xAOD::BTaggingContainer > h_btagContainer( m_BTagCollectionName);
    if (!h_btagContainer.isValid()) {
      ATH_MSG_ERROR( " cannot retrieve BTagging container with key " << m_BTagCollectionName.key()  );
      return StatusCode::FAILURE;
    }

    if (h_btagContainer->size() == 0) {
     ATH_MSG_DEBUG("#BTAG# Empty BTagging collection");
     return StatusCode::SUCCESS;
    }

    SG::ReadDecorHandle<xAOD::BTaggingContainer, ElementLink< xAOD::JetContainer > > h_btagJetLinkName (m_btagJetLinkName);
    if (!h_btagJetLinkName.isAvailable()) {
      ATH_MSG_ERROR( " cannot retrieve Jet container EL decoration with key " << m_btagJetLinkName.key()  );
      return StatusCode::FAILURE;
    }


    for (xAOD::BTaggingContainer::const_iterator btagIter = h_btagContainer->begin(); btagIter != h_btagContainer->end(); ++btagIter) {
      const ElementLink< xAOD::JetContainer > jetCont = h_btagJetLinkName(**btagIter);
      const xAOD::Jet* jet = *jetCont;
      m_jetDecorator->decorate(*jet);
    }

    return StatusCode::SUCCESS;
  } 

} // namespace
