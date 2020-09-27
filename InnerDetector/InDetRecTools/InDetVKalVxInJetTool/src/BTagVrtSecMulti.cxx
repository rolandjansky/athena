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
                          NTracks = xAODwrk->listJetTracks.size();
                          MomentumJet = TotalMom(xAODwrk->listJetTracks);}
      else if(RECwork) {  SelGoodTrkParticle( RECwork->InpTrk, PrimVrt, JetDir, RECwork->listJetTracks);
                          NTracks = RECwork->listJetTracks.size();
                          MomentumJet = TotalMom(GetPerigeeVector(RECwork->listJetTracks));}
      if(m_FillHist){m_hb_ntrkjet->Fill( (double) NTracks, m_w_1); }

      if(NTracks>m_TrackInJetNumberLimit){
        if     (xAODwrk ) xAODwrk->listJetTracks.resize(m_TrackInJetNumberLimit);
        else if(RECwork ) RECwork->listJetTracks.resize(m_TrackInJetNumberLimit);
        NTracks=m_TrackInJetNumberLimit;
      }
      if( NTracks < 2 ) { return finalVertices;} // 0,1 selected track => nothing to do!

      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Number of selected tracks inside jet= " <<NTracks << endmsg;
      
      if(m_FillHist){m_hb_jmom->Fill( MomentumJet.Perp(), m_w_1); }

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
            if     (xAODwrk)mergeAndRefitVertices( WrkVrtSet, iv, jv, newvrt, xAODwrk->listJetTracks);
            else if(RECwork)mergeAndRefitVertices( WrkVrtSet, iv, jv, newvrt, RECwork->listJetTracks);
         }
         (*WrkVrtSet).push_back(newvrt);
	 (*WrkVrtSet)[iv].Good=false;      
	 (*WrkVrtSet)[jv].Good=false;      
       }
    }
//-Remove worst track from vertices with very bad Chi2
    NSoluI=(*WrkVrtSet).size();
    for(int iv=0; iv<NSoluI; iv++){
       if(!(*WrkVrtSet)[iv].Good )               continue;
       if( (*WrkVrtSet)[iv].SelTrk.size() == 2 ) continue;
       if( (*WrkVrtSet)[iv].Chi2 > (5.*(*WrkVrtSet)[iv].SelTrk.size()) ){
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
//- Try to merge all vertices which have at least 1 common track
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
    for(int iv=0; iv<(int)(*WrkVrtSet).size(); iv++ ){
       if(!(*WrkVrtSet)[iv].Good )  continue;
       (*WrkVrtSet)[iv].ProjectedVrt=JetProjDist((*WrkVrtSet)[iv].vertex, PrimVrt, JetDir);
       if( (*WrkVrtSet)[iv].SelTrk.size()!=2)continue;
       if( (*WrkVrtSet)[iv].ProjectedVrt <0.) (*WrkVrtSet)[iv].Good =false;
       if( (*WrkVrtSet)[iv].Chi2 > 10.) (*WrkVrtSet)[iv].Good=false;
       if( (*WrkVrtSet)[iv].vertexMom.M()>6000. ) (*WrkVrtSet)[iv].Good=false; //VK B-tagging specific requirement
     }      
//-Remove all bad vertices from the working set    
    int tmpV=0; while( tmpV<(int)(*WrkVrtSet).size() )if( !(*WrkVrtSet)[tmpV].Good ) { (*WrkVrtSet).erase((*WrkVrtSet).begin()+tmpV);} else {tmpV++;}
    //printWrkSet(WrkVrtSet,"Interm");
//----------------------------------------------------------------------------
//   Add primary vertex
//
     if     (xAODwrk)xAODwrk->tmpListTracks.clear(); else if(RECwork)RECwork->tmpListTracks.clear();
     newvrt.SelTrk.clear();
     nth=m_WorkArray->m_Prmtrack.size();
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
	       if(probV>probVrtMergeLimit && newvrt.vertexMom.M()<6000.){        //  Good merged vertex found
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
	if(probV>probVrtMergeLimit && newvrt.vertexMom.M()<6000.){        //  Good merged vertex found
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
       if( (*WrkVrtSet)[iv].SelTrk.size()<2 )      continue;
       double tmpProb=TMath::Prob( (*WrkVrtSet)[iv].Chi2, 2*(*WrkVrtSet)[iv].SelTrk.size()-3 ); //Chi2 of the original vertex
       if(tmpProb<0.001){
         if     (xAODwrk)tmpProb=improveVertexChi2( WrkVrtSet, iv, xAODwrk->listJetTracks);
         else if(RECwork)tmpProb=improveVertexChi2( WrkVrtSet, iv, RECwork->listJetTracks);
         if(tmpProb<0.001)(*WrkVrtSet)[iv].Good=false;
         (*WrkVrtSet)[iv].ProjectedVrt=JetProjDist((*WrkVrtSet)[iv].vertex, PrimVrt, JetDir);
       }
    }
    //printWrkSet(WrkVrtSet,"Joined");
//--------------------------------------------------------------------------------------------------------
// Final vertex selection/cleaning
//

    int nGoodVertices=0;         // Final number of good vertices
    m_fitSvc->setDefault();
    m_fitSvc->setMomCovCalc(1);
    double Signif3D, Signif3Dproj;
    //double Dist3D=0;
    int n2trVrt=0;              // N vertices with 2 and more tracks
    for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
          if(!(*WrkVrtSet)[iv].Good )                 continue;  //don't work on vertex which is already bad
          if( fabs((*WrkVrtSet)[iv].vertex.z())>650. ){(*WrkVrtSet)[iv].Good=false; continue;}  //vertex outside Pixel det.
          nth=(*WrkVrtSet)[iv].SelTrk.size();
          Signif3Dproj=VrtVrtDist(PrimVrt, (*WrkVrtSet)[iv].vertex, (*WrkVrtSet)[iv].vertexCov, JetDir);
          //VrtVrtDist(PrimVrt,(*WrkVrtSet)[iv].vertex, (*WrkVrtSet)[iv].vertexCov, Signif3D); //VK non-projected Signif3D is worse
 	  //std::cout<<" Solution="<<iv<<" Ntrk="<<nth<<'\n';
          if(nth == 1 && m_MultiWithOneTrkVrt && (*WrkVrtSet)[iv].Good){          /* 1track vertices left unassigned from good 2tr vertices */
             int nBLHits=0, tmpL1=0,tmpL2=0, nLays=0;
             double tmpProb=TMath::Prob( (*WrkVrtSet)[iv].Chi2, 1);                 //Chi2 of the original 2tr vertex
             if     (RECwork) getPixelLayers(RECwork->listJetTracks[(*WrkVrtSet)[iv].SelTrk[0]], nBLHits, tmpL1, tmpL2, nLays);
	     else if(xAODwrk) getPixelLayers(xAODwrk->listJetTracks[(*WrkVrtSet)[iv].SelTrk[0]], nBLHits, tmpL1, tmpL2, nLays);
             if(nBLHits>0 && tmpProb>0.01 && (*WrkVrtSet)[iv].vertex.perp()<m_RlayerB){  /* accept only tracks with b-layer hit coming from good 2tr vertex*/
                std::vector<double> Impact,ImpactError;
                double Signif3DP = 0;
		if     (xAODwrk) Signif3DP=m_fitSvc->VKalGetImpact(xAODwrk->listJetTracks[(*WrkVrtSet)[iv].SelTrk[0]],PrimVrt.position(), 1, Impact, ImpactError);
		else if(RECwork) Signif3DP=m_fitSvc->VKalGetImpact(RECwork->listJetTracks[(*WrkVrtSet)[iv].SelTrk[0]],PrimVrt.position(), 1, Impact, ImpactError);
                if(m_FillHist){m_hb_diffPS->Fill( Signif3DP, m_w_1); }
                if( Signif3DP > 2.*m_TrkSigCut && Signif3Dproj>m_Sel2VrtSigCut){  /* accept only tracks which are far from primary vertex */
	           nGoodVertices++;  continue;    // Vertex is accepted as good.
                } 
             }
	  }
          (*WrkVrtSet)[iv].Good = false;                                     /* Make all vertices bad for futher selection */
          if(nth <= 1)                          continue;                    /* Definitely bad vertices */
          if((*WrkVrtSet)[iv].ProjectedVrt<0.)  continue;                    /* Remove vertices behind primary one */ 
          if(xAODwrk)xAODwrk->tmpListTracks.clear(); else if(RECwork)RECwork->tmpListTracks.clear();
          for(i=0;i<nth;i++) {
	    j=(*WrkVrtSet)[iv].SelTrk[i];                           /*Track number*/
            if     (xAODwrk)xAODwrk->tmpListTracks.push_back( xAODwrk->listJetTracks[j] );
	    else if(RECwork)RECwork->tmpListTracks.push_back( RECwork->listJetTracks[j] );
          }
          //VK   Refitting is not needed here - done previously. Option for safety
	  //sc.setCode(0);
          //if     (xAODwrk)sc = RefitVertex( WrkVrtSet, iv, xAODwrk->listJetTracks);
          //else if(RECwork)sc = RefitVertex( WrkVrtSet, iv, RECwork->listJetTracks);
          //if( sc.isFailure() )                                   continue;           /* Bad fit - goto next solution */
          //---------------------------------------------------------------------------         
          if( TMath::Prob( (*WrkVrtSet)[iv].Chi2, 2*nth-3)<0.001) continue;           /* Bad Chi2 of refitted vertex  */
//----
          //Dist3D=((*WrkVrtSet)[iv].vertex-PrimVrt.position()).mag();  /*Not needed currently*/
//----
//        if(PrmVrtAdded && iv==PrmVrtAdded && Dist3D<3.5) continue;  /* Skip added primary vertex */
//----
          bool isolVrt=true; // Check whether this vertex is linked with any 1tr vertex
	  if(nth==2){
             for(int jv=0; jv<(int)WrkVrtSet->size(); jv++)  if( (*WrkVrtSet)[jv].Good  && iv!=jv && (*WrkVrtSet)[jv].SelTrk.size()==1) {
                for(i=0;i<nth;i++) if( (*WrkVrtSet)[iv].SelTrk[i] == (*WrkVrtSet)[jv].detachedTrack )isolVrt=false;
                if(!isolVrt) break;
             }
          }
//----
          if(nth==2 && m_useVertexCleaning){
            if(RECwork){
	       if(!Check2TrVertexInPixel(RECwork->tmpListTracks[0],RECwork->tmpListTracks[1],(*WrkVrtSet)[iv].vertex,Signif3Dproj))continue;
            }else if(xAODwrk){
	       if(!Check2TrVertexInPixel(xAODwrk->tmpListTracks[0],xAODwrk->tmpListTracks[1],(*WrkVrtSet)[iv].vertex,Signif3Dproj))continue;
            }
	  }
//
//---  Check interactions on pixel layers
//
          if(nth==2 && isolVrt){

	    if(m_useMaterialRejection){
	      double xvt=(*WrkVrtSet)[iv].vertex.x();
	      double yvt=(*WrkVrtSet)[iv].vertex.y();
	      float Dist2DBP=std::hypot(xvt-m_Xbeampipe, yvt-m_Ybeampipe);
	      float Dist2DBL=std::hypot(xvt-m_XlayerB, yvt-m_XlayerB);
	      float Dist2DL1=std::hypot(xvt-m_Xlayer1, yvt-m_Xlayer1);
	      float Dist2DL2=std::hypot(xvt-m_Xlayer2, yvt-m_Xlayer2);
	      if(m_existIBL){              // 4-layer pixel detector
		if( std::abs(Dist2DBP-m_Rbeampipe)< 1.0)  continue;           // Beam Pipe removal
		if( std::abs(Dist2DBL-m_RlayerB)  < 2.5)  continue;
		if( std::abs(Dist2DL1-m_Rlayer1)  < 3.0)  continue;
		if( std::abs(Dist2DL2-m_Rlayer2)  < 3.0)  continue;
	      }else{                       // 3-layer pixel detector
		if( std::abs(Dist2DBP-m_Rbeampipe)< 1.5)  continue;           // Beam Pipe removal
		if( std::abs(Dist2DBL-m_RlayerB)  < 3.5)  continue;
		if( std::abs(Dist2DL1-m_Rlayer1)  < 4.0)  continue;
		if( std::abs(Dist2DL2-m_Rlayer2)  < 5.0)  continue;
	      }
	    }

	    if(m_useITkMaterialRejection){
	      double xvt = (*WrkVrtSet)[iv].vertex.x(); double yvt = (*WrkVrtSet)[iv].vertex.y();
	      double zvt = (*WrkVrtSet)[iv].vertex.z(); double Rvt = sqrt(xvt*xvt+yvt*yvt);
	      int bin = m_ITkPixMaterialMap->FindBin(zvt,Rvt);
	      if(m_ITkPixMaterialMap->GetBinContent(bin)>0) continue;
	    }

	  }

//---  Check V0s and conversions
          if(nth==2 && (*WrkVrtSet)[iv].vertexCharge==0 && isolVrt){
             double mass_PiPi =  (*WrkVrtSet)[iv].vertexMom.M();  
             double mass_PPi  =  massV0((*WrkVrtSet)[iv].TrkAtVrt,m_massP,m_massPi);
             double mass_EE   =  massV0((*WrkVrtSet)[iv].TrkAtVrt,m_massE,m_massE);
	     if(m_FillHist)m_hb_massPiPi1->Fill( mass_PiPi , m_w_1);       
             if(m_FillHist)m_hb_totmassEE->Fill( mass_EE, m_w_1);
	     if( fabs(mass_PiPi-m_massK0) < 22.)   continue;
 	     if( fabs(mass_PPi-m_massLam) <  8.)   continue;
             if( mass_EE < 40. && (*WrkVrtSet)[iv].vertex.perp() > 20.) continue;
          }          
//---
          if(m_FillHist){m_hb_sig3DTot->Fill( Signif3Dproj, m_w_1); }
//---
          if(Signif3Dproj<m_Sel2VrtSigCut)continue;      //Main PV-SV distance quality cut 
//-----
//        float Dist2D= (*WrkVrtSet)[iv].vertex.perp();
//	  if(Dist2D<2.){
//            double tmpProb=0.;
//            if     (xAODwrk)tmpProb=FitVertexWithPV( WrkVrtSet, iv, PrimVrt, xAODwrk->listJetTracks);
//            else if(RECwork)tmpProb=FitVertexWithPV( WrkVrtSet, iv, PrimVrt, RECwork->listJetTracks);
//            if(m_FillHist){m_hb_trkPtMax->Fill( tmpProb*1.e5, m_w_1); }
//            if(tmpProb>0.01)continue; // Vertex can be associated with PV
//	  }
//---
          (*WrkVrtSet)[iv].Good = true;  /* Vertex is absolutely good */
          nGoodVertices++;
          if(nth>=2)n2trVrt++;
    }
//
// Cleaning - single vertex close to PV . Doesn't improve anything => commented out
//    if( n2trVrt==1 && nGoodVertices==1 && (!m_MultiWithPrimary) ){
//      for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
//        if( !(*WrkVrtSet)[iv].Good ) continue;
//        if( ((*WrkVrtSet)[iv].vertex-PrimVrt.position()).perp()<3.){
//          double tmpProb=0.;
//          if     (xAODwrk)tmpProb=FitVertexWithPV( WrkVrtSet, iv, PrimVrt, xAODwrk->listJetTracks);
//          else if(RECwork)tmpProb=FitVertexWithPV( WrkVrtSet, iv, PrimVrt, RECwork->listJetTracks);
//          if(m_FillHist){m_hb_trkPtMax->Fill( tmpProb*1.e5, w_1); }
//          if(tmpProb>0.01){  (*WrkVrtSet)[iv].Good =false; continue; }// Vertex can be associated with PV
//    } } }
//
//--Experimental lifetime-based selection
//    for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
//      if(!(*WrkVrtSet)[iv].Good)continue;
//      double lifeTime=(*WrkVrtSet)[iv].ProjectedVrt*(*WrkVrtSet)[iv].vertexMom.M()/(*WrkVrtSet)[iv].vertexMom.E();
//      if(m_FillHist){m_hb_lifetime->Fill( sqrt(lifeTime), m_w_1); }
//      if(lifeTime>2.)(*WrkVrtSet)[iv].Good=false;  
//    }
//
//--Cleaning -  1track vertices set
    Clean1TrVertexSet(WrkVrtSet);
//-------------------------------------------
//checks
    std::vector<WrkVrt> GoodVertices(0);
    nGoodVertices=0;         // Final number of good vertices
    n2trVrt=0;               // N vertices with 2 and more tracks
    for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
       nth=(*WrkVrtSet)[iv].SelTrk.size(); if(nth == 0) continue;   /* Definitely bad vertices */
       Amg::Vector3D tmpVec=(*WrkVrtSet)[iv].vertex-PrimVrt.position();
       TLorentzVector Momentum(tmpVec.x(),tmpVec.y(),tmpVec.z(),m_massPi);
       if(Momentum.DeltaR(JetDir)>m_ConeForTag) (*WrkVrtSet)[iv].Good=false; /* Vertex outside jet cone */
       if( (*WrkVrtSet)[iv].Good) {
	  nGoodVertices++;                                    
          WrkVrt tmpVrt((*WrkVrtSet)[iv]);
	  GoodVertices.push_back(tmpVrt);    /* add it */
	  if(nth>1)n2trVrt++;
       }
    }
    if(nGoodVertices == 0 || n2trVrt==0){
      delete WrkVrtSet;
      delete TrkInVrt; if(weit)delete[] weit; if(Solution)delete[] Solution;
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
    if( m_MultiWithOneTrkVrt && (!m_MultiWithPrimary) && nGoodVertices<4){        // Addition of one-track vertices is allowed
      double addVrtChi2Cut   =3.0;
      double addDistRatioCut =0.25;
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
	  if     (RECwork) getPixelLayers( RECwork->listJetTracks[atr], tmpNBLHits, tmpL1, tmpL2, tmpNLays);
	  else if(xAODwrk) getPixelLayers( xAODwrk->listJetTracks[atr], tmpNBLHits, tmpL1, tmpL2, tmpNLays);
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
        ErrorMatrix[0]=ErrorMatrix[2]=ErrorMatrix[5]=1.e-2;
        for(int iv=0; iv<(int)GoodVertices.size(); iv++) {
          if(GoodVertices[iv].SelTrk.size()<2) continue; 
          VrtVrtDist(PrimVrt, GoodVertices[iv].vertex, newvrt.vertexCov, Signif3D);
          if(Signif3D<10.) continue;   //Too close to primary vertex. Direction not robust
          Amg::Vector3D tmpVec=GoodVertices[iv].vertex-PrimVrt.position(); tmpVec.normalize();
	  ErrorMatrix[ 9]=1.e9*tmpVec.x()*tmpVec.x();
	  ErrorMatrix[14]=1.e9*tmpVec.y()*tmpVec.y();
	  ErrorMatrix[20]=1.e9*tmpVec.z()*tmpVec.z();
	  Amg::setMag(tmpVec,1.e5); TLorentzVector Momentum(tmpVec.x(),tmpVec.y(),tmpVec.z(),m_massPi);
          //std::copy(GoodVertices[iv].vertexCov.begin(),GoodVertices[iv].vertexCov.begin()+6,ErrorMatrix.begin());
          //sc=m_fitSvc->VKalVrtCvtTool(GoodVertices[iv].vertex,Momentum,ErrorMatrix,0,VKPerigee,CovPerigee);
          sc=m_fitSvc->VKalVrtCvtTool(PrimVrt.position(),Momentum,ErrorMatrix,0,VKPerigee,CovPerigee);
          if(sc.isSuccess()) {
            double distRef=(GoodVertices[iv].vertex-PrimVrt.position()).perp(); //Distance of already found vertex
            double maxDistRatio=-999.;                            //For selection of the best candidate for given existing vertex
            WrkVrt newvrt; newvrt.Good=true;
            Trk::Track* TT(0); 
            Rec::TrackParticle* TP(0);
            xAOD::TrackParticle * xaodTP(0);
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
              xaodTP=new xAOD::TrackParticle();
              xaodTP->makePrivateStore();
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
                if(tmpVec.dot(newvrt.vertex-PrimVrt.position())<0.) continue;
	        double distRatio=(newvrt.vertex-PrimVrt.position()).perp()/distRef;
                if( distRatio < 0.05) continue;                                              // Too close to primary vertex (Preliminary!!!) 
                if(m_FillHist){ m_hb_tr2SelVar->Fill( newvrt.Chi2 , 1.); }
                if(newvrt.Chi2>addVrtChi2Cut) continue;
                if(m_FillHist)m_hb_addRatioMV->Fill( distRatio,1.);
                if( distRatio < addDistRatioCut) continue;                                 // Too close to primary vertex
                newvrt.Good=true;  newvrt.vertexCharge +=1.;                               //Compensate charge of pseudotrack
                newvrt.SelTrk.clear(); newvrt.SelTrk.push_back(nonusedTracks[add]);        //Add the only nonused track
                newvrt.vertexMom=MomAtVrt(newvrt.TrkAtVrt[1]);                             //Redefine VertexMom using SINGLE REAL track
                if(distRatio > maxDistRatio){ if(maxDistRatio!=-999.)AddedVertices.pop_back(); AddedVertices.push_back(newvrt); maxDistRatio=distRatio;}
	      } 
	      if(RECwork)RECwork->tmpListTracks.pop_back(); else if(xAODwrk)xAODwrk->tmpListTracks.pop_back();
	    }
            if(maxDistRatio>0. && m_FillHist) m_hb_addChi2MV->Fill(AddedVertices[AddedVertices.size()-1].Chi2,1.);
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
//
//    printWrkSet(&GoodVertices,"FinalVrtSet");
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
            m_hb_r2dc->Fill( GoodVertices[iv].vertex.perp(), m_w_1);
            Signif3D=VrtVrtDist(PrimVrt, GoodVertices[iv].vertex, GoodVertices[iv].vertexCov, JetDir);
            if( nth==2 ){
	      if(GoodVertices[iv].vertexCharge==0){m_hb_totmass2T1->Fill( GoodVertices[iv].vertexMom.M(), m_w_1);}
	      else                                {m_hb_totmass2T2->Fill( GoodVertices[iv].vertexMom.M(), m_w_1);}
	      m_hb_sig3D2tr->Fill( Signif3D , m_w_1);
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
    if(m_FillHist){m_hb_goodvrtN->Fill( (double)nGoodVertices, m_w_1); }

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
           if( (*WrkVrtSet)[NVrtCur].Chi2<newvrt.Chi2 ) continue;  // previous vertex was better
           WrkVrtSet->erase(WrkVrtSet->begin()+NVrtCur);
           WrkVrtSet->push_back(newvrt);
      }
      (*WrkVrtSet)[iv].SelTrk.erase( (*WrkVrtSet)[iv].SelTrk.begin() + SelT ); //remove track
      sc = RefitVertex( WrkVrtSet, iv, AllTracks);
      if( sc.isFailure() )  std::cout<<" Wrong vertex"<<'\n';
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
         if( std::find((*WrkVrtSet)[mtv].SelTrk.begin(),(*WrkVrtSet)[mtv].SelTrk.end(), Trk1) != (*WrkVrtSet)[mtv].SelTrk.end()){//Linked vertex found
           double m2Vrt=((*WrkVrtSet)[mtv].vertexMom+(*WrkVrtSet)[i1tv].vertexMom).M();
           if(m2Vrt>6000.){ (*WrkVrtSet)[i1tv].Good=false;  break; } //1Tr + manyTr system is too heavy
	   foundInGoodVrt++; countVT[mtv]++; linkedVrt[i1tv]=mtv;
           break;  // Linked vertex is already found, so stop sycling
         }
       }
       if(!foundInGoodVrt)(*WrkVrtSet)[i1tv].Good=false;
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
        if( (*WrkVrtSet)[i1tv].SelTrk.size()!=1 ) continue;
	if( (*WrkVrtSet)[i1tv].Chi2>10.){
	  (*WrkVrtSet)[i1tv].Good=false;
        }
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
	    for( itmp=0; itmp<NTrkInVrt; itmp++){
	       if( (*WrkVrtSet)[VertexNumber].SelTrk[itmp] == it ) {         /* Track found*/        
                //Chi2Red=(*WrkVrtSet)[VertexNumber].Chi2PerTrk.at(itmp)/m_chiScale[(NTrkInVrt<10?NTrkInVrt:10)]; //   Reduced Chi2
	        //if(NTrkInVrt==2){ Chi2Red += 30./((*WrkVrtSet)[VertexNumber].vertex.perp()+5.);}       //VK Reduce vrt multiplicity. May decrease fake rate
	        //Chi2Red=(*WrkVrtSet)[VertexNumber].Chi2PerTrk.at(itmp);            //   Normal Chi2 seems the best
	          Chi2Red=(*WrkVrtSet)[VertexNumber].Chi2PerTrk.at(itmp)+(*WrkVrtSet)[VertexNumber].Chi2/NTrkInVrt;//Improved normal
                  if(NTrkInVrt==2){
		  //Chi2Red=(*WrkVrtSet)[VertexNumber].Chi2/2.;                     //VK 2track vertices with Normal Chi2Red
	            if((*WrkVrtSet)[VertexNumber].vertexMom.M()>6000.)Chi2Red=100.; //VK break immediately very heavy 2tr vertices
                  }
                  if( MaxOf < Chi2Red ){
		      MaxOf = Chi2Red;
		      SelectedTrack=it;
		      SelectedVertex=VertexNumber;
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
              double vr1=(*WrkVrtSet)[v1].vertex.perp(); double vr2=(*WrkVrtSet)[v2].vertex.perp();
              if     (SelectedVertex==v1 && vr2<vr1/2.) SelectedVertex=v2;  // Swap to remove the closest vertex
              else if(SelectedVertex==v2 && vr1<vr2/2.) SelectedVertex=v1;  // Swap to remove the closest vertex
            }
	  }
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
      if(!(*WrkVrtSet).at(V1).Good)return 0.;         //bad vertex
      if(!(*WrkVrtSet).at(V2).Good)return 0.;         //bad vertex

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
      if( sc.isFailure() )             return 0.;  
      if( newvrt.Chi2>500. )           return 0.;  //VK protection
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
