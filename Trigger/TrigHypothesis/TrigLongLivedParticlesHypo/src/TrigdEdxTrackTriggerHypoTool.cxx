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

   const std::string header = "UTT: ";

   ATH_MSG_INFO( header << "Initialization completed successfully:" );
   ATH_MSG_INFO( header << "cutTrackPt            = " << m_cutTrackPt ); 
   ATH_MSG_INFO( header << "cutTrackEta           = " << m_cutTrackEta );  
   ATH_MSG_INFO( header << "cutTrackdEdx          = " << m_cutTrackdEdx );  
   ATH_MSG_INFO( header << "cutTrackNhighdEdxHits = " << m_cutTrackNhighdEdxHits ); 
   ATH_MSG_INFO( header << "cutTrackHighdEdxDef   = " << m_cutTrackHighdEdxDef ); 
   ATH_MSG_INFO( header << "Tool configured for chain/id: " << m_decisionId  );
   
   return StatusCode::SUCCESS;
}

TrigdEdxTrackTriggerHypoTool::~TrigdEdxTrackTriggerHypoTool() {}

StatusCode TrigdEdxTrackTriggerHypoTool::decide( TrackInfo& input ) const
{
   auto monTrackPt     = Monitored::Scalar( "trackPt",     -999. );
   auto monTrackEta    = Monitored::Scalar( "trackEta",    -999. );
   auto monTracka0beam = Monitored::Scalar( "tracka0beam", -999. );
   auto monTrackdEdx   = Monitored::Scalar( "trackdEdx",   -999. );
   auto monTrackNhighdEdxHits = Monitored::Scalar( "trackNhighdEdxHits",  -999. );
   auto monitorIt = Monitored::Group( m_monTool, monTrackdEdx, monTrackPt, monTrackEta, monTracka0beam, monTrackNhighdEdxHits );
   
   const xAOD::TrigCompositeContainer* trksContainer = input.trksContainer;
   const xAOD::TrigCompositeContainer* hitsContainer = input.hitsContainer;
   xAOD::TrigCompositeContainer&       dedxContainer = input.dedxContainer;

   const std::string header = "UTT: ";

   const int cutIndex = 0;

   unsigned int i_trk=0;
   unsigned int n_passed_trk=0;
   for ( const xAOD::TrigComposite* trk : *trksContainer ) {
      ATH_MSG_DEBUG( header << "+++++ i_trk: " << i_trk++ << " +++++");

      // pT cut
      float trackPt = trk->getDetail<float>("trk_pt");
      monTrackPt = trackPt;
      if( trackPt < m_cutTrackPt[cutIndex] ) {
	 ATH_MSG_DEBUG( header << "Fails pt cut: " << trackPt << " < " << m_cutTrackPt[cutIndex] );
	 continue;
      }

      // eta cut
      float trackEta = trk->getDetail<float>("trk_eta");
      monTrackEta = trackEta;
      if( std::abs(trackEta) > m_cutTrackEta[cutIndex] ) {
	 ATH_MSG_DEBUG( header << "Fails eta cut: " << trackEta << " < " << m_cutTrackEta[cutIndex] );
	 continue;
      }

      // a0beam cut
      float tracka0beam = trk->getDetail<float>("trk_a0beam");
      monTracka0beam = tracka0beam;
      if( std::abs(tracka0beam) > m_cutTracka0beam[cutIndex] ) {
	 ATH_MSG_DEBUG( header << "Fails a0beam cut: " << tracka0beam << " < " << m_cutTracka0beam[cutIndex] );
	 continue;
      }
   
      // dEdx cut
      float trackdEdx = trk->getDetail<float>("trk_dedx");
      monTrackdEdx = trackdEdx;
      if( trackdEdx < m_cutTrackdEdx[cutIndex] ) {
	 ATH_MSG_DEBUG( header << "Fails dEdx cut: " << trackdEdx << " < " << m_cutTrackdEdx[cutIndex] );
	 continue;
      }

      // nr of high dEdx hits cut
      int trackId = trk->getDetail<int>("trk_id");
      int n_hdedx_hits = 0;
      for ( const xAOD::TrigComposite* hit : *hitsContainer ) {
	 int id = hit->getDetail<int>("hit_trkid");
	 if( id != trackId ) continue;
	 float dedx  = hit->getDetail<float>("hit_dedx");
	 ATH_MSG_DEBUG( header << "... dEdx = " << dedx);
	 if( dedx < m_cutTrackHighdEdxDef[cutIndex] ) continue;
	 n_hdedx_hits++;
      }
      monTrackNhighdEdxHits = n_hdedx_hits;
      if( n_hdedx_hits < m_cutTrackNhighdEdxHits[cutIndex] ) {
	 ATH_MSG_DEBUG( header << "Fails nr of high dEdx hits cut" << n_hdedx_hits << " < " << m_cutTrackNhighdEdxHits[cutIndex] );
	 continue;
      }

      // passed selection
      ATH_MSG_DEBUG( header << "Passed selection" );
      n_passed_trk++;

      // create EDM object
      xAOD::TrigComposite *dedx = new xAOD::TrigComposite();
      dedx->makePrivateStore();
      dedx->setDetail("trk_pt",     trackPt);
      dedx->setDetail("trk_eta",    trackEta);
      dedx->setDetail("trk_a0beam", tracka0beam);
      dedx->setDetail("trk_dedx",   trackdEdx);
      dedx->setDetail("trk_n_hdedx_hits", n_hdedx_hits);
      int n_hits_innermost = trk->getDetail<int>("trk_n_hits_innermost");
      int n_hits_inner     = trk->getDetail<int>("trk_n_hits_inner");
      int n_hits_pix       = trk->getDetail<int>("trk_n_hits_pix");
      int n_hits_sct       = trk->getDetail<int>("trk_n_hits_sct");
      dedx->setDetail("trk_n_hits_innermost", n_hits_innermost);
      dedx->setDetail("trk_n_hits_inner", n_hits_inner);
      dedx->setDetail("trk_n_hits_pix", n_hits_pix);
      dedx->setDetail("trk_n_hits_sct", n_hits_sct);
      ATH_MSG_DEBUG( header << "Created a new entry EDM" );
      dedxContainer.push_back(dedx);
   }

   // 
   if( n_passed_trk > 0 ) {
      addDecisionID( m_decisionId.numeric(), input.decision );
      ATH_MSG_DEBUG( header << "REGTEST event accepted" );
      return StatusCode::SUCCESS;
   }
   else {
      return StatusCode::SUCCESS;
   }
}
