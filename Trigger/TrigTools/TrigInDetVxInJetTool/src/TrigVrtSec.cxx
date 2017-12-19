/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrigInDetVxInJetTool/TrigInDetVxInJetTool.h"
//-------------------------------------------------
// Other stuff
#include  "AnalysisUtils/AnalysisMisc.h"
#include  "TrigInDetEvent/TrigInDetTrack.h"
#include  "TrigInDetEvent/TrigVertex.h"
#include  "VxVertex/RecVertex.h"
#include  "TMath.h"

#include<iostream>


//----------------------------------------------------------------------------------------
//  GetVrtSec resurns the vector Results with the following
//   0) Vertex mass
//   1) Vertex/jet energy fraction
//   2) Number of initially selected 2tr vertices
//   3) Number of selected for vertexing tracks in jet 
//   4) Number of track in secondary vertex
//   5) 3D SV-PV significance with sign
//   6) Maximal track Pt with respect to jet axis
//   7) Jet energy used in (2) calculation 
//---------------------------------------------------------------------------------------- 


TrigVertex* TrigInDetVxInJetTool::GetVrtSec(const std::vector<const TrigInDetTrack*>& InpTrk,
                                            const Trk::RecVertex                    & PrimVrt,
                                            const CLHEP::HepLorentzVector           & JetDir,
                                            std::vector<double>                     & Results,
                                            std::vector<const TrigInDetTrack*>      & ListSecondTracks,
                                            std::vector<const TrigInDetTrack*>      & TrkFromV0) {
  
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "GetVrtSec() called # tracks = " << InpTrk.size() << endmsg;
   
  std::vector<const TrigInDetTrack*> SelectedTracks;
  SelectedTracks.clear();
  ListSecondTracks.clear();
  Results.clear();        

  //* Reason "# ini trks = 0,1" *//
  if( InpTrk.size() < 2 ) { 
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stop SV fitting routine. Reason '# ini trks = 0,1'" << endmsg;
    m_cutFlowTrkSel.push_back(1); return 0; 
  }

  int NPVParticle = 0;
  if (m_doTrackSelection)
    NPVParticle = SelGoodTrigTrack( InpTrk, PrimVrt, JetDir, SelectedTracks);
  else
    SelectedTracks = InpTrk;

  long int NTracks = (int) (SelectedTracks.size());
      
  //* Reason "# sel trks = 0,1" *//
  if( NTracks < 2 ) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stop SV fitting routine. Reason '# sel trks = 0,1'" << endmsg; 
    m_cutFlowTrkSel.push_back(1); return 0; 
  }

  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Number of selected tracks inside jet = " << NTracks << endmsg;
      
  CLHEP::HepLorentzVector MomentumJet = TotalMom(SelectedTracks);


  //--------------------------------------------------------------------------------------------	 
  //                    Initial TrigInDetTrack list ready

  std::vector<const TrigInDetTrack*>  TracksForFit;
  std::vector<double> InpMass; 
  for(int i=0; i<NTracks; i++){InpMass.push_back(m_massPi);}

  Select2TrVrt(SelectedTracks, TracksForFit, PrimVrt, JetDir, InpMass, TrkFromV0, ListSecondTracks);

  //
  //--- Cleaning
  // 
  std::vector<const TrigInDetTrack*>::iterator   TransfEnd ;
  if( TrkFromV0.size() > 1) {
    sort(TrkFromV0.begin(),TrkFromV0.end());
    TransfEnd =  unique(TrkFromV0.begin(),TrkFromV0.end());
    TrkFromV0.erase( TransfEnd, TrkFromV0.end());
    // sort(TrkFromV0.begin(),TrkFromV0.end(),TrigInDetVxInJetTool::sortbyPtPredicate); TODO: is it needed?
  }

  double Vrt2TrackNumber = (double) ListSecondTracks.size()/2.;
  sort(ListSecondTracks.begin(),ListSecondTracks.end());
  TransfEnd =  unique(ListSecondTracks.begin(),ListSecondTracks.end());
  ListSecondTracks.erase( TransfEnd, ListSecondTracks.end());
  // sort(ListSecondTracks.begin(),ListSecondTracks.end(),TrigInDetVxInJetTool::sortbyPtPredicate); TODO: is it needed?

  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "Found different tracks in pairs = "<< ListSecondTracks.size() << endmsg;

  //* Reason "# 2vrt trk = 0,1" *//
  if(ListSecondTracks.size() < 2) { 
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stop SV fitting routine. Reason '# 2vrt trk = 0,1'" << endmsg; 
    m_cutFlowTrkSel.push_back(2); return 0;
  }

  //
  //-----------------------------------------------------------------------------------------------------
  //            Secondary track list is ready
  //            Now common vertex fit
  //
  std::vector<const TrigInDetTrack*>::const_iterator   i_ntrk, i_found;
  Amg::Vector3D FitVertex;
  std::vector<double> ErrorMatrix;
  std::vector< std::vector<double> > TrkAtVrt; 
  CLHEP::HepLorentzVector Momentum;
  double Dist3D=0, Signif3D=0, Signif3DP=0, Signif3DS=0;
  std::vector<double> Impact,ImpactError;

  double Chi2 =  FitCommonVrt( ListSecondTracks, PrimVrt, InpMass, FitVertex, ErrorMatrix, Momentum, TrkAtVrt);

  //* Reason "vtx not reconstr." *//
  if( Chi2 < 0) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stop SV fitting routine. Reason 'vtx not reconstr.'" << endmsg; 
    m_cutFlowTrkSel.push_back(3); return 0; 
  }

  // Check jet tracks not in secondary vertex
  std::vector<const TrigInDetTrack*> AdditionalTracks;
  std::vector<const TrigInDetTrack*> AdditionalTracksNotSelected;
  VrtVrtDist(PrimVrt, FitVertex, ErrorMatrix, Signif3D);

  if(Signif3D>8.){
    for (i_ntrk = SelectedTracks.begin(); i_ntrk < SelectedTracks.end(); ++i_ntrk) {
      i_found = find( ListSecondTracks.begin(), ListSecondTracks.end(), (*i_ntrk));
      if( i_found != ListSecondTracks.end() ) continue;
      Signif3DS = m_fitSvc->VKalGetImpact((*i_ntrk), FitVertex         , 1, Impact, ImpactError);
      if( Signif3DS > 10.) {AdditionalTracksNotSelected.push_back(*i_ntrk); continue;}
      Signif3DP = m_fitSvc->VKalGetImpact((*i_ntrk), PrimVrt.position(), 1, Impact, ImpactError);
      //if(m_FillHist){ m_hb_diffPS->Fill( Signif3DP-Signif3DS, w_1); }
      if(Signif3DP-Signif3DS>1.0) AdditionalTracks.push_back((*i_ntrk));
      else AdditionalTracksNotSelected.push_back((*i_ntrk));
    }
  }

  //* Reason "vtx not reconstr." *//
  if( Chi2 < 0) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stop SV fitting routine. Reason 'vtx not reconstr.'" << endmsg;  
    m_cutFlowTrkSel.push_back(3); return 0;
  }

  // Find highest track Pt with respect to jet direction
  // double trackPt;
  double trackPtMax=0.;
  // for (int tr=0; tr<(int)ListSecondTracks.size(); tr++) {
    // trackPt=pTvsDir(JetDir.vect() , TrkAtVrt[tr]); if(trackPt>trackPtMax)trackPtMax=trackPt;
  // }

  // OFF for now; this gives gain in rejection at expense of lowering starting b-jet efficiency
  // (just use the commented line instead to enable)
  if( ListSecondTracks.size()==0 ) { 
  // if( ListSecondTracks.size()==2 ) {         // If there are 2 only tracks

    int Charge=0, tCnt=0;
    int blTrk[2]={0,0}; 
    int l1Trk[2]={0,0}; 
    int l2Trk[2]={0,0}; 
    int BLshared=0;
    int PXshared=0;

    std::vector<const TrigInDetTrack*>::const_iterator   i_ntrk;
    for (i_ntrk = ListSecondTracks.begin(); i_ntrk < ListSecondTracks.end(); ++i_ntrk) {

      if (!((*i_ntrk)->HitPattern())) continue;

      Charge += (*i_ntrk)->param()->pT()>0 ? 1 : -1;

      blTrk[tCnt] = ((*i_ntrk)->HitPattern() & 0x1) ? 1 : 0;
      l1Trk[tCnt] = ((*i_ntrk)->HitPattern() & 0x2) ? 1 : 0;
      l2Trk[tCnt] = ((*i_ntrk)->HitPattern() & 0x4) ? 1 : 0;
      
      tCnt++;
    }

    double xDif=FitVertex.x()-m_XlayerB, yDif=FitVertex.y()-m_YlayerB ; 
    double Dist2D=sqrt(xDif*xDif+yDif*yDif);

    if (Dist2D < m_RlayerB) {

      //* Reason "inside bl but no bl hits" *//
      if( blTrk[0]==0 || blTrk[1]==0 ) {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stop SV fitting routine. Reason 'inside bl but no bl hits'" << endmsg;   
	m_cutFlowTrkSel.push_back(4); return 0;
      }

      if(BLshared>m_CutSharedHits) return 0;
    } else {

      //* Reason "outside bl but no bl hits" *//
      if( blTrk[0]==1 || blTrk[1]==1 ) {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stop SV fitting routine. Reason 'inside bl but no bl hits'" << endmsg;   
	m_cutFlowTrkSel.push_back(5); return 0;
      }

      if(PXshared>m_CutSharedHits) return 0;
    }

    // Only if vertex is in acceptance
    if(fabs(FitVertex.z())<400.) { 

      xDif=FitVertex.x()-m_Xlayer1, yDif=FitVertex.y()-m_Ylayer1 ; 
      Dist2D=sqrt(xDif*xDif+yDif*yDif);

      //* Reason "inside l1 but no l1 hits" *//
      if (Dist2D < m_Rlayer1) {  
	if( l1Trk[0]==0 || l1Trk[1]==0 ) { 
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stop SV fitting routine. Reason 'inside l1 but no l1 hits'" << endmsg;   
	  m_cutFlowTrkSel.push_back(6); return 0;
	}
      } else {  

	//* Reason "outside l1 but has l1 hits" *//
	if( l1Trk[0]==1 || l1Trk[1]==1 ) { 
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stop SV fitting routine. Reason 'inside l1 but no l1 hits'" << endmsg;   
	  m_cutFlowTrkSel.push_back(7); return 0;
	}
      }  // Outside 1st-layer

      xDif=FitVertex.x()-m_Xlayer2, yDif=FitVertex.y()-m_Ylayer2 ; 
      Dist2D=sqrt(xDif*xDif+yDif*yDif);

      //* Reason "inside l2 but no l2 hits" *//
      if (Dist2D < m_Rlayer2) {  
	if( l2Trk[0]==0 || l2Trk[1]==0 ) {
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stop SV fitting routine. Reason 'inside l2 but no l2 hits'" << endmsg;   
	  m_cutFlowTrkSel.push_back(8); return 0;
	}
      } else {

	//* Reason "outside l2 but has l2 hits" *//
	if( l2Trk[0]==1 || l2Trk[1]==1 ) {
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stop SV fitting routine. Reason 'inside l2 but no l2 hits'" << endmsg;   
	  m_cutFlowTrkSel.push_back(9); return 0;
	}
      }
    }

    // Final rejection of K0
    if( !Charge && fabs(Momentum.m()-m_massK0)<15. ) {
	  TrkFromV0.push_back(ListSecondTracks[0]);
	  TrkFromV0.push_back(ListSecondTracks[1]);
      if( TrkFromV0.size() > 2) {
        sort(TrkFromV0.begin(),TrkFromV0.end());
        TransfEnd =  unique(TrkFromV0.begin(),TrkFromV0.end());
        TrkFromV0.erase( TransfEnd, TrkFromV0.end());
        // sort(ListSecondTracks.begin(),ListSecondTracks.end(),TrigInDetVxInJetTool::sortbyPtPredicate);
      }

      //* Reason "K0 rejection" *//
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stop SV fitting routine. Reason 'K0 rejection'" << endmsg;   
      m_cutFlowTrkSel.push_back(10); return 0;
    }

    //-- Protection against fake vertices far from interaction point 
    if(NPVParticle<1)NPVParticle=1;
    double vvdist3D=VrtVrtDist(PrimVrt, FitVertex, ErrorMatrix, Signif3D);
    double t3Dimp1= m_fitSvc->VKalGetImpact(ListSecondTracks[0], PrimVrt.position(), 1, Impact, ImpactError)/fabs(TrkAtVrt[0][2]);
    double t3Dimp2= m_fitSvc->VKalGetImpact(ListSecondTracks[1], PrimVrt.position(), 1, Impact, ImpactError)/fabs(TrkAtVrt[1][2]);
    double selVar=(t3Dimp1<t3Dimp2 ? t3Dimp1 : t3Dimp2)/sqrt((double)NPVParticle)/vvdist3D/500.;

    //* Reason "AntiFake2trVrtCut" *//
    if(selVar<m_AntiFake2trVrtCut) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stop SV fitting routine. Reason 'AntiFake2trVrtCut'" << endmsg;   
      m_cutFlowTrkSel.push_back(11); return 0;
    }
  }


  double JetVrtDir =
    JetDir.px()*(FitVertex.x()-PrimVrt.position().x())
    + JetDir.py()*(FitVertex.y()-PrimVrt.position().y())
    + JetDir.pz()*(FitVertex.z()-PrimVrt.position().z());

  if(  m_getNegativeTag ) {

    //* Reason "getNegTag but pos. sig." *//
    if( JetVrtDir>0. ) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stop SV fitting routine. Reason 'getNegTag but pos. sig.'" << endmsg;   
      m_cutFlowTrkSel.push_back(11); return 0;
    }

  } else if( m_getNegativeTail ) { 
    ;
  } else { 

    //* Reason "not NegTail but neg. sig." *//
    if( JetVrtDir<0. ) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stop SV fitting routine. Reason 'not NegTail but neg. sig.'" << endmsg;   
      m_cutFlowTrkSel.push_back(11); return 0; 
    }
  } 

  Results.push_back(Momentum.m());

  double eRatio = Momentum.e()/MomentumJet.e(); 
  Results.push_back(eRatio<0.99999 ? eRatio : 0.99999);        //1st
  Results.push_back(Vrt2TrackNumber);                          //2nd
  Results.push_back((double)NTracks);                          //3rd
  Results.push_back((double)ListSecondTracks.size());          //4th
  Dist3D=VrtVrtDist(PrimVrt, FitVertex, ErrorMatrix, Signif3D);
  if(JetVrtDir < 0) Signif3D = -Signif3D;
  Results.push_back(Signif3D);                                 //5th
  Results.push_back(trackPtMax);                               //6th
  Results.push_back(MomentumJet.e());                          //7th

  double covVrt[6]={ErrorMatrix[0],ErrorMatrix[1],ErrorMatrix[2],ErrorMatrix[3],ErrorMatrix[4],ErrorMatrix[5]};
  TrackInVertexList* tmpTrkInVrt = new TrackInVertexList();
  for(int tt=0; tt<(int)ListSecondTracks.size(); tt++)
    tmpTrkInVrt->push_back(ListSecondTracks[tt]);

  TrigVertex * tmpVx=new TrigVertex(FitVertex.x(),FitVertex.y(),FitVertex.z(),
                                    covVrt, Chi2, (int)(ListSecondTracks.size()*2-3), tmpTrkInVrt,
                                    Results[0], /* mass */
                                    Results[1], /* energy fraction */
                                    (int)Vrt2TrackNumber,
                                    Dist3D,
                                    Signif3D,
                                    TrigVertex::BJETSECVTXID);
  //===================================================

  return tmpVx;
}



//
//--------------------------------------------------------
//  Routine for global secondary vertex fitting
//

template <class Track>
double TrigInDetVxInJetTool::FitCommonVrt(std::vector<const Track*>          & ListSecondTracks,
                                          const Trk::RecVertex               & PrimVrt,
                                          std::vector<double>                & InpMass, 
                                          Amg::Vector3D                      & FitVertex,
                                          std::vector<double>                & ErrorMatrix,
                                          CLHEP::HepLorentzVector            & Momentum,
                                          std::vector< std::vector<double> > & TrkAtVrt)
{

  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "FitCommonVrt() called " <<ListSecondTracks.size()<< endmsg;
  //preparation
  StatusCode sc;
  std::vector<double> Chi2PerTrk;
  long int           Charge;
  double             Chi2 = 0.;
  ;
  int Outlier=1, i=0;
  //
  // Start of fit
  //
  m_fitSvc->setDefault();
  m_fitSvc->setMassInputParticles( InpMass );            // Use pions masses
  m_fitSvc->setMomCovCalc(1);  /* Total momentum and its covariance matrix are calculated*/
  sc=VKalVrtFitFastBase(ListSecondTracks,FitVertex);          /* Fast crude estimation */
  if(sc.isFailure() || FitVertex.perp() > m_Rlayer2*2. ) {    /* No initial estimation */ 
    m_fitSvc->setApproximateVertex(PrimVrt.position().x(),   /* Use as starting point */
                                   PrimVrt.position().y(),
                                   PrimVrt.position().z()); 
  } else {
    m_fitSvc->setApproximateVertex(FitVertex.x(),FitVertex.y(),FitVertex.z()); /*Use as starting point*/
  }
  if(m_RobustFit)m_fitSvc->setRobustness(m_RobustFit);
  //fit itself
  int NTracksVrt = ListSecondTracks.size(); double FitProb=0.;
  for (i=0; i < NTracksVrt-1; i++) {
    sc=VKalVrtFitBase(ListSecondTracks,FitVertex, Momentum,Charge,
                      ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2);
    if(sc.isFailure() ||  Chi2 > 1000000. ) { return -10000.;}    // No fit
    Outlier = FindMax( Chi2PerTrk ); 
    FitProb=TMath::Prob( Chi2, 2*ListSecondTracks.size()-3);
    if(ListSecondTracks.size() == 2 )                break;         // Only 2 tracks left
    if( Momentum.m() <6000. && FitProb > 0.001) {
      if( Chi2PerTrk[Outlier] < m_SecTrkChi2Cut*m_chiScale[ListSecondTracks.size()<10?ListSecondTracks.size():10])  break;  // Solution found
    }
    RemoveEntryInList(ListSecondTracks,Outlier);
    m_fitSvc->setApproximateVertex(FitVertex.x(),FitVertex.y(),FitVertex.z()); /*Use as starting point*/
  }
  //--
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" SecVrt fit converged="<< ListSecondTracks.size()<<", "
                                       <<Chi2<<", "<<Chi2PerTrk[Outlier]<<" Mass="<<Momentum.m()<<endmsg;
  //--
  if( ListSecondTracks.size()==2 ){
    if( Momentum.m() > 6000. || FitProb < 0.001 || Chi2PerTrk[Outlier] > m_SecTrkChi2Cut) { return -10000.;  }  
  } 
  //
  //-- To kill remnants of conversion
  double Dist2D=sqrt(FitVertex.x()*FitVertex.x()+FitVertex.y()*FitVertex.y());
  if( ListSecondTracks.size()==2  && (Dist2D > 20.) && Charge==0 ) {      
    double mass_EE   =  massV0( TrkAtVrt,m_massE,m_massE);
    //if(m_FillHist){m_hb_totmassEE->Fill( mass_EE, w_1); }
    if( mass_EE < 100. ) return -40.;
  }
  return Chi2;
}



//
//
//--------------------------------------------------------
//   Template routine for 2track secondary vertices selection
//

template <class Track>
void TrigInDetVxInJetTool::Select2TrVrt(std::vector<const Track*>     & SelectedTracks,
                                        std::vector<const Track*>     & TracksForFit,
                                        const Trk::RecVertex          & PrimVrt,
                                        const CLHEP::HepLorentzVector & JetDir,
                                        std::vector<double>           & InpMass, 
                                        std::vector<const Track*>     & TrkFromV0,
                                        std::vector<const Track*>     & ListSecondTracks) {

  StatusCode sc;
  sc.setChecked();
  Amg::Vector3D FitVertex, vDist;
  std::vector<double> ErrorMatrix,Chi2PerTrk,VKPerigee,CovPerigee;
  std::vector< std::vector<double> > TrkAtVrt; 
  CLHEP::HepLorentzVector Momentum;
  std::vector<double> Impact,ImpactError;
  double ImpactSignif=0;
  double             Chi2=0, Signif3D=0, Dist2D=0, JetVrtDir=0;
  long int           Charge;
  int i,j;
  //
  long int NTracks = (int) (SelectedTracks.size());

  //
  //  Impact parameters with sign calculations
  //
  double SignifR,SignifZ;
  std::vector<double> TrackSignif,TrackPt;
  int NPrimTrk=0, NSecTrk=0;
  for (i=0; i<NTracks; i++) {
    ImpactSignif = m_fitSvc->VKalGetImpact(SelectedTracks[i], PrimVrt.position(), 1, Impact, ImpactError);

    const TrigInDetTrackFitPar * tmpPerigee = SelectedTracks[i]->param(); 
    if( sin(tmpPerigee->phi0() - JetDir.phi())*Impact[0] < 0 ){ Impact[0] = -fabs(Impact[0]);}
    else{ Impact[0] =  fabs(Impact[0]);}
    double tTheta= 2.0*atan(exp(-(tmpPerigee->eta()))); 
    if(  (tTheta-JetDir.theta())*Impact[1] < 0 ){ Impact[1] = -fabs(Impact[1]);}
    else{ Impact[1] =  fabs(Impact[1]);}
    SignifR = Impact[0]/ sqrt(ImpactError[0]);
    SignifZ = Impact[1]/ sqrt(ImpactError[2]);
    //if(m_FillHist){
    //  m_hb_impactR->Fill( SignifR, w_1); 
    //  m_hb_impactZ->Fill( SignifZ, w_1); 
    //  m_hb_impactRZ->Fill(SignifR, SignifZ, w_1); 
    //}
    if(ImpactSignif < 3.) { NPrimTrk += 1;}
    else{NSecTrk += 1;}
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
      if(SignifZ >  m_AntiPileupSigZCut ) ImpactSignif=0.;  
      if(SignifZ < -m_AntiPileupSigZCut ) ImpactSignif=0.;  
    }
      
    TrackSignif.push_back(ImpactSignif); TrackPt.push_back( fabs(tmpPerigee->pT()) ) ;
    //if(m_FillHist){m_hb_impact->Fill( ImpactSignif, w_1);}
  }

  int NTrDiff = NPrimTrk-NSecTrk; if(NTrDiff < 1) NTrDiff=1;
  double SelLim = m_TrkSigCut;

  m_fitSvc->setDefault();
  m_fitSvc->setMassInputParticles( InpMass );     // Use pion masses for fit
  m_fitSvc->setMomCovCalc(1);                     // Total momentum and its covariance matrix are calculated
  for (i=0; i<NTracks-1; i++) {
    double adp1=TrackPt[i]/64000.; if(adp1<0.)adp1=0;if(adp1>1.)adp1=1.;adp1=sqrt(adp1)/2.;
  
    for (j=i+1; j<NTracks; j++) {

      double adp2=TrackPt[j]/64000.; if(adp2<0.)adp2=0;if(adp2>1.)adp2=1.;adp2=sqrt(adp2)/2.;

      //* Reason "trk_{1} dca/#sigma(dca) - adp <" *//
      if(TrackSignif[i] < SelLim+adp1) { m_cutFlowTwoTrkVtxSel.push_back(1); continue; }

      //* Reason "trk_{2} dca/#sigma(dca) - adp <" *//
      if(TrackSignif[j] < SelLim+adp2) { m_cutFlowTwoTrkVtxSel.push_back(2); continue; }

      //* Reason "#Sigma(dca/#sigma(dca))- #Sigma(adp) <" *//
      if(TrackSignif[i]+TrackSignif[j] < (2.*SelLim+m_TrkSigSumCut+adp1+adp2)) { m_cutFlowTwoTrkVtxSel.push_back(3); continue; }

      int BadTracks = 0;                                       //Bad tracks identification }
      TracksForFit.clear();
      m_fitSvc->setDefault();                          //Reset VKalVrt settings
      m_fitSvc->setMomCovCalc(1);                     // Total momentum and its covariance matrix are calculated
      TracksForFit.push_back( SelectedTracks[i] );
      TracksForFit.push_back( SelectedTracks[j] );

      sc=VKalVrtFitFastBase(TracksForFit,FitVertex);              /* Fast crude estimation*/
      
      if( sc.isFailure() || FitVertex.perp() > m_Rlayer2*2. ) {   /* No initial estimation */ 
        m_fitSvc->setApproximateVertex(PrimVrt.position().x(),   /*Use as starting point*/
				       PrimVrt.position().y(),
                                       PrimVrt.position().z()); 
      } else {
        vDist=FitVertex-PrimVrt.position();
        JetVrtDir = JetDir.px()*vDist.x() + JetDir.py()*vDist.y() + JetDir.pz()*vDist.z();
        if( JetVrtDir>0. ) {                           /* Good initial estimation */ 
          m_fitSvc->setApproximateVertex(FitVertex.x(),FitVertex.y(),FitVertex.z()); /*Use as starting point*/
        }else{
          m_fitSvc->setApproximateVertex(PrimVrt.position().x(), PrimVrt.position().y(), PrimVrt.position().z()); 
        }
      }

      sc=VKalVrtFitBase(TracksForFit,FitVertex, Momentum,Charge,
                        ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2);

      //* Reason "2vrt fit failed" *//
      if(sc.isFailure()) { m_cutFlowTwoTrkVtxSel.push_back(4); continue; }
      
      //* Reason "2vrt fit #chi^{2} >" *//
      if(Chi2 > m_Sel2VrtChi2Cut) { m_cutFlowTwoTrkVtxSel.push_back(5); continue; }

      vDist=FitVertex-PrimVrt.position();
      JetVrtDir = JetDir.px()*vDist.x() + JetDir.py()*vDist.y() + JetDir.pz()*vDist.z();
      double vPos=(vDist.x()*Momentum.px()+vDist.y()*Momentum.py()+vDist.z()*Momentum.pz())/Momentum.rho();
 
      //* Reason "2vrt behind p.v." *//
      if((!m_getNegativeTail) && (!m_getNegativeTag) && (JetVrtDir<0.) ) { m_cutFlowTwoTrkVtxSel.push_back(6); continue; }

      /* secondary vertex behind primary*/

      //* Reason "2vrt too far behind p.v." *//
      if(vPos<-150.) { m_cutFlowTwoTrkVtxSel.push_back(7); continue; }
      
      //
      // Check pixel hit - vertex position. If Rv>Rlayer->tracks must not have given layer hits
      int blTrk[2]={0,0};
      int l1Trk[2]={0,0};
      int l2Trk[2]={0,0};

      for (int tr = 0; tr < (int) TracksForFit.size(); tr++) {
	
        if (!(TracksForFit[tr]->HitPattern())) continue;
	
	blTrk[tr] = (TracksForFit[tr]->HitPattern() & 0x1) ? 1 : 0;
	l1Trk[tr] = (TracksForFit[tr]->HitPattern() & 0x2) ? 1 : 0;
	l2Trk[tr] = (TracksForFit[tr]->HitPattern() & 0x4) ? 1 : 0;

      }

      double xdf=FitVertex.x()-m_XlayerB;
      double ydf=FitVertex.y()-m_YlayerB;

      //* Reason "si hits pattern not compatible with 2vrt radius" *//
      m_cutFlowTwoTrkVtxSel.push_back(8); 

      if( sqrt(xdf*xdf+ydf*ydf) > (m_RlayerB+4.)  && (blTrk[0] || blTrk[1])) continue;
      if(fabs(FitVertex.z())<400.){ //Only if vertex is in acceptance
        xdf=FitVertex.x()-m_Xlayer1;
        ydf=FitVertex.y()-m_Ylayer1;
        if( sqrt(xdf*xdf+ydf*ydf) > (m_Rlayer1+4.)  && (l1Trk[0] || l1Trk[1])) continue;
        xdf=FitVertex.x()-m_Xlayer2;
        ydf=FitVertex.y()-m_Ylayer2;
        if( sqrt(xdf*xdf+ydf*ydf) > (m_Rlayer2+4.)  && (l2Trk[0] || l2Trk[1])) continue;
      }

      Dist2D=sqrt(FitVertex.x()*FitVertex.x()+FitVertex.y()*FitVertex.y()); 
      double Signif3Dproj=VrtVrtDist(PrimVrt, FitVertex, ErrorMatrix, JetDir);
      double mass_PiPi =  Momentum.m();  

      //* Reason "2vrt mass > 6000" - can't be from B decay *//
      if(mass_PiPi > 6000.) { m_cutFlowTwoTrkVtxSel.push_back(9); continue; }

      //* Reason "2vrt radius > 180" - can't be from B decay *//
      if(Dist2D > 180.) { m_cutFlowTwoTrkVtxSel.push_back(10); continue; } 

      //if(m_FillHist)m_hb_massPiPi2->Fill( mass_PiPi, w_1);

      //Check if V0 or material interaction on Pixel layer is present

      if( Charge == 0 && Signif3D>8. && mass_PiPi<900.) {

        double mass_PPi  =  massV0( TrkAtVrt,m_massP,m_massPi);
        double mass_EE   =  massV0( TrkAtVrt,m_massE,m_massE);

        //if(m_FillHist){m_hb_massEE->Fill( mass_EE, w_1);}

        if(       mass_EE <  40.)  { 
          BadTracks = 3;
        }else{

          //if(m_FillHist){m_hb_massPiPi->Fill( mass_PiPi, w_1);}     /* Total mass with input particles masses*/
          //if(m_FillHist){m_hb_massPPi->Fill( mass_PPi, w_1);} 

          if( fabs(mass_PiPi-m_massK0) < 22. )  BadTracks = 1;
          if( fabs(mass_PPi-m_massLam) <  8. )  BadTracks = 2;
        }
        //
        //  Creation on V0 tracks
        //
        if(BadTracks){
          std::vector<double> inpMass;
          m_fitSvc->setDefault();                     //Reset VKalVrt settings
          m_fitSvc->setMomCovCalc(1);                 //Total momentum and its covariance 
		                                              //matrix are calculated
		  if( BadTracks == 1 ) {  // K0 case
		    inpMass.push_back(m_massPi);inpMass.push_back(m_massPi);
            m_fitSvc->setMassInputParticles( inpMass );
            m_fitSvc->setMassForConstraint(m_massK0);
            m_fitSvc->setCnstType(1);       // Set mass  constraint
          }
		  if( BadTracks == 2 ) {  // Lambda case
            if( fabs(1./TrkAtVrt[0][2]) > fabs(1./TrkAtVrt[1][2]) ) {
              inpMass.push_back(m_massP);inpMass.push_back(m_massPi);
            }else{  inpMass.push_back(m_massPi);inpMass.push_back(m_massP); }
            m_fitSvc->setMassInputParticles( inpMass );
            m_fitSvc->setMassForConstraint(m_massLam);
            m_fitSvc->setCnstType(1);       // Set mass  constraint
          }
		  if( BadTracks == 3 ) {  // Gamma case
		    inpMass.push_back(m_massE);inpMass.push_back(m_massE);
            m_fitSvc->setMassInputParticles( inpMass );
            m_fitSvc->setCnstType(12);       // Set 3d angular constraint
          }
          m_fitSvc->setApproximateVertex(FitVertex.x(),FitVertex.y(),FitVertex.z()); 
          sc=VKalVrtFitBase(TracksForFit,FitVertex, Momentum,Charge,
                            ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2);
          if(sc.isSuccess()) {
            sc=m_fitSvc->VKalVrtCvtTool(FitVertex,Momentum,ErrorMatrix,0,VKPerigee,CovPerigee);
            if(sc.isSuccess()) {
              const TrigInDetTrack* TT = m_fitSvc->CreateTrigTrack(VKPerigee,CovPerigee); 
              ImpactSignif=m_fitSvc->VKalGetImpact(TT, PrimVrt.position(), 0, Impact, ImpactError);
              //if(m_FillHist){m_hb_impV0->Fill( ImpactSignif, w_1); }
              if(ImpactSignif>5.) BadTracks=0;
		      delete TT;
            } else { BadTracks=0;}
          } 
        }
      }
      
      //
      //  Check interactions on pixel layers
      //
      double xvt=FitVertex.x();
      double yvt=FitVertex.y();
      Dist2D=sqrt( (xvt-m_Xbeampipe)*(xvt-m_Xbeampipe) + (yvt-m_Ybeampipe)*(yvt-m_Ybeampipe) );

      if( fabs(Dist2D-m_Rbeampipe)< 1.5) BadTracks = 4;           // Beam Pipe removal
      //Dist2D=sqrt(pow(xvt-m_XlayerB,2.)+pow(yvt-m_YlayerB,2.)); 
      Dist2D=sqrt( (xvt-m_XlayerB)*(xvt-m_XlayerB) + (yvt-m_YlayerB)*(yvt-m_YlayerB) ); 
      if( fabs(Dist2D-m_RlayerB) < 3.5)  BadTracks = 4;
      //Dist2D=sqrt(pow(xvt-m_Xlayer1,2.)+pow(yvt-m_Ylayer1,2.)); 
      Dist2D=sqrt( (xvt-m_Xlayer1)*(xvt-m_Xlayer1) + (yvt-m_Ylayer1)*(yvt-m_Ylayer1) );
      if( fabs(Dist2D-m_Rlayer1) < 4.0)  BadTracks = 4;
      Dist2D=sqrt( (xvt-m_Xlayer2)*(xvt-m_Xlayer2) + (yvt-m_Ylayer2)*(yvt-m_Ylayer2) );
      if( fabs(Dist2D-m_Rlayer2) < 5.0)  BadTracks = 4;

      //* Reason "interaction with pixel layers" *//
      if (BadTracks == 4) m_cutFlowTwoTrkVtxSel.push_back(11); 
      //* Reason "matches K^{0}_{S},#Lambda^{0},#gamma" *//
      else if (BadTracks > 0) m_cutFlowTwoTrkVtxSel.push_back(12); 
                                
      //
      //  Creation of tracks from V0 list
      //
      if( BadTracks ){
        TrkFromV0.push_back(SelectedTracks[i]);
        TrkFromV0.push_back(SelectedTracks[j]);
        continue;
      }

      Dist2D=sqrt( (xvt-m_XlayerB)*(xvt-m_XlayerB) + (yvt-m_YlayerB)*(yvt-m_YlayerB) );

      int hBL1 = (TracksForFit[0]->HitPattern() & 0x1) ? 1 : 0;
      int hBL2 = (TracksForFit[1]->HitPattern() & 0x1) ? 1 : 0;

      if( Dist2D > (m_RlayerB+5.) &&  ( hBL1 || hBL2 ) ) continue;
      if( Dist2D < (m_RlayerB-5.) ) {
        if( hBL1==0 && hBL2==0  ) continue;
        if( Signif3D <10. && (hBL1*hBL2)==0)continue;
      }
      
      double JetVrtDir =
        JetDir.px()*(FitVertex.x()-PrimVrt.position().x())
        + JetDir.py()*(FitVertex.y()-PrimVrt.position().y())
        + JetDir.pz()*(FitVertex.z()-PrimVrt.position().z());
      if(m_getNegativeTail) JetVrtDir=fabs(JetVrtDir);  // For negative TAIL
      // accepts also negative track pairs
      if(m_getNegativeTag) JetVrtDir=-JetVrtDir;        // For negative TAG
      // accepts only negative track pairs

      if( (Signif3D>m_Sel2VrtSigCut) && (JetVrtDir>0) ) {

	//* Reason "L/#sigma(L)_{proj} <" *//
        if(Signif3Dproj<m_Sel2VrtSigCut) { m_cutFlowTwoTrkVtxSel.push_back(13); continue; }

        ListSecondTracks.push_back(SelectedTracks[i]);
        ListSecondTracks.push_back(SelectedTracks[j]);
      }
    }
  }
}




