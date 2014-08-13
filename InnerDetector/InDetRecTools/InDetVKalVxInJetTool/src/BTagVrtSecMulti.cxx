/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "InDetVKalVxInJetTool/InDetVKalVxInJetTool.h"
//-------------------------------------------------
// Other stuff
#include  "GaudiKernel/ToolFactory.h"
#include  "TrkParticleBase/LinkToTrackParticleBase.h"
#include  "AnalysisUtils/AnalysisMisc.h"
#include  "TrkParticleBase/TrackParticleBaseCollection.h"
#include  "VxVertex/VxTrackAtVertex.h"
#include  "TrkParticleCreator/TrackParticleCreatorTool.h"
#include  "TMath.h"
#include  <algorithm>
//
//#include<iostream>

//----------------------------------------------------------------------------------------
//  GetVrtSecMulti resurns the vector Results with the following
//   1) Vertex mass
//   2) Vertex/jet energy fraction
//   3) Number of initially selected 2tr vertices
//   4) Number of selected for vertexing tracks in jet 
//   5) Number of track in secondary vertex
//   6) 0. 
//   7) Maximal track Pt with respect to jet axis
//   8) Jet energy used in (2) calculation 
//---------------------------------------------------------------------------------------- 

namespace Trk {
extern   int  pgraphm_(
         long int *weit, long int *edges, long int *nodes,
         long int *set, long int *nptr,  long int *nth);
}
//extern "C" {
//  float prob_(const float & Chi2, const long int& ndf);
//}


namespace InDet{

//   std::vector<xAOD::Vertex*> InDetVKalVxInJetTool::GetVrtSecMulti(
//                                       const std::vector<const Rec::TrackParticle*>& InpTrk,
//                                       const xAOD::Vertex                          & PrimVrt,
//	                               const TLorentzVector                        & JetDir,
//	                               std::vector<double>                         & Results,
//                                  std::vector< std::vector<const Rec::TrackParticle*> > & FoundSecondTracks,
//				               std::vector<const Rec::TrackParticle*>   & TrkFromV0) 
   std::vector<xAOD::Vertex*> InDetVKalVxInJetTool::GetVrtSecMulti(    workVectorArrxAOD * xAODwrk,
                                                                       workVectorArrREC  * RECwork,
                                             const xAOD::Vertex                          & PrimVrt,
	                                     const TLorentzVector                        & JetDir,
	                                     std::vector<double>                         & Results)
   const
   {
      int inpNPart=0; 
      int i,j;
      if(xAODwrk){
        inpNPart=xAODwrk->InpTrk.size();
        xAODwrk->FoundSecondTracks.clear();     // Input clearing for failure return
        Results.clear();                        // Input clearing for failure return
      } else if(RECwork){
        inpNPart=RECwork->InpTrk.size();
        RECwork->FoundSecondTracks.clear();     // Input clearing for failure return
        Results.clear();                        // Input clearing for failure return
      }
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "InDet GetVrtSecMulti() called with NPart=" <<inpNPart<< endreq;
   
      //std::vector<const Rec::TrackParticle*> listJetTracks, tmpListTracks, listSecondTracks, TracksForFit;
      //std::vector<Trk::VxCandidate*>  finalVertices; 
      std::vector<xAOD::Vertex*>  finalVertices; 

      if( inpNPart < 2 ) { return finalVertices;}   // 0,1 track => nothing to do!
   
   
      long int NTracks = 0;
      TLorentzVector MomentumJet;
      if     (xAODwrk) {  SelGoodTrkParticle( xAODwrk->InpTrk, PrimVrt, JetDir, xAODwrk->listJetTracks);
                          NTracks = xAODwrk->listJetTracks.size();
                          MomentumJet = TotalMom(xAODwrk->listJetTracks);}
      else if(RECwork) {  SelGoodTrkParticle( RECwork->InpTrk, PrimVrt, JetDir, RECwork->listJetTracks);
                          NTracks = RECwork->listJetTracks.size();
                          MomentumJet = TotalMom(GetPerigeeVector(RECwork->listJetTracks));}

      if(m_FillHist){m_hb_ntrkjet->Fill( (double) NTracks, w_1); }
      if( NTracks < 2 ) { return finalVertices;} // 0,1 selected track => nothing to do!

      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Number of selected tracks inside jet= " <<NTracks << endreq;
      
      if(m_FillHist){m_hb_jmom->Fill( MomentumJet.Perp(), w_1); }

//
//  InpTrk[]           - input track list
//  listJetTracks[]    - list of good tracks in jet for vertex search
//------------------------------------------------------------	 
//                     Initial track list ready
//                     Find 2track vertices
//
	 
      std::vector<double> InpMass; for(i=0; i<NTracks; i++){InpMass.push_back(m_massPi);}
      double Vrt2TrackNumber = 0;

      if     (xAODwrk){
        Select2TrVrt(xAODwrk->listJetTracks, xAODwrk->TracksForFit, PrimVrt, JetDir, InpMass, xAODwrk->TrkFromV0, xAODwrk->listSecondTracks);
        if( xAODwrk->TrkFromV0.size() > 1) { RemoveDoubleEntries(xAODwrk->TrkFromV0); AnalysisUtils::Sort::pT (&(xAODwrk->TrkFromV0));}
        RemoveDoubleEntries(xAODwrk->listSecondTracks);
        AnalysisUtils::Sort::pT (&(xAODwrk->listSecondTracks));
        Vrt2TrackNumber = (double) xAODwrk->listSecondTracks.size()/2.;
      } else if(RECwork) {  
        Select2TrVrt(RECwork->listJetTracks, RECwork->TracksForFit, PrimVrt, JetDir, InpMass, RECwork->TrkFromV0, RECwork->listSecondTracks);
        if( RECwork->TrkFromV0.size() > 1) { RemoveDoubleEntries(RECwork->TrkFromV0); AnalysisUtils::Sort::pT (&(RECwork->TrkFromV0));}
        RemoveDoubleEntries(RECwork->listSecondTracks);
        AnalysisUtils::Sort::pT (&(RECwork->listSecondTracks));
        Vrt2TrackNumber = (double) RECwork->listSecondTracks.size()/2.;
      }

//      if(m_WorkArray->m_Incomp.size() < 2 ) { return finalVertices;}	// 0,1 tracks left VVK WRONG!!!
//
//---
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" Found different tracks in pairs="<< Vrt2TrackNumber<<endreq;
//      if(listSecondTracks.size() < 2 ) { return finalVertices;}	// 0,1 tracks left VVK WRONG !!!


//
//  listSecondTracks[]   -  list of all tracks which participate in some 2-track vertex
//  TrkFromV0[]          -  "bad" tracks from any V0/material/conversion
//  m_Incomp[]           -  main vector of pointers for multivertex search
//-----------------------------------------------------------------------------------------------------
//            Secondary track list is ready
//            Creation of initial vertex set
//


      std::vector<WrkVrt> *WrkVrtSet= new std::vector<WrkVrt>;
      std::vector<WrkVrt> *WrkVrtSaveKilled= new std::vector<WrkVrt>;
      WrkVrt newvrt;

      int iRet=0;
      long int* weit     = new long int[m_WorkArray->m_Incomp.size()];
      long int* Solution = new long int[NTracks];
      for(i=0; i<(int)m_WorkArray->m_Incomp.size(); i++) *(weit+i)=(long int) (m_WorkArray->m_Incomp[i]+1); /* +1 is needed for PGRAPH*/
      long int edges = m_WorkArray->m_Incomp.size()/2; 
      long int NPTR=0,     nth=2; // VK nth=2 for speed up
      m_fitSvc->setDefault();
      StatusCode sc; if(sc.isSuccess())iRet=1; //Safety!
      while(true) {
          iRet=Trk::pgraphm_( weit, &edges, &NTracks, Solution, &NPTR, &nth);
	  if( iRet != 0) { if(msgLvl(MSG::ERROR))msg(MSG::ERROR) << " Error in PGRAPHm iRet=" <<iRet << endreq;}

                                     if(NPTR <= 0)  break;      /* No more solutions   */
                                     if(NPTR == 1)  continue;   /* Not a good solution */
          if(xAODwrk)xAODwrk->tmpListTracks.clear(); else if(RECwork)RECwork->tmpListTracks.clear();
          newvrt.SelTrk.clear();
          for(i=0;i<NPTR;i++) {         //std::cout<<"Solution="<<Solution[i]<<'\n';
             newvrt.SelTrk.push_back(Solution[i]-1);
             if     (xAODwrk)xAODwrk->tmpListTracks.push_back( xAODwrk->listJetTracks.at(Solution[i]-1) );
	     else if(RECwork)RECwork->tmpListTracks.push_back( RECwork->listJetTracks.at(Solution[i]-1) );
          }
       
          if     (xAODwrk)sc = VKalVrtFitFastBase(xAODwrk->tmpListTracks,newvrt.vertex);
	  else if(RECwork)sc = VKalVrtFitFastBase(RECwork->tmpListTracks,newvrt.vertex);
          if( sc.isFailure() || newvrt.vertex.perp() > m_Rlayer2*2.  ) {   /* No initial estimation */ 
              m_fitSvc->setApproximateVertex(PrimVrt.position().x(),       /*Use as starting point*/
                                                                         PrimVrt.position().y(),
                                                                         PrimVrt.position().z()); 
              if( m_MultiWithPrimary ) m_fitSvc->setApproximateVertex(0., 0., 0.); 
 	  } else {
	      Amg::Vector3D vDist=newvrt.vertex-PrimVrt.position();
              double JetVrtDir = JetDir.Px()*vDist.x() + JetDir.Py()*vDist.y() + JetDir.Pz()*vDist.z();
              if( m_MultiWithPrimary ) JetVrtDir=fabs(JetVrtDir); /* Always positive when primary vertex is seeked for*/ 
              if( JetVrtDir>0. ) {                           /* Good initial estimation */ 
                  m_fitSvc->setApproximateVertex(newvrt.vertex.x(),newvrt.vertex.y(),newvrt.vertex.z()); /*Use as starting point*/
	      }else{
                  m_fitSvc->setApproximateVertex(PrimVrt.position().x(), PrimVrt.position().y(), PrimVrt.position().z()); 
              }
          }
//  std::cout<<"FoundAppVrt="<<newvrt.vertex[0]<<", "<<newvrt.vertex[1]<<", "<<newvrt.vertex[2]<<'\n';
	  sc.setCode(0);
          if     (xAODwrk){sc=VKalVrtFitBase(xAODwrk->tmpListTracks,
	                                     newvrt.vertex,     newvrt.vertexMom, newvrt.vertexCharge, newvrt.vertexCov,
                                             newvrt.Chi2PerTrk, newvrt.TrkAtVrt,  newvrt.Chi2);}
	  else if(RECwork){sc=VKalVrtFitBase(RECwork->tmpListTracks,
	                                     newvrt.vertex,     newvrt.vertexMom, newvrt.vertexCharge, newvrt.vertexCov,
                                             newvrt.Chi2PerTrk, newvrt.TrkAtVrt,  newvrt.Chi2);}
//  std::cout << "Res="<<newvrt.Chi2<<", "<<sc<<'\n';
          if( sc.isFailure() )  continue;   /* Bad fit - goto next solution */
          newvrt.Good         = true;
          newvrt.nCloseVrt    = 0;
          newvrt.dCloseVrt    = 1000000.;
          WrkVrtSet->push_back(newvrt);
    } 
//
// Cleaning of solutions
//
    int NSoluI=(*WrkVrtSet).size();
    for(int iv=0; iv<NSoluI; iv++){
       if( (*WrkVrtSet)[iv].SelTrk.size() == 2 ) continue;
       if( (*WrkVrtSet)[iv].Chi2 > (4.*(*WrkVrtSet)[iv].SelTrk.size()) ){
          if     (xAODwrk)DisassembleVertex(WrkVrtSet,iv,xAODwrk->listJetTracks);
          else if(RECwork)DisassembleVertex(WrkVrtSet,iv,RECwork->listJetTracks);
       }
    }
    for(int iv=0; iv<NSoluI; iv++){
       if( (*WrkVrtSet)[iv].SelTrk.size() <= 2 ) continue;
       if(!(*WrkVrtSet)[iv].Good               ) continue;
       for(int jv=0; jv<NSoluI; jv++){
	  if(  iv == jv )             continue;
          if(!(*WrkVrtSet)[jv].Good ) continue;
          if( (*WrkVrtSet)[iv].SelTrk.size() < (*WrkVrtSet)[jv].SelTrk.size() ) continue;
          if( isPart( (*WrkVrtSet)[jv].SelTrk, (*WrkVrtSet)[iv].SelTrk ) ) (*WrkVrtSet)[jv].Good=false;  //vertex JV is part of vertex IV
       }
    }
    for(int iv=0; iv<(int)(*WrkVrtSet).size(); iv++) {if(!(*WrkVrtSet)[iv].Good) { (*WrkVrtSet).erase((*WrkVrtSet).begin()+iv);} }
//----------------------------------------------------------------------------
//   Add primary vertex
//
     if     (xAODwrk)xAODwrk->tmpListTracks.clear(); else if(RECwork)RECwork->tmpListTracks.clear();
     newvrt.SelTrk.clear();
     nth=m_WorkArray->m_Prmtrack.size();
     int PrmVrtAdded=0;
//     if( nth>1 && WrkVrtSet->size()>0 ){
//          for(i=0;i<nth;i++) { 
//             newvrt.SelTrk.push_back( m_WorkArray->m_Prmtrack[i] );
//             tmpListTracks.push_back( listJetTracks.at( m_WorkArray->m_Prmtrack[i] ) );
//          }
//          sc = VKalVrtFitFastBase(tmpListTracks,newvrt.vertex);
//          m_fitSvc->setApproximateVertex(newvrt.vertex[0],newvrt.vertex[1],newvrt.vertex[2]);
//          sc=VKalVrtFitBase(tmpListTracks,
//                            newvrt.vertex,
//                            newvrt.vertexMom,
//                            newvrt.vertexCharge,
//                            newvrt.vertexCov,
//                            newvrt.Chi2PerTrk, 
//                            newvrt.TrkAtVrt,
//                            newvrt.Chi2);
//          if( sc.isSuccess() ) {
//             PrmVrtAdded=WrkVrtSet->size();    /* Primary vertex position in vertices list */
//	     WrkVrtSet->push_back(newvrt);    /* Good fit - add it */
//          }
//          //std::cout<<"PrimVrt="<<nth<<", "<<newvrt.vertex[0]<<", "<<newvrt.vertex[1]<<", "<<newvrt.vertex[2]
//          //         <<" Chi2="<<newvrt.Chi2<<'\n';
//     }
//
//----------------------------------------------------------------------------
//             Here we have the overlapping solutions
//              Now solution cleaning

    std::vector< std::deque<long int> > *TrkInVrt  =new std::vector< std::deque<long int> >(NTracks);  
    double foundMaxT; long int SelectedTrack, SelectedVertex;
    double foundMinVrtDst; int foundV1, foundV2;
    
    TrackClassification( WrkVrtSet, TrkInVrt);

    m_fitSvc->setDefault();
    while((foundMaxT=MaxOfShared( WrkVrtSet, TrkInVrt, SelectedTrack, SelectedVertex))>0) {
// std::cout << "MAX="<<foundMaxT<<", "<<SelectedTrack<<", "<<SelectedVertex<<'\n';
// std::cout << "VRT="<<minVrtVrtDist( WrkVrtSet, foundV1, foundV2)<<", "<<foundV1<<", "<<foundV2<<'\n';
// printWrkSet(WrkVrtSet,"Interm");

         foundMinVrtDst = 1000000.;
         if(foundMaxT<m_TrackDetachCut) foundMinVrtDst = minVrtVrtDist( WrkVrtSet, foundV1, foundV2);

//Choice of action
          if( foundMaxT<m_TrackDetachCut && foundMinVrtDst<m_VertexMergeCut){
             if(foundV1<foundV2) { int tmp=foundV1; foundV1=foundV2; foundV2=tmp;} /*Always drop vertex with smallest number*/
	     MergeVertices( WrkVrtSet, foundV1, foundV2);                          /* to keep primary(last) one */
	     sc.setCode(0);
             if     (xAODwrk) sc = RefitVertex( WrkVrtSet, foundV1, xAODwrk->listJetTracks);
             else if(RECwork) sc = RefitVertex( WrkVrtSet, foundV1, RECwork->listJetTracks);
             if( sc.isFailure() )  continue;                            /* Bad fit - goto next solution */
             while ( minVrtVrtDistNext( WrkVrtSet, foundV1, foundV2) < m_VertexMergeCut) {   //check other vertices
                if(foundV1<foundV2) { int tmp=foundV1; foundV1=foundV2; foundV2=tmp;}
	        MergeVertices( WrkVrtSet, foundV1, foundV2);
	        sc.setCode(0);
                if     (xAODwrk)sc = RefitVertex( WrkVrtSet, foundV1, xAODwrk->listJetTracks);
                else if(RECwork)sc = RefitVertex( WrkVrtSet, foundV1, RECwork->listJetTracks);
                if( sc.isFailure() )  break;                            /* Bad fit - goto next solution */
             }
             delete TrkInVrt;
             TrkInVrt = new std::vector< std::deque<long int> >(NTracks);  
             TrackClassification( WrkVrtSet, TrkInVrt);
	  }else{
	     if((*WrkVrtSet)[SelectedVertex].SelTrk.size() == 2) {   /* Ntr_in_vertex=2 and then vertex will be killed*/
                newvrt.Good = false;
                newvrt.SelTrk.clear(); newvrt.SelTrk.push_back( (*WrkVrtSet)[SelectedVertex].SelTrk[0] );
                                       newvrt.SelTrk.push_back( (*WrkVrtSet)[SelectedVertex].SelTrk[1] );
                newvrt.vertex       = (*WrkVrtSet)[SelectedVertex].vertex;
                newvrt.vertexMom    = (*WrkVrtSet)[SelectedVertex].vertexMom;
                newvrt.vertexCov    = (*WrkVrtSet)[SelectedVertex].vertexCov;
                newvrt.vertexCharge = (*WrkVrtSet)[SelectedVertex].vertexCharge;
                newvrt.Chi2PerTrk.clear(); newvrt.Chi2PerTrk.push_back( (*WrkVrtSet)[SelectedVertex].Chi2PerTrk[0] );
                                           newvrt.Chi2PerTrk.push_back( (*WrkVrtSet)[SelectedVertex].Chi2PerTrk[1] );
                newvrt.Chi2         = (*WrkVrtSet)[SelectedVertex].Chi2;
                newvrt.nCloseVrt    = (*WrkVrtSet)[SelectedVertex].nCloseVrt;
                newvrt.dCloseVrt    = (*WrkVrtSet)[SelectedVertex].dCloseVrt;
	        WrkVrtSaveKilled->push_back(newvrt);    /* add it */
	     }
             RemoveTrackFromVertex(WrkVrtSet, TrkInVrt, SelectedTrack, SelectedVertex);
	     sc.setCode(0);
             if     (xAODwrk)sc = RefitVertex( WrkVrtSet, SelectedVertex, xAODwrk->listJetTracks);
             else if(RECwork)sc = RefitVertex( WrkVrtSet, SelectedVertex, RECwork->listJetTracks);
             if( sc.isFailure() )  continue;                            /* Bad fit - goto next solution */
          }
    }
//
// Final check of close vertices
//
    while ( minVrtVrtDist( WrkVrtSet, foundV1, foundV2) < m_VertexMergeCut) {
        if(foundV1<foundV2) { int tmp=foundV1; foundV1=foundV2; foundV2=tmp;}
	MergeVertices( WrkVrtSet, foundV1, foundV2);
	sc.setCode(0);
        if     (xAODwrk)sc = RefitVertex( WrkVrtSet, foundV1, xAODwrk->listJetTracks);
        else if(RECwork)sc = RefitVertex( WrkVrtSet, foundV1, RECwork->listJetTracks);
        if( sc.isFailure() )  break;                            /* Bad fit - goto next solution */
    }
    //printWrkSet(WrkVrtSet,"Joined");
//--------------------------------------------------------------------------------------------------------
// Final vertices refitting
//

    int nGoodVertices=0;         // Final number of good vertices
    m_fitSvc->setDefault();
    m_fitSvc->setMomCovCalc(1);
    double Signif3D, Dist2D, Dist3D;
    for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
          nth=(*WrkVrtSet).at(iv).SelTrk.size();
 	  //std::cout<<" Solution="<<iv<<" Ntrk="<<nth<<'\n';
          if(nth == 1 && m_MultiWithOneTrkVrt){                      /* 1track vertices are already classified */
             int nBLHits=0, tmpL1=0,tmpL2=0;
             if     (RECwork) getPixelLayers(RECwork->listJetTracks[(*WrkVrtSet)[iv].SelTrk[0]], nBLHits, tmpL1, tmpL2);
	     else if(xAODwrk) getPixelLayers(xAODwrk->listJetTracks[(*WrkVrtSet)[iv].SelTrk[0]], nBLHits, tmpL1, tmpL2);
             if(nBLHits>0){            /* accept only tracks with b-layer hit */
                std::vector<double> Impact,ImpactError;
                double Signif3DP = 0;
		if     (xAODwrk) Signif3DP=m_fitSvc->VKalGetImpact(xAODwrk->listJetTracks[(*WrkVrtSet)[iv].SelTrk[0]],PrimVrt.position(), 1, Impact, ImpactError);
		else if(RECwork) Signif3DP=m_fitSvc->VKalGetImpact(RECwork->listJetTracks[(*WrkVrtSet)[iv].SelTrk[0]],PrimVrt.position(), 1, Impact, ImpactError);
                if(m_FillHist){m_hb_diffPS->Fill( Signif3DP, w_1); }
                if( Signif3DP > 2.*m_TrkSigCut ){                    /* accept only tracks which are far from primary vertex */
                  double mass_PiPi   =  massV0( (*WrkVrtSet)[iv].TrkAtVrt,m_massPi,m_massPi);
                  if(m_FillHist){m_hb_totmass2T2->Fill( mass_PiPi, w_1); }
	          nGoodVertices++;  continue;
                }
             }
	  }
          (*WrkVrtSet)[iv].Good = false;                                     /* Make all vertices bad for futher selection */
          if(nth <= 1) continue;                                             /* Definitely bad vertices */
          if(xAODwrk)xAODwrk->tmpListTracks.clear(); else if(RECwork)RECwork->tmpListTracks.clear();
          for(i=0;i<nth;i++) {
	    j=(*WrkVrtSet)[iv].SelTrk[i];                           /*Track number*/
            if     (xAODwrk)xAODwrk->tmpListTracks.push_back( xAODwrk->listJetTracks[j] );
	    else if(RECwork)RECwork->tmpListTracks.push_back( RECwork->listJetTracks[j] );
          }
	  sc.setCode(0);
          if     (xAODwrk)sc = RefitVertex( WrkVrtSet, iv, xAODwrk->listJetTracks);
          else if(RECwork)sc = RefitVertex( WrkVrtSet, iv, RECwork->listJetTracks);
          if( sc.isFailure() )  continue;   /* Bad fit - goto next solution */
          (*WrkVrtSet)[iv].Good = false;    /* Make all vertices after RefitVertex bad again  */
//	  float FitProb=TMath::Prob( (*WrkVrtSet)[iv].Chi2, 2*nth-3);
//          if( FitProb < 0.001 ) continue;   /* Bad fit - goto next solution */
//----
          Dist2D=sqrt( (*WrkVrtSet)[iv].vertex.x()*(*WrkVrtSet)[iv].vertex.x()
	              +(*WrkVrtSet)[iv].vertex.y()*(*WrkVrtSet)[iv].vertex.y() );
          Dist3D=VrtVrtDist(PrimVrt, (*WrkVrtSet)[iv].vertex, (*WrkVrtSet)[iv].vertexCov, Signif3D);
//----
          if(PrmVrtAdded && iv==PrmVrtAdded && Dist3D<3.5) continue;  /* Skip added primary vertex */
//----
          bool BadVertex=false;
          if( nth==2  && (Dist2D < m_RlayerB) ) {             // If there are 2 only tracks
            int tmpBLHits=0, tmpL1=0,tmpL2=0;
            if(RECwork){
              std::vector<const Rec::TrackParticle*>::const_iterator   i_ntrk;
              for (i_ntrk = RECwork->tmpListTracks.begin(); i_ntrk != RECwork->tmpListTracks.end(); ++i_ntrk) {
                getPixelLayers((*i_ntrk), tmpBLHits, tmpL1, tmpL2);
	        if( tmpBLHits==0)BadVertex=true; 
              }
            }else if(xAODwrk){
              std::vector<const xAOD::TrackParticle*>::const_iterator   i_ntrk;
              for (i_ntrk = xAODwrk->tmpListTracks.begin(); i_ntrk != xAODwrk->tmpListTracks.end(); ++i_ntrk) {
                 getPixelLayers((*i_ntrk), tmpBLHits, tmpL1, tmpL2);
	        if( tmpBLHits==0)BadVertex=true; 
              }
	    }
          }
          if(BadVertex) continue;
//----
          if( nth==2  && (Dist2D > 20.) && (*WrkVrtSet)[iv].vertexCharge==0 ) {      
            double mass_EE   =  massV0( (*WrkVrtSet)[iv].TrkAtVrt,m_massE,m_massE);
            if(m_FillHist){m_hb_totmassEE->Fill( mass_EE, w_1); }
            if( mass_EE < 60. ) continue;
          }
//
//-----  Check interactions on pixel layers
//
	  double xvt=(*WrkVrtSet)[iv].vertex.x(); double yvt=(*WrkVrtSet)[iv].vertex.y();
          Dist2D=sqrt( (xvt-m_XlayerB)*(xvt-m_XlayerB) + (yvt-m_YlayerB)*(yvt-m_YlayerB) ); 
	  if( fabs(Dist2D-m_RlayerB) < 3.5)  continue;
          Dist2D=sqrt( (xvt-m_Xlayer1)*(xvt-m_Xlayer1) + (yvt-m_Ylayer1)*(yvt-m_Ylayer1) ); 
	  if( fabs(Dist2D-m_Rlayer1) < 4.0)  continue;
//-----
          if( nth==2 && (*WrkVrtSet)[iv].vertexCharge==0 ){
             double mass_PiPi =  (*WrkVrtSet)[iv].vertexMom.M();  
             double mass_PPi  =  massV0((*WrkVrtSet)[iv].TrkAtVrt,m_massP,m_massPi);
	     if(m_FillHist)m_hb_massPiPi1->Fill( mass_PiPi , w_1);       
	     if( fabs(mass_PiPi-m_massK0) < 22.)   continue;
 	     if( fabs(mass_PPi-m_massLam) <  8.)   continue;
          }          
//-----
          if(m_FillHist){m_hb_sig3DTot->Fill( Signif3D, w_1); }
//-----
//	  if(Signif3D < 3.5) continue;
//-----
//          if( nth==2 ) {             // If there are 2 only tracks
//            if(m_FillHist){m_hb_totmass2T0->Fill( (*WrkVrtSet)[iv].vertexMom.m(), w_1); }
//            if( (*WrkVrtSet)[iv].vertexMom.m() < 400.) continue;
//          }
//-----
          (*WrkVrtSet)[iv].Good = true;  /* Vertex is absolutely good */
          nGoodVertices++;
    }
//
// Cleaning - single 2tr vertex
    if(nGoodVertices == 1 && (!m_MultiWithPrimary) ){
      for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
          if( !(*WrkVrtSet)[iv].Good ) continue;
          if( (*WrkVrtSet)[iv].SelTrk.size() != 2) continue;
          Dist2D=sqrt( (*WrkVrtSet)[iv].vertex.x()*(*WrkVrtSet)[iv].vertex.x()
	              +(*WrkVrtSet)[iv].vertex.y()*(*WrkVrtSet)[iv].vertex.y() );
          Dist3D=VrtVrtDist(PrimVrt, (*WrkVrtSet)[iv].vertex, (*WrkVrtSet)[iv].vertexCov, Signif3D);
          if(m_FillHist){m_hb_sig3D2tr->Fill( Signif3D , w_1); }
	  if(Signif3D < 5.0) (*WrkVrtSet)[iv].Good = false;
          if(Dist2D<m_RlayerB){
            if(m_FillHist){m_hb_totmass2T0->Fill( (*WrkVrtSet)[iv].vertexMom.M(), w_1); }
//            if( (*WrkVrtSet)[iv].vertexMom.m() < 400.) (*WrkVrtSet)[iv].Good = false; 
          }
      }
    }
//
// Check killed vertices. Drop shared track from summary momentum. WRONG and should be corrected
//
//    for(int iv=0; iv<(int)WrkVrtSaveKilled->size(); iv++) {
//      int nonsharedTrk = CheckKilledVertices( &(*WrkVrtSaveKilled)[iv], WrkVrtSet);
//      if( nonsharedTrk ){
//         (*WrkVrtSaveKilled)[iv].Good=true;           //make vertex good
//         double phi   = (*WrkVrtSaveKilled)[iv].TrkAtVrt[nonsharedTrk-1][0]; // take momentum of nonshared track only
//	 double theta = (*WrkVrtSaveKilled)[iv].TrkAtVrt[nonsharedTrk-1][1];
//	 double ap    = (*WrkVrtSaveKilled)[iv].TrkAtVrt[nonsharedTrk-1][2];
//         TLorentzVector    tmp( cos(phi)*sin(theta)/fabs(ap),
//                                sin(phi)*sin(theta)/fabs(ap),
//                                         cos(theta)/fabs(ap),
//                                          0.         );
//         tmp.SetE( sqrt(tmp.Vect().Mag2()+m_massPi*m_massPi) );
//         (*WrkVrtSaveKilled)[iv].vertexMom = tmp;
//         WrkVrtSet->push_back((*WrkVrtSaveKilled)[iv]);
//      }
//    }
//-------------------------------------------
//checks
    std::vector<WrkVrt> GoodVertices;
    nGoodVertices=0;         // Final number of good vertices
    int n2trVrt=0;           // N vertices with 2 and more tracks
    for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
       nth=(*WrkVrtSet)[iv].SelTrk.size(); if(nth == 0) continue;   /* Definitely bad vertices */
       if( (*WrkVrtSet)[iv].Good) {
	  nGoodVertices++;                                    
          newvrt.Good = true;
          newvrt.SelTrk.resize(nth); newvrt.Chi2PerTrk.resize(nth); newvrt.TrkAtVrt.resize(nth);
          copy((*WrkVrtSet)[iv].SelTrk.begin(),     (*WrkVrtSet)[iv].SelTrk.end(),     newvrt.SelTrk.begin()); 
          copy((*WrkVrtSet)[iv].Chi2PerTrk.begin(), (*WrkVrtSet)[iv].Chi2PerTrk.end(), newvrt.Chi2PerTrk.begin()); 
          for(int it=0; it<nth; it++){
            newvrt.TrkAtVrt[it].resize(3); 
            copy((*WrkVrtSet)[iv].TrkAtVrt[it].begin(),   (*WrkVrtSet)[iv].TrkAtVrt[it].end(),   newvrt.TrkAtVrt[it].begin()); 
	  }
          newvrt.vertex       = (*WrkVrtSet)[iv].vertex;
          newvrt.vertexMom    = (*WrkVrtSet)[iv].vertexMom;
          newvrt.vertexCov    = (*WrkVrtSet)[iv].vertexCov;
          newvrt.vertexCharge = (*WrkVrtSet)[iv].vertexCharge;
          newvrt.Chi2         = (*WrkVrtSet)[iv].Chi2;
          newvrt.ProjectedVrt =  ProjPos( newvrt.vertex, JetDir) ;
	  GoodVertices.push_back(newvrt);    /* add it */
	  if(nth>1)n2trVrt++;
       }
    }
    if(nGoodVertices == 0 || n2trVrt==0){
      delete WrkVrtSaveKilled; delete WrkVrtSet;
      delete TrkInVrt; delete[] weit; delete[] Solution;
      return finalVertices;
    }
//
//sorting
    while(1){ bool swapDone=false;                              // Sort on XY distance from (0.,0.)
      for(int iv=0; iv<(int)GoodVertices.size()-1; iv++) {
        double Rtmp1=  GoodVertices[iv].vertex.x()*GoodVertices[iv].vertex.x() 
	             + GoodVertices[iv].vertex.y()*GoodVertices[iv].vertex.y();
        double Rtmp2=  GoodVertices[iv+1].vertex.x()*GoodVertices[iv+1].vertex.x() 
	             + GoodVertices[iv+1].vertex.y()*GoodVertices[iv+1].vertex.y();
        if( Rtmp1 > Rtmp2){
	  std::swap( GoodVertices[iv], GoodVertices[iv+1] );
          swapDone=true; 
        }
      }    if(!swapDone) break;
    }
    while(1){ bool swapDone=false;                            // Then sort on Projected dist if R<20.
      for(int iv=0; iv<(int)GoodVertices.size()-1; iv++) {
        double Rtmp=  GoodVertices[iv+1].vertex.x()*GoodVertices[iv+1].vertex.x() 
	            + GoodVertices[iv+1].vertex.y()*GoodVertices[iv+1].vertex.y();
        if(Rtmp > 400.) continue;
        if(GoodVertices[iv].ProjectedVrt > GoodVertices[iv+1].ProjectedVrt){
	  std::swap( GoodVertices[iv], GoodVertices[iv+1] );
          swapDone=true; 
        }
      }    if(!swapDone) break;
    }
    if(nGoodVertices>1){
        if( GoodVertices[1].vertexMom.M()-GoodVertices[0].vertexMom.M() > 5000.){
	   std::swap( GoodVertices[0], GoodVertices[1] );
	}
    }
//std::cout<<" dist="<< minVrtVrtDist( WrkVrtSet, foundV1, foundV2)<<'\n';
    if(m_FillHist && GoodVertices.size() == 2){m_hb_distVV->Fill( minVrtVrtDist( WrkVrtSet, foundV1, foundV2), w_1); }
//----------------------------------------------------------------------------------
//  Nonused tracks for one-track-vertex search
//
    if( m_MultiWithOneTrkVrt && (!m_MultiWithPrimary) ){        // Addition of one-track vertices is allowed
      double addVrtChi2Cut   =3.0;
      double addDistRatioCut =0.5;
      int tmpNTrk=0; if(xAODwrk)tmpNTrk=xAODwrk->listJetTracks.size(); else if(RECwork)tmpNTrk=RECwork->listJetTracks.size();
      std::vector<long int> nonusedTracks;
      std::vector<double> Impact,ImpactError;
      for(int atr=0; atr<tmpNTrk; atr++){
        int exist=0;
        for(int iv=0; iv<(int)GoodVertices.size(); iv++) {
          nth=GoodVertices[iv].SelTrk.size(); 
          for(i=0;i<nth;i++) { if(atr==GoodVertices[iv].SelTrk[i]){exist=1; break;}}
	  if(exist)break;
        }
        if(!exist){
          int tmpNBLHits=0, tmpL1=0, tmpL2=0;
	  double Signif3DP=0.;
	  if     (RECwork) getPixelLayers( RECwork->listJetTracks[atr], tmpNBLHits, tmpL1, tmpL2);
	  else if(xAODwrk) getPixelLayers( xAODwrk->listJetTracks[atr], tmpNBLHits, tmpL1, tmpL2);
          if(tmpNBLHits>0){       /* accept only tracks with b-layer hit */
            if     (RECwork)Signif3DP = m_fitSvc->VKalGetImpact(RECwork->listJetTracks[atr], PrimVrt.position(), 1, Impact, ImpactError);
            else if(xAODwrk)Signif3DP = m_fitSvc->VKalGetImpact(xAODwrk->listJetTracks[atr], PrimVrt.position(), 1, Impact, ImpactError);
            if( Signif3DP > 2.*m_TrkSigCut ) nonusedTracks.push_back(atr);
          }
        }
      }       
      std::vector<WrkVrt> AddedVertices;
      std::vector<WrkVrt> AddedVerticesClean;
      if(nonusedTracks.size()>0 ){
        std::vector<double> VKPerigee,CovPerigee; std::vector<double> ErrorMatrix(21,0.);
        ErrorMatrix[0]=ErrorMatrix[2]=ErrorMatrix[5]=ErrorMatrix[9]=ErrorMatrix[14]=ErrorMatrix[20]=1.e-6;
        for(int iv=0; iv<(int)GoodVertices.size(); iv++) {
          if(GoodVertices[iv].SelTrk.size()<2) continue; 
          VrtVrtDist(PrimVrt, GoodVertices[iv].vertex, newvrt.vertexCov, Signif3D);
          if(Signif3D<10.) continue;   //Too close to primary vertex. Direction not robust
          Amg::Vector3D tmpVec=GoodVertices[iv].vertex-PrimVrt.position(); tmpVec.normalize();
          TLorentzVector Momentum(tmpVec.x(),tmpVec.y(),tmpVec.z(),1.); Momentum *= 1.e5;
          sc=m_fitSvc->VKalVrtCvtTool(GoodVertices[iv].vertex,Momentum,ErrorMatrix,0,VKPerigee,CovPerigee);
          if(sc.isSuccess()) {
            double distRef=(GoodVertices[iv].vertex-PrimVrt.position()).mag(); //Distance of already found vertex
            double addChi2Min=999999.;                            //For selection of the best candidate for given existing vertex
            WrkVrt newvrt;
            Trk::Track* TT(0); 
            Rec::TrackParticle* TP(0);
            xAOD::TrackParticle * xaodTP(0);
            if( RECwork && m_trkPartCreator){
              TT = m_fitSvc->CreateTrkTrack(VKPerigee,CovPerigee); 
              TP=m_trkPartCreator->createParticle(TT);
              RECwork->tmpListTracks.clear(); RECwork->tmpListTracks.push_back(TP);
            } else if(xAODwrk){
              xaodTP=new xAOD::TrackParticle();
              xaodTP->makePrivateStore();
	      xaodTP->setDefiningParameters(VKPerigee[0],VKPerigee[1],VKPerigee[2],VKPerigee[3],VKPerigee[4]);
	      std::vector<float> floatCovPerigee(15,0); for(i=0; i<15; i++)floatCovPerigee[i]=(float)CovPerigee[i];
	      xaodTP->setDefiningParametersCovMatrixVec(floatCovPerigee);
              xaodTP->setParametersOrigin(0.,0.,0.);
              xAODwrk->tmpListTracks.clear(); xAODwrk->tmpListTracks.push_back(xaodTP);
	    }
            //
	    // - cycle over nonused tracks 
	    for(int add=0; add<(int)nonusedTracks.size(); add++){
	      sc.setCode(0);
	      if(RECwork){
	        RECwork->tmpListTracks.push_back(RECwork->listJetTracks[nonusedTracks[add]]);
                sc=VKalVrtFitBase(RECwork->tmpListTracks, newvrt.vertex,    newvrt.vertexMom,  newvrt.vertexCharge,
		                                  newvrt.vertexCov, newvrt.Chi2PerTrk, newvrt.TrkAtVrt, newvrt.Chi2);   
              }else if(xAODwrk){
	        xAODwrk->tmpListTracks.push_back(xAODwrk->listJetTracks[nonusedTracks[add]]);
                sc=VKalVrtFitBase(xAODwrk->tmpListTracks, newvrt.vertex,    newvrt.vertexMom,  newvrt.vertexCharge,
		                                  newvrt.vertexCov, newvrt.Chi2PerTrk, newvrt.TrkAtVrt, newvrt.Chi2);   
              }	      
              if( sc.isSuccess() ) {
                if(newvrt.Chi2>addVrtChi2Cut) continue;
                if(m_FillHist)m_hb_addRatioMV->Fill( (newvrt.vertex-PrimVrt.position()).mag()/distRef,1.);
                if( (newvrt.vertex-PrimVrt.position()).mag()/distRef < addDistRatioCut) continue;  // Too close to primary vertex
                newvrt.Good=true;  newvrt.vertexCharge +=1.;                               //Compensate charge of pseudotrack
                newvrt.SelTrk.clear(); newvrt.SelTrk.push_back(nonusedTracks[add]);        //Add the only nonused track
                newvrt.vertexMom=MomAtVrt(newvrt.TrkAtVrt[1]);                             //Redefine VertexMom using SINGLE REAL track
                if(newvrt.Chi2 < addChi2Min){ if(addChi2Min!=999999.)AddedVertices.pop_back(); AddedVertices.push_back(newvrt); addChi2Min=newvrt.Chi2;}
	      } 
	      if(RECwork)RECwork->tmpListTracks.pop_back(); else if(xAODwrk)xAODwrk->tmpListTracks.pop_back();
	    }
            if(addChi2Min<999999. && m_FillHist) m_hb_addChi2MV->Fill(AddedVertices[AddedVertices.size()-1].Chi2,1.);
            if     (RECwork){ RECwork->tmpListTracks.clear(); delete TT; delete TP; }
	    else if(xAODwrk){ xAODwrk->tmpListTracks.clear(); delete xaodTP; }
          }
        }
	//---------Clean added vertices
	for(int add=0; add<(int)nonusedTracks.size(); add++){
          int iBestVrt=-1;
          double addChi2Min=999999.;                            //For selection of the best candidate for given track
          for(int iv=0; iv<(int)AddedVertices.size(); iv++){
	    if(AddedVertices[iv].SelTrk[0]==nonusedTracks[add]){    //Vertex created by given track
               if(AddedVertices[iv].Chi2<addChi2Min){ iBestVrt=iv; addChi2Min=AddedVertices[iv].Chi2;}
	    }
	  }
	  if(addChi2Min<999999.)AddedVerticesClean.push_back(AddedVertices.at(iBestVrt));
        }
        if(m_FillHist)m_hb_addNVrtMV->Fill((float)AddedVerticesClean.size(),1.);
        if(AddedVertices.size()) for(int iv=0; iv<(int)AddedVerticesClean.size(); iv++) GoodVertices.push_back(AddedVerticesClean[iv]);
      }
    }   //--------------- End of one-track-vertex addition
//-------------------------------------------
// Saving and final cleaning
//
    nGoodVertices=0;         // Final number of good vertices
    TLorentzVector VertexMom;
    double trackPt, trackPtMax=0.;
    for(int iv=0; iv<(int)GoodVertices.size(); iv++) {
          nth=GoodVertices[iv].SelTrk.size();
          if(xAODwrk)xAODwrk->tmpListTracks.clear(); else if(RECwork)RECwork->tmpListTracks.clear();
          for(i=0;i<nth;i++) {
             j=GoodVertices[iv].SelTrk[i];                           /*Track number*/
             if     (xAODwrk)xAODwrk->tmpListTracks.push_back( xAODwrk->listJetTracks[j] );
             else if(RECwork)RECwork->tmpListTracks.push_back( RECwork->listJetTracks[j] );
          }
          if( nth == 2 && GoodVertices[iv].vertexCharge==0){
            if(m_FillHist){m_hb_totmass2T1->Fill( GoodVertices[iv].vertexMom.M(), w_1); }
          }
          for(i=0; i<nth; i++) {
             trackPt=pTvsDir(Amg::Vector3D(JetDir.Vect().X(),JetDir.Vect().X(),JetDir.Vect().Z()), GoodVertices[iv].TrkAtVrt[i]);
             if(trackPt>trackPtMax)trackPtMax=trackPt;
          }
//
// New code------------------------------------------------------------------------------------------
//          Trk::RecVertex* tmpRecV = new Trk::RecVertex( GoodVertices[iv].vertex , makeVrtCovMatrix( GoodVertices[iv].vertexCov ), 
//	                                              tmpListTracks.size()*2.-3., GoodVertices[iv]. Chi2); 
//          std::vector<Trk::VxTrackAtVertex*> * tmpVTAV = new std::vector<Trk::VxTrackAtVertex*>();
//          for(i=0; i<nth; i++) {
//             AmgSymMatrix(5) * tmpCovMatr=new AmgSymMatrix(5); (*tmpCovMatr).setIdentity(); 
//             Trk::Perigee * measPerigee =  new Trk::Perigee( 0.,0., GoodVertices[iv].TrkAtVrt[i][0],
//                                                                    GoodVertices[iv].TrkAtVrt[i][1],
//                                                                    GoodVertices[iv].TrkAtVrt[i][2],
//                                                            Trk::PerigeeSurface(GoodVertices[iv].vertex),
//                                                            tmpCovMatr  );
//             tmpPointer = new Trk::VxTrackAtVertex( 1., measPerigee ) ;
//             ElementLink<Trk::TrackParticleBaseCollection> TEL;  TEL.setElement( (Trk::TrackParticleBase*) tmpListTracks[i]);
//             Trk::LinkToTrackParticleBase * ITL = new Trk::LinkToTrackParticleBase(TEL); //pointer to initial Trk
//             Trk::VxTrackAtVertex * tmpPointer->setOrigTrack(ITL);              //pointer to initial TrackParticle
//             tmpPointer->setWeight(1.);
//             tmpVTAV->push_back( tmpPointer );
//          }
//          Trk::VxCandidate * tmpVx =new Trk::VxCandidate(*tmpRecV,*tmpVTAV);
//          tmpVx->setVertexType(Trk::SecVtx);
//          delete tmpVTAV;delete tmpRecV;
//          finalVertices.push_back(tmpVx);
//
// xAOD::Vertex creation-----------------------------
          xAOD::Vertex * tmpVertex=new xAOD::Vertex();
          tmpVertex->makePrivateStore();
          tmpVertex->setPosition(GoodVertices[iv].vertex);
          int NERMS=GoodVertices[iv].vertexCov.size();
          std::vector<float> floatErrMtx;   floatErrMtx.resize(NERMS);
          for(int i=0; i<NERMS; i++) floatErrMtx[i]=GoodVertices[iv].vertexCov[i];
          tmpVertex->setCovariance(floatErrMtx);
          tmpVertex->setFitQuality(GoodVertices[iv].Chi2, (float)(nth*2-3));

          std::vector<Trk::VxTrackAtVertex> & tmpVTAV=tmpVertex->vxTrackAtVertex();    tmpVTAV.clear();
          for(int ii=0; ii<nth; ii++) {
             AmgSymMatrix(5) *CovMtxP=new AmgSymMatrix(5);    (*CovMtxP).setIdentity(); 
             Trk::Perigee * tmpMeasPer  =  new Trk::Perigee( 0.,0.,  GoodVertices[iv].TrkAtVrt[ii][0], 
	                                                             GoodVertices[iv].TrkAtVrt[ii][1],
                                                                     GoodVertices[iv].TrkAtVrt[ii][2],
                                                                Trk::PerigeeSurface(GoodVertices[iv].vertex), CovMtxP );
             tmpVTAV.push_back( Trk::VxTrackAtVertex( 1., tmpMeasPer) );
	     if(xAODwrk){            //No way to store a link to Rec::TrackParticleContainer 
               ElementLink<xAOD::TrackParticleContainer> TEL;  TEL.setElement( xAODwrk->tmpListTracks[ii] );
               const xAOD::TrackParticleContainer* cont = (const xAOD::TrackParticleContainer* ) (xAODwrk->tmpListTracks[ii]->container() );
	       TEL.setStorableObject(*cont);
               tmpVertex->addTrackAtVertex(TEL,1.);
	     }
          }
          finalVertices.push_back(tmpVertex);
//-----
          nGoodVertices++;
//-----
          if( iv==0 && m_MultiWithPrimary ) continue;  //skip primary vertex if present
          VertexMom += GoodVertices[iv].vertexMom;
    }
    if(m_FillHist){m_hb_goodvrtN->Fill( (double)nGoodVertices, w_1); }

//-----------------------------------------------------------------------------------
//  Saving of results
//
//
      double totMass = VertexMom.M();
      //double totE    = VertexMom.E();
      Results.push_back(totMass);                           //1st
      double eRatio = VertexMom.E()/MomentumJet.E(); 
      Results.push_back(  eRatio<1. ? eRatio : 1.);         //2nd
      Results.push_back(Vrt2TrackNumber);                   //3rd
      Results.push_back((double)NTracks);                   //4th
      if     (xAODwrk) Results.push_back((double)xAODwrk->listSecondTracks.size());   //5th
      else if(RECwork) Results.push_back((double)RECwork->listSecondTracks.size());   //5th
//      Dist3D=VrtVrtDist(PrimVrt, FitVertex, ErrorMatrix, Signif3D);
//      Results.push_back(Signif3D);
      Results.push_back(0.);                                //6th  -  not clear what to use here -> return 0.
      Results.push_back(trackPtMax);                        //7th
      Results.push_back(MomentumJet.E());                   //8th

      if(m_FillHist){m_hb_ratio->Fill( Results[1], w_1); }
      if(m_FillHist){m_hb_totmass->Fill( Results[0], w_1); }
      if(m_FillHist){m_hb_nvrt2->Fill( Results[2], w_1); }
      if(m_FillHist){m_hb_mom->Fill( MomentumJet.Perp(), w_1);} 

      delete WrkVrtSaveKilled; delete WrkVrtSet;
      delete TrkInVrt; delete[] weit; delete[] Solution;

      return finalVertices;




  }




















                      /* Service routines*/

   template <class Particle>
   void InDetVKalVxInJetTool::DisassembleVertex(std::vector<WrkVrt> *WrkVrtSet, int iv, 
                         std::vector<const Particle*>  AllTracks)
   const
   {
      WrkVrt newvrt;
      std::vector<const Particle*>  ListBaseTracks;
      int NTrk=(*WrkVrtSet)[iv].SelTrk.size(), SelT=-1;
      double Chi2Max=0.;
      for(int i=0; i<NTrk; i++){
         if( (*WrkVrtSet)[iv].Chi2PerTrk[i]>Chi2Max) { Chi2Max=(*WrkVrtSet)[iv].Chi2PerTrk[i];  SelT=i;}
      }	    
      if(SelT==-1)return;
      StatusCode sc;
      for(int i=0; i<NTrk; i++){
	   if(i==SelT)continue;
           ListBaseTracks.clear();
	   ListBaseTracks.push_back( AllTracks[(*WrkVrtSet)[iv].SelTrk[i]] );
	   ListBaseTracks.push_back( AllTracks[(*WrkVrtSet)[iv].SelTrk[SelT]] );
           newvrt.SelTrk.resize(2); 
	   newvrt.SelTrk[0]=(*WrkVrtSet)[iv].SelTrk[i]; 
	   newvrt.SelTrk[1]=(*WrkVrtSet)[iv].SelTrk[SelT];
           sc = VKalVrtFitFastBase(ListBaseTracks,newvrt.vertex);
           if( sc.isFailure() )  continue;
           if( newvrt.vertex.perp() > m_Rlayer2*2. )  newvrt.vertex=Amg::Vector3D(0.,0.,0.);
           m_fitSvc->setApproximateVertex(newvrt.vertex[0],newvrt.vertex[1],newvrt.vertex[2]);
           sc=VKalVrtFitBase(ListBaseTracks,
                                       newvrt.vertex,
                                       newvrt.vertexMom,
			               newvrt.vertexCharge,
		                       newvrt.vertexCov,
		                       newvrt.Chi2PerTrk, 
			               newvrt.TrkAtVrt,
			               newvrt.Chi2);   
           if( sc.isFailure() )  continue;  
           newvrt.nCloseVrt    = 0;
           newvrt.dCloseVrt    = 1000000.;
           WrkVrtSet->push_back(newvrt);
      }
      (*WrkVrtSet)[iv].SelTrk.erase( (*WrkVrtSet)[iv].SelTrk.begin() + SelT ); //remove track
      sc = RefitVertex( WrkVrtSet, iv, AllTracks);
      if( sc.isFailure() )  std::cout<<" Wrong vertex"<<'\n';
   }



   int InDetVKalVxInJetTool::CheckKilledVertices( WrkVrt *KilledVertex,
                                                  std::vector<WrkVrt> *VrtSet)
   const
   { 
      int trk1=KilledVertex->SelTrk[0];   // 2 -track killed vertex
      int trk2=KilledVertex->SelTrk[1];
      if( KilledVertex->Chi2PerTrk[0]>3.) trk1=-1;   // track-vertex assocoation is bad in any case
      if( KilledVertex->Chi2PerTrk[1]>3.) trk2=-1;
      int NSet=VrtSet->size();
      for(int iv=0; iv<NSet; iv++) {
         if( !(*VrtSet)[iv].Good) continue;
         int NTrkAtVrt=(*VrtSet)[iv].SelTrk.size();
         if(NTrkAtVrt<2) continue;
         for(int jt=0; jt<NTrkAtVrt; jt++){
           if( trk1==(*VrtSet)[iv].SelTrk[jt]) 	     return 2;  // Sharing found. Track 2 in nonshared
           if( trk2==(*VrtSet)[iv].SelTrk[jt])	     return 1;  // Sharing found. Track 1 in nonshared
         }
      }
      return 0;
   }

   void InDetVKalVxInJetTool::TrackClassification(std::vector<WrkVrt> *WrkVrtSet, 
                                             std::vector< std::deque<long int> > *TrkInVrt)
   const
   { 
      int NSet=WrkVrtSet->size(); int iv, jt, NTrkAtVrt, tracknum;
      for( iv=0; iv<NSet; iv++) {
         NTrkAtVrt=(*WrkVrtSet)[iv].SelTrk.size();
         if(NTrkAtVrt<2) continue;
         for( jt=0; jt<NTrkAtVrt; jt++){
           tracknum=(*WrkVrtSet)[iv].SelTrk[jt];
	   (*TrkInVrt).at(tracknum).push_back(iv);
         }
      }
   }


   double InDetVKalVxInJetTool::MaxOfShared(std::vector<WrkVrt> *WrkVrtSet, 
                                       std::vector< std::deque<long int> > *TrkInVrt,
				       long int & SelectedTrack,
				       long int & SelectedVertex)
   const
   {
      long int NTrack=TrkInVrt->size(); int it, jv, itmp, NVrt, VertexNumber, NTrk;
 
      double MaxOf=-999999999; double Chi2Red;
//
      long int NShMax=0;
      for( it=0; it<NTrack; it++) {
         NVrt=(*TrkInVrt)[it].size();         /*Number of vertices for this track*/
         if(  NVrt > NShMax ) NShMax=NVrt;
      }
      if(NShMax<=1)return MaxOf;              /* No shared tracks */
//      
      for( it=0; it<NTrack; it++) {
         NVrt=(*TrkInVrt)[it].size();         /*Number of vertices for this track*/
         if(  NVrt <= 1 )        continue;    /*Should ALWAYS be checked for safety*/
         if(  NVrt < NShMax )    continue;    /*Not a shared track with maximal sharing*/
         for( jv=0; jv<NVrt; jv++ ){
	    VertexNumber=(*TrkInVrt)[it][jv];
	    int NTrkInVrt=(*WrkVrtSet).at(VertexNumber).SelTrk.size();
	    if( NTrkInVrt <= 1) continue;                              // one track vertex - nothing to do
	    NTrk=(*WrkVrtSet)[VertexNumber].SelTrk.size();
	    for( itmp=0; itmp<NTrk; itmp++){
	       if( (*WrkVrtSet)[VertexNumber].SelTrk[itmp] == it ) {         /* Track found*/        
                //Chi2Red=(*WrkVrtSet)[VertexNumber].Chi2PerTrk.at(itmp)/m_chiScale[(NTrk<10?NTrk:10)];  //   Reduced Chi2
	          Chi2Red=(*WrkVrtSet)[VertexNumber].Chi2PerTrk.at(itmp)/(1.-3./(2.*NTrkInVrt));         //   Reduced Chi2
	        //if(NTrkInVrt==2){ Chi2Red += 30./((*WrkVrtSet)[VertexNumber].vertex.perp()+5.);}       //VK Reduce vrt multiplicity. May decrease fake rate
                  if( MaxOf < Chi2Red ){
		      MaxOf = Chi2Red;
		      SelectedTrack=it;
		      SelectedVertex=VertexNumber;
		  }
               }
            }
	 }
      }
      return MaxOf;
   }


    void InDetVKalVxInJetTool::RemoveTrackFromVertex(std::vector<WrkVrt> *WrkVrtSet, 
                                       std::vector< std::deque<long int> > *TrkInVrt,
				       long int & SelectedTrack,
				       long int & SelectedVertex)
    const
    {   
        int posInVrtFit=0;                    //Position of SelectedTrack in vertex fit track list.
	std::deque<long int>::iterator it;
//std::cout<<" In Found ="<<SelectedTrack<<", "<<SelectedVertex<<'\n';
	for(it=(*WrkVrtSet).at(SelectedVertex).SelTrk.begin(); 
	    it!=(*WrkVrtSet)[SelectedVertex].SelTrk.end();     it++) {
	    if( (*it) == SelectedTrack ) { 
	       (*WrkVrtSet)[SelectedVertex].SelTrk.erase(it);
	       break;
	    }     
            posInVrtFit++;
	}   

	for(it=(*TrkInVrt).at(SelectedTrack).begin(); 
	    it!=(*TrkInVrt)[SelectedTrack].end();     it++) {
	    if( (*it) == SelectedVertex ) { 
	       (*TrkInVrt)[SelectedTrack].erase(it);
	       break;
	    }     
	}   
//Check if track is removed from 2tr vertex => then sharing of track left should also be decreased
        if( (*WrkVrtSet)[SelectedVertex].SelTrk.size() == 1){
	   long int LeftTrack=(*WrkVrtSet)[SelectedVertex].SelTrk[0];  // track left in 1tr vertex
	   for(it=(*TrkInVrt).at(LeftTrack).begin();  it!=(*TrkInVrt)[LeftTrack].end();  it++) {
	      if( (*it) == SelectedVertex ) { 
	       (*TrkInVrt)[LeftTrack].erase(it); break;
	      }     
	   }   
           int ipos=0; if(posInVrtFit==0)ipos=1;  // Position of remaining track in previous 2track vertex fit
	   (*WrkVrtSet)[SelectedVertex].vertexMom=MomAtVrt((*WrkVrtSet)[SelectedVertex].TrkAtVrt[ipos]); //Redefine vertexMom using remaining track
	   if((*TrkInVrt)[LeftTrack].size()>0)(*WrkVrtSet)[SelectedVertex].Good=false;    //Vertex is declared false if remaining track 
	    										  // is still linked to another vertex
        }


   }

//
//  Minimal normalized vertex-vertex distance
//
   double InDetVKalVxInJetTool::minVrtVrtDist( std::vector<WrkVrt> *WrkVrtSet, int & V1, int & V2)
   const
   {  
     for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) { (*WrkVrtSet)[iv].dCloseVrt=1000000.; (*WrkVrtSet)[iv].nCloseVrt=0;}
     
     double foundMinVrtDst=1000000.;
     for(int iv=0; iv<(int)WrkVrtSet->size()-1; iv++) {
        if( (*WrkVrtSet).at(iv).SelTrk.size()< 2) continue;   /* Bad vertices */
        for(int jv=iv+1; jv<(int)WrkVrtSet->size(); jv++) {
           if( (*WrkVrtSet).at(jv).SelTrk.size()< 2) continue;   /* Bad vertices */
           double tmp= fabs((*WrkVrtSet)[iv].vertex.x()-(*WrkVrtSet)[jv].vertex.x())
                      +fabs((*WrkVrtSet)[iv].vertex.y()-(*WrkVrtSet)[jv].vertex.y())
                      +fabs((*WrkVrtSet)[iv].vertex.z()-(*WrkVrtSet)[jv].vertex.z());
           if(tmp>15.) continue;
           double tmpDst = VrtVrtDist((*WrkVrtSet)[iv].vertex,(*WrkVrtSet)[iv].vertexCov,
                                      (*WrkVrtSet)[jv].vertex,(*WrkVrtSet)[jv].vertexCov);
           if( tmpDst < foundMinVrtDst){foundMinVrtDst = tmpDst; V1=iv; V2=jv;} 
           if( tmpDst < (*WrkVrtSet)[iv].dCloseVrt ) {(*WrkVrtSet)[iv].dCloseVrt=tmpDst; (*WrkVrtSet)[iv].nCloseVrt=jv;}
           if( tmpDst < (*WrkVrtSet)[jv].dCloseVrt ) {(*WrkVrtSet)[jv].dCloseVrt=tmpDst; (*WrkVrtSet)[jv].nCloseVrt=iv;}
         }
      }
      return foundMinVrtDst;
   }
//   
// Give minimal distance between nonmodifed yet vertices
//   
   double InDetVKalVxInJetTool::minVrtVrtDistNext( std::vector<WrkVrt> *WrkVrtSet, int & V1, int & V2)
   const
   {  
     V1=0; V2=0;
     double foundMinVrtDst=1000000.;
     for(int iv=0; iv<(int)WrkVrtSet->size()-1; iv++) {
        if( (*WrkVrtSet)[iv].SelTrk.size()< 2) continue;   /* Bad vertex */
        if( (*WrkVrtSet)[iv].nCloseVrt==0)     continue;   /* Used vertex */
        if( (*WrkVrtSet)[iv].dCloseVrt <foundMinVrtDst ) {
	   int vtmp=(*WrkVrtSet)[iv].nCloseVrt;
           if((*WrkVrtSet)[vtmp].nCloseVrt==0) { continue;}  // Close vertex to given [iv] one is modified already 
	   foundMinVrtDst=(*WrkVrtSet)[iv].dCloseVrt;
	   V1=iv;
	   V2=vtmp;
	}
      }
      return foundMinVrtDst;
   }


//
//  Merge two close vertices into one (first) and set NTr=0 for second vertex
//

   void InDetVKalVxInJetTool::MergeVertices( std::vector<WrkVrt> *WrkVrtSet, int & V1, int & V2)
   const
   {

      int i;
      int nth=(*WrkVrtSet).at(V2).SelTrk.size();   //number of tracks in second vertex

      for(i=0;i<nth;i++) (*WrkVrtSet)[V1].SelTrk.push_back(   (*WrkVrtSet)[V2].SelTrk[i]   );
//
// Cleaning
      std::deque<long int>::iterator   TransfEnd ;
      sort( (*WrkVrtSet)[V1].SelTrk.begin(), (*WrkVrtSet)[V1].SelTrk.end() );
      TransfEnd =  unique((*WrkVrtSet)[V1].SelTrk.begin(), (*WrkVrtSet)[V1].SelTrk.end() );
      (*WrkVrtSet)[V1].SelTrk.erase( TransfEnd, (*WrkVrtSet)[V1].SelTrk.end());
//
//----------------------------------------------------------
      (*WrkVrtSet)[V2].SelTrk.clear();     //Clean dropped vertex
      (*WrkVrtSet)[V2].dCloseVrt=1000000.; //Clean dropped vertex
      (*WrkVrtSet)[V2].nCloseVrt=0;        //Clean dropped vertex
      (*WrkVrtSet)[V1].dCloseVrt=1000000.; //Clean new vertex
      (*WrkVrtSet)[V1].nCloseVrt=0;        //Clean new vertex

   }
 
   template <class Particle>
   StatusCode InDetVKalVxInJetTool::RefitVertex( std::vector<WrkVrt> *WrkVrtSet, int SelectedVertex,
                                            std::vector<const Particle*> & SelectedTracks)
   const
   {
      int i,j;
      int nth = (*WrkVrtSet)[SelectedVertex].SelTrk.size();
 
      if(nth<2) return StatusCode::SUCCESS;
 
      std::vector<const Particle*>  ListTracks;
      ListTracks.clear();
      for(i=0;i<nth;i++) {
	  j=(*WrkVrtSet)[SelectedVertex].SelTrk[i];                           /*Track number*/
          ListTracks.push_back( SelectedTracks[j] );
      }
      (*WrkVrtSet)[SelectedVertex].Good = false;
      (*WrkVrtSet)[SelectedVertex].Chi2PerTrk.resize(nth);
      for(i=0;i<nth;i++)(*WrkVrtSet)[SelectedVertex].Chi2PerTrk[i]=100000.+i; //VK safety

      m_fitSvc->setApproximateVertex((*WrkVrtSet)[SelectedVertex].vertex[0],
	                             (*WrkVrtSet)[SelectedVertex].vertex[1],
	                             (*WrkVrtSet)[SelectedVertex].vertex[2]);
      StatusCode SC=VKalVrtFitBase(ListTracks,
                                (*WrkVrtSet)[SelectedVertex].vertex,
                                (*WrkVrtSet)[SelectedVertex].vertexMom,
				(*WrkVrtSet)[SelectedVertex].vertexCharge,
		                (*WrkVrtSet)[SelectedVertex].vertexCov,
				(*WrkVrtSet)[SelectedVertex].Chi2PerTrk, 
				(*WrkVrtSet)[SelectedVertex].TrkAtVrt,
				(*WrkVrtSet)[SelectedVertex].Chi2); 
      if(SC.isSuccess())(*WrkVrtSet)[SelectedVertex].Good = true;
      return SC;
   }


   bool InDetVKalVxInJetTool::isPart( std::deque<long int> test, std::deque<long int> base)
   const
   {
      std::deque<long int>::iterator trk=test.begin();
      for(trk=test.begin(); trk!=test.end(); trk++)
         if(find(base.begin(), base.end(), (*trk)) == base.end()) return false;  //element not found => test is not part of base
      return true;
   }

} //end namespace
