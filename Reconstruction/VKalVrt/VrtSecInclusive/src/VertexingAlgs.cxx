/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecInclusive/VrtSecInclusive.h"
#include "VrtSecInclusive/NtupleVars.h"
#include "VrtSecInclusive/Tools.h"

#include "TrkTrackSummary/TrackSummary.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"


#include "TH1F.h"
#include "TH2F.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TROOT.h"
#include "TLorentzVector.h"

#include <iostream>
#include <algorithm>
#include <array>

//-------------------------------------------------

using namespace std;

namespace Trk {
  extern 
    int  pgraphm_(
        long int *weit, long int *edges, long int *nodes,
        long int *set, long int *nptr,  long int *nth);
}

namespace VKalVrtAthena {


  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::extractIncompatibleTrackPairs( std::vector<WrkVrt>* workVerticesContainer )
  {

    // Output SVs as xAOD::Vertex
    // Needs a conversion function from WrkVrtSet to xAOD::Vertex here.
    // The supposed form of the function will be as follows:
    const xAOD::TrackParticleContainer* trackParticleContainer ( nullptr );
    ATH_CHECK( evtStore()->retrieve( trackParticleContainer, m_jp.TrackLocation) );
   
    xAOD::VertexContainer *twoTrksVertexContainer( nullptr );
    if( m_jp.FillIntermediateVertices ) {
      ATH_CHECK( evtStore()->retrieve( twoTrksVertexContainer, "VrtSecInclusive_" + m_jp.all2trksVerticesContainerName ) );
    }
    
    m_incomp.clear();
   
    // Work variables
    std::vector<const xAOD::TrackParticle*>    baseTracks;
    std::vector<const xAOD::NeutralParticle*>  dummyNeutrals;
   
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": Selected Tracks = "<< m_selectedTracks->size());
    
    std::string msg;

    // first make all 2-track vertices
    for( auto itrk = m_selectedTracks->begin(); itrk != m_selectedTracks->end(); ++itrk ) {
      for( auto jtrk = std::next(itrk); jtrk != m_selectedTracks->end(); ++jtrk ) {

        const int itrk_id = itrk - m_selectedTracks->begin();
        const int jtrk_id = jtrk - m_selectedTracks->begin();
        
        WrkVrt wrkvrt;
        wrkvrt.selectedTrackIndices.emplace_back( itrk_id );
        wrkvrt.selectedTrackIndices.emplace_back( jtrk_id );

        // Attempt to think the combination is incompatible by default
        m_incomp.emplace_back( std::pair<int, int>(itrk_id, jtrk_id) );

        baseTracks.clear();
        baseTracks.emplace_back( *itrk );
        baseTracks.emplace_back( *jtrk );

        // new code to find initial approximate vertex
        Amg::Vector3D IniVertex;

        StatusCode sc = m_fitSvc->VKalVrtFitFast( baseTracks, IniVertex );/* Fast crude estimation */
        if(sc.isFailure()) ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": fast crude estimation fails ");

        m_fitSvc->setApproximateVertex( IniVertex.x(), IniVertex.y(), IniVertex.z() );
        
        if( m_jp.FillHist ) m_hists["incompMonitor"]->Fill( 0 );
        
        // Vertex VKal Fitting
        sc = m_fitSvc->VKalVrtFit( baseTracks,
                                   dummyNeutrals,
                                   wrkvrt.vertex, wrkvrt.vertexMom, wrkvrt.Charge,
                                   wrkvrt.vertexCov, wrkvrt.Chi2PerTrk,
                                   wrkvrt.TrkAtVrt, wrkvrt.Chi2  );
        
        if( sc.isFailure() ) {
          if( m_jp.FillHist ) m_hists["incompMonitor"]->Fill( 1 );
          continue;          /* No fit */ 
        }
        
        // Compatibility to the primary vertex.
        Amg::Vector3D vDist = wrkvrt.vertex - m_thePV->position();
        double vPos = ( vDist.x()*wrkvrt.vertexMom.Px()+vDist.y()*wrkvrt.vertexMom.Py()+vDist.z()*wrkvrt.vertexMom.Pz() )/wrkvrt.vertexMom.Rho();
        
        if( m_jp.FillNtuple ) {
          // Fill the 2-track vertex properties to AANT
          m_ntupleVars->get<unsigned int>( "All2TrkVrtNum" )++;
          m_ntupleVars->get< std::vector<double> >( "All2TrkVrtMass" )   .emplace_back(wrkvrt.vertexMom.M());
          m_ntupleVars->get< std::vector<double> >( "All2TrkVrtPt" )     .emplace_back(wrkvrt.vertexMom.Perp());
          m_ntupleVars->get< std::vector<int> >   ( "All2TrkVrtCharge" ) .emplace_back(wrkvrt.Charge);
          m_ntupleVars->get< std::vector<double> >( "All2TrkVrtX" )      .emplace_back(wrkvrt.vertex.x());
          m_ntupleVars->get< std::vector<double> >( "All2TrkVrtY" )      .emplace_back(wrkvrt.vertex.y());
          m_ntupleVars->get< std::vector<double> >( "All2TrkVrtZ" )      .emplace_back(wrkvrt.vertex.z());
          m_ntupleVars->get< std::vector<double> >( "All2TrkVrtChiSq" )  .emplace_back(wrkvrt.Chi2);
        }


        // Create a xAOD::Vertex instance
        xAOD::Vertex *vertex { nullptr };
        
        if( m_jp.FillIntermediateVertices ) {
          vertex = new xAOD::Vertex;
          twoTrksVertexContainer->emplace_back( vertex );

          for( auto *trk: baseTracks ) {

            // Acquire link to the track
            ElementLink<xAOD::TrackParticleContainer>  trackElementLink( *( dynamic_cast<const xAOD::TrackParticleContainer*>( trk->container() ) ), trk->index() );

            // Register link to the vertex
            vertex->addTrackAtVertex( trackElementLink, 1. );
          }

          vertex->setVertexType( xAOD::VxType::SecVtx );
          vertex->setPosition( wrkvrt.vertex );
          vertex->setFitQuality( wrkvrt.Chi2, 1 ); // Ndof is always 1

          vertex->auxdata<float>("mass")   = wrkvrt.vertexMom.M();
          vertex->auxdata<float>("pT")     = wrkvrt.vertexMom.Perp();
          vertex->auxdata<float>("charge") = wrkvrt.Charge;
          vertex->auxdata<float>("vPos")   = vPos;
          vertex->auxdata<bool>("isFake")  = true;
        }


        /////////////////////////////

        uint8_t trkiBLHit,trkjBLHit;
        if( !((*itrk)->summaryValue( trkiBLHit,xAOD::numberOfInnermostPixelLayerHits)))  trkiBLHit=0;
        if( !((*jtrk)->summaryValue( trkjBLHit,xAOD::numberOfInnermostPixelLayerHits)))  trkjBLHit=0;

        if( m_jp.FillNtuple ) m_ntupleVars->get< std::vector<int> >( "All2TrkSumBLHits" ).emplace_back( trkiBLHit + trkjBLHit );

        // track chi2 cut
        if( m_jp.FillHist ) m_hists["2trkChi2Dist"]->Fill( log10( wrkvrt.Chi2 ) );
        
        if( wrkvrt.Chi2 > m_jp.SelVrtChi2Cut) {
          if( m_jp.FillHist ) m_hists["incompMonitor"]->Fill( 2 );
          continue;          /* Bad Chi2 */
        }
        ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": 2-track vrt mass/pt "<<wrkvrt.vertexMom.M()<<","<<wrkvrt.vertexMom.Perp());


        // fake rejection cuts with track hit pattern consistencies
        if( m_jp.removeFakeVrt && !m_jp.removeFakeVrtLate ) {
          if( !this->passedFakeReject( wrkvrt.vertex, (*itrk), (*jtrk) ) ) {
            if( m_jp.FillHist ) m_hists["incompMonitor"]->Fill( 3 );
            continue;
          }
        }
        
        if( m_jp.FillHist ) dynamic_cast<TH2F*>( m_hists["vPosDist"] )->Fill( wrkvrt.vertex.perp(), vPos );
        
        if( m_jp.FillNtuple ) {
          // Fill AANT for vertices after fake rejection
          m_ntupleVars->get< unsigned int >( "AfFakVrtNum" )++;
          m_ntupleVars->get< std::vector<double> >( "AfFakVrtMass" )   .emplace_back(wrkvrt.vertexMom.M());
          m_ntupleVars->get< std::vector<double> >( "AfFakVrtPt" )     .emplace_back(wrkvrt.vertexMom.Perp());
          m_ntupleVars->get< std::vector<int> >   ( "AfFakVrtCharge" ) .emplace_back(wrkvrt.Charge);
          m_ntupleVars->get< std::vector<double> >( "AfFakVrtX" )      .emplace_back(wrkvrt.vertex.x());
          m_ntupleVars->get< std::vector<double> >( "AfFakVrtY" )      .emplace_back(wrkvrt.vertex.y());
          m_ntupleVars->get< std::vector<double> >( "AfFakVrtZ" )      .emplace_back(wrkvrt.vertex.z());
          m_ntupleVars->get< std::vector<double> >( "AfFakVrtChiSq" )  .emplace_back(wrkvrt.Chi2);
        }

        // The vertex passed the quality cut: overwrite isFake to false
        if( m_jp.FillIntermediateVertices && vertex ) {
          vertex->auxdata<bool>("isFake")  = false;
        }

        
        if( m_jp.doPVcompatibilityCut ) {
          if( vPos < m_jp.pvCompatibilityCut ) {
            if( m_jp.FillHist ) m_hists["incompMonitor"]->Fill( 3 );
            continue;
          }
        }

        // Now this vertex passed all criteria and considred to be a compatible vertices.
        // Therefore the track pair is removed from the incompatibility list.
        m_incomp.pop_back();
        
        if( m_jp.FillIntermediateVertices ) {
          wrkvrt.isGood = true;
          workVerticesContainer->emplace_back( wrkvrt );
        }
        
        msg += Form(" (%d, %d), ", itrk_id, jtrk_id );
      }
    }
    
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": compatible track pairs = " << msg );
    
    if( m_jp.FillNtuple ) m_ntupleVars->get<unsigned int>( "SizeIncomp" ) = m_incomp.size();
    
    return StatusCode::SUCCESS;
  }


  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::findNtrackVertices( std::vector<WrkVrt> *workVerticesContainer )
  {
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": begin");
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": incompatible track pair size = " << m_incomp.size() );
    
    
    if( m_incomp.size() < 500 ) {
      
      ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": incompatibility graph finder mode" );
      
      // clear the container
      workVerticesContainer->clear();

      // Graph method: Trk::pgraphm_()
      // used in order to find compatible sub-graphs from the incompatible graph

      // List of edgeds between imcompatible nodes
      // This weit is the data model of imcompatible graph used in Trk::pgraphm_().
      std::vector<long int> weit;
    
      for( auto& pair : m_incomp ) {
        weit.emplace_back( pair.first  + 1 ); /* +1 is needed for PGRAPH due to FORTRAN-style counting */
        weit.emplace_back( pair.second + 1 ); /* +1 is needed for PGRAPH due to FORTRAN-style counting */
      }

      // Solution of the graph method routine (minimal covering of the graph)
      // The size of the solution is returned by NPTR (see below)
      std::vector<long int> solution( m_selectedTracks->size() );

      // Number of edges in the list is the size of incompatibility track pairs.
      long int nEdges = m_incomp.size();

      // input number of nodes in the graph.
      long int nTracks = static_cast<long int>( m_selectedTracks->size() );

      // Input variable; the threshold. Solutions shorter than nth are not returned (ignored).
      long int nth = 2;    //VK some speed up

      // NPTR: I/O variable (Destructive FORTRAN Style!!!)
      // - on input:   =0 for initialization, >0 to get next solution
      // - on output:  >0 : length of the solution stored in set; =0 : no more solutions can be found
      long int solutionSize { 0 };

      // This is just a unused strawman needed for m_fitSvc->VKalVrtFit()
      std::vector<const xAOD::TrackParticle*>    baseTracks;
      std::vector<const xAOD::NeutralParticle*>  dummyNeutrals;

      m_fitSvc->setDefault();

      // Main iteration
      while(true) {

        // Find a solution from the given set of incompatible tracks (==weit)
        Trk::pgraphm_( weit.data(), &nEdges, &nTracks, solution.data(), &solutionSize, &nth);

        ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": Trk::pgraphm_() output: solutionSize = " << solutionSize );

        if(solutionSize <= 0)  break;      // No more solutions ==> Exit
        if(solutionSize == 1)  continue;   // i.e. single node  ==> Not a good solution

        baseTracks.clear();

        std::string msg = "solution = [ ";
        for( int i=0; i< solutionSize; i++) {
          msg += Form( "%ld, ", solution[i] );
        }
        msg += " ]";
        ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": " << msg );
      
        // varaible of new vertex
        WrkVrt wrkvrt;

        // Try to compose a new vertex using the solution nodes
        // Here the track ID is labelled with array
        wrkvrt.isGood = true;
        wrkvrt.selectedTrackIndices.clear();
      
        for(long int i = 0; i<solutionSize; i++) {
          wrkvrt.selectedTrackIndices.emplace_back(solution[i]-1);
          baseTracks.emplace_back( m_selectedTracks->at(solution[i]-1) );
        }

        // Perform vertex fitting
        StatusCode sc =m_fitSvc->VKalVrtFit(baseTracks, dummyNeutrals,
                                            wrkvrt.vertex,
                                            wrkvrt.vertexMom,
                                            wrkvrt.Charge,
                                            wrkvrt.vertexCov,
                                            wrkvrt.Chi2PerTrk, 
                                            wrkvrt.TrkAtVrt,
                                            wrkvrt.Chi2);

        ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": FoundAppVrt=" << solutionSize << ", (r, z) = " << wrkvrt.vertex.perp() << ", " << wrkvrt.vertex.z()  <<  ", chi2 = "  <<  wrkvrt.Chi2);

        if( sc.isFailure() )  {
          ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": VKalVrtFit failed; continue.");
          continue;   /* Bad fit - goto next solution */
        }

        ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": VKalVrtFit succeeded; register the vertex to the list.");
        wrkvrt.isGood                = true;
        wrkvrt.closestWrkVrtIndex    = AlgConsts::invalidUnsigned;
        wrkvrt.closestWrkVrtValue    = AlgConsts::maxValue;
        workVerticesContainer->emplace_back( wrkvrt );

      } 
      
      
    } else {
    
      ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": rapid finder mode" );
      
      struct Cluster {
        Amg::Vector3D position;
        std::set<long int> tracks;
      };
      
      std::vector<struct Cluster> clusters;
      
      for( auto& wrkvrt : *workVerticesContainer ) {
        
        bool foundCluster = false;
        
        for( auto& cluster: clusters ) {
          if( (wrkvrt.vertex - cluster.position).norm() < 1.0 ) {
            for( auto& itrk : wrkvrt.selectedTrackIndices ) {
              cluster.tracks.insert( itrk );
            }
            foundCluster = true;
            break;
          }
        }
        
        if( !foundCluster ) {
          Cluster c;
          c.position = wrkvrt.vertex;
          for( auto& itrk : wrkvrt.selectedTrackIndices ) {
            c.tracks.insert( itrk );
          }
          clusters.emplace_back( c );
          ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": added a new cluster" );
        }
        
      }
      
      // This is just a unused strawman needed for m_fitSvc->VKalVrtFit()
      std::vector<const xAOD::TrackParticle*>    baseTracks;
      std::vector<const xAOD::NeutralParticle*>  dummyNeutrals;

      workVerticesContainer->clear();
      
      ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": found cluster size =" << clusters.size() );
      
      for( auto& cluster : clusters ) {
        
        // varaible of new vertex
        WrkVrt wrkvrt;

        // Try to compose a new vertex using the solution nodes
        // Here the track ID is labelled with array
        wrkvrt.isGood = true;
        wrkvrt.selectedTrackIndices.clear();
      
        for(auto& index: cluster.tracks) {
          wrkvrt.selectedTrackIndices.emplace_back( index );
          baseTracks.emplace_back( m_selectedTracks->at( index ) );
        }
        
        m_fitSvc->setDefault();

        m_fitSvc->setApproximateVertex( cluster.position.x(), cluster.position.y(), cluster.position.z() );

        // Perform vertex fitting
        StatusCode sc = m_fitSvc->VKalVrtFit(baseTracks, dummyNeutrals,
                                             wrkvrt.vertex,
                                             wrkvrt.vertexMom,
                                             wrkvrt.Charge,
                                             wrkvrt.vertexCov,
                                             wrkvrt.Chi2PerTrk, 
                                             wrkvrt.TrkAtVrt,
                                             wrkvrt.Chi2);
        
        if( sc.isFailure() ) {
          
          wrkvrt.selectedTrackIndices.clear();
          baseTracks.clear();
          
          for(auto& index: cluster.tracks) {
            wrkvrt.selectedTrackIndices.emplace_back( index );
            baseTracks.emplace_back( m_selectedTracks->at( index ) );
            
            if( wrkvrt.nTracksTotal() < 2 ) continue;
            
            m_fitSvc->setDefault();
            
            m_fitSvc->setApproximateVertex( cluster.position.x(), cluster.position.y(), cluster.position.z() );
            
            // Perform vertex fitting
            sc = m_fitSvc->VKalVrtFit(baseTracks, dummyNeutrals,
                                      wrkvrt.vertex,
                                      wrkvrt.vertexMom,
                                      wrkvrt.Charge,
                                      wrkvrt.vertexCov,
                                      wrkvrt.Chi2PerTrk, 
                                      wrkvrt.TrkAtVrt,
                                      wrkvrt.Chi2);
            
            if( sc.isFailure() ) {
              wrkvrt.selectedTrackIndices.pop_back();
              baseTracks.pop_back();
            }
            
          }
        }
        
        workVerticesContainer->emplace_back( wrkvrt );
      }
      
    }

    //-------------------------------------------------------
    // Iterative cleanup algorithm

    //-Remove worst track from vertices with very bad Chi2
    /*
    ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": Remove worst track from vertices with very bad Chi2.");
    for(size_t iv=0; iv<workVerticesContainer->size(); iv++) {
      auto& wrkvrt = workVerticesContainer->at(iv);
        
      if( wrkvrt.Chi2 > (4.*wrkvrt.selectedTrackIndices.size()) ) {
        StatusCode sc = disassembleVertex( workVerticesContainer, iv );
        if( sc.isFailure() ) {}
      }
        
    }
    */

    //-Remove vertices fully contained in other vertices 
    ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": Remove vertices fully contained in other vertices .");
    while( workVerticesContainer->size() > 1 ) {
      size_t tmpN = workVerticesContainer->size();

      size_t iv = 0;
      for(; iv<tmpN-1; iv++) {
        size_t jv = iv+1;
        for(; jv<tmpN; jv++) {
          const auto nTCom = nTrkCommon( workVerticesContainer, {iv, jv} );

          if(      nTCom == workVerticesContainer->at(iv).selectedTrackIndices.size() ) {  workVerticesContainer->erase(workVerticesContainer->begin()+iv); break; }
          else if( nTCom == workVerticesContainer->at(jv).selectedTrackIndices.size() ) {  workVerticesContainer->erase(workVerticesContainer->begin()+jv); break; }

        }
        if(jv!=tmpN)   break;  // One vertex is erased. Restart check
      }
      if(iv==tmpN-1) break;  // No vertex deleted
    }

    //-Identify remaining 2-track vertices with very bad Chi2 and mass (b-tagging)
    ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": Identify remaining 2-track vertices with very bad Chi2 and mass (b-tagging).");
    for( auto& wrkvrt : *workVerticesContainer ) {
      if( wrkvrt.selectedTrackIndices.size() != 2 ) continue;
      if( m_jp.FillHist ) m_hists["NtrkChi2Dist"]->Fill( log10( wrkvrt.Chi2/wrkvrt.ndof() ) );
      //if( wrkvrt.Chi2/wrkvrt.ndof() > 100.) wrkvrt.isGood=false;
    }

    //-Remove all bad vertices from the working set    
    ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": Remove all bad vertices from the working set.");
    { 
      auto end = std::remove_if( workVerticesContainer->begin(), workVerticesContainer->end(), []( WrkVrt& wrkvrt ) { return wrkvrt.isGood == false || wrkvrt.selectedTrackIndices.size() < 2; } );
      workVerticesContainer->erase( end, workVerticesContainer->end() );
    }
    
    if( m_jp.FillNtuple) m_ntupleVars->get<unsigned int>( "NumInitSecVrt" ) = workVerticesContainer->size();

    return StatusCode::SUCCESS;
  }


  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::rearrangeTracks( std::vector<WrkVrt> *workVerticesContainer )
  {
    
    //
    //  Rearrangement of solutions
    //
    
    std::vector<long int> processedTracks;

    while( true ) {
      
      // worstChi2: unit in [chi2 per track]
      long int maxSharedTrack;
      long int worstMatchingVertex;
      std::pair<unsigned, unsigned> indexPair { AlgConsts::invalidUnsigned, AlgConsts::invalidUnsigned };
    
    
      // trackToVertexMap has IDs of each track which can contain array of vertices.
      // e.g. TrkInVrt->at( track_id ).size() gives the number of vertices which use the track [track_id].
      //using trkitr = xAOD::TrackParticleContainer::const_iterator;
      //using vtxitr = std::vector<WrkVrt>::const_iterator;
      
      std::map<long int, std::vector<long int> > trackToVertexMap;
      
      // Fill trackToVertexMap with vertex IDs of each track
      trackClassification( workVerticesContainer, trackToVertexMap );
      
      // Remove already processed tracks
      for( auto& pt : processedTracks ) {
        if( trackToVertexMap.find( pt ) != trackToVertexMap.end() ) {
          trackToVertexMap.erase( pt );
        }
      }
          
    
      auto worstChi2 = findWorstChi2ofMaximallySharedTrack( workVerticesContainer, trackToVertexMap, maxSharedTrack, worstMatchingVertex );
      
      if( worstChi2 == AlgConsts::invalidFloat ) {
        ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": no shared tracks are found --> exit the while loop." );
        break;
      }
      
      ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": maximally shared track index = " << maxSharedTrack
                     << ", multiplicity = " << trackToVertexMap.at( maxSharedTrack ).size()
                     << ", workst chi2 = " << worstChi2
                     << ", vertex index = " << worstMatchingVertex );
            
      //Choice of action
      if( worstChi2 < m_jp.TrackDetachCut ) {
        
        // Here, the max-shared track is well-associated and cannot be detached.
        // The closest vertex should be merged.
        
        std::vector< std::pair<unsigned, unsigned> > badPairs;
        
        while( true ) {
          
          // find the closest vertices pair that share the track of interest
          double minSignificance { AlgConsts::maxValue };
          unsigned nShared { 0 };
        
          {
            auto& vrtList = trackToVertexMap.at( maxSharedTrack );
            
            auto nGood = std::count_if( vrtList.begin(), vrtList.end(), [&]( auto& v ) { return workVerticesContainer->at(v).isGood; } );
            ATH_MSG_VERBOSE( " > " << __FUNCTION__ << ": size of good vertices = " << nGood );
          
            std::vector< std::tuple< std::pair<unsigned, unsigned>, double, unsigned> > significanceTuple;
            enum { kIndexPair, kSignificance, kNshared };
          
            for( auto ivrt = vrtList.begin(); ivrt != vrtList.end(); ++ivrt ) {
              for( auto jvrt = std::next( ivrt ); jvrt != vrtList.end(); ++jvrt ) {
                auto pair = std::pair<unsigned, unsigned>( *ivrt, *jvrt );
                
                if( !( workVerticesContainer->at(*ivrt).isGood ) ) continue;
                if( !( workVerticesContainer->at(*jvrt).isGood ) ) continue;
                
                // skip known bad pairs
                if( std::find( badPairs.begin(), badPairs.end(), pair ) != badPairs.end() ) continue;
                
                auto signif = significanceBetweenVertices( workVerticesContainer->at( *ivrt ), workVerticesContainer->at( *jvrt ) );
                
                auto& ivrtTrks = workVerticesContainer->at(*ivrt).selectedTrackIndices;
                auto& jvrtTrks = workVerticesContainer->at(*jvrt).selectedTrackIndices;
                
                auto nSharedTracks = std::count_if( ivrtTrks.begin(), ivrtTrks.end(),
                                                    [&]( auto& index ) {
                                                      return std::find( jvrtTrks.begin(), jvrtTrks.end(), index ) != jvrtTrks.end();
                                                    } );
                
                significanceTuple.emplace_back( std::tuple< std::pair<unsigned, unsigned>, double, unsigned>( pair, signif, nSharedTracks ) );
              }
            }
            
            if( significanceTuple.size() == 0 ) {
              ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": no vertex pairs are found --> exit the while loop." );
              break;
            }
          
            auto minSignificanceTuple = std::min_element( significanceTuple.begin(), significanceTuple.end(), [&]( auto& t1, auto&t2 ) { return std::get<kSignificance>(t1) < std::get<kSignificance>(t2); } );
        
            indexPair       = std::get<kIndexPair>    ( *minSignificanceTuple );
            minSignificance = std::get<kSignificance> ( *minSignificanceTuple );
            nShared         = std::get<kNshared>      ( *minSignificanceTuple );
          }
          
          ATH_MSG_VERBOSE( " > " << __FUNCTION__ << ": minSignificance = " << minSignificance );
        
          if( minSignificance < m_jp.VertexMergeCut || nShared >= 2 ) {
            
            WrkVrt vertex_backup1 = workVerticesContainer->at( indexPair.first );
            WrkVrt vertex_backup2 = workVerticesContainer->at( indexPair.second );
          
            StatusCode sc = mergeVertices( workVerticesContainer->at( indexPair.first ), workVerticesContainer->at( indexPair.second ) );
          
            if( m_jp.FillHist ) { m_hists["mergeType"]->Fill( RECONSTRUCT_NTRK ); }
          
            if( sc.isFailure() ) {
              // revert to the original
              workVerticesContainer->at( indexPair.first  ) = vertex_backup1;
              workVerticesContainer->at( indexPair.second ) = vertex_backup2;
              badPairs.emplace_back( indexPair );
            }
          
            // The second vertex is merged to the first.
            // Explicity flag the second vertex is invalid.
            workVerticesContainer->at( indexPair.second ).isGood = false;
            
            // Now the vertex is merged and the bad pair record is outdated.
            badPairs.clear();
            
            ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": Merged vertices " << indexPair.first << " and " << indexPair.second << ". merged vertex multiplicity = " << workVerticesContainer->at( indexPair.first ).selectedTrackIndices.size() );
          } else {
            break;
          }
        }
        
        processedTracks.emplace_back( maxSharedTrack );
        
      } else {
        
        // Here, a bad track association is detected
        // The track is detached from the worst-associated vertex and refit.
        
        auto& wrkvrt = workVerticesContainer->at( worstMatchingVertex );
        
        auto end = std::remove_if( wrkvrt.selectedTrackIndices.begin(), wrkvrt.selectedTrackIndices.end(), [&]( auto& index ) { return index == maxSharedTrack; } );
        wrkvrt.selectedTrackIndices.erase( end, wrkvrt.selectedTrackIndices.end() );
        
        auto wrkvrt_backup = wrkvrt;
        StatusCode sc = refitVertex( wrkvrt );
        if( sc.isFailure() ) {
          wrkvrt = wrkvrt_backup;
        }
        
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": removed track " << maxSharedTrack << " from vertex " << worstMatchingVertex );
      }
      
    }

    //
    // Try to improve vertices with big Chi2
    for(int iv=0; iv<(int)workVerticesContainer->size(); iv++) {
      
      auto& vertex = workVerticesContainer->at(iv);

      if(!vertex.isGood )                 continue;  //don't work on vertex which is already bad
      if( vertex.selectedTrackIndices.size() < 3 )      continue;

      double chi2Probability = TMath::Prob( vertex.Chi2, vertex.ndof() ); //Chi2 of the original vertex

      if( chi2Probability < AlgConsts::minVertexChi2Probability ) {
        improveVertexChi2( workVerticesContainer->at(iv) );
      }

    }
    //
    //-----------------------------------------------------------------------------------------------
    //
    if( m_jp.FillNtuple ) {
      m_ntupleVars->get<unsigned int>( "NumRearrSecVrt" )=workVerticesContainer->size();
      ATH_MSG_DEBUG(" > rearrangeTracks: Size of Solution Set: "<< m_ntupleVars->get<unsigned int>( "NumRearrSecVrt" ));
    }

    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::reassembleVertices( std::vector<WrkVrt>* workVerticesContainer )
  {
    // Here, the supposed issue is that, the position of the reconstructed vertex may be significantly
    // displaced from its truth position, even if the constituent tracks are all from that truth.
    // The fundamental reason of this is speculated that the VKalVrt vertex fitting could fall in
    // a local minimum. This function attempts to improve the situation, given that N-track vertices
    // are already reconstructed, by attempting to asociate a track of a small multiplicity vertex
    // to another large multiplicity vertex.
    
    // First, sort WrkVrt by the track multiplicity
    std::sort( workVerticesContainer->begin(), workVerticesContainer->end(), [](WrkVrt& v1, WrkVrt& v2) { return v1.selectedTrackIndices.size() < v2.selectedTrackIndices.size(); } );
    
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": #vertices = " << workVerticesContainer->size() );
    // Loop over vertices (small -> large Ntrk order)
    for( auto& workVertex : *workVerticesContainer ) {
      if( !workVertex.isGood               ) continue;
      if(  workVertex.selectedTrackIndices.size() <= 1 ) continue;
      
      ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": vertex " << &workVertex << " #tracks = " << workVertex.selectedTrackIndices.size() );
      std::map<unsigned, std::vector<WrkVrt>::reverse_iterator> mergiableVertex;
      std::set<std::vector<WrkVrt>::reverse_iterator> mergiableVerticesSet;
      
      for( auto& index : workVertex.selectedTrackIndices ) {
        
        const xAOD::TrackParticle* trk = m_selectedTracks->at( index );
        
        mergiableVertex[index] = workVerticesContainer->rend();
        
        std::vector<double> distances;
        
        // Reverse iteration: large Ntrk -> small Ntrk order
        for( auto ritr = workVerticesContainer->rbegin(); ritr != workVerticesContainer->rend(); ++ritr ) {
          auto& targetVertex = *ritr;
          
          if( &workVertex == &targetVertex ) continue;
          if( workVertex.selectedTrackIndices.size() >= targetVertex.selectedTrackIndices.size() ) continue;
          if( ! ( this->*m_patternStrategyFuncs[m_checkPatternStrategy] )( trk, targetVertex.vertex ) ) continue;
          
          // Get the closest approach
          std::vector<double> impactParameters;
          std::vector<double> impactParErrors;
        
          m_fitSvc->VKalGetImpact(trk, targetVertex.vertex, static_cast<int>( trk->charge() ), impactParameters, impactParErrors);
          
          enum { k_d0, k_z0, k_theta, k_phi, k_qOverP };
          
          const auto& distance = hypot( impactParameters.at(k_d0), impactParameters.at(k_z0) );
          distances.emplace_back( distance );
          
          if( fabs( impactParameters.at(k_d0) > m_jp.reassembleMaxImpactParameterD0 ) ) continue;
          if( fabs( impactParameters.at(k_z0) > m_jp.reassembleMaxImpactParameterZ0 ) ) continue;
          
          mergiableVertex[index] = ritr;
          mergiableVerticesSet.emplace( ritr );
          
        }
        
        auto min_distance = distances.size() > 0 ? *(std::min_element( distances.begin(), distances.end() )) : AlgConsts::invalidFloat;
        
        if( mergiableVertex[index] == workVerticesContainer->rend() ) {
          ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": track " << trk << " --> none : min distance = " << min_distance );
        } else {
          ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": track " << trk << " --> " << &( *(mergiableVertex[index]) ) << " --> size = " << mergiableVertex[index]->selectedTrackIndices.size() << ": min distance = " << min_distance );
        }
        
      }
      
      size_t count_mergiable = std::count_if( mergiableVertex.begin(), mergiableVertex.end(),
                                              [&](std::pair<unsigned, std::vector<WrkVrt>::reverse_iterator> p ) {
                                                return p.second != workVerticesContainer->rend(); } );
      
      if( mergiableVerticesSet.size() == 1 && count_mergiable == workVertex.selectedTrackIndices.size() ) {
        
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": identified a unique association destination vertex" );
        
        WrkVrt& destination = *( mergiableVertex.begin()->second );
        ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": destination #tracks before merging = " << destination.selectedTrackIndices.size() );
        
        StatusCode sc = mergeVertices( destination, workVertex );
        if( sc.isFailure() ) {}
        
        if( m_jp.FillHist ) { m_hists["mergeType"]->Fill( REASSEMBLE ); }
        
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": destination #tracks after merging = " << destination.selectedTrackIndices.size() );
        
      }
          
    }
    
    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::associateNonSelectedTracks( std::vector<WrkVrt>* workVerticesContainer )
  {
    
    const xAOD::TrackParticleContainer *allTracks ( nullptr );
    ATH_CHECK( evtStore()->retrieve(allTracks, "InDetTrackParticles") );
    
    const xAOD::VertexContainer *pvs (nullptr);
    ATH_CHECK( evtStore()->retrieve( pvs, "PrimaryVertices") );
    
    static SG::AuxElement::Decorator<char> decor_isAssociated( "is_associated" );
    
    ATH_MSG_DEBUG( " >> " << __FUNCTION__ << ": #verticess = " << workVerticesContainer->size() );
    
    // Loop over vertices
    for( auto& workVertex : *workVerticesContainer ) {
      
      if( !workVertex.isGood               ) continue;
      if(  workVertex.selectedTrackIndices.size() <= 1 ) continue;
      
      auto& vertexPos = workVertex.vertex;
      
      std::vector<double> distanceToPVs;
      
      for( auto* pv : *pvs ) {
        distanceToPVs.emplace_back( VKalVrtAthena::vtxVtxDistance( vertexPos, pv->position() ) );
      }
      const auto& minDistance = *( std::min_element( distanceToPVs.begin(), distanceToPVs.end() ) );
      
      if( minDistance < m_jp.associateMinDistanceToPV ) continue;
      
      
      ATH_MSG_DEBUG( " >> " << __FUNCTION__ << ": vertex pos = (" << vertexPos.x() << ", " << vertexPos.y() << ", " << vertexPos.z() << "), "
                     "#selected = " << workVertex.selectedTrackIndices.size() << ", #assoc = " << workVertex.associatedTrackIndices.size() );
      
      std::vector<const xAOD::TrackParticle*> candidates;
      
      // Search for candidate tracks
      for( auto itr = allTracks->begin(); itr != allTracks->end(); ++itr ) {
        auto* trk = *itr;
        
        // If the track is already used for any DV candidate, reject.
        {
          auto result = std::find_if( workVerticesContainer->begin(), workVerticesContainer->end(),
                                      [&] ( WrkVrt& wrkvrt ) {
                                        auto found = std::find_if( wrkvrt.selectedTrackIndices.begin(), wrkvrt.selectedTrackIndices.end(),
                                                                   [&]( long int index ) {
                                                                     return trk == m_selectedTracks->at(index);
                                                                   } );
                                        return found != wrkvrt.selectedTrackIndices.end();
                                      } );
          if( result != workVerticesContainer->end() ) continue;
        }
        
        // If the track is already registered to the associated track list, rejct.
        {
          auto result = std::find_if( m_associatedTracks->begin(), m_associatedTracks->end(),
                                      [&] (const auto* atrk) { return trk == atrk; } );
          if( result != m_associatedTracks->end() ) continue;
        }
        
        // pT selection
        if( trk->pt() < m_jp.associatePtCut ) continue;
        
        // chi2 selection
        if( trk->chiSquared() / trk->numberDoF() > m_jp.associateChi2Cut ) continue;
        
        // Get the closest approach
        std::vector<double> impactParameters;
        std::vector<double> impactParErrors;
        
        m_fitSvc->VKalGetImpact(trk, vertexPos, static_cast<int>( trk->charge() ), impactParameters, impactParErrors);
        
        enum { k_d0, k_z0, k_theta, k_phi, k_qOverP };
        
        if( fabs( impactParameters.at(k_d0) ) > m_jp.associateMaxD0 ) continue;
        if( fabs( impactParameters.at(k_z0) ) > m_jp.associateMaxZ0 ) continue;
        
        // Hit pattern consistentcy requirement
        if( ! ( this->*m_patternStrategyFuncs[m_checkPatternStrategy] )( trk, vertexPos ) ) continue;
        
        ATH_MSG_DEBUG( " >> " << __FUNCTION__ << ": trk " << trk
                       << ": d0 to vtx = " << impactParameters.at(k_d0)
                       << ", z0 to vtx = " << impactParameters.at(k_z0)
                       << ", distance to vtx = " << hypot( impactParameters.at(k_d0), impactParameters.at(k_z0) ) );
        
        candidates.emplace_back( trk );
        
      }
      
      ATH_MSG_DEBUG( " >> " << __FUNCTION__ << ": number of candidate tracks = " << candidates.size() );
      
      // Attempt to add the track to the vertex and try fitting
      for( const auto* trk : candidates ) {
        
        ATH_MSG_DEBUG( " >> " << __FUNCTION__ << ": attempting to associate track = " << trk );
        
        // Backup the current vertes status
        WrkVrt wrkvrt_backup = workVertex;
        
        m_fitSvc->setDefault();
        m_fitSvc->setApproximateVertex( vertexPos.x(), vertexPos.y(), vertexPos.z() );
        
        std::vector<const xAOD::TrackParticle*>   baseTracks;
        std::vector<const xAOD::NeutralParticle*> dummyNeutrals;
        
        workVertex.Chi2PerTrk.clear();
        
        for( const auto& index : workVertex.selectedTrackIndices ) {
          baseTracks.emplace_back( m_selectedTracks->at( index ) );
          workVertex.Chi2PerTrk.emplace_back( AlgConsts::chi2PerTrackInitValue );
        }
        
        baseTracks.emplace_back( trk );
        workVertex.Chi2PerTrk.emplace_back( AlgConsts::chi2PerTrackInitValue );
        
        Amg::Vector3D initPos;
        
        {
          StatusCode sc = m_fitSvc->VKalVrtFitFast( baseTracks, initPos );/* Fast crude estimation */
        
          if( sc.isFailure() ) ATH_MSG_DEBUG(" >> " << __FUNCTION__ << ": fast crude estimation failed.");
        
          const auto& diffPos = initPos - vertexPos;
        
          if( diffPos.norm() > 10. ) {
        
            ATH_MSG_VERBOSE( " >> " << __FUNCTION__ << ": approx vertex as original" );
            m_fitSvc->setApproximateVertex( vertexPos.x(), vertexPos.y(), vertexPos.z() );
          
          } else {
          
            ATH_MSG_VERBOSE( " >> " << __FUNCTION__ << ": approx vertex set to (" << initPos.x() << ", " << initPos.y() << ", " << initPos.z() << ")" );
            m_fitSvc->setApproximateVertex( initPos.x(), initPos.y(), initPos.z() );
          
          }
        }
        
        
        ATH_MSG_VERBOSE( " >> " << __FUNCTION__ << ": now vertex fitting..." );
        
        StatusCode sc = m_fitSvc->VKalVrtFit(baseTracks, dummyNeutrals,
                                             workVertex.vertex,
                                             workVertex.vertexMom,
                                             workVertex.Charge,
                                             workVertex.vertexCov,
                                             workVertex.Chi2PerTrk, 
                                             workVertex.TrkAtVrt,
                                             workVertex.Chi2            );
        
        if( sc.isFailure() ) {
          ATH_MSG_DEBUG(" >> " << __FUNCTION__ << ": VKalVrtFit failure. Revert to backup");
          workVertex = wrkvrt_backup;
          
          if( m_jp.FillHist ) m_hists["associateMonitor"]->Fill( 1 );
                                
          continue;
        }
        
        if( workVertex.Chi2 / (workVertex.ndof() + AlgConsts::infinitesimal) > 1.e4 ) {
          
          if( m_jp.FillHist ) m_hists["associateMonitor"]->Fill( 2 );
          
          workVertex = wrkvrt_backup;
          continue;
        }
        
        if( m_jp.FillHist ) m_hists["associateMonitor"]->Fill( 0 );

        auto& cov = workVertex.vertexCov;
        
        ATH_MSG_DEBUG( " >> " << __FUNCTION__ << ": succeeded in associating. New vertex pos = (" << vertexPos.perp() << ", " << vertexPos.z() << ", " << vertexPos.perp()*vertexPos.phi() << ")" );
        ATH_MSG_DEBUG( " >> " << __FUNCTION__ << ": New vertex cov = (" << cov.at(0) << ", " << cov.at(1) << ", " << cov.at(2) << ", " << cov.at(3) << ", " << cov.at(4) << ", " << cov.at(5) << ")" );
        
        workVertex.associatedTrackIndices.emplace_back( m_associatedTracks->size() );
        
        m_associatedTracks->emplace_back( trk );
        decor_isAssociated( *trk ) = true;
        
      }

    }
    
    return StatusCode::SUCCESS;
  }
    
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::mergeByShuffling( std::vector<WrkVrt> *workVerticesContainer )
  {
    
    ATH_MSG_DEBUG( " >> " << __FUNCTION__ << ": #verticess = " << workVerticesContainer->size() );
    
    // First, sort WrkVrt by the track multiplicity
    std::sort( workVerticesContainer->begin(), workVerticesContainer->end(), [](WrkVrt& v1, WrkVrt& v2) { return v1.selectedTrackIndices.size() < v2.selectedTrackIndices.size(); } );
    
    // Loop over vertices (small -> large Ntrk order)
    for( auto& workVertex : *workVerticesContainer ) {
      if( !workVertex.isGood               )             continue;
      if(  workVertex.selectedTrackIndices.size() <= 1 ) continue;
      
      // Reverse iteration: large Ntrk -> small Ntrk order
      for( auto ritr = workVerticesContainer->rbegin(); ritr != workVerticesContainer->rend(); ++ritr ) {
        auto& vertexToMerge = *ritr;
        
        if( !vertexToMerge.isGood               )                                                 continue;
        if(  vertexToMerge.selectedTrackIndices.size() <= 1 )                                     continue;
        if( &workVertex == &vertexToMerge     )                                                   continue;
        if(  vertexToMerge.selectedTrackIndices.size() < workVertex.selectedTrackIndices.size() ) continue;
        
        const double& significance = significanceBetweenVertices( workVertex, vertexToMerge );
        
        if( significance > m_jp.mergeByShufflingMaxSignificance ) continue;
        
        bool mergeFlag { false };
        
        ATH_MSG_DEBUG(" > " << __FUNCTION__ 
                      << ": vertex " << &workVertex << " #tracks = " << workVertex.selectedTrackIndices.size()
                      << " --> to Merge : " << &vertexToMerge << ", #tracks = " << vertexToMerge.selectedTrackIndices.size()
                      << " significance = " << significance );
        
        double min_signif = AlgConsts::maxValue;
        
        // Method 1. Assume that the solution is somewhat wrong, and the solution gets correct if it starts from the other vertex position
        if( m_jp.doSuggestedRefitOnMerging && !mergeFlag ) {
          WrkVrt testVertex = workVertex;
          StatusCode sc = refitVertexWithSuggestion( testVertex, vertexToMerge.vertex );
          if( sc.isFailure() ) {}
          
          const auto signif = significanceBetweenVertices( testVertex, vertexToMerge );
          if( signif < min_signif ) min_signif = signif;
          
          if( signif < m_jp.mergeByShufflingAllowance ) {
            ATH_MSG_DEBUG(" > " << __FUNCTION__ << ":  method1:  vertexToMerge " << &vertexToMerge << ": test signif = " << signif );
            mergeFlag = true;
            
          }
          
          if( m_jp.FillHist && min_signif > 0. ) m_hists["shuffleMinSignif1"]->Fill( log10( min_signif ) );
          if( m_jp.FillHist && mergeFlag ) { m_hists["mergeType"]->Fill( SHUFFLE1 ); }
        }
        
        // Method 2. magnet merging: borrowing another track from the target vertex to merge
        if( m_jp.doMagnetMerging && !mergeFlag ) {
          
          // Loop over tracks in vertexToMerge
          for( auto& index : vertexToMerge.selectedTrackIndices ) {
          
            WrkVrt testVertex = workVertex;
            testVertex.selectedTrackIndices.emplace_back( index );
          
            StatusCode sc = refitVertexWithSuggestion( testVertex, vertexToMerge.vertex );
            if( sc.isFailure() ) {}
          
            const auto signif = significanceBetweenVertices( testVertex, vertexToMerge );
            if( signif < min_signif ) min_signif = signif;
          
            if( signif < m_jp.mergeByShufflingAllowance ) {
              ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": method2:  vertexToMerge " << &vertexToMerge << " track index " << index << ": test signif = " << signif );
              mergeFlag = true;
            }
          
          }
        
          if( m_jp.FillHist && min_signif > 0. ) m_hists["shuffleMinSignif2"]->Fill( log10( min_signif ) );
          
          if( m_jp.FillHist && mergeFlag ) { m_hists["mergeType"]->Fill( SHUFFLE2 ); }
        }
        
        // Method 3. Attempt to force merge
        if( m_jp.doWildMerging && !mergeFlag ) {
          
          WrkVrt testVertex = workVertex;
          
          for( auto& index : vertexToMerge.selectedTrackIndices ) {
            testVertex.selectedTrackIndices.emplace_back( index );
          }
          
          StatusCode sc = refitVertexWithSuggestion( testVertex, vertexToMerge.vertex );
          if( sc.isFailure() ) {}
          
          const auto signif = significanceBetweenVertices( testVertex, vertexToMerge );
          if( signif < min_signif ) min_signif = signif;
          
          if( signif < m_jp.mergeByShufflingAllowance ) {
            ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": method3:  vertexToMerge " << &vertexToMerge << ": test signif = " << signif );
            mergeFlag = true;
          }
          
          if( m_jp.FillHist && min_signif > 0. ) m_hists["shuffleMinSignif3"]->Fill( log10( min_signif ) );
          if( m_jp.FillHist && mergeFlag ) { m_hists["mergeType"]->Fill( SHUFFLE3 ); }
        }
        
        
        if( mergeFlag ) {
          ATH_MSG_DEBUG(" > " << __FUNCTION__ << ":   vertexToMerge " << &vertexToMerge << " ==> min signif = " << min_signif << " judged to merge" );
          StatusCode sc = mergeVertices( vertexToMerge, workVertex );
          if( sc.isFailure() ) {}
        }
          
      }
      
    }
    
    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::mergeFinalVertices( std::vector<WrkVrt> *workVerticesContainer )
  {
    
    while (true) {
      //
      //  Minimal vertex-vertex distance
      //
      for( auto& wrkvrt : *workVerticesContainer) {
        wrkvrt.closestWrkVrtIndex = AlgConsts::invalidUnsigned;
        wrkvrt.closestWrkVrtValue = AlgConsts::maxValue;
      }
      
      std::pair<unsigned, unsigned> indexPair { AlgConsts::invalidUnsigned, AlgConsts::invalidUnsigned };
      auto minDistance = findMinVerticesPair( workVerticesContainer, indexPair, &VrtSecInclusive::distanceBetweenVertices );

      if( minDistance      == AlgConsts::maxValue )        break;
      if( indexPair.first  == AlgConsts::invalidUnsigned ) break;
      if( indexPair.second == AlgConsts::invalidUnsigned ) break;
      
      auto& v1 = workVerticesContainer->at(indexPair.first);
      auto& v2 = workVerticesContainer->at(indexPair.second);
      
      const double averageRadius = ( v1.vertex.perp() + v2.vertex.perp() ) / 2.0;
      
      if( minDistance >  m_jp.VertexMergeFinalDistCut + m_jp.VertexMergeFinalDistScaling * averageRadius ) {
        ATH_MSG_DEBUG( "Vertices " << indexPair.first << " and " << indexPair.second
            <<" are separated by distance " << minDistance );
        break;
      }

      ATH_MSG_DEBUG( "Merging FINAL vertices " << indexPair.first << " and " << indexPair.second
                     <<" which are separated by distance "<< minDistance );
        
      StatusCode sc = mergeVertices( v1, v2 );
      if( sc.isFailure() ) {}
      if( m_jp.FillHist ) { m_hists["mergeType"]->Fill( FINAL ); }
        
    }

    return StatusCode::SUCCESS;

  } // end of mergeFinalVertices
  
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::refitAndSelectGoodQualityVertices( std::vector<WrkVrt> *workVerticesContainer )
  {

    // Output SVs as xAOD::Vertex
    // Needs a conversion function from workVerticesContainer to xAOD::Vertex here.
    // The supposed form of the function will be as follows:

    xAOD::VertexContainer *secondaryVertexContainer( nullptr );
    ATH_CHECK( evtStore()->retrieve( secondaryVertexContainer, "VrtSecInclusive_" + m_jp.secondaryVerticesContainerName ) );
    
    const xAOD::TrackParticleContainer* trackParticleContainer ( nullptr );
    ATH_CHECK( evtStore()->retrieve( trackParticleContainer, m_jp.TrackLocation) );
    
    enum { kPt, kEta, kPhi, kD0, kZ0, kErrP, kErrD0, kErrZ0, kChi2SV };
    static std::map< unsigned, SG::AuxElement::Decorator<float> > trkDecors
    {
       { kPt,     SG::AuxElement::Decorator<float>("pt_wrtSV")    },
       { kEta,    SG::AuxElement::Decorator<float>("eta_wrtSV")   }, 
       { kPhi,    SG::AuxElement::Decorator<float>("phi_wrtSV")   },
       { kD0,     SG::AuxElement::Decorator<float>("d0_wrtSV")    },
       { kZ0,     SG::AuxElement::Decorator<float>("z0_wrtSV")    },
       { kErrP,   SG::AuxElement::Decorator<float>("errP_wrtSV")  },
       { kErrD0,  SG::AuxElement::Decorator<float>("errd0_wrtSV") },
       { kErrZ0,  SG::AuxElement::Decorator<float>("errz0_wrtSV") },
       { kChi2SV, SG::AuxElement::Decorator<float>("chi2_toSV")   }
    };
    static SG::AuxElement::Decorator<char> decor_svtrk( "is_svtrk_final" );

    std::map<const WrkVrt*, const xAOD::Vertex*> wrkvrtLinkMap;
    
    //----------------------------------------------------------

    m_fitSvc->setDefault();
    m_fitSvc->setMomCovCalc(1);

    // Loop over vertices
    for( auto& wrkvrt : *workVerticesContainer ) {
      
      if( m_jp.FillHist ) m_hists["finalCutMonitor"]->Fill( 0 );

      if( m_jp.removeFakeVrt && m_jp.removeFakeVrtLate ) {
        removeInconsistentTracks( wrkvrt );
      }
      
      int nth = wrkvrt.selectedTrackIndices.size() + wrkvrt.associatedTrackIndices.size();

      if( nth < 2 ) continue;               /* Bad vertices */

      if( m_jp.FillHist ) m_hists["finalCutMonitor"]->Fill( 1 );
      
      if( m_jp.doFinalImproveChi2 ) {
        improveVertexChi2( wrkvrt );
        
        // If the number of remaining tracks is less than 2, drop.
        if( wrkvrt.selectedTrackIndices.size() + wrkvrt.associatedTrackIndices.size() < 2 ) continue;
      }
      
      if( m_jp.FillHist ) m_hists["finalCutMonitor"]->Fill( 2 );
      
      
      {
        WrkVrt backup = wrkvrt;
      
        StatusCode sc = refitVertexWithSuggestion( wrkvrt, wrkvrt.vertex );
        if( sc.isFailure() ) {
          
          auto indices = wrkvrt.associatedTrackIndices;
          
          wrkvrt.associatedTrackIndices.clear();
          sc = refitVertexWithSuggestion( wrkvrt, wrkvrt.vertex );
          if( sc.isFailure() ) {
            wrkvrt = backup;
          }
          
          for( auto& index : indices ) {
            backup = wrkvrt;
            wrkvrt.associatedTrackIndices.emplace_back( index );
            sc = refitVertexWithSuggestion( wrkvrt, wrkvrt.vertex );
            if( sc.isFailure() || wrkvrt.Chi2 / wrkvrt.ndof() > 1.e4 ) {
              wrkvrt = backup;
              continue;
            }
          }
          
        }
      }
      
      if( m_jp.FillHist ) m_hists["finalCutMonitor"]->Fill( 3 );
      
      //
      //  Store good vertices into StoreGate 
      //
      if( m_jp.FillNtuple ) m_ntupleVars->get<unsigned int>( "NumSecVrt" )++;

      std::vector <double> CovFull;
      { 
        StatusCode sc = m_fitSvc->VKalGetFullCov( static_cast<long int>( nth ), CovFull); 
        if( sc.isFailure() ){}
      }
      

      double vert_mass=0; double vert_pt=0; double vert_pz=0;
      double vert_masse=0; //assuming tracks are electrons
      double vert_massp=0; //assuming tracks are protons
      double vx=0; double vy=0; double vz=0; double ve=0; double vee=0; double vep=0;
      int AllBLay = 1;
      int SumBLay=0;

      // Pre-check before storing vertex if the SV perigee is available
      bool good_flag = true;
      
      for(size_t itrk=0; itrk<wrkvrt.selectedTrackIndices.size(); itrk++) {
        const auto* trk = m_selectedTracks->at( wrkvrt.selectedTrackIndices[itrk] );
        const Trk::Perigee* sv_perigee = m_trackToVertexTool->perigeeAtVertex( *trk, wrkvrt.vertex );
        if( !sv_perigee ) {
          ATH_MSG_INFO(" > refitAndSelectGoodQualityVertices: > Track index " << trk->index() << ": Failed in obtaining the SV perigee!" );
          good_flag = false;
        }
        delete sv_perigee;
      }
      for(size_t itrk=0; itrk<wrkvrt.associatedTrackIndices.size(); itrk++) {
        const auto* trk = m_associatedTracks->at( wrkvrt.associatedTrackIndices[itrk] );
        const Trk::Perigee* sv_perigee = m_trackToVertexTool->perigeeAtVertex( *trk, wrkvrt.vertex );
        if( !sv_perigee ) {
          ATH_MSG_INFO(" > refitAndSelectGoodQualityVertices: > Track index " << trk->index() << ": Failed in obtaining the SV perigee!" );
          good_flag = false;
        }
        delete sv_perigee;
      }
      if( !good_flag ) {
        continue;
      }
      
      if( m_jp.FillHist ) m_hists["finalCutMonitor"]->Fill( 4 );
      
      
      std::vector<const xAOD::TrackParticle*> tracks;
      std::vector< std::pair<const xAOD::TrackParticle*, double> > trackChi2Pairs;
      
      {
        
        for( const auto& index : wrkvrt.selectedTrackIndices )   tracks.emplace_back( m_selectedTracks->at( index ) );
        for( const auto& index : wrkvrt.associatedTrackIndices ) tracks.emplace_back( m_associatedTracks->at( index ) );
      
        
        auto trkitr = tracks.begin();
        auto chi2itr = wrkvrt.Chi2PerTrk.begin();
        
        for( ; ( trkitr!=tracks.end() && chi2itr!=wrkvrt.Chi2PerTrk.end() ); ++trkitr, ++chi2itr ) {
          trackChi2Pairs.emplace_back( std::make_pair(*trkitr, *chi2itr) );
        }
        
      }
      
      
      TLorentzVector vsum_total;
      TLorentzVector vsum_selected;
      TLorentzVector vsum_ptAbove1GeV;
      unsigned mult_total       { 0 };
      unsigned mult_selected    { 0 };
      unsigned mult_ptAbove1GeV { 0 };
      
      // loop over vertex tracks
      ATH_MSG_DEBUG(" > refitAndSelectGoodQualityVertices: Track loop: size = " << tracks.size() );
      for( auto& pair : trackChi2Pairs ) {
        
        const auto* trk      = pair.first;
        const auto& chi2AtSV = pair.second;

        ATH_MSG_VERBOSE(" > refitAndSelectGoodQualityVertices: > Track index " << trk->index() << ": start." );

        track_summary trk_summary;
        fillTrackSummary( trk_summary, trk );

        if( ! (trk->hitPattern() & (1<<Trk::pixelBarrel0)) ) AllBLay = 0;

        //
        // calculate mass/pT of tracks and track parameters
        //

        double trk_phi   = trk->phi();
        double trk_theta = trk->theta();
        double trk_eta   = trk->eta();

        double trk_p  = 1.0/fabs( trk->qOverP() );
        double trk_pz = trk_p*cos(trk_theta);  
        double trk_pt = trk->pt();
        double trk_px = trk_pt*cos( trk_phi );  
        double trk_py = trk_pt*sin( trk_phi);  

        double trk_e  = hypot(trk_p, VKalVrtAthena::PhysConsts::mass_chargedPion); 
        
        // for all tracks
        vsum_total += trk->p4();
        mult_total++;
        
        // for selected tracks only
        if( trk->isAvailable<char>("is_selected") ) {
          if( trk->auxdataConst<char>("is_selected") ) {
            vsum_selected += trk->p4();
            mult_selected++;
          }
        }
        
        // for pT > 1 GeV tracks only
        if( trk->pt() > 1.e3 ) {
          vsum_ptAbove1GeV += trk->p4();
          mult_ptAbove1GeV++;
        }
        
        ATH_MSG_VERBOSE(" > refitAndSelectGoodQualityVertices: > Track index " << trk->index() << ": in vrt chg/px/py/pz/pt/e/phi/eta = "
            << trk->charge() <<","
            <<trk_px<<","
            <<trk_py<<","
            <<trk_pz<<","
            <<trk_pt<<","
            <<trk_e<<","
            <<trk_phi<<","
            <<trk_eta);

        /////////////////////////////////////////////
        // Get the perigee of the track at the vertex
        ATH_MSG_VERBOSE(" > refitAndSelectGoodQualityVertices: > Track index " << trk->index() << ": Get the prigee of the track at the vertex." );

        const Trk::Perigee* sv_perigee = m_trackToVertexTool->perigeeAtVertex( *trk, wrkvrt.vertex );
        if( !sv_perigee ) {
          ATH_MSG_WARNING(" > refitAndSelectGoodQualityVertices: > Track index " << trk->index() << ": Failed in obtaining the SV perigee!" );
          
          for( auto& pair : trkDecors ) {
            pair.second( *trk ) = AlgConsts::invalidFloat;
          }
          decor_svtrk( *trk ) = true;
          continue;
        }

        double qOverP_wrtSV    = sv_perigee->parameters() [Trk::qOverP];
        double theta_wrtSV     = sv_perigee->parameters() [Trk::theta];
        double p_wrtSV         = 1.0 / fabs( qOverP_wrtSV );
        double pt_wrtSV        = p_wrtSV * sin( theta_wrtSV );
        double eta_wrtSV       = -log( tan( theta_wrtSV/2. ) );
        double phi_wrtSV       = sv_perigee->parameters() [Trk::phi];
        double d0_wrtSV        = sv_perigee->parameters() [Trk::d0];
        double z0_wrtSV        = sv_perigee->parameters() [Trk::z0];
        double errd0_wrtSV     = (*sv_perigee->covariance())( Trk::d0 );
        double errz0_wrtSV     = (*sv_perigee->covariance())( Trk::z0 );
        //double errQoverP_wrtSV = (*sv_perigee->covariance())( Trk::qOverP );
        double errP_wrtSV      = (*sv_perigee->covariance())( Trk::qOverP );

        // xAOD::Track augmentation
        ( trkDecors.at(kPt)    )( *trk ) = pt_wrtSV;
        ( trkDecors.at(kEta)   )( *trk ) = eta_wrtSV;
        ( trkDecors.at(kPhi)   )( *trk ) = phi_wrtSV;
        ( trkDecors.at(kD0)    )( *trk ) = d0_wrtSV;
        ( trkDecors.at(kZ0)    )( *trk ) = z0_wrtSV;
        ( trkDecors.at(kErrP)  )( *trk ) = errP_wrtSV;
        ( trkDecors.at(kErrD0) )( *trk ) = errd0_wrtSV;
        ( trkDecors.at(kErrZ0) )( *trk ) = errz0_wrtSV;
        ( trkDecors.at(kChi2SV))( *trk ) = chi2AtSV;
        
        decor_svtrk( *trk ) = true;
        
        
        vert_pt += pt_wrtSV;
        vert_pz += p_wrtSV * cos(theta_wrtSV);
        vx      += p_wrtSV * sin(theta_wrtSV) * cos(phi_wrtSV);
        vy      += p_wrtSV * sin(theta_wrtSV) * sin(phi_wrtSV);
        vz      += p_wrtSV * cos(theta_wrtSV);
        ve      += hypot( p_wrtSV, PhysConsts::mass_chargedPion );
        vee     += hypot( p_wrtSV, PhysConsts::mass_electron );
        vep     += hypot( p_wrtSV, PhysConsts::mass_proton );
        
        delete sv_perigee;

        ATH_MSG_VERBOSE(" > refitAndSelectGoodQualityVertices: > Track index " << trk->index() << ": end." );
      } // loop over tracks in vertex

      ATH_MSG_VERBOSE(" > refitAndSelectGoodQualityVertices: Track loop end. ");

      //nVrtVx->emplace_back(tmpVx);

      // Make vertex mass
      vert_mass  = sqrt(ve*ve - vx*vx -vy*vy -vz*vz);
      vert_masse = sqrt(vee*vee - vx*vx -vy*vy -vz*vz);
      vert_massp = sqrt(vep*vep - vx*vx -vy*vy -vz*vz);


      ATH_MSG_DEBUG(" > refitAndSelectGoodQualityVertices: Final Sec.Vertex="<<nth<<", "
          <<wrkvrt.vertex[0]<<", "<<wrkvrt.vertex[1]<<", "
          <<wrkvrt.vertex[2]<<","<<vert_mass<<","<<vert_pt<<","<<vert_masse);


      //
      // calculate opening angle between all 2-track pairs, and store the minimum
      //
      double minOpAng = AlgConsts::invalidFloat;
      std::vector<double> opAngles;
      
      for( auto itr1 = tracks.begin(); itr1 != tracks.end(); ++itr1 ) {
        for( auto itr2 = std::next( itr1 ); itr2 != tracks.end(); ++itr2 ) {
          const auto& p1 = (*itr1)->p4().Vect();
          const auto& p2 = (*itr2)->p4().Vect();
          auto cos = p1 * p2 / p1.Mag() / p2.Mag();
          opAngles.emplace_back( cos );
        }
      }
      minOpAng = *( std::max_element( opAngles.begin(), opAngles.end() ) );
      if( m_jp.FillNtuple ) m_ntupleVars->get< vector<double> >( "SecVtx_MinOpAng" ).emplace_back(minOpAng);
      
        
      if( m_jp.FillHist ) m_hists["finalCutMonitor"]->Fill( 5 );

      ///////////////////////////////////////////////////
      // Data filling to xAOD container
      
      wrkvrt.isGood = true;

      // Firstly store the new vertex to the container before filling properties.
      // (This is the feature of xAOD.)
      xAOD::Vertex* vertex = new xAOD::Vertex;
      secondaryVertexContainer->emplace_back( vertex );

      // Registering the vertex position to xAOD::Vertex
      vertex->setPosition( wrkvrt.vertex );

      // Registering the vertex type: SV
      vertex->setVertexType( xAOD::VxType::SecVtx );

      // Registering the vertex chi2 and Ndof
      int ndof = wrkvrt.ndof();
      vertex->setFitQuality( wrkvrt.Chi2, ndof );

      // Registering the vertex covariance matrix
      std::vector<float> fCov(wrkvrt.vertexCov.cbegin(), wrkvrt.vertexCov.cend());
      vertex->setCovariance(fCov);

      // Registering the vertex momentum and charge
      vertex->auxdata<float>("vtx_px")                  = wrkvrt.vertexMom.Px();
      vertex->auxdata<float>("vtx_py")                  = wrkvrt.vertexMom.Py();
      vertex->auxdata<float>("vtx_pz")                  = wrkvrt.vertexMom.Pz();

      vertex->auxdata<float>("vtx_mass")                = wrkvrt.vertexMom.M();
      vertex->auxdata<float>("vtx_charge")              = wrkvrt.Charge;

      // Other SV properties
      vertex->auxdata<float>("mass")                    = vert_mass;
      vertex->auxdata<float>("mass_e")                  = vert_masse;
      vertex->auxdata<float>("mass_proton")             = vert_massp;
      vertex->auxdata<float>("pT")                      = vert_pt;
      vertex->auxdata<float>("pz")                      = vert_pz;
      vertex->auxdata<float>("sumBLayHits")             = SumBLay;
      vertex->auxdata<float>("allTrksBLayHits")         = AllBLay;
      vertex->auxdata<float>("minOpAng")                = minOpAng;
      vertex->auxdata<float>("num_trks")                = ( wrkvrt.selectedTrackIndices.size() + wrkvrt.associatedTrackIndices.size() );
      vertex->auxdata<float>("num_selectedTracks")      = wrkvrt.selectedTrackIndices.size();
      vertex->auxdata<float>("num_associatedTracks")    = wrkvrt.associatedTrackIndices.size();
      vertex->auxdata<float>("dCloseVrt")               = wrkvrt.closestWrkVrtValue;
      vertex->auxdata<float>("mass_selectedTracks")     = vsum_selected.M();
      vertex->auxdata<float>("mass_ptAbove1GeV")        = vsum_ptAbove1GeV.M();
      vertex->auxdata<float>("num_trks_selectedTracks") = mult_selected;
      vertex->auxdata<float>("num_trks_ptAbove1GeV")    = mult_ptAbove1GeV;
      
      
      // Registering tracks comprising the vertex to xAOD::Vertex
      // loop over the tracks comprising the vertex
      for( auto trk_id : wrkvrt.selectedTrackIndices ) {

        const xAOD::TrackParticle *trk = m_selectedTracks->at( trk_id );

        // Acquire link the track to the vertex
        ElementLink<xAOD::TrackParticleContainer> link_trk( *( dynamic_cast<const xAOD::TrackParticleContainer*>( trk->container() ) ), static_cast<long unsigned int>(trk->index()) );

        // Register the link to the vertex
        vertex->addTrackAtVertex( link_trk, 1. );

      }
      
      for( auto trk_id : wrkvrt.associatedTrackIndices ) {

        const xAOD::TrackParticle *trk = m_associatedTracks->at( trk_id );

        // Acquire link the track to the vertex
        ElementLink<xAOD::TrackParticleContainer> link_trk( *( dynamic_cast<const xAOD::TrackParticleContainer*>( trk->container() ) ), static_cast<long unsigned int>(trk->index()) );

        // Register the link to the vertex
        vertex->addTrackAtVertex( link_trk, 1. );

      }

      
      if( m_jp.doMapToLocal ) {
        // Obtain the local mapping of the reconstructed vertex
        Trk::MappedVertex mappedVtx = m_vertexMapper->mapToLocal( wrkvrt.vertex );
        if( mappedVtx.valid ) {
          vertex->auxdata<int>("local_identifierHash") = mappedVtx.identifierHash;
          vertex->auxdata<int>("local_layerIndex")     = mappedVtx.layerIndex;
          vertex->auxdata<float>("local_posX")         = mappedVtx.localPosition.x();
          vertex->auxdata<float>("local_posY")         = mappedVtx.localPosition.y();
          vertex->auxdata<float>("local_posZ")         = mappedVtx.localPosition.z();
        } else {
          vertex->auxdata<int>("local_identifierHash") = AlgConsts::invalidInt;
          vertex->auxdata<int>("local_layerIndex")     = AlgConsts::invalidInt;
          vertex->auxdata<float>("local_posX")         = AlgConsts::invalidFloat;
          vertex->auxdata<float>("local_posY")         = AlgConsts::invalidFloat;
          vertex->auxdata<float>("local_posZ")         = AlgConsts::invalidFloat;
        }
      }


      // For MC, try to trace down to the truth particles,
      // and depending on the topology, categorize the label of the reconstructed vertex.
      if( m_jp.doTruth ) {
        ATH_CHECK( categorizeVertexTruthTopology( vertex ) );
      }
      
      // Keep the link between wrkvrt and vertex for later use
      wrkvrtLinkMap[&wrkvrt] = vertex;
      
      
    } // loop over vertices

    if( m_jp.FillNtuple ) {
      ATH_CHECK( fillAANT_SecondaryVertices( secondaryVertexContainer ) );
    }
    
    
    // Post process -- Additional augmentations
    if( m_jp.doAugmentDVimpactParametersToMuons     ) { ATH_CHECK( augmentDVimpactParametersToMuons()     ); }
    if( m_jp.doAugmentDVimpactParametersToElectrons ) { ATH_CHECK( augmentDVimpactParametersToElectrons() ); }

    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::monitorVertexingAlgorithmStep( std::vector<WrkVrt>* workVerticesContainer, const std::string name, bool final ) {
    
    if( m_jp.FillIntermediateVertices ) {
      
      const xAOD::TrackParticleContainer* trackParticleContainer ( nullptr );
      ATH_CHECK( evtStore()->retrieve( trackParticleContainer, m_jp.TrackLocation) );
      
      xAOD::VertexContainer* intermediateVertexContainer { nullptr };
      
      ATH_CHECK( evtStore()->retrieve( intermediateVertexContainer,      "VrtSecInclusive_IntermediateVertices_" + name           ) );
      
      for( auto& wrkvrt : *workVerticesContainer ) {
        
        xAOD::Vertex* vertex = new xAOD::Vertex;
        intermediateVertexContainer->emplace_back( vertex );

        // Registering the vertex position to xAOD::Vertex
        vertex->setPosition( wrkvrt.vertex );

        // Registering the vertex type: SV
        vertex->setVertexType( xAOD::VxType::SecVtx );

        // Registering the vertex chi2 and Ndof
        int ndof = wrkvrt.ndof();
        vertex->setFitQuality( wrkvrt.Chi2, ndof );

        // Registering the vertex covariance matrix
        std::vector<float> fCov(wrkvrt.vertexCov.cbegin(), wrkvrt.vertexCov.cend());
        vertex->setCovariance(fCov);
      
        // Registering tracks comprising the vertex to xAOD::Vertex
        // loop over the tracks comprising the vertex
        for( auto trk_id : wrkvrt.selectedTrackIndices ) {

          const xAOD::TrackParticle *trk = m_selectedTracks->at( trk_id );

          // Acquire link the track to the vertex
          ElementLink<xAOD::TrackParticleContainer> link_trk( *( dynamic_cast<const xAOD::TrackParticleContainer*>( trk->container() ) ), static_cast<long unsigned int>(trk->index()) );

          // Register the link to the vertex
          vertex->addTrackAtVertex( link_trk, 1. );

        }
      
        for( auto trk_id : wrkvrt.associatedTrackIndices ) {

          const xAOD::TrackParticle *trk = m_associatedTracks->at( trk_id );

          // Acquire link the track to the vertex
          ElementLink<xAOD::TrackParticleContainer> link_trk( *( dynamic_cast<const xAOD::TrackParticleContainer*>( trk->container() ) ), static_cast<long unsigned int>(trk->index()) );

          // Register the link to the vertex
          vertex->addTrackAtVertex( link_trk, 1. );

        }
      }
      
    }
    
    
    
    if( !m_jp.FillHist ) return StatusCode::SUCCESS;
    
    printWrkSet( workVerticesContainer, Form("%s (step %u)", name.c_str(), m_vertexingAlgorithmStep) );
    
    unsigned count = std::count_if( workVerticesContainer->begin(), workVerticesContainer->end(),
                                    []( WrkVrt& v ) { return ( v.selectedTrackIndices.size() + v.associatedTrackIndices.size() ) >= 2; } );
    
    m_hists["vertexYield"]->Fill( m_vertexingAlgorithmStep, count );
    m_hists["vertexYield"]->GetXaxis()->SetBinLabel( m_vertexingAlgorithmStep+1, name.c_str() );
    
    for( auto& vertex : *workVerticesContainer ) {
      auto ntrk = vertex.selectedTrackIndices.size() + vertex.associatedTrackIndices.size();
      if( vertex.isGood && ntrk >= 2 ) {
        dynamic_cast<TH2F*>( m_hists["vertexYieldNtrk"] )->Fill( ntrk, m_vertexingAlgorithmStep );
        dynamic_cast<TH2F*>( m_hists["vertexYieldChi2"] )->Fill( vertex.Chi2/(vertex.ndof() + AlgConsts::infinitesimal), m_vertexingAlgorithmStep );
      }
    }
    m_hists["vertexYieldNtrk"]->GetYaxis()->SetBinLabel( m_vertexingAlgorithmStep+1, name.c_str() );
    m_hists["vertexYieldChi2"]->GetYaxis()->SetBinLabel( m_vertexingAlgorithmStep+1, name.c_str() );
    
    
    if( !final ) return StatusCode::SUCCESS;
    
    for( auto& vertex : *workVerticesContainer ) {
      auto ntrk = vertex.selectedTrackIndices.size() + vertex.associatedTrackIndices.size();
      if( vertex.isGood && ntrk >= 2 ) {
        m_hists["finalVtxNtrk"] ->Fill( ntrk );
        m_hists["finalVtxR"]    ->Fill( vertex.vertex.perp() );
        dynamic_cast<TH2F*>( m_hists["finalVtxNtrkR"] )->Fill( ntrk, vertex.vertex.perp() );
      }
    }
    
    return StatusCode::SUCCESS;
  }
  

} // end of namespace VKalVrtAthena
