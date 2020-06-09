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
	     float vrtR=tmpVrt.FitVertex.perp();
//Check close material layer
             double dstMatSignif=1.e4;
	     if(m_removeTrkMatSignif>0. && vrtR>20.){
		if(vrtR<30.){ dstMatSignif=fabs(vrtR-m_beampipeR)/VrtRadiusError(tmpVrt.FitVertex,tmpVrt.ErrorMatrix );} //beampipe
		else        { dstMatSignif=distToMatLayerSignificance(tmpVrt);}     //Material in Pixel volume
		if(dstMatSignif<m_removeTrkMatSignif)continue;
	     }
//
// Check pixel hits vs vertex positions.
             int ihitIBL  = getIBLHit(SelectedTracks[i]);
	     int jhitIBL  = getIBLHit(SelectedTracks[j]);
             if( ihitIBL<0 && jhitIBL<0)continue;
             float ihitR  = SelectedTracks[i]->radiusOfFirstHit();
	     float jhitR  = SelectedTracks[j]->radiusOfFirstHit();
             if(m_useVertexCleaning){
               if(fabs(ihitR-jhitR)>15.) continue;
               if( std::min(ihitR,jhitR)-vrtR > 36.) continue; // Too big dR between vertex and hit in pixel
							       // Should be another layer in between 
               if( std::max(ihitR,jhitR)-vrtR <-10.) continue; // Vertex is behind hit in pixel 
             }
//
// Debugging and BDT
	     double minPtT = std::min(TracksForFit[0]->pt(),TracksForFit[1]->pt());
	     double Sig3D=0.,Sig2D=0., Dist2D=0.; 
             if( m_fillHist && m_curTup ){
                int ihitBL   = getBLHit (SelectedTracks[i]);
	        int jhitBL   = getBLHit (SelectedTracks[j]);
	        int idisk1=0,idisk2=0,idisk3=0,jdisk1=0,jdisk2=0,jdisk3=0;
                int sumIBLHits =  std::max(ihitIBL,0)+std::max(jhitIBL,0);
                int sumBLHits  =  std::max(ihitBL, 0)+std::max(jhitBL, 0);
                getPixelDiscs(SelectedTracks[i],idisk1,idisk2,idisk3);
                getPixelDiscs(SelectedTracks[j],jdisk1,jdisk2,jdisk3);
                VrtVrtDist(PrimVrt, tmpVrt.FitVertex, tmpVrt.ErrorMatrix, Sig3D);
                Dist2D=VrtVrtDist2D(PrimVrt, tmpVrt.FitVertex, tmpVrt.ErrorMatrix, Sig2D);
                m_hb_signif3D->Fill(Sig3D,1.);
                m_curTup->VrtTrkHF [m_curTup->n2Vrt] = getIdHF(TracksForFit[0])+ getIdHF(TracksForFit[1]);       
                m_curTup->VrtTrkI  [m_curTup->n2Vrt] = getG4Inter(TracksForFit[0])+ getG4Inter(TracksForFit[1]);       
                m_curTup->VrtCh    [m_curTup->n2Vrt] = Charge;
                m_curTup->VrtProb  [m_curTup->n2Vrt] = Prob2v;          
                m_curTup->VrtSig3D [m_curTup->n2Vrt] = Sig3D;
                m_curTup->VrtSig2D [m_curTup->n2Vrt] = Sig2D;
                m_curTup->VrtDist2D[m_curTup->n2Vrt] = vrtR<20. ? Dist2D : vrtR;
                m_curTup->VrtM     [m_curTup->n2Vrt] = tmpVrt.Momentum.M();
                m_curTup->VrtZ     [m_curTup->n2Vrt] = tmpVrt.FitVertex.z();
                m_curTup->VrtPt    [m_curTup->n2Vrt] = tmpVrt.Momentum.Pt();
                m_curTup->VrtEta   [m_curTup->n2Vrt] = SVPV.Eta();
                m_curTup->VrtIBL   [m_curTup->n2Vrt] = sumIBLHits;
                m_curTup->VrtBL    [m_curTup->n2Vrt] = sumBLHits;
                m_curTup->VrtSinSPM[m_curTup->n2Vrt] = sqrt(1.-cosSVPV*cosSVPV);
                m_curTup->VMinPtT  [m_curTup->n2Vrt] = minPtT;
                m_curTup->VMinS3DT [m_curTup->n2Vrt] = std::min(TrackSignif[i],TrackSignif[j]);
                m_curTup->VMaxS3DT [m_curTup->n2Vrt] = std::max(TrackSignif[i],TrackSignif[j]);
                m_curTup->VrtBDT   [m_curTup->n2Vrt] = 1.1;
                m_curTup->VrtHR1   [m_curTup->n2Vrt] = ihitR;
                m_curTup->VrtHR2   [m_curTup->n2Vrt] = jhitR;
                m_curTup->VrtDisk  [m_curTup->n2Vrt] = idisk1+10*idisk2+20*idisk3+30*jdisk1+40*jdisk2+50*jdisk3;
                m_curTup->VSigMat  [m_curTup->n2Vrt] = dstMatSignif;
                if(m_curTup->n2Vrt<DevTuple::maxNVrt-1)m_curTup->n2Vrt++;
             }
//-------------------BDT based rejection
	     if(tmpVrt.Momentum.Pt() > m_Vrt2TrPtLimit) continue;
             VrtVrtDist(PrimVrt, tmpVrt.FitVertex, tmpVrt.ErrorMatrix, Sig3D);
	     Dist2D=VrtVrtDist2D(PrimVrt, tmpVrt.FitVertex, tmpVrt.ErrorMatrix, Sig2D);
             std::vector<float> VARS(10);
	     VARS[0]=Prob2v;
	     VARS[1]=log(tmpVrt.Momentum.Pt());
	     VARS[2]=log(std::max(minPtT,m_cutPt));
	     VARS[3]=log(vrtR<20. ? Dist2D : vrtR);
	     VARS[4]=log(std::max(std::min(TrackSignif[i],TrackSignif[j]),m_trkSigCut));
	     VARS[5]=log(std::max(TrackSignif[i],TrackSignif[j]));
	     VARS[6]=tmpVrt.Momentum.M();
	     VARS[7]=sqrt(fabs(1.-cosSVPV*cosSVPV));
	     VARS[8]=SVPV.Eta();
	     VARS[9]=std::max(ihitR,jhitR);
	     //VARS[9]=sumIBLHits;
	     //VARS[10]=sumBLHits;
	     //VARS[11]=std::max(Sig3D,m_selVrtSigCut);
             float wgtSelect=m_SV2T_BDT->GetGradBoostMVA(VARS);
             //std::vector<float> weights=m_SV2T_BDT->GetMultiResponse(VARS,3);
	     //float wgtSelect=weights[0];
	     if( m_fillHist && m_curTup ) m_curTup->VrtBDT[m_curTup->n2Vrt-1] = wgtSelect;
	     if(wgtSelect<m_v2tIniBDTCut) continue;
             //if(weights[2]>weights[0] && weights[2]>weights[1])continue;
//
//---  Save good candidate for multi-vertex fit
//
             add_edge(i,j,*m_compatibilityGraph);
         }
      } 

      return;
   }


}  //end of namespace
