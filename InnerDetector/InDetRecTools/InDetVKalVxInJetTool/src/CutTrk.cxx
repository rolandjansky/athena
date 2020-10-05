/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
// Author: Vadim Kostyukhin (vadim.kostyukhin@cern.ch)

// Header include
#include "InDetVKalVxInJetTool/InDetVKalVxInJetTool.h"
#include  "AnalysisUtils/AnalysisMisc.h"
#include  "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

#include "TH1F.h"

//-------------------------------------------------
namespace InDet{



  StatusCode InDetVKalVxInJetTool::cutTrk(double PInvVert,double ThetaVert, 
         double A0Vert, double ZVert, double Chi2, 
         long int PixelHits,long int SctHits,long int SharedHits, long int BLayHits)
  const
  {
     double Pt = sin(ThetaVert)/std::abs(PInvVert);
//- Track quality
     if(Pt               < m_cutPt) 			return StatusCode::FAILURE;
     if(!m_multiWithPrimary){           //Must not be used for primary vertex search
       if(std::abs(ZVert)      > m_cutZVrt)	                return StatusCode::FAILURE;
     }
     if(Chi2 	         > m_cutChi2) 			return StatusCode::FAILURE;
     if(std::abs(A0Vert)     > m_cutA0) 			return StatusCode::FAILURE;


     if(PixelHits	    < m_cutPixelHits) 		return StatusCode::FAILURE;
     if(SctHits		    < m_cutSctHits) 		return StatusCode::FAILURE;
     if((PixelHits+SctHits) < m_cutSiHits) 		return StatusCode::FAILURE;
     if(BLayHits	    < m_cutBLayHits) 		return StatusCode::FAILURE;
     if(SharedHits	    > m_cutSharedHits) 		return StatusCode::FAILURE;

     return StatusCode::SUCCESS;
  }
 
//==============================================================================================================
//          xAOD based stuff
//
   int  InDetVKalVxInJetTool::selGoodTrkParticle( const std::vector<const xAOD::TrackParticle*>& InpTrk,
                                                  const xAOD::Vertex                           & PrimVrt,
	                                          const TLorentzVector                         & JetDir,
                                                        std::vector<const xAOD::TrackParticle*>& SelectedTracks)
   const
   {    

    std::vector<const xAOD::TrackParticle*>::const_iterator   i_ntrk;
    std::vector<double> Impact,ImpactError;
    std::multimap<double,const xAOD::TrackParticle*> orderedTrk;
    int nPrimTrk=0;
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



	  if ( CovTrkMtx11 > m_a0TrkErrorCut*m_a0TrkErrorCut )  continue;
	  if ( CovTrkMtx22 > m_zTrkErrorCut*m_zTrkErrorCut )    continue;
	  if ( coneDist(VectPerig,JetDir) > m_coneForTag )      continue;
          if( (*i_ntrk)->pt() > JetDir.Pt() )                   continue;

          double trkP=1./std::abs(VectPerig[4]);         
          if(trkP>10000.){  double trkPErr=std::sqrt(CovTrkMtx55)*trkP;
	                    if(m_fillHist)m_hb_trkPErr->Fill( trkPErr , m_w_1);       
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

          Amg::Vector3D perigeePos=mPer.position();
          double ImpactA0=std::sqrt( (perigeePos.x()-PrimVrt.x())*(perigeePos.x()-PrimVrt.x())
                               +(perigeePos.y()-PrimVrt.y())*(perigeePos.y()-PrimVrt.y()) );
          double ImpactZ=perigeePos.z()-PrimVrt.z();
          double ImpactSignif=std::sqrt(ImpactA0*ImpactA0/CovTrkMtx11+ImpactZ*ImpactZ/CovTrkMtx22);

          //double ImpactSignif = m_fitSvc->VKalGetImpact((*i_ntrk), PrimVrt.position(), 1, Impact, ImpactError);
          //double ImpactA0=Impact[0];  
          //double ImpactZ=Impact[1];   
          if(m_fillHist){  m_hb_trkD0->Fill( ImpactA0, m_w_1); }
//---- Improved cleaning
/////          if(PixelHits<=2 && ( outPixHits || splPixHits )) continue;  //VK Bad idea at high Pt!
          if(std::abs((*i_ntrk)->eta())>2.  ) {
            if( PixelHits<=3 && ( splSCTHits || outSCTHits || outPixHits || splPixHits ))continue;
            if(m_existIBL){if(PixelHits)PixelHits -=1; if(SctHits)SctHits  -=1;}             // 4-layer pixel detector
            else          {if(PixelHits)PixelHits -=1;}                                      // 3-layer pixel detector
          }
          if(std::abs((*i_ntrk)->eta())>1.65)   if(SctHits)SctHits   -=1;
//----Anti-pileup cut (in Sel2TrVrt now)
//          double SignifR = Impact[0]/ std::sqrt(ImpactError[0]);
//          if(std::abs(SignifR) < m_AntiPileupSigRCut) {   // cut against tracks from pileup vertices
//            double SignifZ = Impact[1]/ std::sqrt(ImpactError[2]);
//            if(SignifZ > 1.+m_AntiPileupSigZCut ) continue;
//            if(SignifZ < 1.-m_AntiPileupSigZCut ) continue;
//          }
//----
          StatusCode sc = cutTrk( VectPerig[4] , VectPerig[3],
                          ImpactA0 , ImpactZ, trkChi2,
                          PixelHits, SctHits, SharedHits, BLayHits);
          if( sc.isFailure() )                 continue;
	  double rnkBTrk=rankBTrk(0.,0.,ImpactSignif);
	  if(rnkBTrk<0.7)nPrimTrk += 1;
	  orderedTrk.emplace(rnkBTrk,*i_ntrk);
      }
//---- Order tracks according to ranks
      std::map<double,const xAOD::TrackParticle*>::reverse_iterator rt=orderedTrk.rbegin();
      SelectedTracks.resize(orderedTrk.size());
      for ( int cntt=0; rt!=orderedTrk.rend(); ++rt,++cntt) {SelectedTracks[cntt]=(*rt).second;}
      return nPrimTrk;
   }

}//end namespace
