/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/HighLevelBTagAlg.h"

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
    //declareProperty("PrimaryVertexName",  m_vxPrimaryName);
    //List of the secondary vertex finders in jet to be run
    declareProperty("BTaggingLink", m_BTagLink);
    declareProperty("JetDecorator", m_jetDecorator);
  }

  HighLevelBTagAlg::~HighLevelBTagAlg() {
  }

  StatusCode HighLevelBTagAlg::initialize()
  {
    // This will check that the properties were initialized properly
    // by job configuration.
    ATH_CHECK( m_JetCollectionName.initialize() );
    m_jetBTaggingLinkName = m_JetCollectionName.key() + m_BTagLink;
    ATH_CHECK( m_jetBTaggingLinkName.initialize() );

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
    //retrieve the Jet container
    SG::ReadHandle<xAOD::JetContainer> h_JetCollectionName (m_JetCollectionName);
    if (!h_JetCollectionName.isValid()) {
      ATH_MSG_ERROR( " cannot retrieve jet container with key " << m_JetCollectionName.key()  );
      return StatusCode::FAILURE;
    }

    for (xAOD::JetContainer::const_iterator jetIter = h_JetCollectionName->begin(); jetIter != h_JetCollectionName->end(); ++jetIter) {
      const xAOD::Jet& jetToTag = **jetIter;

      m_jetDecorator->decorate(jetToTag);   
    }// for loop on jets

    return StatusCode::SUCCESS;
  } 

} // namespace
