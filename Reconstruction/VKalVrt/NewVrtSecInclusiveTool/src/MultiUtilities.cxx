/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
///
///     @author Vadim Kostyukhin <vadim.kostyukhin@cern.ch>
///
// Header include
#include "NewVrtSecInclusiveTool/NewVrtSecInclusiveTool.h"
//-------------------------------------------------
// Other stuff
#include  "AnalysisUtils/AnalysisMisc.h"
#include  "GeoPrimitives/GeoPrimitivesHelpers.h"
#include  "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include  "TMath.h"
#include  <algorithm>
//


namespace Rec{


                      /* Service routines*/

//-----------------------------------------------------------------------------------
//  Find track contributing most to the vertex invariant mass
   int NewVrtSecInclusiveTool::mostHeavyTrk(WrkVrt V, std::vector<const xAOD::TrackParticle*> AllTracks)
   const
   {
      int NTrk=V.selTrk.size(), SelT=-1;
      if(NTrk<3)return -1;
//--------------------------------------------------
      TLorentzVector TSum;
      for(int i=0; i<NTrk; i++){
         TSum += AllTracks.at(V.selTrk[i])->p4();
      }
      double massMin=1.e9;
      for(int i=0; i<NTrk; i++){
         TLorentzVector TSum_m1 = TSum - AllTracks[V.selTrk[i]]->p4();
         if(TSum_m1.M()<massMin){massMin=TSum_m1.M(); SelT=i;}
      }
      return SelT;
    }


//
//  Number of common tracks for 2 vertices
//
   int NewVrtSecInclusiveTool::nTrkCommon( std::vector<WrkVrt> *wrkVrtSet, int V1, int V2)
   const
   {
      int nTrk_V1 = (*wrkVrtSet).at(V1).selTrk.size(); if( nTrk_V1< 2) return 0;   /* Bad vertex */
      int nTrk_V2 = (*wrkVrtSet).at(V2).selTrk.size(); if( nTrk_V2< 2) return 0;   /* Bad vertex */
      int nTrkCom=0;
      if(nTrk_V1 < nTrk_V2){
        for(int i=0; i<nTrk_V1; i++){
          int trk=(*wrkVrtSet)[V1].selTrk[i];
          for(int j=0; j<nTrk_V2; j++){ if( trk==(*wrkVrtSet)[V2].selTrk[j]){ nTrkCom++; break;} }
        }
      }else{
        for(int i=0; i<nTrk_V2; i++){
          int trk=(*wrkVrtSet)[V2].selTrk[i];
          for(int j=0; j<nTrk_V1; j++){ if( trk==(*wrkVrtSet)[V1].selTrk[j]){ nTrkCom++; break;} }
        }
      }
      return nTrkCom;
   }
//
//  Minimal normalized vertex-vertex distance
//
   double NewVrtSecInclusiveTool::minVrtVrtDist( std::vector<WrkVrt> *wrkVrtSet, int & V1, int & V2, std::vector<double> & checked)
   const
   {  
     V1=V2=-1;
     double foundMinVrtDst=1000000.;

     for(int iv=0; iv<(int)wrkVrtSet->size()-1; iv++) {
        if( (*wrkVrtSet)[iv].selTrk.size()< 2)    continue;   /* Bad vertices */
        if(!(*wrkVrtSet)[iv].Good )               continue;   /* Bad vertices */
        for(int jv=iv+1; jv<(int)wrkVrtSet->size(); jv++) {
           if( (*wrkVrtSet)[jv].selTrk.size()< 2) continue;   /* Bad vertices */
           if(!(*wrkVrtSet)[jv].Good )            continue;   /* Bad vertices */
           double tmp= std::abs((*wrkVrtSet)[iv].vertex.x()-(*wrkVrtSet)[jv].vertex.x())
                      +std::abs((*wrkVrtSet)[iv].vertex.y()-(*wrkVrtSet)[jv].vertex.y())
                      +std::abs((*wrkVrtSet)[iv].vertex.z()-(*wrkVrtSet)[jv].vertex.z());
           if(tmp>20.) continue;
           double tmpDst = vrtVrtDist((*wrkVrtSet)[iv].vertex,(*wrkVrtSet)[iv].vertexCov,
                                      (*wrkVrtSet)[jv].vertex,(*wrkVrtSet)[jv].vertexCov);
           if(std::find(checked.begin(),checked.end(),tmpDst) != checked.end()) continue; //Alreasy tried
           if( tmpDst < foundMinVrtDst){foundMinVrtDst = tmpDst; V1=iv; V2=jv;} 
         }
      }
      return foundMinVrtDst;
   }

//
//  Refine two vertices with common tracks.
//  Main logics for clique->vertex conversion is implemented here.
//  Common tracks are removed from "bad" vertex and left in "good" vertex.
//
   double  NewVrtSecInclusiveTool::refineVerticesWithCommonTracks( WrkVrt &v1, WrkVrt &v2,
                                                        std::vector<const xAOD::TrackParticle*> & allTrackList,
                                                        Trk::IVKalState& istate)
   const
   {
      if(!v1.Good || !v2.Good)return -1.;         //bad vertex check for safety

      int ntv1=v1.selTrk.size();
      int ntv2=v2.selTrk.size();
      if( ntv1<2 || ntv2<2 ) return -1.;          //for safety
      double prb_v1=TMath::Prob( v1.chi2, 2*ntv1-3);
      double prb_v2=TMath::Prob( v2.chi2, 2*ntv2-3);
      WrkVrt * badV =&v1;
      WrkVrt * goodV=&v2;
      bool swap=false;
      //---- Good vertex selection
      if(prb_v1>0.01&&prb_v2>0.01){     //Both vertices are good Prob>1%
         if( ntv1==ntv2 ) {if(prb_v1>prb_v2) swap=true;}  //If multiplicities are equal- select better Chi2
         else             {if(ntv1>ntv2)     swap=true;}
      }
      if(prb_v1<0.01&&prb_v2<0.01){     //Both vertices are bad Prob<1%
         if(prb_v1>prb_v2) swap=true;   // select better Chi2
      }
      if(prb_v1>0.01&&prb_v2<0.01){     //Second vertex is bad Prob<1%
         if(prb_v1>prb_v2) swap=true;   
      }
      if(swap) {badV =&v2; goodV =&v1;}
      int badVNtrk=(*badV).selTrk.size();
      //-----------------
      unsigned int it=0;
      while( it<(*badV).selTrk.size() ){
         int trk=(*badV).selTrk[it];
         if(std::find((*goodV).selTrk.begin(),(*goodV).selTrk.end(),trk) != (*goodV).selTrk.end()){
            (*badV).selTrk.erase((*badV).selTrk.begin()+it);
            (*badV).detachedTrack=trk;
         } else it++;
      }
      if((*badV).selTrk.size()<2){
         (*badV).Good=false;
         if((*badV).selTrk.size()==1 && m_multiWithOneTrkVrt){  //Special case if 1-track vertices are allowed
           (*badV).vertexCharge=allTrackList[(*badV).selTrk.at(0)]->charge();
           (*badV).vertexMom=allTrackList[(*badV).selTrk.at(0)]->p4();
           if(badVNtrk>=2)(*badV).Good=true;  //For 1-track vertices
         }
         return -1.;
      }
      return refitVertex((*badV),allTrackList, istate, false);
   }
//
//  Check two close vertices by explicit vertex fit and create combined vertex if successful
//
   double  NewVrtSecInclusiveTool::mergeAndRefitVertices( WrkVrt & v1, WrkVrt & v2, WrkVrt & newvrt,
                                                        std::vector<const xAOD::TrackParticle*> & AllTrackList,
                                                        Trk::IVKalState& istate, int robKey)
   const
   {
      if(!v1.Good)return -1.;         //bad vertex
      if(!v2.Good)return -1.;         //bad vertex
      std::vector<const xAOD::NeutralParticle*> neutralPartDummy(0);
      newvrt.Good=true;
      int nTrk_V1=v1.selTrk.size();
      int nTrk_V2=v2.selTrk.size();
      newvrt.selTrk.resize(nTrk_V1+nTrk_V2);
      std::copy(v1.selTrk.begin(),v1.selTrk.end(), newvrt.selTrk.begin());
      std::copy(v2.selTrk.begin(),v2.selTrk.end(), newvrt.selTrk.begin()+nTrk_V1);

      std::deque<long int>::iterator   TransfEnd ;
      sort( newvrt.selTrk.begin(), newvrt.selTrk.end() );
      TransfEnd =  unique( newvrt.selTrk.begin(), newvrt.selTrk.end() );
      newvrt.selTrk.erase( TransfEnd, newvrt.selTrk.end());
      std::vector<const xAOD::TrackParticle*>  fitTrackList(0);
      for(int it=0; it<(int)newvrt.selTrk.size(); it++)fitTrackList.push_back( AllTrackList[newvrt.selTrk[it]] );
      m_fitSvc->setApproximateVertex( 0.5*(v1.vertex[0]+v2.vertex[0]),
                                      0.5*(v1.vertex[1]+v2.vertex[1]),
                                      0.5*(v1.vertex[2]+v2.vertex[2]),
                                      istate);
      m_fitSvc->setRobustness(robKey,istate);
      
      StatusCode sc=m_fitSvc->VKalVrtFit(fitTrackList,neutralPartDummy,
                                   newvrt.vertex, newvrt.vertexMom, newvrt.vertexCharge, newvrt.vertexCov,
                                   newvrt.chi2PerTrk,  newvrt.trkAtVrt, newvrt.chi2,
                                   istate, false);
      if( sc.isFailure() )             return -1.;  
      if( newvrt.chi2>500. )           return -1.;  //VK protection
      if( newvrt.chi2PerTrk.size()==2) newvrt.chi2PerTrk[0]=newvrt.chi2PerTrk[1]=newvrt.chi2/2.;

      m_fitSvc->setRobustness(0,istate); //restore default behaviour

      return TMath::Prob( newvrt.chi2, 2*newvrt.selTrk.size()-3);
   }

//
//  Iterate track removal until vertex get good Chi2
//

   double  NewVrtSecInclusiveTool::improveVertexChi2( WrkVrt &vertex, std::vector<const xAOD::TrackParticle*> & allTrackList,
                                                    Trk::IVKalState& istate,
                                                    bool ifCovV0)
   const
   {

      int nTrk=vertex.selTrk.size();
      if( nTrk<2 )return 0.;
      double prob=TMath::Prob( vertex.chi2, 2*nTrk-3);
      //
      //----Start track removal iterations
      while(prob<0.01){
        nTrk=vertex.selTrk.size();
        if(nTrk==2)return prob;
        int selT=0;
        for(int i=0; i<nTrk; i++){ if( vertex.chi2PerTrk[i]>vertex.chi2PerTrk[selT]) selT=i; }
        vertex.detachedTrack=vertex.selTrk[selT];
        vertex.selTrk.erase( vertex.selTrk.begin() + selT ); //remove track
        prob = refitVertex( vertex, allTrackList, istate, ifCovV0);
        if(prob<0.)return 0.;
      }
      return prob;
   }



   double NewVrtSecInclusiveTool::refitVertex(WrkVrt &vrt,
                                            std::vector<const xAOD::TrackParticle*> & selectedTracks,
                                            Trk::IVKalState& istate,
                                            bool ifCovV0) const
   {
      int i,j;
      int nth = vrt.selTrk.size();
 
      if(nth<2) return -1.;
 
      std::vector<const xAOD::NeutralParticle*> neutralPartDummy(0);
      std::vector<const xAOD::TrackParticle*>  listTracks(0);
      for(i=0;i<nth;i++) {
          j=vrt.selTrk[i];                           /*Track number*/
          listTracks.push_back( selectedTracks[j] );
      }
      vrt.Good = false;
      vrt.chi2PerTrk.resize(nth);
      for(i=0;i<nth;i++)vrt.chi2PerTrk[i]=100000.+i; //VK safety

      m_fitSvc->setApproximateVertex(vrt.vertex[0],vrt.vertex[1],vrt.vertex[2],istate);
      StatusCode sc=m_fitSvc->VKalVrtFit(listTracks,neutralPartDummy,vrt.vertex,vrt.vertexMom,vrt.vertexCharge,
                                         vrt.vertexCov,vrt.chi2PerTrk, vrt.trkAtVrt,vrt.chi2,
                                         istate, ifCovV0);
      if(sc.isSuccess())vrt.Good = true;
      else {vrt.Good = false; return -1.;}
      if(vrt.chi2PerTrk.size()==2) vrt.chi2PerTrk[0]=vrt.chi2PerTrk[1]=vrt.chi2/2.;
      return TMath::Prob( vrt.chi2, 2*nth-1);
   }


   bool NewVrtSecInclusiveTool::isPart( std::deque<long int> test, std::deque<long int> base)
   const
   {
      std::deque<long int>::iterator trk=test.begin();
      for(trk=test.begin(); trk!=test.end(); trk++)
         if(std::find(base.begin(), base.end(), (*trk)) == base.end()) return false;  //element not found => test is not part of base
      return true;
   }

   double NewVrtSecInclusiveTool::MomProjDist(const Amg::Vector3D & SecVrt,const xAOD::Vertex &primVrt,const TLorentzVector &Mom)
   const
   {
      Amg::Vector3D vv=SecVrt-primVrt.position();
      return ( vv.x()*Mom.X()+vv.y()*Mom.Y()+vv.z()*Mom.Z() )/ Mom.P();
   }

} //end namespace



