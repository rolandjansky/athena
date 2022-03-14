/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
                                                                      const xAOD::Vertex & primVrt )
   const
   {

      const double probVrtMergeLimit=0.01;

      int i,j;
      int inpNPart=xAODwrk->inpTrk.size();
      std::vector<const xAOD::NeutralParticle*> neutralPartDummy(0);
      ATH_MSG_DEBUG( "getVrtSecMulti() called with NPart=" << inpNPart);

      std::vector<xAOD::Vertex*>  finalVertices(0); 

      if( inpNPart < 2 ) { return finalVertices;}   // 0,1 track => nothing to do!

      selGoodTrkParticle( xAODwrk, primVrt);
      int nTracks = xAODwrk->listSelTracks.size();

      if( nTracks < 2 ) { return finalVertices;} // 0,1 selected track => nothing to do!

      ATH_MSG_DEBUG( "Number of selected tracks = " <<nTracks);
      
      if(m_fillHist){ m_hb_ntrksel->Fill( (double) nTracks, m_w_1); }

//
//  inpTrk[]           - input track list
//  listSelTracks[]    - list of good tracks in jet for vertex search
//------------------------------------------------------------	 
//                     Initial track list ready
//                     Find 2track vertices
//

      std::map<long int,std::vector<double>> foundVrt2t;
      select2TrVrt(xAODwrk->listSelTracks, primVrt, foundVrt2t);

//---
      ATH_MSG_DEBUG(" Defined edges in the graph="<< num_edges(*m_compatibilityGraph));
      if(num_edges(*m_compatibilityGraph)==0){ return finalVertices;} //No vertices!

//
//  m_Incomp[]           -  main vector of pointers for multivertex search
//-----------------------------------------------------------------------------------------------------
//            Secondary track list is ready
//            Creation of initial vertex set
//


      std::unique_ptr<std::vector<WrkVrt>> wrkVrtSet = std::make_unique<std::vector<WrkVrt>>();
      WrkVrt newvrt; newvrt.Good=true;
      std::unique_ptr<Trk::IVKalState> state = m_fitSvc->makeState();
      StatusCode sc;
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
          std::vector<double> iniVrtPos=estimVrtPos(nTracks,newvrt.selTrk,foundVrt2t);
          m_fitSvc->setApproximateVertex(iniVrtPos[0], iniVrtPos[1], iniVrtPos[2], *state); /*Use as starting point*/
          sc=m_fitSvc->VKalVrtFit(xAODwrk->tmpListTracks, neutralPartDummy,
                                             newvrt.vertex,     newvrt.vertexMom, newvrt.vertexCharge, newvrt.vertexCov,
                                             newvrt.chi2PerTrk, newvrt.trkAtVrt,  newvrt.chi2,
                                             *state, false);
          if( sc.isFailure() )           continue;   /* Bad fit - goto next solution */
          ATH_MSG_VERBOSE("Found IniVertex="<<newvrt.vertex[0]<<", "<<newvrt.vertex[1]<<", "<<newvrt.vertex[2]);
          ATH_MSG_VERBOSE("with Chi2="<<newvrt.chi2<<" Ntrk="<<NPTR<<" trk1,2="<<newvrt.selTrk[0]<<", "<<newvrt.selTrk[1]); 
          if(NPTR==2 && newvrt.chi2>10.) continue;   /* Bad 2track vertex */
          if(newvrt.chi2PerTrk.size()==2) newvrt.chi2PerTrk[0]=newvrt.chi2PerTrk[1]=newvrt.chi2/2.;
          newvrt.Good         = true;
          newvrt.projectedVrt=MomProjDist(newvrt.vertex, primVrt, newvrt.vertexMom); //3D SV-PV distance
          wrkVrtSet->push_back(newvrt);
    } 
//==================================================================================
// boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS>::vertex_iterator vertexIt, vertexEnd;
// boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS>::adjacency_iterator neighbourIt, neighbourEnd;
// tie(vertexIt, vertexEnd) = vertices(*m_compatibilityGraph);
// for (; vertexIt != vertexEnd; ++vertexIt) { std::cout << *vertexIt << " is connected with "; 
//    tie(neighbourIt, neighbourEnd) = adjacent_vertices(*vertexIt, *m_compatibilityGraph); 
//    for (; neighbourIt != neighbourEnd; ++neighbourIt) std::cout << *neighbourIt << " ";   std::cout << "\n"; }
//==================================================================================
    if((*wrkVrtSet).size()==0)return finalVertices;
    if(msgLvl(MSG::DEBUG))printWrkSet(wrkVrtSet.get(),"Initial Vertices");
    //
    //--Count track participation in different vertices
    std::vector<int> trkNPairs(nTracks,0);
    for(auto &vrt : (*wrkVrtSet)){
       int ntInV=vrt.selTrk.size()-1;
       for(auto &trk : vrt.selTrk)trkNPairs.at(trk) +=  ntInV;
    }
//
//- Resolve all overlapped vertices
//
    state = m_fitSvc->makeState();
    std::multimap<double,std::pair<int,int>> vrtWithCommonTrk;
    while(true){
      int nSoluI=(*wrkVrtSet).size();
      vrtWithCommonTrk.clear();
      unsigned int nTComMax=0;
      for(int iv=0; iv<nSoluI-1; iv++ ){  
        if(!(*wrkVrtSet)[iv].Good)                      continue;
        if( (*wrkVrtSet)[iv].selTrk.size()<nTComMax)    continue;  // Optimisation. Only biggest overlap matters
        for(int jv=iv+1; jv<nSoluI; jv++){
          if(!(*wrkVrtSet)[jv].Good)                    continue;
          if( (*wrkVrtSet)[jv].selTrk.size()<nTComMax)  continue;  // Optimisation. Only biggest overlap matters
          unsigned int nTCom=nTrkCommon( wrkVrtSet.get(), iv, jv);
          if(!nTCom)         continue;
          if(nTCom<nTComMax) continue;
          double sumChi2=(*wrkVrtSet)[iv].chi2+(*wrkVrtSet)[jv].chi2;
          sumChi2=std::min(sumChi2,999.)*1.e-3;
          vrtWithCommonTrk.emplace(nTCom+sumChi2,std::make_pair(iv,jv));
          nTComMax=std::max(nTComMax,nTCom);
      } }
      if(vrtWithCommonTrk.size()==0)break;
      //============================== DEBUG output
      //for(auto ku : vrtWithCommonTrk)std::cout<<" nCom="<<ku.first<<" v1="<<ku.second.first<<" v2="<<ku.second.second<<'\n';
      //if(msgLvl(MSG::DEBUG))printWrkSet(wrkVrtSet.get(),"Overlapped  Vertex Cleaning");
      //===========================================
      unsigned int nTCom=(*vrtWithCommonTrk.rbegin()).first;
      WrkVrt  & v1 = (*wrkVrtSet)[(*vrtWithCommonTrk.rbegin()).second.first];
      WrkVrt  & v2 = (*wrkVrtSet)[(*vrtWithCommonTrk.rbegin()).second.second];
      //--First check if one vertex is fully contained in another
      if( nTCom==v1.selTrk.size() || nTCom==v2.selTrk.size() ){
         if(nTCom==v1.selTrk.size()){v1.Good = false; continue;}
         if(nTCom==v2.selTrk.size()){v2.Good = false; continue;}
      }
      //--Then check if 2 vertices with common tracks can be simply merged
      if( nTCom>1 && TMath::Prob( v1.chi2, 2*v1.selTrk.size()-3) > probVrtMergeLimit
                  && TMath::Prob( v2.chi2, 2*v2.selTrk.size()-3) > probVrtMergeLimit){
          double prbV=mergeAndRefitVertices( v1, v2, newvrt, xAODwrk->listSelTracks, *state);
          if(prbV>probVrtMergeLimit){
             v1.Good = false;  v2.Good = false;
             newvrt.Good         = true;
             newvrt.projectedVrt=MomProjDist(newvrt.vertex, primVrt, newvrt.vertexMom); //3D SV-PV distance
             std::swap(v1,newvrt); //Replace v1 by new vertex
             continue;
       }  }
       //--If not mergeable - refine them
       refineVerticesWithCommonTracks( v1, v2, xAODwrk->listSelTracks, *state);
    }
    if(m_fillHist){ int cvgood=0; for(auto vrt:(*wrkVrtSet)) if(vrt.Good)cvgood++;
                    m_hb_rawVrtN->Fill( (float)cvgood, m_w_1); }
//
//-Clean duplicated 1track vertices if they exist
//
    if(m_multiWithOneTrkVrt){
      for(auto &v1t : (*wrkVrtSet)){
        if(v1t.selTrk.size()!=1 || !v1t.Good)continue;
        int ind_t=v1t.selTrk[0];
        if(trkNPairs[ind_t]<2){ v1t.Good=false; continue; } //Remove 1tr-vertex if track crosses only one other track
        if( xAODwrk->listSelTracks[ind_t]->pt()<m_cutPt*2){ v1t.Good=false; continue; }; //Tighten track_pt cut for 1-track vertex
        for(auto &vrt :(*wrkVrtSet)){   // Check if the track is present in another vertex, including other 1-track ones
          if(!vrt.Good || &v1t==&vrt)continue;
          if(std::find(vrt.selTrk.begin(),vrt.selTrk.end(),ind_t) != vrt.selTrk.end()){ v1t.Good=false; break; }
      } }
    }
//
//-Remove all bad vertices from the working set
//
    int tmpV=0; while( tmpV<(int)(*wrkVrtSet).size() )if( !(*wrkVrtSet)[tmpV].Good ) { (*wrkVrtSet).erase((*wrkVrtSet).begin()+tmpV);} else {tmpV++;}
    if((*wrkVrtSet).size()==0)return finalVertices;
    if(msgLvl(MSG::DEBUG))printWrkSet(wrkVrtSet.get(),"Intermediate Vertices");
    for( auto &tmpV : (*wrkVrtSet) ) tmpV.projectedVrt=MomProjDist(tmpV.vertex, primVrt, tmpV.vertexMom );  //Setup ProjectedVrt
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//
// Final check/merge for close vertices
//
    int foundV1=-1, foundV2=-1;
    std::vector<double> checkedDst(0);
    double minDistVV=minVrtVrtDist( wrkVrtSet.get(), foundV1, foundV2, checkedDst); //recalculate VV distances
    if(m_fillHist){m_hb_distVV->Fill( minDistVV, m_w_1); }
    while ( minDistVV < m_vertexMergeCut) {
        if(foundV1<foundV2) { int tmp=foundV1; foundV1=foundV2; foundV2=tmp;}
        double probV=mergeAndRefitVertices( (*wrkVrtSet)[foundV1], (*wrkVrtSet)[foundV2], newvrt, xAODwrk->listSelTracks, *state, 0);
        ATH_MSG_DEBUG( "Merged vertex prob=" << probV<<" Vrt1="<<foundV1<<" Vrt2="<<foundV2<<" dst="<<minDistVV);
        if(probV<probVrtMergeLimit){ //--- If merged vertex is bad - try to remove the worst track
           int pos=std::max_element(newvrt.chi2PerTrk.begin(),newvrt.chi2PerTrk.end())-newvrt.chi2PerTrk.begin();
           newvrt.detachedTrack=newvrt.selTrk[pos];
           newvrt.selTrk.erase(newvrt.selTrk.begin()+pos);
           probV = refitVertex( newvrt, xAODwrk->listSelTracks, *state, false);
           ATH_MSG_DEBUG( "Attempt to improve prob=" << probV);
        }
        if(probV>probVrtMergeLimit && newvrt.vertexMom.M()<m_vrtMassLimit){        //  Good merged vertex found
           newvrt.projectedVrt=MomProjDist(newvrt.vertex, primVrt, newvrt.vertexMom);
           std::swap((*wrkVrtSet)[foundV1],newvrt);
           (*wrkVrtSet)[foundV2].Good=false;         //Drop vertex
           (*wrkVrtSet)[foundV2].selTrk.clear();     //Clean dropped vertex
        } else checkedDst.push_back(minDistVV);
        minDistVV=minVrtVrtDist( wrkVrtSet.get(), foundV1, foundV2, checkedDst);
    }
//
// Try to improve vertices with big Chi2 if something went wrong. Just precaution.
    for(int iv=0; iv<(int)wrkVrtSet->size(); iv++) {
       if(!(*wrkVrtSet)[iv].Good )                 continue;  //don't work on vertex which is already bad
       if( (*wrkVrtSet)[iv].selTrk.size()<3 )      continue;
       double tmpProb=TMath::Prob( (*wrkVrtSet)[iv].chi2, 2*(*wrkVrtSet)[iv].selTrk.size()-3 ); //Chi2 of the original vertex
       if(tmpProb<m_globVrtProbCut){
         ATH_MSG_DEBUG( "BAD vertex found prob=" << tmpProb);
         tmpProb=improveVertexChi2( (*wrkVrtSet)[iv], xAODwrk->listSelTracks, *state, false);
         (*wrkVrtSet)[iv].projectedVrt=MomProjDist((*wrkVrtSet)[iv].vertex, primVrt, (*wrkVrtSet)[iv].vertexMom);
       }
    }
//
//-Modify too heavy vertices 
    for(auto & iv : (*wrkVrtSet)){
      if( iv.vertexMom.M()>m_vrtMassLimit ) {
        ATH_MSG_DEBUG( "Heavy vertex found Mass=" << iv.vertexMom.M());
        int it_bad=mostHeavyTrk(iv,xAODwrk->listSelTracks);
        if(it_bad>-1){
        iv.selTrk.erase( iv.selTrk.begin() + it_bad );
        refitVertex(iv, xAODwrk->listSelTracks, *state, false);
        iv.projectedVrt=MomProjDist(iv.vertex, primVrt, iv.vertexMom);
     } } }
//--------------------------------------------------------------------------------------------------------
// Final vertex selection/cleaning
//
    double signif3D=0., signif2D=0.;

//-----  Vertices with >1 tracks
    for(int iv=0; iv<(int)wrkVrtSet->size(); iv++) {
          WrkVrt & curVrt=(*wrkVrtSet)[iv];
          nth=(*wrkVrtSet)[iv].selTrk.size(); 
          if(nth == 1)                          continue;  // 1track vertices for later...
          if(!curVrt.Good )                     continue;  //don't work on vertex which is already bad
          (*wrkVrtSet)[iv].Good = false;                                     /* Make all vertices bad for futher selection */
          if(nth < 1)                           continue;                    /* Definitely bad vertices */
          if((*wrkVrtSet)[iv].projectedVrt<0.)  continue;                    /* Remove vertices behind primary one */ 
          if( TMath::Prob( curVrt.chi2, 2*nth-3)<m_globVrtProbCut) continue;           /* Bad Chi2 of refitted vertex  */
//-----------------------------------------------------------------------------------------
          vrtVrtDist(primVrt,curVrt.vertex, curVrt.vertexCov, signif3D); 
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
             if( std::abs(mass_PiPi-m_massK0) < 22.)     continue;
             if( std::abs(mass_PPi-m_massLam) <  8.)     continue;
             if( mass_EE < 60. && curVrt.vertex.perp() > 20.) continue;
          }
#endif
//---
          if(signif3D<m_selVrtSigCut)                 continue;      //Main PV-SV distance quality cut 
          if(curVrt.vertex.perp() > m_maxSVRadiusCut) continue;      // Too far from interaction point
          curVrt.Good = true;  /* Vertex is absolutely good */
    }
    if(msgLvl(MSG::DEBUG))printWrkSet(wrkVrtSet.get(),"Final Vertices");
//-------------------------------------------
// Debug ntuple filling and BDT application
//
    std::vector<double> impact,impactError;
    for(int iv=0; iv<(int)wrkVrtSet->size(); iv++) {
          WrkVrt & curVrt=(*wrkVrtSet)[iv];
          nth=curVrt.selTrk.size();
          if(!curVrt.Good || nth<2)      continue;
          double minPtT=1.e6, minSig3DT=1.e6, maxSig3DT=0.;
          int ntrkBC=0,ntrkI=0,sumIBLHits=0,sumBLHits=0;
          for(i=0;i<nth;i++) {
             j=curVrt.selTrk[i];                           //Track number
             minPtT=std::min( minPtT, xAODwrk->listSelTracks[j]->pt());
             m_fitSvc->VKalGetImpact(xAODwrk->listSelTracks[j], primVrt.position(), 1, impact, impactError);
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
          float cosSVPVM=projSV_PV(curVrt.vertex, primVrt, curVrt.vertexMom);
          float vrtR=curVrt.vertex.perp();
          TLorentzVector SVPV(curVrt.vertex.x()-primVrt.x(),curVrt.vertex.y()-primVrt.y(),curVrt.vertex.z()-primVrt.z(), 10.);
          if(m_fillHist){
             if( m_curTup && nth>1 ){
                vrtVrtDist(primVrt,curVrt.vertex, curVrt.vertexCov, signif3D); 
                float Dist2D=vrtVrtDist2D(primVrt,curVrt.vertex, curVrt.vertexCov, signif2D); 
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
                m_curTup->NVrtCosSPM[m_curTup->nNVrt] = cosSVPVM;
                m_curTup->NVrtCh    [m_curTup->nNVrt] = curVrt.vertexCharge;
                m_curTup->NVMinPtT  [m_curTup->nNVrt] = minPtT;
                m_curTup->NVMinS3DT [m_curTup->nNVrt] = minSig3DT;
                m_curTup->NVrtBDT   [m_curTup->nNVrt] = 1.1;
                m_curTup->NVrtHR1   [m_curTup->nNVrt] = xAODwrk->listSelTracks[curVrt.selTrk[0]]->radiusOfFirstHit();
                m_curTup->NVrtHR2   [m_curTup->nNVrt] = xAODwrk->listSelTracks[curVrt.selTrk[1]]->radiusOfFirstHit();
                if( m_curTup->nNVrt < DevTuple::maxNVrt-1 )m_curTup->nNVrt++;
            }
          }
//-------------------BDT based rejection
          if(nth==2){
             float curVrtPt=std::min(curVrt.vertexMom.Pt(), (double)m_vrt2TrPtLimit);
             float rhit0=xAODwrk->listSelTracks[curVrt.selTrk[0]]->radiusOfFirstHit();
             float rhit1=xAODwrk->listSelTracks[curVrt.selTrk[1]]->radiusOfFirstHit();
             std::vector<float> VARS(10);
             VARS[0]=vProb;
             VARS[1]=log(curVrtPt);
             VARS[2]=log(std::max(minPtT,m_cutPt));
             VARS[3]=log(vrtR<20. ? SVPV.Perp() : vrtR);
             VARS[4]=log(std::max(minSig3DT,m_trkSigCut));
             VARS[5]=log(maxSig3DT);
             VARS[6]=curVrt.vertexMom.M();
             VARS[7]=sqrt(std::abs(1.-cosSVPVM*cosSVPVM));
             VARS[8]=SVPV.Eta();
             VARS[9]=std::max(rhit0,rhit1);
             float wgtSelect=m_SV2T_BDT->GetGradBoostMVA(VARS);
             if(m_fillHist){
               m_hb_fakeSVBDT->Fill(wgtSelect,1.);
               if( m_curTup ) m_curTup->NVrtBDT[m_curTup->nNVrt-1] = wgtSelect;
             }
             if(wgtSelect<m_v2tFinBDTCut) curVrt.Good = false;
          }
    }
//
//-- Debug ntuple for 1track vertex is filled here
//
    if(m_fillHist && m_curTup && m_multiWithOneTrkVrt){
      for(auto & vrt : (*wrkVrtSet)) {
        if( !vrt.Good || vrt.selTrk.size() != 1 ) continue;  // Good 1track vertices
        auto xaodtp=xAODwrk->listSelTracks[vrt.selTrk[0]];
        m_fitSvc->VKalGetImpact(xaodtp, primVrt.position(), 1, impact, impactError);
        double SigR2 = std::abs(impact[0]*impact[0]/impactError[0]);
        double SigZ2 = std::abs(impact[1]*impact[1]/impactError[2]);
        float dist2D=vrtVrtDist2D(primVrt,vrt.vertex, vrt.vertexCov, signif2D); 
        m_curTup->NVrtTrk   [m_curTup->nNVrt] = 1;
        m_curTup->NVrtTrkHF [m_curTup->nNVrt] = getIdHF(xaodtp);
        m_curTup->NVrtProb  [m_curTup->nNVrt] = trkNPairs[vrt.selTrk[0]];
        m_curTup->NVrtSig3D [m_curTup->nNVrt] = 0.;
        m_curTup->NVrtSig2D [m_curTup->nNVrt] = signif2D;
        m_curTup->NVrtDist2D[m_curTup->nNVrt] = dist2D;
        m_curTup->NVrtM     [m_curTup->nNVrt] = vrt.vertexMom.M();
        m_curTup->NVrtPt    [m_curTup->nNVrt] = vrt.vertexMom.Pt();
        m_curTup->NVrtCosSPM[m_curTup->nNVrt] = 0.;
        m_curTup->NVrtCh    [m_curTup->nNVrt] = vrt.vertexCharge;
        m_curTup->NVMinPtT  [m_curTup->nNVrt] = xaodtp->pt();
        m_curTup->NVMinS3DT [m_curTup->nNVrt] = sqrt(SigR2 + SigZ2);
        m_curTup->NVrtBDT   [m_curTup->nNVrt] = 1.1;
        m_curTup->NVrtIBL   [m_curTup->nNVrt] = std::max(getIBLHit(xaodtp),0);
        m_curTup->NVrtBL    [m_curTup->nNVrt] = std::max(getBLHit (xaodtp),0);
        if( m_curTup->nNVrt < DevTuple::maxNVrt-1 )m_curTup->nNVrt++;
   }  }
//-------------------------------------------
//Sorting and check
    std::multimap<double,WrkVrt> goodVertexMap;
    int nNtrVrt=0;
    for(auto & iv : (*wrkVrtSet) ) {
       nth=iv.selTrk.size(); 
       if( iv.Good && nth>0 ) {
          goodVertexMap.emplace(iv.vertex.perp(),iv);    // add it
          if(nth>1)nNtrVrt++;
       }
    }
    if(nNtrVrt==0){             //--- No good vertices at all
      if(m_fillHist && m_curTup ) m_curTup->nNVrt=0;
      return finalVertices;
    }
//
//-------------------------------------------
// Final vertex refit for full covariance matrix and xAOD::Vertex creation
//
    int n1trVrt=0;           // Final number of good 1-track vertices
    for(auto & iv : goodVertexMap){
          WrkVrt & curVrt=iv.second;
          nth=curVrt.selTrk.size();
          xAODwrk->tmpListTracks.clear();
          for(auto t : curVrt.selTrk)xAODwrk->tmpListTracks.push_back( xAODwrk->listSelTracks[t] );
          if(m_fillHist){ 
             m_hb_totmass->Fill(curVrt.vertexMom.M(), m_w_1);
             m_hb_r2d->Fill( curVrt.vertex.perp(), m_w_1); } 
//--- Re-fit with full error matrix and xAOD::Vertex creation
          xAOD::Vertex * tmpVertex=nullptr;
          if(nth>1){                                    //-- Common case with full refit
             tmpVertex=m_fitSvc->fit(xAODwrk->tmpListTracks,curVrt.vertex,*state);
          } else if(nth==1){                            //-- Special case for 1-track vertex
             xAOD::Vertex * tmpVertex=new (std::nothrow) xAOD::Vertex();
             if(!tmpVertex)continue;
             tmpVertex->makePrivateStore();
             tmpVertex->setPosition(curVrt.vertex);
             std::vector<float> floatErrMtx(6);
             for(int i=0; i<6; i++) floatErrMtx[i]=curVrt.vertexCov[i];
             tmpVertex->setCovariance(floatErrMtx);
             tmpVertex->setFitQuality(curVrt.chi2, (float)(nth*2-3));
             std::vector<Trk::VxTrackAtVertex> & tmpVTAV=tmpVertex->vxTrackAtVertex();    tmpVTAV.clear();
             AmgSymMatrix(5) CovMtxP;    
             CovMtxP.setIdentity(); 
             Trk::Perigee * tmpMeasPer  =  new (std::nothrow) Trk::Perigee( 0.,0.,
                                                                     curVrt.trkAtVrt[0][0],
                                                                     curVrt.trkAtVrt[0][1],
                                                                     curVrt.trkAtVrt[0][2],
                                                              Trk::PerigeeSurface(curVrt.vertex),
                                                              std::move(CovMtxP) );
             tmpVTAV.push_back( Trk::VxTrackAtVertex( 1., tmpMeasPer) );
             ElementLink<xAOD::TrackParticleContainer> TEL;  TEL.setElement( xAODwrk->tmpListTracks[0] );
             const xAOD::TrackParticleContainer* cont = (const xAOD::TrackParticleContainer* ) (xAODwrk->tmpListTracks[0]->container() );
             TEL.setStorableObject(*cont);
             tmpVertex->addTrackAtVertex(TEL,1.);
             n1trVrt++;
          }
          if(tmpVertex)finalVertices.push_back(tmpVertex);
    }
    if(m_fillHist){
      m_hb_goodvrtN->Fill( finalVertices.size()+0.1, m_w_1);
      m_hb_goodvrt1N->Fill( n1trVrt+0.1, m_w_1);
    }
//-----------------------------------------------------------------------------------
//  Saving of results
//
    return finalVertices;

  }

} //end namespace


