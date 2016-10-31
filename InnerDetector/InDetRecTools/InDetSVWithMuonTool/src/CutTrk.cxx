/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "InDetSVWithMuonTool/InDetSVWithMuonTool.h"
#include  "AnalysisUtils/AnalysisMisc.h"
#include <iostream>
//-------------------------------------------------
namespace InDet{

  StatusCode InDetSVWithMuonTool::CutTrk(double PInvVert,double ThetaVert, 
         double A0Vert, double ZVert, double Chi2, 
         long int PixelHits,long int SctHits,long int SharedHits, long int BLayHits)
  const
  {
     double Pt = sin(ThetaVert)/fabs(PInvVert);
//- Track quality
     if(Pt               < m_CutPt) 			return StatusCode::FAILURE;
     if(fabs(ZVert)      > m_CutZVrt/sin(ThetaVert))	return StatusCode::FAILURE;
     if(Chi2 	         > m_CutChi2) 			return StatusCode::FAILURE;
     if(fabs(A0Vert)     > m_CutA0) 			return StatusCode::FAILURE;


     if(PixelHits	    < m_CutPixelHits) 		return StatusCode::FAILURE;
     if(SctHits		    < m_CutSctHits) 		return StatusCode::FAILURE;
     if((PixelHits+SctHits) < m_CutSiHits) 		return StatusCode::FAILURE;
     if(BLayHits	    < m_CutBLayHits) 		return StatusCode::FAILURE;
     if(SharedHits	    > m_CutSharedHits) 		return StatusCode::FAILURE;

     return StatusCode::SUCCESS;
  }
 




//==============================================================================================================
//          xAOD based stuff
//
   void  InDetSVWithMuonTool::SelGoodTrkParticle( const std::vector<const xAOD::TrackParticle*>& InpTrk,
                                              const xAOD::Vertex                           & PrimVrt,
	                                      const xAOD::TrackParticle                    * Muon,
                                              std::vector<const xAOD::TrackParticle*>& SelectedTracks)
   const
   {    

    std::vector<const xAOD::TrackParticle*>::const_iterator   i_ntrk;
    std::vector<double> Impact,ImpactError;
    for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//
//-- Perigee in TrackParticle
//
          const Trk::Perigee m_mPer=(*i_ntrk)->perigeeParameters() ;
          AmgVector(5) VectPerig = m_mPer.parameters(); 


          if((*i_ntrk)->numberDoF() == 0) continue; //Protection
          double trkChi2 = (*i_ntrk)->chiSquared() / (*i_ntrk)->numberDoF();

          double CovTrkMtx11 = (*i_ntrk)->definingParametersCovMatrix()(0,0);
          double CovTrkMtx22 = (*i_ntrk)->definingParametersCovMatrix()(1,1);
          double CovTrkMtx55 = (*i_ntrk)->definingParametersCovMatrix()(4,4);



	  if ( CovTrkMtx11 > m_A0TrkErrorCut*m_A0TrkErrorCut )  continue;
	  if ( CovTrkMtx22 > m_ZTrkErrorCut*m_ZTrkErrorCut )    continue;
	  if( ConeDist(VectPerig,Muon->p4()) > m_ConeForTag )   continue;
	  if( ConeDist(VectPerig,Muon->p4()) < 0.005 )          continue;  //muon itself. To protect code.

          double trkP=1./fabs(VectPerig[4]);         
          if(trkP>10000.){  double trkPErr=sqrt(CovTrkMtx55)*trkP;
                            if(trkPErr>0.5) continue;   }

          uint8_t PixelHits,SctHits,BLayHits;
          if( !((*i_ntrk)->summaryValue(PixelHits,xAOD::numberOfPixelHits)) )   continue; // Track is 
          if( !((*i_ntrk)->summaryValue(  SctHits,xAOD::numberOfSCTHits))   )   continue; // definitely  
          if( SctHits<3 )                                                       continue; // bad
          if( !((*i_ntrk)->summaryValue(BLayHits,xAOD::numberOfInnermostPixelLayerHits)))  BLayHits=0;
          long int SharedHits = 0;  //VK Should always be

          uint8_t splSCTHits,outSCTHits,splPixHits,outPixHits;
          if( !((*i_ntrk)->summaryValue(splSCTHits,xAOD::numberOfSCTSpoiltHits)))  splSCTHits=0;
          if( !((*i_ntrk)->summaryValue(outSCTHits,xAOD::numberOfSCTOutliers)))    outSCTHits=0;
          if( !((*i_ntrk)->summaryValue(splPixHits,xAOD::numberOfPixelSpoiltHits)))splPixHits=0;
          if( !((*i_ntrk)->summaryValue(outPixHits,xAOD::numberOfPixelOutliers)))  outPixHits=0;


          m_fitSvc->VKalGetImpact((*i_ntrk), PrimVrt.position(), 1, Impact, ImpactError);
          double ImpactA0=VectPerig[0];                         // Temporary
          double ImpactZ=VectPerig[1]-PrimVrt.position().z();   // Temporary
	  ImpactA0=Impact[0];  
	  ImpactZ=Impact[1];   
          if(fabs((*i_ntrk)->eta())>2.  ) {
            if( PixelHits<=3 && ( splSCTHits || outSCTHits || outPixHits || splPixHits ))continue;
            if(m_existIBL){PixelHits -=1; SctHits   -=1;}             // 4-layer pixel detector
            else          {PixelHits -=1;}                            // 3-layer pixel detector
          }
          if(fabs((*i_ntrk)->eta())>1.65)  SctHits   -=1;
//----
          StatusCode sc = CutTrk( VectPerig[4] , VectPerig[3],
                          ImpactA0 , ImpactZ, trkChi2,
		          PixelHits, SctHits, SharedHits, BLayHits);  //
          if( sc.isFailure() )                 continue;
	  SelectedTracks.push_back(*i_ntrk);
      }
      AnalysisUtils::Sort::pT (&SelectedTracks); // no equivalent for TrkTrack yet...
      return;
   }


}//end namespace
