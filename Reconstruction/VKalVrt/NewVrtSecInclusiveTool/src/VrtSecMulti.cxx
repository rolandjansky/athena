/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
///
///    @author Vadim Kostyukhin <vadim.kostyukhin@cern.ch>
///
//
// Header include
#include "NewVrtSecInclusiveTool/NewVrtSecInclusiveTool.h"
//-------------------------------------------------
// Other stuff
#include  "AnalysisUtils/AnalysisMisc.h"
#include  "GeoPrimitives/GeoPrimitivesHelpers.h"
#include  "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include  "MVAUtils/BDT.h" 

#include  "boost/graph/bron_kerbosch_all_cliques.hpp"
#include  "TMath.h"
#include  "TH1.h"

#include  <algorithm>
//


namespace Rec{


   std::vector<xAOD::Vertex*> NewVrtSecInclusiveTool::getVrtSecMulti(  workVectorArrxAOD * xAODwrk,
                                                                      const xAOD::Vertex & PrimVrt )
   const
   {

      const double probVrtMergeLimit=0.01;

      int inpNPart=0; 
      int i,j;
      inpNPart=xAODwrk->inpTrk.size();
      std::vector<const xAOD::NeutralParticle*> neutralPartDummy(0);
      ATH_MSG_DEBUG( "getVrtSecMulti() called with NPart=" << inpNPart);
   
      std::vector<xAOD::Vertex*>  finalVertices(0); 

      if( inpNPart < 2 ) { return finalVertices;}   // 0,1 track => nothing to do!
   
   
      long int NTracks = 0;
      selGoodTrkParticle( xAODwrk, PrimVrt);
      NTracks = xAODwrk->listSelTracks.size();

      if( NTracks < 2 ) { return finalVertices;} // 0,1 selected track => nothing to do!

      ATH_MSG_DEBUG( "Number of selected tracks = " <<NTracks);
      
      if(m_fillHist){ m_hb_ntrksel->Fill( (double) NTracks, m_w_1); }

//
//  inpTrk[]           - input track list
//  listSelTracks[]    - list of good tracks in jet for vertex search
//------------------------------------------------------------	 
//                     Initial track list ready
//                     Find 2track vertices
//

      select2TrVrt(xAODwrk->listSelTracks, PrimVrt);

//---
      ATH_MSG_DEBUG(" Defined edges in the graph="<< num_edges(*m_compatibilityGraph));

//
//  m_Incomp[]           -  main vector of pointers for multivertex search
//-----------------------------------------------------------------------------------------------------
//            Secondary track list is ready
//            Creation of initial vertex set
//


      std::vector<WrkVrt> *WrkVrtSet= new std::vector<WrkVrt>;
      WrkVrt newvrt; newvrt.Good=true;
      std::unique_ptr<Trk::IVKalState> state = m_fitSvc->makeState();
      StatusCode sc; sc.setChecked(); 
      long int NPTR=0, nth=2; // VK nth=2 to speed up PGRAPH when it's used


      std::vector< std::vector<int> > allCliques;
      bron_kerbosch_all_cliques(*m_compatibilityGraph, clique_visitor(allCliques));
      for(int cq=0; cq<(int)allCliques.size();cq++){
          newvrt.selTrk.clear();
          NPTR=allCliques[cq].size();
          for(i=0;i<NPTR;i++){ newvrt.selTrk.push_back(allCliques[cq][i]);}
//==================================================
          xAODwrk->tmpListTracks.clear();
	  TLorentzVector vpsum;
          for(i=0;i<NPTR;i++) {
             xAODwrk->tmpListTracks.push_back( xAODwrk->listSelTracks.at(newvrt.selTrk[i]) );
	     vpsum += xAODwrk->listSelTracks.at(newvrt.selTrk[i])->p4();
          }
          sc = m_fitSvc->VKalVrtFitFast(xAODwrk->tmpListTracks,newvrt.vertex,*state);            /* Fast crude estimation*/

          if( sc.isFailure()  ) {   /* No initial estimation */ 
              m_fitSvc->setApproximateVertex(PrimVrt.x(), PrimVrt.y(), PrimVrt.z(),*state); /*Use as starting point*/
 	  } else {
	      Amg::Vector3D vDist=newvrt.vertex-PrimVrt.position();
              double MomVrtDir = vpsum.Px()*vDist.x() + vpsum.Py()*vDist.y() + vpsum.Pz()*vDist.z();
              if( MomVrtDir>0. ) {                           /* Good initial estimation */ 
                  m_fitSvc->setApproximateVertex(newvrt.vertex.x(),newvrt.vertex.y(),newvrt.vertex.z(),*state); /*Use as starting point*/
	      }else{
                  m_fitSvc->setApproximateVertex(PrimVrt.x(), PrimVrt.y(), PrimVrt.z(),*state); 
              }
          }
	  sc = StatusCode::FAILURE;
          sc=m_fitSvc->VKalVrtFit(xAODwrk->tmpListTracks, neutralPartDummy,
	                                     newvrt.vertex,     newvrt.vertexMom, newvrt.vertexCharge, newvrt.vertexCov,
                                             newvrt.chi2PerTrk, newvrt.trkAtVrt,  newvrt.chi2,
                                             *state, false);

          ATH_MSG_VERBOSE("Found IniVertex="<<newvrt.vertex[0]<<", "<<newvrt.vertex[1]<<", "<<newvrt.vertex[2]);
          ATH_MSG_VERBOSE("with Chi2="<<newvrt.chi2<<" Ntrk="<<NPTR<<" trk1,2="<<newvrt.selTrk[0]<<", "<<newvrt.selTrk[1]); 
          if( sc.isFailure() )           continue;   /* Bad fit - goto next solution */
          if(NPTR==2 && newvrt.chi2>10.) continue;   /* Bad 2track vertex */
          if(newvrt.chi2PerTrk.size()==2) newvrt.chi2PerTrk[0]=newvrt.chi2PerTrk[1]=newvrt.chi2/2.;
          newvrt.Good         = true;
          newvrt.nCloseVrt    = 0;
          newvrt.dCloseVrt    = 1000000.;
          newvrt.projectedVrt=MomProjDist(newvrt.vertex, PrimVrt, newvrt.vertexMom); //3D SV-PV distance
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
//========= Initial cleaning of solutions
//-Remove worst track from vertices with very bad Chi2
    bool disassembled=false;
    int NSoluI=0;
    do{ 
      disassembled=false;
      NSoluI=(*WrkVrtSet).size();
      for(int iv=0; iv<NSoluI; iv++){
        if(!(*WrkVrtSet)[iv].Good || (*WrkVrtSet)[iv].selTrk.size() == 2 ) continue;
        if( TMath::Prob( (*WrkVrtSet)[iv].chi2, 2*(*WrkVrtSet)[iv].selTrk.size()-3) <1.e-3){
          //printWrkSet(WrkVrtSet,"BigChi2Vertex present");
          DisassembleVertex(WrkVrtSet,iv,xAODwrk->listSelTracks,*state);
          disassembled=true;
      } }
    }while(disassembled);
//-Modify too heavy vertices 
    for(auto & iv : (*WrkVrtSet)){
      if( iv.vertexMom.M()>m_VrtMassLimit ) {
        int it_bad=mostHeavyTrk(iv,xAODwrk->listSelTracks);
        if(it_bad>-1){
	  iv.selTrk.erase( iv.selTrk.begin() + it_bad );
	  refitVertex(iv, xAODwrk->listSelTracks, *state, false);
     } } }
//-Remove vertices fully contained in other vertices 
    while( (*WrkVrtSet).size()>1 ){
      int tmpN=(*WrkVrtSet).size();  int iv=0;
      for(; iv<tmpN-1; iv++){        int jv=iv+1;
        if(!(*WrkVrtSet)[iv].Good )               continue;
        for(; jv<tmpN; jv++){
          if(!(*WrkVrtSet)[jv].Good )             continue;
          int nTCom=nTrkCommon( WrkVrtSet, iv, jv);
          if(      nTCom==(int)(*WrkVrtSet)[iv].selTrk.size()){  (*WrkVrtSet).erase((*WrkVrtSet).begin()+iv); break; }
          else if( nTCom==(int)(*WrkVrtSet)[jv].selTrk.size()){  (*WrkVrtSet).erase((*WrkVrtSet).begin()+jv); break; }
        }   if(jv!=tmpN)   break;  // One vertex is erased. Restart check
      }     if(iv==tmpN-1) break;  // No vertex deleted
    }
//
//- Try to merge all vertices with common tracks
    std::multimap<int,std::pair<int,int>> vrtWithCommonTrk;
    std::multimap<int,std::pair<int,int>>::reverse_iterator icvrt;
    do{
      NSoluI=(*WrkVrtSet).size();
      vrtWithCommonTrk.clear();
      for(int iv=0; iv<NSoluI-1; iv++ ){  for(int jv=iv+1; jv<NSoluI; jv++){
          if(!(*WrkVrtSet)[iv].Good || !(*WrkVrtSet)[jv].Good)    continue;
          int nTCom=nTrkCommon( WrkVrtSet, iv, jv);     if(!nTCom)continue;
          vrtWithCommonTrk.emplace(nTCom,std::make_pair(iv,jv));
      } }
      //============================== DEBUG output
      //for(auto ku : vrtWithCommonTrk)std::cout<<" nCom="<<ku.first<<" v1="<<ku.second.first<<" v2="<<ku.second.second<<'\n';
      if(msgLvl(MSG::DEBUG))printWrkSet(WrkVrtSet,"Initial Vertices");
      //===========================================
      for(icvrt=vrtWithCommonTrk.rbegin(); icvrt!=vrtWithCommonTrk.rend(); icvrt++){ 
          int nTCom=(*icvrt).first;
          int iv=(*icvrt).second.first;
          int jv=(*icvrt).second.second;
          int nTrkI=(*WrkVrtSet)[iv].selTrk.size();
          int nTrkJ=(*WrkVrtSet)[jv].selTrk.size();
	  double probV=-1.;
          probV=mergeAndRefitVertices( WrkVrtSet, iv, jv, newvrt, xAODwrk->listSelTracks, *state);
          ////std::cout<<" ntcommon="<<(*icvrt).first<<" prb="<<probV<<" itrk="<<nTrkI<<" jtrk="<<nTrkJ<<'\n';
          if(newvrt.vertexMom.M()>m_VrtMassLimit) continue; //Do not merge vertices if summary one is too heavy
	  if(probV<probVrtMergeLimit){
            if(nTrkI==2 || nTrkJ==2 || nTCom<2) continue;
            if((nTCom>nTrkI-nTCom || nTCom>nTrkJ-nTCom)){  //2 and more common tracks for NTr>=3 vertices. Merge anyway.
              mergeAndRefitOverlapVertices( WrkVrtSet, iv, jv, xAODwrk->listSelTracks, *state);
              break; //Vertex list is changed. Restart merging from scratch.
            }
            continue;  //Continue merging loop 
          }
          newvrt.Good = true;
          (*WrkVrtSet).push_back(newvrt);
	  (*WrkVrtSet)[iv].Good=false;      
	  (*WrkVrtSet)[jv].Good=false;
          break;  //Merging successful. Break merging loop and remake list of connected vertices
       }
    } while( icvrt != vrtWithCommonTrk.rend() );
    if(m_fillHist){ int cvgood=0; for(int iv=0; iv<(int)(*WrkVrtSet).size(); iv++) if((*WrkVrtSet)[iv].Good)cvgood++;
                    m_hb_rawVrtN->Fill( (float)cvgood, m_w_1); }
//
//-Remove all bad vertices from the working set    
    int tmpV=0; while( tmpV<(int)(*WrkVrtSet).size() )if( !(*WrkVrtSet)[tmpV].Good ) { (*WrkVrtSet).erase((*WrkVrtSet).begin()+tmpV);} else {tmpV++;}
    if((*WrkVrtSet).size()==0){             // No vertices at all
      delete WrkVrtSet;
      return finalVertices;
    }
    //------
    if(msgLvl(MSG::DEBUG))printWrkSet(WrkVrtSet,"Intermediate Vertices");
     //------
    for( auto &tmpV : (*WrkVrtSet) ) tmpV.projectedVrt=MomProjDist(tmpV.vertex, PrimVrt, tmpV.vertexMom );  //Setup ProjectedVrt
//----------------------------------------------------------------------------
//             Here we have the overlapping solutions.
//             Vertices may have only 1 common track. 
//              Now solution cleaning

    int nGoodVertices=0;         // Final number of good vertices
    int n2trVrt=0;               // N of vertices with 2  tracks
    int nNtrVrt=0;               // N vertices with 3 and more tracks
    std::vector< std::deque<long int> > *TrkInVrt  =new std::vector< std::deque<long int> >(NTracks);  
    double foundMaxT; long int SelectedTrack, SelectedVertex;
    int foundV1, foundV2;
    
    trackClassification( WrkVrtSet, TrkInVrt);

    state = m_fitSvc->makeState();
    while((foundMaxT=maxOfShared( WrkVrtSet, TrkInVrt, SelectedTrack, SelectedVertex))>0) {
         if(msgLvl(MSG::VERBOSE)) { 
           printWrkSet(WrkVrtSet,"Iteration");
           ATH_MSG_VERBOSE("MAX="<<foundMaxT<<", "<<SelectedTrack<<", "<<SelectedVertex<<
                           "VRT="<<minVrtVrtDist( WrkVrtSet, foundV1, foundV2)<<", "<<foundV1<<", "<<foundV2);
         }

         double foundMinVrtDst = 1000000.;
         if(foundMaxT<m_trackDetachCut) foundMinVrtDst = minVrtVrtDist( WrkVrtSet, foundV1, foundV2);

//Choice of action
          if( foundMaxT<m_trackDetachCut && foundMinVrtDst<m_vertexMergeCut && nTrkCommon( WrkVrtSet, foundV1, foundV2)){
          //if( foundMaxT<m_trackDetachCut && foundMinVrtDst<m_vertexMergeCut){
             bool vrtMerged=false;   //to check whether something is really merged
             while(foundMinVrtDst<m_vertexMergeCut){
               if(foundV1<foundV2) { int tmp=foundV1; foundV1=foundV2; foundV2=tmp;} /*Always drop vertex with smallest number*/
	       double probV=0.;
               probV=mergeAndRefitVertices( WrkVrtSet, foundV1, foundV2, newvrt, xAODwrk->listSelTracks, *state);
	       if(probV>probVrtMergeLimit && newvrt.vertexMom.M()<m_VrtMassLimit){        //  Good merged vertex found
                 double tstDst=MomProjDist(newvrt.vertex, PrimVrt, newvrt.vertexMom);
	         if(tstDst>0.){                               // only positive vertex directions are accepted as merging result
                   std::swap((*WrkVrtSet)[foundV1],newvrt);
                   (*WrkVrtSet)[foundV1].projectedVrt=tstDst;
	           (*WrkVrtSet)[foundV2].Good=false;         //Drop vertex
                   (*WrkVrtSet)[foundV2].selTrk.clear();     //Clean dropped vertex
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
               trackClassification( WrkVrtSet, TrkInVrt);
	       continue;  // Something was merged => goto next cycle. Otherwise break the found track-vertex link
	     }
	  }
          removeTrackFromVertex(WrkVrtSet, TrkInVrt, SelectedTrack, SelectedVertex);
          sc = refitVertex( WrkVrtSet, SelectedVertex, xAODwrk->listSelTracks, *state, false);
          if( sc.isFailure() ) { (*WrkVrtSet)[SelectedVertex].Good=false;  continue; } //bad vertex
          (*WrkVrtSet)[SelectedVertex].projectedVrt=MomProjDist((*WrkVrtSet)[SelectedVertex].vertex, PrimVrt, (*WrkVrtSet)[SelectedVertex].vertexMom);
          if( (*WrkVrtSet)[SelectedVertex].projectedVrt<0. && (*WrkVrtSet)[SelectedVertex].selTrk.size()==2 )
	      (*WrkVrtSet)[SelectedVertex].Good=false;  // 2track vertex migrates behind PV - drop it.
          //std::cout<<"Dropped v="<<SelectedVertex<<" dst="<<(*WrkVrtSet)[SelectedVertex].projectedVrt<<'\n';
    }
//
// Final check/merge for close vertices
//
    double minDistVV=minVrtVrtDist( WrkVrtSet, foundV1, foundV2); //recalculate VV distances
    while ( minDistVV < m_vertexMergeCut) {
        if(foundV1<foundV2) { int tmp=foundV1; foundV1=foundV2; foundV2=tmp;}
	double probV=0.;
        probV=mergeAndRefitVertices( WrkVrtSet, foundV1, foundV2, newvrt, xAODwrk->listSelTracks, *state);
	if(probV>probVrtMergeLimit && newvrt.vertexMom.M()<m_VrtMassLimit){        //  Good merged vertex found
           double tstDst=MomProjDist(newvrt.vertex, PrimVrt, newvrt.vertexMom);
	   if(tstDst>0.){                               // only positive vertex directions are accepted as merging result
              std::swap((*WrkVrtSet)[foundV1],newvrt);
              (*WrkVrtSet)[foundV1].projectedVrt=tstDst;
	      (*WrkVrtSet)[foundV2].Good=false;         //Drop vertex
              (*WrkVrtSet)[foundV2].selTrk.clear();     //Clean dropped vertex
           }
        }
        (*WrkVrtSet)[foundV1].nCloseVrt=-1; (*WrkVrtSet)[foundV1].dCloseVrt=1000000.; //For minVrtVrtDistNext optimisation(!)
        (*WrkVrtSet)[foundV2].nCloseVrt=-1; (*WrkVrtSet)[foundV2].dCloseVrt=1000000.; //Exclude given pair
        minDistVV=minVrtVrtDistNext( WrkVrtSet, foundV1, foundV2);
    }
//
// Try to improve vertices with big Chi2 if something went wrong. Just precaution.
    for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
       if(!(*WrkVrtSet)[iv].Good )                 continue;  //don't work on vertex which is already bad
       if( (*WrkVrtSet)[iv].selTrk.size()<3 )      continue;
       double tmpProb=TMath::Prob( (*WrkVrtSet)[iv].chi2, 2*(*WrkVrtSet)[iv].selTrk.size()-3 ); //Chi2 of the original vertex
       if(tmpProb<m_globVrtProbCut){
         tmpProb=improveVertexChi2( WrkVrtSet, iv, xAODwrk->listSelTracks, *state, false);
         if(tmpProb<m_globVrtProbCut)(*WrkVrtSet)[iv].Good=false;
         (*WrkVrtSet)[iv].projectedVrt=MomProjDist((*WrkVrtSet)[iv].vertex, PrimVrt, (*WrkVrtSet)[iv].vertexMom);
       }
    }
    //
    //printWrkSet(WrkVrtSet,"Joined");
//--------------------------------------------------------------------------------------------------------
// Final vertex selection/cleaning
//
    state = m_fitSvc->makeState();
    double signif3D=0., signif2D=0., vProb=0.;
    //double Dist3D=0;

//--------- Start with 1-track vertices
//=First check if the track was detached from a multitrack vertex. If so - reattach. 
    for(auto &ntrVrt : (*WrkVrtSet)){ if(!ntrVrt.Good || ntrVrt.selTrk.size()<=1)      continue;
      for(auto &onetVrt : (*WrkVrtSet)){ if(!onetVrt.Good || onetVrt.selTrk.size()!=1) continue;
        if(ntrVrt.detachedTrack==onetVrt.selTrk[0]){   
	  WrkVrt newV(ntrVrt); newV.selTrk.push_back(ntrVrt.detachedTrack);
          vProb = refitVertex( newV, xAODwrk->listSelTracks, *state, true);
	  if( vProb>probVrtMergeLimit && newV.vertexMom.M()<m_VrtMassLimit ){ onetVrt.Good=false; ntrVrt=newV;  ntrVrt.detachedTrack=-1;}
	  break;
    } } }
//=Recheck if the track was detached from a 2track vertex. If so - reattach. 
    for(auto &iVrt : (*WrkVrtSet)){   if(!iVrt.Good || iVrt.selTrk.size()!=1) continue;
      for(auto &jVrt : (*WrkVrtSet)){ if(!jVrt.Good || jVrt.selTrk.size()!=1) continue;
        if(iVrt.detachedTrack==jVrt.selTrk[0]){   
	  WrkVrt newV(iVrt); newV.selTrk.push_back(iVrt.detachedTrack);
          vProb = refitVertex( newV, xAODwrk->listSelTracks, *state, true);
	  if( vProb>probVrtMergeLimit && newV.vertexMom.M()<m_VrtMassLimit ){ jVrt.Good=false; iVrt=newV;  iVrt.detachedTrack=-1;}
	  break;
    } } }
    for(auto &ntrVrt : (*WrkVrtSet)){ if(!ntrVrt.Good  || ntrVrt.selTrk.size()<=1)  continue;
      for(auto tr : ntrVrt.selTrk){ 
        for(auto &onetVrt  : (*WrkVrtSet)){ if(!onetVrt.Good || onetVrt.selTrk.size()!=1) continue;
	  if(onetVrt.detachedTrack==tr){
	    WrkVrt newV(ntrVrt); newV.selTrk.push_back(onetVrt.selTrk[0]);
            vProb = refitVertex( newV, xAODwrk->listSelTracks, *state, true);
	    if( vProb>probVrtMergeLimit && newV.vertexMom.M()<m_VrtMassLimit ){ onetVrt.Good=false; ntrVrt=newV;}
    } } } }
    for(auto & curVrt : (*WrkVrtSet) ) {
       if(!curVrt.Good )                 continue;  //don't work on vertex which is already bad
       if(curVrt.selTrk.size() != 1)     continue;
       curVrt.Good=false;       // Make them bad by default
       if(m_multiWithOneTrkVrt){          /* 1track vertices left unassigned from good 2tr vertices */
          VrtVrtDist(PrimVrt,curVrt.vertex, curVrt.vertexCov, signif3D); //VK non-projected signif3D is worse
          double tmpProb=TMath::Prob( curVrt.chi2, 1);                 //Chi2 of the original 2tr vertex
          if(tmpProb>0.01){  /* accept only good tracks coming from good 2tr vertex*/
             std::vector<double> impact,impactError;   double signif3DP = 0;
             signif3DP=m_fitSvc->VKalGetImpact(xAODwrk->listSelTracks[curVrt.selTrk[0]],PrimVrt.position(), 1, impact, impactError, *state);
             if(m_fillHist){
               m_hb_diffPS->Fill( signif3DP, m_w_1); 
               m_hb_sig3D1tr->Fill( signif3D, m_w_1);
             }
             if( signif3DP>2.*m_trkSigCut && signif3D>m_selVrtSigCut) curVrt.Good=true; // accept only tracks which are far from PV
          }
       }
    }
//-----  Vertices with >1 tracks
    for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
          WrkVrt & curVrt=(*WrkVrtSet)[iv];
          if(!curVrt.Good )                      continue;  //don't work on vertex which is already bad
          nth=(*WrkVrtSet)[iv].selTrk.size(); if(nth == 1) continue;  // 1track vertices are treated already
          VrtVrtDist(PrimVrt,curVrt.vertex, curVrt.vertexCov, signif3D); 
          xAODwrk->tmpListTracks.resize(nth);
          for(i=0;i<nth;i++) {
            xAODwrk->tmpListTracks[i]=xAODwrk->listSelTracks[curVrt.selTrk[i]];
          }
          (*WrkVrtSet)[iv].Good = false;                                     /* Make all vertices bad for futher selection */
          if(nth <= 1)                          continue;                    /* Definitely bad vertices */
          if((*WrkVrtSet)[iv].projectedVrt<0.)  continue;                    /* Remove vertices behind primary one */ 
          if( TMath::Prob( curVrt.chi2, 2*nth-3)<m_globVrtProbCut) continue;           /* Bad Chi2 of refitted vertex  */
//-----------------------------------------------------------------------------------------
          if(m_fillHist){ 
             if(nth==2 && curVrt.vertexCharge==0) m_hb_massPiPi1->Fill(curVrt.vertexMom.M(), m_w_1);
 	     m_hb_sig3DTot->Fill( signif3D, m_w_1);
             if(nth==2)m_hb_sig3D2tr->Fill( signif3D, m_w_1);
             if(nth >2)m_hb_sig3DNtr->Fill( signif3D, m_w_1);
          }
//
//---  Check V0s and conversions. Necessity must be checked in physics applications
#if 0
          if(nth==2 && curVrt.vertexCharge==0 && curVrt.detachedTrack<0){
             double mass_PiPi =  curVrt.vertexMom.M();  
             double mass_PPi  =  massV0(curVrt.trkAtVrt,m_massP,m_massPi);
             double mass_EE   =  massV0(curVrt.trkAtVrt,m_massE,m_massE);
             if(m_fillHist){ m_hb_massPiPi->Fill( mass_PiPi, m_w_1);
                             m_hb_massPPi ->Fill( mass_PPi,  m_w_1); 
                             if( curVrt.vertex.perp()>20.)m_hb_massEE  ->Fill( mass_EE,   m_w_1);  } 
 	     if( fabs(mass_PiPi-m_massK0) < 22.)     continue;
 	     if( fabs(mass_PPi-m_massLam) <  8.)     continue;
             if( mass_EE < 60. && curVrt.vertex.perp() > 20.) continue;
          }          
#endif
//---
          if(signif3D<m_selVrtSigCut+1.)              continue;      //Main PV-SV distance quality cut 
          if(curVrt.vertex.perp() > m_maxSVRadiusCut) continue;      // Too far from interaction point
//---
          curVrt.Good = true;  /* Vertex is absolutely good */
    }
//-------------------------------------------
// Debug ntuple filling and BDT application
//
    std::vector<double> impact,impactError;
    for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
          WrkVrt & curVrt=(*WrkVrtSet)[iv];
          nth=curVrt.selTrk.size();
          if(!curVrt.Good )      continue;  //don't work on vertex which is already bad
          double minPtT=1.e6, minSig3DT=1.e6, maxSig3DT=0.;
	  int ntrkBC=0,ntrkI=0,sumIBLHits=0,sumBLHits=0;
          for(i=0;i<nth;i++) {
             j=curVrt.selTrk[i];                           /*Track number*/
             minPtT=std::min( minPtT, xAODwrk->listSelTracks[j]->pt());
             m_fitSvc->VKalGetImpact(xAODwrk->listSelTracks[j], PrimVrt.position(), 1, impact, impactError);
	     double SigR2 = impact[0]*impact[0]/impactError[0];
	     double SigZ2 = impact[1]*impact[1]/impactError[2];
             minSig3DT=std::min( minSig3DT, sqrt( SigR2 + SigZ2) );
             maxSig3DT=std::max( maxSig3DT, sqrt( SigR2 + SigZ2) );
	     sumIBLHits += std::max(getIBLHit(xAODwrk->listSelTracks[j]),0);
	     sumBLHits  += std::max(getBLHit(xAODwrk->listSelTracks[j]),0);
             if(m_fillHist && m_curTup) {
	        ntrkBC += getIdHF(xAODwrk->listSelTracks[j]);
	        ntrkI  += getG4Inter(xAODwrk->listSelTracks[j]);
             }
          }
	  float vProb=TMath::Prob(curVrt.chi2, 2*nth-3);
          float cosSVPVM=ProjSV_PV(curVrt.vertex, PrimVrt, curVrt.vertexMom);
	  float vrtR=curVrt.vertex.perp();
	  TLorentzVector SVPV(curVrt.vertex.x()-PrimVrt.x(),curVrt.vertex.y()-PrimVrt.y(),curVrt.vertex.z()-PrimVrt.z(), 10.);
          if(m_fillHist){
             if( m_curTup && nth>1 ){
                VrtVrtDist(PrimVrt,curVrt.vertex, curVrt.vertexCov, signif3D); 
                float Dist2D=VrtVrtDist2D(PrimVrt,curVrt.vertex, curVrt.vertexCov, signif2D); 
                m_curTup->NVrtTrk   [m_curTup->nNVrt] = nth;          
                m_curTup->NVrtTrkHF [m_curTup->nNVrt] = ntrkBC;          
                m_curTup->NVrtTrkI  [m_curTup->nNVrt] = ntrkI;          
                m_curTup->NVrtProb  [m_curTup->nNVrt] = vProb;          
                m_curTup->NVrtSig3D [m_curTup->nNVrt] = signif3D;
                m_curTup->NVrtSig2D [m_curTup->nNVrt] = signif2D;
                m_curTup->NVrtDist2D[m_curTup->nNVrt] = vrtR<20. ? Dist2D : vrtR;
                m_curTup->NVrtM     [m_curTup->nNVrt] = curVrt.vertexMom.M();
                m_curTup->NVrtPt    [m_curTup->nNVrt] = curVrt.vertexMom.Pt();
                m_curTup->NVrtEta   [m_curTup->nNVrt] = SVPV.Eta();
                m_curTup->NVrtIBL   [m_curTup->nNVrt] = sumIBLHits;
                m_curTup->NVrtBL    [m_curTup->nNVrt] = sumBLHits;
                m_curTup->NVrtSinSPM[m_curTup->nNVrt] = sqrt(1.-cosSVPVM*cosSVPVM);
                m_curTup->NVrtCh    [m_curTup->nNVrt] = curVrt.vertexCharge;
                m_curTup->NVMinPtT  [m_curTup->nNVrt] = minPtT;
                m_curTup->NVMinS3DT [m_curTup->nNVrt] = minSig3DT;
                m_curTup->NVrtBDT   [m_curTup->nNVrt] = 1.1;
                if( m_curTup->nNVrt < DevTuple::maxNVrt-1 )m_curTup->nNVrt++;
            }
          }
//-------------------BDT based rejection
          if(nth==2){
	     if(curVrt.vertexMom.Pt() > m_Vrt2TrPtLimit){ curVrt.Good = false; continue; }
             float rhit0=xAODwrk->listSelTracks[curVrt.selTrk[0]]->radiusOfFirstHit();
             float rhit1=xAODwrk->listSelTracks[curVrt.selTrk[1]]->radiusOfFirstHit();
	     VrtVrtDist(PrimVrt,curVrt.vertex, curVrt.vertexCov, signif3D); 
	     float Dist2D=VrtVrtDist2D(PrimVrt,curVrt.vertex, curVrt.vertexCov, signif2D); 
	     std::vector<float> VARS(10);
	     VARS[0]=vProb;
	     VARS[1]=log(curVrt.vertexMom.Pt());
	     VARS[2]=log(std::max(minPtT,m_cutPt));
	     VARS[3]=log(vrtR<20. ? Dist2D : vrtR);
	     VARS[4]=log(std::max(minSig3DT,m_trkSigCut));
	     VARS[5]=log(maxSig3DT);
	     VARS[6]=curVrt.vertexMom.M();
	     VARS[7]=sqrt(fabs(1.-cosSVPVM*cosSVPVM));
	     VARS[8]=SVPV.Eta();
	     VARS[9]=std::max(rhit0,rhit1);
	     //VARS[9]=sumIBLHits;
	     //VARS[10]=sumBLHits;
	     //VARS[4]=std::max(signif3D,m_selVrtSigCut);
	     float wgtSelect=m_SV2T_BDT->GetGradBoostMVA(VARS);
	     //std::vector<float> weights=m_SV2T_BDT->GetMultiResponse(VARS,3);
	     //float wgtSelect=weights[0];
	     if(m_fillHist){
	       m_hb_fakeSVBDT->Fill(wgtSelect,1.);
	       if( m_curTup ) m_curTup->NVrtBDT[m_curTup->nNVrt-1] = wgtSelect;
	     }
	     if(wgtSelect<m_v2tFinBDTCut) curVrt.Good = false;
	   }
    }
//
//--Final cleaning of the 1-track vertices set. Must be behind all other cleanings.
//-- Debug ntuple for 1track vertex is filled here as well
//
    if(m_multiWithOneTrkVrt) Clean1TrVertexSet(WrkVrtSet);
    if(m_fillHist && m_curTup && m_multiWithOneTrkVrt){
      for(auto & V : (*WrkVrtSet)) {
        nth=V.selTrk.size();
        if( !V.Good || nth != 1 ) continue;  // Good 1track vertices
        m_fitSvc->VKalGetImpact(xAODwrk->listSelTracks[V.selTrk[0]], PrimVrt.position(), 1, impact, impactError);
        int ntrkBC = getIdHF(xAODwrk->listSelTracks[V.selTrk[0]]);
	double SigR2 = impact[0]*impact[0]/impactError[0];
	double SigZ2 = impact[1]*impact[1]/impactError[2];
        float Dist2D=VrtVrtDist2D(PrimVrt,V.vertex, V.vertexCov, signif2D); 
        m_curTup->NVrtTrk   [m_curTup->nNVrt] = nth;          
        m_curTup->NVrtTrkHF [m_curTup->nNVrt] = ntrkBC;          
        m_curTup->NVrtProb  [m_curTup->nNVrt] = TMath::Prob(V.chi2, 1);        
        m_curTup->NVrtSig3D [m_curTup->nNVrt] = 0.;
        m_curTup->NVrtSig2D [m_curTup->nNVrt] = signif2D;
        m_curTup->NVrtDist2D[m_curTup->nNVrt] = Dist2D;
        m_curTup->NVrtM     [m_curTup->nNVrt] = V.vertexMom.M();
        m_curTup->NVrtPt    [m_curTup->nNVrt] = V.vertexMom.Pt();
        m_curTup->NVrtSinSPM[m_curTup->nNVrt] = 0.;
        m_curTup->NVrtCh    [m_curTup->nNVrt] = V.vertexCharge;
        m_curTup->NVMinPtT  [m_curTup->nNVrt] = xAODwrk->listSelTracks[V.selTrk[0]]->pt();
        m_curTup->NVMinS3DT [m_curTup->nNVrt] = sqrt(SigR2 + SigZ2);
        m_curTup->NVrtBDT   [m_curTup->nNVrt] = 1.1;
        if( m_curTup->nNVrt < DevTuple::maxNVrt-1 )m_curTup->nNVrt++;
   }  }
//-------------------------------------------
//Checks
    std::vector<WrkVrt> GoodVertices(0);
    nGoodVertices=0;         // Final number of good vertices
    n2trVrt=nNtrVrt=0;       // N of vertices with different amount of tracks
    for(auto & iv : (*WrkVrtSet) ) {
       nth=iv.selTrk.size(); if(nth == 0) continue;   /* Definitely bad vertices */
       if( iv.Good) {
	  nGoodVertices++;                                    
	  GoodVertices.emplace_back(iv);    /* add it */
	  if(nth==2)n2trVrt++;
	  if(nth>=3)nNtrVrt++;
       }
    }
    if(nGoodVertices == 0 || (n2trVrt+nNtrVrt)==0){  // No good vertices at all
      delete WrkVrtSet;  delete TrkInVrt;
      if(m_fillHist && m_curTup ) m_curTup->nNVrt=0;
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
        if(GoodVertices[iv].projectedVrt > GoodVertices[iv+1].projectedVrt){
	  std::swap( GoodVertices[iv], GoodVertices[iv+1] );
          swapDone=true; 
        }
      }    if(!swapDone) break;
    }
    if(nGoodVertices>1){
      if( GoodVertices[1].vertexMom.M()-GoodVertices[0].vertexMom.M() > 5000.) std::swap( GoodVertices[0], GoodVertices[1] );
    }
    if(m_fillHist){m_hb_distVV->Fill( minVrtVrtDist( WrkVrtSet, foundV1, foundV2), m_w_1); }
//-------------------------------------------
// Saving and final cleaning
//
    nGoodVertices=0;         // Final number of good vertices
    int n1trVrt=0;           // Final number of good 1-track vertices
    TLorentzVector VertexMom;
    for(int iv=0; iv<(int)GoodVertices.size(); iv++) {
          nth=GoodVertices[iv].selTrk.size();
          xAODwrk->tmpListTracks.clear();
          for(i=0;i<nth;i++) {
             j=GoodVertices[iv].selTrk[i];                           /*Track number*/
             xAODwrk->tmpListTracks.push_back( xAODwrk->listSelTracks[j] );
          }
          if(m_fillHist){ 
	     m_hb_totmass->Fill(GoodVertices[iv].vertexMom.M(), m_w_1);
             m_hb_r2d->Fill( GoodVertices[iv].vertex.perp(), m_w_1); } 
// xAOD::Vertex creation-----------------------------
          xAOD::Vertex * tmpVertex=new xAOD::Vertex();
          tmpVertex->makePrivateStore();
          tmpVertex->setPosition(GoodVertices[iv].vertex);
          int NERMS=GoodVertices[iv].vertexCov.size();
	  NERMS=6;
          std::vector<float> floatErrMtx;   floatErrMtx.resize(NERMS);
          for(int i=0; i<NERMS; i++) floatErrMtx[i]=GoodVertices[iv].vertexCov[i];
          tmpVertex->setCovariance(floatErrMtx);
          tmpVertex->setFitQuality(GoodVertices[iv].chi2, (float)(nth*2-3));

          std::vector<Trk::VxTrackAtVertex> & tmpVTAV=tmpVertex->vxTrackAtVertex();    tmpVTAV.clear();
          for(int ii=0; ii<nth; ii++) {
             AmgSymMatrix(5) *CovMtxP=new AmgSymMatrix(5);    (*CovMtxP).setIdentity(); 
             Trk::Perigee * tmpMeasPer  =  new Trk::Perigee( 0.,0.,  GoodVertices[iv].trkAtVrt[ii][0], 
	                                                             GoodVertices[iv].trkAtVrt[ii][1],
                                                                     GoodVertices[iv].trkAtVrt[ii][2],
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
          if(nth==1)n1trVrt++;
//-----
           VertexMom += GoodVertices[iv].vertexMom;
    }
    if(m_fillHist){
      m_hb_goodvrtN->Fill( nGoodVertices+0.1, m_w_1);
      m_hb_goodvrt1N->Fill( n1trVrt+0.1, m_w_1);
    }


    if(nGoodVertices == 0){
      delete WrkVrtSet;
      delete TrkInVrt;
      return finalVertices;
    }

//-----------------------------------------------------------------------------------
//  Saving of results
//
//


      delete WrkVrtSet; delete TrkInVrt;

      return finalVertices;


  }


} //end namespace


