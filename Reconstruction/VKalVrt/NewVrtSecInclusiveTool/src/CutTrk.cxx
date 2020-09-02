/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
///
///   @author   V.Kostykhin <Vadim.Kostyukhin@cern.ch>
/// 

// Header include
#include "NewVrtSecInclusiveTool/NewVrtSecInclusiveTool.h"
#include  "AnalysisUtils/AnalysisMisc.h"
#include  "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

#include "TH1.h"

//-------------------------------------------------
namespace Rec{


//==============================================================================================================
//          xAOD based stuff
//
   void NewVrtSecInclusiveTool::selGoodTrkParticle( workVectorArrxAOD * xAODwrk,
                                                    const xAOD::Vertex & PrimVrt)
   const
   {    
    std::vector<const xAOD::TrackParticle*>& inpTrk          = xAODwrk->inpTrk;
    std::vector<const xAOD::TrackParticle*>& selectedTracks  = xAODwrk->listSelTracks;
    std::vector<const xAOD::TrackParticle*>::const_iterator   i_ntrk;
    std::vector<double> impact,impactError;
    std::multimap<double,const xAOD::TrackParticle*> orderedTrk;
    if(m_fillHist){ m_hb_ntrkInput->Fill( inpTrk.size()+0.1, m_w_1);}
    for (i_ntrk = inpTrk.begin(); i_ntrk < inpTrk.end(); ++i_ntrk) {
//
//-- Perigee in TrackParticle
//
          if(m_fillHist){ m_hb_trkSelect->Fill( 0., m_w_1);}
          if((*i_ntrk)->numberDoF() == 0) continue; //Protection
          double trkChi2 = (*i_ntrk)->chiSquared() / (*i_ntrk)->numberDoF();
          if(trkChi2 	      > m_cutChi2)           continue;
          if( (*i_ntrk)->pt()  < m_cutPt)            continue;
          if(m_fillHist){ m_hb_trkSelect->Fill( 1., m_w_1);}
 
          const Trk::Perigee mPer=(*i_ntrk)->perigeeParameters() ;
          double CovTrkMtx00 = (*(mPer.covariance()))(0,0);
 
          uint8_t PixelHits,SctHits,BLayHits;
          if( !((*i_ntrk)->summaryValue(PixelHits,xAOD::numberOfPixelHits)) )   continue; // Track is 
          if( !((*i_ntrk)->summaryValue(  SctHits,xAOD::numberOfSCTHits))   )   continue; // definitely  
          if( SctHits<3 )                                                       continue; // bad
          if( !((*i_ntrk)->summaryValue(BLayHits,xAOD::numberOfInnermostPixelLayerHits)))  BLayHits=0;
          if(m_fillHist){ m_hb_trkSelect->Fill( 2., m_w_1);}

          uint8_t splSCTHits,outSCTHits,splPixHits,outPixHits;
          if( !((*i_ntrk)->summaryValue(splSCTHits,xAOD::numberOfSCTSpoiltHits)))  splSCTHits=0;
          if( !((*i_ntrk)->summaryValue(outSCTHits,xAOD::numberOfSCTOutliers)))    outSCTHits=0;
          if( !((*i_ntrk)->summaryValue(splPixHits,xAOD::numberOfPixelSpoiltHits)))splPixHits=0;
          if( !((*i_ntrk)->summaryValue(outPixHits,xAOD::numberOfPixelOutliers)))  outPixHits=0;

          Amg::Vector3D perigeePos=mPer.position();
          double impactA0=sqrt( (perigeePos.x()-PrimVrt.x())*(perigeePos.x()-PrimVrt.x())
                               +(perigeePos.y()-PrimVrt.y())*(perigeePos.y()-PrimVrt.y()) );
          double impactZ=perigeePos.z()-PrimVrt.z();
          if(m_fillHist){  
            m_hb_trkD0->Fill( impactA0, m_w_1);
            m_hb_trkZ ->Fill( impactZ, m_w_1);
          }
          if(fabs(impactZ)>m_cutZVrt) continue;
          if(impactA0>m_cutA0)        continue;
          if(m_fillHist){ m_hb_trkSelect->Fill( 3., m_w_1);}
     
          double bX=xAODwrk->beamX + (perigeePos.z()-xAODwrk->beamZ)*xAODwrk->tanBeamTiltX;
          double bY=xAODwrk->beamY + (perigeePos.z()-xAODwrk->beamZ)*xAODwrk->tanBeamTiltY;
          double impactBeam=sqrt( (perigeePos.x()-bX)*(perigeePos.x()-bX) + (perigeePos.y()-bY)*(perigeePos.y()-bY));
//----Anti-pileup
          double signifBeam = impactBeam    / sqrt(CovTrkMtx00);
          if(signifBeam < m_antiPileupSigRCut) continue;   // cut against tracks from pileup vertices
          if(m_fillHist){ m_hb_trkSelect->Fill( 4., m_w_1);}
//----
          if(PixelHits	    < m_cutPixelHits) 		continue;
          if(SctHits	    < m_cutSctHits) 		continue;
          if((PixelHits+SctHits) < m_cutSiHits) 	continue;
          if(BLayHits	    < m_cutBLayHits) 		continue;
          if(m_fillHist){ m_hb_trkSelect->Fill( 5., m_w_1);}
//----
          orderedTrk.emplace(signifBeam,*i_ntrk);
          selectedTracks.push_back(*i_ntrk);
      }
//---- Order tracks according to pt
//      AnalysisUtils::Sort::pT (selectedTracks)     
//---- Order tracks according to ranks
      std::map<double,const xAOD::TrackParticle*>::reverse_iterator rt=orderedTrk.rbegin();
      selectedTracks.resize(orderedTrk.size());
      for ( int cntt=0; rt!=orderedTrk.rend(); ++rt,++cntt) {selectedTracks[cntt]=(*rt).second;}
      return;
   }

}//end namespace
