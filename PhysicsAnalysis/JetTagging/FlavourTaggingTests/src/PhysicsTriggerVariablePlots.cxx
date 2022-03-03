//Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 

#include "FlavourTaggingTests/PhysicsTriggerVariablePlots.h"

#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingUtilities.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"

#include "CxxUtils/phihelper.h"

namespace FTAGValidation {

  PhysicsTriggerVariablePlots::PhysicsTriggerVariablePlots( const std::string& name, 
					                   ISvcLocator* pSvcLocator ) :
    PhysicsVariablePlots( name, pSvcLocator )
    ,m_bJetChains{}
    {
      declareProperty("bJetChains", m_bJetChains);
    }

  PhysicsTriggerVariablePlots::~PhysicsTriggerVariablePlots() {}
 
  /* ===================================================================== */

  StatusCode PhysicsTriggerVariablePlots::initialize() {
    ATH_MSG_INFO( "Inizializing " << name() << " ..." );
    ATH_CHECK( FTAGValidationAlgorithm::initialize() );

    ATH_MSG_INFO( "Properties declaration:" );
    ATH_MSG_INFO( "   -- " << m_trigVertexKey   );
    ATH_MSG_INFO( "   -- " << m_trigTrackKey    );
    ATH_MSG_INFO( "   -- " << m_offJetKey   );
    ATH_MSG_INFO( "   -- " << m_trigDec     );
    ATH_MSG_INFO( "   -- " << m_bJetChains  );

    ATH_MSG_INFO( "   -- " << m_minJetEta );
    ATH_MSG_INFO( "   -- " << m_maxJetEta );
    ATH_MSG_INFO( "   -- " << m_minJetPt  );

    ATH_CHECK( m_trigVertexKey.initialize() );
    ATH_CHECK( m_trigTrackKey.initialize() );
    ATH_CHECK( m_offJetKey.initialize() );

    ATH_CHECK( initializeHistograms() );

    m_nTotal_events = 0;
    m_nTotal_noPV = 0;
    m_nTotal_noMatchedOffJet = 0;
    m_nTotal_otherFlavour = 0;
    m_nTotal_qualityOnlineJets = 0;

    return StatusCode::SUCCESS;
  }

  StatusCode PhysicsTriggerVariablePlots::execute() {
    ATH_MSG_DEBUG( "Executing " << name() << " ..." );
    m_nTotal_events++;

    const EventContext context = getContext();

    /* =========================================================================================================================================== */
    /* ==== Retrieve Collections */
    /* =========================================================================================================================================== */

    // Event Info
    SG::ReadHandle< xAOD::EventInfo > eventInfoHandle = SG::makeHandle( m_eventInfoKey,context );
    ATH_CHECK( eventInfoHandle.isValid() );
    const xAOD::EventInfo *eventInfo = eventInfoHandle.get();

    // Retrieve Offline-Jet Collection
    const xAOD::JetContainer *offlineJetCollection = nullptr;
    ATH_CHECK( retrieveCollectionfromStoreGate( context,offlineJetCollection,m_offJetKey ) );
    
    std::vector< const xAOD::Jet* > qualityOfflineJetCollection;
    for ( const xAOD::Jet *jet : *offlineJetCollection ) {
      //For the moment running without offline jet quality requirments
      //if ( not passJetQualitySelection( jet ) ) continue;
      //if ( not passJetKinematicSelection( jet ) ) continue;
      //if ( not passJetJVTSelection( jet )  ) continue;
      qualityOfflineJetCollection.push_back( jet );
    }

    //run over b-jet chains
    for( const std::string& chain: m_bJetChains ){
      if( m_trigDec->isPassed(chain) ){
	ATH_MSG_DEBUG("Chain passed " << chain);
      }
      else{
	ATH_MSG_DEBUG("Chain did not pass " << chain);
        continue;
      }

      // ==== Primary Vertex =================
      // Chain is passed, only one PV per event, retrieve it in a standard way, filled once, doesnt matter how many jets
      const xAOD::VertexContainer *vertexCollection = nullptr;
      ATH_CHECK( retrieveCollectionfromStoreGate( context,vertexCollection,m_trigVertexKey ) );
      const xAOD::Vertex *primaryVertex = getPrimaryVertex( vertexCollection );
      if ( primaryVertex == nullptr ) {
        ATH_MSG_WARNING( "Could not retrieve Primary Vertex. Skip this event!" );
        m_nTotal_noPV++;
        return StatusCode::SUCCESS;
      }
      ATH_CHECK( fillHistogram( chain + "_nPrimVtx", vertexCollection->size() ) );
      ATH_CHECK( fillHistogram( chain + "_nTracksPrimVtx",  primaryVertex->nTrackParticles() ) );
      ATH_CHECK( fillHistogram( chain + "_PV_x", primaryVertex->x() ) );
      ATH_CHECK( fillHistogram( chain + "_PV_y", primaryVertex->y() ) );
      ATH_CHECK( fillHistogram( chain + "_PV_z", primaryVertex->z() ) );

      // ==== Tracks =========================
      const xAOD::TrackParticleContainer *trackParticleCollection = nullptr;
      ATH_CHECK( retrieveCollectionfromStoreGate( context,trackParticleCollection,m_trigTrackKey) );
      ATH_CHECK( fillHistogram( chain + "_nTracks",trackParticleCollection->size() ) );

      // ==== Jet ============================
      int nJetsWithMuon = 0;
      int nJetsWithSV   = 0;

      std::vector< TrigCompositeUtils::LinkInfo<xAOD::JetContainer> > onlineJets = m_trigDec->features<xAOD::JetContainer>(chain, TrigDefs::Physics);
      ATH_MSG_DEBUG("onlineJets.size(): " << onlineJets.size() );
      int n_qualityJets = 0;
      for( const auto& jetLinkInfo : onlineJets ) {
	const xAOD::Jet* jet = *(jetLinkInfo.link);
        //Check quality requirement
        if ( not passTriggerJetKinematicSelection( jet ) ) continue;
	n_qualityJets++;
        m_nTotal_qualityOnlineJets++;

        ATH_MSG_DEBUG( "JET KINEMATICS:" );
        ATH_MSG_DEBUG( "  ** pt=" << jet->p4().Et() / Gaudi::Units::GeV << " eta=" << jet->eta() << " phi=" << jet->phi() );
        ATH_CHECK( fillHistogram( chain + "_jet_E"   , jet->e()  / Gaudi::Units::GeV ) );
        ATH_CHECK( fillHistogram( chain + "_jet_pT"  , jet->pt() / Gaudi::Units::GeV ) );
        ATH_CHECK( fillHistogram( chain + "_jet_eta" , jet->eta() ) );
        ATH_CHECK( fillHistogram( chain + "_jet_phi" , jet->phi() ) );

        //nMuons and nSV1
        const xAOD::BTagging* bTag = xAOD::BTaggingUtilities::getBTagging( *jet );
        std::vector< ElementLink< xAOD::VertexContainer > > SV1_vertex = bTag->auxdata<std::vector< ElementLink< xAOD::VertexContainer > > >("SV1_vertices");
        if(SV1_vertex.size() >= 1) nJetsWithSV++;

        if (bTag->isAvailable< ElementLink<xAOD::MuonContainer> >("SMT_mu_link")) {
          ElementLink<xAOD::MuonContainer> muonLink = bTag->auxdata< ElementLink<xAOD::MuonContainer> >("SMT_mu_link");
          if ( muonLink.isValid() ) {
            const xAOD::Muon* muon=(*muonLink);
            if ( muon != 0 ) {
              nJetsWithMuon++;
              ATH_CHECK( fillHistogram( chain + "_muon_pT_frac" , muon->pt() / jet->pt() ) );
            }
          }
        }
        else {
          ATH_MSG_DEBUG( "SMT_mu_link is not available." );
        }

        // Retrieve jet flavour
        std::string flavour = "DATA_";
        if( eventInfo->eventType(xAOD::EventInfo::EventType::IS_SIMULATION) ) {
          int offlineJetIndex = getMatchedOfflineJetIndex( jet, qualityOfflineJetCollection );
          if( offlineJetIndex == -1 ) {
            ATH_MSG_WARNING("Could not match online jet to an offline jet. Skipping Event.");
            m_nTotal_noMatchedOffJet++;
            continue;
          }
          const xAOD::Jet *matchedOfflineJet = qualityOfflineJetCollection.at(offlineJetIndex);

          if( matchedOfflineJet->isAvailable< int >( "HadronConeExclTruthLabelID" ) == false ) {
            ATH_MSG_ERROR( "Input sample is MC but matched offline jet has no 'HadronConeExclTruthLabelID' aux data. Something is wrong!" );
            return StatusCode::FAILURE;
          }
          int HadronConeExclTruthLabelID = matchedOfflineJet->auxdata< int >( "HadronConeExclTruthLabelID");
          ATH_MSG_DEBUG( "  ** 'HadronConeExclTruthLabelID' is " << HadronConeExclTruthLabelID );
          ATH_CHECK( fillHistogram( chain + "_truth_label" , HadronConeExclTruthLabelID ) );
          if( HadronConeExclTruthLabelID == 0 ) flavour = "U_";
          else if( HadronConeExclTruthLabelID == 4 ) flavour = "C_";
          else if( HadronConeExclTruthLabelID == 5 ) flavour = "B_";
          else { m_nTotal_otherFlavour++; continue; }
        }

        ATH_CHECK( analyseTrackParticleObjects( jet,primaryVertex,flavour,chain ) );
        ATH_CHECK( analyseBTaggingObject( jet,flavour,chain ) );
      }
      ATH_CHECK( fillHistogram( chain + "_nJets",n_qualityJets ) );
      ATH_CHECK( fillHistogram( chain + "_nJetsWithMuon" , nJetsWithMuon ) );
      ATH_CHECK( fillHistogram( chain + "_fracJetsWithMuon" , double(nJetsWithMuon)/double(n_qualityJets) ) );
      ATH_CHECK( fillHistogram( chain + "_nJetsWithSV"   , nJetsWithSV ) );
      ATH_CHECK( fillHistogram( chain + "_fracJetsWithSV" , double(nJetsWithSV)/double(n_qualityJets) ) );
    }

    return StatusCode::SUCCESS;
  }

  StatusCode PhysicsTriggerVariablePlots::finalize() {
    ATH_MSG_INFO( "Finalizing " << name() << " ..." );
    ATH_MSG_INFO( "Fraction of events skipped because no primary vertex was found: " << float(m_nTotal_noPV)/float(m_nTotal_events));
    ATH_MSG_INFO( "Fraction of online jets skipped because no offline jet could be matched (average over all chains): " << float(m_nTotal_noMatchedOffJet)/float(m_nTotal_qualityOnlineJets));
    ATH_MSG_INFO( "Fraction of online jets skipped because flavour was different from u,c,b (average over all chains): " << float(m_nTotal_otherFlavour)/float(m_nTotal_qualityOnlineJets-m_nTotal_noMatchedOffJet));
    ATH_MSG_INFO("m_trigDec->getListOfTriggers() " << m_trigDec->getListOfTriggers());
    return StatusCode::SUCCESS;
  }

}


