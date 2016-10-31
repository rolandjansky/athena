/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "InDetSVWithMuonTool/InDetSVWithMuonTool.h"
#include "xAODBTagging/SecVtxHelper.h"
//-------------------------------------------------
#include  "AnalysisUtils/AnalysisMisc.h"
#include  "TMath.h"
//
#include<iostream>

namespace InDet{


  xAOD::Vertex* InDetSVWithMuonTool::MuonVrtSec(const std::vector<const xAOD::TrackParticle*>& InpTrk,
                                            const xAOD::Vertex                           & PrimVrt,
                                            const xAOD::TrackParticle                    * Muon,
                                            std::vector<const xAOD::TrackParticle*>      & ListTracksNearMuon)
  const
  {

      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "MuonVrtSec() called with xAOD::TrackParticle=" <<InpTrk.size()<< endreq;
      std::vector<const xAOD::TrackParticle*> SelectedTracks(0);
      ListTracksNearMuon.clear();      

      if( InpTrk.size() < 1 ) { return 0;} // 0,1 input track => nothing to do!
      SelGoodTrkParticle( InpTrk, PrimVrt, Muon, SelectedTracks);
      long int NTracks = SelectedTracks.size();
      TLorentzVector TrkJet = TotalMom(SelectedTracks);
      if(m_FillHist)m_hb_nseltrk->Fill( (double)NTracks, w_1);    
      if( NTracks < 1 ) { return 0;}      // 0,1 selected track => nothing to do!

      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Number of selected tracks dR-close to muon= " <<NTracks << endreq;

      if(m_FillHist)m_hb_muonPt->Fill( Muon->pt(), w_1);    

//--------------------------------------------------------------------------------------------	 
//                    Initial xAOD::TrackParticle list ready


      std::vector<const xAOD::TrackParticle*>  TracksForFit;
      std::vector<double> InpMass(NTracks,m_massPi);

      GetTrkWithMuonVrt(SelectedTracks, PrimVrt, Muon, ListTracksNearMuon);
      int nTracksNearMuon=ListTracksNearMuon.size();
//
//--- Cleaning
// 
      RemoveDoubleEntries(ListTracksNearMuon);
      AnalysisUtils::Sort::pT (&ListTracksNearMuon);      //no sorting for xAOD
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" Found different xAOD tracks crossing muon="<< ListTracksNearMuon.size()<<endreq;
      if(ListTracksNearMuon.size()==0) { return 0;} // Less than one track left

//
//-----------------------------------------------------------------------------------------------------
//            Secondary track list is ready
//            Now common vertex fit
//
      std::vector<const xAOD::TrackParticle*>::const_iterator   i_ntrk, i_found;
      Amg::Vector3D          FitVertex;
      std::vector<double> ErrorMatrix;
      std::vector< std::vector<double> > TrkAtVrt; 
      TLorentzVector    Momentum;
      std::vector<double> Impact,ImpactError;

      double Chi2 =  FitCommonVrt( ListTracksNearMuon, PrimVrt, Muon, FitVertex, ErrorMatrix, Momentum, TrkAtVrt);
      if( Chi2 < 0 && ListTracksNearMuon.size()>2 ) { // Vertex not reconstructed. Try to remove one track with biggest pt.
        double tpmax=0; int ipmax=-1;
        for(int it=0; it<(int)ListTracksNearMuon.size(); it++) if(tpmax<ListTracksNearMuon[it]->pt()){tpmax=ListTracksNearMuon[it]->pt(); ipmax=it;}
        if(ipmax>=0)RemoveEntryInList(ListTracksNearMuon,ipmax);
        Chi2 =  FitCommonVrt( ListTracksNearMuon,  PrimVrt, Muon, FitVertex, ErrorMatrix, Momentum, TrkAtVrt);
        if( Chi2 < 0 && ListTracksNearMuon.size()>2 ) { // Vertex not reconstructed. Try to remove another track with biggest pt.
          tpmax=0.; ipmax=-1;
          for(int it=0; it<(int)ListTracksNearMuon.size(); it++) if(tpmax<ListTracksNearMuon[it]->pt()){tpmax=ListTracksNearMuon[it]->pt(); ipmax=it;}
          if(ipmax>=0)RemoveEntryInList(ListTracksNearMuon,ipmax);
          Chi2 =  FitCommonVrt( ListTracksNearMuon, PrimVrt, Muon, FitVertex, ErrorMatrix, Momentum, TrkAtVrt);
        }
      }
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" FitCommonVrt result="<< Chi2<<endreq;
      if( Chi2 < 0) { return 0; }      // Vertex not reconstructed

//
//  Saving of results
//

     if(m_FillHist){
          m_hb_r2dc->Fill( FitVertex.perp(), w_1);    
          m_hb_totmass->Fill( Momentum.M(), w_1); 
          m_hb_nvrt2->Fill( (double)nTracksNearMuon, w_1);
      }

//-------------------------------------------------------------------------------------
//Return xAOD::Vertex
       xAOD::Vertex * tmpVertex=new xAOD::Vertex();
       tmpVertex->makePrivateStore();
       tmpVertex->setPosition(FitVertex);
       std::vector<float> floatErrMtx; floatErrMtx.resize(ErrorMatrix.size());
       for(int i=0; i<(int)ErrorMatrix.size(); i++) floatErrMtx[i]=ErrorMatrix[i];
       tmpVertex->setCovariance(floatErrMtx);
       tmpVertex->setFitQuality(Chi2, (float)(ListTracksNearMuon.size()*2.-3.));
       xAOD::SecVtxHelper::setVertexMass    (tmpVertex, Momentum.M()); 
       xAOD::SecVtxHelper::setVtxNtrk       (tmpVertex, nTracksNearMuon); 
       xAOD::SecVtxHelper::setEnergyFraction(tmpVertex, Momentum.E()/TrkJet.E()); 
       double Signif3D; VrtVrtDist(PrimVrt, FitVertex, ErrorMatrix, Signif3D);
       xAOD::SecVtxHelper::setVtxnormDist   (tmpVertex, Signif3D); 

       std::vector<Trk::VxTrackAtVertex> & tmpVTAV=tmpVertex->vxTrackAtVertex();    tmpVTAV.clear();
       for(int ii=0; ii<(int)ListTracksNearMuon.size(); ii++) {
         AmgSymMatrix(5) *CovMtxP=new AmgSymMatrix(5);   (*CovMtxP).setIdentity(); 
         Trk::Perigee * tmpMeasPer  =  new Trk::Perigee( 0.,0., TrkAtVrt[ii][0], TrkAtVrt[ii][1], TrkAtVrt[ii][2],
                                                                Trk::PerigeeSurface(FitVertex), CovMtxP );
         tmpVTAV.push_back( Trk::VxTrackAtVertex( 1., tmpMeasPer) );
         ElementLink<xAOD::TrackParticleContainer> TEL;  TEL.setElement( ListTracksNearMuon[ii] );
         const xAOD::TrackParticleContainer* cont = (const xAOD::TrackParticleContainer* ) (ListTracksNearMuon[ii]->container() );
	 TEL.setStorableObject(*cont);
         tmpVertex->addTrackAtVertex(TEL,1.);
       }
       return tmpVertex;


  }




//
//-------------------------------------------------------------
//   Template routine for global secondary vertex fitting
//  It also inserts(!!!) muon into the list of tracks on return
//  so that this list can be directly used for vertex saving  
//

  double InDetSVWithMuonTool::FitCommonVrt(std::vector<const xAOD::TrackParticle*>& ListSecondTracks,
                                  const xAOD::Vertex        & PrimVrt,
 	                          const xAOD::TrackParticle * Muon,
	                          Amg::Vector3D             & FitVertex,
                                  std::vector<double>       & ErrorMatrix,
	                          TLorentzVector            & Momentum,
		        std::vector< std::vector<double> >  & TrkAtVrt)
  const
 {
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "FitCommonVrt() called with Ntrk="<<ListSecondTracks.size()<< endreq;
//preparation
      StatusCode sc; sc.setChecked();
      ListSecondTracks.insert(ListSecondTracks.begin(), Muon);
      RemoveDoubleEntries(ListSecondTracks);
      int NTracksVrt = ListSecondTracks.size();
   
      const double maxRecMASS=6000.;
      long int           Charge;
      double             Chi2 = 0., FitProb=0.;
      Amg::Vector3D      tmpVertex;
      std::vector<double> InpMass(NTracksVrt, m_massPi);
      std::vector<double> Chi2PerTrk(0);
//
// Initialisation of fit
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
//
//fit itself
//
      for (int i=0; i < NTracksVrt-1; i++) {
         sc=VKalVrtFitBase(ListSecondTracks,FitVertex, Momentum,Charge,ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2);
         if(sc.isFailure() ||  Chi2 > 1000000. ) { return -10000.;}    // No fit
         int Outlier = FindMaxAfterFirst( Chi2PerTrk); 
	 FitProb=TMath::Prob( Chi2, 2*ListSecondTracks.size()-3);
	 if(ListSecondTracks.size() == 2 )              break;         // Only 2 tracks left
	 if( FitProb > 0.001) {
	   if( Momentum.M() <maxRecMASS) {
	     if( Chi2PerTrk[Outlier] < m_SecTrkChi2Cut)  break;  // Solution found
           } else {
	     double minM=1.e12; int minT=-1; double minChi2=1.e12;
	     for(int it=0; it<(int)ListSecondTracks.size(); it++){
                std::vector<const xAOD::TrackParticle*> tmpList(ListSecondTracks);
                tmpList.erase(tmpList.begin()+it);
                sc=VKalVrtFitBase(tmpList,tmpVertex,Momentum,Charge,ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2);
                if(sc.isFailure())continue;
		if(Momentum.M()<minM  && minM>maxRecMASS){minM=Momentum.M(); minT=it; minChi2=Chi2;}
		else if(Momentum.M()<maxRecMASS && minM<maxRecMASS && Chi2<minChi2){minChi2=Chi2; minT=it;}
	     }
	     if(minT>=0)Outlier=minT;
	   }
	 }
         RemoveEntryInList(ListSecondTracks,Outlier);
         m_fitSvc->setApproximateVertex(FitVertex.x(),FitVertex.y(),FitVertex.z()); /*Use as starting point*/
      }
//--
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" SecVrt fit converged="<< ListSecondTracks.size()<<" Mass="<<Momentum.M()<<endreq;
//--
     return Chi2;
}







//
//
//--------------------------------------------------------
//   Routine for muon+track secondary vertices selection
//

    void InDetSVWithMuonTool::GetTrkWithMuonVrt(std::vector<const xAOD::TrackParticle*>  & SelectedTracks,
                                  const xAOD::Vertex                        & PrimVrt,
	                          const xAOD::TrackParticle                 * Muon,
	                          std::vector<const xAOD::TrackParticle*>   & ListSecondTracks)
    const
    {
      Amg::Vector3D          FitVertex, vDist;
      std::vector<double> ErrorMatrix,Chi2PerTrk,VKPerigee,CovPerigee;
      std::vector< std::vector<double> > TrkAtVrt; 
      TLorentzVector   Momentum;
      std::vector<double> Impact,ImpactError;
      double   Chi2, Signif3D, Dist2D, MuonVrtDir;
      long int Charge;
//
      long int NTracks = (int) (SelectedTracks.size());
      std::vector<const xAOD::TrackParticle*>   TracksForFit(2,0);
      std::vector<double> InpMass(NTracks, m_massPi);

//
//  Impact parameters with sign calculations
//
      std::vector<double> TrackSignif(NTracks);
      double SignifR,SignifZ;
      for (int i=0; i<NTracks; i++) {
         TrackSignif[i] = m_fitSvc->VKalGetImpact(SelectedTracks[i], PrimVrt.position(), 1, Impact, ImpactError);
         if( sin(SelectedTracks[i]->phi() - Muon->phi())*Impact[0] < 0 )
	                          { Impact[0] = -fabs(Impact[0]);}
                              else{ Impact[0] =  fabs(Impact[0]);}
         if(  (SelectedTracks[i]->p4().Theta() - Muon->p4().Theta())*Impact[1] < 0 )
	                          { Impact[1] = -fabs(Impact[1]);}
                              else{ Impact[1] =  fabs(Impact[1]);}
         SignifR = Impact[0]/ sqrt(ImpactError[0]);
         SignifZ = Impact[1]/ sqrt(ImpactError[2]);
         if(m_FillHist){
            m_hb_impactR->Fill( SignifR, w_1); 
            m_hb_impactZ->Fill( SignifZ, w_1); 
            m_hb_impact->Fill( TrackSignif[i], w_1);
         }
      }


      StatusCode sc; sc.setChecked();
      ListSecondTracks.reserve(2*NTracks);                 // Reserve memory for sigle vertex

      Amg::Vector3D iniVrt(0.,0.,0.);
      m_fitSvc->setMassInputParticles( InpMass );     // Use pion masses for fit
      TracksForFit[0]=  Muon;
      for (int i=0; i<NTracks; i++) {
             if(TrackSignif[i] < m_TrkSigCut) continue;
             m_fitSvc->setDefault();                          //Reset VKalVrt settings
             m_fitSvc->setMomCovCalc(1);                     // Total momentum and its covariance matrix are calculated
             TracksForFit[1]=SelectedTracks[i];
             sc=VKalVrtFitFastBase(TracksForFit,FitVertex);              /* Fast crude estimation*/
             if( sc.isFailure() || FitVertex.perp() > m_Rlayer2*2. ) {   /* No initial estimation */ 
                iniVrt=PrimVrt.position();
 	     } else {
                vDist=FitVertex-PrimVrt.position();
                MuonVrtDir = Muon->p4().Px()*vDist.x() + Muon->p4().Py()*vDist.y() + Muon->p4().Pz()*vDist.z();
                if( MuonVrtDir>0. ) iniVrt=FitVertex;                /* Good initial estimation */ 
                else                iniVrt=PrimVrt.position();
             }
             m_fitSvc->setApproximateVertex(iniVrt.x(), iniVrt.y(), iniVrt.z()); 
             sc=VKalVrtFitBase(TracksForFit,FitVertex, Momentum,Charge,
                               ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2);
             if(sc.isFailure())                continue;          /* No fit */ 
             if(Chi2 > m_Sel2VrtChi2Cut)       continue;          /* Bad Chi2 */
             double mass_PiPi =  Momentum.M();  
	     if(mass_PiPi > 6000.)             continue;  // can't be from B decay
             if(m_FillHist){m_hb_massPiPi->Fill( mass_PiPi, w_1);}
             Dist2D=FitVertex.perp(); 
	     if(Dist2D    > 180. )             continue;  // can't be from B decay
             VrtVrtDist(PrimVrt, FitVertex, ErrorMatrix, Signif3D);
//---
	     vDist=FitVertex-PrimVrt.position();
             MuonVrtDir = Muon->p4().Px()*vDist.x() + Muon->p4().Py()*vDist.y() + Muon->p4().Pz()*vDist.z();
	     double vPos=(vDist.x()*Momentum.Px()+vDist.y()*Momentum.Py()+vDist.z()*Momentum.Pz())/Momentum.Rho();
	     if(vPos<0.) continue;                                              /*  Vertex is too far behind primary one*/
             if(m_FillHist){
	        m_hb_r2d->Fill( Dist2D, w_1);
 	        m_hb_signif3D->Fill( Signif3D, w_1);
             }
//
//  Save track crossing muon
//	     
             if(Signif3D>m_Sel2VrtSigCut) ListSecondTracks.push_back(SelectedTracks[i]);
      } 

   }




}  //end of namespace
