/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "InDetVKalVxInJetTool/InDetVKalVxInJetTool.h"
#include  "AnalysisUtils/AnalysisMisc.h"
//#include <iostream>
//-------------------------------------------------
namespace InDet{

//double getConeSize(double pt){   //Variable cone size from b-tagging for reference
//  double  m_coneSizeFitPar1 = +0.239;
//  double  m_coneSizeFitPar2 = -1.220;
//  double  m_coneSizeFitPar3 = -1.64e-5;
//  return (m_coneSizeFitPar1 + exp(m_coneSizeFitPar2 + m_coneSizeFitPar3*pt));
//}


  StatusCode InDetVKalVxInJetTool::CutTrk(double PInvVert,double ThetaVert, 
         double A0Vert, double ZVert, double Chi2, 
         long int PixelHits,long int SctHits,long int SharedHits, long int BLayHits)
  const
  {
     double Pt = sin(ThetaVert)/fabs(PInvVert);
//- Track quality
     if(Pt               < m_CutPt) 			return StatusCode::FAILURE;
//std::cout<<" ZVert="<<ZVert<<", "<<sin(ThetaVert)<<'\n';
//std::cout<<" Chi2="<<Chi2<<'\n';
//std::cout<<" A0Vert="<<A0Vert<<'\n';
     if(!m_MultiWithPrimary){           //Must not be used for primary vertex search
       if(fabs(ZVert)      > m_CutZVrt/sin(ThetaVert))	return StatusCode::FAILURE;
     }
     if(Chi2 	         > m_CutChi2) 			return StatusCode::FAILURE;
     if(fabs(A0Vert)     > m_CutA0) 			return StatusCode::FAILURE;


     if(PixelHits	    < m_CutPixelHits) 		return StatusCode::FAILURE;
     if(SctHits		    < m_CutSctHits) 		return StatusCode::FAILURE;
     if((PixelHits+SctHits) < m_CutSiHits) 		return StatusCode::FAILURE;
     if(BLayHits	    < m_CutBLayHits) 		return StatusCode::FAILURE;
     if(SharedHits	    > m_CutSharedHits) 		return StatusCode::FAILURE;

     return StatusCode::SUCCESS;
  }
 




   int  InDetVKalVxInJetTool::SelGoodTrkParticle( const std::vector<const Rec::TrackParticle*>& InpTrk,
                                                  const xAOD::Vertex                          & PrimVrt,
	                                          const TLorentzVector                        & JetDir,
                                                  std::vector<const Rec::TrackParticle*>   & SelectedTracks)
   const
   {    

    std::vector<const Rec::TrackParticle*>::const_iterator   i_ntrk;
    AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
    const Trk::Perigee* mPer;
    const Trk::FitQuality*     TrkQual;
    std::vector<double> Impact,ImpactError;
    int NPrimTrk=0;
    for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//
//-- Perigee in TrackParticle
//
          mPer=GetPerigee( (*i_ntrk) ) ;
          if( mPer == NULL )  continue;
          VectPerig = mPer->parameters(); 
          TrkQual   = (*i_ntrk)->fitQuality();
          if(TrkQual && TrkQual->numberDoF()== 0) continue; //Protection
          double trkChi2=1.; if(TrkQual) trkChi2=TrkQual->chiSquared() / TrkQual->numberDoF();
          double CovTrkMtx11 = (*(mPer->covariance()))(0,0);
          double CovTrkMtx22 = (*(mPer->covariance()))(1,1);

	  if ( CovTrkMtx11 > m_A0TrkErrorCut*m_A0TrkErrorCut )  continue;
	  if ( CovTrkMtx22 > m_ZTrkErrorCut*m_ZTrkErrorCut )    continue;
	  if( ConeDist(VectPerig,JetDir) > m_ConeForTag )       continue;

          double trkP=1./fabs(VectPerig[4]);         
          double CovTrkMtx55 = (*(mPer->covariance()))(4,4);
          if(trkP>10000.){  double trkPErr=sqrt(CovTrkMtx55)*trkP;
	                    if(m_FillHist)m_hb_trkPErr->Fill( trkPErr , m_w_1);       
                            if(trkPErr>0.5) continue;   }

          long int PixelHits     = 3;
          long int SctHits       = 9; 
          long int SharedHits    = 0; //Always 0 now
          long int BLayHits      = 1;
//----------------------------------- Summary tools
          const Trk::TrackSummary* testSum = (*i_ntrk)->trackSummary();
          PixelHits = (long int) testSum->get(Trk::numberOfPixelHits);
          SctHits   = (long int) testSum->get(Trk::numberOfSCTHits);
          BLayHits  = (long int) testSum->get(Trk::numberOfInnermostPixelLayerHits);
	  if(PixelHits < 0 ) PixelHits=0; 
	  if(SctHits   < 0 ) SctHits=0; 
	  if(BLayHits  < 0 ) BLayHits=0; 
//std::cout<<"NwTrkSummary="<<PixelHits<<", "<<SctHits<<", "<<BLayHits<<", fitter="<<'\n';
          double ImpactSignif = m_fitSvc->VKalGetImpact((*i_ntrk), PrimVrt.position(), 1, Impact, ImpactError);
          double ImpactA0=VectPerig[0];                         // Temporary
          double ImpactZ=VectPerig[1]-PrimVrt.position().z();   // Temporary
	  ImpactA0=Impact[0];  
	  ImpactZ=Impact[1];   
//----
          StatusCode sc = CutTrk( VectPerig[4] , VectPerig[3],
                       ImpactA0 , ImpactZ, trkChi2,
		       PixelHits, SctHits, SharedHits, BLayHits);

					  
          if( sc.isFailure() )                 continue;
	  if(ImpactSignif < 3.)NPrimTrk += 1;
	  SelectedTracks.push_back(*i_ntrk);
      }
      AnalysisUtils::Sort::pT (&SelectedTracks); // no equivalent for TrkTrack yet...
      return NPrimTrk;
   }
//
//  Simplified TrackParticle selection to get reliably reconstructed tracks for jet momentum estimation 
// 
   int  InDetVKalVxInJetTool::SelGoodTrkParticleRelax( const std::vector<const Rec::TrackParticle*>& InpTrk,
                                         const xAOD::Vertex                                        & PrimVrt,
	                                 const TLorentzVector                                      & JetDir,
                                               std::vector<const Rec::TrackParticle*>    & SelectedTracks)
   const
   {    

    std::vector<const Rec::TrackParticle*>::const_iterator   i_ntrk;
    AmgVector(5) VectPerig; VectPerig<<0.,0.,0.,0.,0.;
    const Trk::Perigee* mPer;
    std::vector<double> Impact,ImpactError;
    int NPrimTrk=0;
    for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//
//-- MeasuredPerigee in TrackParticle
//
          mPer=GetPerigee( (*i_ntrk) ) ;
          if( mPer == NULL ){ continue; } 
          VectPerig = mPer->parameters(); 
	  if( ConeDist(VectPerig,JetDir) > m_ConeForTag )       continue;
//----------------------------------- Summary tools
          const Trk::TrackSummary* testSum = (*i_ntrk)->trackSummary();
          long int PixelHits = (long int) testSum->get(Trk::numberOfPixelHits);
          long int SctHits   = (long int) testSum->get(Trk::numberOfSCTHits);
	  if(PixelHits < 0 ) PixelHits=0; 
	  if(SctHits   < 0 ) SctHits=0; 

          double ImpactSignif = m_fitSvc->VKalGetImpact((*i_ntrk), PrimVrt.position(), 1, Impact, ImpactError);
          if(fabs(Impact[0])     > m_CutA0)			continue;
          if(fabs(Impact[1])     > m_CutZVrt/sin(VectPerig[3]))	continue;

          if(PixelHits	< m_CutPixelHits) 	continue;
          if(SctHits	< m_CutSctHits) 	continue;
          if((PixelHits+SctHits) < m_CutSiHits) continue;
	  if(ImpactSignif < 3.)NPrimTrk += 1;
	  SelectedTracks.push_back(*i_ntrk);
      }
      AnalysisUtils::Sort::pT (&SelectedTracks); // no equivalent for TrkTrack yet...
      return NPrimTrk;
  }
//==============================================================================================================
//          xAOD based stuff
//
   int  InDetVKalVxInJetTool::SelGoodTrkParticle( const std::vector<const xAOD::TrackParticle*>& InpTrk,
                                                  const xAOD::Vertex                           & PrimVrt,
	                                          const TLorentzVector                         & JetDir,
                                                        std::vector<const xAOD::TrackParticle*>& SelectedTracks)
   const
   {    

    std::vector<const xAOD::TrackParticle*>::const_iterator   i_ntrk;
    std::vector<double> Impact,ImpactError;
    int NPrimTrk=0;
    for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//
//-- Perigee in TrackParticle
//
          const Trk::Perigee mPer=(*i_ntrk)->perigeeParameters() ;
          AmgVector(5) VectPerig = mPer.parameters(); 


          if((*i_ntrk)->numberDoF() == 0) continue; //Protection
          double trkChi2 = (*i_ntrk)->chiSquared() / (*i_ntrk)->numberDoF();

          double CovTrkMtx11 = (*i_ntrk)->definingParametersCovMatrix()(0,0);
          double CovTrkMtx22 = (*i_ntrk)->definingParametersCovMatrix()(1,1);
          double CovTrkMtx55 = (*i_ntrk)->definingParametersCovMatrix()(4,4);



	  if ( CovTrkMtx11 > m_A0TrkErrorCut*m_A0TrkErrorCut )  continue;
	  if ( CovTrkMtx22 > m_ZTrkErrorCut*m_ZTrkErrorCut )    continue;
	  if ( ConeDist(VectPerig,JetDir) > m_ConeForTag )       continue;

          double trkP=1./fabs(VectPerig[4]);         
          if(trkP>10000.){  double trkPErr=sqrt(CovTrkMtx55)*trkP;
	                    if(m_FillHist)m_hb_trkPErr->Fill( trkPErr , m_w_1);       
                            if(trkPErr>0.5) continue;   }

          uint8_t PixelHits,SctHits,BLayHits;
          if( !((*i_ntrk)->summaryValue(PixelHits,xAOD::numberOfPixelHits)) )   continue; // Track is 
          if( !((*i_ntrk)->summaryValue(  SctHits,xAOD::numberOfSCTHits))   )   continue; // definitely  
          if( SctHits<3 )                                                       continue; // bad
          if( !((*i_ntrk)->summaryValue(BLayHits,xAOD::numberOfInnermostPixelLayerHits)))  BLayHits=0;
          //if( !((*i_ntrk)->summaryValue(sctSharedHits,xAOD::numberOfSCTSharedHits)))  sctSharedHits=0;  //VK Bad for high Pt
          //if( !((*i_ntrk)->summaryValue(pixSharedHits,xAOD::numberOfPixelSharedHits)))pixSharedHits=0;  //and many tracks
          //long int SharedHits = sctSharedHits+pixSharedHits; 
          long int SharedHits = 0;  //VK Should always be

          uint8_t splSCTHits,outSCTHits,splPixHits,outPixHits;
          if( !((*i_ntrk)->summaryValue(splSCTHits,xAOD::numberOfSCTSpoiltHits)))  splSCTHits=0;
          if( !((*i_ntrk)->summaryValue(outSCTHits,xAOD::numberOfSCTOutliers)))    outSCTHits=0;
          if( !((*i_ntrk)->summaryValue(splPixHits,xAOD::numberOfPixelSpoiltHits)))splPixHits=0;
          if( !((*i_ntrk)->summaryValue(outPixHits,xAOD::numberOfPixelOutliers)))  outPixHits=0;
          //uint8_t BLaySharedH,BLaySplitH,BLayOutlier;
          //if( !((*i_ntrk)->summaryValue(BLaySharedH,xAOD::numberOfInnermostPixelLayerSharedHits)) )  BLaySharedH=-1;
          //if( !((*i_ntrk)->summaryValue(BLaySplitH ,xAOD::numberOfInnermostPixelLayerSplitHits))  )  BLaySplitH=-1;
          //if( !((*i_ntrk)->summaryValue(BLayOutlier,xAOD::numberOfInnermostPixelLayerOutliers))   )  BLayOutlier=-1;
//std::cout<<"NwBlayer="<<(long int)BLaySharedH<<", "<<(long int)BLaySplitH<<", "<<(long int)BLayOutlier<<'\n';
          //uint8_t InmHits,InmSharedH,InmSplitH,InmOutlier;
          //if( !((*i_ntrk)->summaryValue(InmHits,   xAOD::numberOfInnermostHits)) )        InmHits=-1;
          //if( !((*i_ntrk)->summaryValue(InmSharedH,xAOD::numberOfInnermostSharedHits)) )  InmSharedH=-1;
          //if( !((*i_ntrk)->summaryValue(InmSplitH ,xAOD::numberOfInnermostSplitHits))  )  InmSplitH=-1;
          //if( !((*i_ntrk)->summaryValue(InmOutlier,xAOD::numberOfInnermostOutliers))   )  InmOutlier=-1;
//std::cout<<"NwInnerM="<<(long int)InmHits<<", "<<(long int)InmSharedH<<", "<<(long int)InmSplitH<<", "<<(long int)InmOutlier<<'\n';


          double ImpactSignif = m_fitSvc->VKalGetImpact((*i_ntrk), PrimVrt.position(), 1, Impact, ImpactError);
          double ImpactA0=VectPerig[0];                         // Temporary
          double ImpactZ=VectPerig[1]-PrimVrt.position().z();   // Temporary
	  ImpactA0=Impact[0];  
	  ImpactZ=Impact[1];   
//---- Improved cleaning
/////          if(PixelHits<=2 && ( outPixHits || splPixHits )) continue;  //VK Bad idea at high Pt!
          if(fabs((*i_ntrk)->eta())>2.  ) {
            if( PixelHits<=3 && ( splSCTHits || outSCTHits || outPixHits || splPixHits ))continue;
            if(m_existIBL){if(PixelHits)PixelHits -=1; if(SctHits)SctHits  -=1;}             // 4-layer pixel detector
            else          {if(PixelHits)PixelHits -=1;}                                      // 3-layer pixel detector
          }
          if(fabs((*i_ntrk)->eta())>1.65)   if(SctHits)SctHits   -=1;
//----
          StatusCode sc = CutTrk( VectPerig[4] , VectPerig[3],
                          ImpactA0 , ImpactZ, trkChi2,
		          PixelHits, SctHits, SharedHits, BLayHits);  //
          if( sc.isFailure() )                 continue;
	  if(ImpactSignif < 3.)NPrimTrk += 1;
	  SelectedTracks.push_back(*i_ntrk);
      }
      AnalysisUtils::Sort::pT (&SelectedTracks); // no equivalent for TrkTrack yet...
      return NPrimTrk;
   }
//
//  Simplified xAOD::TrackParticle selection to get reliably reconstructed tracks for jet momentum estimation 
// 
   int  InDetVKalVxInJetTool::SelGoodTrkParticleRelax( const std::vector<const xAOD::TrackParticle*>& InpTrk,
                                                       const xAOD::Vertex                           & PrimVrt,
	                                               const TLorentzVector                         & JetDir,
                                                             std::vector<const xAOD::TrackParticle*>& SelectedTracks)
   const
   {    
    std::vector<const xAOD::TrackParticle*>::const_iterator   i_ntrk;
    std::vector<double> Impact,ImpactError;
    int NPrimTrk=0;
    for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//
//-- MeasuredPerigee in xAOD::TrackParticle
//
          const Trk::Perigee mPer=(*i_ntrk)->perigeeParameters() ;
          AmgVector(5) VectPerig = mPer.parameters(); 
	  if( ConeDist(VectPerig,JetDir) > m_ConeForTag )       continue;
//----------------------------------- Summary tools
          uint8_t PixelHits,SctHits;
          if( !((*i_ntrk)->summaryValue(PixelHits,xAOD::numberOfPixelHits)) ) PixelHits=0;
          if( !((*i_ntrk)->summaryValue(  SctHits,xAOD::numberOfSCTHits))   )   SctHits=0;
 

          double ImpactSignif = m_fitSvc->VKalGetImpact((*i_ntrk), PrimVrt.position(), 1, Impact, ImpactError);
          if(fabs(Impact[0])     > m_CutA0)			continue;
          if(fabs(Impact[1])     > m_CutZVrt/sin(VectPerig[3]))	continue;

          int currCutPixelHits=m_CutPixelHits; if(fabs((*i_ntrk)->eta())>2.  )currCutPixelHits +=1;
          int currCutSctHits  =m_CutSctHits;   if(fabs((*i_ntrk)->eta())>1.65)currCutSctHits   +=1;
 
          if(PixelHits 	         < currCutPixelHits) continue;
          if(SctHits	         < currCutSctHits)   continue;
          if((PixelHits+SctHits) < m_CutSiHits)	     continue;
	  if(ImpactSignif < 3.)NPrimTrk += 1;
	  SelectedTracks.push_back(*i_ntrk);
      }
      AnalysisUtils::Sort::pT (&SelectedTracks); // no equivalent for TrkTrack yet...
      return NPrimTrk;
  }



}//end namespace
