/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecInclusive/VrtSecInclusive.h"
#include "VrtSecInclusive/NtupleVars.h"

#include <iostream>


#include "TH1D.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TROOT.h"

//-------------------------------------------------

using namespace std;

namespace VKalVrtAthena {
  

 //____________________________________________________________________________________________________
  StatusCode  VrtSecInclusive::SelGoodTrkParticle( const xAOD::TrackParticleContainer *trackParticleContainer )
  { 
    
    
    ATH_MSG_DEBUG( " > SelGoodTrkParticle: begin"  );

    xAOD::TrackParticleContainer *m_selectedBaseTracks ( nullptr );
    ATH_CHECK( evtStore()->retrieve(m_selectedBaseTracks, "VrtSecInclusive_SelectedTrackParticles") );
    
    //std::vector<const xAOD::TrackParticle*>::const_iterator   i_ntrk;
    AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
    vector<double> Impact,ImpactError;
    
    if( m_FillHist ) m_trkSelCuts->Fill(1.); 
    
    //ATH_MSG_VERBOSE("here:0 ");
    
    for( auto *trk : *trackParticleContainer ) {
      
      trk->auxdecor<bool>("is_selected") = false;
      
      //
      //-- Perigee in TrackParticle
      //
      if( m_FillHist ) m_trkSelCuts->Fill(2.); 
      
      if( m_FillNtuple ) m_ntupleVars->get< vector<int> >("RecoTrk_id").emplace_back( trk->index() );
      
      const Trk::Perigee* perigee = GetPerigee( trk );
      if( !perigee ) continue; 
      
      if( m_FillHist ) m_trkSelCuts->Fill(3.); 
      
      VectPerig = perigee->parameters(); 
      double CovTrkMtx11 = trk->definingParametersCovMatrix()(0,0);
      double CovTrkMtx22 = trk->definingParametersCovMatrix()(1,1);
      
      //
      //====================================
      //long int PixelHits     = 3;
      //long int SctHits       = 9; 
      //long int SharedHits    = 0;
      //long int BLayHits      = 1;
      //----------------------------------- Summary tools (OUTDATED)
      //ATH_MSG_DEBUG("here:0a ");
      //const Trk::TrackSummary* testSum = trk->trackSummary();
      //ATH_MSG_DEBUG("here:0b ");
      //=========================================================================
      uint8_t PixelHits     = 3;
      uint8_t SctHits       = 9; 
      uint8_t BLayHits      = 1;
      uint8_t PixShare,SctShare,TRTHits;
      if( !(trk->summaryValue(PixelHits,xAOD::numberOfPixelHits)) )               PixelHits=0;
      if( !(trk->summaryValue(  SctHits,xAOD::numberOfSCTHits))   )               SctHits=0;
      if( !(trk->summaryValue( BLayHits,xAOD::numberOfInnermostPixelLayerHits)))  BLayHits=0;
      if( !(trk->summaryValue( PixShare,xAOD::numberOfPixelSharedHits)) )         PixShare=0;
      if( !(trk->summaryValue( SctShare,xAOD::numberOfSCTSharedHits)) )           SctShare=0;
      if( !(trk->summaryValue(  TRTHits,xAOD::numberOfTRTHits)) )                 TRTHits=0;
      long int SharedHits = PixShare + SctShare;
      
      m_fitSvc->VKalGetImpact(trk, m_thePV->position(), 1, Impact, ImpactError);  // ImpactSignif not needed
      
      if( m_FillHist ) m_trkSelCuts->Fill(4.); 
      
      
      double ImpactA0 = VectPerig[0];                         // relative to (0,0,0)
      double ImpactZ = VectPerig[1]- m_thePV->z();   // (0,0,0)

      if(m_ImpactWrtBL)
      {
        ImpactA0=trk->d0(); // these are relative to beamline
        ImpactZ=trk->z0();
      }
      else
      {
        ImpactA0=Impact[0];  // these are relative to PV
        ImpactZ=Impact[1];
      }

      double theta = perigee->parameters()[Trk::theta];
      double qOverP = perigee->parameters()[Trk::qOverP];
      double ptrk = (1./qOverP);
      double pT = ptrk*sin(theta);
      double trketa = -1.*log( tan(theta/2));
      double chi2 = trk->chiSquared() / trk->numberDoF();
      
      
      if( m_FillNtuple ) {
        m_ntupleVars->get< vector<double> >( "RecoTrk_pT"      ) .emplace_back(pT);
        m_ntupleVars->get< vector<double> >( "RecoTrk_eta"     ) .emplace_back(trketa);
        m_ntupleVars->get< vector<double> >( "RecoTrk_phi"     ) .emplace_back(perigee->parameters()[Trk::phi]);
        m_ntupleVars->get< vector<double> >( "RecoTrk_chi2"    ) .emplace_back(chi2);
        //
        m_ntupleVars->get< vector<double> >( "RecoTrk_2dIP"    ) .emplace_back( perigee->parameters()[Trk::d0]); 
        m_ntupleVars->get< vector<double> >( "RecoTrk_ZIP"     ) .emplace_back( perigee->parameters()[Trk::z0]); 
        m_ntupleVars->get< vector<double> >( "RecoTrk_2dIPErr" ) .emplace_back( CovTrkMtx11);
        m_ntupleVars->get< vector<double> >( "RecoTrk_ZIPErr"  ) .emplace_back( CovTrkMtx22);
        // also these two
        m_ntupleVars->get< vector<double> >( "RecoTrk_2dIPPV"  ) .emplace_back( ImpactA0 ); // relative to PV
        m_ntupleVars->get< vector<double> >( "RecoTrk_ZIPPV"   ) .emplace_back( ImpactZ ); 
        //
        m_ntupleVars->get< vector<int> >( "RecoTrk_BLayHits"   ) .emplace_back( BLayHits );
        m_ntupleVars->get< vector<int> >( "RecoTrk_PixHits"    ) .emplace_back( PixelHits );
        m_ntupleVars->get< vector<int> >( "RecoTrk_SCTHits"    ) .emplace_back( SctHits );
        m_ntupleVars->get< vector<int> >( "RecoTrk_TRTHits"    ) .emplace_back( TRTHits );
        m_ntupleVars->get< vector<int> >( "RecoTrk_PixShare"   ) .emplace_back( PixShare );
        m_ntupleVars->get< vector<int> >( "RecoTrk_SCTShare"   ) .emplace_back( SctShare );
      }
      
      
      double matchProb=-1;
      int barcode=0;
      if(m_doTruth)
	{  
	  const xAOD::TruthParticle* aTemp_truth = getTrkGenParticle(trk);
	  if(aTemp_truth)
	    {
	      barcode = aTemp_truth->barcode();
	      matchProb= trk->auxdata< float >( "truthMatchProbability" );;
	    }
	}
      if( m_FillNtuple ) {
        m_ntupleVars->get< vector<int> >( "RecoTrk_barcode" ).emplace_back(barcode);
        m_ntupleVars->get< vector<double> >( "RecoTrk_matchPr" ).emplace_back(matchProb);
      }      

      #if 0
      // 2016.5.25 Hide Oide - de-commission this part
      
      // extrapolate track to given surfaces
      if( m_doIntersectionPos )
        getIntersection(trk, m_ntupleVars->get< vector<InterSectionPos> >( "RecoTrk_Int" ), perigee); // get intersection and store values in ntuple
      #endif


      int trkauth=0;
      //
      if( m_FillNtuple ) m_ntupleVars->get< vector<int> >( "RecoTrk_TrkAuth" ).emplace_back(trkauth);
      int lowpttrk = 0;
      if( m_FillNtuple ) m_ntupleVars->get< vector<int> >( "RecoTrk_TrkLowPt" ).emplace_back(lowpttrk);


      // d0 significance cut
      if ( CovTrkMtx11 > m_A0TrkErrorCut*m_A0TrkErrorCut )  continue;
      if( m_FillHist ) m_trkSelCuts->Fill(5.); 
      
      // z0 significane cut
      if ( CovTrkMtx22 > m_ZTrkErrorCut*m_ZTrkErrorCut )    continue;
      if( m_FillHist ) m_trkSelCuts->Fill(6.); 
      
      // another d0/z0 significant cut
      if(!m_ImpactWrtBL)
      {
        if ( fabs(ImpactA0)/sqrt(ImpactError[0]) < m_a0TrkPVSignifCut ) continue;
        if ( fabs(ImpactZ) /sqrt(ImpactError[2]) < m_zTrkPVSignifCut  ) continue;
      }
      if( m_FillHist ) m_trkSelCuts->Fill(7.); 
      
      
      StatusCode sc = CutTrk( VectPerig[4] , VectPerig[3], ImpactA0 , ImpactZ ,
			      chi2, PixelHits, SctHits, SharedHits, BLayHits, TRTHits);
      
      if( m_FillHist ) m_trkSelCuts->Fill(8.); 
					  
      if( sc.isFailure() )                 continue;
      if( m_FillHist ) m_trkSelCuts->Fill(9.); 
      
      
      //------------------------------------------------------------
      // Store the selected track to the new container
      // Here we firstly need to register the empty pointer to the container,
      // then need to do deep copy after then. This is the feature of xAOD.
      xAOD::TrackParticle *a_trk = new xAOD::TrackParticle;
      m_selectedBaseTracks->emplace_back( a_trk );
      *a_trk = *trk;
      a_trk->auxdata<unsigned long>("trk_id")  = trk->index();
      a_trk->auxdata<int>("truth_barcode")     = static_cast<long int>( barcode );
      
      
      // change the selection flag of the copy of the original one (cross-linking)
      trk->auxdecor<bool>("is_selected") = true;
      trk->auxdecor<unsigned long>("selected_trk_id")  = a_trk->index();
      
      ElementLink<xAOD::TrackParticleContainer>  trackElementLink_orig( *trackParticleContainer, trk->index() );
      a_trk->auxdata<ElementLink<xAOD::TrackParticleContainer> >("recoTrackLink") = trackElementLink_orig;
      
      
      //------------------------------------------------------------
      if( m_FillNtuple) m_ntupleVars->get< vector<int> >( "SelTrk_barcode" ).emplace_back(barcode); // will need this later          
      
      #if 0
      // 2016.5.25 Hide Oide - de-commission this part
      if( m_doIntersectionPos ) getIntersection(a_trk, m_ntupleVars->get< vector<InterSectionPos> >( "SVTrk_Int" ), perigee); // get intersection and store values in ntuple
      #endif
      
      ATH_MSG_DEBUG( " > SelGoodTrkParticle: Track index " << a_trk->index() << " has been selected." );
      ATH_MSG_VERBOSE( " > SelGoodTrkParticle: Track index " << a_trk->index()
                       << " parameter:"
                       << " pt = "  << a_trk->pt()
                       << " eta = " << a_trk->eta()
                       << " d0 = "  << a_trk->d0()
                       << " z0 = "  << a_trk->z0() << "." );
      
    } // end of track loop
    
    if( m_FillNtuple ) {
      ATH_MSG_DEBUG( " > SelGoodTrkParticle: Size of Inp Reco / w/barcode = "
                     << trackParticleContainer->size() << "," 
                     << m_ntupleVars->get< vector<int> >( "RecoTrk_barcode" ).size()
                     );
    }
    
    return StatusCode::SUCCESS;
  }
  
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::CutTrk(double PInvVert,double ThetaVert, 
				     double A0Vert, double ZVert, double Chi2, 
				     long int PixelHits,long int SctHits,long int SharedHits, long int BLayHits, long int TRTHits)
  {
    
    double Pt = sin(ThetaVert)/fabs(PInvVert);
    //- Track quality
    if( m_FillHist ) m_trkSelCuts->Fill(20.); 

    if(Pt               < m_TrkPtCut) 			return StatusCode::FAILURE;
    if( m_FillHist ) m_trkSelCuts->Fill(21.); 
    
    if(Chi2 	         > m_TrkChi2Cut) 		return StatusCode::FAILURE;
    if( m_FillHist ) m_trkSelCuts->Fill(22.); 

    if(fabs(A0Vert)     < m_a0TrkPVDstMinCut) 	        return StatusCode::FAILURE;
    if(fabs(A0Vert)     > m_a0TrkPVDstMaxCut) 	        return StatusCode::FAILURE;
    if( m_FillHist ) m_trkSelCuts->Fill(23.); 

    if(fabs(ZVert)     < m_zTrkPVDstMinCut) 	        return StatusCode::FAILURE;
    if(fabs(ZVert)     > m_zTrkPVDstMaxCut) 	        return StatusCode::FAILURE;
    if( m_FillHist ) m_trkSelCuts->Fill(24.); 

    // do Pixel/SCT/SiHits only if we exclude StandAlone TRT hits
    //
    if( !m_SAloneTRT ) {
      if(PixelHits	    < m_CutPixelHits) 	        return StatusCode::FAILURE;
      if( m_FillHist ) m_trkSelCuts->Fill(25.); 

      if(SctHits            < m_CutSctHits)             return StatusCode::FAILURE;
      if( m_FillHist ) m_trkSelCuts->Fill(26.); 

      if((PixelHits+SctHits) < m_CutSiHits)             return StatusCode::FAILURE;
      if( m_FillHist ) m_trkSelCuts->Fill(27.); 
      if(BLayHits	    < m_CutBLayHits) 		return StatusCode::FAILURE;
      if( m_FillHist ) m_trkSelCuts->Fill(28.); 
      if(SharedHits	    > m_CutSharedHits)          return StatusCode::FAILURE;
      if( m_FillHist ) m_trkSelCuts->Fill(29.); 
      
    }

    // The folloing part reproduces the track selection in RPVDixpVrt
    if(m_doTRTPixCut){
      if(TRTHits == 0 && PixelHits < 2)                 return StatusCode::FAILURE;
      if( m_FillHist ) m_trkSelCuts->Fill(30.);
    }
    return StatusCode::SUCCESS;
  }
 
  
} // end of namespace VKalVrtAthena
