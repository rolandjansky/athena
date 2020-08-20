/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "InDetVKalVxInJetTool/InDetVKalVxInJetTool.h"
//-------------------------------------------------
// Other stuff
#include  "AnalysisUtils/AnalysisMisc.h"
#include  "TrkToolInterfaces/ITrackSummaryTool.h"
#include  "TMath.h"
//
#include<iostream>

//----------------------------------------------------------------------------------------
//  GetVrtSec resurns the vector Results with the following
//   1) Vertex mass
//   2) Vertex/jet energy fraction
//   3) Number of initially selected 2tr vertices
//   4) Number of selected for vertexing tracks in jet 
//   5) Number of track in secondary vertex
//   6) 3D SV-PV significance with sign
//   7) Jet energy used in (2) calculation 
//   8) Mininal distance between vertex and any material layer
//   9) Transverse vertex/jet energy fraction. Jet pT independent.
//   10) "Product" variable
//   11) "Boost" variable
//---------------------------------------------------------------------------------------- 
namespace InDet{

  //inline double cutNDepNorm(int N, double Prb){return 1.41421356237*TMath::ErfInverse(1.-sqrt(2.*Prb/(N*N-N)));}
  inline double cutNDepNorm(int N, double Prb){return TMath::ChisquareQuantile(1.-sqrt(2.*Prb/(N*N-N)),2.);}

  xAOD::Vertex* InDetVKalVxInJetTool::GetVrtSec(const std::vector<const Rec::TrackParticle*>& InpTrk,
                                                const xAOD::Vertex                          & PrimVrt,
	                                        const TLorentzVector                        & JetDir,
                                                std::vector<double>                         & Results,
                                                std::vector<const Rec::TrackParticle*> & ListSecondTracks,
                                                std::vector<const Rec::TrackParticle*> & TrkFromV0) 
  const
  {

      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "GetVrtSec() called with Rec::TrackParticle=" <<InpTrk.size()<< endmsg;
   
      std::vector<const Rec::TrackParticle*> SelectedTracks,SelectedTracksRelax;
      SelectedTracks.clear(); SelectedTracksRelax.clear();
      ListSecondTracks.clear();
      Results.clear();        

      m_NRefTrk=0;
      if( InpTrk.size() < 2 ) { return 0;} // 0,1 track => nothing to do!


      int NPVParticle = SelGoodTrkParticle( InpTrk, PrimVrt, JetDir, SelectedTracks);

      long int NTracks = (int) (SelectedTracks.size());
      if(m_FillHist){m_hb_ntrkjet->Fill( (double) NTracks, m_w_1); }
      if( NTracks < 2 ) { return 0;} // 0,1 selected track => nothing to do!

      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Number of selected tracks inside jet= " <<NTracks << endmsg;
      
      SelGoodTrkParticleRelax( InpTrk, PrimVrt, JetDir, SelectedTracksRelax);
      TLorentzVector MomentumJet = TotalMom(GetPerigeeVector(SelectedTracksRelax));
      if(m_FillHist){m_hb_jmom->Fill( MomentumJet.E(), m_w_1); }


//--------------------------------------------------------------------------------------------	 
//                    Initial Rec::TrackParticle list ready


      std::vector<const Rec::TrackParticle*>  TracksForFit;
      //std::vector<double> InpMass; for(int i=0; i<NTracks; i++){InpMass.push_back(m_massPi);}
      std::vector<double> InpMass(NTracks,m_massPi);

      xAOD::Vertex xaodPrimVrt; xaodPrimVrt.setPosition(PrimVrt.position());
                                xaodPrimVrt.setCovariancePosition(PrimVrt.covariancePosition());
      Select2TrVrt(SelectedTracks, TracksForFit, xaodPrimVrt, JetDir, InpMass, 
	                           TrkFromV0, ListSecondTracks);
      m_WorkArray->m_Incomp.clear();  // Not needed for single vertex version
//
//--- Cleaning
// 
      if( TrkFromV0.size() > 1) {
        RemoveDoubleEntries(TrkFromV0);
        AnalysisUtils::Sort::pT (&TrkFromV0);
      }
//---
      std::vector<const Rec::TrackParticle*> saveSecondTracks(ListSecondTracks);
      double Vrt2TrackNumber = (double) ListSecondTracks.size()/2.;
      RemoveDoubleEntries(ListSecondTracks);
      AnalysisUtils::Sort::pT (&ListSecondTracks);
//--Number of 2tr vertices where each track is used
      std::vector<int> combCount(ListSecondTracks.size());
      for(int tk=0;tk<(int)ListSecondTracks.size(); tk++){
        combCount[tk]=std::count(saveSecondTracks.begin(),saveSecondTracks.end(),ListSecondTracks[tk]);
      }
//---
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" Found different tracks in pairs="<< ListSecondTracks.size()<<endmsg;
      if(ListSecondTracks.size() < 2 ) { return 0;} // Less than 2 tracks left

//
//-----------------------------------------------------------------------------------------------------
//            Secondary track list is ready
//            Now common vertex fit
//
      std::vector<const Rec::TrackParticle*>::const_iterator   i_ntrk, i_found;
      Amg::Vector3D          FitVertex;
      std::vector<double> ErrorMatrix;
      std::vector< std::vector<double> > TrkAtVrt; 
      TLorentzVector    Momentum;
      double Signif3D, Signif3DP=0, Signif3DS=0;
      std::vector<double> Impact,ImpactError;

      double Chi2 =  FitCommonVrt( ListSecondTracks,combCount, xaodPrimVrt, JetDir, InpMass, FitVertex, ErrorMatrix, Momentum, TrkAtVrt);
//
      if( Chi2 < 0) { return 0; }      // Vertex not reconstructed
//
// Check jet tracks not in secondary vertex
      std::vector<const Rec::TrackParticle*> AdditionalTracks;
      VrtVrtDist(PrimVrt, FitVertex, ErrorMatrix, Signif3D);
      if(Signif3D>8.){
       for (i_ntrk = SelectedTracks.begin(); i_ntrk < SelectedTracks.end(); ++i_ntrk) {
         i_found = find( ListSecondTracks.begin(), ListSecondTracks.end(), (*i_ntrk));
	 if( i_found != ListSecondTracks.end() ) continue;
         Signif3DS = m_fitSvc->VKalGetImpact((*i_ntrk), FitVertex         , 1, Impact, ImpactError);
         if( Signif3DS > 10.) continue;
         Signif3DP = m_fitSvc->VKalGetImpact((*i_ntrk), PrimVrt.position(), 1, Impact, ImpactError);
         if(m_FillHist){ m_hb_diffPS->Fill( Signif3DP-Signif3DS, m_w_1); }
	 if(Signif3DP-Signif3DS>1.0) AdditionalTracks.push_back((*i_ntrk));
       }
      }
//
// Add found tracks and refit
      if( AdditionalTracks.size() > 0){
        for (i_ntrk = AdditionalTracks.begin(); i_ntrk < AdditionalTracks.end(); ++i_ntrk)
                      ListSecondTracks.push_back((*i_ntrk));
        std::vector<int> tmpCount(ListSecondTracks.size(),1);
        Chi2 =  FitCommonVrt( ListSecondTracks, tmpCount, xaodPrimVrt, JetDir, InpMass, FitVertex, ErrorMatrix, Momentum, TrkAtVrt);
        if( Chi2 < 0) { return 0; }      // Vertex not reconstructed
      }
//
//  Saving of results
//
// VK 15.10.2007 Wrong idea. Kills high Pt jets 
// VK 12.02.2008 Idea correct, implementation was wrong!!! Tracks must have BL hits only if vertex is inside BL!! 
      if( ListSecondTracks.size()==2 ){         // If there are 2 only tracks
        int Charge=0, tCnt=0;
	int BLshared=0;
	int PXshared=0;
        std::vector<const Rec::TrackParticle*>::const_iterator   i_ntrk;
        for (i_ntrk = ListSecondTracks.begin(); i_ntrk < ListSecondTracks.end(); ++i_ntrk) {
            Charge +=  (int) (*i_ntrk)->charge();
            const Trk::TrackSummary* testSum = (*i_ntrk)->trackSummary();
            if(testSum){  BLshared   += testSum->get(Trk::numberOfBLayerSharedHits);
                          PXshared   += testSum->get(Trk::numberOfPixelSharedHits); }
            tCnt++;
        }
        if(m_useVertexCleaning){
          if(!Check2TrVertexInPixel(ListSecondTracks[0],ListSecondTracks[1],FitVertex,Signif3D)) return 0;
          double xDif=FitVertex.x()-m_XlayerB, yDif=FitVertex.y()-m_YlayerB ; 
          double Dist2D=sqrt(xDif*xDif+yDif*yDif);
          if     (Dist2D < m_RlayerB-m_SVResolutionR) {       // Inside B-layer
            if(m_FillHist){ if(Charge){m_hb_totmass2T1->Fill(Momentum.M(),m_w_1);}else{m_hb_totmass2T0->Fill(Momentum.M(),m_w_1);} }
            if(m_FillHist){ m_hb_blshared->Fill((float)BLshared,m_w_1); }
            //if(BLshared>m_CutSharedHits) return 0;
          } else if(Dist2D > m_RlayerB+m_SVResolutionR) {       //Outside b-layer
            if(m_FillHist){ m_hb_pxshared->Fill((float)PXshared,m_w_1); }
            //if(PXshared>m_CutSharedHits) return 0;
          }
        }   // end of 2tr vertex cleaning code
//
        if( !Charge && fabs(Momentum.M()-m_massK0)<15. ) {       // Final rejection of K0
	  TrkFromV0.push_back(ListSecondTracks[0]);
	  TrkFromV0.push_back(ListSecondTracks[1]);
          if( TrkFromV0.size() > 1) {
	     RemoveDoubleEntries(TrkFromV0);
             AnalysisUtils::Sort::pT (&TrkFromV0);
          }
          return 0;
        }
//-- Protection against fake vertices far from interaction point 
	if(NPVParticle<1)NPVParticle=1;
        double vvdist3D=VrtVrtDist(PrimVrt, FitVertex, ErrorMatrix, Signif3D);
        double t3Dimp1= m_fitSvc->VKalGetImpact(ListSecondTracks[0], PrimVrt.position(), 1, Impact, ImpactError)/fabs(TrkAtVrt[0][2]);
        double t3Dimp2= m_fitSvc->VKalGetImpact(ListSecondTracks[1], PrimVrt.position(), 1, Impact, ImpactError)/fabs(TrkAtVrt[1][2]);
	double selVar=(t3Dimp1<t3Dimp2 ? t3Dimp1 : t3Dimp2)/sqrt((double)NPVParticle)/vvdist3D/500.;
        if(m_FillHist){ m_hb_tr2SelVar->Fill( selVar , m_w_1); }
	if(selVar<m_AntiFake2trVrtCut)return 0;
        if(m_FillHist){ m_hb_totmass2T2->Fill(Momentum.M(),m_w_1); }
      }


      double JetVrtDir =
             JetDir.Px()*(FitVertex.x()-PrimVrt.x())
           + JetDir.Py()*(FitVertex.y()-PrimVrt.y())
 	   + JetDir.Pz()*(FitVertex.z()-PrimVrt.z());
      if(  m_getNegativeTag )
         { if( JetVrtDir>0. )   return 0; }
      else if( m_getNegativeTail )
         { ; }
      else 
         { if( JetVrtDir<0. ) return 0; } 

 
      double xvt=FitVertex.x(); double yvt=FitVertex.y();
      double Dist2DBP=sqrt( (xvt-m_Xbeampipe)*(xvt-m_Xbeampipe) + (yvt-m_Ybeampipe)*(yvt-m_Ybeampipe) ); 
      double Dist2DBL=sqrt( (xvt-m_XlayerB)*(xvt-m_XlayerB) + (yvt-m_YlayerB)*(yvt-m_YlayerB) ); 
      double Dist2DL1=sqrt( (xvt-m_Xlayer1)*(xvt-m_Xlayer1) + (yvt-m_Ylayer1)*(yvt-m_Ylayer1) );
      double Dist2DL2=sqrt( (xvt-m_Xlayer2)*(xvt-m_Xlayer2) + (yvt-m_Ylayer2)*(yvt-m_Ylayer2) );
      double minDstMat=39.9;
      minDstMat=TMath::Min(minDstMat,fabs(Dist2DBP-m_Rbeampipe));
      minDstMat=TMath::Min(minDstMat,fabs(Dist2DBL-m_RlayerB));
      minDstMat=TMath::Min(minDstMat,fabs(Dist2DL1-m_Rlayer1));
      minDstMat=TMath::Min(minDstMat,fabs(Dist2DL2-m_Rlayer2));
      if(m_existIBL) minDstMat=TMath::Min(minDstMat,fabs(Dist2DL2-m_Rlayer3));  // 4-layer pixel detector

      VrtVrtDist(PrimVrt, FitVertex, ErrorMatrix, Signif3D);
      if(JetVrtDir < 0) Signif3D = -Signif3D;

      Amg::Vector3D DirForPt( FitVertex.x()-PrimVrt.x(),
                              FitVertex.y()-PrimVrt.y(),
		              FitVertex.z()-PrimVrt.z());
      if( m_MassType == 3 ) Results.push_back( TotalTVMom(DirForPt, GetPerigeeVector(ListSecondTracks))); 
      if( m_MassType == 2 ) Results.push_back(TotalTMom(GetPerigeeVector(ListSecondTracks))*1.15); 
      if( m_MassType == 1 ) Results.push_back(Momentum.M());        //1st

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

      if(m_FillHist){
          m_hb_r2dc->Fill( FitVertex.perp(), m_w_1);
          m_hb_mom->Fill( MomentumJet.E(), m_w_1);     
          m_hb_ratio->Fill( Results[1], m_w_1);
          m_hb_totmass->Fill( Results[0], m_w_1);
          m_hb_nvrt2->Fill( Results[2], m_w_1);
          m_hb_sig3DTot->Fill( Signif3D, m_w_1);
          // Find highest track Pt with respect to jet direction
          double trackPt, trackPtMax=0.;
          for (int tr=0; tr<(int)ListSecondTracks.size(); tr++) {
            trackPt=pTvsDir(Amg::Vector3D(JetDir.Vect().X(),JetDir.Vect().Y(),JetDir.Vect().Z()) , TrkAtVrt[tr]);
	    if(trackPt>trackPtMax)trackPtMax=trackPt;
          }
          m_hb_trkPtMax->Fill( trackPtMax, m_w_1);
          m_pr_effVrt->Fill((float)m_NRefTrk,1.);              
	  m_pr_effVrtEta->Fill( JetDir.Eta(),1.);
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
         AmgSymMatrix(5) *CovMtxP=new AmgSymMatrix(5);    (*CovMtxP).setIdentity(); 
         Trk::Perigee * tmpMeasPer  =  new Trk::Perigee( 0.,0., TrkAtVrt[ii][0], TrkAtVrt[ii][1], TrkAtVrt[ii][2],
                                                                Trk::PerigeeSurface(FitVertex), CovMtxP );
         tmpVTAV.push_back( Trk::VxTrackAtVertex( 1., tmpMeasPer) );
       }
       return tmpVertex;
  }



  xAOD::Vertex* InDetVKalVxInJetTool::tryPseudoVertex(const std::vector<const xAOD::TrackParticle*>& SelectedTracks,
                                                      const xAOD::Vertex                           & PrimVrt,
                                                      const TLorentzVector                         & JetDir,
                                                      const TLorentzVector                         & TrkJet,
						      const int                                    & nTrkLead,
	                                              std::vector<double>                          & Results)
  const
  {
//---------------First try jet axis+track
    if((int)SelectedTracks.size()<nTrkLead)return 0;
    //-----------------------------------------
    TLorentzVector sumSelTrk(0.,0.,0.,0.);  //Tracks coming from any SV

    Amg::Vector3D                           FitVertex;
    std::vector<double>                     ErrorMatrix;
    std::vector< std::vector<double> >      TrkAtVrt; 
    TLorentzVector                          Momentum;
    std::vector<double>                     Chi2PerTrk;
    long int              tChrg=0;
    double                Chi2=0.;
    std::vector<const xAOD::TrackParticle*> tTrkForFit(2,0);
    std::vector<float> tmpCov(15,0.); tmpCov[0]=1e-4; tmpCov[2]=4.e-4; tmpCov[5]=1.e-4; tmpCov[9]=1.e-4; tmpCov[14]=1.e-12; 
    StatusCode scode; scode.setChecked();  
    std::vector<const xAOD::TrackParticle*> reducedTrkSet(SelectedTracks.begin(),SelectedTracks.begin()+nTrkLead);
    double maxImp=RemoveNegImpact(reducedTrkSet,PrimVrt,JetDir,m_pseudoSigCut);
    if(reducedTrkSet.size()==0) return 0; 
    if(reducedTrkSet.size()==1 && maxImp<m_pseudoSigCut+1.)return 0;
    if(m_FillHist)m_hb_rawVrtN->Fill(reducedTrkSet.size(),1.);
//
//------ 
    int sel1T=-1; double selDST=0.;
    if(reducedTrkSet.size()>0){ 
       m_fitSvc->setApproximateVertex(PrimVrt.x(), PrimVrt.y(), PrimVrt.z()); 
       xAOD::TrackParticle *tmpBTP=new xAOD::TrackParticle(); tmpBTP->makePrivateStore();
       tmpBTP->setDefiningParameters(0., 0., JetDir.Phi(), JetDir.Theta(), sin(JetDir.Theta())/2.e5);   // Pt=200GeV track
       tmpBTP->setParametersOrigin(PrimVrt.x(),PrimVrt.y(),PrimVrt.z());
       tmpBTP->setDefiningParametersCovMatrixVec(tmpCov);
       tTrkForFit[1]=tmpBTP; 
       TLorentzVector sumB(0.,0.,0.,0.);
       int nvFitted=0;
       for(int it=0; it<(int)reducedTrkSet.size(); it++){
          tTrkForFit[0]=reducedTrkSet[it];
	  if(tTrkForFit[0]->pt()<2000.)continue;
	  scode=VKalVrtFitBase( tTrkForFit, FitVertex, Momentum, tChrg, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2);  nvFitted++;
	  if(scode.isFailure() || Chi2>6.)continue;
          if(FitVertex.perp()>reducedTrkSet[it]->radiusOfFirstHit())continue; 
          //double dSVPV=ProjPosT(FitVertex-PrimVrt.position(),JetDir);
	  double Signif3D; VrtVrtDist(PrimVrt, FitVertex, ErrorMatrix, Signif3D);
          if(FitVertex.perp()>m_Rbeampipe && Signif3D<2.)  continue;  // Cleaning of material interactions
   	  if(m_FillHist)m_hb_DST_JetTrkSV->Fill(Signif3D,1.);
          if(tTrkForFit[0]->pt()<5.e4){
            if(!Check2TrVertexInPixel(tTrkForFit[0],tTrkForFit[0],FitVertex,Signif3D)) continue;
	  }
          sumSelTrk += MomAtVrt(TrkAtVrt[0]) ; sumB += MomAtVrt(TrkAtVrt[1]) ;
	  if(Signif3D>selDST){ sel1T=it; selDST=Signif3D;} 
       }
       if(sumSelTrk.Pt()>0. && sel1T>=0 ){
	  Results.resize(4);
	  double pt1=sumSelTrk.Pt(sumB.Vect()); double E1=sqrt(pt1*pt1+sumSelTrk.M2()); 
          Results[0]=pt1+E1;                      //Invariant mass
          Results[1]=sumSelTrk.Pt()/TrkJet.Pt();  //Ratio
          Results[2]=0.;                          //Should be
          Results[3]=reducedTrkSet.size();        //Found leading tracks with high positive impact
          tTrkForFit[0]=reducedTrkSet[sel1T]; //------------------------- Refit selected vertex for xAOD::Vertex
          if(nvFitted>1){  //If only one fit attempt made - don't need to refit
	    scode=VKalVrtFitBase( tTrkForFit, FitVertex, Momentum, tChrg, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2);
	    if(scode.isFailure()){sumSelTrk.SetXYZT(0.,0.,0.,0.); Results.clear();}
          }
       }
       delete tmpBTP;
     }
// 
//------ Plane-Plane crossing doesn't provide good vertices for the moment
//     int sel2TI=-1,sel2TJ=-1;
//     if(reducedTrkSet.size()>1 && sel1T<0){ 
//       int nPRT=reducedTrkSet.size();
//       for(int it=0; it<nPRT-1; it++){  for(int jt=it+1; jt<nPRT; jt++){
//          TLorentzVector  pseudoB=GetBDir( reducedTrkSet[it],reducedTrkSet[jt],PrimVrt);
//          if(pseudoB.DeltaR(JetDir)>0.3)continue;
//	  sel2TI=it; sel2TJ=jt;
//       } }
//       if(sel1T<0 && sel2TI>=0){
//          tTrkForFit[0]=reducedTrkSet[sel2TI]; tTrkForFit[1]=reducedTrkSet[sel2TJ];
//	  scode=VKalVrtFitBase( tTrkForFit, FitVertex, Momentum, tChrg, ErrorMatrix, Chi2PerTrk, TrkAtVrt, Chi2);
//	  if(scode.isSuccess() && ProjPosT(FitVertex-PrimVrt.position(),JetDir) > 0 ){
//            sumSelTrk += MomAtVrt(TrkAtVrt[0]) ; sumSelTrk += MomAtVrt(TrkAtVrt[1]) ;
//	    Results.resize(4);
//            Results[0]=sumSelTrk.M();               //Invariant mass
//            Results[1]=sumSelTrk.Pt()/TrkJet.Pt();  //Ratio
//            Results[2]=0.;                          //Should be
//            Results[3]=nPRT;        //Found leading tracks with high positive impact
//       } }
//     }
//
//------ 
     if(sumSelTrk.Pt()==0)return 0;    //---------- Nothing found. Damn it! Else - return xAOD::Vertex
     Results.resize(7);
     Results[4]=0.; Results[5]=0.;
     Results[6]=TrkJet.E();
     xAOD::Vertex * tmpVertex=new xAOD::Vertex();
     tmpVertex->makePrivateStore();
     tmpVertex->setPosition(FitVertex);
     std::vector<float> floatErrMtx; floatErrMtx.resize(ErrorMatrix.size());
     for(int i=0; i<(int)ErrorMatrix.size(); i++) floatErrMtx[i]=ErrorMatrix[i];
     tmpVertex->setCovariance(floatErrMtx);
     tmpVertex->setFitQuality(Chi2, (float)(tTrkForFit.size()*2.-3.));
     std::vector<Trk::VxTrackAtVertex> & tmpVTAV=tmpVertex->vxTrackAtVertex();    tmpVTAV.clear();
       AmgSymMatrix(5) *CovMtxP=new AmgSymMatrix(5);   (*CovMtxP).setIdentity(); 
       Trk::Perigee * tmpMeasPer  =  new Trk::Perigee( 0.,0., TrkAtVrt[0][0], TrkAtVrt[0][1], TrkAtVrt[0][2],
                                                              Trk::PerigeeSurface(FitVertex), CovMtxP );
       tmpVTAV.push_back( Trk::VxTrackAtVertex( 1., tmpMeasPer) );
       ElementLink<xAOD::TrackParticleContainer> TEL;  TEL.setElement( tTrkForFit[0] );
       const xAOD::TrackParticleContainer* cont = (const xAOD::TrackParticleContainer* ) (tTrkForFit[0]->container() );
       TEL.setStorableObject(*cont);
       tmpVertex->addTrackAtVertex(TEL,1.);
     if(m_FillHist){m_hb_massJetTrkSV ->Fill(Results[0],1.);
                    m_hb_ratioJetTrkSV->Fill(Results[1],1.); 
                    m_hb_NImpJetTrkSV ->Fill(Results[3],1.);}
     return tmpVertex;
  }





  xAOD::Vertex* InDetVKalVxInJetTool::GetVrtSec(const std::vector<const xAOD::TrackParticle*>& InpTrk,
                                                const xAOD::Vertex                           & PrimVrt,
                                                const TLorentzVector                         & JetDir,
	                                        std::vector<double>                          & Results,
                                                std::vector<const xAOD::TrackParticle*>      & ListSecondTracks,
				                std::vector<const xAOD::TrackParticle*>      & TrkFromV0) 
  const
  {

      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "GetVrtSec() called with xAOD::TrackParticle=" <<InpTrk.size()<< endmsg;
   
      std::vector<const xAOD::TrackParticle*> SelectedTracks(0),SelectedTracksRelax(0);
      Results.clear();        
      ListSecondTracks.clear();

      m_NRefTrk=0;
      if( InpTrk.size() < 2 ) { return 0;} // 0,1 track => nothing to do!
      int NPVParticle = SelGoodTrkParticle( InpTrk, PrimVrt, JetDir, SelectedTracks);
      if((int)SelectedTracks.size()>m_TrackInJetNumberLimit)SelectedTracks.resize(m_TrackInJetNumberLimit); // SelectedTracks are ordered in pT

      long int NTracks = (int) (SelectedTracks.size());
      if(m_FillHist){m_hb_ntrkjet->Fill( (double) NTracks, m_w_1); }
      if( NTracks < 2 ) { return 0;} // 0,1 selected track => nothing to do!

      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Number of selected tracks inside jet= " <<NTracks << endmsg;
      
      SelGoodTrkParticleRelax( InpTrk, PrimVrt, JetDir, SelectedTracksRelax);
      TLorentzVector MomentumJet = TotalMom(SelectedTracksRelax);
      if(m_FillHist){m_hb_jmom->Fill( MomentumJet.E(), m_w_1); }


//--------------------------------------------------------------------------------------------	 
//                    Initial xAOD::TrackParticle list ready
      float Vrt2TrackNumber =0;
      std::vector<const xAOD::TrackParticle*>::const_iterator   i_ntrk;
      Amg::Vector3D          FitVertex;
      std::vector<double> ErrorMatrix,Impact,ImpactError;
      std::vector< std::vector<double> > TrkAtVrt; 
      TLorentzVector    Momentum;
      double Signif3D=0., Chi2=0.;
      const int nTrkLead=5;


      std::vector<const xAOD::TrackParticle*>  TracksForFit;
      //std::vector<double> InpMass; for(int i=0; i<NTracks; i++){InpMass.push_back(m_massPi);}
      std::vector<double> InpMass(NTracks,m_massPi);
      Select2TrVrt(SelectedTracks, TracksForFit, PrimVrt, JetDir, InpMass, TrkFromV0, ListSecondTracks);
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
      for(auto iv0 : TrkFromV0){ auto itf=std::find(SelectedTracks.begin(),SelectedTracks.end(),iv0);
                                 if(itf!=SelectedTracks.end())  SelectedTracks.erase(itf);}
//---
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" Found different xAOD tracks in pairs="<< ListSecondTracks.size()<<endmsg;
      if(ListSecondTracks.size() < 2 ) return tryPseudoVertex( SelectedTracks, PrimVrt, JetDir, MomentumJet, nTrkLead, Results);

//--- At least 2 secondary tracks are found.

      AnalysisUtils::Sort::pT (&ListSecondTracks);
//--Number of 2tr vertices where each track is used
      std::vector<int> combCount(ListSecondTracks.size());
      for(int tk=0;tk<(int)ListSecondTracks.size(); tk++){
        combCount[tk]=std::count(saveSecondTracks.begin(),saveSecondTracks.end(),ListSecondTracks[tk]);
      }
//
//-----------------------------------------------------------------------------------------------------
//            Secondary track list is ready
//            Now common vertex fit
//
      std::vector<const xAOD::TrackParticle*>::const_iterator   i_found;
      double Signif3DP=0, Signif3DS=0;

      Chi2 =  FitCommonVrt( ListSecondTracks, combCount, PrimVrt, JetDir, InpMass, FitVertex, ErrorMatrix, Momentum, TrkAtVrt);
      if( Chi2 < 0 && ListSecondTracks.size()>2 ) { // Vertex not reconstructed. Try to remove one track with biggest pt.
        double tpmax=0; int ipmax=-1;
        for(int it=0; it<(int)ListSecondTracks.size(); it++) if(tpmax<ListSecondTracks[it]->pt()){tpmax=ListSecondTracks[it]->pt(); ipmax=it;}
        if(ipmax>=0)RemoveEntryInList(ListSecondTracks,combCount,ipmax);
        Chi2 =  FitCommonVrt( ListSecondTracks, combCount, PrimVrt, JetDir, InpMass, FitVertex, ErrorMatrix, Momentum, TrkAtVrt);
        if( Chi2 < 0 && ListSecondTracks.size()>2 ) { // Vertex not reconstructed. Try to remove another track with biggest pt.
          tpmax=0.; ipmax=-1;
          for(int it=0; it<(int)ListSecondTracks.size(); it++) if(tpmax<ListSecondTracks[it]->pt()){tpmax=ListSecondTracks[it]->pt(); ipmax=it;}
          if(ipmax>=0)RemoveEntryInList(ListSecondTracks,combCount,ipmax);
          Chi2 =  FitCommonVrt( ListSecondTracks, combCount, PrimVrt, JetDir, InpMass, FitVertex, ErrorMatrix, Momentum, TrkAtVrt);
        }
      }
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" FitCommonVrt result="<< Chi2<<endmsg;
//
      if( Chi2 < 0) return tryPseudoVertex( SelectedTracks, PrimVrt, JetDir, MomentumJet, nTrkLead, Results);
//
// Check jet tracks not in secondary vertex
      std::vector<const xAOD::TrackParticle*> AdditionalTracks;
      VrtVrtDist(PrimVrt, FitVertex, ErrorMatrix, Signif3D);
      if(Signif3D>8.){
       for (i_ntrk = SelectedTracks.begin(); i_ntrk < SelectedTracks.end(); ++i_ntrk) {
         i_found = find( ListSecondTracks.begin(), ListSecondTracks.end(), (*i_ntrk));
	 if( i_found != ListSecondTracks.end() ) continue;
         Signif3DS = m_fitSvc->VKalGetImpact((*i_ntrk), FitVertex         , 1, Impact, ImpactError);
         if( Signif3DS > 10.) continue;
         Signif3DP = m_fitSvc->VKalGetImpact((*i_ntrk), PrimVrt.position(), 1, Impact, ImpactError);
         if(m_FillHist){ m_hb_diffPS->Fill( Signif3DP-Signif3DS, m_w_1); }
	 if(Signif3DP-Signif3DS>1.0) AdditionalTracks.push_back((*i_ntrk));
       }
      }
//
// Add found tracks and refit
      if( AdditionalTracks.size() > 0){
        for (i_ntrk = AdditionalTracks.begin(); i_ntrk < AdditionalTracks.end(); ++i_ntrk)
                      ListSecondTracks.push_back((*i_ntrk));
        std::vector<int> tmpCount(ListSecondTracks.size(),1);
        Chi2 =  FitCommonVrt( ListSecondTracks, tmpCount, PrimVrt, JetDir, InpMass, FitVertex, ErrorMatrix, Momentum, TrkAtVrt);
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" Added track FitCommonVrt output="<< Chi2<<endmsg;
        if( Chi2 < 0) return tryPseudoVertex( SelectedTracks, PrimVrt, JetDir, MomentumJet, nTrkLead, Results);
      }
//
//  Saving of results
//
//
//
      if( ListSecondTracks.size()==2 ){         // If there are 2 only tracks
        int Charge=0;
	uint8_t BLshared=0;
	uint8_t PXshared=0;
        for (i_ntrk = ListSecondTracks.begin(); i_ntrk < ListSecondTracks.end(); ++i_ntrk) {
            Charge +=  (int) (*i_ntrk)->charge();
            uint8_t retval=0;
            if( (*i_ntrk)->summaryValue( retval, xAOD::numberOfPixelSharedHits)  )  PXshared  += retval;
            if( (*i_ntrk)->summaryValue( retval, xAOD::numberOfInnermostPixelLayerSharedHits) )  BLshared  += retval;
        }

        double vvdist3D=VrtVrtDist(PrimVrt, FitVertex, ErrorMatrix, Signif3D);
        if(m_useVertexCleaning){
          if(!Check2TrVertexInPixel(ListSecondTracks[0],ListSecondTracks[1],FitVertex,Signif3D)) return 0;
          double xDif=FitVertex.x()-m_XlayerB, yDif=FitVertex.y()-m_YlayerB ; 
          double Dist2D=sqrt(xDif*xDif+yDif*yDif);
          if     (Dist2D < m_RlayerB-m_SVResolutionR) {       // Inside B-layer
            if(m_FillHist){ if(Charge){m_hb_totmass2T1->Fill(Momentum.M(),m_w_1);}else{m_hb_totmass2T0->Fill(Momentum.M(),m_w_1);} }
            if(m_FillHist){ m_hb_blshared->Fill((float)BLshared,m_w_1); }
            //if(BLshared>m_CutSharedHits) return 0;          //VK Kills more b-jets events
          }else if(Dist2D > m_RlayerB+m_SVResolutionR) {      //Outside b-layer
            if(m_FillHist){ m_hb_pxshared->Fill((float)PXshared,m_w_1); }
            //if(PXshared>m_CutSharedHits) return 0;
          }
        } //end 2tr vertex cleaning code
//
        if( !Charge && fabs(Momentum.M()-m_massK0)<15. ) {       // Final rejection of K0
	  TrkFromV0.push_back(ListSecondTracks[0]);
	  TrkFromV0.push_back(ListSecondTracks[1]);
          if( TrkFromV0.size() > 1) {
             RemoveDoubleEntries(TrkFromV0);
             AnalysisUtils::Sort::pT (&TrkFromV0);
          }
          return 0;
        }
//-- Protection against fake vertices far from interaction point 
	if(NPVParticle<1)NPVParticle=1;
        double t3Dimp1= m_fitSvc->VKalGetImpact(ListSecondTracks[0], PrimVrt.position(), 1, Impact, ImpactError)/fabs(TrkAtVrt[0][2]);
        double t3Dimp2= m_fitSvc->VKalGetImpact(ListSecondTracks[1], PrimVrt.position(), 1, Impact, ImpactError)/fabs(TrkAtVrt[1][2]);
	double selVar=(t3Dimp1<t3Dimp2 ? t3Dimp1 : t3Dimp2)/sqrt((double)NPVParticle)/vvdist3D/500.;
        if(m_FillHist){ m_hb_tr2SelVar->Fill( selVar , m_w_1); }
	if(selVar<m_AntiFake2trVrtCut)return 0;
        if(m_FillHist){ m_hb_totmass2T2->Fill(Momentum.M(),m_w_1); }
      }

	    
      double JetVrtDir =
             JetDir.Px()*(FitVertex.x()-PrimVrt.x())
           + JetDir.Py()*(FitVertex.y()-PrimVrt.y())
 	   + JetDir.Pz()*(FitVertex.z()-PrimVrt.z());
      if(  m_getNegativeTag )
         { if( JetVrtDir>0. )   return 0; }
      else if( m_getNegativeTail )
         { ; }
      else 
         { if( JetVrtDir<0. ) return 0; } 

      double xvt=FitVertex.x(); double yvt=FitVertex.y();
      double Dist2DBP=sqrt( (xvt-m_Xbeampipe)*(xvt-m_Xbeampipe) + (yvt-m_Ybeampipe)*(yvt-m_Ybeampipe) ); 
      double Dist2DBL=sqrt( (xvt-m_XlayerB)*(xvt-m_XlayerB) + (yvt-m_YlayerB)*(yvt-m_YlayerB) ); 
      double Dist2DL1=sqrt( (xvt-m_Xlayer1)*(xvt-m_Xlayer1) + (yvt-m_Ylayer1)*(yvt-m_Ylayer1) );
      double Dist2DL2=sqrt( (xvt-m_Xlayer2)*(xvt-m_Xlayer2) + (yvt-m_Ylayer2)*(yvt-m_Ylayer2) );
      double minDstMat=39.9;
      minDstMat=TMath::Min(minDstMat,fabs(Dist2DBP-m_Rbeampipe));
      minDstMat=TMath::Min(minDstMat,fabs(Dist2DBL-m_RlayerB));
      minDstMat=TMath::Min(minDstMat,fabs(Dist2DL1-m_Rlayer1));
      minDstMat=TMath::Min(minDstMat,fabs(Dist2DL2-m_Rlayer2));
      if(m_existIBL) minDstMat=TMath::Min(minDstMat,fabs(Dist2DL2-m_Rlayer3));  // 4-layer pixel detector
 
 
      VrtVrtDist(PrimVrt, FitVertex, ErrorMatrix, Signif3D);
      if(JetVrtDir < 0) Signif3D = -Signif3D;

      if(FitVertex.perp()>m_Rbeampipe && Signif3D<20.)  return 0;  // Final cleaning of material interactions

      Amg::Vector3D DirForPt( FitVertex.x()-PrimVrt.x(),
                              FitVertex.y()-PrimVrt.y(),
		              FitVertex.z()-PrimVrt.z());
      //if( m_MassType == 3 ) Results.push_back( TotalTVMom(DirForPt, GetPerigeeVector(ListSecondTracks))); 
      //if( m_MassType == 2 ) Results.push_back(TotalTMom(GetPerigeeVector(ListSecondTracks))*1.15); 
      if( m_MassType == 1 ) Results.push_back(Momentum.M());       //1st

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

     if(m_FillHist){
          // Find highest track Pt with respect to jet direction
          double trackPt, trackPtMax=0.;
          for (int tr=0; tr<(int)ListSecondTracks.size(); tr++) {
            trackPt=pTvsDir(Amg::Vector3D(JetDir.X(),JetDir.Y(),JetDir.Z()) , TrkAtVrt[tr]);
	    if(trackPt>trackPtMax)trackPtMax=trackPt;
          }
          m_hb_r2dc->Fill( FitVertex.perp(), m_w_1);    
          m_hb_trkPtMax->Fill( trackPtMax, m_w_1);
          m_pr_effVrt->Fill((float)m_NRefTrk,1.);              
	  m_pr_effVrtEta->Fill( JetDir.Eta(),1.);
          m_hb_mom->Fill( MomentumJet.E(), m_w_1);
          m_hb_ratio->Fill( Results[1], m_w_1); 
          m_hb_totmass->Fill( Results[0], m_w_1); 
          m_hb_nvrt2->Fill( Results[2], m_w_1);
          m_hb_sig3DTot->Fill( Signif3D, m_w_1);
          m_hb_dstToMat->Fill( minDstMat, m_w_1);
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
 				  std::vector<int>          & cntComb,
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
      const double maxRecMASS=6000.;
      long int           Charge;
      double             Chi2 = 0.;
      Amg::Vector3D      tmpVertex;

      int Outlier=1, i=0;
//
// Start of fit
//
      m_fitSvc->setDefault();
      m_fitSvc->setMassInputParticles( InpMass );            // Use pions masses
      m_fitSvc->setMomCovCalc(1);  /* Total momentum and its covariance matrix are calculated*/
      sc=VKalVrtFitFastBase(ListSecondTracks,FitVertex);          /* Fast crude estimation */
      if(sc.isFailure() || FitVertex.perp() > m_Rlayer2*2. ) {    /* No initial estimation */ 
         m_fitSvc->setApproximateVertex(PrimVrt.x(), PrimVrt.y(), PrimVrt.z()); /* Use as starting point */
      } else {
         m_fitSvc->setApproximateVertex(FitVertex.x(),FitVertex.y(),FitVertex.z()); /*Use as starting point*/
      }
//      m_fitSvc-> setVertexForConstraint(PrimVrt.x(),PrimVrt.y(),PrimVrt.z());
//      m_fitSvc->setCovVrtForConstraint(PrimVrt.errorPosition().covValue(Trk::x,Trk::x),
//                                       PrimVrt.errorPosition().covValue(Trk::y,Trk::x),
//                                       PrimVrt.errorPosition().covValue(Trk::y,Trk::y),
//                                       PrimVrt.errorPosition().covValue(Trk::z,Trk::x),
//                                       PrimVrt.errorPosition().covValue(Trk::z,Trk::y),
//                                       PrimVrt.errorPosition().covValue(Trk::z,Trk::z) );
//      m_fitSvc->setCnstType(7);
      if(m_RobustFit)m_fitSvc->setRobustness(m_RobustFit);
//fit itself
      int NTracksVrt = ListSecondTracks.size(); double FitProb=0.;
      for (i=0; i < NTracksVrt-1; i++) {
//         sc=m_fitSvc->VKalVrtFit(ListSecondTracks,FitVertex, Momentum,Charge,
//                                         ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2);
         sc=VKalVrtFitBase(ListSecondTracks,FitVertex, Momentum,Charge,
                                 ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2);
         if(sc.isFailure() ||  Chi2 > 1000000. ) { return -10000.;}    // No fit
         Outlier = FindMax( Chi2PerTrk, cntComb ); 
	 FitProb=TMath::Prob( Chi2, 2*ListSecondTracks.size()-3);
	 if(ListSecondTracks.size() == 2 )              break;         // Only 2 tracks left
//////////////////////////////
         double Signif3Dproj=VrtVrtDist( PrimVrt, FitVertex, ErrorMatrix, JetDir);
         if(Signif3Dproj<0 && (!m_getNegativeTail) && (!m_getNegativeTag) && FitProb < 0.1){
	   double maxDst=-1.e12; int maxT=-1; double minChi2=1.e12;
	   for(int it=0; it<(int)ListSecondTracks.size(); it++){
              std::vector<const Track*> tmpList(ListSecondTracks);
              tmpList.erase(tmpList.begin()+it);
              sc=VKalVrtFitBase(tmpList,tmpVertex,Momentum,Charge,ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2);
              if(sc.isFailure())continue;
              Signif3Dproj=VrtVrtDist( PrimVrt, tmpVertex, ErrorMatrix, JetDir);
	      if(Signif3Dproj>maxDst  && maxDst<10. ){maxDst=Signif3Dproj; maxT=it; minChi2=Chi2;}
	      else if(Signif3Dproj>0. && maxDst>10. && Chi2<minChi2) {minChi2=Chi2; maxT=it;}
	   }
	   if(maxT>=0){ Outlier=maxT;   RemoveEntryInList(ListSecondTracks,cntComb,Outlier);
                        m_fitSvc->setApproximateVertex(FitVertex.x(),FitVertex.y(),FitVertex.z());
			continue;}
	 }
////////////////////////////////////////
//	 if( Momentum.m() <6000.) {
//	   if( Chi2PerTrk[Outlier] < m_SecTrkChi2Cut && FitProb > 0.001)  break;  // Solution found
//	 }
	 if( FitProb > 0.001) {
	   if( Momentum.M() <maxRecMASS) {
	     if( Chi2PerTrk[Outlier] < m_SecTrkChi2Cut*m_chiScale[ListSecondTracks.size()<10?ListSecondTracks.size():10])  break;  // Solution found
           } else {
	     double minM=1.e12; int minT=-1; double minChi2=1.e12;
	     for(int it=0; it<(int)ListSecondTracks.size(); it++){
                std::vector<const Track*> tmpList(ListSecondTracks);
                tmpList.erase(tmpList.begin()+it);
                sc=VKalVrtFitBase(tmpList,tmpVertex,Momentum,Charge,ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2);
                if(sc.isFailure())continue;
		if(Momentum.M()<minM  && minM>maxRecMASS){minM=Momentum.M(); minT=it; minChi2=Chi2;}
		else if(Momentum.M()<maxRecMASS && minM<maxRecMASS && Chi2<minChi2){minChi2=Chi2; minT=it;}
	     }
	     if(minT>=0)Outlier=minT;
	   }
	 }
         RemoveEntryInList(ListSecondTracks,cntComb,Outlier);
         m_fitSvc->setApproximateVertex(FitVertex.x(),FitVertex.y(),FitVertex.z()); /*Use as starting point*/
      }
//--
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" SecVrt fit converged="<< ListSecondTracks.size()<<", "
          <<Chi2<<", "<<Chi2PerTrk[Outlier]<<" Mass="<<Momentum.M()<<endmsg;
//--
      if( ListSecondTracks.size()==2 ){
	 if( Momentum.M() > 6000. || FitProb < 0.001 || Chi2PerTrk[Outlier] > m_SecTrkChi2Cut) { return -10000.;  }  
      } 
//
//-- To kill remnants of conversion
      double Dist2D=sqrt(FitVertex.x()*FitVertex.x()+FitVertex.y()*FitVertex.y());
      if( ListSecondTracks.size()==2  && (Dist2D > 20.) && Charge==0 ) {
        double mass_EE   =  massV0( TrkAtVrt,m_massE,m_massE);
        if(m_FillHist){m_hb_totmassEE->Fill( mass_EE, m_w_1); }
        if( mass_EE < 40. ) return -40.;
      }
//-- Test creation of Trk::Track
//      if( ListSecondTracks.size()==2 && Charge==0 ) {      
//        std::vector<double> VKPerigee,CovPerigee;
//        sc=m_fitSvc->VKalVrtCvtTool(FitVertex,Momentum,ErrorMatrix,0,VKPerigee,CovPerigee);
//        if(sc.isSuccess())const Trk::Track* TT = m_fitSvc->CreateTrkTrack(VKPerigee,CovPerigee); 
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
      Amg::Vector3D          FitVertex, vDist;
      std::vector<double> ErrorMatrix,Chi2PerTrk,VKPerigee,CovPerigee,closeVrtSig(0);
      std::vector< std::vector<double> > TrkAtVrt; 
      TLorentzVector   Momentum;
      std::vector<double> Impact,ImpactError;
      double ImpactSignif=0;
      double             Chi2, Signif3D, Dist2D, JetVrtDir;
      long int           Charge;
      int i,j;
//
      long int NTracks = (int) (SelectedTracks.size());

      m_WorkArray->m_Incomp.clear();   // For multivertex
      m_WorkArray->m_Prmtrack.clear();   // For multivertex
//
//  Impact parameters with sign calculations
//
      double SignifR,SignifZ;
      std::vector<double> TrackSignif(NTracks),TrackPt(NTracks),TrackSignifBase(NTracks);
      AmgVector(5) tmpPerigee; tmpPerigee<<0.,0.,0.,0.,0.;
      int NPrimTrk=0, NSecTrk=0;
      m_NRefTrk=0;
      for (i=0; i<NTracks; i++) {
         ImpactSignif = m_fitSvc->VKalGetImpact(SelectedTracks[i], PrimVrt.position(), 1, Impact, ImpactError);
         TrackSignifBase[i]=ImpactSignif;
         tmpPerigee = GetPerigee(SelectedTracks[i])->parameters(); 
         if( sin(tmpPerigee[2]-JetDir.Phi())*Impact[0] < 0 ){ Impact[0] = -fabs(Impact[0]);}
	                                                else{ Impact[0] =  fabs(Impact[0]);}
         if(  (tmpPerigee[3]-JetDir.Theta())*Impact[1] < 0 ){ Impact[1] = -fabs(Impact[1]);}
	                                                else{ Impact[1] =  fabs(Impact[1]);}
	 SignifR = Impact[0]/ sqrt(ImpactError[0]);
	 SignifZ = Impact[1]/ sqrt(ImpactError[2]);
         if(m_FillHist){
	    m_hb_impactR->Fill( SignifR, m_w_1); 
            m_hb_impactZ->Fill( SignifZ, m_w_1); 
            m_hb_impactRZ->Fill(SignifR, SignifZ, m_w_1); 
         }
         TrackPt[i] = sin(tmpPerigee[3])/fabs(tmpPerigee[4]);
	 if(ImpactSignif < 3.) { NPrimTrk += 1;}
         else{NSecTrk += 1;}
         if( SignifR<0 || SignifZ<0 ) m_NRefTrk++;
         if(m_getNegativeTail){
  	    ImpactSignif = sqrt( SignifR*SignifR + SignifZ*SignifZ);
  	 }else if(m_getNegativeTag){
  	    ImpactSignif = sqrt(  (SignifR-0.6)*(SignifR-0.6)
  	                        + (SignifZ-0.6)*(SignifZ-0.6) );
  	 }else{
  	    ImpactSignif = sqrt(  (SignifR+0.6)*(SignifR+0.6)
  	                        + (SignifZ+0.6)*(SignifZ+0.6) );
  	 }
         if(fabs(SignifR) < m_AntiPileupSigRCut) {   // cut against tracks from pileup vertices  
           if(SignifZ > 1.+m_AntiPileupSigZCut ) ImpactSignif=0.;  
           if(SignifZ < 1.-m_AntiPileupSigZCut ) ImpactSignif=0.;  
         }
      
         TrackSignif[i]=ImpactSignif;
         if(m_FillHist){m_hb_impact->Fill( ImpactSignif, m_w_1);}
      }

      //m_NRefTrk=(4.*m_NRefTrk/3.+NPrimTrk)/2; m_NRefTrk=TMath::Max(m_NRefTrk,2);
      m_NRefTrk=TMath::Max(NPrimTrk,2);
      //double addNTrkDep=m_TrkSigNTrkDep*m_NRefTrk;      // NTrk dependence
      double addNTrkDep=TMath::Min((JetDir.Perp()/1.e6),m_TrkSigNTrkDep)*m_NRefTrk;      // NTrk dependence
      double SelLim = m_TrkSigCut;

      StatusCode sc; if(sc.isSuccess())ImpactSignif=0;      //Safety !
      if(m_MultiVertex || m_MultiWithPrimary) m_WorkArray->m_Incomp.reserve(NTracks*(NTracks-1));   // Reserve memory for multivertex
      ListSecondTracks.reserve(2*NTracks);                 // Reserve memory for sigle vertex

      Amg::Vector3D iniVrt(0.,0.,0.);
      m_fitSvc->setDefault();
      m_fitSvc->setMassInputParticles( InpMass );     // Use pion masses for fit
      m_fitSvc->setMomCovCalc(1);                     // Total momentum and its covariance matrix are calculated
      for (i=0; i<NTracks-1; i++) {
	 //double adp1=trkPtCorr(TrackPt[i]);
	 double adp1=pow(TrackPt[i]/JetDir.Perp(),0.5);
         for (j=i+1; j<NTracks; j++) {
             //if(m_MultiVertex || m_MultiWithPrimary){m_WorkArray->m_Incomp.push_back(i);m_WorkArray->m_Incomp.push_back(j);} // For PGRAPH multivertex   !!!ALL SELECTIONS MUST BE AFTER THIS LINE!!!
 	     if(TrackSignif[i]==0.)continue; // Pileup and other problems
 	     if(TrackSignif[j]==0.)continue; // Pileup and other problems
	     //double adp2=trkPtCorr(TrackPt[j]);
	     double adp2=pow(TrackPt[j]/JetDir.Perp(),0.5);
	     if(!m_MultiWithPrimary) {  // Not used for multi-vertex with primary one search
	        if(TrackSignif[i] < SelLim +adp1 +addNTrkDep) continue;
 	        if(TrackSignif[j] < SelLim +adp2 +addNTrkDep) continue;
	        if(TrackSignif[i]+TrackSignif[j]  < 2.*SelLim +m_TrkSigSumCut +adp1 +adp2 +2.*addNTrkDep) continue;
	     }
	     int BadTracks = 0;                                       //Bad tracks identification 
             TracksForFit.clear();
             m_fitSvc->setDefault();                          //Reset VKalVrt settings
             m_fitSvc->setMomCovCalc(1);                     // Total momentum and its covariance matrix are calculated
             TracksForFit.push_back( SelectedTracks[i] );
             TracksForFit.push_back( SelectedTracks[j] );
             sc=VKalVrtFitFastBase(TracksForFit,FitVertex);              /* Fast crude estimation*/
             if( sc.isFailure() || FitVertex.perp() > m_Rlayer2*2. ) {   /* No initial estimation */ 
                iniVrt=PrimVrt.position();
                if( m_MultiWithPrimary ) iniVrt.setZero(); 
 	     } else {
                vDist=FitVertex-PrimVrt.position();
                JetVrtDir = JetDir.Px()*vDist.x() + JetDir.Py()*vDist.y() + JetDir.Pz()*vDist.z();
                if( m_MultiWithPrimary ) JetVrtDir=fabs(JetVrtDir); /* Always positive when primary vertex is seeked for*/ 
                if( JetVrtDir>0. ) iniVrt=FitVertex;                /* Good initial estimation */ 
                else               iniVrt=PrimVrt.position();
             }
             m_fitSvc->setApproximateVertex(iniVrt.x(), iniVrt.y(), iniVrt.z()); 
             sc=VKalVrtFitBase(TracksForFit,FitVertex, Momentum,Charge,
                               ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2);
             if(sc.isFailure())                continue;          /* No fit */ 
             if(Chi2 > m_Sel2VrtChi2Cut+4.)    continue;          /* Bad Chi2 */
             double mass_PiPi =  Momentum.M();  
	     if(mass_PiPi > 6000.)             continue;  // can't be from B decay
             Dist2D=FitVertex.perp(); 
	     if(Dist2D    > 180. )             continue;  // can't be from B decay
             VrtVrtDist(PrimVrt, FitVertex, ErrorMatrix, Signif3D);
//---
	     vDist=FitVertex-PrimVrt.position();
             JetVrtDir = JetDir.Px()*vDist.x() + JetDir.Py()*vDist.y() + JetDir.Pz()*vDist.z();
	     double vPos=(vDist.x()*Momentum.Px()+vDist.y()*Momentum.Py()+vDist.z()*Momentum.Pz())/Momentum.Rho();
  	     if((!m_MultiWithPrimary) &&(!m_getNegativeTail) && (!m_getNegativeTag) &&  JetVrtDir<0. )  continue; /* secondary vertex behind primary*/
	     if(vPos<-150.) continue;                                              /* Secondary vertex is too far behind primary*/
//
// Check pixel hits vs vertex positions.
             if(m_useVertexCleaning){
               if(!Check2TrVertexInPixel(SelectedTracks[i],SelectedTracks[j],FitVertex,Signif3D)) continue;
             }

             if( m_MultiWithPrimary || m_MultiVertex) { // For multivertex
	       double limPtTr=m_JetPtFractionCut*JetDir.Perp();
               if(TrackPt[i]<limPtTr && Signif3D<9. && m_MultiWithPrimary) continue;
               if(TrackPt[j]<limPtTr && Signif3D<9. && m_MultiWithPrimary) continue;
	       //m_WorkArray->m_Incomp.pop_back();m_WorkArray->m_Incomp.pop_back();   //For PGRAPH 
               add_edge(i,j,*m_compatibilityGraph);
             } 

  	     if( m_MultiWithPrimary )   continue;   /* Multivertex with primary one. All below is not needed */
//--------
//
             double Signif3Dproj=VrtVrtDist( PrimVrt, FitVertex, ErrorMatrix, JetDir);
             double Signif3DSign=Signif3D;
  	     if(  JetVrtDir < 0) Signif3DSign=-Signif3D;
  	     if(m_FillHist)m_hb_signif3D->Fill( Signif3DSign, m_w_1);
	     if(m_FillHist)m_hb_massPiPi2->Fill( mass_PiPi, m_w_1);
             if(Signif3DSign<12. && Chi2>m_Sel2VrtChi2Cut)       continue;          /* Bad Chi2 */
//
//  Check if V0 or material interaction on Pixel layer is present
//
	     if( Charge == 0 && Signif3D>8. && mass_PiPi<900.) {
               double mass_PPi  =  massV0( TrkAtVrt,m_massP,m_massPi);
               double mass_EE   =  massV0( TrkAtVrt,m_massE,m_massE);
               if(m_FillHist){m_hb_massEE->Fill( mass_EE, m_w_1);} 
	       if(       mass_EE <  40.)  { 
	         BadTracks = 3;
	       }else{
                 if(m_FillHist){m_hb_massPiPi->Fill( mass_PiPi, m_w_1);}     /* Total mass with input particles masses*/
                 if(m_FillHist){m_hb_massPPi->Fill( mass_PPi, m_w_1);} 
	         if( fabs(mass_PiPi-m_massK0) < 22. )  BadTracks = 1;
	         if( fabs(mass_PPi-m_massLam) <  8. )  BadTracks = 2;
	         //double TransMass=TotalTMom(GetPerigeeVector(TracksForFit));
	         //if( TransMass<400. && m_FillHist)m_hb_massPiPi1->Fill( mass_PiPi, m_w_1);
	       }
//
//  Creation of V0 tracks
//
	       if(BadTracks){
	          std::vector<double> InpMassV0;
                  m_fitSvc->setDefault();                     //Reset VKalVrt settings
                  m_fitSvc->setMomCovCalc(1);                 //Total momentum and its covariance 
		                                              //matrix are calculated
		  if( BadTracks == 1 ) {  // K0 case
		    InpMassV0.push_back(m_massPi);InpMassV0.push_back(m_massPi);
                    m_fitSvc->setMassInputParticles( InpMassV0 );
                    m_fitSvc->setMassForConstraint(m_massK0);
                    m_fitSvc->setCnstType(1);       // Set mass  constraint
                  }
		  if( BadTracks == 2 ) {  // Lambda case
	            if( fabs(1./TrkAtVrt[0][2]) > fabs(1./TrkAtVrt[1][2]) ) {
		            InpMassV0.push_back(m_massP);InpMassV0.push_back(m_massPi);
	            }else{  InpMassV0.push_back(m_massPi);InpMassV0.push_back(m_massP); }
                    m_fitSvc->setMassInputParticles( InpMassV0 );
                    m_fitSvc->setMassForConstraint(m_massLam);
                    m_fitSvc->setCnstType(1);       // Set mass  constraint
                  }
		  if( BadTracks == 3 ) {  // Gamma case
		    InpMassV0.push_back(m_massE);InpMassV0.push_back(m_massE);
                    m_fitSvc->setMassInputParticles( InpMassV0 );
                    m_fitSvc->setCnstType(12);       // Set 3d angular constraint
//                    m_fitSvc->setVertexForConstraint(PrimVrt.x(),PrimVrt.y(),PrimVrt.z());
//                    m_fitSvc->setCovVrtForConstraint(0.015*0.015,0.,0.015*0.015,0.,0.,56.*56);
//                    m_fitSvc->setCnstType(7);          // Set pointing to primary vertex constraint
                  }
                  m_fitSvc->setApproximateVertex(FitVertex.x(),FitVertex.y(),FitVertex.z()); 
                  TLorentzVector MomentumV0;
                  Amg::Vector3D  FitVertexV0;
                  std::vector< std::vector<double> > TrkAtVrt0; 
		  double Chi2_0;
                  sc=VKalVrtFitBase(TracksForFit, FitVertexV0, MomentumV0, Charge,
                                    ErrorMatrix,Chi2PerTrk,TrkAtVrt0,Chi2_0);
                  if(sc.isSuccess()) {
                    //std::cout<<" cnst result="<< massV0( TrkAtVrt,m_massPi,m_massPi)<<", "
                    //                          << massV0( TrkAtVrt,m_massP,m_massPi)<<", "
                    //                          << massV0( TrkAtVrt,m_massE,m_massE)<<", "<<i<<", "<<j<<'\n';
                    sc=m_fitSvc->VKalVrtCvtTool(FitVertexV0,MomentumV0,ErrorMatrix,0,VKPerigee,CovPerigee);
                    if(sc.isSuccess()) {
                      const Trk::Track* TT = m_fitSvc->CreateTrkTrack(VKPerigee,CovPerigee); 
                      ImpactSignif=m_fitSvc->VKalGetImpact(TT, PrimVrt.position(), 0, Impact, ImpactError);
                      if(m_FillHist){m_hb_impV0->Fill( ImpactSignif, m_w_1); }
	              if(ImpactSignif>3.5) BadTracks=0;
		      delete TT;
	            } else { BadTracks=0;}
	         }  // else { BadTracks=0;}
               }
             }
//
//  Check interactions on pixel layers
//
	     float xvt=FitVertex.x(); float yvt=FitVertex.y();
             if(m_FillHist){m_hb_r2d->Fill( Dist2D, m_w_1);}
             if(m_useMaterialRejection){
              //if(m_materialMap){
              //  if(m_materialMap->inMaterial(FitVertex)) BadTracks=4;
              //  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" MaterialMap test="<< BadTracks<<endreq;
	      //}else{   
               float Dist2DBP=sqrt( (xvt-m_Xbeampipe)*(xvt-m_Xbeampipe) + (yvt-m_Ybeampipe)*(yvt-m_Ybeampipe) ); 
               float Dist2DBL=sqrt( (xvt-m_XlayerB)*(xvt-m_XlayerB) + (yvt-m_YlayerB)*(yvt-m_YlayerB) ); 
               float Dist2DL1=sqrt( (xvt-m_Xlayer1)*(xvt-m_Xlayer1) + (yvt-m_Ylayer1)*(yvt-m_Ylayer1) );
               float Dist2DL2=sqrt( (xvt-m_Xlayer2)*(xvt-m_Xlayer2) + (yvt-m_Ylayer2)*(yvt-m_Ylayer2) );
               if(m_existIBL){              // 4-layer pixel detector
                 if( fabs(Dist2DBP-m_Rbeampipe)< 1.0)  BadTracks = 4;           // Beam Pipe removal  
                 if( fabs(Dist2DBL-m_RlayerB)  < 2.5)  BadTracks = 4;
                 if( fabs(Dist2DL1-m_Rlayer1)  < 3.0)  BadTracks = 4;
                 if( fabs(Dist2DL2-m_Rlayer2)  < 3.0)  BadTracks = 4;
                 if( fabs(Dist2DL2-m_Rlayer3)  < 3.0)  BadTracks = 4;
               }else{                       // 3-layer pixel detector
                 if( fabs(Dist2DBP-m_Rbeampipe)< 1.5)  BadTracks = 4;           // Beam Pipe removal  
                 if( fabs(Dist2DBL-m_RlayerB)  < 3.5)  BadTracks = 4;
                 if( fabs(Dist2DL1-m_Rlayer1)  < 4.0)  BadTracks = 4;
                 if( fabs(Dist2DL2-m_Rlayer2)  < 5.0)  BadTracks = 4;
               }  //}
             }


	     if(m_useITkMaterialRejection){
	       float zvt = FitVertex.z(); float Rvt = sqrt(xvt*xvt+yvt*yvt);
	       int bin = m_ITkPixMaterialMap->FindBin(zvt,Rvt);
	       if(m_ITkPixMaterialMap->GetBinContent(bin)>0) BadTracks = 4;
	     }

//
//  Creation of tracks from V0 list
//	     
	     if( BadTracks ){
	        TrkFromV0.push_back(SelectedTracks[i]);
	        TrkFromV0.push_back(SelectedTracks[j]);
	     }else{
		double limPtTr=m_JetPtFractionCut*JetDir.Perp();
                if(TrackPt[i]<limPtTr && Signif3D<9.)continue;
                if(TrackPt[j]<limPtTr && Signif3D<9.)continue;
//
  		double JetVrtDir =
   	          JetDir.Px()*(FitVertex.x()-PrimVrt.x())
                + JetDir.Py()*(FitVertex.y()-PrimVrt.y())
 		+ JetDir.Pz()*(FitVertex.z()-PrimVrt.z());
                if(m_getNegativeTail) JetVrtDir=fabs(JetVrtDir);  // For negative TAIL
  	                                                          // accepts also negative
  	                                                          // track pairs
                if(m_getNegativeTag) JetVrtDir=-JetVrtDir;        // For negative TAG
  	                                                          // accepts only negative
  	                                                          // track pairs
	        if( (Signif3D>m_Sel2VrtSigCut) && (JetVrtDir>0) ) {
                  if(fabs(Signif3Dproj)<m_Sel2VrtSigCut)continue;
	          ListSecondTracks.push_back(SelectedTracks[i]);
	          ListSecondTracks.push_back(SelectedTracks[j]);
	          closeVrtSig.push_back(Signif3D);
                }
	     }
         }
      } 
//
//-- Post-selection cleaning 
//
      if(ListSecondTracks.size()==1 && closeVrtSig[0]<9.){    //Turned off for the moment (ListSecondTracks.size() can't be 1!)
        auto it0=std::find(SelectedTracks.begin(),SelectedTracks.end(),ListSecondTracks[0]);
        auto it1=std::find(SelectedTracks.begin(),SelectedTracks.end(),ListSecondTracks[1]);
	int eGood=1; double cutNTrkDep=cutNDepNorm(m_NRefTrk,0.05);                        // NTrk dependence
        if(it0!=SelectedTracks.end() && pow(TrackSignifBase[it0-SelectedTracks.begin()],2.) < cutNTrkDep )  eGood=0;
        if(it1!=SelectedTracks.end() && pow(TrackSignifBase[it1-SelectedTracks.begin()],2.) < cutNTrkDep )  eGood=0;
	if(!eGood)ListSecondTracks.clear();
      }
      //--------------------------------------------------------------------
      if(ListSecondTracks.size()>0 ){ 
        if(m_FillHist){ m_pr_effVrt2tr->Fill((float)m_NRefTrk,1.);
                      //m_pr_effVrt2trEta->Fill( JetDir.Eta(),1.);}
                        m_pr_effVrt2trEta->Fill( JetDir.Eta(),(double)ListSecondTracks.size()/2.);}
	//----- Only vertices with unique tracks are considered as bad
	if(TrkFromV0.size()){
	  std::vector<const Track*> tmpVec(0);
          for(int tk=0;tk<(int)TrkFromV0.size(); tk+=2){
            int nFound1=std::count(ListSecondTracks.begin(),ListSecondTracks.end(),TrkFromV0[tk]);
            int nFound2=std::count(ListSecondTracks.begin(),ListSecondTracks.end(),TrkFromV0[tk+1]);
	    if(nFound1+nFound2){
	       ListSecondTracks.push_back(TrkFromV0[tk]);
	       ListSecondTracks.push_back(TrkFromV0[tk+1]);
	    }else{
	       tmpVec.push_back(TrkFromV0[tk]);
	       tmpVec.push_back(TrkFromV0[tk+1]);
            }
          }
          TrkFromV0.swap(tmpVec);
	}
      } else if(ListSecondTracks.size()==0) { if(m_FillHist){m_pr_effVrt2tr->Fill((float)m_NRefTrk,0.);
                                                             m_pr_effVrt2trEta->Fill(JetDir.Eta(),0.); }}
/////////// Attempt to find iasolated tracks with high impact parameter. They are RARE!!! No worth to use them!
//        TLorentzVector psum,tmp; int nprt=0;
//        for (i=0; i<NTracks; i++) {
//          if( TrackSignif[i]<2.*SelLim || signTrk[i]<0) continue;
//     auto it0=std::find(ListSecondTracks.begin(),ListSecondTracks.end(),SelectedTracks[i]); if( it0!=ListSecondTracks.end() )continue;
//          it0=std::find(TrkFromV0.begin(),TrkFromV0.end(),SelectedTracks[i]);               if( it0!=TrkFromV0.end() )continue;
//          it0=std::find(ListCloseTracks.begin(),ListCloseTracks.end(),SelectedTracks[i]);   if( it0!=ListCloseTracks.end() )continue;
//          int ibl,bl,l1,nlay; getPixelLayers(SelectedTracks[i], ibl, bl, l1, nlay); if(ibl+bl<2)continue; 
//          nprt++; tmp.SetPtEtaPhiM(SelectedTracks[i]->pt(),SelectedTracks[i]->eta(),SelectedTracks[i]->phi(),m_massPi); psum += tmp;
//        } if(nprt<1)return;
//        if(nprt==1) { tmp.SetPtEtaPhiM(psum.Pt(),JetDir.Eta(),JetDir.Phi(),m_massPi); psum += tmp; }
//        if(m_FillHist)m_hb_massPiPi1->Fill( psum.M(), m_w_1);
/////////////////////////////////////
      return;
   }




   template <class Track>
   bool InDetVKalVxInJetTool::Check2TrVertexInPixel( const Track* p1, const Track* p2,
                                              Amg::Vector3D &FitVertex, double Signif3D)
   const
   {
	int blTrk[2]={0,0}; 
	int l1Trk[2]={0,0}; 
	int l2Trk[2]={0,0}; 
	int nLays[2]={0,0}; 
        getPixelLayers( p1, blTrk[0] , l1Trk[0], l2Trk[0], nLays[0] );
        getPixelLayers( p2, blTrk[1] , l1Trk[1], l2Trk[1], nLays[1] );    // Very close to PV. Both b-layer hits are mandatory.
        if( Signif3D<15. && FitVertex.perp()<15. ){
	   if( blTrk[0]<1  && l1Trk[0]<1  )  return false;
	   if( blTrk[1]<1  && l1Trk[1]<1  )  return false;
	   if( blTrk[0]==0 && blTrk[1]==0 )  return false;
        }
        double xDif=FitVertex.x()-m_XlayerB, yDif=FitVertex.y()-m_YlayerB ; 
        double Dist2DBL=sqrt(xDif*xDif+yDif*yDif);
        if      (Dist2DBL < m_RlayerB-m_SVResolutionR){       //----------------------------------------- Inside B-layer
          if(blTrk[0]==0 && blTrk[1]==0) return false;  // No b-layer hits at all
          if(  nLays[0]           <2 )   return false;  // Less than 2 layers on track 0
          if(  nLays[1]           <2 )   return false;  // Less than 2 layers on track 1
	  return true;
        }else if(Dist2DBL > m_RlayerB+m_SVResolutionR){      //----------------------------------------- Outside b-layer
          if( blTrk[0]>0 || blTrk[1]>0 ) return false;  // Hit in b-layer is present
        }
// 
// L1 and L2 are considered only if vertex is in acceptance
//
	if(fabs(FitVertex.z())<400.){
          xDif=FitVertex.x()-m_Xlayer1, yDif=FitVertex.y()-m_Ylayer1 ;
	  double Dist2DL1=sqrt(xDif*xDif+yDif*yDif);
          xDif=FitVertex.x()-m_Xlayer2, yDif=FitVertex.y()-m_Ylayer2 ; 
	  double Dist2DL2=sqrt(xDif*xDif+yDif*yDif);
          if      (Dist2DL1 < m_Rlayer1-m_SVResolutionR) {   //------------------------------------------ Inside 1st-layer
	     if( l1Trk[0]==0 && l1Trk[1]==0 )     return false;  // No L1 hits at all
             if( l1Trk[0]<1  && l2Trk[0]<1  )     return false;  // Less than 1 hits on track 0
             if( l1Trk[1]<1  && l2Trk[1]<1  )     return false;  // Less than 1 hits on track 1
             return true;
          }else if(Dist2DL1 > m_Rlayer1+m_SVResolutionR) {  //------------------------------------------- Outside 1st-layer
	     if( l1Trk[0]>0 || l1Trk[1]>0 )       return false;  //  L1 hits are present
          }
          
          if      (Dist2DL2 < m_Rlayer2-m_SVResolutionR) {  //------------------------------------------- Inside 2nd-layer
	     if( (l2Trk[0]+l2Trk[1])==0 )  return false;           // At least one L2 hit must be present
          }else if(Dist2DL2 > m_Rlayer2+m_SVResolutionR) {  
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

    


    template <class Track>
    double InDetVKalVxInJetTool::RemoveNegImpact(std::vector<const Track*>  & inTracks,
                                                 const xAOD::Vertex        & PrimVrt,
	                                         const TLorentzVector      & JetDir,
					         double Limit)
    const
    {
      int blTrk=0, l1Trk=0, l2Trk=0, nLays=0;
      int i=0;
      while( i<(int)inTracks.size()){
        getPixelLayers( inTracks[i], blTrk , l1Trk, l2Trk, nLays );
        //if(nLays<1 || inTracks[i]->pt()>JetDir.Pt()) inTracks.erase(inTracks.begin()+i); //bad track: no pixel hit OR trk_pt>jet_pt
        if(nLays<1) inTracks.erase(inTracks.begin()+i); //bad track: no pixel hit
        else        i++;
      }
//----
      int NTracks = inTracks.size();
      std::vector<double> ImpR(NTracks);
      std::vector<double> Impact,ImpactError;
      AmgVector(5) tmpPerigee; tmpPerigee<<0.,0.,0.,0.,0.;
      double maxImp=-1.e10;
      for (i=0; i<NTracks; i++) {
         m_fitSvc->VKalGetImpact(inTracks[i], PrimVrt.position(), 1, Impact, ImpactError);
         tmpPerigee = GetPerigee(inTracks[i])->parameters(); 
         if( sin(tmpPerigee[2]-JetDir.Phi())*Impact[0] < 0 ){ Impact[0] = -fabs(Impact[0]);}
	                                                else{ Impact[0] =  fabs(Impact[0]);}
         if(  (tmpPerigee[3]-JetDir.Theta())*Impact[1] < 0 ){ Impact[1] = -fabs(Impact[1]);}
	                                                else{ Impact[1] =  fabs(Impact[1]);}
	 double SignifR = Impact[0]/ sqrt(ImpactError[0]); ImpR[i]=SignifR;
         if(fabs(SignifR)   < m_AntiPileupSigRCut) {   // cut against tracks from pileup vertices  
           if( fabs(Impact[1])/sqrt(ImpactError[2]) > m_AntiPileupSigZCut ) ImpR[i]=-9999.;  
         }
	 if(ImpR[i]>maxImp)maxImp=ImpR[i];
      }
      if(maxImp<Limit){  inTracks.clear(); return maxImp;}
      double rmin=1.e6;
      do{ rmin=1.e6; int jpm=0; 
          for(i=0; i<(int)ImpR.size(); i++){ if(rmin>ImpR[i]){rmin=ImpR[i]; jpm=i;}}; if(rmin>Limit)continue;
          ImpR.erase(ImpR.begin()+jpm); inTracks.erase(inTracks.begin()+jpm);
      }while(rmin<=Limit);
      return maxImp;
    }


}  //end of namespace
