/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/BTagTrackAssociation.h"
#include "BTagging/IBTagTrackAssociation.h"

#include "xAODBTagging/BTagging.h"

#include "xAODJet/Jet.h"
// #include "JetEvent/JetCollection.h"

#include "xAODTracking/TrackParticleContainer.h"
// #include "Particle/TrackParticle.h" 
//#include "Particle/TrackParticleContainer.h"

#include "ParticleJetTools/ParticleToJetAssociator.h"

// // // For Soft Lepton Tag
// #include "egammaEvent/ElectronAssociation.h"
// #include "egammaEvent/PhotonAssociation.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"


namespace Analysis {

  BTagTrackAssociation::BTagTrackAssociation(const std::string& t, const std::string& n, const IInterface* p) :
    AthAlgTool(t,n,p),
    m_BTagAssociation(true)
  {
      declareInterface<IBTagTrackAssociation>(this);
      declareProperty( "BTagAssociation",                     m_BTagAssociation);

      declareProperty( "TrackToJetAssociatorList",            m_TrackToJetAssociatorList);
      declareProperty( "ElectronToJetAssociatorList",         m_ElectronToJetAssociatorList );
      declareProperty( "MuonToJetAssociatorList",             m_MuonToJetAssociatorList );

      declareProperty( "TrackToJetAssocNameList",             m_TrackToJetAssocNameList);
      declareProperty( "ElectronToJetAssocNameList",          m_ElectronToJetAssocNameList);
      declareProperty( "PhotonToJetAssocNameList",            m_PhotonToJetAssocNameList);
      declareProperty( "MuonToJetAssocNameList",              m_MuonToJetAssocNameList);

      declareProperty( "TrackContainerNameList",              m_TrackContainerNameList);
      declareProperty( "ElectronContainerNameList",           m_ElectronContainerNameList ); //for SoftEl
      declareProperty( "PhotonContainerNameList",             m_PhotonContainerNameList);   //for SoftEl
      declareProperty( "MuonContainerNameList",               m_MuonContainerNameList );     //for SoftMu


      // if this option is set, run tagging on the named collection
      // already attached to the jet instead of performing the
      // association.
      declareProperty( "ParticlesToAssociateList",            m_ParticlesToAssociateList=std::vector<std::string>() );
      declareProperty( "MuonsToAssociateList",            m_MuonsToAssociateList=std::vector<std::string>() );
  }

  BTagTrackAssociation::~BTagTrackAssociation() {
  }

  StatusCode BTagTrackAssociation::initialize()
  {

    /* ----------------------------------------------------------------------------------- */
    /*                        RETRIEVE SERVICES FROM STOREGATE                             */
    /* ----------------------------------------------------------------------------------- */

    if(m_TrackToJetAssociatorList.size() != m_TrackToJetAssocNameList.size()){
      ATH_MSG_FATAL("Size mismatch between track assoc tools("
		    << m_TrackToJetAssociatorList.size() << ") and names( "
		    << m_TrackToJetAssocNameList.size() << ")");
      return StatusCode::FAILURE;
    }
    if(m_TrackToJetAssociatorList.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve tools " << m_TrackToJetAssociatorList);
      return StatusCode::FAILURE;
    } else {
      if (m_TrackToJetAssociatorList.size() == 0) {
	ATH_MSG_WARNING(" No tool for track to jet association");
      }
      else {
        ATH_MSG_INFO("Retrieved tools " << m_TrackToJetAssociatorList);
      }
    }

    // It isn't obvious that we will need the electron association in the future; so comment it out
    // if(2*m_ElectronToJetAssociatorList.size() != m_ElectronContainerNameList.size() + m_PhotonContainerNameList.size()){ // because use the same tool for electrons and photons
    //   msg( MSG::FATAL ) << "Size mismatch between electron/photon assoc tools("
    // 			<<m_ElectronToJetAssociatorList.size()<<") and names( "
    // 			<<m_ElectronContainerNameList.size()<< "+" << m_PhotonContainerNameList.size()<<  ")"  << endreq;
    //   return StatusCode::FAILURE;
    // }
    // if(m_ElectronToJetAssociatorList.retrieve().isFailure() ) {
    //   msg( MSG::FATAL ) << "Failed to retrieve tools " << m_ElectronToJetAssociatorList << endreq;
    //   return StatusCode::FAILURE;
    // } else {
    //   msg( MSG::INFO ) << "Retrieved tools " << m_ElectronToJetAssociatorList << endreq;
    // }


     if(m_MuonToJetAssociatorList.size() != m_MuonContainerNameList.size()){
       ATH_MSG_FATAL("Size mismatch between muon assoc tools("
		     << m_MuonToJetAssociatorList.size() << ") and names( "
		     << m_MuonContainerNameList.size() << ")");
       return StatusCode::FAILURE;
     }
     if(m_MuonToJetAssociatorList.retrieve().isFailure() ) {
       ATH_MSG_FATAL("Failed to retrieve tools " << m_MuonToJetAssociatorList);
       return StatusCode::FAILURE;
     } else {
       ATH_MSG_INFO("Retrieved tools " << m_MuonToJetAssociatorList);
     }


    return StatusCode::SUCCESS;
  }

  StatusCode BTagTrackAssociation::finalize()
  {
      return StatusCode::SUCCESS;
  }

  StatusCode BTagTrackAssociation::BTagTrackAssociation_exec(jetcollection_t* theJets, const xAOD::TrackParticleContainer* tracks) const {

      /* ----------------------------------------------------------------------------------- */
      /*               Particle to Jet Associations                                          */
      /* ----------------------------------------------------------------------------------- */

    // new style associations
    if ( m_ParticlesToAssociateList.size() || m_MuonsToAssociateList.size() ) { 
      StatusCode sc;
      int i = 0;
      for (const std::string& trackColName : m_ParticlesToAssociateList) {

        for (const xAOD::Jet* jet : *theJets) {
          xAOD::BTagging* tagInfo = const_cast<xAOD::BTagging*>(jet->btagging());
          if (!tagInfo) {
            ATH_MSG_FATAL("The pointer from Jet to BTagging object is invalid");
            return StatusCode::FAILURE;
          }   

          std::vector< ElementLink< xAOD::IParticleContainer > > tmp;
          if (!jet->getAttribute(trackColName, tmp)) {
            ATH_MSG_FATAL("Unable to read track collection " + trackColName + " from jets for b-tagging.");
            return StatusCode::FAILURE;
          }   

          std::vector< ElementLink< xAOD::TrackParticleContainer > > associationLinks;
          const xAOD::TrackParticleContainer * tpContainer( 0 );
          sc = evtStore()->retrieve( tpContainer, m_TrackContainerNameList[i]);
          if ( sc.isFailure() || tpContainer==0) {
            ATH_MSG_ERROR("#BTAG# Failed to retrieve TrackParticles through name: " << m_TrackContainerNameList[i]);
            return StatusCode::SUCCESS;
          }   

          for( const auto& link : tmp ) { 
            const xAOD::IParticle* ipart = *link;

            if( ipart->type() != xAOD::Type::TrackParticle ) { 
              ATH_MSG_ERROR("#BTAG# Failed to change xAOD::IParticle type to xAOD::Type::TrackParticle "); 
            }   
            const xAOD::TrackParticle* tpart = static_cast< const xAOD::TrackParticle* >( ipart );
            ElementLink<xAOD::TrackParticleContainer> EL; 
            EL.toContainedElement(*tpContainer, tpart);
            associationLinks.push_back(EL);

          }   
          tagInfo->auxdata<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(m_TrackToJetAssocNameList[i]) = associationLinks;
        } 

        i++;
      }

      // Muons association
      i = 0;
      for (const std::string& muonColName : m_MuonsToAssociateList) {

        for (const xAOD::Jet* jet : *theJets) {
          xAOD::BTagging* tagInfo = const_cast<xAOD::BTagging*>(jet->btagging());
          if (!tagInfo) {
            ATH_MSG_FATAL("The pointer from Jet to BTagging object is invalid");
            return StatusCode::FAILURE;
          }   

          std::vector< ElementLink< xAOD::IParticleContainer > > tmp;
          if (!jet->getAttribute(muonColName, tmp)) {
            ATH_MSG_FATAL("Unable to read track collection " + muonColName + " from jets for b-tagging.");
            return StatusCode::FAILURE;
          }   

          std::vector< ElementLink< xAOD::MuonContainer > > associationLinks;

          const xAOD::MuonContainer * tpContainer( 0 );
          sc = evtStore()->retrieve( tpContainer, m_MuonContainerNameList[i]);
          if ( sc.isFailure() || tpContainer==0) {
            ATH_MSG_ERROR("#BTAG# Failed to retrieve Muons through name: " << m_MuonContainerNameList[i]);
            return StatusCode::SUCCESS;
          }   

          for( const auto& link : tmp ) { 
            const xAOD::IParticle* ipart = *link;
            if( ipart->type() != xAOD::Type::Muon ) { 
              ATH_MSG_ERROR("#BTAG# Failed to change xAOD::IParticle type to xAOD::Type::Muon "); 
            }   
            const xAOD::Muon* tpart = static_cast< const xAOD::Muon* >( ipart );
            ElementLink<xAOD::MuonContainer> EL; 
            EL.toContainedElement(*tpContainer, tpart);
            associationLinks.push_back(EL);
          }   

          tagInfo->auxdata<std::vector<ElementLink<xAOD::MuonContainer> > >(m_MuonToJetAssocNameList[i]) = associationLinks;

        } 

        i++;
      }


      // TODO
      // should break based on getting aux items
      return sc;
    }


      // ----- associate tracks
      const xAOD::TrackParticleContainer * tpContainer( 0 );
      ToolHandleArray< Analysis::ParticleToJetAssociator >::const_iterator tAssocIter = m_TrackToJetAssociatorList.begin();
      ToolHandleArray< Analysis::ParticleToJetAssociator >::const_iterator tAssocEnd  = m_TrackToJetAssociatorList.end();
      std::vector< std::string >::const_iterator tNameIter = m_TrackContainerNameList.begin();
      std::vector< std::string >::const_iterator tAssocNameIter = m_TrackToJetAssocNameList.begin();

      StatusCode sc;
      for (; tAssocIter!=tAssocEnd; ++tAssocIter) {
          if (tracks) {
              tpContainer = tracks;
              sc.setChecked();
          } else {
              sc = evtStore()->retrieve( tpContainer, *tNameIter );
              if ( sc.isFailure() || tpContainer==0) {
                  ATH_MSG_ERROR("#BTAG# Failed to retrieve TrackParticles " << *tNameIter);
                  return StatusCode::SUCCESS;
              }
          }
          ATH_MSG_VERBOSE("#BTAG# Number of TrackParticles in event: " << tpContainer->size());

          // compute the associations
          // NB we are doing away with the Analysis::TrackAssociation class for this.
          // std::vector<Analysis::TrackAssociation> assocs = 
          //   (*tAssocIter)->associateParticlesToJets<Analysis::TrackAssociation, xAOD::TrackParticleContainer>( theJets, tpContainer, *tAssocNameIter );
          std::vector<std::vector<const xAOD::TrackParticle*>*> assocs = 
              (*tAssocIter)->associateParticlesToJets<std::vector<const xAOD::TrackParticle*>, xAOD::TrackParticleContainer>( theJets, tpContainer, *tAssocNameIter );

          // then store them in the BTagging objects. Note that for this to work, the ElementLink from Jet to BTagging object must exist
          unsigned int i = 0;
          for (jetcollection_t::iterator jetIter = theJets->begin(); jetIter != theJets->end(); ++jetIter) {
              // We need a const_cast here, as the Jet does not have a method providing a pointer to a non-const BTagging object
              // A *much* cleaner solution would be to ask the ElementLink (btaggingLink())
              // for the container name and index of the BTagging object, retrieve
              // the container from StoreGate using a non-const pointer, and get the
              // non-const pointer to the BTagging object like that. That would keep
              // const-correctness intact, as StoreGate could tell us whether it's allowed
              // to modify the BTagging object or not.
              xAOD::BTagging* tagInfo = const_cast<xAOD::BTagging*>((*jetIter)->btagging());
              if (!tagInfo) {
                  ATH_MSG_FATAL("The pointer from Jet to BTagging object is invalid");
                  return StatusCode::FAILURE;
              }
              std::vector< ElementLink< xAOD::TrackParticleContainer > > associationLinks;
              for (std::vector<const xAOD::TrackParticle*>::const_iterator trkIter = assocs[i]->begin(); trkIter != assocs[i]->end(); ++trkIter) {
                  ElementLink<xAOD::TrackParticleContainer> EL;
                  EL.toContainedElement(*tpContainer, *trkIter);
                  associationLinks.push_back(EL);
              }
              tagInfo->auxdata<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(*tAssocNameIter) = associationLinks;
              ++i;
          }
          ATH_MSG_VERBOSE("#BTAG# stored track-to jet associations under name " << *tAssocNameIter);
          //delete pointer created in associateParticlesToJets
          for (i=0; i < assocs.size(); i++) {
              delete assocs[i];
          }
          ++tNameIter;
          ++tAssocNameIter;
      }


      // // ----- associate muons
      const xAOD::MuonContainer * muonContainer( 0 );
      ToolHandleArray< Analysis::ParticleToJetAssociator >::const_iterator muAssocIter = m_MuonToJetAssociatorList.begin();
      ToolHandleArray< Analysis::ParticleToJetAssociator >::const_iterator muAssocEnd  = m_MuonToJetAssociatorList.end();
      std::vector< std::string >::const_iterator muNameIter = m_MuonContainerNameList.begin();
      std::vector< std::string >::const_iterator muAssocNameIter = m_MuonToJetAssocNameList.begin();

      for (; muAssocIter!=muAssocEnd; ++muAssocIter) {
          sc = evtStore()->retrieve( muonContainer, *muNameIter );
          if ( sc.isFailure() || muonContainer==0) {
              ATH_MSG_ERROR("#BTAG# Failed to retrieve Muons " << *muNameIter);
              return StatusCode::SUCCESS;
          }
          ATH_MSG_DEBUG("#BTAG# Number of Muons in event: " << muonContainer->size());

          std::vector<std::vector<const xAOD::Muon*>*> assocs =
              (*muAssocIter)->associateParticlesToJets<std::vector<const xAOD::Muon*>, xAOD::MuonContainer>( theJets, muonContainer, *muAssocNameIter );

          // then store them in the BTagging objects. Note that for this to work, the ElementLink from Jet to BTagging object must exist 
          unsigned int i = 0;
          for (jetcollection_t::iterator jetIter = theJets->begin(); jetIter != theJets->end(); ++jetIter) {
              xAOD::BTagging* tagInfo = const_cast<xAOD::BTagging*>((*jetIter)->btagging());
              std::vector< ElementLink< xAOD::MuonContainer > > associationLinks;
              for (std::vector<const xAOD::Muon*>::const_iterator muonIter = assocs[i]->begin(); muonIter != assocs[i]->end(); ++muonIter) {
                  ElementLink<xAOD::MuonContainer> EL;
                  EL.toContainedElement(*muonContainer, *muonIter);
                  associationLinks.push_back(EL);
              }
              tagInfo->auxdata<std::vector<ElementLink<xAOD::MuonContainer> > >(*muAssocNameIter) = associationLinks;
              ++i;
          }
          ATH_MSG_VERBOSE("#BTAG# stored muon-to-jet associations under name " << *muAssocNameIter);
          //delete pointer created in associateParticlesToJets
          for (i=0; i < assocs.size(); i++) {
              delete assocs[i];
          }
          ++muNameIter;
          ++muAssocNameIter;
      }          

      return sc;
      //return StatusCode::SUCCESS;
  }

} // namespace
