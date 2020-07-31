/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "InDetVKalVxInJetTool/InDetVKalVxInJetTool.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
//-------------------------------------------------
// Other stuff
#include  "AnalysisUtils/AnalysisMisc.h"
#include  "TrkToolInterfaces/ITrackSummaryTool.h"
#include  "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

#include "TProfile.h"
#include "TH2D.h"
#include  "TMath.h"
//
//#include<iostream>

//----------------------------------------------------------------------------------------
//  GetVrtSec resurns the vector Results with the following
//   1) Vertex mass
//   2) Vertex/jet energy fraction
//   3) Number of initially selected 2tr vertices
//   4) Number of selected for vertexing tracks in jet 
//   5) Number of track in secondary vertex
//   6) 3D SV-PV significance with sign
//   7) Jet energy used in (2) calculation 
//   8) Minimal distance between vertex and any material layer
//   9) Transverse vertex/jet energy fraction. Jet pT independent.
//   10) "Product" variable
//   11) "Boost" variable
//---------------------------------------------------------------------------------------- 
namespace InDet{

  float median(std::vector<float> &Vec){
    int N=Vec.size();
    if(N==1) return Vec[0];
    if(N==2) return (Vec[0]+Vec[1])/2.;
    if(N==3) return Vec[1];
    if(N>3){
      std::vector<float> tmp(Vec);
      int N05m=(N-1)/2, N05p=N/2;
      //std::sort(tmp.begin(),tmp.end());  //can use nth_element instead of completely sorting, it's quicker
      if(N05m==N05p){ 
         std::nth_element(tmp.begin(),tmp.begin()+N05m,tmp.end());
         return tmp[N05m];
      } else { 
         std::nth_element(tmp.begin(),tmp.begin()+N05m,tmp.end());
         std::nth_element(tmp.begin(),tmp.begin()+N05p,tmp.end());
         return (tmp[N05m]+tmp[N05p])/2.;
      }
    }
    return 0.;
  }

  const double c_vrtBCMassLimit=5500.;  // Mass limit to consider a vertex not coming from B,C-decays






  xAOD::Vertex* InDetVKalVxInJetTool::GetVrtSec(const std::vector<const xAOD::TrackParticle*>& InpTrk,
                                                const xAOD::Vertex                           & PrimVrt,
                                                const TLorentzVector                         & JetDir,
	                                        std::vector<double>                          & Results,
                                                std::vector<const xAOD::TrackParticle*>      & ListSecondTracks,
				                std::vector<const xAOD::TrackParticle*>      & TrkFromV0) 
  const
  {

      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "GetVrtSec() called with xAOD::TrackParticle=" <<InpTrk.size()<< endmsg;

      std::vector<double> ErrorMatrix,Impact,ImpactError;
      Amg::Vector3D          FitVertex;
      std::vector< std::vector<double> > TrkAtVrt; 
      TLorentzVector    Momentum;
      double Signif3D=0., Chi2=0.;

      std::vector<const xAOD::TrackParticle*> SelectedTracks(0);
      Results.clear();        
      ListSecondTracks.clear();

      m_NRefPVTrk=0;
      if( InpTrk.size() < 2 ) { return 0;} // 0,1 track => nothing to do!
      SelGoodTrkParticle( InpTrk, PrimVrt, JetDir, SelectedTracks);
      if(m_fillHist){m_hb_ntrkjet->Fill( (double)SelectedTracks.size(), m_w_1);
                     m_pr_NSelTrkMean->Fill(JetDir.Pt(),(double)SelectedTracks.size()); }
      long int NTracks = (int) (SelectedTracks.size());
      if( NTracks < 2 ) { return 0;} // 0,1 selected track => nothing to do!

      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Number of selected tracks inside jet= " <<NTracks << endmsg;
      
      TLorentzVector MomentumJet = TotalMom(SelectedTracks);
      if(m_fillHist){m_hb_jmom->Fill( MomentumJet.E(), m_w_1); }


//--------------------------------------------------------------------------------------------	 
//                    Initial xAOD::TrackParticle list ready
      float Vrt2TrackNumber =0;

      std::vector<const xAOD::TrackParticle*>  TracksForFit;
      //std::vector<double> InpMass; for(int i=0; i<NTracks; i++){InpMass.push_back(m_massPi);}
      std::vector<double> InpMass(NTracks,m_massPi);
      Select2TrVrt(SelectedTracks, TracksForFit, PrimVrt, JetDir, InpMass, TrkFromV0,
                     ListSecondTracks);
      m_WorkArray->m_Incomp.clear();  // Not needed for single vertex version
//
//--- Cleaning
// 
      if( TrkFromV0.size() > 1) {
        RemoveDoubleEntries(TrkFromV0);
        AnalysisUtils::Sort::pT (&TrkFromV0); 
      }
//---
      Vrt2TrackNumber = (double) ListSecondTracks.size()/2.;
      std::vector<const xAOD::TrackParticle*> saveSecondTracks(ListSecondTracks);
      RemoveDoubleEntries(ListSecondTracks);
      AnalysisUtils::Sort::pT (&ListSecondTracks);
      for(auto iv0 : TrkFromV0){ auto itf=std::find(SelectedTracks.begin(),SelectedTracks.end(),iv0);
                                 if(itf!=SelectedTracks.end())  SelectedTracks.erase(itf);}
//---
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" Found different xAOD tracks in pairs="<< ListSecondTracks.size()<<endmsg;
      if(ListSecondTracks.size() < 2 ) return 0;

//--Ranking of selected tracks
      std::vector<float> trkRank(0);
      for(auto tk : ListSecondTracks) trkRank.push_back( m_trackClassificator->trkTypeWgts(tk, PrimVrt, JetDir)[0] );
      while( median(trkRank)<0.3 && trkRank.size()>3 ) {
        int Smallest= std::min_element(trkRank.begin(),trkRank.end()) - trkRank.begin();
        RemoveEntryInList(ListSecondTracks,trkRank,Smallest);
      }
//
//-----------------------------------------------------------------------------------------------------
//            Secondary track list is ready
//            Now common vertex fit
//
      double Signif3DP=0, Signif3DS=0;

      Chi2 =  FitCommonVrt( ListSecondTracks, trkRank, PrimVrt, JetDir, InpMass, FitVertex, ErrorMatrix, Momentum, TrkAtVrt);
      if( Chi2 < 0 && ListSecondTracks.size()>2 ) { // Vertex not reconstructed. Try to remove one track with biggest pt.
        double tpmax=0; int ipmax=-1;
        for(int it=0; it<(int)ListSecondTracks.size(); it++) if(tpmax<ListSecondTracks[it]->pt()){tpmax=ListSecondTracks[it]->pt(); ipmax=it;}
        if(ipmax>=0)RemoveEntryInList(ListSecondTracks,trkRank,ipmax);
        Chi2 =  FitCommonVrt( ListSecondTracks, trkRank, PrimVrt, JetDir, InpMass, FitVertex, ErrorMatrix, Momentum, TrkAtVrt);
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" Second FitCommonVrt try="<< Chi2<<" Ntrk="<<ListSecondTracks.size()<<endmsg;
      }
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" FitCommonVrt result="<< Chi2<<" Ntrk="<<ListSecondTracks.size()<<endmsg;
//
      if( Chi2 < 0) return 0;
//
// Check jet tracks not in secondary vertex
      std::map<double,const xAOD::TrackParticle*> AdditionalTracks;
      VrtVrtDist(PrimVrt, FitVertex, ErrorMatrix, Signif3D);
      if(Signif3D>8.){
	int hitL1=0, nLays=0, hitIBL=0, hitBL=0; 
        for (auto i_ntrk : SelectedTracks) {
          if(  find( ListSecondTracks.begin(), ListSecondTracks.end(), i_ntrk) != ListSecondTracks.end() ) continue; // Track is used already
          std::vector<float> trkScore=m_trackClassificator->trkTypeWgts(i_ntrk, PrimVrt, JetDir);
	  if( trkScore[0] < m_cutHFClass/2.) continue;
          Signif3DS = m_fitSvc->VKalGetImpact(i_ntrk, FitVertex         , 1, Impact, ImpactError);
          if( Signif3DS > 10.) continue;
          getPixelLayers(i_ntrk , hitIBL , hitBL, hitL1, nLays );
          if( hitIBL<=0 && hitBL<=0 ) continue;                  // No IBL and BL pixel hits => non-precise track
          Signif3DP = m_fitSvc->VKalGetImpact(i_ntrk, PrimVrt.position(), 1, Impact, ImpactError);
          if(Signif3DP<1.)continue;
          if(m_fillHist){ m_hb_diffPS->Fill( Signif3DP-Signif3DS, m_w_1); }
	  if(Signif3DP-Signif3DS>4.0) AdditionalTracks[Signif3DP-Signif3DS]=i_ntrk;
        }
      }
//
// Add found tracks and refit
//
      if( AdditionalTracks.size() > 0){
        while (AdditionalTracks.size()>3) AdditionalTracks.erase(AdditionalTracks.begin());//Tracks are in increasing DIFF order.
        for (auto atrk : AdditionalTracks) ListSecondTracks.push_back(atrk.second);        //3tracks with max DIFF are selected
        trkRank.clear();
        for(auto tk : ListSecondTracks) trkRank.push_back( m_trackClassificator->trkTypeWgts(tk, PrimVrt, JetDir)[0] );
        Chi2 =  FitCommonVrt( ListSecondTracks, trkRank, PrimVrt, JetDir, InpMass, FitVertex, ErrorMatrix, Momentum, TrkAtVrt);
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" Added track FitCommonVrt output="<< Chi2<<endmsg;
        if( Chi2 < 0) return 0;
      }
//
//  Saving of results
//
//
      if( ListSecondTracks.size()==2 ){         // If there are 2 only tracks
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" Start Ntr=2 vertex check"<<endmsg;
        int Charge=0;
	uint8_t BLshared=0;
	uint8_t PXshared=0;
        for (auto i_ntrk : ListSecondTracks) {
            Charge +=  (int) i_ntrk->charge();
            uint8_t retval=0;
            if( i_ntrk->summaryValue( retval, xAOD::numberOfPixelSharedHits)  )  PXshared  += retval;
            if( i_ntrk->summaryValue( retval, xAOD::numberOfInnermostPixelLayerSharedHits) )  BLshared  += retval;
        }
	VrtVrtDist(PrimVrt, FitVertex, ErrorMatrix, Signif3D);
        if(m_useVertexCleaning){
          if(!Check2TrVertexInPixel(ListSecondTracks[0],ListSecondTracks[1],FitVertex,ErrorMatrix)) return 0;
          if(m_fillHist){
            double xDif=FitVertex.x()-m_xLayerB, yDif=FitVertex.y()-m_yLayerB ; 
            double Dist2D=sqrt(xDif*xDif+yDif*yDif);
            if     (Dist2D < m_rLayerB-VrtRadiusError(FitVertex,ErrorMatrix))  m_hb_blshared->Fill((float)BLshared,m_w_1);
            else if(Dist2D > m_rLayerB+VrtRadiusError(FitVertex,ErrorMatrix))  m_hb_pxshared->Fill((float)PXshared,m_w_1);
         }
        } //end 2tr vertex cleaning code
//
        if(m_fillHist){ if(Charge){m_hb_totmass2T1->Fill(Momentum.M(),m_w_1);}else{m_hb_totmass2T0->Fill(Momentum.M(),m_w_1);} }
        if( !Charge && fabs(Momentum.M()-m_massK0)<15. ) {       // Final rejection of K0
	  TrkFromV0.push_back(ListSecondTracks[0]);
	  TrkFromV0.push_back(ListSecondTracks[1]);
          if( TrkFromV0.size() > 1) {
             RemoveDoubleEntries(TrkFromV0);
             AnalysisUtils::Sort::pT (&TrkFromV0);
          }
          return 0;
        }
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" Ntr=2 vertex check passed"<<endmsg;
      }

	    
      double JetVrtDir = ProjSV_PV(FitVertex,PrimVrt,JetDir);
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Combined SV neg.dir="<<JetVrtDir<<endmsg;
      if(  m_getNegativeTag )
         { if( JetVrtDir>0. )   return 0; }
      else if( m_getNegativeTail )
         { ; }
      else 
         { if( JetVrtDir<0. ) return 0; } 

      double xvt=FitVertex.x(); double yvt=FitVertex.y();
      double Dist2DBP=sqrt( (xvt-m_beampipeX)*(xvt-m_beampipeX) + (yvt-m_beampipeY)*(yvt-m_beampipeY) ); 
      double Dist2DBL=sqrt( (xvt-m_xLayerB)*(xvt-m_xLayerB) + (yvt-m_yLayerB)*(yvt-m_yLayerB) ); 
      double Dist2DL1=sqrt( (xvt-m_xLayer1)*(xvt-m_xLayer1) + (yvt-m_yLayer1)*(yvt-m_yLayer1) );
      double Dist2DL2=sqrt( (xvt-m_xLayer2)*(xvt-m_xLayer2) + (yvt-m_yLayer2)*(yvt-m_yLayer2) );
      double minDstMat=39.9;
      minDstMat=TMath::Min(minDstMat,fabs(Dist2DBP-m_beampipeR));
      minDstMat=TMath::Min(minDstMat,fabs(Dist2DBL-m_rLayerB));
      minDstMat=TMath::Min(minDstMat,fabs(Dist2DL1-m_rLayer1));
      minDstMat=TMath::Min(minDstMat,fabs(Dist2DL2-m_rLayer2));
      if(m_existIBL) minDstMat=TMath::Min(minDstMat,fabs(Dist2DL2-m_rLayer3));  // 4-layer pixel detector
 
 
      VrtVrtDist(PrimVrt, FitVertex, ErrorMatrix, Signif3D);
      if(JetVrtDir < 0) Signif3D = -Signif3D;


      Amg::Vector3D DirForPt( FitVertex.x()-PrimVrt.x(),
                              FitVertex.y()-PrimVrt.y(),
		              FitVertex.z()-PrimVrt.z());
      Results.push_back(Momentum.M());                             //1st
      double eRatio = Momentum.E()/MomentumJet.E();
      Results.push_back(  eRatio<0.99999 ? eRatio : 0.99999);      //2nd
      Results.push_back(Vrt2TrackNumber);                          //3rd
      Results.push_back((double)NTracks);                          //4th
      Results.push_back((double)ListSecondTracks.size());          //5th
      Results.push_back(Signif3D);                                 //6th
      Results.push_back(MomentumJet.E());                          //7th
      Results.push_back(minDstMat);                                //8th
      double nRatio = Momentum.Et(JetDir.Vect())/sqrt(MomentumJet.Perp());   nRatio /= (nRatio+4.); 
      Results.push_back( nRatio );                                 //9th   New transverse energy ration
      Results.push_back((Momentum.M()-2.*m_massPi)*eRatio/m_massB);           //10th   "Product" variable
      Results.push_back((Momentum.Pt()/Momentum.M())*(m_massB/JetDir.Pt()) ); //11th   "Boost" variable

      if(m_fillHist){
          // Find highest track Pt with respect to jet direction
          double trackPt, trackPtMax=0.;
          for (int tr=0; tr<(int)ListSecondTracks.size(); tr++) {
	    if(ListSecondTracks[tr]->pt()/JetDir.Pt()>0.5)continue;
            trackPt=pTvsDir(Amg::Vector3D(JetDir.X(),JetDir.Y(),JetDir.Z()) , TrkAtVrt[tr]);
	    if(trackPt>trackPtMax)trackPtMax=trackPt;
          }
	  m_hb_rNdc->Fill( FitVertex.perp(), m_w_1);
          m_hb_trkPtMax->Fill( trackPtMax, m_w_1);
          m_pr_effVrt->Fill((float)m_NRefPVTrk,1.);              
	  m_pr_effVrtEta->Fill( JetDir.Eta(),1.);
          m_hb_mom->Fill( MomentumJet.E(), m_w_1);
          m_hb_ratio->Fill( Results[1], m_w_1); 
          m_hb_totmass->Fill( Results[0], m_w_1); 
          m_hb_nvrt2->Fill( Results[2], m_w_1);
          m_hb_sig3DTot->Fill( Signif3D, m_w_1);
          m_hb_dstToMat->Fill( minDstMat, m_w_1);
          float R=JetDir.DeltaR(TLorentzVector(FitVertex.x()-PrimVrt.x(),FitVertex.y()-PrimVrt.y(),
	                                       FitVertex.z()-PrimVrt.z(), 1.e4));
          m_hb_deltaRSVPV->Fill( R, m_w_1);
          if(m_curTup)m_curTup->TotM=Momentum.M();
       }

//-------------------------------------------------------------------------------------
//Return xAOD::Vertex
       xAOD::Vertex * tmpVertex=new xAOD::Vertex();
       tmpVertex->makePrivateStore();
       tmpVertex->setPosition(FitVertex);
       std::vector<float> floatErrMtx; floatErrMtx.resize(ErrorMatrix.size());
       for(int i=0; i<(int)ErrorMatrix.size(); i++) floatErrMtx[i]=ErrorMatrix[i];
       tmpVertex->setCovariance(floatErrMtx);
       tmpVertex->setFitQuality(Chi2, (float)(ListSecondTracks.size()*2.-3.));

       std::vector<Trk::VxTrackAtVertex> & tmpVTAV=tmpVertex->vxTrackAtVertex();    tmpVTAV.clear();
       for(int ii=0; ii<(int)ListSecondTracks.size(); ii++) {
         AmgSymMatrix(5) *CovMtxP=new AmgSymMatrix(5);   (*CovMtxP).setIdentity(); 
         Trk::Perigee * tmpMeasPer  =  new Trk::Perigee( 0.,0., TrkAtVrt[ii][0], TrkAtVrt[ii][1], TrkAtVrt[ii][2],
                                                                Trk::PerigeeSurface(FitVertex), CovMtxP );
         tmpVTAV.push_back( Trk::VxTrackAtVertex( 1., tmpMeasPer) );
         ElementLink<xAOD::TrackParticleContainer> TEL;  TEL.setElement( ListSecondTracks[ii] );
         const xAOD::TrackParticleContainer* cont = (const xAOD::TrackParticleContainer* ) (ListSecondTracks[ii]->container() );
	 TEL.setStorableObject(*cont);
         tmpVertex->addTrackAtVertex(TEL,1.);
       }
       return tmpVertex;


  }




//
//--------------------------------------------------------
//   Template routine for global secondary vertex fitting
//

  template <class Track>
  double InDetVKalVxInJetTool::FitCommonVrt(std::vector<const Track*>& ListSecondTracks,
 				  std::vector<float>        & trkRank,
                                  const xAOD::Vertex        & PrimVrt,
 	                          const TLorentzVector      & JetDir,
                                  std::vector<double>       & InpMass, 
	                          Amg::Vector3D             & FitVertex,
                                  std::vector<double>       & ErrorMatrix,
	                          TLorentzVector            & Momentum,
		        std::vector< std::vector<double> >  & TrkAtVrt)
  const
 {
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "FitCommonVrt() called " <<ListSecondTracks.size()<< endmsg;
//preparation
      StatusCode sc;
      std::vector<double> Chi2PerTrk;
      long int           Charge;
      double             Chi2 = 0.;
      Amg::Vector3D      tmpVertex;

      int Outlier=1, i=0;
//
// Start of fit
//
      std::unique_ptr<Trk::IVKalState> state = m_fitSvc->makeState();
      m_fitSvc->setMassInputParticles( InpMass, *state );            // Use pions masses
      sc=VKalVrtFitFastBase(ListSecondTracks,FitVertex,*state);          /* Fast crude estimation */
      if(sc.isFailure() || FitVertex.perp() > m_rLayer2*2. ) {    /* No initial estimation */ 
         m_fitSvc->setApproximateVertex(PrimVrt.x(), PrimVrt.y(), PrimVrt.z(),*state); /* Use as starting point */
      } else {
         m_fitSvc->setApproximateVertex(FitVertex.x(),FitVertex.y(),FitVertex.z(),*state); /*Use as starting point*/
      }
//fit itself
      int NTracksVrt = ListSecondTracks.size(); double FitProb=0.;
      std::vector<double> trkFitWgt(0);
      for (i=0; i < NTracksVrt-1; i++) {
         if(m_RobustFit)m_fitSvc->setRobustness(m_RobustFit, *state);
         else m_fitSvc->setRobustness(0, *state);
         sc=VKalVrtFitBase(ListSecondTracks,FitVertex,Momentum,Charge,
                           ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2,
                           *state, true);
         if(sc.isFailure() ||  Chi2 > 1000000. ) { return -10000.;}    // No fit
         sc=GetTrkFitWeights(trkFitWgt, *state);
         if(sc.isFailure()){ return -10000.;}    // No weights
	 Outlier=std::min_element(trkFitWgt.begin(),trkFitWgt.end())-trkFitWgt.begin();
         //////Outlier = FindMax( Chi2PerTrk, trkRank ); 
	 FitProb=TMath::Prob( Chi2, 2*ListSecondTracks.size()-3);
	 if(ListSecondTracks.size() == 2 )              break;         // Only 2 tracks left
//////////////////////////////
         double Signif3Dproj=VrtVrtDist( PrimVrt, FitVertex, ErrorMatrix, JetDir);
         if(Signif3Dproj<0 && (!m_getNegativeTail) && (!m_getNegativeTag)){
	   double maxDst=-1.e12; int maxT=-1; double minChi2=1.e12;
	   for(int it=0; it<(int)ListSecondTracks.size(); it++){
              std::vector<const Track*> tmpList(ListSecondTracks);
              tmpList.erase(tmpList.begin()+it);
              sc=VKalVrtFitBase(tmpList,tmpVertex,Momentum,Charge,ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2,*state,true);
              if(sc.isFailure())continue;
              Signif3Dproj=VrtVrtDist( PrimVrt, tmpVertex, ErrorMatrix, JetDir);
	      if(Signif3Dproj>maxDst  && maxDst<10. ){maxDst=Signif3Dproj; maxT=it; minChi2=Chi2;}
	      else if(Signif3Dproj>0. && maxDst>10. && Chi2<minChi2) {minChi2=Chi2; maxT=it;}
	   }
	   if(maxT>=0){ Outlier=maxT;   RemoveEntryInList(ListSecondTracks,trkRank,Outlier);
                        m_fitSvc->setApproximateVertex(FitVertex.x(),FitVertex.y(),FitVertex.z(),*state);
                        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" Remove negative outlier="<< maxT<<" from "
                            <<ListSecondTracks.size()+1<<" tracks"<<endmsg;
			continue;}
	 }
////////////////////////////////////////
//	 if( Momentum.m() < c_vrtBCMassLimit) {
//	   if( Chi2PerTrk[Outlier] < m_secTrkChi2Cut && FitProb > 0.001)  break;  // Solution found
//	 }
	 if( FitProb > 0.001) {
	   if( Momentum.M() <c_vrtBCMassLimit) {
	     if( Chi2PerTrk[Outlier] < m_secTrkChi2Cut*m_chiScale[ListSecondTracks.size()<10?ListSecondTracks.size():10])  break;  // Solution found
           } else {
	     double minM=1.e12; int minT=-1; double minChi2=1.e12;
	     for(int it=0; it<(int)ListSecondTracks.size(); it++){
                std::vector<const Track*> tmpList(ListSecondTracks);
                tmpList.erase(tmpList.begin()+it);
                sc=VKalVrtFitBase(tmpList,tmpVertex,Momentum,Charge,ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2,*state,true);
                if(sc.isFailure())continue;
		if(ProjSV_PV(tmpVertex,PrimVrt,JetDir)<0.)continue; // Drop negative direction 
	        Chi2 += trkRank[it];                                // Remove preferably non-HF-tracks
		if(Momentum.M()<minM  && minM>c_vrtBCMassLimit){minM=Momentum.M(); minT=it; minChi2=Chi2;}
		else if(Momentum.M()<c_vrtBCMassLimit && minM<c_vrtBCMassLimit && Chi2<minChi2){minChi2=Chi2; minT=it;}
	     }
             if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" Big mass. Remove trk="<<minT<<" New mass="<<minM<<" New Chi2="<<minChi2<<endmsg;
	     if(minT>=0)Outlier=minT;
	   }
	 }
         if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"SecVrt remove trk="<<Outlier<<" from "<< ListSecondTracks.size()<<" tracks"<<endmsg;
         RemoveEntryInList(ListSecondTracks,trkRank,Outlier);
         m_fitSvc->setApproximateVertex(FitVertex.x(),FitVertex.y(),FitVertex.z(),*state); /*Use as starting point*/
      }
//--
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" SecVrt fit converged. Ntr="<< ListSecondTracks.size()<<" Chi2="<<Chi2
         <<" Chi2_trk="<<Chi2PerTrk[Outlier]<<" Prob="<<FitProb<<" M="<<Momentum.M()<<" Dir="<<ProjSV_PV(FitVertex,PrimVrt,JetDir)<<endmsg;
//--
      if( ListSecondTracks.size()==2 ){
	 if( Momentum.M() > c_vrtBCMassLimit || FitProb < 0.001 || Chi2PerTrk[Outlier] > m_secTrkChi2Cut) return -10000.;  
         if(std::max(trkRank[0],trkRank[1])<m_cutHFClass*2.) return -10000.;
      } 
//
//-- To kill remnants of conversion
      double Dist2D=sqrt(FitVertex.x()*FitVertex.x()+FitVertex.y()*FitVertex.y());
      if( ListSecondTracks.size()==2  && (Dist2D > 20.) && Charge==0 ) {
        double mass_EE   =  massV0( TrkAtVrt,m_massE,m_massE);
        if(m_fillHist){m_hb_totmassEE->Fill( mass_EE, m_w_1); }
        if( mass_EE < 40. ) return -40.;
      }
//-- Test creation of Trk::Track
//      if( ListSecondTracks.size()==2 && Charge==0 ) {      
//        std::vector<double> VKPerigee,CovPerigee;
//        sc=m_fitSvc->VKalVrtCvtTool(FitVertex,Momentum,ErrorMatrix,0,VKPerigee,CovPerigee,*state);
//        if(sc.isSuccess())const Trk::Track* TT = m_fitSvc->CreateTrkTrack(VKPerigee,CovPerigee,*state);
//      }
//      if( ListSecondTracks.size()==2  && (Dist2D > 20.) ) {    // Protection against fake vertices    
//         std::vector<double> Impact,ImpactError;double ImpactSignif;
//         ImpactSignif = m_fitSvc->VKalGetImpact(ListSecondTracks[0], PrimVrt.position(), 1, Impact, ImpactError);
//         if( ImpactSignif<4. && ImpactSignif>-3. ) return -41.;
//         ImpactSignif = m_fitSvc->VKalGetImpact(ListSecondTracks[1], PrimVrt.position(), 1, Impact, ImpactError);
//         if( ImpactSignif<4. && ImpactSignif>-3. ) return -41.;
//      }
      return Chi2;
}







//
//
//--------------------------------------------------------
//   Template routine for 2track secondary vertices selection
//

    template <class Track>
    void InDetVKalVxInJetTool::Select2TrVrt(std::vector<const Track*>  & SelectedTracks,
                                  std::vector<const Track*>            & TracksForFit,
                                  const xAOD::Vertex                   & PrimVrt,
	                          const TLorentzVector & JetDir,
                                  std::vector<double>         & InpMass, 
	                          std::vector<const Track*>   & TrkFromV0,
	                          std::vector<const Track*>   & ListSecondTracks)
    const
    {
      std::vector<double> Chi2PerTrk,VKPerigee,CovPerigee,closeVrtSig(0),closeVrtCh2(0);
      //TLorentzVector   Momentum;
      std::vector<double> Impact,ImpactError;
      double             Signif3D, Dist2D, JetVrtDir;
      long int           Charge;
      int i,j;
      StatusCode sc; sc.setChecked();
      Vrt2Tr tmpVrt;
      std::vector<Vrt2Tr> all2TrVrt(0);
      TLorentzVector TLV; 
//
      int NTracks = (int) (SelectedTracks.size());

      m_WorkArray->m_Incomp.clear();   // For multivertex
      m_WorkArray->m_Prmtrack.clear();   // For multivertex
//
//  Impact parameters with sign calculations
//
      std::vector<float> covPV=PrimVrt.covariance(); 
      double SignifR=0.,SignifZ=0.;
      std::vector<int> hitIBL(NTracks,0), hitBL(NTracks,0);
      std::vector<double> TrkSig3D(NTracks);
      std::vector< std::vector<float> > trkScore(NTracks);
      AmgVector(5) tmpPerigee; tmpPerigee.setZero();
      for (i=0; i<NTracks; i++) {
         TrkSig3D[i] = m_fitSvc->VKalGetImpact(SelectedTracks[i], PrimVrt.position(), 1, Impact, ImpactError);
         tmpPerigee = GetPerigee(SelectedTracks[i])->parameters(); 
         if( sin(tmpPerigee[2]-JetDir.Phi())*Impact[0] < 0 ){ Impact[0] = -fabs(Impact[0]);}
	                                                else{ Impact[0] =  fabs(Impact[0]);}
         if(  (tmpPerigee[3]-JetDir.Theta())*Impact[1] < 0 ){ Impact[1] = -fabs(Impact[1]);}
	                                                else{ Impact[1] =  fabs(Impact[1]);}
	 SignifR = Impact[0]/ sqrt(ImpactError[0]);
	 SignifZ = Impact[1]/ sqrt(ImpactError[2]);
	 int hL1=0, nLays=0; getPixelLayers(SelectedTracks[i] , hitIBL[i] , hitBL[i], hL1, nLays );
         //----
         trkScore[i]=m_trackClassificator->trkTypeWgts(SelectedTracks[i], PrimVrt, JetDir);
         if(m_fillHist){
	    m_hb_impactR->Fill( SignifR, m_w_1); 
            m_hb_impactZ->Fill( SignifZ, m_w_1); 
            m_hb_impactRZ->Fill(SignifR, SignifZ, m_w_1); 
	    m_hb_impact->Fill( TrkSig3D[i], m_w_1);
	    if(i<DevTuple::maxNTrk && m_curTup){
                 m_curTup->etatrk[i]=SelectedTracks[i]->eta();
                 m_curTup->p_prob[i]=RankBTrk(SelectedTracks[i]->pt(),JetDir.Pt(),0.);
                 m_curTup->s_prob[i]=RankBTrk(0.,0.,TrkSig3D[i]); 
                 m_curTup->SigR[i]=SignifR; m_curTup->SigZ[i]=SignifZ; 
                 m_curTup->d0[i]=Impact[0]; m_curTup->Z0[i]=Impact[1];
	         m_curTup->idMC[i]=getG4Inter(SelectedTracks[i]); 
                 if(getIdHF(SelectedTracks[i]))m_curTup->idMC[i]=2;
	         if(getMCPileup(SelectedTracks[i]))m_curTup->idMC[i]=3;
		 m_curTup->wgtB[i]=trkScore[i][0]; m_curTup->wgtL[i]=trkScore[i][1]; m_curTup->wgtG[i]=trkScore[i][2]; 
		 m_curTup->Sig3D[i]=TrkSig3D[i];
		 m_curTup->chg[i]=tmpPerigee[4]<0. ? 1: -1;
                 m_curTup->ibl[i]=hitIBL[i];
		 m_curTup->bl[i]=hitBL[i];
		 TLorentzVector TLV; 
                 TLV.SetPtEtaPhiE(SelectedTracks[i]->pt(),SelectedTracks[i]->eta(),SelectedTracks[i]->phi(),SelectedTracks[i]->e());
		 m_curTup->pTvsJet[i]=TLV.Perp(JetDir.Vect());
		 TLorentzVector normJ;  normJ.SetPtEtaPhiM(1.,JetDir.Eta(),JetDir.Phi(),0.);
		 m_curTup->prodTJ[i]=sqrt(TLV.Dot(normJ));
		 m_curTup->nVrtT[i]=0;
            }
	 }
      }
      if(m_fillHist){  m_curTup->ptjet=JetDir.Perp();  m_curTup->etajet=JetDir.Eta(); m_curTup->phijet=JetDir.Phi();
                       m_curTup->nTrkInJet=std::min(NTracks,DevTuple::maxNTrk); };

      ListSecondTracks.reserve(2*NTracks);                 // Reserve memory for single vertex


      Amg::Vector3D iniVrt(0.,0.,0.);
      for (i=0; i<NTracks-1; i++) {
         for (j=i+1; j<NTracks; j++) {
             if(trkScore[i][0]==0.)continue;
             if(trkScore[j][0]==0.)continue;
             if(trkScore[i][2] > m_antiGarbageCut)continue;  //---- Use classificator to remove Pileup+Interactions
             if(trkScore[j][2] > m_antiGarbageCut)continue;  //---- Use classificator to remove Pileup+Interactions
 	     if(!m_multiWithPrimary) {  // Not used for multi-vertex with primary one search
                if( std::max(trkScore[i][1],trkScore[j][1]) > m_antiFragmentCut ) continue; // Remove definite fragmentation tracks
		bool goodPair=false;
		float minWgtB=std::min(trkScore[i][0],trkScore[j][0]);
		float maxWgtB=std::max(trkScore[i][0],trkScore[j][0]);
                if( minWgtB>m_cutHFClass ) goodPair=true;
		if( maxWgtB>0.5 && minWgtB>m_cutHFClass/2. )goodPair=true;
                if( !goodPair ) continue;
	     }
	     int BadTracks = 0;                                       //Bad tracks identification 
             TracksForFit.resize(2);
             std::unique_ptr<Trk::IVKalState> state = m_fitSvc->makeState();
             m_fitSvc->setMassInputParticles( InpMass, *state );     // Use pion masses for fit
             TracksForFit[0]=SelectedTracks[i];
             TracksForFit[1]=SelectedTracks[j];
             sc=VKalVrtFitFastBase(TracksForFit,tmpVrt.FitVertex,*state);              /* Fast crude estimation*/
             if( sc.isFailure() || tmpVrt.FitVertex.perp() > m_rLayer2*2. ) {   /* No initial estimation */ 
                iniVrt=PrimVrt.position();
                if( m_multiWithPrimary ) iniVrt.setZero(); 
 	     } else {
                JetVrtDir = ProjSV_PV(tmpVrt.FitVertex,PrimVrt,JetDir);
                if( m_multiWithPrimary ) JetVrtDir=fabs(JetVrtDir); /* Always positive when primary vertex is seeked for*/ 
                if( JetVrtDir>0. ) iniVrt=tmpVrt.FitVertex;                /* Good initial estimation */ 
                else               iniVrt=PrimVrt.position();
             }
             m_fitSvc->setApproximateVertex(iniVrt.x(), iniVrt.y(), iniVrt.z(),*state);
             tmpVrt.i=i; tmpVrt.j=j;
             sc=VKalVrtFitBase(TracksForFit,tmpVrt.FitVertex, tmpVrt.Momentum, Charge,
                               tmpVrt.ErrorMatrix, tmpVrt.Chi2PerTrk, tmpVrt.TrkAtVrt, tmpVrt.Chi2,
                               *state, true);
             if(sc.isFailure())                       continue;          /* No fit */ 
             if(tmpVrt.Chi2 > m_sel2VrtChi2Cut)       continue;          /* Bad Chi2 */
	     if(fabs(tmpVrt.FitVertex.z())> 650.)     continue;  // definitely outside of Pixel detector
             Dist2D=tmpVrt.FitVertex.perp(); 
	     if(Dist2D    > 180. )             continue;  // can't be from B decay
             double mass_PiPi =  tmpVrt.Momentum.M();  
	     if(mass_PiPi > m_Vrt2TrMassLimit)      continue;  // can't be from B decay
             VrtVrtDist(PrimVrt, tmpVrt.FitVertex, tmpVrt.ErrorMatrix, Signif3D);
	     tmpVrt.Signif3D=Signif3D;
             VrtVrtDist2D(PrimVrt, tmpVrt.FitVertex, tmpVrt.ErrorMatrix, tmpVrt.Signif2D);
//---
             TVector3 SVmPV(tmpVrt.FitVertex.x()-PrimVrt.x(),tmpVrt.FitVertex.y()-PrimVrt.y(),tmpVrt.FitVertex.z()-PrimVrt.z());
             tmpVrt.dRSVPV=JetDir.DeltaR(TLorentzVector(SVmPV, 1.)); //DeltaR SV-PV vs jet
             if(tmpVrt.dRSVPV > m_coneForTag ) continue;  // SV is outside of the jet cone
//---
             JetVrtDir = SVmPV.Dot(JetDir.Vect());
 	     double vPos=SVmPV.Dot(tmpVrt.Momentum.Vect())/tmpVrt.Momentum.Rho();
             if((!m_multiWithPrimary) &&(!m_getNegativeTail) && (!m_getNegativeTag) &&  JetVrtDir<0. )  continue; /* secondary vertex behind primary*/
	     if(vPos<-100.) continue;                                              /* Secondary vertex is too far behind primary*/
//
// Check pixel hits vs vertex positions.
             if(m_useVertexCleaning){    if(!Check2TrVertexInPixel(SelectedTracks[i],SelectedTracks[j],tmpVrt.FitVertex,tmpVrt.ErrorMatrix)) continue;     }
//--------
//
             double Signif3Dproj=VrtVrtDist( PrimVrt, tmpVrt.FitVertex, tmpVrt.ErrorMatrix, JetDir);
	     tmpVrt.Signif3DProj=Signif3Dproj;
             double Signif3DSign=Signif3D; if(JetVrtDir<0) Signif3DSign=-Signif3D;
  	     if(m_fillHist){ m_hb_signif3D->Fill( Signif3DSign, m_w_1); m_hb_sig3DNtr->Fill(Signif3Dproj, 1.);}

             //if( m_multiWithPrimary || m_multiVertex) { // For multivertex
             //  add_edge(i,j,*m_compatibilityGraph);
             //} 
  	     if( m_multiWithPrimary )   continue;   /* Multivertex with primary one. All below is not needed */
//
//  Check if V0 or material interaction on Pixel layer is present
//
	     if( Charge == 0 && Signif3D>8. && mass_PiPi<900.) {
               double mass_PPi  =  massV0( tmpVrt.TrkAtVrt,m_massP,m_massPi);
               double mass_EE   =  massV0( tmpVrt.TrkAtVrt,m_massE,m_massE);
               if(m_fillHist && !m_multiVertex){m_hb_massEE->Fill( mass_EE, m_w_1);} 
	       if(       mass_EE <  40.)  { 
	         BadTracks = 3;
	       }else{
                 if(m_fillHist && !m_multiVertex){m_hb_massPiPi->Fill( mass_PiPi, m_w_1);} /* Total mass with input particles masses*/
                 if(m_fillHist && !m_multiVertex){m_hb_massPPi->Fill( mass_PPi, m_w_1);} 
	         if( fabs(mass_PiPi-m_massK0) < 22. )  BadTracks = 1;
	         if( fabs(mass_PPi-m_massLam) <  8. )  BadTracks = 2;
	       }
//
//  Creation of V0 tracks
//
	       if(BadTracks){
	          std::vector<double> InpMassV0;
                  //Reset VKalVrt settings
                  state = m_fitSvc->makeState();
		                                              //matrix are calculated
		  if( BadTracks == 1 ) {  // K0 case
		    InpMassV0.push_back(m_massPi);InpMassV0.push_back(m_massPi);
                    m_fitSvc->setMassInputParticles( InpMassV0, *state );
                    m_fitSvc->setMassForConstraint(m_massK0, *state);
                    m_fitSvc->setCnstType(1, *state);       // Set mass  constraint
                  }
		  if( BadTracks == 2 ) {  // Lambda case
	            if( fabs(1./tmpVrt.TrkAtVrt[0][2]) > fabs(1./tmpVrt.TrkAtVrt[1][2]) ) {
		            InpMassV0.push_back(m_massP);InpMassV0.push_back(m_massPi);
	            }else{  InpMassV0.push_back(m_massPi);InpMassV0.push_back(m_massP); }
                    m_fitSvc->setMassInputParticles( InpMassV0, *state );
                    m_fitSvc->setMassForConstraint(m_massLam, *state);
                    m_fitSvc->setCnstType(1, *state);       // Set mass  constraint
                  }
		  if( BadTracks == 3 ) {  // Gamma case
		    InpMassV0.push_back(m_massE);InpMassV0.push_back(m_massE);
                    m_fitSvc->setMassInputParticles( InpMassV0, *state );
                    m_fitSvc->setCnstType(12, *state);       // Set 3d angular constraint
                  }
                  m_fitSvc->setApproximateVertex(tmpVrt.FitVertex.x(),tmpVrt.FitVertex.y(),tmpVrt.FitVertex.z(),*state);
                  TLorentzVector MomentumV0;
                  Amg::Vector3D  FitVertexV0;
                  std::vector< std::vector<double> > TrkAtVrtV0; 
                  std::vector<double> ErrorMatrixV0;
		  double Chi2V0;
                  sc=VKalVrtFitBase(TracksForFit, FitVertexV0, MomentumV0, Charge,
                                    ErrorMatrixV0,Chi2PerTrk,TrkAtVrtV0,Chi2V0,
                                    *state, true);
                  if(sc.isSuccess()) {
                    sc=m_fitSvc->VKalVrtCvtTool(FitVertexV0,MomentumV0,ErrorMatrixV0,0,VKPerigee,CovPerigee,*state);
                    if(sc.isSuccess()) {
                      const Trk::Track* TT = m_fitSvc->CreateTrkTrack(VKPerigee,CovPerigee,*state); 
                      double ImpactSignifV0=m_fitSvc->VKalGetImpact(TT, PrimVrt.position(), 0, Impact, ImpactError, *state);
                      if(m_fillHist){m_hb_impV0->Fill( ImpactSignifV0, m_w_1);}
	              if(ImpactSignifV0>3.0 ) BadTracks=0;
		      delete TT;
	            } else { BadTracks=0;}
	         }  // else { BadTracks=0;}
               }
             }
//
//  Check interactions on material layers
//
            float minWgtI = std::min(trkScore[i][2],trkScore[j][2]);
            if( minWgtI >0.50 && Dist2D > m_beampipeR-VrtRadiusError(tmpVrt.FitVertex, tmpVrt.ErrorMatrix) ) BadTracks = 4;
            //if( (trkScore[i][2]>0.4 || trkScore[j][2]>0.4) 
            //   && insideMatLayer(tmpVrt.FitVertex.x(),tmpVrt.FitVertex.y()) ) BadTracks=5;
//
//-----------------------------------------------
	     tmpVrt.badVrt=BadTracks;          //
	     all2TrVrt.push_back(tmpVrt);      //
//-----------------------------------------------
             if(m_fillHist){  m_hb_r2d->Fill( tmpVrt.FitVertex.perp(), m_w_1); }
	     //if(m_useMaterialRejection && Dist2D>m_beampipeR-2.){
             //if(m_materialMap){
             //  if(m_materialMap->inMaterial(tmpVrt.FitVertex)) BadTracks=4;
             //  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" MaterialMap test="<< BadTracks<<endreq;
             //}
//
//  Creation of tracks from V0 list
//	     
	     if( BadTracks ){
	        TrkFromV0.push_back(SelectedTracks[i]);
	        TrkFromV0.push_back(SelectedTracks[j]);
	     }else{
//
  		double JetVrtDir =
   	          JetDir.Px()*(tmpVrt.FitVertex.x()-PrimVrt.x())
                + JetDir.Py()*(tmpVrt.FitVertex.y()-PrimVrt.y())
 		+ JetDir.Pz()*(tmpVrt.FitVertex.z()-PrimVrt.z());
                if(m_getNegativeTail) JetVrtDir=fabs(JetVrtDir);  // For negative TAIL
  	                                                          // accepts also negative
  	                                                          // track pairs
                if(m_getNegativeTag) JetVrtDir=-JetVrtDir;        // For negative TAG
  	                                                          // accepts only negative
  	                                                          // track pairs
	        if( (Signif3D>m_sel2VrtSigCut) ) {
                  if(fabs(Signif3Dproj)<m_sel2VrtSigCut)continue;
	          ListSecondTracks.push_back(SelectedTracks[i]);
	          ListSecondTracks.push_back(SelectedTracks[j]);
	          closeVrtSig.push_back(Signif3D);
	          closeVrtCh2.push_back(tmpVrt.Chi2);
                }
	     }
         }
      } 
//      if(m_fillHist)  fillVrtNTup(all2TrVrt);

//
//-- Start vertex analysis
/*
      std::vector<int> trkTypeSV(NTracks,-1);   // Track identification: -1 -unknown, 2-fragmentation, 1-Interaction, 0-HF
      std::vector<int> nFrVrtT(NTracks,0);      // Fragmentation vertex per track counter
      int foundHF=0;
      for( auto vv : all2TrVrt){
        if( 1.-1./11.*vv.Signif2D > std::min(trkScore[vv.i][0],trkScore[vv.j][0]) ){
           if( std::min(trkScore[vv.i][1],trkScore[vv.j][1]) > 0.5 ){ nFrVrtT[vv.i]++;  nFrVrtT[vv.j]++; }
           continue;
        }
	if( trkScore[vv.i][0]+trkScore[vv.j][0] < trkScore[vv.i][1]+trkScore[vv.j][1]) continue;
	if( trkScore[vv.i][0]+trkScore[vv.j][0] < trkScore[vv.i][2]+trkScore[vv.j][2]) continue;
        trkTypeSV[vv.i]=0; trkTypeSV[vv.j]=0; foundHF++;
        //if( trkScore[vv.i][0]>trkScore[vv.i][1] && trkScore[vv.j][0]>trkScore[vv.j][1] 
        // && trkScore[vv.i][0]>trkScore[vv.i][2] && trkScore[vv.j][0]>trkScore[vv.j][2] ){ trkTypeSV[vv.i]=0; trkTypeSV[vv.j]=0; foundHF++; } 
      }
      for( auto vv : all2TrVrt){                                                 //Now interactions+V0s
	if( foundHF==1 && (trkTypeSV[vv.i]==0 || trkTypeSV[vv.j]==0 )) continue; //preserve single HF-vertex
        if( vv.badVrt ){ trkTypeSV[vv.i]=1; trkTypeSV[vv.j]=1;}
      }
      for( auto vv : all2TrVrt){                                                              //Now fragmentation
        if( trkTypeSV[vv.i]>=0 || trkTypeSV[vv.j]>=0 ) continue;                              //Skip identified tracks
        if( trkScore[vv.i][1]>trkScore[vv.i][0] && trkScore[vv.j][1]>trkScore[vv.j][0] 
         && trkScore[vv.i][1]>trkScore[vv.i][2] && trkScore[vv.j][1]>trkScore[vv.j][2] ){ trkTypeSV[vv.i]=2; trkTypeSV[vv.j]=2;} 
      }
      for (i=0; i<NTracks; i++) if( trkTypeSV[i]==0 && nFrVrtT[i]>0 && trkScore[i][1]>0.5 ) trkTypeSV[i]=2;
//      
//-- Remove FF, IF(some) and II vertices
//      iv=0; while ( iv < (int)all2TrVrt.size() )
//             { if( trkTypeSV[all2TrVrt[iv].i]>0 && trkTypeSV[all2TrVrt[iv].j]>0) all2TrVrt.erase(all2TrVrt.begin()+iv); else iv++; }
//
*/
//-- Cleaning. Remove small wgtB tracks attached to one vertex only. 
//      std::vector<int> inVrt(NTracks,0);
//      for( auto vv : all2TrVrt){ inVrt[vv.i]++; inVrt[vv.j]++; }
//      ////std::map<float,int> trkInOneV; for( int kt=0; kt<NTracks; kt++) if(inVrt[kt]==1) trkInOneV[trkScore[kt][0]]=kt;
//      for(int kk=0; kk<NTracks; kk++){
//        if( inVrt[kk]==1 && trkScore[kk][0]<2.5*m_cutHFClass ) {
//           int iv=0;   while (iv<(int)all2TrVrt.size()){ if( all2TrVrt[iv].i==kk || all2TrVrt[iv].j==kk ) { all2TrVrt.erase(all2TrVrt.begin()+iv); break;} else iv++; }
//      } }
//============================================================================
//-- Save results
      ListSecondTracks.clear();
      std::map<float,int> trkHF;
      for( auto &vv : all2TrVrt){ 
        if( m_multiWithPrimary || m_multiVertex) add_edge(vv.i,vv.j,*m_compatibilityGraph);
        trkHF[trkScore[vv.i][0]]=vv.i; trkHF[trkScore[vv.j][0]]=vv.j;
      }
      for( auto it : trkHF) { ListSecondTracks.push_back(SelectedTracks[it.second]); }
//-Debug
      if( m_fillHist && m_curTup ){ 
         for( auto &it : trkHF) { m_curTup->itHF[m_curTup->NTHF++]=it.second; }
         for( auto &vv : all2TrVrt){ m_curTup->nVrtT[vv.i]++; m_curTup->nVrtT[vv.j]++; }
         fillVrtNTup(all2TrVrt);
      }
//
//--------------------------------------------------------------------
//-- Post-selection checks 
//--------------------------------------------------------------------
      if(ListSecondTracks.size()>0 ){ 
        if(m_fillHist){ m_pr_effVrt2tr->Fill((float)m_NRefPVTrk,1.);
                      //m_pr_effVrt2trEta->Fill( JetDir.Eta(),1.);}
                        m_pr_effVrt2trEta->Fill( JetDir.Eta(),(double)ListSecondTracks.size()/2.);}
      } else if(ListSecondTracks.size()==0) { if(m_fillHist){m_pr_effVrt2tr->Fill((float)m_NRefPVTrk,0.);
                                                             m_pr_effVrt2trEta->Fill(JetDir.Eta(),0.); }}
      return;
   }




   template <class Track>
   bool InDetVKalVxInJetTool::Check2TrVertexInPixel( const Track* p1, const Track* p2,
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
