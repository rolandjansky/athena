/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecInclusive/VrtSecInclusive.h"
#include "VrtSecInclusive/IntersectionPos.h"
#include "VrtSecInclusive/NtupleVars.h"

#include "TrkTrackSummary/TrackSummary.h"

#include <iostream>


#include "TH1D.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TROOT.h"
//-------------------------------------------------

using namespace std;

namespace Trk {
  extern void dsinv(long int * , double *, long int , long int *);             
}

namespace VKalVrtAthena {
  
  //____________________________________________________________________________________________________
  const Trk::Perigee* VrtSecInclusive::GetPerigee( const xAOD::TrackParticle* i_ntrk) 
  {
    //
    //
    return &(i_ntrk->perigeeParameters()) ;
  }

  
  //____________________________________________________________________________________________________
  double VrtSecInclusive::VrtVrtDist(const Amg::Vector3D & Vrt1, const vector<double>  & VrtErr1,
                                     const Amg::Vector3D & Vrt2, const vector<double>  & VrtErr2)
  {
    double Signif=0.;
    double distx =  Vrt1.x()- Vrt2.x();
    double disty =  Vrt1.y()- Vrt2.y();
    double distz =  Vrt1.z()- Vrt2.z();

    double mmm[3][3];
 
    mmm[0][0] =             VrtErr1[0]+VrtErr2[0];
    mmm[0][1] = mmm[1][0] = VrtErr1[1]+VrtErr2[1];
    mmm[1][1] =             VrtErr1[2]+VrtErr2[2];
    mmm[0][2] = mmm[2][0] = VrtErr1[3]+VrtErr2[3];
    mmm[1][2] = mmm[2][1] = VrtErr1[4]+VrtErr2[4];
    mmm[2][2] =             VrtErr1[5]+VrtErr2[5];

    long int jfail,NN=3;
    Trk::dsinv( &NN, &mmm[0][0], NN, &jfail);

    if(jfail==0){
      Signif = distx*mmm[0][0]*distx
	+disty*mmm[1][1]*disty
	+distz*mmm[2][2]*distz
	+2.*distx*mmm[0][1]*disty
	+2.*distx*mmm[0][2]*distz
	+2.*disty*mmm[1][2]*distz;
      Signif=sqrt(Signif);
    }

    return Signif;
  }
  
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::DisassembleVertex(std::vector<WrkVrt> *WrkVrtSet, int iv, 
					  const xAOD::TrackParticleContainer*  AllTracks)
  {
    WrkVrt& wrkvrt = WrkVrtSet->at(iv);
    ATH_MSG_VERBOSE(" >> DisassembleVertex(): begin: disassembling vertex[" << iv << "], WrkVrtSet.size() = " << WrkVrtSet->size() );
    ATH_MSG_VERBOSE(" >> DisassembleVertex(): Vertex: r = " << sqrt(wrkvrt.vertex[0]*wrkvrt.vertex[0] + wrkvrt.vertex[1]*wrkvrt.vertex[1]) << ", z = " << wrkvrt.vertex[2] );

    // Loop over the tracks associated to the vertex and slect the maximum chi2 track
    int NTrk=wrkvrt.SelTrk.size();
    int SelT=-1;                      // ID of the selected track
    double Chi2Max=0.;
    
    // If the size of the tracks is less than 2, this algorithm is meaningless.
    if( NTrk<=2 ) return StatusCode::SUCCESS;
    
    // find the track with the maximum chi2
    for(int itrk=0; itrk<NTrk; itrk++){
      ATH_MSG_VERBOSE(" >> DisassembleVertex(): > track at vertex[" << itrk << "]: "
		      << "index = " << AllTracks->at(wrkvrt.SelTrk[itrk])->index()
		      << ", pT = " << AllTracks->at(wrkvrt.SelTrk[itrk])->pt()
		      << ", phi = " << AllTracks->at(wrkvrt.SelTrk[itrk])->phi()
		      << ", d0 = " << AllTracks->at(wrkvrt.SelTrk[itrk])->d0()
		      << ", z0 = " << AllTracks->at(wrkvrt.SelTrk[itrk])->z0());
      if( wrkvrt.Chi2PerTrk[itrk] > Chi2Max) {
	Chi2Max = wrkvrt.Chi2PerTrk[itrk];
	SelT = itrk;
      }
    }
    
    // return if no track is found.
    if(SelT==-1) return StatusCode::SUCCESS;
    
    
    // defind work variables
    vector<const xAOD::NeutralParticle*>  dummyNeutrals;
    
    vector<WrkVrt> new_vertices;
    
    // Loop over the tracks associated to the vertex other than the selected tracks
    ATH_MSG_VERBOSE(" >> DisassembleVertex(): Loop over the tracks associated to the vertex other than the selected tracks.");
    for(int itrk=0; itrk<NTrk; itrk++) {
      
      ATH_MSG_VERBOSE(" >> DisassembleVertex(): > Loop itrk = " << itrk << " / " << NTrk );
      
      // reject the selected track
      if( itrk == SelT ) {
	ATH_MSG_VERBOSE(" >> DisassembleVertex(): > skipped." );
	continue;
      }
      
      const size_t this_trk_id     = wrkvrt.SelTrk[itrk];
      const size_t selected_trk_id = wrkvrt.SelTrk[SelT];
      
      ATH_MSG_VERBOSE(" >> DisassembleVertex(): > this_trk_id  = " << this_trk_id << ", selected_trk_id = " << selected_trk_id << ", alltrks_size = " << AllTracks->size() );
      if( this_trk_id >= AllTracks->size() ) {
	ATH_MSG_VERBOSE(" >> DisassembleVertex(): > this_trk_id is invalid. continue!" );
	continue;
      }
      if( selected_trk_id >= AllTracks->size() ) {
	ATH_MSG_VERBOSE(" >> DisassembleVertex(): > selected_trk_id is invalid. continue!" );
	continue;
      }
      
      ATH_MSG_VERBOSE(" >> DisassembleVertex(): > Storing tracks to ListBaseTracks" );
      ATH_MSG_VERBOSE(" >> DisassembleVertex(): > AllTracks->at( this_trk_id ) = " << AllTracks->at( this_trk_id     )->index() );
      ATH_MSG_VERBOSE(" >> DisassembleVertex(): > AllTracks->at( this_trk_id ) = " << AllTracks->at( selected_trk_id )->index() );
      
      vector<const xAOD::TrackParticle*>    ListBaseTracks;
      ListBaseTracks.emplace_back( AllTracks->at( this_trk_id     ) );
      ListBaseTracks.emplace_back( AllTracks->at( selected_trk_id ) );
	
      ATH_MSG_VERBOSE(" >> DisassembleVertex(): > ListBaseTracks was stored." );
      
      WrkVrt newvrt;
      newvrt.SelTrk.emplace_back( this_trk_id );
      newvrt.SelTrk.emplace_back( selected_trk_id );
      
      // Fit the new vertex
      ATH_MSG_VERBOSE(" >> DisassembleVertex(): > Fast Fit" );
      
      m_fitSvc->setDefault();
      ATH_CHECK( m_fitSvc->VKalVrtFitFast( ListBaseTracks, newvrt.vertex ) );
      
      ATH_MSG_VERBOSE( " >> DisassembleVertex(): > ApproxVertex: r = " << sqrt(newvrt.vertex[0]*newvrt.vertex[0] + newvrt.vertex[1]*newvrt.vertex[1]) << ", z = " << newvrt.vertex[2] );
      
      if( (wrkvrt.vertex[0]-newvrt.vertex[0])*(wrkvrt.vertex[0]-newvrt.vertex[0]) + 
          (wrkvrt.vertex[1]-newvrt.vertex[1])*(wrkvrt.vertex[1]-newvrt.vertex[1]) + 
          (wrkvrt.vertex[2]-newvrt.vertex[2])*(wrkvrt.vertex[2]-newvrt.vertex[2])   > 100. )
        {
          m_fitSvc->setApproximateVertex( wrkvrt.vertex[0], wrkvrt.vertex[1], wrkvrt.vertex[2] );
        }
      else
        {
          m_fitSvc->setApproximateVertex( newvrt.vertex[0], newvrt.vertex[1], newvrt.vertex[2] );
        }
      
      ATH_MSG_VERBOSE(" >> DisassembleVertex(): > Fit the new vertex" );
      StatusCode sc = m_fitSvc->VKalVrtFit(ListBaseTracks,
                                           dummyNeutrals,
                                           newvrt.vertex,
                                           newvrt.vertexMom,
                                           newvrt.Charge,
                                           newvrt.vertexCov,
                                           newvrt.Chi2PerTrk, 
                                           newvrt.TrkAtVrt,
                                           newvrt.Chi2          );
      
      if( sc.isFailure() ) continue;
      
      newvrt.nCloseVrt    = 0;
      newvrt.dCloseVrt    = 1000000.;
      
      // register the new vertex to the vertex list
      ATH_MSG_VERBOSE(" >> DisassembleVertex(): > register the new vertex to the vertex list" );
      new_vertices.emplace_back( newvrt );
    }
    
    // remove the selected track from the original vertex
    wrkvrt.SelTrk.erase( wrkvrt.SelTrk.begin() + SelT ); //remove track
    ATH_MSG_VERBOSE(" >> DisassembleVertex(): removed the selected track from the original vertex. wrkvrt.SelTrk.size = " << wrkvrt.SelTrk.size() );
    
    // refit the original vertex
    ATH_MSG_VERBOSE(" >> DisassembleVertex(): refit the original vertex" );

    //ATH_CHECK( RefitVertex( wrkvrt, AllTracks) ); // temporary workaround from Jordi Duarte Campderros below, for now
    StatusCode sc = RefitVertex( wrkvrt, AllTracks );
    if( sc.isFailure() ) {
        // WARNING CODE ATLASRECTS-3145::001 RefitVertex Failure, vertex lost
        ATH_MSG_WARNING("ATLASRECTS-3145::001" );
        return StatusCode::SUCCESS;
    }
    // end of workaround
    
    for( auto vertex : new_vertices ) {
      ATH_MSG_VERBOSE(" >> DisassembleVertex(): > emplace_back new vertex" );
      WrkVrtSet->emplace_back( vertex );
    }
    
    ATH_MSG_VERBOSE(" >> DisassembleVertex(): end. WrkVrtSet.size() = " << WrkVrtSet->size() );
    return StatusCode::SUCCESS;
  }

  
  
  //____________________________________________________________________________________________________
  double  VrtSecInclusive::improveVertexChi2( std::vector<WrkVrt> *WrkVrtSet, int V, 
					      const xAOD::TrackParticleContainer* AllTrackList)
  {
    //
    //  Iterate track removal until vertex get good Chi2
    //

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
      (*WrkVrtSet)[V].SelTrk.erase( (*WrkVrtSet)[V].SelTrk.begin() + SelT ); //remove track
      StatusCode sc = RefitVertex( (*WrkVrtSet)[V], AllTrackList);
      if(sc.isFailure())return 0.;
      Prob=TMath::Prob( (*WrkVrtSet)[V].Chi2, 2*(NTrk-1)-3);
    }
    return Prob;
  }



  //____________________________________________________________________________________________________
  void VrtSecInclusive::RemoveTrackFromVertex(std::vector<WrkVrt> *WrkVrtSet, 
					      std::vector< std::deque<long int> > *TrkInVrt,
					      long int & SelectedTrack,
					      long int & SelectedVertex)
  {   
    std::deque<long int>::iterator it;
    //std::cout<<" In Found ="<<SelectedTrack<<", "<<SelectedVertex<<'\n';
    for(it=(*WrkVrtSet).at(SelectedVertex).SelTrk.begin(); 
	it!=(*WrkVrtSet)[SelectedVertex].SelTrk.end();     it++) {
      if( (*it) == SelectedTrack ) { 
	(*WrkVrtSet)[SelectedVertex].SelTrk.erase(it);
	break;
      }     
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
    }

  }
  
  
  
  //____________________________________________________________________________________________________
  double VrtSecInclusive::minVrtVrtDist( vector<WrkVrt> *WrkVrtSet, int & V1, int & V2)
  {  
    //
    //  Minimal normalized vertex-vertex distance
    //

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


  //____________________________________________________________________________________________________
  double VrtSecInclusive::minVrtVrtDistNext( vector<WrkVrt> *WrkVrtSet, int & V1, int & V2)
  {  
    //   
    // Give minimal distance between nonmodifed yet vertices
    //   

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

  
  //____________________________________________________________________________________________________
  void VrtSecInclusive::MergeVertices( std::vector<WrkVrt> *WrkVrtSet, int & V1, int & V2)
  {
    //
    //  Merge two close vertices into one (first) and set NTr=0 for second vertex
    //

    int i;
    int nth=(*WrkVrtSet).at(V2).SelTrk.size();   //number of tracks in second vertex

    for(i=0;i<nth;i++) (*WrkVrtSet)[V1].SelTrk.emplace_back(   (*WrkVrtSet)[V2].SelTrk[i]   );
    //
    // Cleaning
    deque<long int>::iterator   TransfEnd ;
    sort( (*WrkVrtSet)[V1].SelTrk.begin(), (*WrkVrtSet)[V1].SelTrk.end() );
    TransfEnd =  unique((*WrkVrtSet)[V1].SelTrk.begin(), (*WrkVrtSet)[V1].SelTrk.end() );
    (*WrkVrtSet)[V1].SelTrk.erase( TransfEnd, (*WrkVrtSet)[V1].SelTrk.end());
    //
    //----------------------------------------------------------
    (*WrkVrtSet)[V2].SelTrk.clear();     //Clean dropped vertex
    (*WrkVrtSet)[V2].dCloseVrt=1000000.; //Clean dropped vertex
    (*WrkVrtSet)[V2].nCloseVrt=0;        //Clean dropped vertex
    (*WrkVrtSet)[V2].Good=false;        //Clean dropped vertex
    (*WrkVrtSet)[V1].dCloseVrt=1000000.; //Clean new vertex
    (*WrkVrtSet)[V1].nCloseVrt=0;        //Clean new vertex
    //(*WrkVrtSet)[V2].Good=true;          //Clean new vertex
    (*WrkVrtSet)[V1].Good=true;          //Clean new vertex

  }
  
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::RefitVertex( WrkVrt& WrkVrt,
					   const xAOD::TrackParticleContainer* selectedTracks)
  {
    //
    int i,j;
    vector<const xAOD::NeutralParticle*> dummyNeutrals;
      
    int nth = WrkVrt.SelTrk.size();
 
    if(nth<2) return StatusCode::SUCCESS;

    i = WrkVrt.Chi2PerTrk.size();
    if( nth > i){
      for( ; i<nth; i++) WrkVrt.Chi2PerTrk.emplace_back(100.);
    }
 
    vector<const xAOD::TrackParticle*>  ListBaseTracks;
    ListBaseTracks.clear();
    for(i=0;i<nth;i++) {
      j=WrkVrt.SelTrk[i];                           /*Track number*/
      ATH_MSG_DEBUG(" >>> RefitVertex: WrkVrt.SelTrk[" << i << "] = " << j );
      ListBaseTracks.emplace_back( selectedTracks->at(j) );
    }
    m_fitSvc->setApproximateVertex(WrkVrt.vertex[0],
				   WrkVrt.vertex[1],
				   WrkVrt.vertex[2]);
    
    ATH_MSG_DEBUG( " >>> RefitVertex: ListBaseTracks.size = " << ListBaseTracks.size() );
    for( auto *trk : ListBaseTracks ) {
      ATH_MSG_DEBUG( " >>> RefitVertex: ListBaseTracks = " << trk->index() );
    }
    
    m_fitSvc->setDefault();
    ATH_MSG_DEBUG( " >>> RefitVertex: m_fitSvc is reset." );
    
    Amg::Vector3D IniVertex;
    
    StatusCode sc = m_fitSvc->VKalVrtFitFast( ListBaseTracks, IniVertex );/* Fast crude estimation */
    if(sc.isFailure()) ATH_MSG_DEBUG(" >>> RefitVertex: fast crude estimation failed.");
    ATH_MSG_DEBUG( " >>> RefitVertex: Fast VKalVrtFit succeeded. vertex = (" << IniVertex.x() << ", " << IniVertex.y() << ", " << IniVertex.z() << ")" );
    
    Amg::Vector3D OrigVertex( WrkVrt.vertex[0], WrkVrt.vertex[1], WrkVrt.vertex[2] );
    
    if(  (IniVertex.x()-OrigVertex.x())*(IniVertex.x()-OrigVertex.x()) +
         (IniVertex.y()-OrigVertex.y())*(IniVertex.y()-OrigVertex.y()) +
         (IniVertex.z()-OrigVertex.z())*(IniVertex.z()-OrigVertex.z())  > 100. ) {
      
      m_fitSvc->setApproximateVertex( OrigVertex.x(), OrigVertex.y(), OrigVertex.z() );
      
    } else {
      
      m_fitSvc->setApproximateVertex( IniVertex.x(), IniVertex.y(), IniVertex.z() );
      
    }
    
    ATH_MSG_DEBUG( " >>> RefitVertex: approx vertex is set. Now going to perform fitting..." );
    
    StatusCode SC=m_fitSvc->VKalVrtFit(ListBaseTracks,dummyNeutrals,
				       WrkVrt.vertex,
				       WrkVrt.vertexMom,
				       WrkVrt.Charge,
				       WrkVrt.vertexCov,
				       WrkVrt.Chi2PerTrk, 
				       WrkVrt.TrkAtVrt,
				       WrkVrt.Chi2); 

    if(SC.isFailure()) ATH_MSG_DEBUG(" >>> RefitVertex: SC in RefitVertex returned failure "); 
    ATH_MSG_VERBOSE(" >>> RefitVertex "<<SC<<", "<<ListBaseTracks.size()<<","<<WrkVrt.Chi2PerTrk.size());

    return SC;
  }
  

  //____________________________________________________________________________________________________
  void  VrtSecInclusive::FillCovMatrix(int iTrk, std::vector<double> & Matrix, AmgSymMatrix(5)& CovMtx )
  {
    // Fills 5x5 matrix.  Input Matrix is a full covariance
    int iTmp=(iTrk+1)*3;
    int NContent = Matrix.size();
    CovMtx(0,0)=0.; CovMtx(1,1)=0.;
    int pnt = (iTmp+1)*iTmp/2 + iTmp;   if( pnt   > NContent ) return;
    CovMtx(2,2) =  Matrix[pnt];
    pnt = (iTmp+1+1)*(iTmp+1)/2 + iTmp; if( pnt+1 > NContent ) return;
    CovMtx(2,3) =  CovMtx(3,2) =Matrix[pnt];
    CovMtx(3,3) =  Matrix[pnt+1];
    pnt = (iTmp+2+1)*(iTmp+2)/2 + iTmp; if( pnt+2 > NContent ) return; 
    CovMtx(2,4) = CovMtx(4,2) =Matrix[pnt];
    CovMtx(3,4) = CovMtx(4,3) =Matrix[pnt+1];
    CovMtx(4,4)               =Matrix[pnt+2];
    return;
  }
  
  
  //____________________________________________________________________________________________________
  int VrtSecInclusive::nTrkCommon( std::vector<WrkVrt> *WrkVrtSet, int V1, int V2)
    const
  {
    //
    //  Number of common tracks for 2 vertices
    //
    
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
  
  
  //____________________________________________________________________________________________________
  void VrtSecInclusive::setMcEventCollection(const xAOD::TruthEventContainer* mcEventCollectionIn) {        
    m_importedFullTruthColl = mcEventCollectionIn;
  }


  //____________________________________________________________________________________________________
  void VrtSecInclusive::setTrackParticleTruthCollection(const xAOD::TruthParticleContainer* trackParticleTruthCollectionIn) {
    m_importedTrkTruthColl = trackParticleTruthCollectionIn;
  }
  
  
  //____________________________________________________________________________________________________
  void VrtSecInclusive::setTrackParticleContainer(const xAOD::TrackParticleContainer* importedTrackCollectionIn){ 
    m_importedTrkColl = importedTrackCollectionIn;
  }
  
  
  //____________________________________________________________________________________________________
  void VrtSecInclusive::declareProperties() {
    declareProperty("GeoModel",                     m_geoModel);
    declareProperty("TrackLocation",                m_TrackLocation);
    declareProperty("PrimVrtLocation",              m_PrimVrtLocation);
    declareProperty("SecVrtLocation",               m_SecVrtLocation);
    declareProperty("BeamPosition",                 m_BeamPosition);

    // default values are set upstream - check top of file

    declareProperty("ImpactWrtBL",                  m_ImpactWrtBL);
    declareProperty("a0TrkPVDstMinCut",             m_a0TrkPVDstMinCut);
    declareProperty("a0TrkPVDstMaxCut",             m_a0TrkPVDstMaxCut);
    declareProperty("zTrkPVDstMinCut",              m_zTrkPVDstMinCut);
    declareProperty("zTrkPVDstMaxCut",              m_zTrkPVDstMaxCut);
    declareProperty("a0TrkPVSignifCut",             m_a0TrkPVSignifCut);
    declareProperty("zTrkPVSignifCut",              m_zTrkPVSignifCut);

    declareProperty("TrkChi2Cut",                   m_TrkChi2Cut);
    declareProperty("SelVrtChi2Cut",                m_SelVrtChi2Cut);
    declareProperty("SelTrkMaxCutoff",              m_SelTrkMaxCutoff);   
    declareProperty("TrkPtCut",                     m_TrkPtCut=1000.);   
    declareProperty("TrkLoPtCut",                   m_LoPtCut=250.);   
    declareProperty("TrkA0ErrCut",                  m_A0TrkErrorCut);
    declareProperty("TrkZErrCut",                   m_ZTrkErrorCut);
    declareProperty("CutSctHits",                   m_CutSctHits  );
    declareProperty("CutPixelHits",                 m_CutPixelHits  );
    declareProperty("CutSiHits",                    m_CutSiHits  );
    declareProperty("DoSAloneTRT",                  m_SAloneTRT=false);
    declareProperty("CutBLayHits",                  m_CutBLayHits  );
    declareProperty("CutSharedHits",                m_CutSharedHits );
    declareProperty("doTRTPixCut",                  m_doTRTPixCut=false );
    declareProperty("CutTRTHits",                   m_CutTRTHits );
    declareProperty("doMergeFinalVerticesDistance", m_mergeFinalVerticesDistance=false );
    declareProperty("VertexMergeFinalDistCut",      m_VertexMergeFinalDistCut=1. );
    declareProperty("VertexMergeCut",               m_VertexMergeCut );
    declareProperty("TrackDetachCut",               m_TrackDetachCut );
    declareProperty("FillHist",                     m_FillHist=false  );
    declareProperty("FillNtuple",                   m_FillNtuple=false  );
    declareProperty("DoIntersectionPos",            m_doIntersectionPos=false );
    declareProperty("DoMapToLocal",                 m_doMapToLocal=false );
    declareProperty("DoTruth",                      m_doTruth=false);
    declareProperty("RemoveFake2TrkVrt",            m_removeFakeVrt=true);
    declareProperty("McParticleContainer",          m_truthParticleContainerName = "TruthParticles");
    declareProperty("MCEventContainer",             m_mcEventContainerName       = "TruthEvents");
    declareProperty("MCTrackResolution",            m_mcTrkResolution=0.06); // see getTruth for explanation
    declareProperty("TruthTrkLen",                  m_TruthTrkLen=1000);
    declareProperty("ExtrapPV",                     m_extrapPV=false); // Leave false. only for testing

    //    declareProperty("TrackSummaryTool",       m_sumSvc, " Public InDet TrackSummaryTool" );
    declareProperty("VertexFitterTool",             m_fitSvc, " Private TrkVKalVrtFitter" );
    declareProperty("Extrapolator",                 m_extrapolator);
    declareProperty("VertexMapper",                 m_vertexMapper);
    
  }
  
  
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::processPrimaryVertices() {
    
    //--------------------------------------------------------
    //  Primary vertex extraction from TES
    //
    
    StatusCode sc = evtStore()->retrieve( m_vertexTES, "PrimaryVertices");
    
    if( sc.isFailure()  ||  !m_vertexTES ) {
      ATH_MSG_WARNING("No xAOD vertex container found in TDS"); 
      return StatusCode::SUCCESS;
    }  
    else {
    }
    
    if( m_FillNtuple ) m_ntupleVars->get<unsigned int>( "NumPV" ) = 0;
    m_thePV = nullptr;
    
    ATH_MSG_DEBUG( "processPrimaryVertices(): pv_size = " << m_vertexTES->size() );
    
    // Loop over PV container and get number of tracks of each PV
    
    for( auto *vertex : *m_vertexTES ) {
      
      // Hide (2015-04-21): xAOD::Vertex may contain several types of vertices
      // e.g. if VertexType==NoVtx, this is a dummy vertex.
      // We only need to extract primary vertices here, and skip otherwise.
      
      if( xAOD::VxType::PriVtx != vertex->vertexType() ) continue;
      
      // Not considering pile-up; pick-up the first PV
      m_thePV = vertex;
      
      if( m_FillNtuple ) {
        
        if( 0 == m_ntupleVars->get<unsigned int>( "NumPV" ) ) {
	
          m_ntupleVars->get<double>( "PVX" ) = vertex->x();
          m_ntupleVars->get<double>( "PVY" ) = vertex->y();
          m_ntupleVars->get<double>( "PVZ" ) = vertex->z();
          m_ntupleVars->get<unsigned int>( "PVType" ) = vertex->vertexType();
	
          // number of tracks associated to the PV
          m_ntupleVars->get<unsigned int>( "NTrksPV" ) = vertex->nTrackParticles();
        }
      
        m_ntupleVars->get<unsigned int>( "NumPV" )++;
      
        m_ntupleVars->get< vector<int> >   ( "NdofTrksPV" ) .emplace_back( vertex->numberDoF() );
        m_ntupleVars->get< vector<double> >( "PVZpile" )    .emplace_back( vertex->position().z() );
      }
      
      ATH_MSG_DEBUG("PrimVertex x/y/z/nDOF "
		    << vertex->x() << ","
		    << vertex->y() << ","
		    << vertex->z() << ","
		    << vertex->numberDoF()     );
      
    }
    
    // Use the dummy PV if no PV is composed
    if( !m_thePV ) {
      ATH_MSG_DEBUG("No Reconstructed PV was found. Using the dummy PV instead.");
      for( auto *vertex : *m_vertexTES ) {
	if( xAOD::VxType::NoVtx != vertex->vertexType() ) continue;
	
        if( m_FillNtuple ) {
          // Not considering pile-up; pick-up the first PV
          if( 0 == m_ntupleVars->get<unsigned int>( "NumPV" ) ) {
            m_thePV = vertex;
	  
            m_ntupleVars->get<double>( "PVX" ) = vertex->x();
            m_ntupleVars->get<double>( "PVY" ) = vertex->y();
            m_ntupleVars->get<double>( "PVZ" ) = vertex->z();
            m_ntupleVars->get<unsigned int>( "PVType" ) = vertex->vertexType();
	  
            // number of tracks associated to the PV
            m_ntupleVars->get<unsigned int>( "NTrksPV" ) = vertex->nTrackParticles();
          }
        }
      }
    }
    
    // if thePV is null, the PV is not found.
    if( !m_thePV ) {
      ATH_MSG_INFO("No PV is found in the PV collection!");
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG(" Primary vertex successful. thePV = " << m_thePV );
    
    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  void VrtSecInclusive::TrackClassification(std::vector<WrkVrt> *WrkVrtSet, 
					    std::vector< std::deque<long int> > *TrkInVrt)
  { 
    // Fill TrkInVrt with vertex IDs of each track
    
    for( size_t iv = 0; iv<WrkVrtSet->size(); iv++ ) {
      
      WrkVrt& vertex = WrkVrtSet->at(iv);
      
      std::deque<long int>& tracks = vertex.SelTrk;
      if( tracks.size()<2 ) continue;
      
      for( size_t itrk=0; itrk < tracks.size(); itrk++ ) {
	
	long int track_id = tracks[itrk];
	( TrkInVrt->at(track_id) ).emplace_back(iv);
      }
    }
    
  }
  
  
  //____________________________________________________________________________________________________
  double VrtSecInclusive::MaxOfShared(std::vector<WrkVrt> *WrkVrtSet, 
				      std::vector< std::deque<long int> > *TrkInVrt,
				      long int & SelectedTrack,
				      long int & SelectedVertex)
  {

    long int NTrack=TrkInVrt->size(); long int it, jv, itmp, NVrt, VertexNumber;
 
    double MaxOf=-999999999;
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
      if(  NVrt < NShMax )    continue;     /*Not a shared track with maximal sharing*/
      for( jv=0; jv<NVrt; jv++ ){
	VertexNumber=(*TrkInVrt)[it][jv];
	if( (*WrkVrtSet).at(VertexNumber).SelTrk.size() <= 1) continue; // one track vertex - nothing to do
	for( itmp=0; itmp<(int)(*WrkVrtSet)[VertexNumber].SelTrk.size(); itmp++){
	  if( (*WrkVrtSet)[VertexNumber].SelTrk[itmp] == it ) {         /* Track found*/  
	    if( MaxOf < (*WrkVrtSet)[VertexNumber].Chi2PerTrk.at(itmp) ){
	      MaxOf = (*WrkVrtSet)[VertexNumber].Chi2PerTrk[itmp];
	      SelectedTrack=it;
	      SelectedVertex=VertexNumber;
	    }
	  }
	}
      }
    }
    return MaxOf;
  }


  //____________________________________________________________________________________________________
  void VrtSecInclusive::printWrkSet(const std::vector<WrkVrt> *WrkVrtSet, const std::string name)
  {
    for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
      std::cout<<name
	       <<"= "<<(*WrkVrtSet)[iv].vertex[0]
	       <<", "<<(*WrkVrtSet)[iv].vertex[1]
	       <<", "<<(*WrkVrtSet)[iv].vertex[2]
	       <<" NTrk="<<(*WrkVrtSet)[iv].SelTrk.size()
	       <<" trk=";
      for(int kk=0; kk<(int)(*WrkVrtSet)[iv].SelTrk.size(); kk++) {
	std::cout<<", "<<(*WrkVrtSet)[iv].SelTrk[kk];}
      std::cout<<'\n';
    }
  }
  
  
  //____________________________________________________________________________________________________
  void VrtSecInclusive::fillTrackSummary( track_summary& summary, const xAOD::TrackParticle *trk ) {
    summary.numIBLHits    = 0;
    summary.numBLayerHits = 0;
    summary.numPixelHits  = 0;
    summary.numSctHits    = 0;
    summary.numTrtHits    = 0;
     
    trk->summaryValue( summary.numIBLHits,    xAOD::numberOfInnermostPixelLayerHits );
    trk->summaryValue( summary.numBLayerHits, xAOD::numberOfNextToInnermostPixelLayerHits );
    trk->summaryValue( summary.numPixelHits,  xAOD::numberOfPixelHits );
    trk->summaryValue( summary.numSctHits,    xAOD::numberOfSCTHits );
    trk->summaryValue( summary.numTrtHits,    xAOD::numberOfTRTHits );
  }
  
  
  //____________________________________________________________________________________________________
  bool VrtSecInclusive::passedFakeReject( const Amg::Vector3D& FitVertex,
					  const xAOD::TrackParticle *itrk,
					  const xAOD::TrackParticle *jtrk  )
  {
    
    const double rad  = sqrt(FitVertex.x()*FitVertex.x() + FitVertex.y()*FitVertex.y()); 
    const double absz = fabs( FitVertex.z() );
    
    const uint32_t pattern_itrk = itrk->hitPattern();
    const uint32_t pattern_jtrk = jtrk->hitPattern();
    
	
    if( m_geoModel == VKalVrtAthena::GeoModel::Run2 ) {
    
      //
      // rough guesses for active layers:
      // BeamPipe: 23.5-24.3
      // IBL: 31.0-38.4
      // Pix0 (BLayer): 47.7-54.4, Pix1: 85.5-92.2, Pix2: 119.3-126.1
      // Sct0: 290-315, Sct1: 360-390, Sct2: 430-460, Sct3:500-530
      //
      
      // vertex area classification
      enum vertexArea {
	insideBeamPipe,
	
	insidePixelBarrel0,
	aroundPixelBarrel0,
	
	outsidePixelBarrel0_and_insidePixelBarrel1,
	aroundPixelBarrel1,
	
	outsidePixelBarrel1_and_insidePixelBarrel2,
	aroundPixelBarrel2,
	
	outsidePixelBarrel2_and_insidePixelBarrel3,
	aroundPixelBarrel3,
	
	outsidePixelBarrel3_and_insideSctBarrel0,
	aroundSctBarrel0,
	
	outsideSctBarrel0_and_insideSctBarrel1,
	aroundSctBarrel1,
      };
      
      // Mutually exclusive vertex position pattern
      int vertex_pattern = 0;
      if( rad < 23.50 ) {
	vertex_pattern = insideBeamPipe;
	
      } else if( rad < 31.0 && absz < 331.5 ) {
	vertex_pattern = insidePixelBarrel0;
	
      } else if( rad < 38.4 && absz < 331.5 ) {
	vertex_pattern = aroundPixelBarrel0;
	
      } else if( rad < 47.7 && absz < 400.5 ) {
	vertex_pattern = outsidePixelBarrel0_and_insidePixelBarrel1;
	
      } else if( rad < 54.4 && absz < 400.5 ) {
	vertex_pattern = aroundPixelBarrel1;
	
      } else if( rad < 85.5 && absz < 400.5 ) {
	vertex_pattern = outsidePixelBarrel1_and_insidePixelBarrel2;
	
      } else if( rad < 92.2 && absz < 400.5 ) {
	vertex_pattern = aroundPixelBarrel2;
	
      } else if( rad < 119.3 && absz < 400.5 ) {
	vertex_pattern = outsidePixelBarrel2_and_insidePixelBarrel3;
	
      } else if( rad < 126.1 && absz < 400.5 ) {
	vertex_pattern = aroundPixelBarrel3;
	
      } else if( rad < 290 && absz < 749.0 ) {
	vertex_pattern = outsidePixelBarrel3_and_insideSctBarrel0;
	
      } else if( rad < 315 && absz < 749.0 ) {
	vertex_pattern = aroundSctBarrel0;
	
      } else if( rad < 360 && absz < 749.0 ) {
	vertex_pattern = outsideSctBarrel0_and_insideSctBarrel1;
	
      } else if( rad < 390 && absz < 749.0 ) {
	vertex_pattern = aroundSctBarrel1;
	
      } else {
      }
      
      
      //////////////////////////////////////////////////////////////////////////////////
      if( vertex_pattern == insideBeamPipe ) {
	
	if( ! (pattern_itrk & (1<<Trk::pixelBarrel0)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::pixelBarrel0)) ) return false;
	
	
      } else if( vertex_pattern == insidePixelBarrel0 ) {
	
	if( ! (pattern_itrk & (1<<Trk::pixelBarrel0)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::pixelBarrel0)) ) return false;
      }
      
      
      else if( vertex_pattern == aroundPixelBarrel0 ) {
	
	// require nothing for PixelBarrel0
	if( ! (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
      }
      
      
      else if( vertex_pattern == outsidePixelBarrel0_and_insidePixelBarrel1 ) {
	
	if(   (pattern_itrk & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel0)) ) return false;
	if( ! (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
      }
      
      
      else if( vertex_pattern == aroundPixelBarrel1 ) {
	
	if(   (pattern_itrk & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel0)) ) return false;
	// require nothing for PixelBarrel1
	if( ! (pattern_itrk & (1<<Trk::pixelBarrel2)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::pixelBarrel2)) ) return false;
      }
      
      
      else if( vertex_pattern == outsidePixelBarrel1_and_insidePixelBarrel2 ) {
	
	if(   (pattern_itrk & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
	if( ! (pattern_itrk & (1<<Trk::pixelBarrel2)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::pixelBarrel2)) ) return false;
      }
      
      
      else if( vertex_pattern == aroundPixelBarrel2 ) {
	
	if(   (pattern_itrk & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
	// require nothing for PixelBarrel2
	if( ! (pattern_itrk & (1<<Trk::pixelBarrel3)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::pixelBarrel3)) ) return false;
      }
      

      else if( vertex_pattern == outsidePixelBarrel2_and_insidePixelBarrel3 ) {
	
	if(   (pattern_itrk & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel2)) ) return false;
	if( ! (pattern_itrk & (1<<Trk::pixelBarrel3)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::pixelBarrel3)) ) return false;
      }
	
      else if( vertex_pattern == aroundPixelBarrel3 ) {
	
	if(   (pattern_itrk & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel2)) ) return false;
	// require nothing for PixelBarrel3
	if( ! (pattern_itrk & (1<<Trk::sctBarrel0)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::sctBarrel0)) ) return false;
      }
      
      
      else if( vertex_pattern == outsidePixelBarrel3_and_insideSctBarrel0 ) {
	
	if(   (pattern_itrk & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel3)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel3)) ) return false;
	if( ! (pattern_itrk & (1<<Trk::sctBarrel0)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::sctBarrel0)) ) return false;
      }
      
      
      else if( vertex_pattern == aroundSctBarrel0 ) {
	
	if(   (pattern_itrk & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel3)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel3)) ) return false;
	// require nothing for SctBarrel0
	if( ! (pattern_itrk & (1<<Trk::sctBarrel1)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::sctBarrel1)) ) return false;
      }
      
      
      else if( vertex_pattern == outsideSctBarrel0_and_insideSctBarrel1 ) {
	
	if(   (pattern_itrk & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel3)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel3)) ) return false;
	if(   (pattern_itrk & (1<<Trk::sctBarrel0)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::sctBarrel0)) ) return false;
	if( ! (pattern_itrk & (1<<Trk::sctBarrel1)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::sctBarrel1)) ) return false;
      }
      
      
      else if( vertex_pattern == aroundSctBarrel1 ) {
	if(   (pattern_itrk & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel3)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel3)) ) return false;
	if(   (pattern_itrk & (1<<Trk::sctBarrel0)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::sctBarrel0)) ) return false;
	// require nothing for SctBarrel1
	if( ! (pattern_itrk & (1<<Trk::sctBarrel2)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::sctBarrel2)) ) return false;
      }
      //////////////////////////////////////////////////////////////////////////////////
      
      return true;
      
    }
    
    else if ( m_geoModel == VKalVrtAthena::GeoModel::Run1 ) {
      
      //
      // rough guesses for active layers:
      // BeamPipe: 25.0
      // Pix0 (BLayer): 47.7-54.4, Pix1: 85.5-92.2, Pix2: 119.3-126.1
      // Sct0: 290-315, Sct1: 360-390, Sct2: 430-460, Sct3:500-530
      //
      
      // vertex area classification
      enum vertexArea {
	insideBeamPipe,
	
	insidePixelBarrel1,
	aroundPixelBarrel1,
	
	outsidePixelBarrel1_and_insidePixelBarrel2,
	aroundPixelBarrel2,
	
	outsidePixelBarrel2_and_insidePixelBarrel3,
	aroundPixelBarrel3,
	
	outsidePixelBarrel3_and_insideSctBarrel0,
	aroundSctBarrel0,
	
	outsideSctBarrel0_and_insideSctBarrel1,
	aroundSctBarrel1,
      };
      
      // Mutually exclusive vertex position pattern
      Int_t vertex_pattern = 0;
      if( rad < 25.00 ) {
	vertex_pattern = insideBeamPipe;
	
      } else if( rad < 47.7 && absz < 400.5 ) {
	vertex_pattern = insidePixelBarrel1;
	
      } else if( rad < 54.4 && absz < 400.5 ) {
	vertex_pattern = aroundPixelBarrel1;
	
      } else if( rad < 85.5 && absz < 400.5 ) {
	vertex_pattern = outsidePixelBarrel1_and_insidePixelBarrel2;
	
      } else if( rad < 92.2 && absz < 400.5 ) {
	vertex_pattern = aroundPixelBarrel2;
	
      } else if( rad < 119.3 && absz < 400.5 ) {
	vertex_pattern = outsidePixelBarrel2_and_insidePixelBarrel3;
	
      } else if( rad < 126.1 && absz < 400.5 ) {
	vertex_pattern = aroundPixelBarrel3;
	
      } else if( rad < 290 && absz < 749.0 ) {
	vertex_pattern = outsidePixelBarrel3_and_insideSctBarrel0;
	
      } else if( rad < 315 && absz < 749.0 ) {
	vertex_pattern = aroundSctBarrel0;
	
      } else if( rad < 360 && absz < 749.0 ) {
	vertex_pattern = outsideSctBarrel0_and_insideSctBarrel1;
	
      } else if( rad < 390 && absz < 749.0 ) {
	vertex_pattern = aroundSctBarrel1;
	
      } else {
      }
      
      
      //////////////////////////////////////////////////////////////////////////////////
      if( vertex_pattern == insideBeamPipe ) {
	
	if( ! (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
	
      }
      
      
      else if( vertex_pattern == insidePixelBarrel1 ) {
	
	if( ! (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
      }
      
      
      else if( vertex_pattern == aroundPixelBarrel1 ) {
	
	// require nothing for PixelBarrel1
	if( ! (pattern_itrk & (1<<Trk::pixelBarrel2)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::pixelBarrel2)) ) return false;
      }
      
      
      else if( vertex_pattern == outsidePixelBarrel1_and_insidePixelBarrel2 ) {
	
	if(   (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
	if( ! (pattern_itrk & (1<<Trk::pixelBarrel2)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::pixelBarrel2)) ) return false;
      }
      
      
      else if( vertex_pattern == aroundPixelBarrel2 ) {
	
	if(   (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
	// require nothing for PixelBarrel2
	if( ! (pattern_itrk & (1<<Trk::pixelBarrel3)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::pixelBarrel3)) ) return false;
      }
      

      else if( vertex_pattern == outsidePixelBarrel2_and_insidePixelBarrel3 ) {
	
	if(   (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel2)) ) return false;
	if( ! (pattern_itrk & (1<<Trk::pixelBarrel3)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::pixelBarrel3)) ) return false;
      }
	
      else if( vertex_pattern == aroundPixelBarrel3 ) {
	
	if(   (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel2)) ) return false;
	// require nothing for PixelBarrel3
	if( ! (pattern_itrk & (1<<Trk::sctBarrel0)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::sctBarrel0)) ) return false;
      }
      
      
      else if( vertex_pattern == outsidePixelBarrel3_and_insideSctBarrel0 ) {
	
	if(   (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel3)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel3)) ) return false;
	if( ! (pattern_itrk & (1<<Trk::sctBarrel0)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::sctBarrel0)) ) return false;
      }
      
      
      else if( vertex_pattern == aroundSctBarrel0 ) {
	
	if(   (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel3)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel3)) ) return false;
	// require nothing for SctBarrel0
	if( ! (pattern_itrk & (1<<Trk::sctBarrel1)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::sctBarrel1)) ) return false;
      }
      
      
      else if( vertex_pattern == outsideSctBarrel0_and_insideSctBarrel1 ) {
	
	if(   (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel3)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel3)) ) return false;
	if(   (pattern_itrk & (1<<Trk::sctBarrel0)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::sctBarrel0)) ) return false;
	if( ! (pattern_itrk & (1<<Trk::sctBarrel1)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::sctBarrel1)) ) return false;
      }
      
      
      else if( vertex_pattern == aroundSctBarrel1 ) {
	if(   (pattern_itrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern_itrk & (1<<Trk::pixelBarrel3)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::pixelBarrel3)) ) return false;
	if(   (pattern_itrk & (1<<Trk::sctBarrel0)) ) return false;
	if(   (pattern_jtrk & (1<<Trk::sctBarrel0)) ) return false;
	// require nothing for SctBarrel1
	if( ! (pattern_itrk & (1<<Trk::sctBarrel2)) ) return false;
	if( ! (pattern_jtrk & (1<<Trk::sctBarrel2)) ) return false;
      }
      //////////////////////////////////////////////////////////////////////////////////
      
      return true;
      
    }
    
    return true;
  }
  

} // end of namespace VKalVrtAthena


