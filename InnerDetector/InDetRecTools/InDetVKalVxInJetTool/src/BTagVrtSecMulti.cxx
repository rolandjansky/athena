/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "InDetVKalVxInJetTool/InDetVKalVxInJetTool.h"
//-------------------------------------------------
// Other stuff
#include  "GaudiKernel/ToolFactory.h"
#include  "AnalysisUtils/AnalysisMisc.h"
#include  "TrkParticleBase/TrackParticleBaseCollection.h"
#include  "TrkParticleCreator/TrackParticleCreatorTool.h"
#include  "GeoPrimitives/GeoPrimitivesHelpers.h"
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
//   7) Jet energy used in (2) calculation 
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

const double VrtBCMassLimit=6000.;  // Mass limit to consider a vertex not comoming from B,C-decays

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

      const double probVrtMergeLimit=0.04;

      m_NRefTrk=0;
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
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "InDet GetVrtSecMulti() called with NPart=" <<inpNPart<< endmsg;
   
      //std::vector<const Rec::TrackParticle*> listJetTracks, tmpListTracks, listSecondTracks, TracksForFit;
      std::vector<xAOD::Vertex*>  finalVertices; 

      if( inpNPart < 2 ) { return finalVertices;}   // 0,1 track => nothing to do!
   
   
      long int NTracks = 0;
      TLorentzVector MomentumJet;
      if     (xAODwrk) {  SelGoodTrkParticle( xAODwrk->InpTrk, PrimVrt, JetDir, xAODwrk->listJetTracks);
			  while( xAODwrk->listJetTracks.size() && xAODwrk->listJetTracks[0]->pt()/JetDir.Pt()>1.)
			         xAODwrk->listJetTracks.erase(xAODwrk->listJetTracks.begin());
                          NTracks = xAODwrk->listJetTracks.size();
                          MomentumJet = TotalMom(xAODwrk->listJetTracks);}
      else if(RECwork) {  SelGoodTrkParticle( RECwork->InpTrk, PrimVrt, JetDir, RECwork->listJetTracks);
			  while( RECwork->listJetTracks.size() && RECwork->listJetTracks[0]->pt()/JetDir.Pt()>1.)
			         RECwork->listJetTracks.erase(RECwork->listJetTracks.begin());
                          NTracks = RECwork->listJetTracks.size();
                          MomentumJet = TotalMom(GetPerigeeVector(RECwork->listJetTracks));}

      if(NTracks>m_TrackInJetNumberLimit){
        if     (xAODwrk ) xAODwrk->listJetTracks.resize(m_TrackInJetNumberLimit);
        else if(RECwork ) RECwork->listJetTracks.resize(m_TrackInJetNumberLimit);
        NTracks=m_TrackInJetNumberLimit;
      }
      if( NTracks < 2 ) { return finalVertices;} // 0,1 selected track => nothing to do!

      if(xAODwrk){
          while( xAODwrk->listJetTracks.size()>4 && medianPtF(xAODwrk->listJetTracks)/JetDir.Pt()<0.01) 
             xAODwrk->listJetTracks.pop_back();
          NTracks = xAODwrk->listJetTracks.size();
      }
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Number of selected tracks inside jet= " <<NTracks << endmsg;
      
      if(m_FillHist){m_hb_jmom->Fill( MomentumJet.Perp(), m_w_1);
                     m_hb_ntrkjet->Fill( (double) NTracks, m_w_1); }

//
//  InpTrk[]           - input track list
//  listJetTracks[]    - list of good tracks in jet for vertex search
//------------------------------------------------------------	 
//                     Initial track list ready
//                     Find 2track vertices
//
	 
      std::vector<double> InpMass(NTracks,m_massPi);
      double Vrt2TrackNumber = 0;

      if     (xAODwrk){
        Select2TrVrt(xAODwrk->listJetTracks, xAODwrk->TracksForFit, PrimVrt, JetDir, InpMass, xAODwrk->TrkFromV0, xAODwrk->listSecondTracks);
        if( xAODwrk->TrkFromV0.size() > 1) { RemoveDoubleEntries(xAODwrk->TrkFromV0); AnalysisUtils::Sort::pT (&(xAODwrk->TrkFromV0));}
        Vrt2TrackNumber = (double) xAODwrk->listSecondTracks.size()/2.;
        RemoveDoubleEntries(xAODwrk->listSecondTracks);
        AnalysisUtils::Sort::pT (&(xAODwrk->listSecondTracks));
      } else if(RECwork) {  
        Select2TrVrt(RECwork->listJetTracks, RECwork->TracksForFit, PrimVrt, JetDir, InpMass, RECwork->TrkFromV0, RECwork->listSecondTracks);
        if( RECwork->TrkFromV0.size() > 1) { RemoveDoubleEntries(RECwork->TrkFromV0); AnalysisUtils::Sort::pT (&(RECwork->TrkFromV0));}
        Vrt2TrackNumber = (double) RECwork->listSecondTracks.size()/2.;
        RemoveDoubleEntries(RECwork->listSecondTracks);
        AnalysisUtils::Sort::pT (&(RECwork->listSecondTracks));
      }

//      if(m_WorkArray->m_Incomp.size() < 2 ) { return finalVertices;}	// 0,1 tracks left VVK WRONG!!!
//
//---
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" Found different tracks in pairs="<< Vrt2TrackNumber<<endmsg;
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
      WrkVrt newvrt; newvrt.Good=true;
      m_fitSvc->setDefault();
      StatusCode sc; sc.setChecked(); 
      long int NPTR=0, nth=2; // VK nth=2 to speed up PGRAPH when it's used

//================================================ PGRAPH version
//      int iRet=0;
//      long int* weit     = new long int[m_WorkArray->m_Incomp.size()];
//      long int* Solution = new long int[NTracks];
//      for(i=0; i<(int)m_WorkArray->m_Incomp.size(); i++) *(weit+i)=(long int) (m_WorkArray->m_Incomp[i]+1); /* +1 is needed for PGRAPH*/
//      long int edges = m_WorkArray->m_Incomp.size()/2; 
//      while(true) {
//          iRet=Trk::pgraphm_( weit, &edges, &NTracks, Solution, &NPTR, &nth);
//          if( iRet != 0) { if(msgLvl(MSG::ERROR))msg(MSG::ERROR) << " Error in PGRAPHm iRet=" <<iRet << endmsg;}
//                                     if(NPTR <= 0)  break;      /* No more solutions   */
//                                     if(NPTR == 1)  continue;   /* Not a good solution */
//          newvrt.SelTrk.clear();
//          for(i=0;i<NPTR;i++) { newvrt.SelTrk.push_back(Solution[i]-1);}//std::cout<<"Solution="<<Solution[i]<<'\n';
//================================================== Boost version (don't forget to uncomment addEdge in Select2TrVrt()
      long int* weit=0; long int* Solution=0;
      std::vector< std::vector<int> > allCliques;
      bron_kerbosch_all_cliques(*m_compatibilityGraph, clique_visitor(allCliques));
      for(int cq=0; cq<(int)allCliques.size();cq++){
          newvrt.SelTrk.clear();
          NPTR=allCliques[cq].size();
          for(i=0;i<NPTR;i++){ newvrt.SelTrk.push_back(allCliques[cq][i]);}
//==================================================
          if(xAODwrk)xAODwrk->tmpListTracks.clear(); else if(RECwork)RECwork->tmpListTracks.clear();
          for(i=0;i<NPTR;i++) {
             if     (xAODwrk)xAODwrk->tmpListTracks.push_back( xAODwrk->listJetTracks.at(newvrt.SelTrk[i]) );
	     else if(RECwork)RECwork->tmpListTracks.push_back( RECwork->listJetTracks.at(newvrt.SelTrk[i]) );
          }
          if     (xAODwrk)sc = VKalVrtFitFastBase(xAODwrk->tmpListTracks,newvrt.vertex);
	  else if(RECwork)sc = VKalVrtFitFastBase(RECwork->tmpListTracks,newvrt.vertex);
          if( sc.isFailure() || newvrt.vertex.perp() > m_Rlayer2*2.  ) {   /* No initial estimation */ 
              m_fitSvc->setApproximateVertex(PrimVrt.x(), PrimVrt.y(), PrimVrt.z()); /*Use as starting point*/
              if( m_MultiWithPrimary ) m_fitSvc->setApproximateVertex(0., 0., 0.); 
 	  } else {
	      Amg::Vector3D vDist=newvrt.vertex-PrimVrt.position();
              double JetVrtDir = JetDir.Px()*vDist.x() + JetDir.Py()*vDist.y() + JetDir.Pz()*vDist.z();
              if( m_MultiWithPrimary ) JetVrtDir=fabs(JetVrtDir); /* Always positive when primary vertex is seeked for*/ 
              if( JetVrtDir>0. ) {                           /* Good initial estimation */ 
                  m_fitSvc->setApproximateVertex(newvrt.vertex.x(),newvrt.vertex.y(),newvrt.vertex.z()); /*Use as starting point*/
	      }else{
                  m_fitSvc->setApproximateVertex(PrimVrt.x(), PrimVrt.y(), PrimVrt.z()); 
              }
          }
//  std::cout<<"FoundAppVrt="<<newvrt.vertex[0]<<", "<<newvrt.vertex[1]<<", "<<newvrt.vertex[2]<<'\n';
	  sc.setCode(StatusCode::FAILURE);
          if     (xAODwrk){sc=VKalVrtFitBase(xAODwrk->tmpListTracks,
	                                     newvrt.vertex,     newvrt.vertexMom, newvrt.vertexCharge, newvrt.vertexCov,
                                             newvrt.Chi2PerTrk, newvrt.TrkAtVrt,  newvrt.Chi2);}
	  else if(RECwork){sc=VKalVrtFitBase(RECwork->tmpListTracks,
	                                     newvrt.vertex,     newvrt.vertexMom, newvrt.vertexCharge, newvrt.vertexCov,
                                             newvrt.Chi2PerTrk, newvrt.TrkAtVrt,  newvrt.Chi2);}
//  std::cout << "Res="<<newvrt.Chi2<<", "<<NPTR<<", "<<newvrt.SelTrk[0]<<", "<<newvrt.SelTrk[1]<<'\n'; 
          if( sc.isFailure() )           continue;   /* Bad fit - goto next solution */
          if(NPTR==2 && newvrt.Chi2>10.) continue;   /* Bad 2track vertex */
          if(newvrt.Chi2PerTrk.size()==2) newvrt.Chi2PerTrk[0]=newvrt.Chi2PerTrk[1]=newvrt.Chi2/2.;
          newvrt.Good         = true;
          newvrt.nCloseVrt    = 0;
          newvrt.dCloseVrt    = 1000000.;
          WrkVrtSet->push_back(newvrt);
    } 
//==================================================================================
// boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS>::vertex_iterator vertexIt, vertexEnd;
// boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS>::adjacency_iterator neighbourIt, neighbourEnd;
// tie(vertexIt, vertexEnd) = vertices(*m_compatibilityGraph);
// for (; vertexIt != vertexEnd; ++vertexIt) { std::cout << *vertexIt << " is connected with "; 
//    tie(neighbourIt, neighbourEnd) = adjacent_vertices(*vertexIt, *m_compatibilityGraph); 
//    for (; neighbourIt != neighbourEnd; ++neighbourIt) std::cout << *neighbourIt << " ";   std::cout << "\n"; }
//==================================================================================
//------------ Debug
//    std::vector<const xAOD::TrackParticle*> tempTrk(0);
//    for(auto & iv : (*WrkVrtSet)){ if(iv.Good){for(auto & it : iv.SelTrk)tempTrk.push_back(xAODwrk->listJetTracks.at(it));}}
//    RemoveDoubleEntries(tempTrk);
//--- Initial cleaning of solutions
//
//- Try to merge vertices with 3 and more tracks which have at least 2 common tracks
    int NSoluI=(*WrkVrtSet).size();
    for(int iv=0; iv<NSoluI-1; iv++ ){
       if(!(*WrkVrtSet)[iv].Good )                continue;
       int nTrk1=(*WrkVrtSet)[iv].SelTrk.size();
       for(int jv=iv+1; jv<NSoluI; jv++){
         if(!(*WrkVrtSet)[jv].Good )              continue;
         int nTrk2=(*WrkVrtSet)[jv].SelTrk.size();
         if( nTrk1<3 || nTrk2<3 || nTrk1+nTrk2<7) continue; 
         int nTCom=nTrkCommon( WrkVrtSet, iv, jv);
         if( nTrk1-nTCom==1 || nTrk2-nTCom==1){
	    double probV=-1.;
            if     (xAODwrk)probV=mergeAndRefitVertices( WrkVrtSet, iv, jv, newvrt, xAODwrk->listJetTracks);
            else if(RECwork)probV=mergeAndRefitVertices( WrkVrtSet, iv, jv, newvrt, RECwork->listJetTracks);
	    if(probV<probVrtMergeLimit)continue;
            newvrt.Good = true;
            (*WrkVrtSet).push_back(newvrt);
	    (*WrkVrtSet)[iv].Good=false;      
	    (*WrkVrtSet)[jv].Good=false;      
         }
       }
    }
//-Remove worst track from vertices with very bad Chi2
    NSoluI=(*WrkVrtSet).size();
    for(int iv=0; iv<NSoluI; iv++){
       if(!(*WrkVrtSet)[iv].Good )               continue;
       if( (*WrkVrtSet)[iv].SelTrk.size() == 2 ) continue;
       //if( (*WrkVrtSet)[iv].Chi2 > (5.*(*WrkVrtSet)[iv].SelTrk.size()) ){
       if( TMath::Prob( (*WrkVrtSet)[iv].Chi2, 2*(*WrkVrtSet)[iv].SelTrk.size()-3) <1.e-2){
          if     (xAODwrk)DisassembleVertex(WrkVrtSet,iv,xAODwrk->listJetTracks);
          else if(RECwork)DisassembleVertex(WrkVrtSet,iv,RECwork->listJetTracks);
       }
    }
//-Remove vertices fully contained in other vertices 
    while( (*WrkVrtSet).size()>1 ){
      int tmpN=(*WrkVrtSet).size();  int iv=0;
      for(; iv<tmpN-1; iv++){        int jv=iv+1;
        if(!(*WrkVrtSet)[iv].Good )               continue;
        for(; jv<tmpN; jv++){
          if(!(*WrkVrtSet)[jv].Good )             continue;
          int nTCom=nTrkCommon( WrkVrtSet, iv, jv);
          if(      nTCom==(int)(*WrkVrtSet)[iv].SelTrk.size()){  (*WrkVrtSet).erase((*WrkVrtSet).begin()+iv); break; }
          else if( nTCom==(int)(*WrkVrtSet)[jv].SelTrk.size()){  (*WrkVrtSet).erase((*WrkVrtSet).begin()+jv); break; }
        }   if(jv!=tmpN)   break;  // One vertex is erased. Restart check
      }     if(iv==tmpN-1) break;  // No vertex deleted
    }
    if(m_FillHist){ int cvgood=0; for(int iv=0; iv<(int)(*WrkVrtSet).size(); iv++) if((*WrkVrtSet)[iv].Good)cvgood++;
                    m_hb_rawVrtN->Fill( (float)cvgood, m_w_1); }
//- Try to merge all vertices which have at least 2 common track
    for(int iv=0; iv<(int)(*WrkVrtSet).size()-1; iv++ ){
       if(!(*WrkVrtSet)[iv].Good )           continue;
       for(int jv=iv+1; jv<(int)(*WrkVrtSet).size(); jv++){
         if(!(*WrkVrtSet)[jv].Good )           continue;
         if(nTrkCommon( WrkVrtSet, iv, jv)<2)  continue;
         if( VrtVrtDist((*WrkVrtSet)[iv].vertex,(*WrkVrtSet)[iv].vertexCov,
                        (*WrkVrtSet)[jv].vertex,(*WrkVrtSet)[jv].vertexCov) < m_VertexMergeCut) {
	    double probV=0.;
            if     (xAODwrk)probV=mergeAndRefitVertices( WrkVrtSet, iv, jv, newvrt, xAODwrk->listJetTracks);
            else if(RECwork)probV=mergeAndRefitVertices( WrkVrtSet, iv, jv, newvrt, RECwork->listJetTracks);
	    if(probV>probVrtMergeLimit){        //  Good merged vertex found
              double tstDst=JetProjDist(newvrt.vertex, PrimVrt, JetDir);
	      if(tstDst>0.){                               // only positive vertex directions are accepted as merging result
                (*WrkVrtSet).push_back(newvrt);
	        (*WrkVrtSet)[iv].Good=false;      
	        (*WrkVrtSet)[jv].Good=false;      
	        break;
              }
            }
         }
       }
    }
//-Identify/remove vertices behind the PV wrt jet direction
//-Identify remaining 2-track vertices with very bad Chi2 and mass (b-tagging)
//    for(int iv=0; iv<(int)(*WrkVrtSet).size(); iv++ ){
//       if(!(*WrkVrtSet)[iv].Good )  continue;
//       (*WrkVrtSet)[iv].ProjectedVrt=JetProjDist((*WrkVrtSet)[iv].vertex, PrimVrt, JetDir);
//       if( (*WrkVrtSet)[iv].SelTrk.size()!=2)continue;
//       if( (*WrkVrtSet)[iv].ProjectedVrt <0.) (*WrkVrtSet)[iv].Good =false;
//       if( (*WrkVrtSet)[iv].Chi2 > 10.) (*WrkVrtSet)[iv].Good=false;
//       if( (*WrkVrtSet)[iv].vertexMom.M()>VrtBCMassLimit) (*WrkVrtSet)[iv].Good=false; //VK B-tagging specific requirement
//     }      
//-Remove all bad vertices from the working set    
    int tmpV=0; while( tmpV<(int)(*WrkVrtSet).size() )if( !(*WrkVrtSet)[tmpV].Good ) { (*WrkVrtSet).erase((*WrkVrtSet).begin()+tmpV);} else {tmpV++;}
    //printWrkSet(WrkVrtSet,"Interm");
//----------------------------------------------------------------------------
//   Add primary vertex
//
//     if     (xAODwrk)xAODwrk->tmpListTracks.clear(); else if(RECwork)RECwork->tmpListTracks.clear();
//     newvrt.SelTrk.clear();
//     nth=m_WorkArray->m_Prmtrack.size();
//     int PrmVrtAdded=-1;
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

    int nGoodVertices=0;         // Final number of good vertices
    int n2trVrt=0;               // N vertices with 2 and more tracks
    std::vector< std::deque<long int> > *TrkInVrt  =new std::vector< std::deque<long int> >(NTracks);  
    double foundMaxT; long int SelectedTrack, SelectedVertex;
    int foundV1, foundV2;
    
    TrackClassification( WrkVrtSet, TrkInVrt);

    m_fitSvc->setDefault();
    while((foundMaxT=MaxOfShared( WrkVrtSet, TrkInVrt, SelectedTrack, SelectedVertex))>0) {
 //std::cout << "MAX="<<foundMaxT<<", "<<SelectedTrack<<", "<<SelectedVertex<<'\n';
 //std::cout << "VRT="<<minVrtVrtDist( WrkVrtSet, foundV1, foundV2)<<", "<<foundV1<<", "<<foundV2<<'\n';
 //printWrkSet(WrkVrtSet,"Iterat");

         double foundMinVrtDst = 1000000.;
         if(foundMaxT<m_TrackDetachCut) foundMinVrtDst = minVrtVrtDist( WrkVrtSet, foundV1, foundV2);

//Choice of action
          if( foundMaxT<m_TrackDetachCut && foundMinVrtDst<m_VertexMergeCut && nTrkCommon( WrkVrtSet, foundV1, foundV2)){
          //if( foundMaxT<m_TrackDetachCut && foundMinVrtDst<m_VertexMergeCut){
             bool vrtMerged=false;   //to check whether something is really merged
             while(foundMinVrtDst<m_VertexMergeCut){
               if(foundV1<foundV2) { int tmp=foundV1; foundV1=foundV2; foundV2=tmp;} /*Always drop vertex with smallest number*/
	       double probV=0.;
               if     (xAODwrk)probV=mergeAndRefitVertices( WrkVrtSet, foundV1, foundV2, newvrt, xAODwrk->listJetTracks);
               else if(RECwork)probV=mergeAndRefitVertices( WrkVrtSet, foundV1, foundV2, newvrt, RECwork->listJetTracks);
	       if(probV>probVrtMergeLimit && newvrt.vertexMom.M()<VrtBCMassLimit){        //  Good merged vertex found
                 double tstDst=JetProjDist(newvrt.vertex, PrimVrt, JetDir);
	         if(tstDst>0.){                               // only positive vertex directions are accepted as merging result
                   std::swap((*WrkVrtSet)[foundV1],newvrt);
                   (*WrkVrtSet)[foundV1].ProjectedVrt=tstDst;
	           (*WrkVrtSet)[foundV2].Good=false;         //Drop vertex
                   (*WrkVrtSet)[foundV2].SelTrk.clear();     //Clean dropped vertex
	           vrtMerged=true;
	         }
               }
               (*WrkVrtSet)[foundV1].nCloseVrt=-1; (*WrkVrtSet)[foundV1].dCloseVrt=1000000.; //For minVrtVrtDistNext optimisation(!)
               (*WrkVrtSet)[foundV2].nCloseVrt=-1; (*WrkVrtSet)[foundV2].dCloseVrt=1000000.; //Exclude given pair
                foundMinVrtDst=minVrtVrtDistNext( WrkVrtSet, foundV1, foundV2);     //Check other vertices
             }
	     if(vrtMerged){
               delete TrkInVrt;
               TrkInVrt = new std::vector< std::deque<long int> >(NTracks);  
               TrackClassification( WrkVrtSet, TrkInVrt);
	       continue;  // Something was merged => goto next cycle. Otherwise break the found track-vertex link
	     }
	  }
          RemoveTrackFromVertex(WrkVrtSet, TrkInVrt, SelectedTrack, SelectedVertex);
	  sc.setCode(0);
          if     (xAODwrk)sc = RefitVertex( WrkVrtSet, SelectedVertex, xAODwrk->listJetTracks);
          else if(RECwork)sc = RefitVertex( WrkVrtSet, SelectedVertex, RECwork->listJetTracks);
          (*WrkVrtSet)[SelectedVertex].ProjectedVrt=JetProjDist((*WrkVrtSet)[SelectedVertex].vertex, PrimVrt, JetDir);
          if( sc.isFailure() )  (*WrkVrtSet)[SelectedVertex].Good=false;  //bad vertex
          if( (*WrkVrtSet)[SelectedVertex].ProjectedVrt<0. && (*WrkVrtSet)[SelectedVertex].SelTrk.size()==2 )
	      (*WrkVrtSet)[SelectedVertex].Good=false;  // 2track vertex migrates behind PV - drop it.
          //std::cout<<"Dropped v="<<SelectedVertex<<" dst="<<(*WrkVrtSet)[SelectedVertex].ProjectedVrt<<'\n';
    }
//
// Final check/merge for close vertices
//
    double minDistVV=minVrtVrtDist( WrkVrtSet, foundV1, foundV2); //recalculate VV distances
    while ( minDistVV < m_VertexMergeCut) {
        if(foundV1<foundV2) { int tmp=foundV1; foundV1=foundV2; foundV2=tmp;}
	double probV=0.;
        if     (xAODwrk)probV=mergeAndRefitVertices( WrkVrtSet, foundV1, foundV2, newvrt, xAODwrk->listJetTracks);
        else if(RECwork)probV=mergeAndRefitVertices( WrkVrtSet, foundV1, foundV2, newvrt, RECwork->listJetTracks);
	if(probV>probVrtMergeLimit && newvrt.vertexMom.M()<VrtBCMassLimit){        //  Good merged vertex found
           double tstDst=JetProjDist(newvrt.vertex, PrimVrt, JetDir);
	   if(tstDst>0.){                               // only positive vertex directions are accepted as merging result
              std::swap((*WrkVrtSet)[foundV1],newvrt);
              (*WrkVrtSet)[foundV1].ProjectedVrt=tstDst;
	      (*WrkVrtSet)[foundV2].Good=false;         //Drop vertex
              (*WrkVrtSet)[foundV2].SelTrk.clear();     //Clean dropped vertex
           }
        }
        (*WrkVrtSet)[foundV1].nCloseVrt=-1; (*WrkVrtSet)[foundV1].dCloseVrt=1000000.; //For minVrtVrtDistNext optimisation(!)
        (*WrkVrtSet)[foundV2].nCloseVrt=-1; (*WrkVrtSet)[foundV2].dCloseVrt=1000000.; //Exclude given pair
        minDistVV=minVrtVrtDistNext( WrkVrtSet, foundV1, foundV2);
    }
//
// Try to improve vertices with big Chi2
    for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
       if(!(*WrkVrtSet)[iv].Good )                 continue;  //don't work on vertex which is already bad
       if( (*WrkVrtSet)[iv].SelTrk.size()<3 )      continue;
       double tmpProb=TMath::Prob( (*WrkVrtSet)[iv].Chi2, 2*(*WrkVrtSet)[iv].SelTrk.size()-3 ); //Chi2 of the original vertex
       if(tmpProb<0.001){
         if     (xAODwrk)tmpProb=improveVertexChi2( WrkVrtSet, iv, xAODwrk->listJetTracks);
         else if(RECwork)tmpProb=improveVertexChi2( WrkVrtSet, iv, RECwork->listJetTracks);
         if(tmpProb<0.001)(*WrkVrtSet)[iv].Good=false;
         (*WrkVrtSet)[iv].ProjectedVrt=JetProjDist((*WrkVrtSet)[iv].vertex, PrimVrt, JetDir);
       }
    }
    //printWrkSet(WrkVrtSet,"Joined");
//------------ Debug
//    std::vector<const xAOD::TrackParticle*> tempTrk(0);
//    for(auto & iv : (*WrkVrtSet)){ if(iv.Good){for(auto & it : iv.SelTrk)tempTrk.push_back(xAODwrk->listJetTracks.at(it));}}
//    RemoveDoubleEntries(tempTrk);
//--------------------------------------------------------------------------------------------------------
// Final vertex selection/cleaning
//
    m_fitSvc->setDefault();
    m_fitSvc->setMomCovCalc(1);
    double Signif3D, vProb=0.;
    //double Dist3D=0;

//--------- Start with 1-track vertices
//=First check if the track was detached from a multitrack vertex. If so - reattach. 
    for(auto &ntrVrt : (*WrkVrtSet)){ if(!ntrVrt.Good || ntrVrt.SelTrk.size()<=1)      continue;
      for(auto &onetVrt : (*WrkVrtSet)){ if(!onetVrt.Good || onetVrt.SelTrk.size()!=1) continue;
        if(ntrVrt.detachedTrack==onetVrt.SelTrk[0]){   
	  WrkVrt newV(ntrVrt); newV.SelTrk.push_back(ntrVrt.detachedTrack);
          if     (xAODwrk)vProb = RefitVertex( newV, xAODwrk->listJetTracks);
          else if(RECwork)vProb = RefitVertex( newV, RECwork->listJetTracks);
	  if(vProb>0.01){ onetVrt.Good=false; ntrVrt=newV;  ntrVrt.detachedTrack=-1;}
	  break;
    } } }
//=Recheck if the track was detached from a 2track vertex. If so - reattach. 
    for(auto &iVrt : (*WrkVrtSet)){   if(!iVrt.Good || iVrt.SelTrk.size()!=1) continue;
      for(auto &jVrt : (*WrkVrtSet)){ if(!jVrt.Good || jVrt.SelTrk.size()!=1) continue;
        if(iVrt.detachedTrack==jVrt.SelTrk[0]){   
	  WrkVrt newV(iVrt); newV.SelTrk.push_back(iVrt.detachedTrack);
          if     (xAODwrk)vProb = RefitVertex( newV, xAODwrk->listJetTracks);
          else if(RECwork)vProb = RefitVertex( newV, RECwork->listJetTracks);
	  if(vProb>0.01){ jVrt.Good=false; iVrt=newV;  iVrt.detachedTrack=-1;}
	  break;
    } } }
    for(auto &ntrVrt : (*WrkVrtSet)){ if(!ntrVrt.Good  || ntrVrt.SelTrk.size()<=1)  continue;
      for(auto tr : ntrVrt.SelTrk){ 
        for(auto &onetVrt  : (*WrkVrtSet)){ if(!onetVrt.Good || onetVrt.SelTrk.size()!=1) continue;
	  if(onetVrt.detachedTrack==tr){
	    WrkVrt newV(ntrVrt); newV.SelTrk.push_back(onetVrt.SelTrk[0]);
            if     (xAODwrk)vProb = RefitVertex( newV, xAODwrk->listJetTracks);
            else if(RECwork)vProb = RefitVertex( newV, RECwork->listJetTracks);
	    if(vProb>0.01){ onetVrt.Good=false; ntrVrt=newV;}
    } } } }
    for(auto & curVrt : (*WrkVrtSet) ) {
       if(!curVrt.Good )                 continue;  //don't work on vertex which is already bad
       if( fabs(curVrt.vertex.z())>650. ){curVrt.Good=false; continue;}  //vertex outside Pixel det. For ALL vertices
       if(curVrt.SelTrk.size() != 1)     continue;
       curVrt.Good=false;       // Make them bad by default
       if(MomAtVrt(curVrt.TrkAtVrt[0]).Pt() < TMath::Max(m_hadronIntPtCut,m_JetPtFractionCut*JetDir.Perp()) ) continue; //Low Pt
       if(m_MultiWithOneTrkVrt){          /* 1track vertices left unassigned from good 2tr vertices */
          VrtVrtDist(PrimVrt,curVrt.vertex, curVrt.vertexCov, Signif3D); //VK non-projected Signif3D is worse
          double tmpProb=TMath::Prob( curVrt.Chi2, 1);                 //Chi2 of the original 2tr vertex
          bool trkGood=false;
          if     (RECwork)trkGood=Check1TrVertexInPixel(RECwork->listJetTracks[curVrt.SelTrk[0]],curVrt.vertex);
          else if(xAODwrk)trkGood=Check1TrVertexInPixel(xAODwrk->listJetTracks[curVrt.SelTrk[0]],curVrt.vertex);
          if(trkGood && tmpProb>0.01){  /* accept only good tracks coming from good 2tr vertex*/
             //if( m_useMaterialRejection && insideMatLayer(curVrt.vertex.x(),curVrt.vertex.y()) ) continue;
             std::vector<double> Impact,ImpactError;   double Signif3DP = 0;
             if     (xAODwrk) Signif3DP=m_fitSvc->VKalGetImpact(xAODwrk->listJetTracks[curVrt.SelTrk[0]],PrimVrt.position(), 1, Impact, ImpactError);
	     else if(RECwork) Signif3DP=m_fitSvc->VKalGetImpact(RECwork->listJetTracks[curVrt.SelTrk[0]],PrimVrt.position(), 1, Impact, ImpactError);
             if(m_FillHist&&curVrt.vertex.perp()>20.){m_hb_diffPS->Fill( Signif3DP, m_w_1); }
             if( Signif3DP>2.*m_TrkSigCut && Signif3D>m_Sel2VrtSigCut) curVrt.Good=true; // accept only tracks which are far from primary vertex
          }
       }
    }
//------------ Debug
//    std::vector<const xAOD::TrackParticle*> tempTrk(0);
//    for(auto & iv : (*WrkVrtSet)){ if(iv.Good){for(auto & it : iv.SelTrk)tempTrk.push_back(xAODwrk->listJetTracks.at(it));}}
//    RemoveDoubleEntries(tempTrk);
//
//-----  Vertices with >1 tracks
    for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
          WrkVrt & curVrt=(*WrkVrtSet)[iv];
          if(!curVrt.Good )                      continue;  //don't work on vertex which is already bad
          nth=(*WrkVrtSet)[iv].SelTrk.size(); if(nth == 1) continue;  // 1track vertices are treated already
          //Signif3Dproj=VrtVrtDist(PrimVrt, curVrt.vertex, curVrt.vertexCov, JetDir);
          VrtVrtDist(PrimVrt,curVrt.vertex, curVrt.vertexCov, Signif3D); //VK non-projected Signif3D is worse
 	  //std::cout<<" Solution="<<iv<<" Ntrk="<<nth<<'\n';
          if(xAODwrk)xAODwrk->tmpListTracks.resize(nth); else if(RECwork)RECwork->tmpListTracks.resize(nth);
          for(i=0;i<nth;i++) {
            if     (xAODwrk)xAODwrk->tmpListTracks[i]=xAODwrk->listJetTracks[curVrt.SelTrk[i]];
	    else if(RECwork)RECwork->tmpListTracks[i]=RECwork->listJetTracks[curVrt.SelTrk[i]];
          }
          (*WrkVrtSet)[iv].Good = false;                                     /* Make all vertices bad for futher selection */
          if(nth <= 1)                          continue;                    /* Definitely bad vertices */
          if((*WrkVrtSet)[iv].ProjectedVrt<0.)  continue;                    /* Remove vertices behind primary one */ 
          //VK   Refitting is not needed here - done previously. Option for safety
	  //sc.setCode(0);
          //if     (xAODwrk)sc = RefitVertex( WrkVrtSet, iv, xAODwrk->listJetTracks);
          //else if(RECwork)sc = RefitVertex( WrkVrtSet, iv, RECwork->listJetTracks);
          //if( sc.isFailure() )                                   continue;           /* Bad fit - goto next solution */
          //---------------------------------------------------------------------------         
          if( TMath::Prob( curVrt.Chi2, 2*nth-3)<0.001) continue;           /* Bad Chi2 of refitted vertex  */
//----
//        Dist3D=((*WrkVrtSet)[iv].vertex-PrimVrt.position()).mag();            /* Not needed currently*/
//        if(PrmVrtAdded && iv==PrmVrtAdded && Dist3D<3.5) continue;  /* Skip added primary vertex */
//-----------------------------------------------------------------------------------------
          if(nth==2 && m_useVertexCleaning){
            if(RECwork){
	       if(!Check2TrVertexInPixel(RECwork->tmpListTracks[0],RECwork->tmpListTracks[1],curVrt.vertex,Signif3D))continue;
            }else if(xAODwrk){
	       if(!Check2TrVertexInPixel(xAODwrk->tmpListTracks[0],xAODwrk->tmpListTracks[1],curVrt.vertex,Signif3D))continue;
            }
	  }
//
//---  Check interactions on pixel layers
//
          //if( curVrt.vertex.perp()>m_Rbeampipe-2. && curVrt.detachedTrack<0) {
          if( curVrt.vertex.perp()>m_Rbeampipe-2.) {
	    double minPt=1.e9;  for(int ti=0; ti<nth; ti++) minPt=TMath::Min(minPt,MomAtVrt(curVrt.TrkAtVrt[ti]).Pt());
	    double ptLim=TMath::Max(m_hadronIntPtCut,m_JetPtFractionCut*JetDir.Perp());
            if(m_FillHist){  
	       m_hb_totmass2T0->Fill( curVrt.vertexMom.M()-nth*m_massPi, m_w_1);
	       m_hb_r2d->Fill( curVrt.vertex.perp(), m_w_1);
            }
            if(m_useMaterialRejection && minPt<ptLim){
	      if( insideMatLayer(curVrt.vertex.x(),curVrt.vertex.y()) ) continue;
            }
	    //double dR=0; for(int mi=0;mi<nth-1;mi++)for(int mj=mi+1;mj<nth;mj++)
	    //         dR=TMath::Max(dR,MomAtVrt(curVrt.TrkAtVrt[mi]).DeltaR(MomAtVrt(curVrt.TrkAtVrt[mj])));
            //if(m_FillHist)m_hb_deltaRSVPV->Fill(dR,m_w_1);
            //if( m_killHighPtIBLFakes && curVrt.vertex.perp()<m_Rlayer1 && dR<0.015)continue;
	    if(Signif3D<20.) continue;
          }
//
//---  Check V0s and conversions
          if(nth==2 && curVrt.vertexCharge==0 && curVrt.detachedTrack<0){
             double mass_PiPi =  curVrt.vertexMom.M();  
             double mass_PPi  =  massV0(curVrt.TrkAtVrt,m_massP,m_massPi);
             double mass_EE   =  massV0(curVrt.TrkAtVrt,m_massE,m_massE);
             if(m_FillHist){ m_hb_massPiPi->Fill( mass_PiPi, m_w_1);
                             m_hb_massPPi ->Fill( mass_PPi,  m_w_1); 
                             if( curVrt.vertex.perp()>20.)m_hb_massEE  ->Fill( mass_EE,   m_w_1);  } 
 	     if( fabs(mass_PiPi-m_massK0) < 22.)     continue;
 	     if( fabs(mass_PPi-m_massLam) <  8.)     continue;
             if( mass_EE < 60. && curVrt.vertex.perp() > 20.) continue;
          }          
//---
	  if(m_FillHist){m_hb_sig3DTot->Fill( Signif3D, m_w_1); }
          if(Signif3D<m_Sel2VrtSigCut)continue;      //Main PV-SV distance quality cut 
//-----
//        float Dist2D= (*WrkVrtSet)[iv].vertex.perp();
//	  if(Dist2D<2.){
//            double tmpProb=0.;
//            if       (xAODwrk)tmpProb=FitVertexWithPV( WrkVrtSet, iv, PrimVrt, xAODwrk->listJetTracks);
//            else if(RECwork)tmpProb=FitVertexWithPV( WrkVrtSet, iv, PrimVrt, RECwork->listJetTracks);
//            if(m_FillHist){m_hb_trkPtMax->Fill( tmpProb*1.e5, m_w_1); }
//            if(tmpProb>0.01)continue; // Vertex can be associated with PV
//	  }
//---
          curVrt.Good = true;  /* Vertex is absolutely good */
          nGoodVertices++;
          if(nth>=2)n2trVrt++;
    }
//
//--Final cleaning of the 1-track vertices set. Must be behind all other cleanings.
    if(m_MultiWithOneTrkVrt) Clean1TrVertexSet(WrkVrtSet);
//------------ Debug
//    std::vector<const xAOD::TrackParticle*> tempTrk(0);
//    for(auto & iv : (*WrkVrtSet)){ if(iv.Good){for(auto & it : iv.SelTrk)tempTrk.push_back(xAODwrk->listJetTracks.at(it));}}
//    RemoveDoubleEntries(tempTrk);
//-------------------------------------------
//Checks
    std::vector<WrkVrt> GoodVertices(0);
    nGoodVertices=0;         // Final number of good vertices
    n2trVrt=0;                    // N vertices with 2 and more tracks
    int nNtrVrt=0;               // N vertices with 3 and more tracks
    for(auto & iv : (*WrkVrtSet) ) {
       nth=iv.SelTrk.size(); if(nth == 0) continue;   /* Definitely bad vertices */
       Amg::Vector3D tmpVec=iv.vertex-PrimVrt.position();
       TLorentzVector Momentum(tmpVec.x(),tmpVec.y(),tmpVec.z(),m_massPi);
       //if(Momentum.DeltaR(JetDir)>m_ConeForTag) iv.Good=false; /* Vertex outside jet cone??? Very bad cut*/
       if( iv.Good) {
	  nGoodVertices++;                                    
	  GoodVertices.emplace_back(iv);    /* add it */
	  if(nth==2)n2trVrt++;
          if(nth>=3)nNtrVrt++;
       }
    }
    if(nGoodVertices == 0 || (n2trVrt+nNtrVrt)==0){  // No good vertices at all
      delete WrkVrtSet;  delete TrkInVrt; if(weit)delete[] weit; if(Solution)delete[] Solution;
      return finalVertices;
    }
//
//sorting
    while(1){ bool swapDone=false;                              // Sort on XY distance from (0.,0.)
      for(int iv=0; iv<(int)GoodVertices.size()-1; iv++) {
        if( GoodVertices[iv].vertex.perp() > GoodVertices[iv+1].vertex.perp()){
	  std::swap( GoodVertices[iv], GoodVertices[iv+1] );
          swapDone=true; 
        }
      }    if(!swapDone) break;
    }
    while(1){ bool swapDone=false;                            // Then sort on Projected dist if R<20.
      for(int iv=0; iv<(int)GoodVertices.size()-1; iv++) {
        if(GoodVertices[iv+1].vertex.perp() > 400.) continue;
        if(GoodVertices[iv].ProjectedVrt > GoodVertices[iv+1].ProjectedVrt){
	  std::swap( GoodVertices[iv], GoodVertices[iv+1] );
          swapDone=true; 
        }
      }    if(!swapDone) break;
    }
    if(nGoodVertices>1){
      if( GoodVertices[1].vertexMom.M()-GoodVertices[0].vertexMom.M() > 5000.) std::swap( GoodVertices[0], GoodVertices[1] );
    }
    if(m_FillHist){m_hb_distVV->Fill( minVrtVrtDist( WrkVrtSet, foundV1, foundV2), m_w_1); }
//----------------------------------------------------------------------------------
//  Nonused tracks for one-track-vertex search
//
// VK - It tries to recover tracks which were already rejected on previous stages of algorithm.
//      Gain is negligible. The code is commented out to the end of this file for future... 
// VK - 27.04.2017 - new try
    std::vector<int> nonusedTrk(0);
    for(int trk=0; trk<NTracks; trk++){    bool present=false;
      for(auto iv : (*WrkVrtSet)){ 
          if(iv.Good){for(auto t_in_V : iv.SelTrk){if(trk==t_in_V){present=true; break;}}};
	  if(present)break;
      }        if(!present)nonusedTrk.push_back(trk);
    }
    struct MatchedSV { int indVrt; double Signif3D;}; std::vector<MatchedSV> matchSV(0);
    double Signif=0.; std::vector<double> Impact,ImpactError;
    for(int it=0; it<(int)nonusedTrk.size(); it++){
      MatchedSV tmpV; tmpV.Signif3D=1.e9;
      for(int iv=0; iv<(int)GoodVertices.size(); iv++){
        if(GoodVertices[iv].SelTrk.size()<2) continue;
        if     (RECwork)Signif = m_fitSvc->VKalGetImpact(RECwork->listJetTracks[nonusedTrk[it]], GoodVertices[iv].vertex, 1, Impact, ImpactError);
        else if(xAODwrk)Signif = m_fitSvc->VKalGetImpact(xAODwrk->listJetTracks[nonusedTrk[it]], GoodVertices[iv].vertex, 1, Impact, ImpactError);
        if(Signif<tmpV.Signif3D){tmpV.Signif3D=Signif; tmpV.indVrt=iv;}
      }
      matchSV.push_back(tmpV);
    }
    for(int iv=0; iv<(int)GoodVertices.size(); iv++){
      WrkVrt newV(GoodVertices[iv]); 
      bool addedT=false;   std::map<double,int> addTrk;
      for(int it=0; it<(int)nonusedTrk.size(); it++){ if(matchSV[it].indVrt==iv){addTrk[matchSV[it].Signif3D]=it;} }
      std::map<double,int>::iterator atrk=addTrk.begin();
      if(addTrk.size()>0){       if(atrk->first<4.){newV.SelTrk.push_back(nonusedTrk[atrk->second]);addedT=true;}}
      if(addTrk.size()>1){atrk++;if(atrk->first<4.){newV.SelTrk.push_back(nonusedTrk[atrk->second]);}}
      if(addedT){ if     (xAODwrk)vProb = RefitVertex( newV, xAODwrk->listJetTracks);
                  else if(RECwork)vProb = RefitVertex( newV, RECwork->listJetTracks); 
                  if(vProb>0.01)GoodVertices[iv]=newV;
                  else{ std::vector<WrkVrt> TestVertices(1,newV);
                        if     (xAODwrk)vProb=improveVertexChi2( &TestVertices, 0, xAODwrk->listJetTracks);
                        else if(RECwork)vProb=improveVertexChi2( &TestVertices, 0, RECwork->listJetTracks);
	                if(vProb>0.01)GoodVertices[iv]=TestVertices[0];
		  }
      } 
    }
//
    //printWrkSet(&GoodVertices,"FinalVrtSet");
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
             trackPt=pTvsDir(Amg::Vector3D(JetDir.Vect().X(),JetDir.Vect().X(),JetDir.Vect().Z()), GoodVertices[iv].TrkAtVrt[i]);
             if(trackPt>trackPtMax)trackPtMax=trackPt;
          }
          if( m_FillHist ){
            if(nth==1)m_hb_r1dc->Fill( GoodVertices[iv].vertex.perp(), m_w_1);
            if(nth==2)m_hb_r2dc->Fill( GoodVertices[iv].vertex.perp(), m_w_1);
            if(nth==3)m_hb_r3dc->Fill( GoodVertices[iv].vertex.perp(), m_w_1);
            if(nth> 3)m_hb_rNdc->Fill( GoodVertices[iv].vertex.perp(), m_w_1);
            Signif3D=VrtVrtDist(PrimVrt, GoodVertices[iv].vertex, GoodVertices[iv].vertexCov, JetDir);
            if( nth==2 ){
              if(GoodVertices[iv].vertexCharge==0){ m_hb_totmass2T1->Fill( GoodVertices[iv].vertexMom.M(), m_w_1);}
              else                                { m_hb_totmass2T2->Fill( GoodVertices[iv].vertexMom.M(), m_w_1);}
              m_hb_sig3D2tr->Fill( Signif3D , m_w_1);
              if(GoodVertices[iv].vertexCharge==0)m_hb_totmassEE->Fill(massV0(GoodVertices[iv].TrkAtVrt,m_massE,m_massE),m_w_1);
            } else if( nth==1){
              m_hb_sig3D1tr->Fill( Signif3D, m_w_1);
            } else {
              m_hb_sig3DNtr->Fill( Signif3D, m_w_1);
            }
          }
//
// xAOD::Vertex creation-----------------------------
          xAOD::Vertex * tmpVertex=new xAOD::Vertex();
          tmpVertex->makePrivateStore();
          tmpVertex->setPosition(GoodVertices[iv].vertex);
          int NERMS=GoodVertices[iv].vertexCov.size();
	  NERMS=6;
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
//===================Fake vertex with list of all tracks for optimisation
//    xAOD::Vertex * tmpVertex=new xAOD::Vertex();   tmpVertex->makePrivateStore();
//    tmpVertex->setPosition(Amg::Vector3D(0.,0.,0.));
//    std::vector<Trk::VxTrackAtVertex> & tmpVTAV=tmpVertex->vxTrackAtVertex();    tmpVTAV.clear();
//    for(auto ii : tempTrk) { tmpVTAV.push_back( Trk::VxTrackAtVertex(1, new Trk::Perigee( ii->perigeeParameters()) ) );
//        ElementLink<xAOD::TrackParticleContainer> TEL;  TEL.setElement( ii );
//         const xAOD::TrackParticleContainer* cont = (const xAOD::TrackParticleContainer* ) (ii->container() );
//	TEL.setStorableObject(*cont);     tmpVertex->addTrackAtVertex(TEL,1.);
//    }       finalVertices.push_back(tmpVertex);
//==============================================

    if(m_FillHist){m_hb_goodvrtN->Fill( (double)nGoodVertices, m_w_1); }
    if(nGoodVertices == 0){
      delete WrkVrtSet;
      delete TrkInVrt; if(weit)delete[] weit; if(Solution)delete[] Solution;
      return finalVertices;
    }

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
      Results.push_back(0.);                                        //6th  -  not clear what to use here -> return 0.
      Results.push_back(MomentumJet.E());                 //7th

      if(m_FillHist){m_hb_ratio->Fill( Results[1], m_w_1); }
      if(m_FillHist){m_hb_totmass->Fill( Results[0], m_w_1); }
      if(m_FillHist){m_hb_nvrt2->Fill( Results[2], m_w_1); }
      if(m_FillHist){m_hb_mom->Fill( MomentumJet.Perp(), m_w_1);} 

      delete WrkVrtSet; delete TrkInVrt; if(weit)delete[] weit; if(Solution)delete[] Solution;

      return finalVertices;




  }




















                      /* Service routines*/

//-----------------------------------------------------------------------------------
//  Detach the worst Chi2 track from the vertex and join it (if possible) with other track 
//    from the vertex into additional 2-track vertices
//
   template <class Particle>
   void InDetVKalVxInJetTool::DisassembleVertex(std::vector<WrkVrt> *WrkVrtSet, int iv, 
                         std::vector<const Particle*>  AllTracks)
   const
   {
      WrkVrt newvrt; newvrt.Good=true;
      std::vector<const Particle*>  ListBaseTracks;
      int NTrk=(*WrkVrtSet)[iv].SelTrk.size(), SelT=-1;
      double Chi2Max=0.;
      for(int i=0; i<NTrk; i++){
         if( (*WrkVrtSet)[iv].Chi2PerTrk[i]>Chi2Max) { Chi2Max=(*WrkVrtSet)[iv].Chi2PerTrk[i];  SelT=i;}
      }	    
      if(SelT==-1)return;
      StatusCode sc;
      int NVrtCur=WrkVrtSet->size();
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
           if( newvrt.Chi2>10.)  continue;  // Too bad 2-track vertex fit
           newvrt.Chi2PerTrk[0]=newvrt.Chi2PerTrk[1]=newvrt.Chi2/2.;
           newvrt.nCloseVrt    = 0;
           newvrt.dCloseVrt    = 1000000.;
           if((int)WrkVrtSet->size()==NVrtCur) { WrkVrtSet->push_back(newvrt); continue;}  // just the first added vertex
           if( (*WrkVrtSet).at(NVrtCur).Chi2<newvrt.Chi2 ) continue;  // previously added 2tr vertex was better
           WrkVrtSet->pop_back();
           WrkVrtSet->push_back(newvrt);
      }
      (*WrkVrtSet)[iv].SelTrk.erase( (*WrkVrtSet)[iv].SelTrk.begin() + SelT ); //remove track
      sc = RefitVertex( WrkVrtSet, iv, AllTracks);
      if( sc.isFailure() ) {(*WrkVrtSet)[iv].Good=false; /*std::cout<<" Wrong vertex"<<'\n';*/}
   }


   void InDetVKalVxInJetTool::Clean1TrVertexSet(std::vector<WrkVrt> *WrkVrtSet)
   const
   {
     std::vector<int> countVT(WrkVrtSet->size(),0);
     std::vector<int> linkedVrt(WrkVrtSet->size(),0);
//--- Mark as bad the 1track vertex if the detached track is NOT present in any remaining good vertex (>=2tr)    
     for(int i1tv=0; i1tv<(int)WrkVrtSet->size(); i1tv++) {
       if( (*WrkVrtSet)[i1tv].SelTrk.size()!=1) continue;
       if(!(*WrkVrtSet)[i1tv].Good)             continue;   
       int Trk1=(*WrkVrtSet)[i1tv].detachedTrack; 
       int foundInGoodVrt=0;
       for(int mtv=0; mtv<(int)WrkVrtSet->size(); mtv++) {                        //cycle over good vertices with many tracks
         if( (*WrkVrtSet)[mtv].SelTrk.size()<2) continue;
	 if(!(*WrkVrtSet)[mtv].Good)            continue;   
         if( std::find((*WrkVrtSet)[mtv].SelTrk.begin(),(*WrkVrtSet)[mtv].SelTrk.end(), Trk1) != (*WrkVrtSet)[mtv].SelTrk.end()){
           //double m2Vrt=((*WrkVrtSet)[mtv].vertexMom+(*WrkVrtSet)[i1tv].vertexMom).M(); //VK Commented.  M cut in other places
           //if(m2Vrt>VrtBCMassLimit){ (*WrkVrtSet)[i1tv].Good=false;  break; } //1Tr + manyTr system is too heavy
	   foundInGoodVrt++; countVT[mtv]++; linkedVrt[i1tv]=mtv;  //Linked vertex found
         }
       }
       if(!foundInGoodVrt)(*WrkVrtSet)[i1tv].Good=false;             // Make the vertex bad
     }
//
//---Select SINGLE 1tr-vertex from many pointing to one multi-track vertex
     for(int mtv=0; mtv<(int)WrkVrtSet->size(); mtv++) {
       if( (*WrkVrtSet)[mtv].SelTrk.size()<2 ) continue;
       if(!(*WrkVrtSet)[mtv].Good )            continue;   
       if(      countVT[mtv] < 2 )             continue;
       double distM=1000000.;
       int    best1TVrt=-1;
       for(int i1tv=0; i1tv<(int)WrkVrtSet->size(); i1tv++) {
         if( (*WrkVrtSet)[i1tv].SelTrk.size()!=1 ) continue;
         if(!(*WrkVrtSet)[i1tv].Good )             continue;   
	 if( linkedVrt[i1tv] != mtv )              continue;
         double dist=((*WrkVrtSet)[mtv].vertex - (*WrkVrtSet)[i1tv].vertex).mag();
         if( dist < distM ) {distM=dist; best1TVrt=i1tv;}
         (*WrkVrtSet)[i1tv].Good=false;   
       }
       if(best1TVrt>-1)(*WrkVrtSet)[best1TVrt].Good=true;
     }
//
//---  Final Chi2 (last 2track vertex with the given track)
     for(int i1tv=0; i1tv<(int)WrkVrtSet->size(); i1tv++) {
        if( (*WrkVrtSet)[i1tv].SelTrk.size()!=1 || !(*WrkVrtSet)[i1tv].Good )                continue;   
	if( (*WrkVrtSet)[i1tv].Chi2>8.)	  (*WrkVrtSet)[i1tv].Good=false;
     }
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
      long int NTrack=TrkInVrt->size(); int it, jv, itmp, NVrt, VertexNumber;
 
      double MaxOf=-999999999; double Chi2Red; double SelectedProb=-1.;
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
	    for( itmp=0; itmp<NTrkInVrt; itmp++){
	       if( (*WrkVrtSet)[VertexNumber].SelTrk[itmp] == it ) {         /* Track found*/        
                //Chi2Red=(*WrkVrtSet)[VertexNumber].Chi2PerTrk.at(itmp)/m_chiScale[(NTrkInVrt<10?NTrkInVrt:10)]; //   Reduced Chi2
	        //if(NTrkInVrt==2){ Chi2Red += 30./((*WrkVrtSet)[VertexNumber].vertex.perp()+5.);}       //VK Reduce vrt multiplicity. May decrease fake rate
	          Chi2Red=(*WrkVrtSet)[VertexNumber].Chi2PerTrk.at(itmp);            //   Normal Chi2 seems the best
                  if(NTrkInVrt==2){
		    Chi2Red=(*WrkVrtSet)[VertexNumber].Chi2/2.;                     //VK 2track vertices with Normal Chi2Red
	            if((*WrkVrtSet)[VertexNumber].vertexMom.M()>VrtBCMassLimit)Chi2Red=100.; //VK break immediately very heavy 2tr vertices
                  }
                  double prob_vrt = TMath::Prob( (*WrkVrtSet)[VertexNumber].Chi2, 2*(*WrkVrtSet)[VertexNumber].SelTrk.size()-3);
                  if( MaxOf < Chi2Red ){
		      if(MaxOf>0 && prob_vrt>0.01 && SelectedProb<0.01 ) continue; // Don't disassemble good vertices is a bad one is present
		      MaxOf = Chi2Red;
		      SelectedTrack=it; SelectedVertex=VertexNumber;
                      SelectedProb = prob_vrt;
		  }
               }
            }
	 }
      }
//-- Additional check for a common track in 2tr-2tr vertex topology
      if( (*TrkInVrt)[SelectedTrack].size() == 2){
          int v1=(*TrkInVrt)[SelectedTrack][0]; int v2=(*TrkInVrt)[SelectedTrack][1];
          if( (*WrkVrtSet)[v1].SelTrk.size()==2 && (*WrkVrtSet)[v2].SelTrk.size()==2){
            if( (*WrkVrtSet)[SelectedVertex].Chi2 < TMath::ChisquareQuantile(0.9, 1) ){    // Probability > 10%!!!
              //double vr1=(*WrkVrtSet)[v1].vertex.perp(); double vr2=(*WrkVrtSet)[v2].vertex.perp();
              double M1=(*WrkVrtSet)[v1].vertexMom.M();  double M2=(*WrkVrtSet)[v2].vertexMom.M();
              if     (SelectedVertex==v1 && M2<M1)  SelectedVertex=v2;  // Swap to remove the lightest vertex
              else if(SelectedVertex==v2 && M1<M2)  SelectedVertex=v1;  // Swap to remove the lightest vertex
              if( M1>VrtBCMassLimit && M2<VrtBCMassLimit ) SelectedVertex=v1;
              if( M1<VrtBCMassLimit && M2>VrtBCMassLimit ) SelectedVertex=v2;
            }
	  }
          if( (*WrkVrtSet)[v1].SelTrk.size()+(*WrkVrtSet)[v2].SelTrk.size() > 4){
            if(TMath::Prob( (*WrkVrtSet)[v1].Chi2, 2*(*WrkVrtSet)[v1].SelTrk.size()-3)>0.05 && 
               TMath::Prob( (*WrkVrtSet)[v2].Chi2, 2*(*WrkVrtSet)[v2].SelTrk.size()-3)>0.05){
	       if( (*WrkVrtSet)[v1].SelTrk.size()==2 ) SelectedVertex=v1;
	       if( (*WrkVrtSet)[v2].SelTrk.size()==2 ) SelectedVertex=v2;
          } }	
      }
//
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
	       (*WrkVrtSet)[SelectedVertex].SelTrk.erase(it); break;
	    }     
            posInVrtFit++;
	}   

	for(it=(*TrkInVrt).at(SelectedTrack).begin(); 
	    it!=(*TrkInVrt)[SelectedTrack].end();     it++) {
	    if( (*it) == SelectedVertex ) { 
	       (*TrkInVrt)[SelectedTrack].erase(it); break;
	    }     
	}   
        (*WrkVrtSet)[SelectedVertex].detachedTrack=SelectedTrack;
//Check if track is removed from 2tr vertex => then sharing of track left should also be decreased
        if( (*WrkVrtSet)[SelectedVertex].SelTrk.size() == 1){
	   long int LeftTrack=(*WrkVrtSet)[SelectedVertex].SelTrk[0];  // track left in 1tr vertex
	   for(it=(*TrkInVrt).at(LeftTrack).begin();  it!=(*TrkInVrt)[LeftTrack].end();  it++) {
	      if( (*it) == SelectedVertex ) { 
	       (*TrkInVrt)[LeftTrack].erase(it); break;
	      }     
	   }   
	   if( (*WrkVrtSet)[SelectedVertex].vertexMom.M()>VrtBCMassLimit)(*WrkVrtSet)[SelectedVertex].Good=false; // Vertex is too heavy
           int ipos=0; if(posInVrtFit==0)ipos=1;  // Position of remaining track in previous 2track vertex fit
	   (*WrkVrtSet)[SelectedVertex].vertexMom=MomAtVrt((*WrkVrtSet)[SelectedVertex].TrkAtVrt[ipos]); //Redefine vertexMom using remaining track
	   if((*TrkInVrt)[LeftTrack].size()>0)(*WrkVrtSet)[SelectedVertex].Good=false;    //Vertex is declared false only if remaining track 
	    										  // is still linked to another vertex
	   (*WrkVrtSet)[SelectedVertex].TrkAtVrt.erase((*WrkVrtSet)[SelectedVertex].TrkAtVrt.begin()+posInVrtFit);  //Remove also TrkAtVrt
        }

   }
//
//  Number of common tracks for 2 vertices
//
   int InDetVKalVxInJetTool::nTrkCommon( std::vector<WrkVrt> *WrkVrtSet, 
                                         int V1, int V2)
   const
   {
      int NTrk_V1 = (*WrkVrtSet).at(V1).SelTrk.size(); if( NTrk_V1< 2) return 0;   /* Bad vertex */
      int NTrk_V2 = (*WrkVrtSet).at(V2).SelTrk.size(); if( NTrk_V2< 2) return 0;   /* Bad vertex */
      int nTrkCom=0;
      if(NTrk_V1 < NTrk_V2){
        for(int i=0; i<NTrk_V1; i++){
          int trk=(*WrkVrtSet)[V1].SelTrk[i];
          if( std::find((*WrkVrtSet)[V2].SelTrk.begin(),(*WrkVrtSet)[V2].SelTrk.end(),trk) != (*WrkVrtSet)[V2].SelTrk.end()) nTrkCom++;
        }
      }else{
        for(int i=0; i<NTrk_V2; i++){
          int trk=(*WrkVrtSet)[V2].SelTrk[i];
          if( std::find((*WrkVrtSet)[V1].SelTrk.begin(),(*WrkVrtSet)[V1].SelTrk.end(),trk) != (*WrkVrtSet)[V1].SelTrk.end()) nTrkCom++;
        }
      }
      return nTrkCom;
   }
//
//  Minimal normalized vertex-vertex distance
//
   double InDetVKalVxInJetTool::minVrtVrtDist( std::vector<WrkVrt> *WrkVrtSet, int & V1, int & V2)
   const
   {  
     V1=V2=0;
     for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) { (*WrkVrtSet).at(iv).dCloseVrt=1000000.; (*WrkVrtSet)[iv].nCloseVrt=-1;}
     double foundMinVrtDst=1000000.;

     for(int iv=0; iv<(int)WrkVrtSet->size()-1; iv++) {
        if( (*WrkVrtSet)[iv].SelTrk.size()< 2)    continue;   /* Bad vertices */
        if(!(*WrkVrtSet)[iv].Good )               continue;   /* Bad vertices */
        for(int jv=iv+1; jv<(int)WrkVrtSet->size(); jv++) {
           if( (*WrkVrtSet)[jv].SelTrk.size()< 2) continue;   /* Bad vertices */
           if(!(*WrkVrtSet)[jv].Good )            continue;   /* Bad vertices */
           //if(!nTrkCommon(WrkVrtSet, V1, V2))     continue;   /* No common tracks*/
           double tmp= fabs((*WrkVrtSet)[iv].vertex.x()-(*WrkVrtSet)[jv].vertex.x())
                      +fabs((*WrkVrtSet)[iv].vertex.y()-(*WrkVrtSet)[jv].vertex.y())
                      +fabs((*WrkVrtSet)[iv].vertex.z()-(*WrkVrtSet)[jv].vertex.z());
           if(tmp>20.) continue;
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
        if( (*WrkVrtSet)[iv].nCloseVrt==-1)    continue;   /* Used vertex */
        if( (*WrkVrtSet)[iv].dCloseVrt <foundMinVrtDst ) {
	   int vtmp=(*WrkVrtSet)[iv].nCloseVrt;
           if((*WrkVrtSet)[vtmp].nCloseVrt==-1) { continue;}  // Close vertex to given [iv] one is modified already 
	   foundMinVrtDst=(*WrkVrtSet)[iv].dCloseVrt;
	   V1=iv;
	   V2=vtmp;
	}
      }
      return foundMinVrtDst;
   }

//
//  Check two close vertices by explicit vertex fit and create combined vertex if successful
//
   template <class Particle>
   double  InDetVKalVxInJetTool::mergeAndRefitVertices( std::vector<WrkVrt> *WrkVrtSet, int V1, int V2, WrkVrt & newvrt,
                                                         std::vector<const Particle*> & AllTrackList)
   const
   {
      if(!(*WrkVrtSet).at(V1).Good)return -1.;         //bad vertex
      if(!(*WrkVrtSet).at(V2).Good)return -1.;         //bad vertex
      double RMin=TMath::Min((*WrkVrtSet)[V1].vertex.perp(),(*WrkVrtSet)[V2].vertex.perp());
      double RMax=TMath::Max((*WrkVrtSet)[V1].vertex.perp(),(*WrkVrtSet)[V2].vertex.perp());
      if(RMax-RMin>m_SVResolutionR){
        if(RMin<m_RlayerB && m_RlayerB<RMax) return -1.;
        if(RMin<m_Rlayer1 && m_Rlayer1<RMax) return -1.;
        if(RMin<m_Rlayer2 && m_Rlayer2<RMax) return -1.;
      }
      
      newvrt.Good=true;
      int NTrk_V1=(*WrkVrtSet)[V1].SelTrk.size();
      int NTrk_V2=(*WrkVrtSet)[V2].SelTrk.size();
      newvrt.SelTrk.resize(NTrk_V1+NTrk_V2);
      std::copy((*WrkVrtSet)[V1].SelTrk.begin(),(*WrkVrtSet)[V1].SelTrk.end(), newvrt.SelTrk.begin());
      std::copy((*WrkVrtSet)[V2].SelTrk.begin(),(*WrkVrtSet)[V2].SelTrk.end(), newvrt.SelTrk.begin()+NTrk_V1);

      std::deque<long int>::iterator   TransfEnd ;
      sort( newvrt.SelTrk.begin(), newvrt.SelTrk.end() );
      TransfEnd =  unique( newvrt.SelTrk.begin(), newvrt.SelTrk.end() );
      newvrt.SelTrk.erase( TransfEnd, newvrt.SelTrk.end());
      std::vector<const Particle*>  fitTrackList(0);
      for(int it=0; it<(int)newvrt.SelTrk.size(); it++)fitTrackList.push_back( AllTrackList[newvrt.SelTrk[it]] );
      m_fitSvc->setApproximateVertex( 0.5*((*WrkVrtSet)[V1].vertex[0]+(*WrkVrtSet)[V2].vertex[0]),
                                      0.5*((*WrkVrtSet)[V1].vertex[1]+(*WrkVrtSet)[V2].vertex[1]),
                                      0.5*((*WrkVrtSet)[V1].vertex[2]+(*WrkVrtSet)[V2].vertex[2]));
      
      StatusCode sc=VKalVrtFitBase(fitTrackList,
                                       newvrt.vertex,
                                       newvrt.vertexMom,
			               newvrt.vertexCharge,
		                       newvrt.vertexCov,
		                       newvrt.Chi2PerTrk, 
			               newvrt.TrkAtVrt,
			               newvrt.Chi2);   
      if( sc.isFailure() )             return -1.;  
      if( newvrt.Chi2>500. )           return -1.;  //VK protection
      if( newvrt.Chi2PerTrk.size()==2) newvrt.Chi2PerTrk[0]=newvrt.Chi2PerTrk[1]=newvrt.Chi2/2.;
      return TMath::Prob( newvrt.Chi2, 2*newvrt.SelTrk.size()-3);
   }

//
//  Iterate track removal until vertex get good Chi2
//
   template <class Particle>
   double  InDetVKalVxInJetTool::improveVertexChi2( std::vector<WrkVrt> *WrkVrtSet, int V, std::vector<const Particle*> & AllTrackList)
   const
   {

      int NTrk=(*WrkVrtSet)[V].SelTrk.size();
      if( NTrk<2 )return 0.;
      double Prob=TMath::Prob( (*WrkVrtSet)[V].Chi2, 2*NTrk-3);
      //
      //----Start track removal iterations
      while(Prob<0.01){
        NTrk=(*WrkVrtSet)[V].SelTrk.size();
        if(NTrk==2)return Prob;
        int SelT=-1; double Chi2Max=0.;
        for(int i=0; i<NTrk; i++){ if( (*WrkVrtSet)[V].Chi2PerTrk[i]>Chi2Max) { Chi2Max=(*WrkVrtSet)[V].Chi2PerTrk[i];  SelT=i;}}	    
        (*WrkVrtSet)[V].detachedTrack=(*WrkVrtSet)[V].SelTrk[SelT];
        (*WrkVrtSet)[V].SelTrk.erase( (*WrkVrtSet)[V].SelTrk.begin() + SelT ); //remove track
        StatusCode sc = RefitVertex( WrkVrtSet, V, AllTrackList);
        if(sc.isFailure())return 0.;
        Prob=TMath::Prob( (*WrkVrtSet)[V].Chi2, 2*(NTrk-1)-3);
      }
      return Prob;
   }




 
   template <class Particle>
   StatusCode InDetVKalVxInJetTool::RefitVertex( std::vector<WrkVrt> *WrkVrtSet, int SelectedVertex,
                                            std::vector<const Particle*> & SelectedTracks)
   const
   {
      int i,j;
      int nth = (*WrkVrtSet)[SelectedVertex].SelTrk.size();
 
      if(nth<2) return StatusCode::SUCCESS;
 
      std::vector<const Particle*>  ListTracks(0);
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
      if((*WrkVrtSet)[SelectedVertex].Chi2PerTrk.size()==2) 
         (*WrkVrtSet)[SelectedVertex].Chi2PerTrk[0]=
	 (*WrkVrtSet)[SelectedVertex].Chi2PerTrk[1]=(*WrkVrtSet)[SelectedVertex].Chi2/2.;
      return SC;
   }

   template <class Particle>
   double InDetVKalVxInJetTool::RefitVertex(WrkVrt &Vrt, std::vector<const Particle*> & SelectedTracks)
   const
   {
      int i,j;
      int nth = Vrt.SelTrk.size();
 
      if(nth<2) return -1.;
 
      std::vector<const Particle*>  ListTracks(0);
      for(i=0;i<nth;i++) {
	  j=Vrt.SelTrk[i];                           /*Track number*/
          ListTracks.push_back( SelectedTracks[j] );
      }
      Vrt.Good = false;
      Vrt.Chi2PerTrk.resize(nth);
      for(i=0;i<nth;i++)Vrt.Chi2PerTrk[i]=100000.+i; //VK safety

      m_fitSvc->setApproximateVertex(Vrt.vertex[0],Vrt.vertex[1],Vrt.vertex[2]);
      StatusCode SC=VKalVrtFitBase(ListTracks,Vrt.vertex,Vrt.vertexMom,Vrt.vertexCharge,
		                Vrt.vertexCov,Vrt.Chi2PerTrk, Vrt.TrkAtVrt,Vrt.Chi2); 
      if(SC.isSuccess())Vrt.Good = true;
      else {Vrt.Good = false; return -1.;}
      if(Vrt.Chi2PerTrk.size()==2) Vrt.Chi2PerTrk[0]=Vrt.Chi2PerTrk[1]=Vrt.Chi2/2.;
      return TMath::Prob( Vrt.Chi2, 2*nth-1);
   }

   template <class Particle>
   double InDetVKalVxInJetTool::FitVertexWithPV( std::vector<WrkVrt> *WrkVrtSet, int SelectedVertex, const xAOD::Vertex & PV,
                                                          std::vector<const Particle*> & SelectedTracks)
   const
   {
      int i,j;
      int nth = (*WrkVrtSet)[SelectedVertex].SelTrk.size();  if(nth<2) return 0.;
 
      std::vector<const Particle*>  ListTracks(0);
      for(i=0;i<nth;i++) {
	  j=(*WrkVrtSet)[SelectedVertex].SelTrk[i];                           /*Track number*/
          ListTracks.push_back( SelectedTracks[j] );
      }
      WrkVrt newvrt;
      m_fitSvc->setApproximateVertex((*WrkVrtSet)[SelectedVertex].vertex[0],
	                             (*WrkVrtSet)[SelectedVertex].vertex[1],
	                             (*WrkVrtSet)[SelectedVertex].vertex[2]);
      m_fitSvc->setCnstType(6);
      m_fitSvc->setVertexForConstraint(PV.x(),PV.y(),PV.z());
      m_fitSvc->setCovVrtForConstraint(PV.covariancePosition()(0,0), PV.covariancePosition()(0,1),
                                       PV.covariancePosition()(1,1), PV.covariancePosition()(0,2),
                                       PV.covariancePosition()(1,2), PV.covariancePosition()(2,2)*10000.);
      StatusCode SC=VKalVrtFitBase(ListTracks,
                                newvrt.vertex,
                                newvrt.vertexMom,
				newvrt.vertexCharge,
		                newvrt.vertexCov,
				newvrt.Chi2PerTrk, 
				newvrt.TrkAtVrt,
				newvrt.Chi2); 
      m_fitSvc->setCnstType(0);     //reset constraints
      if(SC.isFailure())return 0.;
      if(newvrt.Chi2PerTrk.size()==2) newvrt.Chi2PerTrk[0]=newvrt.Chi2PerTrk[1]=newvrt.Chi2/2.;
      return TMath::Prob( newvrt.Chi2, 2*nth);
   }

   bool InDetVKalVxInJetTool::isPart( std::deque<long int> test, std::deque<long int> base)
   const
   {
      std::deque<long int>::iterator trk=test.begin();
      for(trk=test.begin(); trk!=test.end(); trk++)
         if(std::find(base.begin(), base.end(), (*trk)) == base.end()) return false;  //element not found => test is not part of base
      return true;
   }

   double InDetVKalVxInJetTool::JetProjDist( Amg::Vector3D & SecVrt,const xAOD::Vertex &PrimVrt,const TLorentzVector &JetDir)
   const
   {
      Amg::Vector3D vv=SecVrt-PrimVrt.position();
      return ( vv.x()*JetDir.X()+vv.y()*JetDir.Y()+vv.z()*JetDir.Z() )/ JetDir.P();
   }

} //end namespace


//For possble future use 
/*    if( m_MultiWithOneTrkVrt && (!m_MultiWithPrimary) && nGoodVertices<5){        // Addition of one-track vertices is allowed
      double addVrtChi2Cut   =5.0;
      double addSig3DCut     =5.0;
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
          int tmpNBLHits=0, tmpL1=0, tmpL2=0, tmpNLays=0;
	  double Signif3DP=0.;
	  if       (RECwork) getPixelLayers( RECwork->listJetTracks[atr], tmpNBLHits, tmpL1, tmpL2, tmpNLays);
	  else if(xAODwrk) getPixelLayers( xAODwrk->listJetTracks[atr], tmpNBLHits, tmpL1, tmpL2, tmpNLays);
          if(tmpNBLHits>0){       // accept only tracks with b-layer hit
            if       (RECwork)Signif3DP = m_fitSvc->VKalGetImpact(RECwork->listJetTracks[atr], PrimVrt.position(), 1, Impact, ImpactError);
            else if(xAODwrk)Signif3DP = m_fitSvc->VKalGetImpact(xAODwrk->listJetTracks[atr], PrimVrt.position(), 1, Impact, ImpactError);
            if( Signif3DP > m_TrkSigCut ) nonusedTracks.push_back(atr);
          }
        }
      }       
      std::vector<WrkVrt> AddedVertices;
      std::vector<WrkVrt> AddedVerticesClean;
      if(nonusedTracks.size()>0 ){
        std::vector<double> VKPerigee,CovPerigee; std::vector<double> ErrorMatrix(21,0.);
        ErrorMatrix[0]=ErrorMatrix[2]=ErrorMatrix[5]=1.e-2;
        for(auto & iv : GoodVertices) {
          if(iv.SelTrk.size()<2) continue; 
          VrtVrtDist(PrimVrt, iv.vertex, iv.vertexCov, Signif3D);
          if(Signif3D<10.) continue;      //------------------Too close to primary vertex. Direction not robust
          Amg::Vector3D tmpVec=iv.vertex-PrimVrt.position(); tmpVec.normalize();
	  ErrorMatrix[ 9]=1.e9*tmpVec.x()*tmpVec.x();
	  ErrorMatrix[14]=1.e9*tmpVec.y()*tmpVec.y();
	  ErrorMatrix[20]=1.e9*tmpVec.z()*tmpVec.z();
	  Amg::setMag(tmpVec,1.e5); TLorentzVector Momentum(tmpVec.x(),tmpVec.y(),tmpVec.z(),m_massPi);
          //std::copy(iv.vertexCov.begin(),iv.vertexCov.begin()+6,ErrorMatrix.begin());
          //sc=m_fitSvc->VKalVrtCvtTool(iv.vertex,Momentum,ErrorMatrix,0,VKPerigee,CovPerigee);
          sc=m_fitSvc->VKalVrtCvtTool(PrimVrt.position(),Momentum,ErrorMatrix,0,VKPerigee,CovPerigee);
          if(sc.isSuccess()) {
            double distRef=(iv.vertex-PrimVrt.position()).perp(); //Distance of already found vertex
            double maxDistRatio=-999.;                            //For selection of the best candidate for given existing vertex
            WrkVrt newvrt; newvrt.Good=true;
            Trk::Track* TT(0);          Rec::TrackParticle* TP(0);     xAOD::TrackParticle * xaodTP(0);
            if( RECwork ) {
              if ( !m_trkPartCreator ){ 
                 if ( m_trkPartCreator.retrieve().isFailure() )
                    {if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< "Failed to retrieve TrackParticleCreator tool" << endmsg;}
                 else {if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Retrieved Trk::TrackParticleCreator tool" << m_trkPartCreator << endmsg;}
              }
	      if( m_trkPartCreator ){
                TT = m_fitSvc->CreateTrkTrack(VKPerigee,CovPerigee); 
                TP=m_trkPartCreator->createParticle(TT);
                RECwork->tmpListTracks.clear(); RECwork->tmpListTracks.push_back(TP);
	      }
            } else if(xAODwrk){
              xaodTP=new xAOD::TrackParticle();   xaodTP->makePrivateStore();
	      xaodTP->setDefiningParameters(VKPerigee[0],VKPerigee[1],VKPerigee[2],VKPerigee[3],VKPerigee[4]);
	      std::vector<float> floatCovPerigee(15,0); for(i=0; i<15; i++)floatCovPerigee[i]=(float)CovPerigee[i];
	      xaodTP->setDefiningParametersCovMatrixVec(floatCovPerigee);
              xaodTP->setParametersOrigin(0.,0.,0.);
              //xaodTP->setRadiusOfFirstHit(25.);
              xAODwrk->tmpListTracks.clear(); xAODwrk->tmpListTracks.push_back(xaodTP);
	    }
            //
	    // - cycle over nonused tracks 
	    for(int add=0; add<(int)nonusedTracks.size(); add++){
	      if(RECwork){
	        RECwork->tmpListTracks.resize(2);    //--VK tmpListTracks[0] always contains the PV-SV track
	        RECwork->tmpListTracks[1]=RECwork->listJetTracks[nonusedTracks[add]];
                sc=VKalVrtFitBase(RECwork->tmpListTracks, newvrt.vertex,    newvrt.vertexMom,  newvrt.vertexCharge,
		                                  newvrt.vertexCov, newvrt.Chi2PerTrk, newvrt.TrkAtVrt, newvrt.Chi2);   
              }else if(xAODwrk){
	        xAODwrk->tmpListTracks.resize(2);    //--VK tmpListTracks[0] always contains the PV-SV track
	        xAODwrk->tmpListTracks[1]=xAODwrk->listJetTracks[nonusedTracks[add]];
                sc=VKalVrtFitBase(xAODwrk->tmpListTracks, newvrt.vertex,    newvrt.vertexMom,  newvrt.vertexCharge,
		                                  newvrt.vertexCov, newvrt.Chi2PerTrk, newvrt.TrkAtVrt, newvrt.Chi2);   
              }	      
              if( sc.isSuccess() ) {
                if(tmpVec.dot(newvrt.vertex-PrimVrt.position())<0.) continue;
                if(newvrt.Chi2>addVrtChi2Cut) continue;
	        double distRatio=(newvrt.vertex-PrimVrt.position()).perp()/distRef;
                if( distRatio < 1./20.) continue;                                            // Too close to primary vertex
                VrtVrtDist(PrimVrt, newvrt.vertex, newvrt.vertexCov, Signif3D);
                if( Signif3D < addSig3DCut) continue;                                 // Too close to primary vertex
                newvrt.Good=true;  newvrt.vertexCharge +=1.;                               //Compensate charge of pseudotrack
                newvrt.SelTrk.clear(); newvrt.SelTrk.push_back(nonusedTracks[add]);        //Add the only nonused track
                newvrt.vertexMom=MomAtVrt(newvrt.TrkAtVrt[1]);                             //Redefine VertexMom using SINGLE REAL track
                if(distRatio > maxDistRatio){ if(maxDistRatio!=-999.)AddedVertices.pop_back(); AddedVertices.push_back(newvrt); maxDistRatio=distRatio;}
	      } 
	    }
            if       (RECwork){ RECwork->tmpListTracks.clear(); delete TT; delete TP; }
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
        if(AddedVertices.size()) for(int iv=0; iv<(int)AddedVerticesClean.size(); iv++) GoodVertices.push_back(AddedVerticesClean[iv]);
      }
    }   //--------------- End of one-track-vertex addition
*/
