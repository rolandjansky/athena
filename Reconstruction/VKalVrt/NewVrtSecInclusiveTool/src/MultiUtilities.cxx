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
//-----------------------------------------------------------------------------------
//  Detach the worst Chi2 track from the vertex and join it (if possible) with other track 
//    from the vertex into additional 2-track vertices
//
   void NewVrtSecInclusiveTool::DisassembleVertex(std::vector<WrkVrt> *WrkVrtSet, int iv, 
                                                std::vector<const xAOD::TrackParticle*>  AllTracks,
                                                Trk::IVKalState& istate)
   const
   {
      WrkVrt newvrt; newvrt.Good=true;
      int NTrk=(*WrkVrtSet)[iv].selTrk.size(), SelT=-1;
      if(NTrk<3)return;

      std::vector<const xAOD::NeutralParticle*> neutralPartDummy(0);
      std::vector<const xAOD::TrackParticle*>  ListBaseTracks;
      StatusCode sc; sc.setChecked();
//=== To get robust definition of most bad outlier
      m_fitSvc->setRobustness(5, istate);
      sc = refitVertex( WrkVrtSet, iv, AllTracks, istate, false);
      if(sc.isFailure()){ (*WrkVrtSet)[iv].Good=false; return; }
      m_fitSvc->setRobustness(0, istate);
//--------------------------------------------------
      double Chi2Max=0.;
      for(int i=0; i<NTrk; i++){
         if( (*WrkVrtSet)[iv].chi2PerTrk[i]>Chi2Max) { Chi2Max=(*WrkVrtSet)[iv].chi2PerTrk[i];  SelT=i;}
      }	    
      int NVrtCur=WrkVrtSet->size();
      for(int i=0; i<NTrk; i++){
	   if(i==SelT)continue;
           ListBaseTracks.clear();
	   ListBaseTracks.push_back( AllTracks[(*WrkVrtSet)[iv].selTrk[i]] );
	   ListBaseTracks.push_back( AllTracks[(*WrkVrtSet)[iv].selTrk[SelT]] );
           newvrt.selTrk.resize(2); 
	   newvrt.selTrk[0]=(*WrkVrtSet)[iv].selTrk[i]; 
	   newvrt.selTrk[1]=(*WrkVrtSet)[iv].selTrk[SelT];
           sc = m_fitSvc->VKalVrtFitFast(ListBaseTracks,newvrt.vertex,istate);            /* Fast crude estimation*/
           if( sc.isFailure() )  continue;
           if( newvrt.vertex.perp() > m_maxSVRadiusCut )  newvrt.vertex=Amg::Vector3D(0.,0.,0.);
           m_fitSvc->setApproximateVertex(newvrt.vertex[0],newvrt.vertex[1],newvrt.vertex[2],istate);
           sc=m_fitSvc->VKalVrtFit(ListBaseTracks, neutralPartDummy,
                             newvrt.vertex,
                             newvrt.vertexMom,
                             newvrt.vertexCharge,
                             newvrt.vertexCov,
                             newvrt.chi2PerTrk, 
                             newvrt.trkAtVrt,
                             newvrt.chi2,
                             istate, false);
           if( sc.isFailure() )  continue;  
           if( newvrt.chi2>10.)  continue;  // Too bad 2-track vertex fit
           newvrt.chi2PerTrk[0]=newvrt.chi2PerTrk[1]=newvrt.chi2/2.;
           newvrt.nCloseVrt    = 0;
           newvrt.dCloseVrt    = 1000000.;
           newvrt.projectedVrt = 0.9999;
           if((int)WrkVrtSet->size()==NVrtCur) { WrkVrtSet->push_back(newvrt); continue;}  // just the first added vertex
           if( (*WrkVrtSet).at(NVrtCur).chi2<newvrt.chi2 ) continue;  // previously added 2tr vertex was better
           WrkVrtSet->pop_back();
           WrkVrtSet->push_back(newvrt);
      }
      (*WrkVrtSet)[iv].selTrk.erase( (*WrkVrtSet)[iv].selTrk.begin() + SelT ); //remove track
      sc = refitVertex( WrkVrtSet, iv, AllTracks, istate, false);
      if( sc.isFailure() ) {(*WrkVrtSet)[iv].Good=false; /*std::cout<<" Wrong vertex"<<'\n';*/}
   }


   void NewVrtSecInclusiveTool::Clean1TrVertexSet(std::vector<WrkVrt> *WrkVrtSet)
   const
   {
     std::vector<int> countVT(WrkVrtSet->size(),0);
     std::vector<int> linkedVrt(WrkVrtSet->size(),0);
//--- Mark as bad the 1track vertex if the detached track is NOT present in any remaining good vertex (>=2tr)    
     for(int i1tv=0; i1tv<(int)WrkVrtSet->size(); i1tv++) {
       if( (*WrkVrtSet)[i1tv].selTrk.size()!=1) continue;
       if(!(*WrkVrtSet)[i1tv].Good)             continue;   
       int Trk1=(*WrkVrtSet)[i1tv].detachedTrack; 
       int foundInGoodVrt=0;
       for(int mtv=0; mtv<(int)WrkVrtSet->size(); mtv++) {                        //cycle over good vertices with many tracks
         if( (*WrkVrtSet)[mtv].selTrk.size()<2) continue;
	 if(!(*WrkVrtSet)[mtv].Good)            continue;   
         if( std::find((*WrkVrtSet)[mtv].selTrk.begin(),(*WrkVrtSet)[mtv].selTrk.end(), Trk1) != (*WrkVrtSet)[mtv].selTrk.end()){
           //double m2Vrt=((*WrkVrtSet)[mtv].vertexMom+(*WrkVrtSet)[i1tv].vertexMom).M(); //VK Commented.  M cut in other places
           //if(m2Vrt>m_VrtMassLimit){ (*WrkVrtSet)[i1tv].Good=false;  break; } //1Tr + manyTr system is too heavy
	   foundInGoodVrt++; countVT[mtv]++; linkedVrt[i1tv]=mtv;  //Linked vertex found
         }
       }
       if(!foundInGoodVrt)(*WrkVrtSet)[i1tv].Good=false;             // Make the vertex bad
     }
//
//---Select SINGLE 1tr-vertex from many pointing to one multi-track vertex
     for(int mtv=0; mtv<(int)WrkVrtSet->size(); mtv++) {
       if( (*WrkVrtSet)[mtv].selTrk.size()<2 ) continue;
       if(!(*WrkVrtSet)[mtv].Good )            continue;   
       if(      countVT[mtv] < 1 )             continue;
       double distM=1.e9;
       int    best1TVrt=-1;
       for(int i1tv=0; i1tv<(int)WrkVrtSet->size(); i1tv++) {
         if( (*WrkVrtSet)[i1tv].selTrk.size()!=1 ) continue;
         if(!(*WrkVrtSet)[i1tv].Good )             continue;   
	 if( linkedVrt[i1tv] != mtv )              continue;
         //double dist=((*WrkVrtSet)[mtv].vertex - (*WrkVrtSet)[i1tv].vertex).mag();
         double dist=((*WrkVrtSet)[mtv].vertexMom+(*WrkVrtSet)[i1tv].vertexMom).M(); //Use 
         if( dist < distM ) {distM=dist; best1TVrt=i1tv;}
         (*WrkVrtSet)[i1tv].Good=false;   
       }
       if(best1TVrt>-1 && distM<m_VrtMassLimit)(*WrkVrtSet)[best1TVrt].Good=true;
     }
   }
   
   
   void NewVrtSecInclusiveTool::trackClassification(std::vector<WrkVrt> *WrkVrtSet, 
                                             std::vector< std::deque<long int> > *TrkInVrt)
   const
   { 
      int NSet=WrkVrtSet->size(); 
      for(int iv=0; iv<NSet; iv++) {
         if(!(*WrkVrtSet)[iv].Good) continue;
         int NTrkAtVrt=(*WrkVrtSet)[iv].selTrk.size();
         if(NTrkAtVrt<2) continue;
         for(int jt=0; jt<NTrkAtVrt; jt++){
           int tracknum=(*WrkVrtSet)[iv].selTrk[jt];
	   (*TrkInVrt).at(tracknum).push_back(iv);
         }
      }
   }


   double NewVrtSecInclusiveTool::maxOfShared(std::vector<WrkVrt> *WrkVrtSet, 
                                       std::vector< std::deque<long int> > *TrkInVrt,
				       long int & SelectedTrack,
				       long int & SelectedVertex)
   const
   {
      long int NTrack=TrkInVrt->size(); 
      int it, jv, itmp, NVrt, VertexNumber;
 
      double MaxOf=-999999999, Chi2Red=0., SelectedProb=1.;
//
      int NShareMax=0;
      for( it=0; it<NTrack; it++) {
         NVrt=(*TrkInVrt)[it].size();         /*Number of vertices for this track*/
         if(  NVrt > NShareMax ) NShareMax=NVrt;
      }
      if(NShareMax<=1)return MaxOf;              /* No shared tracks */
//      
      for( it=0; it<NTrack; it++) {
         NVrt=(*TrkInVrt)[it].size();         /*Number of vertices for this track*/
         if(  NVrt <= 1 )        continue;    /*Should ALWAYS be checked for safety*/
         if(  NVrt < NShareMax ) continue;    /*Not a shared track with maximal sharing*/
         double maxRadius=0;
         for(auto &vrtn :(*TrkInVrt)[it] ){ maxRadius=std::max((*WrkVrtSet).at(vrtn).vertex.perp(),maxRadius); } //count number of 2-track vertices
         for( jv=0; jv<NVrt; jv++ ){
	    VertexNumber=(*TrkInVrt)[it][jv];
	    if(!(*WrkVrtSet).at(VertexNumber).Good)continue;
	    int NTrkInVrt=(*WrkVrtSet)[VertexNumber].selTrk.size();
	    if( NTrkInVrt <= 1) continue;                                // one track vertex - nothing to do
            ////if( N2trVrt>0 && N2trVrt<NShareMax && NTrkInVrt>2) continue; // Mixture of multi-track and 2-track vrts. Skip multi-track then.??? Bad!!!
	    for( itmp=0; itmp<NTrkInVrt; itmp++){
	       if( (*WrkVrtSet)[VertexNumber].selTrk[itmp] == it ) {         /* Track found*/        
	          //Chi2Red=(*WrkVrtSet)[VertexNumber].chi2PerTrk.at(itmp);            //   Normal Chi2 
	          Chi2Red=(*WrkVrtSet)[VertexNumber].chi2PerTrk.at(itmp)*(*WrkVrtSet)[VertexNumber].vertex.perp()/maxRadius; // Works much better
                  double prob_vrt = TMath::Prob( (*WrkVrtSet)[VertexNumber].chi2, 2*(*WrkVrtSet)[VertexNumber].selTrk.size()-3);
                  if( MaxOf < Chi2Red ){
		      if(MaxOf>0 && prob_vrt>0.01 && SelectedProb<0.01 ) continue; // Don't disassemble good vertices if a bad one is present
		      MaxOf = Chi2Red;
		      SelectedTrack=it; SelectedVertex=VertexNumber;
                      SelectedProb = prob_vrt;
		  }
               }
            }
	 }
      }
//-- Additional check for a common track in 2tr-2tr/Ntr vertex topology
      if( (*TrkInVrt)[SelectedTrack].size() == 2){
          int v1=(*TrkInVrt)[SelectedTrack][0]; int v2=(*TrkInVrt)[SelectedTrack][1];
          double prb1=TMath::Prob( (*WrkVrtSet)[v1].chi2, 2*(*WrkVrtSet)[v1].selTrk.size()-3),
                 prb2=TMath::Prob( (*WrkVrtSet)[v2].chi2, 2*(*WrkVrtSet)[v2].selTrk.size()-3);
	  double dst1=(*WrkVrtSet)[v1].projectedVrt, dst2=(*WrkVrtSet)[v2].projectedVrt; 
          if(prb1>0.05 && prb2>0.05){
            if( (*WrkVrtSet)[v1].selTrk.size()==2 && (*WrkVrtSet)[v2].selTrk.size()==2){
              if     (SelectedVertex==v1 && dst2<dst1)  SelectedVertex=v2;  // Swap to remove the closest to PV vertex
              else if(SelectedVertex==v2 && dst1<dst2)  SelectedVertex=v1;  // Swap to remove the closest to PV vertex
              double M1=(*WrkVrtSet)[v1].vertexMom.M();  double M2=(*WrkVrtSet)[v2].vertexMom.M();
              if( M1>m_VrtMassLimit && M2<m_VrtMassLimit ) SelectedVertex=v1;
              if( M1<m_VrtMassLimit && M2>m_VrtMassLimit ) SelectedVertex=v2;
            }
            if( (*WrkVrtSet)[v1].selTrk.size()+(*WrkVrtSet)[v2].selTrk.size() > 4){
	       if( (*WrkVrtSet)[v1].selTrk.size()==2 && dst1>dst2) SelectedVertex=v2;
	       if( (*WrkVrtSet)[v2].selTrk.size()==2 && dst2>dst1) SelectedVertex=v1;
          } }	
      }
//
      return MaxOf;
   }


   void NewVrtSecInclusiveTool::removeTrackFromVertex(std::vector<WrkVrt> *WrkVrtSet, 
                                       std::vector< std::deque<long int> > *TrkInVrt,
				       long int selectedTrack,
				       long int selectedVertex)
   const
   {   
        int posInVrtFit=0;                    //Position of SelectedTrack in vertex fit track list.
	std::deque<long int>::iterator it;
        WrkVrt & CVRT=(*WrkVrtSet).at(selectedVertex);
//std::cout<<" In Found ="<<selectedTrack<<", "<<selectedVertex<<'\n';
	for(it=CVRT.selTrk.begin(); 
	    it!=CVRT.selTrk.end();     it++) {
	    if( (*it) == selectedTrack ) { 
	       CVRT.selTrk.erase(it); break;
	    }     
            posInVrtFit++;
	}   

	for(it=(*TrkInVrt).at(selectedTrack).begin(); 
	    it!=(*TrkInVrt)[selectedTrack].end();     it++) {
	    if( (*it) == selectedVertex ) { 
	       (*TrkInVrt)[selectedTrack].erase(it); break;
	    }     
	}   
        (*WrkVrtSet)[selectedVertex].detachedTrack=selectedTrack;
//Check if track is removed from 2tr vertex => then sharing of track left should also be decreased
        if( CVRT.selTrk.size() == 1){
	   long int LeftTrack=CVRT.selTrk[0];  // track left in 1tr vertex
	   for(it=(*TrkInVrt).at(LeftTrack).begin();  it!=(*TrkInVrt)[LeftTrack].end();  it++) {
	      if( (*it) == selectedVertex ) { 
	       (*TrkInVrt)[LeftTrack].erase(it); break;
	      }     
	   }   
           if( TMath::Prob( CVRT.chi2, 1) < 0.05 ) CVRT.Good=false; // Not really good Chi2 for one-track vertex
	   if( CVRT.vertexMom.M()>m_VrtMassLimit)CVRT.Good=false;   // Vertex is too heavy
           int ipos=0; if(posInVrtFit==0)ipos=1;                    // Position of remaining track in previous 2track vertex fit
	   CVRT.vertexMom=MomAtVrt(CVRT.trkAtVrt[ipos]);            //Redefine vertexMom using remaining track
	   if((*TrkInVrt)[LeftTrack].size()>0)CVRT.Good=false;      //Vertex is declared false only if remaining track 
	    						            // is still linked to another vertex
           if(CVRT.vertexMom.Pt()<2.*m_cutPt) CVRT.Good=false;      //1track vertex should have twice momentum of track pt cut
	   CVRT.trkAtVrt.erase((*WrkVrtSet)[selectedVertex].trkAtVrt.begin()+posInVrtFit);  //Remove also TrkAtVrt
        }

   }
//
//  Number of common tracks for 2 vertices
//
   int NewVrtSecInclusiveTool::nTrkCommon( std::vector<WrkVrt> *WrkVrtSet, 
                                         int V1, int V2)
   const
   {
      int NTrk_V1 = (*WrkVrtSet).at(V1).selTrk.size(); if( NTrk_V1< 2) return 0;   /* Bad vertex */
      int NTrk_V2 = (*WrkVrtSet).at(V2).selTrk.size(); if( NTrk_V2< 2) return 0;   /* Bad vertex */
      int nTrkCom=0;
      if(NTrk_V1 < NTrk_V2){
        for(int i=0; i<NTrk_V1; i++){
          int trk=(*WrkVrtSet)[V1].selTrk[i];
          if( std::find((*WrkVrtSet)[V2].selTrk.begin(),(*WrkVrtSet)[V2].selTrk.end(),trk) != (*WrkVrtSet)[V2].selTrk.end()) nTrkCom++;
        }
      }else{
        for(int i=0; i<NTrk_V2; i++){
          int trk=(*WrkVrtSet)[V2].selTrk[i];
          if( std::find((*WrkVrtSet)[V1].selTrk.begin(),(*WrkVrtSet)[V1].selTrk.end(),trk) != (*WrkVrtSet)[V1].selTrk.end()) nTrkCom++;
        }
      }
      return nTrkCom;
   }
//
//  Minimal normalized vertex-vertex distance
//
   double NewVrtSecInclusiveTool::minVrtVrtDist( std::vector<WrkVrt> *WrkVrtSet, int & V1, int & V2)
   const
   {  
     V1=V2=0;
     for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) { (*WrkVrtSet).at(iv).dCloseVrt=1000000.; (*WrkVrtSet)[iv].nCloseVrt=-1;}
     double foundMinVrtDst=1000000.;

     for(int iv=0; iv<(int)WrkVrtSet->size()-1; iv++) {
        if( (*WrkVrtSet)[iv].selTrk.size()< 2)    continue;   /* Bad vertices */
        if(!(*WrkVrtSet)[iv].Good )               continue;   /* Bad vertices */
        for(int jv=iv+1; jv<(int)WrkVrtSet->size(); jv++) {
           if( (*WrkVrtSet)[jv].selTrk.size()< 2) continue;   /* Bad vertices */
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
   double NewVrtSecInclusiveTool::minVrtVrtDistNext( std::vector<WrkVrt> *WrkVrtSet, int & V1, int & V2)
   const
   {  
     V1=0; V2=0;
     double foundMinVrtDst=1000000.;
     for(int iv=0; iv<(int)WrkVrtSet->size()-1; iv++) {
        if( (*WrkVrtSet)[iv].selTrk.size()< 2) continue;   /* Bad vertex */
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
   double  NewVrtSecInclusiveTool::mergeAndRefitVertices( std::vector<WrkVrt> *WrkVrtSet, int V1, int V2, WrkVrt & newvrt,
                                                        std::vector<const xAOD::TrackParticle*> & AllTrackList,
                                                        Trk::IVKalState& istate)
   const
   {
      if(!(*WrkVrtSet).at(V1).Good)return -1.;         //bad vertex
      if(!(*WrkVrtSet).at(V2).Good)return -1.;         //bad vertex
      std::vector<const xAOD::NeutralParticle*> neutralPartDummy(0);
      newvrt.Good=true;
      int NTrk_V1=(*WrkVrtSet)[V1].selTrk.size();
      int NTrk_V2=(*WrkVrtSet)[V2].selTrk.size();
      newvrt.selTrk.resize(NTrk_V1+NTrk_V2);
      std::copy((*WrkVrtSet)[V1].selTrk.begin(),(*WrkVrtSet)[V1].selTrk.end(), newvrt.selTrk.begin());
      std::copy((*WrkVrtSet)[V2].selTrk.begin(),(*WrkVrtSet)[V2].selTrk.end(), newvrt.selTrk.begin()+NTrk_V1);

      std::deque<long int>::iterator   TransfEnd ;
      sort( newvrt.selTrk.begin(), newvrt.selTrk.end() );
      TransfEnd =  unique( newvrt.selTrk.begin(), newvrt.selTrk.end() );
      newvrt.selTrk.erase( TransfEnd, newvrt.selTrk.end());
      std::vector<const xAOD::TrackParticle*>  fitTrackList(0);
      for(int it=0; it<(int)newvrt.selTrk.size(); it++)fitTrackList.push_back( AllTrackList[newvrt.selTrk[it]] );
      m_fitSvc->setApproximateVertex( 0.5*((*WrkVrtSet)[V1].vertex[0]+(*WrkVrtSet)[V2].vertex[0]),
                                      0.5*((*WrkVrtSet)[V1].vertex[1]+(*WrkVrtSet)[V2].vertex[1]),
                                      0.5*((*WrkVrtSet)[V1].vertex[2]+(*WrkVrtSet)[V2].vertex[2]),
                                      istate);
      
      StatusCode sc=m_fitSvc->VKalVrtFit(fitTrackList,neutralPartDummy,
                                   newvrt.vertex, newvrt.vertexMom, newvrt.vertexCharge, newvrt.vertexCov,
                                   newvrt.chi2PerTrk,  newvrt.trkAtVrt, newvrt.chi2,
                                   istate, false);
      if( sc.isFailure() )             return -1.;  
      if( newvrt.chi2>500. )           return -1.;  //VK protection
      if( newvrt.chi2PerTrk.size()==2) newvrt.chi2PerTrk[0]=newvrt.chi2PerTrk[1]=newvrt.chi2/2.;
      return TMath::Prob( newvrt.chi2, 2*newvrt.selTrk.size()-3);
   }
   //
   //
   //================== Intelligent merge of multitrack vertices with 2 and more common tracks
   void  NewVrtSecInclusiveTool::mergeAndRefitOverlapVertices( std::vector<WrkVrt> *WrkVrtSet, int V1, int V2,
                                                             std::vector<const xAOD::TrackParticle*> & AllTrackList,
                                                             Trk::IVKalState& istate) const
   {
      if(!(*WrkVrtSet).at(V1).Good)return;         //bad vertex
      if(!(*WrkVrtSet).at(V2).Good)return;         //bad vertex
      std::vector<const xAOD::NeutralParticle*> neutralPartDummy(0);
      WrkVrt newvrt;
      newvrt.Good=true;
      if( nTrkCommon( WrkVrtSet, V1, V2)<2 )return;       //No overlap
      //- V1 should become ref. vertex. Another Vrt tracks will be added to it. 
      if(      (*WrkVrtSet)[V1].selTrk.size() <  (*WrkVrtSet)[V2].selTrk.size() )
                                                                           {int itmp=V2; V2=V1; V1=itmp;}   //Vertex with NTrk=max is chosen
      else if( (*WrkVrtSet)[V1].selTrk.size() == (*WrkVrtSet)[V2].selTrk.size() )
         { if( (*WrkVrtSet)[V1].chi2           > (*WrkVrtSet)[V2].chi2 )   {int itmp=V2; V2=V1; V1=itmp;} } // Vertex with minimal Chi2 is chosen
      //- Fill base track list for new vertex
      newvrt.selTrk.resize( (*WrkVrtSet)[V1].selTrk.size() );
      std::copy((*WrkVrtSet)[V1].selTrk.begin(),(*WrkVrtSet)[V1].selTrk.end(), newvrt.selTrk.begin());
      //- Identify non-common tracks in second vertex
      std::vector<int> noncommonTrk(0);
      for(auto &it : (*WrkVrtSet)[V2].selTrk){
        if( std::find((*WrkVrtSet)[V1].selTrk.begin(), (*WrkVrtSet)[V1].selTrk.end(), it) == (*WrkVrtSet)[V1].selTrk.end() )
           noncommonTrk.push_back(it);
      }
      //      
      // Try to add non-common tracks one by one
      std::vector<const xAOD::TrackParticle*>  fitTrackList(0);
      std::vector<int> detachedTrk(0);
      StatusCode sc; sc.setChecked();
      WrkVrt bestVrt;
      bool foundMerged=false;
      for( auto nct : noncommonTrk){  
         fitTrackList.clear();
         for(int it=0; it<(int)newvrt.selTrk.size(); it++)fitTrackList.push_back( AllTrackList[newvrt.selTrk[it]] );
         fitTrackList.push_back( AllTrackList.at(nct) );
         m_fitSvc->setApproximateVertex( (*WrkVrtSet)[V1].vertex[0],(*WrkVrtSet)[V1].vertex[1],(*WrkVrtSet)[V1].vertex[2],istate);
         sc=m_fitSvc->VKalVrtFit(fitTrackList, neutralPartDummy, newvrt.vertex, newvrt.vertexMom, newvrt.vertexCharge, newvrt.vertexCov,
                                 newvrt.chi2PerTrk, newvrt.trkAtVrt, newvrt.chi2,
                                 istate, false);
         if( sc.isFailure() || TMath::Prob( newvrt.chi2, 2*newvrt.selTrk.size()+2-3)<m_globVrtProbCut ) {
           detachedTrk.push_back(nct);
           continue;
         }
         newvrt.selTrk.push_back(nct);   // Compatible track. Add to common vertex.
         bestVrt=newvrt;
         foundMerged=true;
      }
      if(foundMerged)(*WrkVrtSet)[V1]=bestVrt;
      (*WrkVrtSet)[V2].Good=false;
      //
      // Now detached tracks
      if(detachedTrk.size()>1){
         WrkVrt nVrt;
         fitTrackList.clear(); nVrt.selTrk.clear();
         for(auto nct : detachedTrk){ fitTrackList.push_back( AllTrackList[nct] );  nVrt.selTrk.push_back(nct); }
         m_fitSvc->setApproximateVertex( (*WrkVrtSet)[V2].vertex[0],(*WrkVrtSet)[V2].vertex[1],(*WrkVrtSet)[V2].vertex[2],istate);
         sc=m_fitSvc->VKalVrtFit(fitTrackList, neutralPartDummy, nVrt.vertex, nVrt.vertexMom, nVrt.vertexCharge, nVrt.vertexCov,
                                 nVrt.chi2PerTrk, nVrt.trkAtVrt, nVrt.chi2,
                                 istate, false);
         if(sc.isSuccess()) (*WrkVrtSet).push_back(nVrt);
      } else if( detachedTrk.size()==1 ){
         bool tFound=false;
         for( auto &vrt : (*WrkVrtSet) ){  
           if( !vrt.Good || vrt.selTrk.size()<2 )continue;
           if( std::find(vrt.selTrk.begin(), vrt.selTrk.end(), detachedTrk[0]) != vrt.selTrk.end() ) { tFound=true; break; }
         }
         if( !tFound ) {   //Track is not present in other vertices. 
           double Chi2min=1.e9; int selectedTrk=-1;
           WrkVrt saveVrt;
           fitTrackList.resize(2);
           fitTrackList[0]= AllTrackList[detachedTrk[0]];
           for(auto trk : (*WrkVrtSet)[V2].selTrk){
              if(trk==detachedTrk[0])continue;
              WrkVrt nVrt; nVrt.Good=true;
              fitTrackList[1]=AllTrackList[trk];
              m_fitSvc->setApproximateVertex( (*WrkVrtSet)[V2].vertex[0],(*WrkVrtSet)[V2].vertex[1],(*WrkVrtSet)[V2].vertex[2],istate);
              sc=m_fitSvc->VKalVrtFit(fitTrackList, neutralPartDummy, nVrt.vertex, nVrt.vertexMom, nVrt.vertexCharge, nVrt.vertexCov,
                                      nVrt.chi2PerTrk, nVrt.trkAtVrt, nVrt.chi2,
                                      istate, false);
              if(sc.isSuccess() &&   nVrt.chi2<Chi2min) {Chi2min=nVrt.chi2;  saveVrt=nVrt; selectedTrk=trk; }
           }    
	   if(Chi2min<1.e9){
             saveVrt.selTrk.resize(1); saveVrt.selTrk[0]=detachedTrk[0];
             saveVrt.detachedTrack=selectedTrk;
             saveVrt.vertexMom=MomAtVrt(saveVrt.trkAtVrt[0]);  //redefine vertex momentum
             (*WrkVrtSet).push_back(saveVrt);
           }
         }
      }
      return ;
   }

//
//  Iterate track removal until vertex get good Chi2
//

   double  NewVrtSecInclusiveTool::improveVertexChi2( std::vector<WrkVrt> *WrkVrtSet, int V, std::vector<const xAOD::TrackParticle*> & AllTrackList,
                                                    Trk::IVKalState& istate,
                                                    bool ifCovV0)
   const
   {

      int NTrk=(*WrkVrtSet)[V].selTrk.size();
      if( NTrk<2 )return 0.;
      double Prob=TMath::Prob( (*WrkVrtSet)[V].chi2, 2*NTrk-3);
      //
      //----Start track removal iterations
      while(Prob<0.01){
        NTrk=(*WrkVrtSet)[V].selTrk.size();
        if(NTrk==2)return Prob;
        int SelT=-1; double Chi2Max=0.;
        for(int i=0; i<NTrk; i++){
          if( (*WrkVrtSet)[V].chi2PerTrk[i]>Chi2Max) { 
            Chi2Max=(*WrkVrtSet)[V].chi2PerTrk[i];  
            SelT=i;
          }
        }
        if (SelT<0) return 0; 
        (*WrkVrtSet)[V].detachedTrack=(*WrkVrtSet)[V].selTrk[SelT];
        (*WrkVrtSet)[V].selTrk.erase( (*WrkVrtSet)[V].selTrk.begin() + SelT ); //remove track
        StatusCode sc = refitVertex( WrkVrtSet, V, AllTrackList, istate, ifCovV0);
        if(sc.isFailure())return 0.;
        Prob=TMath::Prob( (*WrkVrtSet)[V].chi2, 2*(NTrk-1)-3);
      }
      return Prob;
   }




   StatusCode NewVrtSecInclusiveTool::refitVertex( std::vector<WrkVrt> *WrkVrtSet,
                                                 int SelectedVertex,
                                                 std::vector<const xAOD::TrackParticle*> & selectedTracks,
                                                 Trk::IVKalState& istate,
                                                 bool ifCovV0) const
   {
      int nth = (*WrkVrtSet)[SelectedVertex].selTrk.size();
 
      if(nth<2) return StatusCode::SUCCESS;
 
      std::vector<const xAOD::NeutralParticle*> neutralPartDummy(0);
      std::vector<const xAOD::TrackParticle*>  ListTracks(0);
      for(int i=0;i<nth;i++) {
	  int j=(*WrkVrtSet)[SelectedVertex].selTrk[i];                           /*Track number*/
          ListTracks.push_back( selectedTracks[j] );
      }
      (*WrkVrtSet)[SelectedVertex].Good = false;
      (*WrkVrtSet)[SelectedVertex].chi2PerTrk.resize(nth);
      for(int i=0;i<nth;i++)(*WrkVrtSet)[SelectedVertex].chi2PerTrk[i]=100000.+i; //VK safety

      m_fitSvc->setApproximateVertex((*WrkVrtSet)[SelectedVertex].vertex[0],
	                             (*WrkVrtSet)[SelectedVertex].vertex[1],
	                             (*WrkVrtSet)[SelectedVertex].vertex[2],
                                     istate);
      StatusCode sc=m_fitSvc->VKalVrtFit(ListTracks, neutralPartDummy,
                                (*WrkVrtSet)[SelectedVertex].vertex,
                                (*WrkVrtSet)[SelectedVertex].vertexMom,
				(*WrkVrtSet)[SelectedVertex].vertexCharge,
		                (*WrkVrtSet)[SelectedVertex].vertexCov,
				(*WrkVrtSet)[SelectedVertex].chi2PerTrk, 
				(*WrkVrtSet)[SelectedVertex].trkAtVrt,
				(*WrkVrtSet)[SelectedVertex].chi2,
                                 istate, ifCovV0);
      if(sc.isSuccess())(*WrkVrtSet)[SelectedVertex].Good = true;
      if((*WrkVrtSet)[SelectedVertex].chi2PerTrk.size()==2) 
         (*WrkVrtSet)[SelectedVertex].chi2PerTrk[0]=
	 (*WrkVrtSet)[SelectedVertex].chi2PerTrk[1]=(*WrkVrtSet)[SelectedVertex].chi2/2.;
      return sc;
   }



   double NewVrtSecInclusiveTool::refitVertex(WrkVrt &Vrt,
                                            std::vector<const xAOD::TrackParticle*> & selectedTracks,
                                            Trk::IVKalState& istate,
                                            bool ifCovV0) const
   {
      int i,j;
      int nth = Vrt.selTrk.size();
 
      if(nth<2) return -1.;
 
      std::vector<const xAOD::NeutralParticle*> neutralPartDummy(0);
      std::vector<const xAOD::TrackParticle*>  ListTracks(0);
      for(i=0;i<nth;i++) {
	  j=Vrt.selTrk[i];                           /*Track number*/
          ListTracks.push_back( selectedTracks[j] );
      }
      Vrt.Good = false;
      Vrt.chi2PerTrk.resize(nth);
      for(i=0;i<nth;i++)Vrt.chi2PerTrk[i]=100000.+i; //VK safety

      m_fitSvc->setApproximateVertex(Vrt.vertex[0],Vrt.vertex[1],Vrt.vertex[2],istate);
      StatusCode sc=m_fitSvc->VKalVrtFit(ListTracks,neutralPartDummy,Vrt.vertex,Vrt.vertexMom,Vrt.vertexCharge,
                                         Vrt.vertexCov,Vrt.chi2PerTrk, Vrt.trkAtVrt,Vrt.chi2,
                                         istate, ifCovV0);
      if(sc.isSuccess())Vrt.Good = true;
      else {Vrt.Good = false; return -1.;}
      if(Vrt.chi2PerTrk.size()==2) Vrt.chi2PerTrk[0]=Vrt.chi2PerTrk[1]=Vrt.chi2/2.;
      return TMath::Prob( Vrt.chi2, 2*nth-1);
   }


   bool NewVrtSecInclusiveTool::isPart( std::deque<long int> test, std::deque<long int> base)
   const
   {
      std::deque<long int>::iterator trk=test.begin();
      for(trk=test.begin(); trk!=test.end(); trk++)
         if(std::find(base.begin(), base.end(), (*trk)) == base.end()) return false;  //element not found => test is not part of base
      return true;
   }

   double NewVrtSecInclusiveTool::MomProjDist(const Amg::Vector3D & SecVrt,const xAOD::Vertex &PrimVrt,const TLorentzVector &Mom)
   const
   {
      Amg::Vector3D vv=SecVrt-PrimVrt.position();
      return ( vv.x()*Mom.X()+vv.y()*Mom.Y()+vv.z()*Mom.Z() )/ Mom.P();
   }

} //end namespace



