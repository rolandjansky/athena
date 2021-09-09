/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

  * Trigger Hypo Tool, that is aimed at triggering large dEdx tracks
  * author Kunihiro Nagano <kunihiro.nagano@cern.ch> - KEK
*/
#include "TrigdEdxTrackHypoAlg.h"
#include "AthViews/ViewHelper.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "AthenaMonitoringKernel/Monitored.h"

using TrigCompositeUtils::createAndStore; 
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionAuxContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::newDecisionIn;
using TrigCompositeUtils::linkToPrevious;
using TrigCompositeUtils::viewString;
using TrigCompositeUtils::featureString;
using TrigCompositeUtils::hypoAlgNodeName;
using TrigCompositeUtils::findLink;
using TrigCompositeUtils::LinkInfo;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::allFailed;

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

TrigdEdxTrackHypoAlg::TrigdEdxTrackHypoAlg( const std::string& name, 
					    ISvcLocator* pSvcLocator ) : 
   ::HypoBase( name, pSvcLocator ) {}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigdEdxTrackHypoAlg::initialize()
{
   CHECK( m_hypoTools.retrieve() );
   CHECK( m_dEdxTrkKey.initialize() );
   CHECK( m_dEdxHitKey.initialize() );
   CHECK( m_HPtdEdxTrkKey.initialize());

   if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );

   m_highdEdxHitDefThres.clear();         m_highdEdxHitDefNames.clear();
   m_highdEdxHitDefThres.push_back(1.45); m_highdEdxHitDefNames.push_back("1p45");
   m_highdEdxHitDefThres.push_back(1.50); m_highdEdxHitDefNames.push_back("1p50");
   m_highdEdxHitDefThres.push_back(1.55); m_highdEdxHitDefNames.push_back("1p55");
   m_highdEdxHitDefThres.push_back(1.60); m_highdEdxHitDefNames.push_back("1p60");
   m_highdEdxHitDefThres.push_back(1.65); m_highdEdxHitDefNames.push_back("1p65");
   m_highdEdxHitDefThres.push_back(1.70); m_highdEdxHitDefNames.push_back("1p70");
   m_highdEdxHitDefThres.push_back(1.75); m_highdEdxHitDefNames.push_back("1p75");
   m_highdEdxHitDefThres.push_back(1.80); m_highdEdxHitDefNames.push_back("1p80");

   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigdEdxTrackHypoAlg::execute( const EventContext& context ) const
{
   // previous decisions
   ATH_MSG_DEBUG( "Retrieving pervious decision: \"" << decisionInput().key() << "\"" );
   auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
   ATH_CHECK( previousDecisionsHandle.isValid() );
   
   ATH_MSG_DEBUG( "Running with " << previousDecisionsHandle->size() << " previous decisions" ); 
   if( previousDecisionsHandle->size()!=1 ) {
      ATH_MSG_ERROR( "Previous decision handle size is not 1. It is" << previousDecisionsHandle->size() );
      return StatusCode::FAILURE;
   }
   const Decision * previousDecision = previousDecisionsHandle->at(0);

   TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
   TrigCompositeUtils::decisionIDs(previousDecision, previousDecisionIDs);
   ATH_MSG_VERBOSE( "IDs of active legs:" );
   for(auto decisionID: previousDecisionIDs) { ATH_MSG_VERBOSE( "    " << decisionID ); }

   // new output decisions
   ATH_MSG_DEBUG( "Creating new output decision handle" ); 
   SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
   auto outputDecisions = outputHandle.ptr();

   // input objects
   
   // tracks
   auto dEdxTrkHandle = SG::makeHandle(m_dEdxTrkKey, context );
   ATH_CHECK( dEdxTrkHandle.isValid() );
   ATH_MSG_DEBUG( "dEdxTrk handle size: " << dEdxTrkHandle->size() );

   // hits
   auto dEdxHitHandle = SG::makeHandle(m_dEdxHitKey, context );
   ATH_CHECK( dEdxHitHandle.isValid() );
   ATH_MSG_DEBUG( "dEdxHit handle size: " << dEdxHitHandle->size() );

   const xAOD::TrigCompositeContainer * dEdxTrksContainer = dEdxTrkHandle.get();
   const xAOD::TrigCompositeContainer * dEdxHitsContainer = dEdxHitHandle.get();

   if( dEdxTrksContainer == nullptr ) {
      ATH_MSG_ERROR( "ERROR Cannot get dEdxTrks container");
      return StatusCode::FAILURE;
   }
   if( dEdxHitsContainer == nullptr ) {
      ATH_MSG_ERROR( "ERROR Cannot get dEdxHits container");
      return StatusCode::FAILURE;
   }

   // output EDM object
   auto HPtdEdxTrkContainer    = std::make_unique<xAOD::TrigCompositeContainer>();
   auto HPtdEdxTrkContainerAux = std::make_unique<xAOD::TrigCompositeAuxContainer>();
   HPtdEdxTrkContainer->setStore(HPtdEdxTrkContainerAux.get());

   xAOD::TrigCompositeContainer* dedxContainer = HPtdEdxTrkContainer.get();

   // Prepare inputs to HypoTool
   int n_cands = 0;
   ATH_CHECK( createCandidates(dEdxTrksContainer, dEdxHitsContainer, dedxContainer, n_cands) );
   ATH_MSG_DEBUG( "nr of cands by createCandidates = " << n_cands );

   std::vector<TrigdEdxTrackHypoTool::dEdxTrkHypoInfo> dEdxTrkHypoInputs;

   for ( auto dedx : *dedxContainer ) {
      Decision* newDecision = TrigCompositeUtils::newDecisionIn( outputDecisions, previousDecision, TrigCompositeUtils::hypoAlgNodeName(), context);

      ElementLink<xAOD::TrigCompositeContainer> dedxEL = ElementLink<xAOD::TrigCompositeContainer>(*outputHandle, dedx->index());
      ATH_CHECK( dedxEL.isValid() );

      ATH_CHECK( newDecision->setObjectLink<xAOD::TrigCompositeContainer>(TrigCompositeUtils::featureString(), dedxEL) );

      TrigdEdxTrackHypoTool::dEdxTrkHypoInfo hypoInfo{ newDecision, dedx, previousDecisionIDs };
      dEdxTrkHypoInputs.push_back( hypoInfo );
   }

   // monitor
   ATH_CHECK( doMonitor(dedxContainer) );

   // Loop over all hypoToolinputs and get their decisions
   for ( auto & tool: m_hypoTools ) {
      ATH_MSG_VERBOSE( "+++++ Now computing decision for " << tool->name() );
      ATH_CHECK( tool->decide( dEdxTrkHypoInputs ) );    
   } 
   
   DecisionContainer::iterator it = outputDecisions->begin();
   while(it != outputDecisions->end()) {
      ATH_MSG_VERBOSE( "+++++ outputDecision: " << *it << " +++++" );
      if ( allFailed( *it ) ) {
	 ATH_MSG_VERBOSE( "---> all failed, erasing" );
	 it = outputDecisions->erase(it);
      } else {
	 ATH_MSG_VERBOSE( "---> not all failed" );
	 ++it;
      }
   }

   // record 
   SG::WriteHandle<xAOD::TrigCompositeContainer> HPtdEdxTrkHandle(m_HPtdEdxTrkKey, context);
   ATH_CHECK( HPtdEdxTrkHandle.record( std::move( HPtdEdxTrkContainer ), std::move( HPtdEdxTrkContainerAux ) ) );
   ATH_MSG_VERBOSE( "recorded HPtdEdxTrk object to SG" );

   //
   ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

   //
   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigdEdxTrackHypoAlg::doMonitor(const xAOD::TrigCompositeContainer* dedxContainer ) const
{
   auto monTrackPtGeV  = Monitored::Scalar( "trackPtGeV",  -999. );
   auto monTrackEta    = Monitored::Scalar( "trackEta",    -999. );
   auto monTracka0beam = Monitored::Scalar( "tracka0beam", -999. );
   auto monTrackdEdx   = Monitored::Scalar( "trackdEdx",   -999. );
   auto monTrackNhighdEdxHits = Monitored::Scalar( "trackNhighdEdxHits",  -999. );
   auto monitorIt = Monitored::Group( m_monTool, monTrackdEdx, monTrackPtGeV, monTrackEta, monTracka0beam, monTrackNhighdEdxHits );

   for ( const xAOD::TrigComposite* trk : *dedxContainer ) {
      float trackPt     = trk->getDetail<float>("HPtdEdxTrk_pt");
      float trackEta    = trk->getDetail<float>("HPtdEdxTrk_eta");
      float tracka0beam = trk->getDetail<float>("HPtdEdxTrk_a0beam");
      float trackdEdx   = trk->getDetail<float>("HPtdEdxTrk_dedx");
      int trackNhighdEdxHits = (int)trk->getDetail<int16_t>("HPtdEdxTrk_n_hdedx_hits_1p70");
      monTrackPtGeV  = trackPt / 1000.0;
      monTrackEta    = trackEta;
      monTracka0beam = tracka0beam;
      monTrackdEdx   = trackdEdx;
      monTrackNhighdEdxHits = trackNhighdEdxHits;
   }

   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

StatusCode TrigdEdxTrackHypoAlg::createCandidates( const xAOD::TrigCompositeContainer* trksContainer, const xAOD::TrigCompositeContainer* hitsContainer,
						   xAOD::TrigCompositeContainer* dedxContainer, int& n_trks) const
{
   n_trks = 0;

   for ( const xAOD::TrigComposite* trk : *trksContainer ) {
      
      ATH_MSG_VERBOSE("+++++ i_trk: " << n_trks << " +++++");

      float trackPt     = trk->getDetail<float>("dEdxTrk_pt");
      float trackEta    = trk->getDetail<float>("dEdxTrk_eta");
      float trackPhi    = trk->getDetail<float>("dEdxTrk_phi");
      float tracka0beam = trk->getDetail<float>("dEdxTrk_a0beam");
      float trackdEdx   = trk->getDetail<float>("dEdxTrk_dedx");
      int   trackId     = trk->getDetail<int>  ("dEdxTrk_id");
      ATH_MSG_VERBOSE( "track pt / eta / a0beam / dEdx / Id = " << trackPt << " / " << trackEta << " / " << tracka0beam << " / " << trackdEdx << " / " << trackId );

      std::vector<int16_t> n_hdedx_hits;
      for(unsigned int idef=0; idef<m_highdEdxHitDefThres.size(); ++idef) {  n_hdedx_hits.push_back(0); }

      for ( const xAOD::TrigComposite* hit : *hitsContainer ) {
	 int id = hit->getDetail<int>("dEdxHit_trkid");
	 if( id != trackId ) continue;
	 float dedx  = hit->getDetail<float>("dEdxHit_dedx");
	 for(unsigned int idef=0; idef<m_highdEdxHitDefThres.size(); ++idef) {
	    if( dedx >= m_highdEdxHitDefThres[idef] ) n_hdedx_hits[idef]++;
	 }
      }
      for(unsigned int idef=0; idef<m_highdEdxHitDefThres.size(); ++idef) {
	 ATH_MSG_VERBOSE( m_highdEdxHitDefNames[idef] << ", thres=" << m_highdEdxHitDefThres[idef] << ": nr of high dEdx hits=" << n_hdedx_hits[idef] );
      }

      int16_t n_hits_innermost = (int16_t)trk->getDetail<int>("dEdxTrk_n_hits_innermost");
      int16_t n_hits_inner     = (int16_t)trk->getDetail<int>("dEdxTrk_n_hits_inner");
      int16_t n_hits_pix       = (int16_t)trk->getDetail<int>("dEdxTrk_n_hits_pix");
      int16_t n_hits_sct       = (int16_t)trk->getDetail<int>("dEdxTrk_n_hits_sct");

      // create EDM object
      ++n_trks;
      xAOD::TrigComposite *dedx = new xAOD::TrigComposite();
      dedx->makePrivateStore();
      dedxContainer->push_back(dedx);

      dedx->setDetail<float>("HPtdEdxTrk_pt",    trackPt);
      dedx->setDetail<float>("HPtdEdxTrk_eta",   trackEta);
      dedx->setDetail<float>("HPtdEdxTrk_phi",   trackPhi);
      dedx->setDetail<float>("HPtdEdxTrk_a0beam",tracka0beam);
      dedx->setDetail<float>("HPtdEdxTrk_dedx", trackdEdx);
      for(unsigned int idef=0; idef<m_highdEdxHitDefThres.size(); ++idef) {
	 std::string name = "HPtdEdxTrk_n_hdedx_hits_" + m_highdEdxHitDefNames[idef];
	 dedx->setDetail<int16_t>(name, n_hdedx_hits[idef]);
      }
      dedx->setDetail<int16_t>("HPtdEdxTrk_n_hits_innermost", n_hits_innermost);
      dedx->setDetail<int16_t>("HPtdEdxTrk_n_hits_inner",     n_hits_inner);
      dedx->setDetail<int16_t>("HPtdEdxTrk_n_hits_pix",       n_hits_pix);
      dedx->setDetail<int16_t>("HPtdEdxTrk_n_hits_sct",       n_hits_sct);
   }

   //
   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
