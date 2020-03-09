/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */


#include "BTagging/JetParticleAssociationAlg.h"
#include "StoreGate/WriteDecorHandle.h"
#include "ParticleJetTools/ParticleToJetAssociator.h"
#include "xAODTracking/TrackParticleContainer.h"

using namespace std;
using namespace xAOD;

namespace Analysis {

  JetParticleAssociationAlg::JetParticleAssociationAlg(
      const string& name
      , ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator),
      m_assoc("Analysis::JetParticleAssociation",this),
      m_TrackToJetAssociator("Analysis::ParticleToJetAssociator", this),
      m_release("21")
    {
      declareProperty("Associator", m_assoc);
      declareProperty( "TrackToJetAssociator",            m_TrackToJetAssociator);
      declareProperty( "Release", m_release);
    }

  StatusCode JetParticleAssociationAlg::initialize() {
    // This will check that the properties were initialized properly
    // by job configuration.
    ATH_CHECK( m_JetCollectionName.initialize() );
    ATH_CHECK( m_TrackParticleContainerName.initialize() );
    ATH_CHECK( m_jetParticleLinkName.initialize() );

    if (m_release == "21") {
      if(m_TrackToJetAssociator.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve tools " << m_TrackToJetAssociator);
        return StatusCode::FAILURE;
      }
    }
    else {//release == 22
      if ( m_assoc.retrieve().isFailure() ) {
        ATH_MSG_FATAL(name() + " jet-particle association tool retrieval failed.");
        return StatusCode::FAILURE;
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode JetParticleAssociationAlg::execute() {
    //retrieve the Jet container
    SG::ReadHandle<xAOD::JetContainer> h_JetCollectionName (m_JetCollectionName);
    if (!h_JetCollectionName.isValid()) {
      ATH_MSG_ERROR( " cannot retrieve jet container with key " << m_JetCollectionName.key()  );
      return StatusCode::FAILURE;
    }

    //retrieve the Particle container
    SG::ReadHandle<xAOD::TrackParticleContainer> h_TrackParticleContainerName (m_TrackParticleContainerName);
    if (!h_TrackParticleContainerName.isValid()) {
      ATH_MSG_ERROR( " cannot retrieve particle container with key " << m_TrackParticleContainerName.key()  );
      return StatusCode::FAILURE;
    }

    //Decor Jet with vector of element link
    SG::WriteDecorHandle<xAOD::JetContainer,vector<ElementLink< TrackParticleContainer> > > h_jetParticleLinkName(m_jetParticleLinkName);

    if (m_release == "21") {
          // compute the associations
      std::vector<std::vector<const xAOD::TrackParticle*>*> assocs =
              m_TrackToJetAssociator->associateParticlesToJets<std::vector<const xAOD::TrackParticle*>, xAOD::TrackParticleContainer>( h_JetCollectionName.ptr(), h_TrackParticleContainerName.ptr(), "'BTagTrackToJetAssociator ala 21" );

      unsigned int i = 0;
      for (size_t jetIndex=0; jetIndex < h_JetCollectionName->size() ; ++jetIndex) {
        const xAOD::Jet * jet = h_JetCollectionName->at(jetIndex);
        std::vector< ElementLink< xAOD::TrackParticleContainer > > associationLinks;
        for (std::vector<const xAOD::TrackParticle*>::const_iterator trkIter = assocs[i]->begin(); trkIter != assocs[i]->end(); ++trkIter) {
          ElementLink<xAOD::TrackParticleContainer> EL;
          EL.toContainedElement(*h_TrackParticleContainerName, *trkIter);
          associationLinks.push_back(EL);
        }
        ++i;

        h_jetParticleLinkName(*jet) = associationLinks;
      }
    }
    else { //release == 22
 
      const vector<vector<ElementLink<IParticleContainer> > >* matches = m_assoc->match(*(h_JetCollectionName.ptr()),*(h_TrackParticleContainerName.ptr()));

      for (size_t jetIndex=0; jetIndex < h_JetCollectionName->size() ; ++jetIndex) {
        const xAOD::Jet * jet = h_JetCollectionName->at(jetIndex);
        vector<ElementLink<TrackParticleContainer>> el_tp;
        for (const ElementLink<IParticleContainer>& el : (*matches)[jetIndex]) {
          el_tp.emplace_back (el.dataID(), el.index(), el.source());
        }

        h_jetParticleLinkName(*jet) = el_tp;
      }
    }

    return StatusCode::SUCCESS;
  }

  JetParticleAssociationAlg::~JetParticleAssociationAlg()
  {
    /////....
  }


}  //// namespace analysis
