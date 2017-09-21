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
  StatusCode VrtSecInclusive::extractIncompatibleTracks()
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
    
    // Work variables
    vector<const xAOD::TrackParticle*>    ListBaseTracks;
    vector<const xAOD::NeutralParticle*>  dummyNeutrals(0);
    vector<double> Results;
    Amg::Vector3D               FitVertex;
    TLorentzVector              Momentum;
    vector< vector<double> > TrkAtVrt; 
    vector<double> Chi2PerTrk,ErrorMatrix;
    long int           Charge;
    double             Chi2 = 0.;
    
    ATH_MSG_DEBUG(" > extractIncompatibleTracks: Selected Tracks = "<< m_selectedTracks->size());


    // first make all 2-track vertices
    for( auto itrk = m_selectedTracks->begin(); itrk != m_selectedTracks->end(); ++itrk ) {
      for( auto jtrk = itrk; jtrk != m_selectedTracks->end(); ++jtrk ) {

        if( itrk == jtrk ) continue;

        int itrk_id = itrk - m_selectedTracks->begin();
        int jtrk_id = jtrk - m_selectedTracks->begin();

        // Attempt to think the combination is incompatible by default
        m_incomp.emplace_back( itrk_id );
        m_incomp.emplace_back( jtrk_id );

        ListBaseTracks.clear();
        ListBaseTracks.emplace_back( *itrk );
        ListBaseTracks.emplace_back( *jtrk );

        // new code to find initial approximate vertex
        Amg::Vector3D IniVertex;

        StatusCode sc = m_fitSvc->VKalVrtFitFast( ListBaseTracks, IniVertex );/* Fast crude estimation */
        if(sc.isFailure()) ATH_MSG_DEBUG(" > extractIncompatibleTracks: fast crude estimation fails ");

        m_fitSvc->setApproximateVertex( IniVertex.x(), IniVertex.y(), IniVertex.z() );

        // Vertex VKal Fitting
        sc = m_fitSvc->VKalVrtFit( ListBaseTracks,
                                   dummyNeutrals,
                                   FitVertex, Momentum, Charge,
                                   ErrorMatrix, Chi2PerTrk,
                                   TrkAtVrt, Chi2  );
        
        if( sc.isFailure() )  continue;          /* No fit */ 

        // Compatibility to the primary vertex.
        Amg::Vector3D vDist = FitVertex - m_thePV->position();
        double vPos = ( vDist.x()*Momentum.Px()+vDist.y()*Momentum.Py()+vDist.z()*Momentum.Pz() )/Momentum.Rho();
        
        if( m_jp.FillNtuple ) {
          // Fill the 2-track vertex properties to AANT
          m_ntupleVars->get<unsigned int>( "All2TrkVrtNum" )++;
          m_ntupleVars->get< vector<double> >( "All2TrkVrtMass" )   .emplace_back(Momentum.M());
          m_ntupleVars->get< vector<double> >( "All2TrkVrtPt" )     .emplace_back(Momentum.Perp());
          m_ntupleVars->get< vector<int> >   ( "All2TrkVrtCharge" ) .emplace_back(Charge);
          m_ntupleVars->get< vector<double> >( "All2TrkVrtX" )      .emplace_back(FitVertex.x());
          m_ntupleVars->get< vector<double> >( "All2TrkVrtY" )      .emplace_back(FitVertex.y());
          m_ntupleVars->get< vector<double> >( "All2TrkVrtZ" )      .emplace_back(FitVertex.z());
          m_ntupleVars->get< vector<double> >( "All2TrkVrtChiSq" )  .emplace_back(Chi2);
        }


        // Create a xAOD::Vertex instance
        xAOD::Vertex *vertex { nullptr };
        
        if( m_jp.FillIntermediateVertices ) {
          vertex = new xAOD::Vertex;
          twoTrksVertexContainer->emplace_back( vertex );

          for( auto *trk: ListBaseTracks ) {

            // Acquire link to the track
            ElementLink<xAOD::TrackParticleContainer>  trackElementLink( *( dynamic_cast<const xAOD::TrackParticleContainer*>( trk->container() ) ), trk->index() );

            // Register link to the vertex
            vertex->addTrackAtVertex( trackElementLink, 1. );
          }

          vertex->setVertexType( xAOD::VxType::SecVtx );
          vertex->setPosition( FitVertex );
          vertex->setFitQuality( Chi2, 1 ); // Ndof is always 1

          vertex->auxdata<float>("mass")   = Momentum.M();
          vertex->auxdata<float>("pT")     = Momentum.Perp();
          vertex->auxdata<float>("charge") = Charge;
          vertex->auxdata<float>("vPos")   = vPos;
          vertex->auxdata<bool>("isFake")  = true;
        }


        /////////////////////////////

        uint8_t trkiBLHit,trkjBLHit;
        if( !((*itrk)->summaryValue( trkiBLHit,xAOD::numberOfInnermostPixelLayerHits)))  trkiBLHit=0;
        if( !((*jtrk)->summaryValue( trkjBLHit,xAOD::numberOfInnermostPixelLayerHits)))  trkjBLHit=0;

        if( m_jp.FillNtuple ) m_ntupleVars->get< vector<int> >( "All2TrkSumBLHits" ).emplace_back( trkiBLHit + trkjBLHit );

        // track chi2 cut
        if( Chi2 > m_jp.SelVrtChi2Cut)           continue;          /* Bad Chi2 */
        ATH_MSG_VERBOSE(" > extractIncompatibleTracks: 2-track vrt mass/pt "<<Momentum.M()<<","<<Momentum.Perp());


        // fake rejection cuts with track hit pattern consistencies
        if( m_jp.removeFakeVrt && !m_jp.removeFakeVrtLate ) {
          if( !this->passedFakeReject( FitVertex, (*itrk), (*jtrk) ) ) continue;
        }
        
        if( m_jp.FillHist ) dynamic_cast<TH2F*>( m_hists["vPosDist"] )->Fill( FitVertex.perp(), vPos );
        
        if( m_jp.FillNtuple ) {
          // Fill AANT for vertices after fake rejection
          m_ntupleVars->get< unsigned int >( "AfFakVrtNum" )++;
          m_ntupleVars->get< vector<double> >( "AfFakVrtMass" )   .emplace_back(Momentum.M());
          m_ntupleVars->get< vector<double> >( "AfFakVrtPt" )     .emplace_back(Momentum.Perp());
          m_ntupleVars->get< vector<int> >   ( "AfFakVrtCharge" ) .emplace_back(Charge);
          m_ntupleVars->get< vector<double> >( "AfFakVrtX" )      .emplace_back(FitVertex.x());
          m_ntupleVars->get< vector<double> >( "AfFakVrtY" )      .emplace_back(FitVertex.y());
          m_ntupleVars->get< vector<double> >( "AfFakVrtZ" )      .emplace_back(FitVertex.z());
          m_ntupleVars->get< vector<double> >( "AfFakVrtChiSq" )  .emplace_back(Chi2);
        }

        // The vertex passed the quality cut: overwrite isFake to false
        if( m_jp.FillIntermediateVertices && vertex ) {
          vertex->auxdata<bool>("isFake")  = false;
        }

        
        if( m_jp.doPVcompatibilityCut ) {
          if( vPos < m_jp.pvCompatibilityCut ) continue;
        }

        // Removefrom the incompatible track list only after passing all criteria
        m_incomp.pop_back();
        m_incomp.pop_back();
      }
    }

    return StatusCode::SUCCESS;
  }


  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::reconstruct2TrackVertices( std::vector<WrkVrt> *workVerticesContainer )
  {
    ATH_MSG_DEBUG(" > reconstruct2TrackVertices(): begin");

    // Graph method: Trk::pgraphm_()
    // used in order to find compatible sub-graphs from the incompatible graph

    // List of edgeds between imcompatible nodes
    long int* weit     = new long int[ m_incomp.size() ];
    for(size_t i=0; i<m_incomp.size(); i++) {
      weit[i] = static_cast<long int>( m_incomp[i]+ 1 ); /* +1 is needed for PGRAPH*/
    }

    // Solution of the graph method routine (minimal covering of the graph)
    // The size of the solution is returned by NPTR (see below)
    long int* Solution = new long int[ m_selectedTracks->size() ];

    // Number of edges in the list
    // This is two-folded (a pair of incompatible tracks composes an edge).
    long int edges = m_incomp.size()/2; 

    // input number of nodes in the graph.
    long int NTracks = static_cast<long int>( m_selectedTracks->size() );

    // Input variable; the threshold. Solutions shorter than nth are not returned (ignored).
    long int nth=2;    //VK some speed up

    // NPTR: I/O variable (Destructive FORTRAN Style!!!)
    // - on input:   =0 for initialization, >0 to get next solution
    // - on output:  >0 : length of the solution stored in set; =0 : no more solutions can be found
    long int NPTR=0;

    // This is just a unused strawman needed for m_fitSvc->VKalVrtFit()
    vector<const xAOD::TrackParticle*>    ListBaseTracks;
    vector<const xAOD::NeutralParticle*>  dummyNeutrals(0);

    m_fitSvc->setDefault();

    // Main iteration
    while(true) {

      // varaible of new vertex
      WrkVrt newvrt;

      // Find a solution from the given set of incompatible tracks (==weit)
      Trk::pgraphm_( weit, &edges, &NTracks, Solution, &NPTR, &nth);

      ATH_MSG_VERBOSE(" > reconstruct2TrackVertices(): Trk::pgraphm_() output: NPTR = " << NPTR );

      if(NPTR <= 0)  break;      // No more solutions ==> Exit
      if(NPTR == 1)  continue;   // i.e. single node  ==> Not a good solution

      ListBaseTracks.clear();

      // Try to compose a new vertex using the solution nodes
      // Here the track ID is labelled with array
      newvrt.isGood = true;
      newvrt.selectedTrackIndices.clear();
      for(int i=0; i<NPTR; i++) {
        newvrt.selectedTrackIndices.emplace_back(Solution[i]-1);
        ListBaseTracks.emplace_back( m_selectedTracks->at(Solution[i]-1) );
      }

      // Perform vertex fitting
      StatusCode sc =m_fitSvc->VKalVrtFit(ListBaseTracks,dummyNeutrals,
                                          newvrt.vertex,
                                          newvrt.vertexMom,
                                          newvrt.Charge,
                                          newvrt.vertexCov,
                                          newvrt.Chi2PerTrk, 
                                          newvrt.TrkAtVrt,
                                          newvrt.Chi2);

      ATH_MSG_DEBUG(" > reconstruct2TrackVertices(): FoundAppVrt="<<NPTR<<", "<<newvrt.vertex[0]<<", "<<newvrt.vertex[1]<<
          ", "<<newvrt.vertex[2]<<", "<<newvrt.Chi2);

      if( sc.isFailure() )  {
        ATH_MSG_VERBOSE(" > reconstruct2TrackVertices(): VKalVrtFit failed; continue.");
        continue;   /* Bad fit - goto next solution */
      }

      ATH_MSG_VERBOSE(" > reconstruct2TrackVertices(): VKalVrtFit succeeded; register the vertex to the list.");
      newvrt.isGood         = true;
      newvrt.closestWrkVrtIndex    = 0;
      newvrt.closestWrkVrtValue    = AlgConsts::maxValue;
      workVerticesContainer->emplace_back(newvrt);

    } 

    //-------------------------------------------------------
    // Iterative cleanup algorithm

    //-Remove worst track from vertices with very bad Chi2
    ATH_MSG_VERBOSE(" > reconstruct2TrackVertices(): Remove worst track from vertices with very bad Chi2.");
    for(size_t iv=0; iv<workVerticesContainer->size(); iv++) {
        auto& wrkvrt = workVerticesContainer->at(iv);
        
        if( wrkvrt.Chi2 > (4.*wrkvrt.selectedTrackIndices.size()) ) {
          StatusCode sc = disassembleVertex( workVerticesContainer, iv );
          if( sc.isFailure() ) {}
        }
        
    }

    //-Remove vertices fully contained in other vertices 
    ATH_MSG_VERBOSE(" > reconstruct2TrackVertices(): Remove vertices fully contained in other vertices .");
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
    ATH_MSG_VERBOSE(" > reconstruct2TrackVertices(): Identify remaining 2-track vertices with very bad Chi2 and mass (b-tagging).");
    for( auto& wrkvrt : *workVerticesContainer ) {
      if( wrkvrt.selectedTrackIndices.size() != 2 ) continue;
      if( wrkvrt.Chi2 > 15.) wrkvrt.isGood=false;
    }

    //-Remove all bad vertices from the working set    
    ATH_MSG_VERBOSE(" > reconstruct2TrackVertices(): Remove all bad vertices from the working set.");
    { 
      auto end = std::remove_if( workVerticesContainer->begin(), workVerticesContainer->end(), []( WrkVrt& wrkvrt ) { return wrkvrt.isGood == false || wrkvrt.selectedTrackIndices.size() < 2; } );
      workVerticesContainer->erase( end, workVerticesContainer->end() );
    }
    
    ATH_MSG_DEBUG(" > reconstruct2TrackVertices(): Found Initial Solution Set");
    if( m_jp.FillNtuple) m_ntupleVars->get<unsigned int>( "NumInitSecVrt" ) = workVerticesContainer->size();

    delete[] weit;
    delete[] Solution; 

    ATH_MSG_DEBUG(" > reconstruct2TrackVertices(): end.");
        
    return StatusCode::SUCCESS;
  }


  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::reconstructNTrackVertices( std::vector<WrkVrt> *workVerticesContainer )
  {
    
    //
    //  Rearrangement of solutions
    //
    const long int NTracks = static_cast<long int>( m_selectedTracks->size() );

    // TrkInVrt has IDs of each track which can contain array of vertices.
    // e.g. TrkInVrt->at( track_id ).size() gives the number of vertices which use the track [track_id].
    vector< deque<long int> > *TrkInVrt  = new vector< deque<long int> >(NTracks);  

    // FoundMax: unit in [chi2 per track]
    double FoundMax;

    long int SelectedTrack;
    long int SelectedVertex;
    double minSignificance;
    std::pair<unsigned, unsigned> indexPair { AlgConsts::invalidUnsigned, AlgConsts::invalidUnsigned };

    // Fill TrkInVrt with vertex IDs of each track
    trackClassification( workVerticesContainer, TrkInVrt );

    m_fitSvc->setDefault();
    while( (FoundMax = maxOfShared( workVerticesContainer, TrkInVrt, SelectedTrack, SelectedVertex))>0 ) {

      minSignificance = AlgConsts::maxValue;
      if(FoundMax < m_jp.TrackDetachCut) minSignificance = findMinVerticesPair( workVerticesContainer, indexPair, &VrtSecInclusive::significanceBetweenVertices );

      //
      //Choice of action
      if( FoundMax < m_jp.TrackDetachCut && minSignificance < m_jp.VertexMergeCut ) {

        StatusCode sc = mergeVertices( workVerticesContainer->at( indexPair.first ), workVerticesContainer->at( indexPair.second ) );
        
        if( m_jp.FillHist ) { m_hists["mergeType"]->Fill( RECONSTRUCT_NTRK ); }

        if( sc.isFailure() )  continue;                            /* Bad fit - goto next solution */

        while ( findMinVerticesNextPair( workVerticesContainer, indexPair ) < m_jp.VertexMergeCut ) {
          
          sc = mergeVertices( workVerticesContainer->at( indexPair.first ), workVerticesContainer->at( indexPair.second ) );
          if( sc.isFailure() )  break;                            /* Bad fit - goto next solution */
          
          if( m_jp.FillHist ) { m_hists["mergeType"]->Fill( RECONSTRUCT_NTRK ); }
          
        }

        delete TrkInVrt;
        TrkInVrt  = new vector< deque<long int> >(NTracks);  

        trackClassification( workVerticesContainer, TrkInVrt);

      }else{

        removeTrackFromVertex(workVerticesContainer, TrkInVrt, SelectedTrack, SelectedVertex);
        
        WrkVrt& vertex = workVerticesContainer->at(SelectedVertex);
        WrkVrt  vertex_backup = vertex;
        
        StatusCode sc = refitVertex( vertex );
        if( sc.isFailure() ) {
          vertex = vertex_backup;
          continue;                            /* Bad fit - goto next solution */
        }
      }

    }

    //
    // Try to improve vertices with big Chi2
    for(int iv=0; iv<(int)workVerticesContainer->size(); iv++) {
      
      auto& vertex = workVerticesContainer->at(iv);

      if(!vertex.isGood )                 continue;  //don't work on vertex which is already bad
      if( vertex.selectedTrackIndices.size()<3 )      continue;

      double chi2Probability = TMath::Prob( vertex.Chi2, 2*vertex.selectedTrackIndices.size()-3 ); //Chi2 of the original vertex

      if( chi2Probability < AlgConsts::minVertexChi2Probability ) {
        improveVertexChi2( workVerticesContainer->at(iv) );
      }

    }
    //
    //-----------------------------------------------------------------------------------------------
    //
    if( m_jp.FillNtuple ) {
      m_ntupleVars->get<unsigned int>( "NumRearrSecVrt" )=workVerticesContainer->size();
      ATH_MSG_DEBUG(" > reconstructNTrackVertices: Size of Solution Set: "<< m_ntupleVars->get<unsigned int>( "NumRearrSecVrt" ));
    }

    delete TrkInVrt; 

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
          ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": track " << trk << " --> none : min distance = " << min_distance );
        } else {
          ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": track " << trk << " --> " << &( *(mergiableVertex[index]) ) << " --> size = " << mergiableVertex[index]->selectedTrackIndices.size() << ": min distance = " << min_distance );
        }
        
      }
      
      size_t count_mergiable = std::count_if( mergiableVertex.begin(), mergiableVertex.end(),
                                              [&](std::pair<unsigned, std::vector<WrkVrt>::reverse_iterator> p ) {
                                                return p.second != workVerticesContainer->rend(); } );
      
      if( mergiableVerticesSet.size() == 1 && count_mergiable == workVertex.selectedTrackIndices.size() ) {
        
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": identified a unique association destination vertex" );
        
        WrkVrt& destination = *( mergiableVertex.begin()->second );
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": destination #tracks before merging = " << destination.selectedTrackIndices.size() );
        
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
    
    ATH_MSG_DEBUG( " >> associateNonSeletedTracks: #verticess = " << workVerticesContainer->size() );
    
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
      
      
      ATH_MSG_DEBUG( " >> associateNonSeletedTracks: vertex pos = (" << vertexPos.x() << ", " << vertexPos.y() << ", " << vertexPos.z() << "), "
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
        
        ATH_MSG_DEBUG( " >> associateNonSeletedTracks: trk " << trk
                       << ": d0 to vtx = " << impactParameters.at(k_d0)
                       << ", z0 to vtx = " << impactParameters.at(k_z0)
                       << ", distance to vtx = " << hypot( impactParameters.at(k_d0), impactParameters.at(k_z0) ) );
        
        candidates.emplace_back( trk );
        
      }
      
      ATH_MSG_DEBUG( " >> associateNonSeletedTracks: number of candidate tracks = " << candidates.size() );
      
      // Attempt to add the track to the vertex and try fitting
      for( const auto* trk : candidates ) {
        
        ATH_MSG_DEBUG( " >> associateNonSeletedTracks: attempting to associate track = " << trk );
        
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
        
          if( sc.isFailure() ) ATH_MSG_DEBUG(" >> associateNonSeletedTracks: fast crude estimation failed.");
        
          const auto& diffPos = initPos - vertexPos;
        
          if( diffPos.norm() > 10. ) {
        
            ATH_MSG_VERBOSE( " >> associateNonSeletedTracks: approx vertex as original" );
            m_fitSvc->setApproximateVertex( vertexPos.x(), vertexPos.y(), vertexPos.z() );
          
          } else {
          
            ATH_MSG_VERBOSE( " >> associateNonSeletedTracks: approx vertex set to (" << initPos.x() << ", " << initPos.y() << ", " << initPos.z() << ")" );
            m_fitSvc->setApproximateVertex( initPos.x(), initPos.y(), initPos.z() );
          
          }
        }
        
        
        ATH_MSG_VERBOSE( " >> associateNonSeletedTracks: now vertex fitting..." );
        
        StatusCode sc = m_fitSvc->VKalVrtFit(baseTracks, dummyNeutrals,
                                             workVertex.vertex,
                                             workVertex.vertexMom,
                                             workVertex.Charge,
                                             workVertex.vertexCov,
                                             workVertex.Chi2PerTrk, 
                                             workVertex.TrkAtVrt,
                                             workVertex.Chi2            );
        
        if( sc.isFailure() ) {
          ATH_MSG_DEBUG(" >> associateNonSeletedTracks: VKalVrtFit failure. Revert to backup");
          workVertex = wrkvrt_backup;
          
          if( m_jp.FillHist ) m_hists["associateMonitor"]->Fill( 1 );
                                
          continue;
        }
        
        if( workVertex.Chi2 / (2.0*baseTracks.size()-3.0) > 1.e4 ) {
          
          if( m_jp.FillHist ) m_hists["associateMonitor"]->Fill( 2 );
          
          workVertex = wrkvrt_backup;
          continue;
        }
        
        if( m_jp.FillHist ) m_hists["associateMonitor"]->Fill( 0 );

        auto& cov = workVertex.vertexCov;
        
        ATH_MSG_DEBUG( " >> associateNonSeletedTracks: succeeded in associating. New vertex pos = (" << vertexPos.perp() << ", " << vertexPos.z() << ", " << vertexPos.perp()*vertexPos.phi() << ")" );
        ATH_MSG_DEBUG( " >> associateNonSeletedTracks: New vertex cov = (" << cov.at(0) << ", " << cov.at(1) << ", " << cov.at(2) << ", " << cov.at(3) << ", " << cov.at(4) << ", " << cov.at(5) << ")" );
        
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
        if( !mergeFlag ) {
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
        if( !mergeFlag ) {
          
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
        if( !mergeFlag ) {
          
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
            if( sc.isFailure() || wrkvrt.Chi2 / (2.0*(wrkvrt.selectedTrackIndices.size() + wrkvrt.associatedTrackIndices.size())-3.0) > 1.e4 ) {
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
      int ndof = 2*( wrkvrt.selectedTrackIndices.size() + wrkvrt.associatedTrackIndices.size() ) - 3;
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
    if( m_jp.doAugmentDVimpactParametersToMuons ) {
      
      ATH_CHECK( augmentDVimpactParametersToMuons() );
      
    }

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
        int ndof = 2*( wrkvrt.selectedTrackIndices.size() + wrkvrt.associatedTrackIndices.size() ) - 3;
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
        dynamic_cast<TH2F*>( m_hists["vertexYieldChi2"] )->Fill( vertex.Chi2/( (ntrk*2 - 3 ) + AlgConsts::infinitesimal), m_vertexingAlgorithmStep );
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
