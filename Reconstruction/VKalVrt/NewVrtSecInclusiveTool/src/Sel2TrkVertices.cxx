/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "NewVrtSecInclusiveTool/NewVrtSecInclusiveTool.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include  "MVAUtils/BDT.h" 
//-------------------------------------------------
// Other stuff
#include  "AnalysisUtils/AnalysisMisc.h"
#include  "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

#include "TMath.h"
#include "TH1.h"
#include "TH2D.h"
//
//#include<iostream>


namespace Rec{


//
//
//--------------------------------------------------------
//   Template routine for 2track secondary vertices selection
//

    void NewVrtSecInclusiveTool::Select2TrVrt(std::vector<const xAOD::TrackParticle*>  & SelectedTracks,
                                  const xAOD::Vertex                 & PrimVrt)
    const
    {
      std::vector<const xAOD::NeutralParticle*> neutralPartDummy(0);
      std::vector<const xAOD::TrackParticle*>  TracksForFit(2,0);
      std::vector<double> Impact,ImpactError;
      std::vector<double> InpMass(2,m_massPi);
      long int      Charge;
      int i,j;
      StatusCode sc; sc.setChecked();
      Vrt2Tr tmpVrt;
      std::vector<Vrt2Tr> all2TrVrt(0);
      TLorentzVector PSum2T; 
      Amg::Vector3D iniVrt(0.,0.,0.);
 //
      int NTracks = (int) (SelectedTracks.size());
//
//  Impact parameters with sign calculations
//
      std::vector<float> covPV=PrimVrt.covariance(); 
      double SignifR=0.,SignifZ=0.;
      std::vector<int> hitIBL(NTracks,0), hitBL(NTracks,0);
      std::vector<double> TrackSignif(NTracks),dRdZratio(NTracks);
      for (i=0; i<NTracks; i++) {
         m_fitSvc->VKalGetImpact(SelectedTracks[i], PrimVrt.position(), 1, Impact, ImpactError);
	 SignifR = Impact[0]/ sqrt(ImpactError[0]);
	 SignifZ = Impact[1]/ sqrt(ImpactError[2]);
  	 TrackSignif[i] = sqrt( SignifR*SignifR + SignifZ*SignifZ);
	 dRdZratio[i] = fabs(SignifR/SignifZ);
         if(m_fillHist){
	    m_hb_impactR->Fill( SignifR, m_w_1); 
            m_hb_impactZ->Fill( SignifZ, m_w_1); 
            m_hb_impactRZ->Fill(SignifR, SignifZ, m_w_1); 
	    m_hb_impact->Fill( TrackSignif[i], m_w_1);
	    if(TrackSignif[i]>2.) m_hb_pileupRat->Fill(dRdZratio[i],1.);
            if( i<DevTuple::maxNTrk && m_curTup){
	       m_curTup->pttrk[i]=SelectedTracks[i]->pt();
	       m_curTup->Sig3D[i]=TrackSignif[i];
	    }
	 }
      }

      if( m_fillHist && m_curTup ){
          m_curTup->nTrk=std::min(NTracks,DevTuple::maxNTrk);
          m_curTup->n2Vrt=0;
      }

      for (i=0; i<NTracks-1; i++) {
         if(TrackSignif[i]<m_trkSigCut || dRdZratio[i]<m_dRdZRatioCut )continue;
         for (j=i+1; j<NTracks; j++) {
             if(TrackSignif[j]<m_trkSigCut || dRdZratio[j]<m_dRdZRatioCut )continue;
             PSum2T=SelectedTracks[i]->p4()+SelectedTracks[j]->p4();
             if(PSum2T.M()>m_Vrt2TrMassLimit)continue;
             
             std::unique_ptr<Trk::IVKalState> state = m_fitSvc->makeState();
             m_fitSvc->setMassInputParticles( InpMass, *state );     // Use pion masses for fit
             TracksForFit[0]=SelectedTracks[i];
             TracksForFit[1]=SelectedTracks[j];
             sc=m_fitSvc->VKalVrtFitFast(TracksForFit,tmpVrt.FitVertex,*state);            /* Fast crude estimation*/

             if( sc.isFailure()  ) {   /* No initial estimation */ 
                iniVrt=PrimVrt.position();
 	     } else {
                double PMomVrtDir = ProjSV_PV(tmpVrt.FitVertex,PrimVrt,PSum2T);
                if( PMomVrtDir>0. ) iniVrt=tmpVrt.FitVertex;                /* Good initial estimation */ 
                else                iniVrt=PrimVrt.position();
             }
             m_fitSvc->setApproximateVertex(iniVrt.x(), iniVrt.y(), iniVrt.z(),*state);
             tmpVrt.i=i; tmpVrt.j=j;
             sc=m_fitSvc->VKalVrtFit(TracksForFit, neutralPartDummy, tmpVrt.FitVertex, tmpVrt.Momentum, Charge,
                                  tmpVrt.ErrorMatrix, tmpVrt.Chi2PerTrk, tmpVrt.TrkAtVrt, tmpVrt.Chi2, *state, true );
             if(sc.isFailure())                       continue;          /* No fit */ 
             /////if(tmpVrt.Chi2 > m_sel2VrtChi2Cut)       continue;          /* Bad Chi2 */
             double Prob2v=TMath::Prob(tmpVrt.Chi2,1);
             if( Prob2v < m_sel2VrtProbCut )    continue;
	     if( tmpVrt.Momentum.M()> m_Vrt2TrMassLimit )      continue; 
  	     if( ProjSV_PV(tmpVrt.FitVertex, PrimVrt, tmpVrt.Momentum) < 0.) continue;  // SV-PV don't coincide with summary momentum direction
             if( tmpVrt.FitVertex.perp() > m_maxSVRadiusCut) continue;                  // Too far from interaction point
             double cosSVPV=ProjSV_PV(tmpVrt.FitVertex, PrimVrt, tmpVrt.Momentum);
	     TLorentzVector SVPV(tmpVrt.FitVertex.x()-PrimVrt.x(),
	                         tmpVrt.FitVertex.y()-PrimVrt.y(),
	                         tmpVrt.FitVertex.z()-PrimVrt.z(), 10.);
             if(m_fillHist){
               if(Charge==0){m_hb_massPiPi->Fill(tmpVrt.Momentum.M(),1.);}
               m_hb_cosSVMom->Fill(cosSVPV,1.);
               m_hb_etaSV->Fill(SVPV.Eta(),1.);
             }
	     if(cosSVPV<0.8)continue;
	     if(tmpVrt.Momentum.Pt()<1000.)continue;
//Check close material layer
             //double dstMatSignif=distToMatLayerSignificance(tmpVrt); //Doesn't work due to wrong TrkGeometry
//
// Check pixel hits vs vertex positions.
             if(m_useVertexCleaning){  
	       if(!Check2TrVertexInPixel(SelectedTracks[i],SelectedTracks[j],tmpVrt.FitVertex,tmpVrt.ErrorMatrix)) continue;
	     }
//
// Debugging and BDT
	     double minPtT = std::min(TracksForFit[0]->pt(),TracksForFit[1]->pt());
	     double Sig3D=0.,Sig2D=0.,Dist2D=0.; 
             if( m_fillHist && m_curTup ){
                VrtVrtDist(PrimVrt, tmpVrt.FitVertex, tmpVrt.ErrorMatrix, Sig3D);
	        Dist2D=VrtVrtDist2D(PrimVrt, tmpVrt.FitVertex, tmpVrt.ErrorMatrix, Sig2D);
                m_hb_signif3D->Fill(Sig3D,1.);
                m_curTup->VrtTrkHF [m_curTup->n2Vrt] = getIdHF(TracksForFit[0])+ getIdHF(TracksForFit[1]);       
                m_curTup->VrtTrkI  [m_curTup->n2Vrt] = getG4Inter(TracksForFit[0])+ getG4Inter(TracksForFit[1]);       
                m_curTup->VrtCh    [m_curTup->n2Vrt] = Charge;
                m_curTup->VrtProb  [m_curTup->n2Vrt] = Prob2v;          
                m_curTup->VrtSig3D [m_curTup->n2Vrt] = Sig3D;
                m_curTup->VrtSig2D [m_curTup->n2Vrt] = Sig2D;
                m_curTup->VrtDist2D[m_curTup->n2Vrt] = Dist2D;
                m_curTup->VrtM     [m_curTup->n2Vrt] = tmpVrt.Momentum.M();
                m_curTup->VrtPt    [m_curTup->n2Vrt] = tmpVrt.Momentum.Pt();
                m_curTup->VrtEta   [m_curTup->n2Vrt] = SVPV.Eta();
                m_curTup->VrtCosSPM[m_curTup->n2Vrt] = cosSVPV;
                m_curTup->VMinPtT  [m_curTup->n2Vrt] = minPtT;
                m_curTup->VMinS3DT [m_curTup->n2Vrt] = std::min(TrackSignif[i],TrackSignif[j]);
                m_curTup->VMaxS3DT [m_curTup->n2Vrt] = std::max(TrackSignif[i],TrackSignif[j]);
                m_curTup->VrtBDT   [m_curTup->n2Vrt] = 1.1;
                if(m_curTup->n2Vrt<DevTuple::maxNVrt-1)m_curTup->n2Vrt++;
             }
//-------------------BDT based rejection
             VrtVrtDist(PrimVrt, tmpVrt.FitVertex, tmpVrt.ErrorMatrix, Sig3D);
	     Dist2D=VrtVrtDist2D(PrimVrt, tmpVrt.FitVertex, tmpVrt.ErrorMatrix, Sig2D);
             std::vector<float> VARS(11);
	     VARS[0]=Prob2v;
	     VARS[1]=tmpVrt.Momentum.Pt();
	     VARS[2]=std::max(minPtT,m_cutPt);
	     VARS[3]=Dist2D;
	     VARS[4]=std::max(Sig3D,m_selVrtSigCut);
	     VARS[5]=Sig2D;
	     VARS[6]=std::max(std::min(TrackSignif[i],TrackSignif[j]),m_trkSigCut);
	     VARS[7]=std::max(TrackSignif[i],TrackSignif[j]);
	     VARS[8]=tmpVrt.Momentum.M();
	     VARS[9]=cosSVPV;
	     VARS[10]=SVPV.Eta();
             float wgtSelect=1.1;
             wgtSelect=m_SV2T_BDT->GetGradBoostMVA(VARS);
	     if( m_fillHist && m_curTup ) m_curTup->VrtBDT[m_curTup->n2Vrt-1] = wgtSelect;
	     if(wgtSelect<m_v2tIniBDTCut) continue;

//
//---  Save good candidate for multi-vertex fit
//
             add_edge(i,j,*m_compatibilityGraph);
         }
      } 

      return;
   }




   bool NewVrtSecInclusiveTool::Check2TrVertexInPixel( const xAOD::TrackParticle* p1, const xAOD::TrackParticle* p2,
                                              Amg::Vector3D &FitVertex, std::vector<double> & VrtErr)
   const
   {
	int blTrk[2]={0,0};
	int blP[2]={0,0};
	int l1Trk[2]={0,0};
	int l1P[2]={0,0};
	int l2Trk[2]={0,0};
	int nLays[2]={0,0};
        getPixelLayers( p1, blTrk[0] , l1Trk[0], l2Trk[0], nLays[0] );
        getPixelLayers( p2, blTrk[1] , l1Trk[1], l2Trk[1], nLays[1] );    // Very close to PV. Both b-layer hits are mandatory.
        getPixelProblems(p1, blP[0], l1P[0] );
        getPixelProblems(p2, blP[1], l1P[1] );
        double xDif=FitVertex.x()-m_xLayerB, yDif=FitVertex.y()-m_yLayerB ; 
        double Dist2DBL=sqrt(xDif*xDif+yDif*yDif);
        if      (Dist2DBL < m_rLayerB-VrtRadiusError(FitVertex, VrtErr)) {       //----------------------------------------- Inside B-layer
          if( blTrk[0]==0 && blTrk[1]==0) return false;  // No b-layer hits at all, but all expected
	  if( blTrk[0]<1  && l1Trk[0]<1 ) return false;
	  if( blTrk[1]<1  && l1Trk[1]<1 ) return false;
          if(  nLays[0]           <2 )    return false;  // Less than 2 layers on track 0
          if(  nLays[1]           <2 )    return false;  // Less than 2 layers on track 1
	  return true;
        }else if(Dist2DBL > m_rLayerB+VrtRadiusError(FitVertex, VrtErr)){      //----------------------------------------- Outside b-layer
          if( blTrk[0]>0 && blP[0]==0 && blTrk[1]>0 && blP[1]==0 ) return false;  // Good hit in b-layer is present
        }
// 
// L1 and L2 are considered only if vertex is in acceptance
//
	if(fabs(FitVertex.z())<400.){
          xDif=FitVertex.x()-m_xLayer1, yDif=FitVertex.y()-m_yLayer1 ;
	  double Dist2DL1=sqrt(xDif*xDif+yDif*yDif);
          xDif=FitVertex.x()-m_xLayer2, yDif=FitVertex.y()-m_yLayer2 ; 
	  double Dist2DL2=sqrt(xDif*xDif+yDif*yDif);
          if      (Dist2DL1 < m_rLayer1-VrtRadiusError(FitVertex, VrtErr)) {   //------------------------------------------ Inside 1st-layer
	     if( l1Trk[0]==0 && l1Trk[1]==0 )     return false;  // No L1 hits at all
             if( l1Trk[0]<1  && l2Trk[0]<1  )     return false;  // Less than 1 hits on track 0
             if( l1Trk[1]<1  && l2Trk[1]<1  )     return false;  // Less than 1 hits on track 1
             return true;
          }else if(Dist2DL1 > m_rLayer1+VrtRadiusError(FitVertex, VrtErr)) {  //------------------------------------------- Outside 1st-layer
	     if( l1Trk[0]>0 && l1P[0]==0 && l1Trk[1]>0 && l1P[1]==0 )       return false;  //  Good L1 hit is present
          }
          
          if      (Dist2DL2 < m_rLayer2-VrtRadiusError(FitVertex, VrtErr)) {  //------------------------------------------- Inside 2nd-layer
	     if( (l2Trk[0]+l2Trk[1])==0 )  return false;           // At least one L2 hit must be present
          }else if(Dist2DL2 > m_rLayer2+VrtRadiusError(FitVertex, VrtErr)) {  
	  //   if( (l2Trk[0]+l2Trk[1])>0  )  return false;           // L2 hits are present
	  }     
        } else {
	  int d0Trk[2]={0,0}; 
	  int d1Trk[2]={0,0}; 
	  int d2Trk[2]={0,0}; 
          getPixelDiscs( p1, d0Trk[0] , d1Trk[0], d2Trk[0] );
          getPixelDiscs( p2, d0Trk[1] , d1Trk[1], d2Trk[1] );
          if( d0Trk[0]+d1Trk[0]+d2Trk[0] ==0 )return false;
          if( d0Trk[1]+d1Trk[1]+d2Trk[1] ==0 )return false;
        }
        return true;
   }

}  //end of namespace
