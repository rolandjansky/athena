/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecInclusive/VrtSecInclusive.h"
#include "VrtSecInclusive/IntersectionPos.h"
#include "VrtSecInclusive/NtupleVars.h"

#include "TrkTrackSummary/TrackSummary.h"

#include <iostream>
#include <map>
#include <vector>
#include <deque>

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
  bool isAssociatedToVertices( const xAOD::TrackParticle *trk, const xAOD::VertexContainer* vertices ) {
    
      bool is_pv_associated = false;
      
      for( auto* vtx : *vertices ) {
        for( size_t iv = 0; iv < vtx->nTrackParticles(); iv++ ) {
          auto* pvtrk = vtx->trackParticle( iv );
          if( trk == pvtrk ) {
            is_pv_associated = true;
            break;
          }
        }
      }
      return is_pv_associated;
  }
  
  //____________________________________________________________________________________________________
  double vtxVtxDistance( const Amg::Vector3D& v1, const Amg::Vector3D& v2 ) {
    return (v1-v2).norm();
  }
  
  
  //____________________________________________________________________________________________________
  double VrtSecInclusive::significanceBetweenVertices( const WrkVrt& v1, const WrkVrt& v2 ) const {
    try {
      const auto distance = v2.vertex - v1.vertex;
      AmgSymMatrix(3) sumCov;
      
      sumCov.fillSymmetric(0, 0, v1.vertexCov.at(0) + v2.vertexCov.at(0));
      sumCov.fillSymmetric(1, 0, v1.vertexCov.at(1) + v2.vertexCov.at(1));
      sumCov.fillSymmetric(1, 1, v1.vertexCov.at(2) + v2.vertexCov.at(2));
      sumCov.fillSymmetric(2, 0, v1.vertexCov.at(3) + v2.vertexCov.at(3));
      sumCov.fillSymmetric(2, 1, v1.vertexCov.at(4) + v2.vertexCov.at(4));
      sumCov.fillSymmetric(2, 2, v1.vertexCov.at(5) + v2.vertexCov.at(5));
      
      const double s2 = distance.transpose() * sumCov.inverse() * distance;
      
      return s2 > 0. ? sqrt( s2 ) : AlgConsts::maxValue;
    } catch(...) {
      ATH_MSG_WARNING( " >>> " << __FUNCTION__ << ": detected covariance matrix broken exception" );
      return AlgConsts::maxValue;
    }
  }
  
  
  //____________________________________________________________________________________________________
  double VrtSecInclusive::distanceBetweenVertices( const WrkVrt& v1, const WrkVrt& v2 ) const {
    return (v2.vertex - v1.vertex).norm();
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::disassembleVertex(std::vector<WrkVrt> *workVerticesContainer, const unsigned& iv )
  {
    
    auto& wrkvrt = workVerticesContainer->at(iv);
    
    ATH_MSG_VERBOSE(" >> disassembleVertex(): begin: disassembling vertex[" << iv << "], workVerticesContainer.size() = " << workVerticesContainer->size() );
    ATH_MSG_VERBOSE(" >> disassembleVertex(): Vertex: r = " << wrkvrt.vertex.perp() << ", z = " << wrkvrt.vertex.z() );

    // Loop over the tracks associated to the vertex and slect the maximum chi2 track
    const auto& ntrk = wrkvrt.selectedTrackIndices.size();
    size_t maxChi2TrackIndex       = AlgConsts::invalidUnsigned;
    
    // If the size of the tracks is less than 2, this algorithm is meaningless.
    if( wrkvrt.selectedTrackIndices.size() <= 2 ) return StatusCode::SUCCESS;
    
    for( auto& index : wrkvrt.selectedTrackIndices ) {
      const xAOD::TrackParticle* trk = m_selectedTracks->at( index );
      
      ATH_MSG_VERBOSE(" >> disassembleVertex(): > track at vertex[" << iv << "]: "
		      << "index = " << trk->index()
		      << ", pT = "  << trk->pt()
		      << ", phi = " << trk->phi()
		      << ", d0 = "  << trk->d0()
		      << ", z0 = "  << trk->z0());
    }
    
    // find the track with the maximum chi2
    const auto& max = std::max_element( wrkvrt.Chi2PerTrk.begin(), wrkvrt.Chi2PerTrk.end() );
    
    if( max == wrkvrt.Chi2PerTrk.end() ) return StatusCode::SUCCESS;
    
    maxChi2TrackIndex = max - wrkvrt.Chi2PerTrk.begin();
    
    // return if no track is found.
    if(maxChi2TrackIndex == AlgConsts::invalidUnsigned ) return StatusCode::SUCCESS;
    
    
    // defind work variables
    vector<const xAOD::NeutralParticle*>  dummyNeutrals;
    
    vector<WrkVrt> new_vertices;
    
    // Loop over the tracks associated to the vertex other than the selected tracks
    ATH_MSG_VERBOSE(" >> disassembleVertex(): Loop over the tracks associated to the vertex other than the selected tracks.");
    for(size_t itrk=0; itrk<ntrk; itrk++) {
      
      ATH_MSG_VERBOSE(" >> disassembleVertex(): > Loop itrk = " << itrk << " / " << ntrk );
      
      // reject the selected track
      if( itrk == maxChi2TrackIndex ) {
	ATH_MSG_VERBOSE(" >> disassembleVertex(): > skipped." );
	continue;
      }
      
      const size_t this_trk_id     = wrkvrt.selectedTrackIndices[itrk];
      const size_t selected_trk_id = wrkvrt.selectedTrackIndices[maxChi2TrackIndex];
      
      ATH_MSG_VERBOSE(" >> disassembleVertex(): > this_trk_id  = " << this_trk_id << ", selected_trk_id = " << selected_trk_id << ", alltrks_size = " << m_selectedTracks->size() );
      if( this_trk_id >= m_selectedTracks->size() ) {
	ATH_MSG_VERBOSE(" >> disassembleVertex(): > this_trk_id is invalid. continue!" );
	continue;
      }
      if( selected_trk_id >= m_selectedTracks->size() ) {
	ATH_MSG_VERBOSE(" >> disassembleVertex(): > selected_trk_id is invalid. continue!" );
	continue;
      }
      
      ATH_MSG_VERBOSE(" >> disassembleVertex(): > Storing tracks to ListBaseTracks" );
      ATH_MSG_VERBOSE(" >> disassembleVertex(): > m_selectedTracks->at( this_trk_id ) = " << m_selectedTracks->at( this_trk_id     )->index() );
      ATH_MSG_VERBOSE(" >> disassembleVertex(): > m_selectedTracks->at( this_trk_id ) = " << m_selectedTracks->at( selected_trk_id )->index() );
      
      vector<const xAOD::TrackParticle*>    ListBaseTracks;
      ListBaseTracks.emplace_back( m_selectedTracks->at( this_trk_id     ) );
      ListBaseTracks.emplace_back( m_selectedTracks->at( selected_trk_id ) );
	
      ATH_MSG_VERBOSE(" >> disassembleVertex(): > ListBaseTracks was stored." );
      
      WrkVrt newvrt;
      newvrt.selectedTrackIndices.emplace_back( this_trk_id );
      newvrt.selectedTrackIndices.emplace_back( selected_trk_id );
      
      // Fit the new vertex
      ATH_MSG_VERBOSE(" >> disassembleVertex(): > Fast Fit" );
      
      m_fitSvc->setDefault();
      ATH_CHECK( m_fitSvc->VKalVrtFitFast( ListBaseTracks, newvrt.vertex ) );
      
      ATH_MSG_VERBOSE( " >> disassembleVertex(): > ApproxVertex: r = " << newvrt.vertex.perp() << ", z = " << newvrt.vertex.z() );
      
      if( vtxVtxDistance( wrkvrt.vertex, newvrt.vertex ) > 10. ) 
        {
          m_fitSvc->setApproximateVertex( wrkvrt.vertex[0], wrkvrt.vertex[1], wrkvrt.vertex[2] );
        }
      else
        {
          m_fitSvc->setApproximateVertex( newvrt.vertex[0], newvrt.vertex[1], newvrt.vertex[2] );
        }
      
      ATH_MSG_VERBOSE(" >> disassembleVertex(): > Fit the new vertex" );
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
      
      newvrt.closestWrkVrtIndex    = 0;
      newvrt.closestWrkVrtValue    = AlgConsts::maxValue;
      
      // register the new vertex to the vertex list
      ATH_MSG_VERBOSE(" >> disassembleVertex(): > register the new vertex to the vertex list" );
      new_vertices.emplace_back( newvrt );
    }
    
    // remove the selected track from the original vertex
    wrkvrt.selectedTrackIndices.erase( wrkvrt.selectedTrackIndices.begin() + maxChi2TrackIndex ); //remove track
    ATH_MSG_VERBOSE(" >> disassembleVertex(): removed the selected track from the original vertex. wrkvrt.selectedTrackIndices.size = " << wrkvrt.selectedTrackIndices.size() );
    
    // refit the original vertex
    ATH_MSG_VERBOSE(" >> disassembleVertex(): refit the original vertex" );

    StatusCode sc = refitVertex( wrkvrt );
    if( sc.isFailure() ) {
        // WARNING CODE ATLASRECTS-3145::001 refitVertex Failure, vertex lost
        //ATH_MSG_WARNING("ATLASRECTS-3145::001" );
        return StatusCode::SUCCESS;
    }
    // end of workaround
    
    for( auto vertex : new_vertices ) {
      ATH_MSG_VERBOSE(" >> disassembleVertex(): > emplace_back new vertex" );
      workVerticesContainer->emplace_back( vertex );
    }
    
    ATH_MSG_VERBOSE(" >> disassembleVertex(): end. workVerticesContainer.size() = " << workVerticesContainer->size() );
    return StatusCode::SUCCESS;
  }

  
  
  //____________________________________________________________________________________________________
  double  VrtSecInclusive::improveVertexChi2( WrkVrt& vertex )
  {
    //
    //  Iterate track removal until vertex get good Chi2
    //
    
    ATH_MSG_VERBOSE( " >>> " << __FUNCTION__ << ": begin: chi2 = " << vertex.Chi2);
    
    if( vertex.nTracksTotal() < 2 ) return 0.;
    
    StatusCode sc = refitVertexWithSuggestion( vertex, vertex.vertex );
    if( sc.isFailure() ) {
      return 0;
    }
    
    double chi2Probability = TMath::Prob( vertex.Chi2, vertex.ndof() );
    
    while (chi2Probability < m_jp.improveChi2ProbThreshold ) {
      if( vertex.nTracksTotal() == 2 ) return chi2Probability;
      
      WrkVrt vertex_backup = vertex;
      
      auto maxChi2 = std::max_element( vertex.Chi2PerTrk.begin(), vertex.Chi2PerTrk.end() );
      size_t index   = maxChi2 - vertex.Chi2PerTrk.begin();
      
      
      ATH_MSG_VERBOSE( " >>> " << __FUNCTION__ << ": maxChi2 index = " << index << " / " << vertex.Chi2PerTrk.size() );
      
      if( index < vertex.selectedTrackIndices.size() ) {
        vertex.selectedTrackIndices.erase( vertex.selectedTrackIndices.begin() + index ); //remove track
      } else {
        index -= vertex.selectedTrackIndices.size();
        if( index >= vertex.associatedTrackIndices.size() ) {
          ATH_MSG_WARNING( " >>> " << __FUNCTION__ << ": invalid index" );
          break;
        }
        vertex.associatedTrackIndices.erase( vertex.associatedTrackIndices.begin() + index ); //remove track
      }
      
      StatusCode sc = refitVertexWithSuggestion( vertex, vertex.vertex );
      
      if( sc.isFailure() ) {
        vertex = vertex_backup;
        return 0.;
      }
      
      chi2Probability = TMath::Prob( vertex.Chi2, vertex.ndof() );
    }
    
    //if(chi2Probability<0.001) vertex.isGood = false;
    
    ATH_MSG_VERBOSE( " >>> " << __FUNCTION__ << ": end: chi2 = " << vertex.Chi2);
    
    return chi2Probability;
  }



  //____________________________________________________________________________________________________
  void VrtSecInclusive::removeTrackFromVertex(std::vector<WrkVrt> *workVerticesContainer, 
					      std::vector< std::deque<long int> > *TrkInVrt,
					      const long int & trackIndexToRemove,
					      const long int & SelectedVertex)
  {
    
    auto& wrkvrt = workVerticesContainer->at(SelectedVertex);
    auto& tracks = wrkvrt.selectedTrackIndices;
    
    {
      auto end = std::remove_if( tracks.begin(), tracks.end(), [&](long int index) { return index == trackIndexToRemove; } );
      tracks.erase( end, tracks.end() );
    }
    
    { 
      for( auto& trks : *TrkInVrt ) {
        auto end = std::remove_if( trks.begin(), trks.end(), [&](long int index) { return index == trackIndexToRemove; } );
        trks.erase( end, trks.end() );
      }
    }


    //Check if track is removed from 2tr vertex => then sharing of track left should also be decreased
    if( wrkvrt.selectedTrackIndices.size() == 1 ) {
      
      const auto& leftTrackIndex = *( tracks.begin() );
      auto& list = TrkInVrt->at(leftTrackIndex);
      auto end = std::remove_if( list.begin(), list.end(), [&](long int index) { return index == trackIndexToRemove; } );
      list.erase( end, list.end() );
      
    }

  }
  
  
  
  //____________________________________________________________________________________________________
  double VrtSecInclusive::findMinVerticesPair( std::vector<WrkVrt> *workVerticesContainer, std::pair<unsigned, unsigned>& indexPair, VrtSecInclusive::AlgForVerticesPair algorithm )
  {  
    //
    //  Minimal normalized vertex-vertex distance
    //
    
    for( auto& workVertex : *workVerticesContainer ) {
      workVertex.closestWrkVrtValue = AlgConsts::maxValue;
      workVertex.closestWrkVrtIndex = 0;
    }

    double minValue = AlgConsts::maxValue;
    
    for( auto iv = workVerticesContainer->begin(); iv != workVerticesContainer->end(); ++iv ) {
      if( (*iv).selectedTrackIndices.size()< 2) continue;   /* Bad vertices */
      
      auto i_index = iv - workVerticesContainer->begin();
      
      for( auto jv = std::next(iv); jv != workVerticesContainer->end(); ++jv ) {
        if( (*jv).selectedTrackIndices.size()< 2) continue;   /* Bad vertices */
        
        auto j_index = iv - workVerticesContainer->begin();
        
        double value = (this->*algorithm)( (*iv), (*jv) );
        
        if( value < minValue ){
          minValue = value;
          indexPair.first  = i_index;
          indexPair.second = j_index;
        }
        if( value < (*iv).closestWrkVrtValue ) {(*iv).closestWrkVrtValue = value; (*iv).closestWrkVrtIndex = j_index; }
        if( value < (*jv).closestWrkVrtValue ) {(*jv).closestWrkVrtValue = value; (*jv).closestWrkVrtIndex = i_index; }
      }
    }
    
    return minValue;
  }


  //____________________________________________________________________________________________________
  double VrtSecInclusive::findMinVerticesNextPair( std::vector<WrkVrt> *workVerticesContainer, std::pair<unsigned, unsigned>& indexPair )
  {  
    //   
    // Give minimal distance between nonmodifed yet vertices
    //   

    indexPair.first = 0;
    indexPair.second = 0;
   
    double minValue = AlgConsts::maxValue;
   
    for(unsigned iv=0; iv<workVerticesContainer->size()-1; iv++) {
      auto& vertex = workVerticesContainer->at(iv);
     
      if( vertex.selectedTrackIndices.size() < 2) continue;   /* Bad vertex */
      if( vertex.closestWrkVrtIndex == 0 )   continue;   /* Used vertex */
     
      if( vertex.closestWrkVrtValue < minValue ) {
       
        unsigned jv = vertex.closestWrkVrtIndex;
       
        // Close vertex to given [iv] one is modified already 
        if( workVerticesContainer->at(jv).closestWrkVrtIndex == 0 ) continue;
       
        minValue = vertex.closestWrkVrtValue;
       
        indexPair.first  = iv;
        indexPair.second = jv;
       
      }
    }
   
    return minValue;
  }
  
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::mergeVertices( WrkVrt& v1, WrkVrt& v2 )
  {
    //
    //  Merge two close vertices into one (first) and set NTr=0 for second vertex
    //
    
    // firstly, take a backup of the original vertices
    auto v1_bak = v1;
    auto v2_bak = v2;
    
    for( auto& index : v2.selectedTrackIndices ) { v1.selectedTrackIndices.emplace_back( index ); }

    // Cleaning
    deque<long int>::iterator TransfEnd;
    sort( v1.selectedTrackIndices.begin(), v1.selectedTrackIndices.end() );
    TransfEnd =  unique(v1.selectedTrackIndices.begin(), v1.selectedTrackIndices.end() );
    v1.selectedTrackIndices.erase( TransfEnd, v1.selectedTrackIndices.end());
    //
    //----------------------------------------------------------
    v2.selectedTrackIndices.clear();             //Clean dropped vertex
    v2.closestWrkVrtValue = AlgConsts::maxValue; //Clean dropped vertex
    v2.closestWrkVrtIndex=0;                     //Clean dropped vertex
    v2.isGood=false;                             //Clean dropped vertex
    
    v1.closestWrkVrtValue = AlgConsts::maxValue; //Clean new vertex
    v1.closestWrkVrtIndex=0;                     //Clean new vertex
    v1.isGood=true;                              //Clean new vertex
    
    StatusCode sc = refitVertex( v1 );
    if( sc.isFailure() ) {
      v1 = v1_bak;
      v2 = v2_bak;
      
      ATH_MSG_DEBUG(" >>> " << __FUNCTION__ << ": failure in merging" );
        
      return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
  }
  
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::refitVertex( WrkVrt& workVertex )
  {
    
    //
    vector<const xAOD::NeutralParticle*> dummyNeutrals;
      
    int nth = workVertex.selectedTrackIndices.size();
 
    if(nth<2) {
      workVertex.isGood = false;
      return StatusCode::SUCCESS;
    }

    vector<const xAOD::TrackParticle*>  ListBaseTracks;
    
    workVertex.Chi2PerTrk.clear();
    
    for( const auto& index : workVertex.selectedTrackIndices ) {
      ListBaseTracks.emplace_back( m_selectedTracks->at( index ) );
      workVertex.Chi2PerTrk.emplace_back( AlgConsts::chi2PerTrackInitValue );
    }
    
    for( const auto& index : workVertex.associatedTrackIndices ) {
      ListBaseTracks.emplace_back( m_associatedTracks->at( index ) );
      workVertex.Chi2PerTrk.emplace_back( AlgConsts::chi2PerTrackInitValue );
    }
    
    auto& vertexPos = workVertex.vertex;
        
    m_fitSvc->setApproximateVertex( vertexPos.x(), vertexPos.y(), vertexPos.z() );
    
    ATH_MSG_VERBOSE( " >>> refitVertex: ListBaseTracks.size = " << ListBaseTracks.size()
                   << ", #selectedBaseTracks = " << workVertex.selectedTrackIndices.size()
                   << ", #assocTracks = " << workVertex.associatedTrackIndices.size() );
    for( auto *trk : ListBaseTracks ) {
      ATH_MSG_VERBOSE( " >>> refitVertex: track index = " << trk->index() );
    }
    
    m_fitSvc->setDefault();
    ATH_MSG_VERBOSE( " >>> refitVertex: m_fitSvc is reset." );
    
    Amg::Vector3D initVertex;
    
    StatusCode sc = m_fitSvc->VKalVrtFitFast( ListBaseTracks, initVertex );/* Fast crude estimation */
    if(sc.isFailure()) ATH_MSG_DEBUG(" >>> refitVertex: fast crude estimation failed.");
    ATH_MSG_VERBOSE( " >>> refitVertex: Fast VKalVrtFit succeeded. vertex (r,z) = (" << initVertex.perp() << ", " << initVertex.z() << ", " << ")" );
    
    if( vtxVtxDistance( initVertex, vertexPos ) > 10. ) {
      
      m_fitSvc->setApproximateVertex( vertexPos.x(), vertexPos.y(), vertexPos.z() );
      
    } else {
      
      m_fitSvc->setApproximateVertex( initVertex.x(), initVertex.y(), initVertex.z() );
      
    }
    
    ATH_MSG_VERBOSE( " >>> refitVertex: approx vertex is set. Now going to perform fitting..." );
    
    StatusCode SC=m_fitSvc->VKalVrtFit(ListBaseTracks,dummyNeutrals,
				       workVertex.vertex,
				       workVertex.vertexMom,
				       workVertex.Charge,
				       workVertex.vertexCov,
				       workVertex.Chi2PerTrk, 
				       workVertex.TrkAtVrt,
				       workVertex.Chi2); 

    auto& cov = workVertex.vertexCov;
        
    if(SC.isFailure()) ATH_MSG_DEBUG(" >>> refitVertex: SC in refitVertex returned failure "); 
    ATH_MSG_VERBOSE(" >>> refitVertex "<<SC<<", "<<ListBaseTracks.size()<<","<<workVertex.Chi2PerTrk.size());
    ATH_MSG_VERBOSE( " >>> refitVertex: succeeded in fitting. New vertex pos (r,z) = (" << vertexPos.perp() << ", " << vertexPos.z() << ")" );
    ATH_MSG_VERBOSE( " >>> refitVertex: New vertex cov = (" << cov.at(0) << ", " << cov.at(1) << ", " << cov.at(2) << ", " << cov.at(3) << ", " << cov.at(4) << ", " << cov.at(5) << ")" );

    return SC;
  }
  

  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::refitVertexWithSuggestion( WrkVrt& workVertex, const Amg::Vector3D& suggestedPosition )
  {
    
    //
    vector<const xAOD::NeutralParticle*> dummyNeutrals;
      
    int nth = workVertex.selectedTrackIndices.size();
 
    if(nth<2) {
      workVertex.isGood = false;
      return StatusCode::SUCCESS;
    }

    vector<const xAOD::TrackParticle*>  ListBaseTracks;
    
    workVertex.Chi2PerTrk.clear();
    
    for( const auto& index : workVertex.selectedTrackIndices ) {
      ListBaseTracks.emplace_back( m_selectedTracks->at( index ) );
      workVertex.Chi2PerTrk.emplace_back( AlgConsts::chi2PerTrackInitValue );
    }
    
    for( const auto& index : workVertex.associatedTrackIndices ) {
      ListBaseTracks.emplace_back( m_associatedTracks->at( index ) );
      workVertex.Chi2PerTrk.emplace_back( AlgConsts::chi2PerTrackInitValue );
    }
    
    auto& vertexPos = workVertex.vertex;
        
    m_fitSvc->setApproximateVertex( suggestedPosition.x(), suggestedPosition.y(), suggestedPosition.z() );
    
    ATH_MSG_VERBOSE( " >>> " << __FUNCTION__ <<": ListBaseTracks.size = " << ListBaseTracks.size()
                   << ", #selectedBaseTracks = " << workVertex.selectedTrackIndices.size()
                   << ", #assocTracks = " << workVertex.associatedTrackIndices.size() );
    for( auto *trk : ListBaseTracks ) {
      ATH_MSG_VERBOSE( " >>> " << __FUNCTION__ << ": track index = " << trk->index() );
    }
    
    m_fitSvc->setDefault();
    ATH_MSG_VERBOSE( " >>> " << __FUNCTION__ << ": m_fitSvc is reset." );
    
    ATH_MSG_VERBOSE( " >>> " << __FUNCTION__ << ": approx vertex is set. Now going to perform fitting..." );
    
    StatusCode SC=m_fitSvc->VKalVrtFit(ListBaseTracks,dummyNeutrals,
				       workVertex.vertex,
				       workVertex.vertexMom,
				       workVertex.Charge,
				       workVertex.vertexCov,
				       workVertex.Chi2PerTrk, 
				       workVertex.TrkAtVrt,
				       workVertex.Chi2); 

    auto& cov = workVertex.vertexCov;
        
    if( SC.isFailure() ) ATH_MSG_VERBOSE(" >>> " << __FUNCTION__ << ": SC in refitVertex returned failure "); 
    ATH_MSG_VERBOSE(" >>> " << __FUNCTION__ << ": "<<SC<<", "<<ListBaseTracks.size()<<","<<workVertex.Chi2PerTrk.size());
    
    if( SC.isSuccess() ) {
      ATH_MSG_VERBOSE( " >>> " << __FUNCTION__ << ": succeeded in fitting. New vertex pos = (" << vertexPos.x() << ", " << vertexPos.y() << ", " << vertexPos.z() << ")" );
      ATH_MSG_VERBOSE( " >>> " << __FUNCTION__ << ": New vertex cov = (" << cov.at(0) << ", " << cov.at(1) << ", " << cov.at(2) << ", " << cov.at(3) << ", " << cov.at(4) << ", " << cov.at(5) << ")" );
    }

    return SC;
  }
  

  //____________________________________________________________________________________________________
  size_t VrtSecInclusive::nTrkCommon( std::vector<WrkVrt> *workVerticesContainer, const std::pair<unsigned, unsigned>& pairIndex) const
  {
    //
    //  Number of common tracks for 2 vertices
    //
    
    auto& trackIndices1 = workVerticesContainer->at( pairIndex.first ).selectedTrackIndices;
    auto& trackIndices2 = workVerticesContainer->at( pairIndex.second ).selectedTrackIndices;
    
    if( trackIndices1.size() < 2 ) return 0;
    if( trackIndices2.size() < 2 ) return 0;
    
    size_t nTrkCom = 0;
    
    for( auto& index : trackIndices1 ) {
	if( std::find(trackIndices2.begin(),trackIndices2.end(), index) != trackIndices2.end()) nTrkCom++;
    }
    
    return nTrkCom;
  }
  
  
  
  //____________________________________________________________________________________________________
  void VrtSecInclusive::declareProperties() {
    
    declareProperty("GeoModel",                        m_jp.geoModel                        = VKalVrtAthena::GeoModel::Run2 );
    
    declareProperty("TrackLocation",                   m_jp.TrackLocation                   = "InDetTrackParticles"         );
    declareProperty("PrimVrtLocation",                 m_jp.PrimVrtLocation                 = "PrimaryVertices"             );
    declareProperty("McParticleContainer",             m_jp.truthParticleContainerName      = "TruthParticles"              );
    declareProperty("MCEventContainer",                m_jp.mcEventContainerName            = "TruthEvents"                 );
    
    declareProperty("All2trkVerticesContainerName",    m_jp.all2trksVerticesContainerName   = "All2TrksVertices"            );
    declareProperty("SecondaryVerticesContainerName",  m_jp.secondaryVerticesContainerName  = "SecondaryVertices"           );

    declareProperty("FillHist",                        m_jp.FillHist                        = false                         );
    declareProperty("FillNtuple",                      m_jp.FillNtuple                      = false                         );
    declareProperty("FillIntermediateVertices",        m_jp.FillIntermediateVertices        = false                         );
    declareProperty("DoIntersectionPos",               m_jp.doIntersectionPos               = false                         );
    declareProperty("DoMapToLocal",                    m_jp.doMapToLocal                    = false                         );
    declareProperty("DoTruth",                         m_jp.doTruth                         = false                         );
    declareProperty("DoPVcompatibility",               m_jp.doPVcompatibilityCut            = true                          );
    declareProperty("RemoveFake2TrkVrt",               m_jp.removeFakeVrt                   = true                          );
    declareProperty("DoDelayedFakeReject",             m_jp.removeFakeVrtLate               = false                         );
    declareProperty("CheckHitPatternStrategy",         m_checkPatternStrategy               = "Classical"                   ); // Either Classical or Extrapolation
    declareProperty("MCTrackResolution",               m_jp.mcTrkResolution                 = 0.06                          ); // see getTruth for explanation
    declareProperty("TruthTrkLen",                     m_jp.TruthTrkLen                     = 1000                          ); // in [mm]
    declareProperty("ExtrapPV",                        m_jp.extrapPV                        = false                         ); // Leave false. only for testing
    
    // default values are set upstream - check top of file
    declareProperty("do_PVvetoCut",                    m_jp.do_PVvetoCut                    = true                          );
    declareProperty("do_d0Cut",                        m_jp.do_d0Cut                        = true                          );
    declareProperty("do_z0Cut",                        m_jp.do_z0Cut                        = true                          );
    declareProperty("do_d0errCut",                     m_jp.do_d0errCut                     = false                         );
    declareProperty("do_z0errCut",                     m_jp.do_z0errCut                     = false                         );
    declareProperty("do_d0signifCut",                  m_jp.do_d0signifCut                  = false                         );
    declareProperty("do_z0signifCut",                  m_jp.do_z0signifCut                  = false                         );
    
    declareProperty("ImpactWrtBL",                     m_jp.ImpactWrtBL                     = true                          ); // false option is going to be deprecated
    declareProperty("a0TrkPVDstMinCut",                m_jp.d0TrkPVDstMinCut                = 0.                            ); // in [mm]
    declareProperty("a0TrkPVDstMaxCut",                m_jp.d0TrkPVDstMaxCut                = 1000.                         ); // in [mm]
    declareProperty("a0TrkPVSignifCut",                m_jp.d0TrkPVSignifCut                = 0.                            ); // in [mm]
    declareProperty("zTrkPVDstMinCut",                 m_jp.z0TrkPVDstMinCut                = 0.                            ); // in [mm]
    declareProperty("zTrkPVDstMaxCut",                 m_jp.z0TrkPVDstMaxCut                = 1000.                         ); // in [mm]
    declareProperty("zTrkPVSignifCut",                 m_jp.z0TrkPVSignifCut                = 0.                            ); // in unit of sigma
    declareProperty("TrkA0ErrCut",                     m_jp.d0TrkErrorCut                   = 10000                         ); // in [mm]
    declareProperty("TrkZErrCut",                      m_jp.z0TrkErrorCut                   = 20000                         ); // in [mm]

    declareProperty("SelTrkMaxCutoff",                 m_jp.SelTrkMaxCutoff                 = 50                            ); // max number of tracks
    declareProperty("TrkPtCut",                        m_jp.TrkPtCut                        = 1000.                         ); // low pT threshold. in [MeV]
    declareProperty("TrkChi2Cut",                      m_jp.TrkChi2Cut                      = 3.                            ); // in terms of chi2 / ndof
    declareProperty("PVcompatibilityCut",              m_jp.pvCompatibilityCut              = -20.                          ); // in [mm]
    declareProperty("SelVrtChi2Cut",                   m_jp.SelVrtChi2Cut                   = 4.5                           ); // in terms of chi2 / ndof
    
    declareProperty("CutSctHits",                      m_jp.CutSctHits                      = 0                             );
    declareProperty("CutPixelHits",                    m_jp.CutPixelHits                    = 0                             );
    declareProperty("CutSiHits",                       m_jp.CutSiHits                       = 0                             );
    declareProperty("DoSAloneTRT",                     m_jp.SAloneTRT                       = false                         ); // SAlone = "standalone"
    declareProperty("CutBLayHits",                     m_jp.CutBLayHits                     = 0                             );
    declareProperty("CutSharedHits",                   m_jp.CutSharedHits                   = 0                             );
    declareProperty("doTRTPixCut",                     m_jp.doTRTPixCut                     = false                         ); // mode for R-hadron displaced vertex
    declareProperty("CutTRTHits",                      m_jp.CutTRTHits                      = 0                             );
    
    declareProperty("doReassembleVertices",            m_jp.doReassembleVertices            = false                         );
    declareProperty("doMergeByShuffling",              m_jp.doMergeByShuffling              = false                         );
    declareProperty("doSuggestedRefitOnMerging",       m_jp.doSuggestedRefitOnMerging       = true                          ); // sub-option of doMergeByShuffling-1
    declareProperty("doMagnetMerging",                 m_jp.doMagnetMerging                 = true                          ); // sub-option of doMergeByShuffling-2
    declareProperty("doWildMerging",                   m_jp.doWildMerging                   = true                          ); // sub-option of doMergeByShuffling-3
    declareProperty("doMergeFinalVerticesDistance",    m_jp.doMergeFinalVerticesDistance    = false                         );
    declareProperty("doAssociateNonSelectedTracks",    m_jp.doAssociateNonSelectedTracks    = false                         );
    declareProperty("doFinalImproveChi2",              m_jp.doFinalImproveChi2              = false                         );
    
    declareProperty("VertexMergeCut",                  m_jp.VertexMergeCut                  = 3                             );
    declareProperty("TrackDetachCut",                  m_jp.TrackDetachCut                  = 6                             );
    declareProperty("associateMinDistanceToPV",        m_jp.associateMinDistanceToPV        = 1.                            );
    declareProperty("associateMaxD0",                  m_jp.associateMaxD0                  = 1.                            ); // wrt. DV in [mm]
    declareProperty("associateMaxZ0",                  m_jp.associateMaxZ0                  = 3.                            ); // wrt. DV in [mm]
    declareProperty("associatePtCut",                  m_jp.associatePtCut                  = 0.                            ); // in [MeV]
    declareProperty("associateChi2Cut",                m_jp.associateChi2Cut                = 20.                           );
    declareProperty("reassembleMaxImpactParameterD0",  m_jp.reassembleMaxImpactParameterD0  = 1.                            ); // wrt. DV in [mm]
    declareProperty("reassembleMaxImpactParameterZ0",  m_jp.reassembleMaxImpactParameterZ0  = 5.                            ); // wrt. DV in [mm]
    declareProperty("mergeByShufflingMaxSignificance", m_jp.mergeByShufflingMaxSignificance = 100.                          ); // in unit of sigma
    declareProperty("mergeByShufflingAllowance",       m_jp.mergeByShufflingAllowance       = 4.                            ); // in unit of sigma
    declareProperty("VertexMergeFinalDistCut",         m_jp.VertexMergeFinalDistCut         = 1.                            ); // in [mm]
    declareProperty("VertexMergeFinalDistScaling",     m_jp.VertexMergeFinalDistScaling     = 0.                            ); // in [1/mm]
    declareProperty("improveChi2ProbThreshold",        m_jp.improveChi2ProbThreshold        = 1.e-4                         ); 
    
    // A test implementation for muon vertices
    declareProperty("doSelectTracksFromMuons",         m_jp.doSelectTracksFromMuons         = false                         );
    declareProperty("doSelectTracksFromElectrons",     m_jp.doSelectTracksFromElectrons     = false                         );
    
    // Additional dressing option
    declareProperty("doAugmentDVimpactParametersToMuons",     m_jp.doAugmentDVimpactParametersToMuons     = false           );
    declareProperty("doAugmentDVimpactParametersToElectrons", m_jp.doAugmentDVimpactParametersToElectrons = false           );
    
    // Additional ToolHandles
    declareProperty("VertexFitterTool",                m_fitSvc, " Private TrkVKalVrtFitter"                                );
    declareProperty("Extrapolator",                    m_extrapolator                                                       );
    declareProperty("VertexMapper",                    m_vertexMapper                                                       );
    
  }
  
  
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::processPrimaryVertices() {
    
    //--------------------------------------------------------
    //  Primary vertex extraction from TES
    //
    
    ATH_CHECK( evtStore()->retrieve( m_primaryVertices, "PrimaryVertices") );
    
    if( m_jp.FillNtuple ) m_ntupleVars->get<unsigned int>( "NumPV" ) = 0;
    m_thePV = nullptr;
    
    ATH_MSG_DEBUG( "processPrimaryVertices(): pv_size = " << m_primaryVertices->size() );
    
    // Loop over PV container and get number of tracks of each PV
    
    for( auto *vertex : *m_primaryVertices ) {
      
      // Hide (2015-04-21): xAOD::Vertex may contain several types of vertices
      // e.g. if VertexType==NoVtx, this is a dummy vertex.
      // We only need to extract primary vertices here, and skip otherwise.
      
      if( xAOD::VxType::PriVtx != vertex->vertexType() ) continue;
      
      // Not considering pile-up; pick-up the first PV
      m_thePV = vertex;
      
      if( m_jp.FillNtuple ) {
        
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
      for( auto *vertex : *m_primaryVertices ) {
	if( xAOD::VxType::NoVtx != vertex->vertexType() ) continue;
	
        if( m_jp.FillNtuple ) {
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
      ATH_MSG_DEBUG("No PV is found in the PV collection!");
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG(" Primary vertex successful. thePV = " << m_thePV );
    
    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  void VrtSecInclusive::trackClassification(std::vector<WrkVrt> *workVerticesContainer, std::map<long int, std::vector<long int> >& trackToVertexMap )
  { 
    // Fill TrkInVrt with vertex IDs of each track
    
    trackToVertexMap.clear();
    
    for( size_t iv = 0; iv<workVerticesContainer->size(); iv++ ) {
      
      WrkVrt& vertex = workVerticesContainer->at(iv);
      
      auto& trackIndices = vertex.selectedTrackIndices;
      if( !vertex.isGood ) continue;
      if( trackIndices.size() < 2 ) continue;
      
      for( auto& index : trackIndices ) {
	trackToVertexMap[index].emplace_back( iv );
      }
    }
    
    for( auto& pair: trackToVertexMap ) {
      std::string msg = Form("track index [%ld]: vertices = (", pair.first);
      for( auto& v : pair.second ) {
        msg += Form("%ld, ", v);
      }
      msg += ")";
      if( pair.second.size() >=2 ) ATH_MSG_VERBOSE(" >> " << __FUNCTION__ << ": " << msg );
    }
    
  }
  
  
  //____________________________________________________________________________________________________
  double VrtSecInclusive::findWorstChi2ofMaximallySharedTrack(std::vector<WrkVrt> *workVerticesContainer, 
				      std::map<long int, std::vector<long int> >& trackToVertexMap,
				      long int & maxSharedTrack,
				      long int & worstMatchingVertex)
  {

    double worstChi2 = AlgConsts::invalidFloat;
    
    // Find the track index that has the largest shared vertices
    auto maxSharedTrackToVertices = std::max_element( trackToVertexMap.begin(), trackToVertexMap.end(), []( auto& p1, auto& p2 ) { return p1.second.size() < p2.second.size(); } );
    
    if( maxSharedTrackToVertices == trackToVertexMap.end() ) return worstChi2;
    
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": max-shared track index = " << maxSharedTrackToVertices->first << ", number of shared vertices = " << maxSharedTrackToVertices->second.size() );
    
    if( maxSharedTrackToVertices->second.size() < 2 ) return worstChi2;
    
    // map of vertex index and the chi2 of the track for the maxSharedTrack
    std::map<long int, double> vrtChi2Map;
    
    // loop over vertices for the largest shared track
    for( auto& iv : maxSharedTrackToVertices->second ) {
      ATH_MSG_VERBOSE( " > " << __FUNCTION__ << ": loop over vertices: vertex index " << iv );
      
      auto& wrkvrt = workVerticesContainer->at( iv );
      auto& trackIndices = wrkvrt.selectedTrackIndices;
      
      // find the index of the track
      auto index = std::find_if( trackIndices.begin(), trackIndices.end(), [&]( auto& index ) { return index == maxSharedTrackToVertices->first; } );
      if( index == trackIndices.end() ) {
        ATH_MSG_WARNING(" >> " << __FUNCTION__ << ": index not found (algorithm inconsistent)" );
        return worstChi2;
      }
      
      auto& chi2 = wrkvrt.Chi2PerTrk.at( index - trackIndices.begin() );
      
      vrtChi2Map.emplace( std::pair<long int, double>(iv, chi2) );
    }
    
    auto worstVrtChi2Pair = std::max_element( vrtChi2Map.begin(), vrtChi2Map.end(), []( auto& p1, auto& p2 ) { return p1.second < p2.second; } );
    
    if( worstVrtChi2Pair == vrtChi2Map.end() ) {
      ATH_MSG_WARNING(" >> " << __FUNCTION__ << ": max_element of vrtChi2Map not found" );
      return worstChi2;
    }
    
    maxSharedTrack      = maxSharedTrackToVertices->first;
    worstMatchingVertex = worstVrtChi2Pair->first;
    worstChi2           = worstVrtChi2Pair->second;
    
    return worstChi2;
  }


  //____________________________________________________________________________________________________
  void VrtSecInclusive::printWrkSet(const std::vector<WrkVrt> *workVerticesContainer, const std::string name)
  {
    ATH_MSG_DEBUG( " >> " << __FUNCTION__ << ": " << name << ": #vertices = " << workVerticesContainer->size() );
    
    std::set<long int> usedTracks;
    
    for(size_t iv=0; iv<workVerticesContainer->size(); iv++) {
      auto& wrkvrt = workVerticesContainer->at(iv);
      unsigned ntrk = ( wrkvrt.selectedTrackIndices.size() + wrkvrt.associatedTrackIndices.size() );
      if( ntrk < 2 ) continue;
      if( m_vertexingAlgorithmStep > 0 ) {
        ATH_MSG_DEBUG( " >> " << __FUNCTION__ << ": " << name << " vertex [" <<  iv << "]: "
                       << " isGood  = "            << (wrkvrt.isGood? "true" : "false")
                       << ", #ntrks = "            << ntrk
                       << ", #selectedTracks = "   << wrkvrt.selectedTrackIndices.size()
                       << ", #associatedTracks = " << wrkvrt.associatedTrackIndices.size()
                       << ", chi2/ndof = "         << wrkvrt.Chi2 / ( wrkvrt.ndof() + AlgConsts::infinitesimal )
                       << ", (r, z) = ("           << wrkvrt.vertex.perp()
                       <<", "                      << wrkvrt.vertex.z() << ")" );
      }
      
      for( auto& index : wrkvrt.selectedTrackIndices ) usedTracks.insert( index );
      for( auto& index : wrkvrt.associatedTrackIndices ) usedTracks.insert( index );
    }
    ATH_MSG_DEBUG( " >> " << __FUNCTION__ << ": number of used tracks = " << usedTracks.size() );
    
    std::string msg;
    for( auto& index : usedTracks ) { msg += Form("%ld, ", index); }
    ATH_MSG_DEBUG( " >> " << __FUNCTION__ << ": used tracks = " << msg );
    
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
  VrtSecInclusive::ExtrapolatedPattern* VrtSecInclusive::extrapolatedPattern( const xAOD::TrackParticle* trk ) {
    
    auto* pattern = new ExtrapolatedPattern;
    
    const auto* paramsVector = m_extrapolator->extrapolateBlindly( trk->perigeeParameters(), Trk::alongMomentum );
    
    TVector3 prevPos( AlgConsts::invalidFloat, AlgConsts::invalidFloat, AlgConsts::invalidFloat );
    
    auto nDisabled = 0;
    
    for( auto* params : *paramsVector ) {
      
      const TVector3 position( params->position().x(), params->position().y(), params->position().z() );
      
      if( prevPos == position ) {
        continue;
      }
      
      prevPos = position;
      
      const auto* detElement = params->associatedSurface().associatedDetectorElement();
      
      if( detElement ) {
        
        enum { Pixel = 1, SCT = 2 };
        
        const auto& id = detElement->identify();
        Flag active = false;
        
        if( m_atlasId->is_pixel(id) ) {
          
          auto idHash = m_pixelId->wafer_hash( id );
          active = m_pixelCondSummarySvc->isActive( idHash );
          
          pattern->emplace_back( std::make_tuple( position, Pixel, m_pixelId->barrel_ec(id), m_pixelId->layer_disk(id), active ) );
          
        } else if( m_atlasId->is_sct(id) ) {
          
          auto idHash = m_sctId->wafer_hash( id );
          active = m_sctCondSummarySvc->isActive( idHash );
          
          pattern->emplace_back( std::make_tuple( position, SCT, m_sctId->barrel_ec(id), m_sctId->layer_disk(id), active ) );
          
        }
        
        if( pattern->size() > 0 ) {
          
          ATH_MSG_VERBOSE(" >> " << __FUNCTION__ << ", track " << trk << ": position = (" << position.x() << ", " << position.y() << ", " << position.z() << "), detElement ID = " << id << ", active = " << active
                          << ": (det, bec, layer) = (" << std::get<1>( pattern->back() ) << ", " << std::get<2>( pattern->back() ) << ", "  << std::get<3>( pattern->back() ) << ")" );
          
          if( !active ) nDisabled++;
        }
        
      }
      
    }
    
    if( m_jp.FillHist ) {
      m_hists["disabledCount"]->Fill( nDisabled );
    }

    
    // cleanup
    for( auto* params : *paramsVector ) { delete params; }
    delete paramsVector;
    
    return pattern;

  }
  
  
  //____________________________________________________________________________________________________
  bool VrtSecInclusive::checkTrackHitPatternToVertexByExtrapolation( const xAOD::TrackParticle *trk, const Amg::Vector3D& vertex )
  {
    
    std::unique_ptr<ExtrapolatedPattern> exPattern( extrapolatedPattern( trk ) );
    
    using LayerCombination = std::vector<int>;
    
    static std::map<LayerCombination, unsigned> layerMap;
    if( layerMap.size() == 0 ) {
      layerMap[ { 1, 0, 0 } ] = Trk::pixelBarrel0;
      layerMap[ { 1, 0, 1 } ] = Trk::pixelBarrel1;
      layerMap[ { 1, 0, 2 } ] = Trk::pixelBarrel2;
      layerMap[ { 1, 0, 3 } ] = Trk::pixelBarrel3;
      
      layerMap[ { 1, 2, 0 } ] = Trk::pixelEndCap0;
      layerMap[ { 1, 2, 1 } ] = Trk::pixelEndCap1;
      layerMap[ { 1, 2, 2 } ] = Trk::pixelEndCap2;
      layerMap[ { 1,-2, 0 } ] = Trk::pixelEndCap0;
      layerMap[ { 1,-2, 1 } ] = Trk::pixelEndCap1;
      layerMap[ { 1,-2, 2 } ] = Trk::pixelEndCap2;
      
      layerMap[ { 2, 0, 0 } ] = Trk::sctBarrel0;
      layerMap[ { 2, 0, 1 } ] = Trk::sctBarrel1;
      layerMap[ { 2, 0, 2 } ] = Trk::sctBarrel2;
      layerMap[ { 2, 0, 3 } ] = Trk::sctBarrel3;

      layerMap[ { 2, 2, 0 } ] = Trk::sctEndCap0;
      layerMap[ { 2, 2, 1 } ] = Trk::sctEndCap1;
      layerMap[ { 2, 2, 2 } ] = Trk::sctEndCap2;
      layerMap[ { 2, 2, 3 } ] = Trk::sctEndCap3;
      layerMap[ { 2, 2, 4 } ] = Trk::sctEndCap4;
      layerMap[ { 2, 2, 5 } ] = Trk::sctEndCap5;
      layerMap[ { 2, 2, 6 } ] = Trk::sctEndCap6;
      layerMap[ { 2, 2, 7 } ] = Trk::sctEndCap7;
      layerMap[ { 2, 2, 8 } ] = Trk::sctEndCap8;
      layerMap[ { 2,-2, 0 } ] = Trk::sctEndCap0;
      layerMap[ { 2,-2, 1 } ] = Trk::sctEndCap1;
      layerMap[ { 2,-2, 2 } ] = Trk::sctEndCap2;
      layerMap[ { 2,-2, 3 } ] = Trk::sctEndCap3;
      layerMap[ { 2,-2, 4 } ] = Trk::sctEndCap4;
      layerMap[ { 2,-2, 5 } ] = Trk::sctEndCap5;
      layerMap[ { 2,-2, 6 } ] = Trk::sctEndCap6;
      layerMap[ { 2,-2, 7 } ] = Trk::sctEndCap7;
      layerMap[ { 2,-2, 8 } ] = Trk::sctEndCap8;
    }
    
    enum { position=0, detector=1, bec=2, layer=3, isActive=4 };
    
    // Labmda!
    auto getDetectorType = [&]( const ExtrapolatedPoint& point ) -> unsigned {
      
      const LayerCombination comb { std::get<detector>( point ), std::get<bec>( point ), std::get<layer>( point ) };
      
      for( auto& pair : layerMap ) {
        if( pair.first == comb ) {
          return pair.second;
        }
      }
      
      return AlgConsts::invalidUnsigned;
    };
    
    uint32_t expectedHitPattern { 0 };
    
    // Loop over extrapolated points
    for( auto itr = exPattern->begin(); itr != exPattern->end(); ++itr ) {
      if( std::next( itr ) == exPattern->end() ) continue;
      
      const auto& point      = *itr;
      const auto& nextPoint  = *( std::next( itr ) );
      
      ATH_MSG_VERBOSE( " > " <<  __FUNCTION__ << ": isActive = " << std::get<isActive>( point ) );
      
      // if the front-end module is not active, then the hit is not expected.
      if( false == std::get<isActive>( point ) ) continue;
      
      auto sectionVector = std::get<position>( nextPoint ) - std::get<position>( point );
      auto vertexVector  = TVector3( vertex.x(), vertex.y(), vertex.z() ) - std::get<position>( point );
      
      // if the inner product of the above two vectors is positive,
      // then point is inner than the vertex.
      // Else, the point is outer than the vertex and expect to have hits
      // when the track is originated from the vertex.
      
      if( sectionVector.Mag() == 0. ) continue;
      
      ATH_MSG_VERBOSE( " > " <<  __FUNCTION__
                       << ": sectionVector = (" << sectionVector.x() << ", " << sectionVector.y() << ", " << sectionVector.z() << ")"
                       << ", vertexVector = (" << vertexVector.x() << ", " << vertexVector.y() << ", " << vertexVector.z() << ")"
                       << ", cosTheta( sectionVector, vertexVector)  = " << sectionVector * vertexVector / ( sectionVector.Mag() * vertexVector.Mag() + AlgConsts::infinitesimal ) );
      
      if( sectionVector * vertexVector > 0. ) continue;
      
      // now, the hit is expected to present.
      
      const auto& detectorType = getDetectorType( point );
      
      ATH_MSG_VERBOSE( " > " <<  __FUNCTION__ << ": detType = " << detectorType );
      
      if( detectorType == AlgConsts::invalidUnsigned ) continue;
      
      if( ! (expectedHitPattern & (1 << detectorType)) ) {
        expectedHitPattern += ( 1 << detectorType );
      }
    }
    
    std::string msg = "Expected hit pattern: ";
    for( unsigned i=0; i<Trk::numberOfDetectorTypes; i++) {
      msg += Form("%u", ( expectedHitPattern >> i ) & 1 );
    }
    ATH_MSG_VERBOSE( " > " << __FUNCTION__ << ": " << msg );
    
    msg = "Recorded hit pattern: ";
    for( unsigned i=0; i<Trk::numberOfDetectorTypes; i++) {
      msg += Form("%u", ( trk->hitPattern() >> i ) & 1 );
    }
    ATH_MSG_VERBOSE( " > " << __FUNCTION__ << ": " << msg );
    
    exPattern->clear();
    
    return ( expectedHitPattern == trk->hitPattern() );
  }
    

  //____________________________________________________________________________________________________
  bool VrtSecInclusive::checkTrackHitPatternToVertex( const xAOD::TrackParticle *trk, const Amg::Vector3D& vertex )
  {
    
    const double rad  = vertex.perp();
    const double absz = fabs( vertex.z() );
    
    const uint32_t pattern = trk->hitPattern();
    
	
    if( m_jp.geoModel == VKalVrtAthena::GeoModel::Run2 ) {
    
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
	
	if( ! (pattern & (1<<Trk::pixelBarrel0)) ) return false;
	
	
      } else if( vertex_pattern == insidePixelBarrel0 ) {
	
	if( ! (pattern & (1<<Trk::pixelBarrel0)) ) return false;
      }
      
      
      else if( vertex_pattern == aroundPixelBarrel0 ) {
	
	// require nothing for PixelBarrel0
	if( ! (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      }
      
      
      else if( vertex_pattern == outsidePixelBarrel0_and_insidePixelBarrel1 ) {
	
	if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
	if( ! (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      }
      
      
      else if( vertex_pattern == aroundPixelBarrel1 ) {
	
	if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
	// require nothing for PixelBarrel1
	if( ! (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      }
      
      
      else if( vertex_pattern == outsidePixelBarrel1_and_insidePixelBarrel2 ) {
	
	if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
	if( ! (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      }
      
      
      else if( vertex_pattern == aroundPixelBarrel2 ) {
	
	if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
	// require nothing for PixelBarrel2
	if( ! (pattern & (1<<Trk::pixelBarrel3)) ) return false;
      }
      

      else if( vertex_pattern == outsidePixelBarrel2_and_insidePixelBarrel3 ) {
	
	if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
	if( ! (pattern & (1<<Trk::pixelBarrel3)) ) return false;
      }
	
      else if( vertex_pattern == aroundPixelBarrel3 ) {
	
	if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
	// require nothing for PixelBarrel3
	if( ! (pattern & (1<<Trk::sctBarrel0)) ) return false;
      }
      
      
      else if( vertex_pattern == outsidePixelBarrel3_and_insideSctBarrel0 ) {
	
	if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel3)) ) return false;
	if( ! (pattern & (1<<Trk::sctBarrel0)) ) return false;
      }
      
      
      else if( vertex_pattern == aroundSctBarrel0 ) {
	
	if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel3)) ) return false;
	// require nothing for SctBarrel0
	if( ! (pattern & (1<<Trk::sctBarrel1)) ) return false;
      }
      
      
      else if( vertex_pattern == outsideSctBarrel0_and_insideSctBarrel1 ) {
	
	if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel3)) ) return false;
	if(   (pattern & (1<<Trk::sctBarrel0)) ) return false;
	if( ! (pattern & (1<<Trk::sctBarrel1)) ) return false;
      }
      
      
      else if( vertex_pattern == aroundSctBarrel1 ) {
	if(   (pattern & (1<<Trk::pixelBarrel0)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel3)) ) return false;
	if(   (pattern & (1<<Trk::sctBarrel0)) ) return false;
	// require nothing for SctBarrel1
	if( ! (pattern & (1<<Trk::sctBarrel2)) ) return false;
      }
      //////////////////////////////////////////////////////////////////////////////////
      
      return true;
      
    }
    
    else if ( m_jp.geoModel == VKalVrtAthena::GeoModel::Run1 ) {
      
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
	
	if( ! (pattern & (1<<Trk::pixelBarrel1)) ) return false;
	
      }
      
      
      else if( vertex_pattern == insidePixelBarrel1 ) {
	
	if( ! (pattern & (1<<Trk::pixelBarrel1)) ) return false;
      }
      
      
      else if( vertex_pattern == aroundPixelBarrel1 ) {
	
	// require nothing for PixelBarrel1
	if( ! (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      }
      
      
      else if( vertex_pattern == outsidePixelBarrel1_and_insidePixelBarrel2 ) {
	
	if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
	if( ! (pattern & (1<<Trk::pixelBarrel2)) ) return false;
      }
      
      
      else if( vertex_pattern == aroundPixelBarrel2 ) {
	
	if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
	// require nothing for PixelBarrel2
	if( ! (pattern & (1<<Trk::pixelBarrel3)) ) return false;
      }
      

      else if( vertex_pattern == outsidePixelBarrel2_and_insidePixelBarrel3 ) {
	
	if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
	if( ! (pattern & (1<<Trk::pixelBarrel3)) ) return false;
      }
	
      else if( vertex_pattern == aroundPixelBarrel3 ) {
	
	if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
	// require nothing for PixelBarrel3
	if( ! (pattern & (1<<Trk::sctBarrel0)) ) return false;
      }
      
      
      else if( vertex_pattern == outsidePixelBarrel3_and_insideSctBarrel0 ) {
	
	if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel3)) ) return false;
	if( ! (pattern & (1<<Trk::sctBarrel0)) ) return false;
      }
      
      
      else if( vertex_pattern == aroundSctBarrel0 ) {
	
	if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel3)) ) return false;
	// require nothing for SctBarrel0
	if( ! (pattern & (1<<Trk::sctBarrel1)) ) return false;
      }
      
      
      else if( vertex_pattern == outsideSctBarrel0_and_insideSctBarrel1 ) {
	
	if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel3)) ) return false;
	if(   (pattern & (1<<Trk::sctBarrel0)) ) return false;
	if( ! (pattern & (1<<Trk::sctBarrel1)) ) return false;
      }
      
      
      else if( vertex_pattern == aroundSctBarrel1 ) {
	if(   (pattern & (1<<Trk::pixelBarrel1)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel2)) ) return false;
	if(   (pattern & (1<<Trk::pixelBarrel3)) ) return false;
	if(   (pattern & (1<<Trk::sctBarrel0)) ) return false;
	// require nothing for SctBarrel1
	if( ! (pattern & (1<<Trk::sctBarrel2)) ) return false;
      }
      //////////////////////////////////////////////////////////////////////////////////
      
      return true;
      
    }
    
    return true;
  }
  

  //____________________________________________________________________________________________________
  bool VrtSecInclusive::passedFakeReject( const Amg::Vector3D& FitVertex,
					  const xAOD::TrackParticle *itrk,
					  const xAOD::TrackParticle *jtrk  )
  {
    
    const bool& check_itrk = ( this->*m_patternStrategyFuncs[m_checkPatternStrategy] )( itrk, FitVertex );
    const bool& check_jtrk = ( this->*m_patternStrategyFuncs[m_checkPatternStrategy] )( jtrk, FitVertex );
    
    return ( check_itrk && check_jtrk );
    
  }    
  

  //____________________________________________________________________________________________________
  void VrtSecInclusive::removeInconsistentTracks( WrkVrt& wrkvrt )
  {
    
    const auto& vertex = wrkvrt.vertex;
    
    std::map< std::deque<long int>*, std::vector<const xAOD::TrackParticle*>& > indexMap
    {
      { &(wrkvrt.selectedTrackIndices), *m_selectedTracks }, { &(wrkvrt.associatedTrackIndices), *m_associatedTracks } 
    };
    
    for( auto& pair : indexMap ) {
      
      auto* indices = pair.first;
      auto& tracks  = pair.second;
    
      auto newEnd = std::remove_if( indices->begin(), indices->end(),
                                    [&]( auto& index ) {
                                      bool isConsistent = (this->*m_patternStrategyFuncs[m_checkPatternStrategy] )( tracks.at(index), vertex );
                                      return !isConsistent;
                                    } );
      
      indices->erase( newEnd, indices->end() );
      
    }
    
  }
  

  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::augmentDVimpactParametersToMuons()
  {
    
    const xAOD::VertexContainer *secondaryVertexContainer( nullptr );
    ATH_CHECK( evtStore()->retrieve( secondaryVertexContainer, "VrtSecInclusive_" + m_jp.secondaryVerticesContainerName ) );
    
    const xAOD::MuonContainer *muonContainer( nullptr );
    ATH_CHECK( evtStore()->retrieve( muonContainer, "Muons" ) );
    
    static SG::AuxElement::Decorator< std::vector<float> > decor_d0wrtSV( "d0_wrtSVs" );
    static SG::AuxElement::Decorator< std::vector<float> > decor_z0wrtSV( "z0_wrtSVs" );
    static SG::AuxElement::Decorator< std::vector<ElementLink< xAOD::VertexContainer > > > decor_svLink("svLinks");
    
    for( const auto& muon : *muonContainer ) {
      // Loop over muons
      
      const auto& primaryTrackLink = muon->primaryTrackParticleLink();
      const auto* trk = *primaryTrackLink;
      if( trk ) {
        
        std::map< const xAOD::Vertex*, std::vector<double> > distanceMap;
      
        std::vector<float> d0wrtSV;
        std::vector<float> z0wrtSV;
        std::vector<ElementLink< xAOD::VertexContainer > > links;
      
        for( const auto& vtx : *secondaryVertexContainer ) {
      
          std::vector<double> impactParameters;
          std::vector<double> impactParErrors;
          
          m_fitSvc->VKalGetImpact( trk, vtx->position(), static_cast<int>( muon->charge() ), impactParameters, impactParErrors);
          
          enum { k_d0, k_z0, k_theta, k_phi, k_qOverP };
          
          d0wrtSV.emplace_back( impactParameters.at(k_d0) );
          z0wrtSV.emplace_back( impactParameters.at(k_z0) );
          
          ElementLink<xAOD::VertexContainer> link_SV( *( dynamic_cast<const xAOD::VertexContainer*>( vtx->container() ) ), static_cast<size_t>( vtx->index() ) );
          
          links.emplace_back( link_SV );
          
        }
      
        decor_d0wrtSV( *muon ) = d0wrtSV;
        decor_z0wrtSV( *muon ) = z0wrtSV;
        decor_svLink ( *muon ) = links;
      
      }
    }
    
    return StatusCode::SUCCESS;
  }


  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::augmentDVimpactParametersToElectrons()
  {
    
    const xAOD::VertexContainer *secondaryVertexContainer( nullptr );
    ATH_CHECK( evtStore()->retrieve( secondaryVertexContainer, "VrtSecInclusive_" + m_jp.secondaryVerticesContainerName ) );
    
    const xAOD::ElectronContainer *electronContainer( nullptr );
    ATH_CHECK( evtStore()->retrieve( electronContainer, "Electrons" ) );
    
    static SG::AuxElement::Decorator< std::vector<float> > decor_d0wrtSV( "d0_wrtSVs" );
    static SG::AuxElement::Decorator< std::vector<float> > decor_z0wrtSV( "z0_wrtSVs" );
    static SG::AuxElement::Decorator< std::vector<ElementLink< xAOD::VertexContainer > > > decor_svLink("svLinks");
    
    for( const auto& electron : *electronContainer ) {
      // Loop over electrons
      
      if( 0 == electron->nTrackParticles() ) continue;
      
      // The first track is the best-matched track
      const auto* trk = electron->trackParticle(0);
      if( trk ) {
        
        std::map< const xAOD::Vertex*, std::vector<double> > distanceMap;
      
        std::vector<float> d0wrtSV;
        std::vector<float> z0wrtSV;
        std::vector<ElementLink< xAOD::VertexContainer > > links;
      
        for( const auto& vtx : *secondaryVertexContainer ) {
      
          std::vector<double> impactParameters;
          std::vector<double> impactParErrors;
          
          m_fitSvc->VKalGetImpact( trk, vtx->position(), static_cast<int>( electron->charge() ), impactParameters, impactParErrors);
          
          enum { k_d0, k_z0, k_theta, k_phi, k_qOverP };
          
          d0wrtSV.emplace_back( impactParameters.at(k_d0) );
          z0wrtSV.emplace_back( impactParameters.at(k_z0) );
          
          ElementLink<xAOD::VertexContainer> link_SV( *( dynamic_cast<const xAOD::VertexContainer*>( vtx->container() ) ), static_cast<size_t>( vtx->index() ) );
          
          links.emplace_back( link_SV );
          
        }
      
        decor_d0wrtSV( *electron ) = d0wrtSV;
        decor_z0wrtSV( *electron ) = z0wrtSV;
        decor_svLink ( *electron ) = links;
      
      }
    }
    
    return StatusCode::SUCCESS;
  }


} // end of namespace VKalVrtAthena


