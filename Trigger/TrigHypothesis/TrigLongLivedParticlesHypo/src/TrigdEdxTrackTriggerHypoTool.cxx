/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

  * Trigger Hypo Tool, that is aimed at triggering large dEdx tracks
  * author Kunihiro Nagano <kunihiro.nagano@cern.ch> - KEK
*/

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "TrigdEdxTrackTriggerHypoTool.h"
#include "GaudiKernel/PhysicalConstants.h"

using namespace TrigCompositeUtils;

TrigdEdxTrackTriggerHypoTool::TrigdEdxTrackTriggerHypoTool( const std::string& type, 
							    const std::string& name, 
							    const IInterface* parent ) 
   : AthAlgTool( type, name, parent ),
     m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode TrigdEdxTrackTriggerHypoTool::initialize()
{
   if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );

   ATH_MSG_INFO("Initialization completed successfully:");
   ATH_MSG_INFO("cutTrackPtGeV         = " << m_cutTrackPtGeV); 
   ATH_MSG_INFO("cutTrackEta           = " << m_cutTrackEta);  
   ATH_MSG_INFO("cutTrackdEdx          = " << m_cutTrackdEdx);  
   ATH_MSG_INFO("cutTrackNhighdEdxHits = " << m_cutTrackNhighdEdxHits); 
   ATH_MSG_INFO("cutTrackHighdEdxDef   = " << m_cutTrackHighdEdxDef); 
   ATH_MSG_INFO("Tool configured for chain/id: " << m_decisionId);
   
   return StatusCode::SUCCESS;
}

TrigdEdxTrackTriggerHypoTool::~TrigdEdxTrackTriggerHypoTool() {}

StatusCode TrigdEdxTrackTriggerHypoTool::decide( TrackInfo& input ) const
{
   auto monTrackPtGeV  = Monitored::Scalar( "trackPtGeV",  -999. );
   auto monTrackEta    = Monitored::Scalar( "trackEta",    -999. );
   auto monTracka0beam = Monitored::Scalar( "tracka0beam", -999. );
   auto monTrackdEdx   = Monitored::Scalar( "trackdEdx",   -999. );
   auto monTrackNhighdEdxHits = Monitored::Scalar( "trackNhighdEdxHits",  -999. );
   auto monitorIt = Monitored::Group( m_monTool, monTrackdEdx, monTrackPtGeV, monTrackEta, monTracka0beam, monTrackNhighdEdxHits );
   
   const xAOD::TrigCompositeContainer* trksContainer = input.trksContainer;
   const xAOD::TrigCompositeContainer* hitsContainer = input.hitsContainer;
   xAOD::TrigCompositeContainer&       dedxContainer = input.dedxContainer;

   const int cutIndex = 0;

   unsigned int i_trk=0;
   unsigned int n_passed_trk=0;
   for ( const xAOD::TrigComposite* trk : *trksContainer ) {
      ATH_MSG_DEBUG("+++++ i_trk: " << i_trk++ << " +++++");

      // values
      float trackPt     = trk->getDetail<float>("trk_pt");
      float trackEta    = trk->getDetail<float>("trk_eta");
      float tracka0beam = trk->getDetail<float>("trk_a0beam");
      float trackdEdx   = trk->getDetail<float>("trk_dedx");
      int   trackId     = trk->getDetail<int>  ("trk_id");
      ATH_MSG_DEBUG("track pt / eta / a0beam / dEdx / Id = " << trackPt << " / " << trackEta << " / " << tracka0beam << " / " << trackdEdx << " / " << trackId );
      
      // pT cut
      float trackPtGeV = trackPt / 1000.0;
      monTrackPtGeV = trackPtGeV;
      if( trackPtGeV < m_cutTrackPtGeV[cutIndex] ) {
	 ATH_MSG_DEBUG("Fails pt cut in GeV: " << trackPtGeV << " < " << m_cutTrackPtGeV[cutIndex] );
	 continue;
      }

      // eta cut
      monTrackEta = trackEta;
      if( std::abs(trackEta) > m_cutTrackEta[cutIndex] ) {
	 ATH_MSG_DEBUG("Fails eta cut: " << trackEta << " < " << m_cutTrackEta[cutIndex] );
	 continue;
      }

      // a0beam cut
      monTracka0beam = tracka0beam;
      if( std::abs(tracka0beam) > m_cutTracka0beam[cutIndex] ) {
	 ATH_MSG_DEBUG("Fails a0beam cut: " << tracka0beam << " < " << m_cutTracka0beam[cutIndex] );
	 continue;
      }
   
      // dEdx cut
      monTrackdEdx = trackdEdx;
      if( trackdEdx < m_cutTrackdEdx[cutIndex] ) {
	 ATH_MSG_DEBUG("Fails dEdx cut: " << trackdEdx << " < " << m_cutTrackdEdx[cutIndex] );
	 continue;
      }

      // nr of high dEdx hits cut
      int n_hdedx_hits = 0;
      ATH_MSG_DEBUG("looping hits for trackId=" << trackId);
      for ( const xAOD::TrigComposite* hit : *hitsContainer ) {
	 int id = hit->getDetail<int>("hit_trkid");
	 if( id != trackId ) continue;
	 float dedx  = hit->getDetail<float>("hit_dedx");
	 ATH_MSG_DEBUG("... dEdx = " << dedx);
	 if( dedx < m_cutTrackHighdEdxDef[cutIndex] ) continue;
	 n_hdedx_hits++;
      }
      monTrackNhighdEdxHits = n_hdedx_hits;
      if( n_hdedx_hits < m_cutTrackNhighdEdxHits[cutIndex] ) {
	 ATH_MSG_DEBUG("Fails nr of high dEdx hits cut" << n_hdedx_hits << " < " << m_cutTrackNhighdEdxHits[cutIndex] );
	 continue;
      }

      // passed selection
      ATH_MSG_DEBUG("Passed selection");
      n_passed_trk++;

      // create EDM object
      xAOD::TrigComposite *dedx = new xAOD::TrigComposite();
      dedx->makePrivateStore();
      dedxContainer.push_back(dedx);
      dedx->setDetail<float>("trk_pt",     trackPt);
      dedx->setDetail<float>("trk_eta",    trackEta);
      float trackPhi = trk->getDetail<float>("trk_phi");
      dedx->setDetail<float>("trk_phi",    trackPhi);
      dedx->setDetail<float>("trk_dedx",   trackdEdx);
      dedx->setDetail<int>  ("trk_n_hdedx_hits", n_hdedx_hits);
      dedx->setDetail<float>("trk_a0beam", tracka0beam);
      float n_hits_innermost = trk->getDetail<float>("trk_n_hits_innermost");
      float n_hits_inner     = trk->getDetail<float>("trk_n_hits_inner");
      float n_hits_pix       = trk->getDetail<float>("trk_n_hits_pix");
      float n_hits_sct       = trk->getDetail<float>("trk_n_hits_sct");
      dedx->setDetail<float>("trk_n_hits_innermost", n_hits_innermost);
      dedx->setDetail<float>("trk_n_hits_inner",     n_hits_inner);
      dedx->setDetail<float>("trk_n_hits_pix",       n_hits_pix);
      dedx->setDetail<float>("trk_n_hits_sct",       n_hits_sct);
      ATH_MSG_DEBUG("Created a new entry EDM");
   }

   // 
   if( n_passed_trk > 0 ) {
      addDecisionID( m_decisionId.numeric(), input.decision );
      ATH_MSG_DEBUG("REGTEST event accepted");
      return StatusCode::SUCCESS;
   }
   else {
      return StatusCode::SUCCESS;
   }
}
