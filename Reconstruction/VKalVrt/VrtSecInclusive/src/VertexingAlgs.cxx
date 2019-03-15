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
      ATH_CHECK( evtStore()->retrieve( twoTrksVertexContainer, "VrtSecInclusive_" + m_jp.all2trksVerticesContainerName + m_jp.augVerString ) );
    }
    
    m_incomp.clear();
   
    // Work variables
    std::vector<const xAOD::TrackParticle*>    baseTracks;
    std::vector<const xAOD::NeutralParticle*>  dummyNeutrals;
   
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": Selected Tracks = "<< m_selectedTracks->size());
    if( m_jp.FillHist ) { m_hists["selTracksDist"]->Fill( m_selectedTracks->size() ); }
    
    std::string msg;
    
    enum recoStep { kStart, kInitVtxPosition, kImpactParamCheck, kVKalVrtFit, kChi2, kVposCut, kPatternMatch };
    
    const double maxR { 563. };         // r = 563 mm is the TRT inner surface
    const double roughD0Cut { 100. };
    const double roughZ0Cut { 50.  };
    
    // Truth match map
    std::map<const xAOD::TruthVertex*, bool> matchMap;

    // first make all 2-track vertices
    for( auto itrk = m_selectedTracks->begin(); itrk != m_selectedTracks->end(); ++itrk ) {
      for( auto jtrk = std::next(itrk); jtrk != m_selectedTracks->end(); ++jtrk ) {
        
        // avoid both tracks are too close to the beam line
        
        const int itrk_id = itrk - m_selectedTracks->begin();
        const int jtrk_id = jtrk - m_selectedTracks->begin();
        
        WrkVrt wrkvrt;
        wrkvrt.selectedTrackIndices.emplace_back( itrk_id );
        wrkvrt.selectedTrackIndices.emplace_back( jtrk_id );
        
        if( fabs( (*itrk)->d0() ) < m_jp.twoTrkVtxFormingD0Cut && fabs( (*jtrk)->d0() ) < m_jp.twoTrkVtxFormingD0Cut ) continue;

        // Attempt to think the combination is incompatible by default
        m_incomp.emplace_back( std::pair<int, int>(itrk_id, jtrk_id) );

        baseTracks.clear();
        baseTracks.emplace_back( *itrk );
        baseTracks.emplace_back( *jtrk );

        if( m_jp.FillHist ) m_hists["incompMonitor"]->Fill( kStart );
        
        // new code to find initial approximate vertex
        Amg::Vector3D initVertex;

        StatusCode sc = m_fitSvc->VKalVrtFitFast( baseTracks, initVertex );/* Fast crude estimation */
        if( sc.isFailure() ) {
          ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": fast crude estimation fails ");
          continue;
        }
        
        if( initVertex.perp() > maxR ) {
          continue;
        }
        if( m_jp.FillHist ) m_hists["incompMonitor"]->Fill( kInitVtxPosition );

        std::vector<double> impactParameters;
        std::vector<double> impactParErrors;
        
        m_fitSvc->VKalGetImpact( *itrk, initVertex, static_cast<long int>( (*itrk)->charge() ), impactParameters, impactParErrors);
        const auto roughD0_itrk = impactParameters.at(0);
        const auto roughZ0_itrk = impactParameters.at(1);
        if( fabs( impactParameters.at(0) ) > roughD0Cut || fabs( impactParameters.at(1) ) > roughZ0Cut ) {
          continue;
        }
        m_fitSvc->VKalGetImpact( *jtrk, initVertex, static_cast<long int>( (*jtrk)->charge() ), impactParameters, impactParErrors);
        const auto roughD0_jtrk = impactParameters.at(0);
        const auto roughZ0_jtrk = impactParameters.at(1);
        if( fabs( impactParameters.at(0) ) > roughD0Cut || fabs( impactParameters.at(1) ) > roughZ0Cut ) {
          continue;
        }
        if( m_jp.FillHist ) m_hists["incompMonitor"]->Fill( kImpactParamCheck );
        
        m_fitSvc->setApproximateVertex( initVertex.x(), initVertex.y(), initVertex.z() );
        
        // Vertex VKal Fitting
        sc = m_fitSvc->VKalVrtFit( baseTracks,
                                   dummyNeutrals,
                                   wrkvrt.vertex, wrkvrt.vertexMom, wrkvrt.Charge,
                                   wrkvrt.vertexCov, wrkvrt.Chi2PerTrk,
                                   wrkvrt.TrkAtVrt, wrkvrt.Chi2  );
        
        if( sc.isFailure() ) {
          continue;          /* No fit */ 
        }
        if( m_jp.FillHist ) m_hists["incompMonitor"]->Fill( kVKalVrtFit );
        
        // Compatibility to the primary vertex.
        Amg::Vector3D vDist = wrkvrt.vertex - m_thePV->position();
        const double vPos = ( vDist.x()*wrkvrt.vertexMom.Px()+vDist.y()*wrkvrt.vertexMom.Py()+vDist.z()*wrkvrt.vertexMom.Pz() )/wrkvrt.vertexMom.Rho();
        const double vPosMomAngT = ( vDist.x()*wrkvrt.vertexMom.Px()+vDist.y()*wrkvrt.vertexMom.Py() ) / vDist.perp() / wrkvrt.vertexMom.Pt();
        
        double dphi1 = vDist.phi() - (*itrk)->phi(); while( dphi1 > TMath::Pi() ) { dphi1 -= TMath::TwoPi(); } while( dphi1 < -TMath::Pi() ) { dphi1 += TMath::TwoPi(); }
        double dphi2 = vDist.phi() - (*itrk)->phi(); while( dphi2 > TMath::Pi() ) { dphi2 -= TMath::TwoPi(); } while( dphi2 < -TMath::Pi() ) { dphi2 += TMath::TwoPi(); }
        
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
        
        if( wrkvrt.fitQuality() > m_jp.SelVrtChi2Cut) {
          ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": failed to pass chi2 threshold." );
          continue;          /* Bad Chi2 */
        }
        if( m_jp.FillHist ) m_hists["incompMonitor"]->Fill( kChi2 );
        
        
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": attempting form vertex from ( " << itrk_id << ", " << jtrk_id << " )." );
        ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": candidate vertex: "
                       << " isGood  = "            << (wrkvrt.isGood? "true" : "false")
                       << ", #ntrks = "            << wrkvrt.nTracksTotal()
                       << ", #selectedTracks = "   << wrkvrt.selectedTrackIndices.size()
                       << ", #associatedTracks = " << wrkvrt.associatedTrackIndices.size()
                       << ", chi2/ndof = "         << wrkvrt.fitQuality()
                       << ", (r, z) = ("           << wrkvrt.vertex.perp()
                       <<", "                      << wrkvrt.vertex.z() << ")" );
        
        for( const auto* truthVertex : m_tracingTruthVertices ) {
          Amg::Vector3D vTruth( truthVertex->x(), truthVertex->y(), truthVertex->z() );
          Amg::Vector3D vReco ( wrkvrt.vertex.x(), wrkvrt.vertex.y(), wrkvrt.vertex.z() );
          
          const auto distance = vReco - vTruth;
          
          AmgSymMatrix(3) cov;
          cov.fillSymmetric( 0, 0, wrkvrt.vertexCov.at(0) );
          cov.fillSymmetric( 1, 0, wrkvrt.vertexCov.at(1) );
          cov.fillSymmetric( 1, 1, wrkvrt.vertexCov.at(2) );
          cov.fillSymmetric( 2, 0, wrkvrt.vertexCov.at(3) );
          cov.fillSymmetric( 2, 1, wrkvrt.vertexCov.at(4) );
          cov.fillSymmetric( 2, 2, wrkvrt.vertexCov.at(5) );

          const double s2 = distance.transpose() * cov.inverse() * distance;
          
          if( distance.norm() < 2.0 || s2 < 100. )  {
            ATH_MSG_DEBUG ( " > " << __FUNCTION__ << ": truth-matched candidate! : signif^2 = " << s2 );
            matchMap.emplace( truthVertex, true );
          }
        }
        
        if( m_jp.FillHist ) {
          dynamic_cast<TH2F*>( m_hists["vPosDist"] )->Fill( wrkvrt.vertex.perp(), vPos );
          dynamic_cast<TH2F*>( m_hists["vPosMomAngTDist"] )->Fill( wrkvrt.vertex.perp(), vPosMomAngT );
        }
        
        if( m_jp.doPVcompatibilityCut ) {
          if( cos( dphi1 ) < -0.8 && cos( dphi2 ) < -0.8 ) {
            ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": failed to pass the vPos cut. (both tracks are opposite against the vertex pos)" );
            continue;
          }
          if( vPosMomAngT < -0.8 ) {
            ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": failed to pass the vPos cut. (pos-mom directions are opposite)" );
            continue;
          }
          if( vPos < m_jp.pvCompatibilityCut ) {
            ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": failed to pass the vPos cut." );
            continue;
          }
        }
        if( m_jp.FillHist ) m_hists["incompMonitor"]->Fill( kVposCut );
        
        // fake rejection cuts with track hit pattern consistencies
        if( m_jp.removeFakeVrt && !m_jp.removeFakeVrtLate ) {
          if( !this->passedFakeReject( wrkvrt.vertex, (*itrk), (*jtrk) ) ) {
            
            ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": failed to pass fake rejection algorithm." );
            continue;
          }
        }
        if( m_jp.FillHist ) m_hists["incompMonitor"]->Fill( kPatternMatch );
        
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": passed fake rejection." );
        
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

        
        // Now this vertex passed all criteria and considred to be a compatible vertices.
        // Therefore the track pair is removed from the incompatibility list.
        m_incomp.pop_back();
        
        wrkvrt.isGood = true;
        
        workVerticesContainer->emplace_back( wrkvrt );
        
        msg += Form(" (%d, %d), ", itrk_id, jtrk_id );
        
        if( m_jp.FillHist ) {
          m_hists["initVertexDispD0"]->Fill( roughD0_itrk, initVertex.perp() );
          m_hists["initVertexDispD0"]->Fill( roughD0_jtrk, initVertex.perp() );
          m_hists["initVertexDispZ0"]->Fill( roughZ0_itrk, initVertex.z()    );
          m_hists["initVertexDispZ0"]->Fill( roughZ0_jtrk, initVertex.z()    );
        }
        
      }
    }
    
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": compatible track pairs = " << msg );
    
    if( m_jp.FillNtuple ) m_ntupleVars->get<unsigned int>( "SizeIncomp" ) = m_incomp.size();
    
    if( m_jp.FillHist ) {
      for( auto& pair: matchMap ) {
        if( pair.second ) m_hists["nMatchedTruths"]->Fill( 1, pair.first->perp() );
      }
    }
    
    return StatusCode::SUCCESS;
  }


  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::findNtrackVertices( std::vector<WrkVrt> *workVerticesContainer )
  {
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": begin");
    
    const auto compSize = m_selectedTracks->size()*(m_selectedTracks->size() - 1)/2 - m_incomp.size();
    if( m_jp.FillHist ) { m_hists["2trkVerticesDist"]->Fill( compSize ); }
    
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": compatible track pair size   = " << compSize );
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": incompatible track pair size = " << m_incomp.size() );
    
    
    if( true /*compSize < 500*/ ) {
      
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
          msg += Form( "%ld, ", solution[i]-1 );
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
        Amg::Vector3D initVertex;
        
        StatusCode sc = m_fitSvc->VKalVrtFitFast( baseTracks, initVertex );/* Fast crude estimation */
        if(sc.isFailure()) ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": fast crude estimation fails ");

        m_fitSvc->setDefault();
        m_fitSvc->setApproximateVertex( initVertex.x(), initVertex.y(), initVertex.z() );
        
        sc = m_fitSvc->VKalVrtFit(baseTracks, dummyNeutrals,
                                  wrkvrt.vertex,
                                  wrkvrt.vertexMom,
                                  wrkvrt.Charge,
                                  wrkvrt.vertexCov,
                                  wrkvrt.Chi2PerTrk, 
                                  wrkvrt.TrkAtVrt,
                                  wrkvrt.Chi2);

        ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": FoundAppVrt=" << solutionSize << ", (r, z) = " << wrkvrt.vertex.perp() << ", " << wrkvrt.vertex.z()  <<  ", chi2/ndof = "  <<  wrkvrt.fitQuality() );

        if( sc.isFailure() )  {
          
          if( wrkvrt.selectedTrackIndices.size() <= 2 ) {
            ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": VKalVrtFit failed in 2-trk solution ==> give up.");
            continue;
          }
          
          ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": VKalVrtFit failed ==> retry...");
          
          WrkVrt tmp;
          tmp.isGood = false;
          
          // Create 2-trk vertex combination and find any compatible vertex
          for( auto& itrk: wrkvrt.selectedTrackIndices ) {
            for( auto& jtrk: wrkvrt.selectedTrackIndices ) {
              if( itrk == jtrk ) continue;
              if( tmp.isGood ) continue;
              
              tmp.selectedTrackIndices.clear();
              tmp.selectedTrackIndices.emplace_back( itrk );
              tmp.selectedTrackIndices.emplace_back( jtrk );
              
              baseTracks.clear();
              baseTracks.emplace_back( m_selectedTracks->at( itrk ) );
              baseTracks.emplace_back( m_selectedTracks->at( jtrk ) );
              
              // Perform vertex fitting
              Amg::Vector3D initVertex;
        
              sc = m_fitSvc->VKalVrtFitFast( baseTracks, initVertex );
              if( sc.isFailure() ) ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": fast crude estimation fails ");
              
              m_fitSvc->setDefault();
              m_fitSvc->setApproximateVertex( initVertex.x(), initVertex.y(), initVertex.z() );
              
              sc = m_fitSvc->VKalVrtFit(baseTracks, dummyNeutrals,
                                        tmp.vertex,
                                        tmp.vertexMom,
                                        tmp.Charge,
                                        tmp.vertexCov,
                                        tmp.Chi2PerTrk, 
                                        tmp.TrkAtVrt,
                                        tmp.Chi2);
              
              if( sc.isFailure() ) continue;
              
              tmp.isGood = true;
              
            }
          }
          
          if( !tmp.isGood ) {
            ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": Did not find any viable vertex in all 2-trk combinations. Give up.");
            continue;
          }
          
          // Now, found at least one seed 2-track vertex. ==> attempt to attach other tracks
          for( auto& itrk: wrkvrt.selectedTrackIndices ) {
            
            if( std::find( tmp.selectedTrackIndices.begin(), tmp.selectedTrackIndices.end(), itrk ) != tmp.selectedTrackIndices.end() ) continue;
            
            auto backup = tmp;
            
            tmp.selectedTrackIndices.emplace_back( itrk );
            baseTracks.clear();
            for( auto& jtrk : tmp.selectedTrackIndices ) { baseTracks.emplace_back( m_selectedTracks->at(jtrk) ); }
            
            // Perform vertex fitting
            Amg::Vector3D initVertex;
        
            sc = m_fitSvc->VKalVrtFitFast( baseTracks, initVertex );/* Fast crude estimation */
            if(sc.isFailure()) ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": fast crude estimation fails ");
              
            m_fitSvc->setDefault();
            m_fitSvc->setApproximateVertex( initVertex.x(), initVertex.y(), initVertex.z() );
              
            sc = m_fitSvc->VKalVrtFit(baseTracks, dummyNeutrals,
                                      tmp.vertex,
                                      tmp.vertexMom,
                                      tmp.Charge,
                                      tmp.vertexCov,
                                      tmp.Chi2PerTrk, 
                                      tmp.TrkAtVrt,
                                      tmp.Chi2);
              
            if( sc.isFailure() ) {
              tmp = backup;
              continue;
            }
            
          }
          
          wrkvrt = tmp;
          ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": VKalVrtFit succeeded; register the vertex to the list.");
          wrkvrt.isGood                = true;
          wrkvrt.closestWrkVrtIndex    = AlgConsts::invalidUnsigned;
          wrkvrt.closestWrkVrtValue    = AlgConsts::maxValue;
          workVerticesContainer->emplace_back( wrkvrt );
          
        } else {
        
          ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": VKalVrtFit succeeded; register the vertex to the list.");
          wrkvrt.isGood                = true;
          wrkvrt.closestWrkVrtIndex    = AlgConsts::invalidUnsigned;
          wrkvrt.closestWrkVrtValue    = AlgConsts::maxValue;
          workVerticesContainer->emplace_back( wrkvrt );
          
        }

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
        
        // Perform vertex fitting
        Amg::Vector3D initVertex;
        
        StatusCode sc = m_fitSvc->VKalVrtFitFast( baseTracks, initVertex );/* Fast crude estimation */
        if(sc.isFailure()) ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": fast crude estimation fails ");

        m_fitSvc->setDefault();
        m_fitSvc->setApproximateVertex( initVertex.x(), initVertex.y(), initVertex.z() );
        
        sc = m_fitSvc->VKalVrtFit(baseTracks, dummyNeutrals,
                                  wrkvrt.vertex,
                                  wrkvrt.vertexMom,
                                  wrkvrt.Charge,
                                  wrkvrt.vertexCov,
                                  wrkvrt.Chi2PerTrk, 
                                  wrkvrt.TrkAtVrt,
                                  wrkvrt.Chi2);

        if( sc.isFailure() ) {
          continue;
        }
        
        workVerticesContainer->emplace_back( wrkvrt );
      }
      
    }

    //-------------------------------------------------------
    // Iterative cleanup algorithm

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
      
      if( TMath::Prob( wrkvrt.Chi2, wrkvrt.ndof() ) < m_jp.improveChi2ProbThreshold ) wrkvrt.isGood = false;
      if( wrkvrt.selectedTrackIndices.size() != 2 ) continue;
      if( m_jp.FillHist ) m_hists["NtrkChi2Dist"]->Fill( log10( wrkvrt.fitQuality() ) );
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
    
    unsigned mergeCounter { 0 };
    unsigned brokenCounter { 0 };
    unsigned removeTrackCounter { 0 };

    while( true ) {
      
      // worstChi2: unit in [chi2 per track]
      long int maxSharedTrack;
      long int worstMatchingVertex;
      std::pair<unsigned, unsigned> indexPair { AlgConsts::invalidUnsigned, AlgConsts::invalidUnsigned };
    
    
      // trackToVertexMap has IDs of each track which can contain array of vertices.
      // e.g. TrkInVrt->at( track_id ).size() gives the number of vertices which use the track [track_id].
      
      std::map<long int, std::vector<long int> > trackToVertexMap;
      
      // Fill trackToVertexMap with vertex IDs of each track
      trackClassification( workVerticesContainer, trackToVertexMap );
      
      
      auto worstChi2 = findWorstChi2ofMaximallySharedTrack( workVerticesContainer, trackToVertexMap, maxSharedTrack, worstMatchingVertex );
      
      if( worstChi2 == AlgConsts::invalidFloat ) {
        ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": no shared tracks are found --> exit the while loop." );
        break;
      }
      
      ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": vertex [" << worstMatchingVertex << "]: maximally shared track index = " << maxSharedTrack
                     << ", multiplicity = "   << trackToVertexMap.at( maxSharedTrack ).size()
                     << ", worst chi2_trk = " << worstChi2 );
            
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
            
            ATH_MSG_VERBOSE( " > " << __FUNCTION__ << ": attempt to merge vertices " << indexPair.first << " and " << indexPair.second );
            
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
            
            mergeCounter++;
            
            ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": Merged vertices " << indexPair.first << " and " << indexPair.second << ". merged vertex multiplicity = " << workVerticesContainer->at( indexPair.first ).selectedTrackIndices.size() );
            
          } else {
            
            // Here, the significance between closest vertices sharing the track is sufficiently distant
            // and cannot be merged, while the track-association chi2 is small as well.
            // In order to resolve the ambiguity anyway, remove the track from the worst-associated vertex.
            
            auto& wrkvrt = workVerticesContainer->at( worstMatchingVertex );
            
            auto end = std::remove_if( wrkvrt.selectedTrackIndices.begin(), wrkvrt.selectedTrackIndices.end(), [&]( auto& index ) { return index == maxSharedTrack; } );
            wrkvrt.selectedTrackIndices.erase( end, wrkvrt.selectedTrackIndices.end() );
            
            removeTrackCounter++;
            
            ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": removed track " << maxSharedTrack << " from vertex " << worstMatchingVertex );
            
            if( wrkvrt.selectedTrackIndices.size() < 2 ) {
              wrkvrt.isGood = false;
              brokenCounter++;
              break;
            }
            
            StatusCode sc = refitVertex( wrkvrt );
            if( sc.isFailure() ) {
              ATH_MSG_WARNING(" > " << __FUNCTION__ << ": detected vertex fitting failure!" );
            }
            
            break;
            
          }
        }
        
      } else {
        
        // Here, a bad track association is detected
        // The track is detached from the worst-associated vertex and refit.
        
        auto& wrkvrt = workVerticesContainer->at( worstMatchingVertex );
        
        auto end = std::remove_if( wrkvrt.selectedTrackIndices.begin(), wrkvrt.selectedTrackIndices.end(), [&]( auto& index ) { return index == maxSharedTrack; } );
        wrkvrt.selectedTrackIndices.erase( end, wrkvrt.selectedTrackIndices.end() );
        
        if( wrkvrt.nTracksTotal() >=2 ) {
          
          auto wrkvrt_backup = wrkvrt;
          StatusCode sc = refitVertex( wrkvrt );
          if( sc.isFailure() ) {
            ATH_MSG_WARNING(" > " << __FUNCTION__ << ": detected vertex fitting failure!" );
            wrkvrt = wrkvrt_backup;
          }
          
        } else {
          wrkvrt.isGood = false;
          brokenCounter++;
        }
        
        removeTrackCounter++;
        
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": removed track " << maxSharedTrack << " from vertex " << worstMatchingVertex );
        
      }
      
    }
    
    //
    // Try to improve vertices with big Chi2
    for( auto& wrkvrt : *workVerticesContainer ) {
      
      if(!wrkvrt.isGood )                          continue;  //don't work on wrkvrt which is already bad
      if( wrkvrt.selectedTrackIndices.size() < 3 ) continue;

      WrkVrt backup = wrkvrt;
      improveVertexChi2( wrkvrt );
      if( wrkvrt.fitQuality() > backup.fitQuality() ) wrkvrt = backup;
      
      if( wrkvrt.nTracksTotal() < 2 ) wrkvrt.isGood = false;

    }
    
    if( m_jp.FillNtuple ) {
      m_ntupleVars->get<unsigned int>( "NumRearrSecVrt" )=workVerticesContainer->size();
      ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": Size of Solution Set: "<< m_ntupleVars->get<unsigned int>( "NumRearrSecVrt" ));
    }

    ATH_MSG_DEBUG(" > " << __FUNCTION__ << "----------------------------------------------" );
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": Number of merges = " << mergeCounter << ", Number of track removal = " << removeTrackCounter << ", broken vertices = " << brokenCounter );
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << "----------------------------------------------" );

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
    
    unsigned reassembleCounter { 0 };
    
    // First, sort WrkVrt by the track multiplicity
    std::sort( workVerticesContainer->begin(), workVerticesContainer->end(), [](WrkVrt& v1, WrkVrt& v2) { return v1.selectedTrackIndices.size() < v2.selectedTrackIndices.size(); } );
    
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": #vertices = " << workVerticesContainer->size() );
    // Loop over vertices (small -> large Ntrk order)
    for( auto& wrkvrt : *workVerticesContainer ) {
      if( !wrkvrt.isGood               ) continue;
      if(  wrkvrt.selectedTrackIndices.size() <= 1 ) continue;
      
      ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": vertex " << &wrkvrt << " #tracks = " << wrkvrt.selectedTrackIndices.size() );
      ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": candidate vertex: "
                     << " isGood  = "            << (wrkvrt.isGood? "true" : "false")
                     << ", #ntrks = "            << wrkvrt.nTracksTotal()
                     << ", #selectedTracks = "   << wrkvrt.selectedTrackIndices.size()
                     << ", #associatedTracks = " << wrkvrt.associatedTrackIndices.size()
                     << ", chi2/ndof = "         << wrkvrt.fitQuality()
                     << ", (r, z) = ("           << wrkvrt.vertex.perp()
                     <<", "                      << wrkvrt.vertex.z() << ")" );
      
      std::map<unsigned, std::vector<WrkVrt>::reverse_iterator> mergiableVertex;
      std::set<std::vector<WrkVrt>::reverse_iterator> mergiableVerticesSet;
      
      for( auto& index : wrkvrt.selectedTrackIndices ) {
        
        const xAOD::TrackParticle* trk = m_selectedTracks->at( index );
        
        mergiableVertex[index] = workVerticesContainer->rend();
        
        std::vector<double> distances;
        
        // Reverse iteration: large Ntrk -> small Ntrk order
        for( auto ritr = workVerticesContainer->rbegin(); ritr != workVerticesContainer->rend(); ++ritr ) {
          auto& targetVertex = *ritr;
          
          if( &wrkvrt == &targetVertex ) continue;
          if( wrkvrt.selectedTrackIndices.size() >= targetVertex.selectedTrackIndices.size() ) continue;
          
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
      
      if( mergiableVerticesSet.size() == 1 && count_mergiable == wrkvrt.selectedTrackIndices.size() ) {
        
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": identified a unique association destination vertex" );
        
        WrkVrt& destination = *( mergiableVertex.begin()->second );
        ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": destination #tracks before merging = " << destination.selectedTrackIndices.size() );
        
        StatusCode sc = mergeVertices( destination, wrkvrt );
        if( sc.isFailure() ) {
          ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": failure in vertex merging" );
        }
        
        improveVertexChi2( destination );
        
        ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": merged destination vertex: "
                       << " isGood  = "            << (destination.isGood? "true" : "false")
                       << ", #ntrks = "            << destination.nTracksTotal()
                       << ", #selectedTracks = "   << destination.selectedTrackIndices.size()
                       << ", #associatedTracks = " << destination.associatedTrackIndices.size()
                       << ", chi2/ndof = "         << destination.fitQuality()
                       << ", (r, z) = ("           << destination.vertex.perp()
                       <<", "                      << destination.vertex.z() << ")" );
        
        if( m_jp.FillHist ) { m_hists["mergeType"]->Fill( REASSEMBLE ); }
        
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": destination #tracks after merging = " << destination.selectedTrackIndices.size() );
        
        reassembleCounter++;
        
      }
          
    }
    
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << "----------------------------------------------" );
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": reassembled vertices = " << reassembleCounter );
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << "----------------------------------------------" );
    
    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::associateNonSelectedTracks( std::vector<WrkVrt>* workVerticesContainer )
  {
    
    const xAOD::TrackParticleContainer *allTracks ( nullptr );
    ATH_CHECK( evtStore()->retrieve(allTracks, "InDetTrackParticles") );
    
    const xAOD::VertexContainer *pvs (nullptr);
    ATH_CHECK( evtStore()->retrieve( pvs, "PrimaryVertices") );
    
    if( !m_decor_isAssociated ) m_decor_isAssociated = std::make_unique< SG::AuxElement::Decorator<char> >( "is_associated" + m_jp.augVerString );
    
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": #verticess = " << workVerticesContainer->size() );
    
    unsigned associateCounter { 0 };
    
    // Loop over vertices
    for( auto& wrkvrt : *workVerticesContainer ) {
      
      if( !wrkvrt.isGood               ) continue;
      if(  wrkvrt.selectedTrackIndices.size() <= 1 ) continue;
      
      improveVertexChi2( wrkvrt );
      
      wrkvrt.Chi2_core = wrkvrt.Chi2;
      
      auto& vertexPos = wrkvrt.vertex;
      
      std::vector<double> distanceToPVs;
      
      for( auto* pv : *pvs ) {
        distanceToPVs.emplace_back( VKalVrtAthena::vtxVtxDistance( vertexPos, pv->position() ) );
      }
      const auto& minDistance = *( std::min_element( distanceToPVs.begin(), distanceToPVs.end() ) );
      
      if( minDistance < m_jp.associateMinDistanceToPV ) continue;
      
      
      ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": vertex pos = (" << vertexPos.x() << ", " << vertexPos.y() << ", " << vertexPos.z() << "), "
                     "#selected = " << wrkvrt.selectedTrackIndices.size() << ", #assoc = " << wrkvrt.associatedTrackIndices.size() );
      
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
        
        // If the track is already registered to the associated track list, reject.
        {
          auto result = std::find_if( m_associatedTracks->begin(), m_associatedTracks->end(),
                                      [&] (const auto* atrk) { return trk == atrk; } );
          if( result != m_associatedTracks->end() ) continue;
        }
        
        // Reject PV-associated tracks
        // if( !selectTrack_notPVassociated( trk ) ) continue;
        
        // pT selection
        if( trk->pt() < m_jp.associatePtCut ) continue;
        
        // chi2 selection
        if( trk->chiSquared() / trk->numberDoF() > m_jp.associateChi2Cut ) continue;
        
        // Hit pattern consistentcy requirement
        if( !checkTrackHitPatternToVertexOuterOnly( trk, vertexPos ) ) continue;
        
        // Get the closest approach
        std::vector<double> impactParameters;
        std::vector<double> impactParErrors;
        
        m_fitSvc->VKalGetImpact(trk, vertexPos, static_cast<int>( trk->charge() ), impactParameters, impactParErrors);
        
        enum { k_d0, k_z0, k_theta, k_phi, k_qOverP };
        enum { k_d0d0, k_d0z0, k_z0z0 };
        
        if( fabs( impactParameters.at(k_d0) ) / sqrt( impactParErrors.at(k_d0d0) ) > m_jp.associateMaxD0Signif ) continue;
        if( fabs( impactParameters.at(k_z0) ) / sqrt( impactParErrors.at(k_z0z0) ) > m_jp.associateMaxZ0Signif ) continue;
        
        ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": trk " << trk
                       << ": d0 to vtx = " << impactParameters.at(k_d0)
                       << ", z0 to vtx = " << impactParameters.at(k_z0)
                       << ", distance to vtx = " << hypot( impactParameters.at(k_d0), impactParameters.at(k_z0) ) );
        
        candidates.emplace_back( trk );
        
      }
      
      ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": number of candidate tracks = " << candidates.size() );
      
      // Attempt to add the track to the vertex and try fitting
      for( const auto* trk : candidates ) {
        
        ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": attempting to associate track = " << trk );
        
        // Backup the current vertes status
        WrkVrt wrkvrt_backup = wrkvrt;
        
        m_fitSvc->setDefault();
        m_fitSvc->setApproximateVertex( vertexPos.x(), vertexPos.y(), vertexPos.z() );
        
        std::vector<const xAOD::TrackParticle*>   baseTracks;
        std::vector<const xAOD::NeutralParticle*> dummyNeutrals;
        
        wrkvrt.Chi2PerTrk.clear();
        
        for( const auto& index : wrkvrt.selectedTrackIndices ) {
          baseTracks.emplace_back( m_selectedTracks->at( index ) );
          wrkvrt.Chi2PerTrk.emplace_back( AlgConsts::chi2PerTrackInitValue );
        }
        
        baseTracks.emplace_back( trk );
        wrkvrt.Chi2PerTrk.emplace_back( AlgConsts::chi2PerTrackInitValue );
        
        Amg::Vector3D initPos;
        
        {
          StatusCode sc = m_fitSvc->VKalVrtFitFast( baseTracks, initPos );/* Fast crude estimation */
        
          if( sc.isFailure() ) ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": fast crude estimation failed.");
        
          const auto& diffPos = initPos - vertexPos;
        
          if( diffPos.norm() > 10. ) {
        
            ATH_MSG_VERBOSE( " > " << __FUNCTION__ << ": approx vertex as original" );
            m_fitSvc->setApproximateVertex( vertexPos.x(), vertexPos.y(), vertexPos.z() );
          
          } else {
          
            ATH_MSG_VERBOSE( " > " << __FUNCTION__ << ": approx vertex set to (" << initPos.x() << ", " << initPos.y() << ", " << initPos.z() << ")" );
            m_fitSvc->setApproximateVertex( initPos.x(), initPos.y(), initPos.z() );
          
          }
        }
        
        
        ATH_MSG_VERBOSE( " > " << __FUNCTION__ << ": now vertex fitting..." );
        
        StatusCode sc = m_fitSvc->VKalVrtFit(baseTracks, dummyNeutrals,
                                             wrkvrt.vertex,
                                             wrkvrt.vertexMom,
                                             wrkvrt.Charge,
                                             wrkvrt.vertexCov,
                                             wrkvrt.Chi2PerTrk, 
                                             wrkvrt.TrkAtVrt,
                                             wrkvrt.Chi2            );
        
        if( sc.isFailure() ) {
          ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": VKalVrtFit failure. Revert to backup");
          wrkvrt = wrkvrt_backup;
          
          if( m_jp.FillHist ) m_hists["associateMonitor"]->Fill( 1 );
                                
          continue;
        }
        
        
        if( m_jp.FillHist ) m_hists["associateMonitor"]->Fill( 0 );
        
        auto& cov = wrkvrt.vertexCov;
        
        ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": succeeded in associating. New vertex pos = (" << vertexPos.perp() << ", " << vertexPos.z() << ", " << vertexPos.perp()*vertexPos.phi() << ")" );
        ATH_MSG_VERBOSE( " > " << __FUNCTION__ << ": New vertex cov = (" << cov.at(0) << ", " << cov.at(1) << ", " << cov.at(2) << ", " << cov.at(3) << ", " << cov.at(4) << ", " << cov.at(5) << ")" );
        
        associateCounter++;
        
        wrkvrt.associatedTrackIndices.emplace_back( m_associatedTracks->size() );
        
        m_associatedTracks->emplace_back( trk );
        (*m_decor_isAssociated)( *trk ) = true;
        
      }
      
    }
    
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << "----------------------------------------------" );
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": total associated number of tracks = " << associateCounter );
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << "----------------------------------------------" );
    
    return StatusCode::SUCCESS;
  }
    
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::mergeByShuffling( std::vector<WrkVrt> *workVerticesContainer )
  {
    
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": #verticess = " << workVerticesContainer->size() );
    
    unsigned mergeCounter { 0 };
    
    // First, sort WrkVrt by the track multiplicity
    std::sort( workVerticesContainer->begin(), workVerticesContainer->end(), [](WrkVrt& v1, WrkVrt& v2) { return v1.selectedTrackIndices.size() < v2.selectedTrackIndices.size(); } );
    
    // Loop over vertices (small -> large Ntrk order)
    for( auto& wrkvrt : *workVerticesContainer ) {
      if( !wrkvrt.isGood               )             continue;
      if(  wrkvrt.selectedTrackIndices.size() <= 1 ) continue;
      
      // Reverse iteration: large Ntrk -> small Ntrk order
      for( auto ritr = workVerticesContainer->rbegin(); ritr != workVerticesContainer->rend(); ++ritr ) {
        auto& vertexToMerge = *ritr;
        
        if( !vertexToMerge.isGood               )                                             continue;
        if(  vertexToMerge.selectedTrackIndices.size() <= 1 )                                 continue;
        if( &wrkvrt == &vertexToMerge     )                                                   continue;
        if(  vertexToMerge.selectedTrackIndices.size() < wrkvrt.selectedTrackIndices.size() ) continue;
        
        const double& significance = significanceBetweenVertices( wrkvrt, vertexToMerge );
        
        if( significance > m_jp.mergeByShufflingMaxSignificance ) continue;
        
        bool mergeFlag { false };
        
        ATH_MSG_DEBUG(" > " << __FUNCTION__ 
                      << ": vertex " << &wrkvrt << " #tracks = " << wrkvrt.selectedTrackIndices.size()
                      << " --> to Merge : " << &vertexToMerge << ", #tracks = " << vertexToMerge.selectedTrackIndices.size()
                      << " significance = " << significance );
        
        double min_signif = AlgConsts::maxValue;
        
        // Method 1. Assume that the solution is somewhat wrong, and the solution gets correct if it starts from the other vertex position
        if( m_jp.doSuggestedRefitOnMerging && !mergeFlag ) {
          WrkVrt testVertex = wrkvrt;
          StatusCode sc = refitVertexWithSuggestion( testVertex, vertexToMerge.vertex );
          if( sc.isFailure() ) {
            //ATH_MSG_WARNING(" > " << __FUNCTION__ << ": detected vertex fitting failure!" );
          } else {
          
            const auto signif = significanceBetweenVertices( testVertex, vertexToMerge );
            if( signif < min_signif ) min_signif = signif;
          
            if( signif < m_jp.mergeByShufflingAllowance ) {
              ATH_MSG_DEBUG(" > " << __FUNCTION__ << ":  method1:  vertexToMerge " << &vertexToMerge << ": test signif = " << signif );
              mergeFlag = true;
            
            }
          
            if( m_jp.FillHist && min_signif > 0. ) m_hists["shuffleMinSignif1"]->Fill( log10( min_signif ) );
            if( m_jp.FillHist && mergeFlag ) { m_hists["mergeType"]->Fill( SHUFFLE1 ); }
          }
        }
        
        // Method 2. magnet merging: borrowing another track from the target vertex to merge
        if( m_jp.doMagnetMerging && !mergeFlag ) {
          
          // Loop over tracks in vertexToMerge
          for( auto& index : vertexToMerge.selectedTrackIndices ) {
          
            WrkVrt testVertex = wrkvrt;
            testVertex.selectedTrackIndices.emplace_back( index );
          
            StatusCode sc = refitVertexWithSuggestion( testVertex, vertexToMerge.vertex );
            if( sc.isFailure() ) {
              //ATH_MSG_WARNING(" > " << __FUNCTION__ << ": detected vertex fitting failure!" );
            } else {
          
              const auto signif = significanceBetweenVertices( testVertex, vertexToMerge );
              if( signif < min_signif ) min_signif = signif;
              
              if( signif < m_jp.mergeByShufflingAllowance ) {
                ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": method2:  vertexToMerge " << &vertexToMerge << " track index " << index << ": test signif = " << signif );
                mergeFlag = true;
              }
              
            }
          }
        
          if( m_jp.FillHist && min_signif > 0. ) m_hists["shuffleMinSignif2"]->Fill( log10( min_signif ) );
          
          if( m_jp.FillHist && mergeFlag ) { m_hists["mergeType"]->Fill( SHUFFLE2 ); }
        }
        
        // Method 3. Attempt to force merge
        if( m_jp.doWildMerging && !mergeFlag ) {
          
          WrkVrt testVertex = wrkvrt;
          
          for( auto& index : vertexToMerge.selectedTrackIndices ) {
            testVertex.selectedTrackIndices.emplace_back( index );
          }
          
          StatusCode sc = refitVertexWithSuggestion( testVertex, vertexToMerge.vertex );
          if( sc.isFailure() ) {
            //ATH_MSG_WARNING(" > " << __FUNCTION__ << ": detected vertex fitting failure!" );
          } else {
          
            const auto signif = significanceBetweenVertices( testVertex, vertexToMerge );
            if( signif < min_signif ) min_signif = signif;
          
            if( signif < m_jp.mergeByShufflingAllowance ) {
              ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": method3:  vertexToMerge " << &vertexToMerge << ": test signif = " << signif );
              mergeFlag = true;
            }
          
            if( m_jp.FillHist && min_signif > 0. ) m_hists["shuffleMinSignif3"]->Fill( log10( min_signif ) );
            if( m_jp.FillHist && mergeFlag ) { m_hists["mergeType"]->Fill( SHUFFLE3 ); }
          
          }
        }
        
        
        if( mergeFlag ) {
          ATH_MSG_DEBUG(" > " << __FUNCTION__ << ":   vertexToMerge " << &vertexToMerge << " ==> min signif = " << min_signif << " judged to merge" );
          
          auto vertexToMerge_backup = vertexToMerge;
          auto wrkvrt_backup        = wrkvrt;
          
          StatusCode sc = mergeVertices( vertexToMerge, wrkvrt );
          if( sc.isFailure() ) {
            vertexToMerge = vertexToMerge_backup;
            wrkvrt        = wrkvrt_backup;
            continue;
          }
          
          improveVertexChi2( wrkvrt );
          
          mergeCounter++;
        }
          
      }
      
    }
    
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << "----------------------------------------------" );
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": Number of merges = " << mergeCounter );
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << "----------------------------------------------" );
    
    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::mergeFinalVertices( std::vector<WrkVrt> *workVerticesContainer )
  {
    
    unsigned mergeCounter { 0 };
    
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
      
      improveVertexChi2( v1 );
      
      mergeCounter++;
      
    }

    ATH_MSG_DEBUG(" > " << __FUNCTION__ << "----------------------------------------------" );
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": Number of merges = " << mergeCounter );
    ATH_MSG_DEBUG(" > " << __FUNCTION__ << "----------------------------------------------" );
    
    return StatusCode::SUCCESS;

  } // end of mergeFinalVertices
  
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::refitAndSelectGoodQualityVertices( std::vector<WrkVrt> *workVerticesContainer )
  {

    // Output SVs as xAOD::Vertex
    // Needs a conversion function from workVerticesContainer to xAOD::Vertex here.
    // The supposed form of the function will be as follows:
    
    try {

    xAOD::VertexContainer *secondaryVertexContainer( nullptr );
    ATH_CHECK( evtStore()->retrieve( secondaryVertexContainer, "VrtSecInclusive_" + m_jp.secondaryVerticesContainerName + m_jp.augVerString ) );
    
    const xAOD::TrackParticleContainer* trackParticleContainer ( nullptr );
    ATH_CHECK( evtStore()->retrieve( trackParticleContainer, m_jp.TrackLocation) );
    
    enum { kPt, kEta, kPhi, kD0, kZ0, kErrP, kErrD0, kErrZ0, kChi2SV };
    if( 0 == m_trkDecors.size() ) {
      m_trkDecors.emplace( kPt,     SG::AuxElement::Decorator<float>("pt_wrtSV"    + m_jp.augVerString) );
      m_trkDecors.emplace( kEta,    SG::AuxElement::Decorator<float>("eta_wrtSV"   + m_jp.augVerString) ); 
      m_trkDecors.emplace( kPhi,    SG::AuxElement::Decorator<float>("phi_wrtSV"   + m_jp.augVerString) );
      m_trkDecors.emplace( kD0,     SG::AuxElement::Decorator<float>("d0_wrtSV"    + m_jp.augVerString) );
      m_trkDecors.emplace( kZ0,     SG::AuxElement::Decorator<float>("z0_wrtSV"    + m_jp.augVerString) );
      m_trkDecors.emplace( kErrP,   SG::AuxElement::Decorator<float>("errP_wrtSV"  + m_jp.augVerString) );
      m_trkDecors.emplace( kErrD0,  SG::AuxElement::Decorator<float>("errd0_wrtSV" + m_jp.augVerString) );
      m_trkDecors.emplace( kErrZ0,  SG::AuxElement::Decorator<float>("errz0_wrtSV" + m_jp.augVerString) );
      m_trkDecors.emplace( kChi2SV, SG::AuxElement::Decorator<float>("chi2_toSV"   + m_jp.augVerString) );
    }
    if( !m_decor_is_svtrk_final ) m_decor_is_svtrk_final = std::make_unique< SG::AuxElement::Decorator<char> >( "is_svtrk_final" + m_jp.augVerString );

    std::map<const WrkVrt*, const xAOD::Vertex*> wrkvrtLinkMap;
    
    //----------------------------------------------------------

    m_fitSvc->setDefault();
    m_fitSvc->setMomCovCalc(1);

    ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": input #vertices = " << workVerticesContainer->size() );
    
    // Loop over vertices
    for( auto& wrkvrt : *workVerticesContainer ) {
      
      ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": candidate vertex: "
                     << " isGood  = "            << (wrkvrt.isGood? "true" : "false")
                     << ", #ntrks = "            << wrkvrt.nTracksTotal()
                     << ", #selectedTracks = "   << wrkvrt.selectedTrackIndices.size()
                     << ", #associatedTracks = " << wrkvrt.associatedTrackIndices.size()
                     << ", chi2/ndof = "         << wrkvrt.Chi2 / ( wrkvrt.ndof() + AlgConsts::infinitesimal )
                     << ", (r, z) = ("           << wrkvrt.vertex.perp()
                     <<", "                      << wrkvrt.vertex.z() << ")" );
      
      if( m_jp.FillHist ) m_hists["finalCutMonitor"]->Fill( 0 );

      if( m_jp.removeFakeVrt && m_jp.removeFakeVrtLate ) {
        removeInconsistentTracks( wrkvrt );
      }
      
      if( wrkvrt.nTracksTotal() < 2 ) {
        ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": ntrk < 2  --> rejected." );
        continue;               /* Bad vertices */
      }

      if( m_jp.FillHist ) m_hists["finalCutMonitor"]->Fill( 1 );
      
      
      // Remove track if the vertex is inner than IBL and the track does not have pixel hits!
      if( wrkvrt.vertex.perp() < 31.0 ) {
        
        // for selected tracks
        wrkvrt.selectedTrackIndices.erase( std::remove_if( wrkvrt.selectedTrackIndices.begin(), wrkvrt.selectedTrackIndices.end(),
                                                           [&]( auto& index ) {
                                                             auto* trk = m_selectedTracks->at( index );
                                                             uint8_t nPixelHits { 0 }; trk->summaryValue( nPixelHits,  xAOD::numberOfPixelHits );
                                                             return ( nPixelHits < 3 );
                                                           } ),
                                           wrkvrt.selectedTrackIndices.end() );
        
        // for associated tracks
        wrkvrt.associatedTrackIndices.erase( std::remove_if( wrkvrt.associatedTrackIndices.begin(), wrkvrt.associatedTrackIndices.end(),
                                                             [&]( auto& index ) {
                                                               auto* trk = m_associatedTracks->at( index );
                                                               uint8_t nPixelHits { 0 }; trk->summaryValue( nPixelHits,  xAOD::numberOfPixelHits );
                                                               return ( nPixelHits < 3 );
                                                             } ),
                                             wrkvrt.associatedTrackIndices.end() );

        auto statusCode = refitVertex( wrkvrt );
        if( statusCode.isFailure() ) {}
        
      }
      
      
      if( m_jp.doFinalImproveChi2 ) {
        
        WrkVrt backup = wrkvrt;
        
        improveVertexChi2( wrkvrt );
        
        if( wrkvrt.fitQuality() > backup.fitQuality() ) wrkvrt = backup;
        
      }
        
      // If the number of remaining tracks is less than 2, drop.
      if( wrkvrt.nTracksTotal() < 2 ) continue;
      
      // Select only vertices with keeping more than 2 selectedTracks
      if( wrkvrt.selectedTrackIndices.size() < 2 ) continue;
      
      
      if( m_jp.FillHist ) m_hists["finalCutMonitor"]->Fill( 2 );
      
      
      {
        WrkVrt backup = wrkvrt;
      
        StatusCode sc = refitVertex( wrkvrt );
        if( sc.isFailure() ) {
          
          auto indices = wrkvrt.associatedTrackIndices;
          
          wrkvrt.associatedTrackIndices.clear();
          sc = refitVertex( wrkvrt );
          if( sc.isFailure() ) {
            ATH_MSG_WARNING(" > " << __FUNCTION__ << ": detected vertex fitting failure!" );
            wrkvrt = backup;
          }
          if( wrkvrt.fitQuality() > backup.fitQuality() ) wrkvrt = backup;
          
          for( auto& index : indices ) {
            backup = wrkvrt;
            wrkvrt.associatedTrackIndices.emplace_back( index );
            sc = refitVertex( wrkvrt );
            if( sc.isFailure() || TMath::Prob( wrkvrt.Chi2, wrkvrt.ndof() ) < m_jp.improveChi2ProbThreshold ) {
              ATH_MSG_WARNING(" > " << __FUNCTION__ << ": detected vertex fitting failure!" );
              wrkvrt = backup;
              continue;
            }
          }
          
        } else {
          if( wrkvrt.fitQuality() > backup.fitQuality() ) wrkvrt = backup;
        }
      }
      
      if( m_jp.FillHist ) m_hists["finalCutMonitor"]->Fill( 3 );
      
      //
      //  Store good vertices into StoreGate 
      //
      if( m_jp.FillNtuple ) m_ntupleVars->get<unsigned int>( "NumSecVrt" )++;
      
      TLorentzVector sumP4_pion;
      TLorentzVector sumP4_electron;
      TLorentzVector sumP4_proton;
      
      // Pre-check before storing vertex if the SV perigee is available
      bool good_flag = true;
      
      std::map<const std::deque<long int>*, const std::vector<const xAOD::TrackParticle*>&> indicesSet
        = {
            { &(wrkvrt.selectedTrackIndices),   *m_selectedTracks   },
            { &(wrkvrt.associatedTrackIndices), *m_associatedTracks }
          };
      
      for( auto& pair : indicesSet ) {
        
        auto* indices = pair.first;
        auto& tracks  = pair.second;
        
        for( auto& itrk : *indices ) {
          const auto* trk = tracks.at( itrk );
          const auto* sv_perigee = m_trackToVertexTool->perigeeAtVertex( *trk, wrkvrt.vertex );
          if( !sv_perigee ) {
            ATH_MSG_INFO(" > " << __FUNCTION__ << ": > Track index " << trk->index() << ": Failed in obtaining the SV perigee!" );
            good_flag = false;
          }
          delete sv_perigee;
        }
      
      }
      
      if( !good_flag ) {
        ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": sv perigee could not be obtained --> rejected" );
        continue;
      }
      
      if( m_jp.FillHist ) m_hists["finalCutMonitor"]->Fill( 4 );
      
      
      std::vector<const xAOD::TrackParticle*> tracks;
      std::vector< std::pair<const xAOD::TrackParticle*, double> > trackChi2Pairs;
      
      {
        
        for( auto& pair : indicesSet ) {
          for( const auto& index : *pair.first ) tracks.emplace_back( pair.second.at( index ) );
        }
        
        auto trkitr = tracks.begin();
        auto chi2itr = wrkvrt.Chi2PerTrk.begin();
        
        for( ; ( trkitr!=tracks.end() && chi2itr!=wrkvrt.Chi2PerTrk.end() ); ++trkitr, ++chi2itr ) {
          trackChi2Pairs.emplace_back( std::make_pair(*trkitr, *chi2itr) );
        }
        
      }
      
      
      TLorentzVector sumP4_selected;
      
      bool badIPflag { false };
      
      // loop over vertex tracks
      ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": Track loop: size = " << tracks.size() );
      for( auto& pair : trackChi2Pairs ) {
        
        const auto* trk      = pair.first;
        const auto& chi2AtSV = pair.second;

        ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": > Track index " << trk->index() << ": start." );

        track_summary trk_summary;
        fillTrackSummary( trk_summary, trk );

        //
        // calculate mass/pT of tracks and track parameters
        //

        double trk_pt  = trk->pt();
        double trk_eta = trk->eta();
        double trk_phi = trk->phi();

        ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": > Track index " << trk->index() << ": in vrt chg/pt/phi/eta = "
            << trk->charge() <<","
            <<trk_pt<<","
            <<trk_phi<<","
            <<trk_eta);

        /////////////////////////////////////////////
        // Get the perigee of the track at the vertex
        ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": > Track index " << trk->index() << ": Get the prigee of the track at the vertex." );

        const Trk::Perigee* sv_perigee = m_trackToVertexTool->perigeeAtVertex( *trk, wrkvrt.vertex );
        if( !sv_perigee ) {
          ATH_MSG_WARNING(" > " << __FUNCTION__ << ": > Track index " << trk->index() << ": Failed in obtaining the SV perigee!" );
          
          for( auto& pair : m_trkDecors ) {
            pair.second( *trk ) = AlgConsts::invalidFloat;
          }
          (*m_decor_is_svtrk_final)( *trk ) = true;
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
        double errd0_wrtSV     = (*sv_perigee->covariance())( Trk::d0, Trk::d0 );
        double errz0_wrtSV     = (*sv_perigee->covariance())( Trk::z0, Trk::z0 );
        double errP_wrtSV      = (*sv_perigee->covariance())( Trk::qOverP, Trk::qOverP );
        
        // xAOD::Track augmentation
        ( m_trkDecors.at(kPt)    )( *trk ) = pt_wrtSV;
        ( m_trkDecors.at(kEta)   )( *trk ) = eta_wrtSV;
        ( m_trkDecors.at(kPhi)   )( *trk ) = phi_wrtSV;
        ( m_trkDecors.at(kD0)    )( *trk ) = d0_wrtSV;
        ( m_trkDecors.at(kZ0)    )( *trk ) = z0_wrtSV;
        ( m_trkDecors.at(kErrP)  )( *trk ) = errP_wrtSV;
        ( m_trkDecors.at(kErrD0) )( *trk ) = errd0_wrtSV;
        ( m_trkDecors.at(kErrZ0) )( *trk ) = errz0_wrtSV;
        ( m_trkDecors.at(kChi2SV))( *trk ) = chi2AtSV;
        
        (*m_decor_is_svtrk_final)( *trk ) = true;
        
        TLorentzVector p4wrtSV_pion;
        TLorentzVector p4wrtSV_electron;
        TLorentzVector p4wrtSV_proton;
        
        p4wrtSV_pion    .SetPtEtaPhiM( pt_wrtSV, eta_wrtSV, phi_wrtSV, PhysConsts::mass_chargedPion );
        p4wrtSV_electron.SetPtEtaPhiM( pt_wrtSV, eta_wrtSV, phi_wrtSV, PhysConsts::mass_electron    );
        
        // for selected tracks only
        if( trk->isAvailable<char>("is_associated" + m_jp.augVerString ) ) {
          if( !trk->auxdataConst<char>("is_associated" + m_jp.augVerString) ) {
            sumP4_selected += p4wrtSV_pion;
          }
        } else {
          sumP4_selected += p4wrtSV_pion;
        }
        
        sumP4_pion     += p4wrtSV_pion;
        sumP4_electron += p4wrtSV_electron;
        sumP4_proton   += p4wrtSV_proton;
        
        delete sv_perigee;

        ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": > Track index " << trk->index() << ": end." );
      } // loop over tracks in vertex

      ATH_MSG_VERBOSE(" > " << __FUNCTION__ << ": Track loop end. ");

      ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": Final Sec.Vertex=" << wrkvrt.nTracksTotal() <<", "
                    <<wrkvrt.vertex.perp() <<", "<<wrkvrt.vertex.z() <<", "
                    <<wrkvrt.vertex.phi() <<", mass = "<< sumP4_pion.M() << "," << sumP4_electron.M() );


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
      
      if( badIPflag ) {
        ATH_MSG_DEBUG(" > " << __FUNCTION__ << ": Bad impact parameter signif wrt SV was flagged." );
      }

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
      // Here, we register the core chi2 of the core (before track association)
      vertex->setFitQuality( wrkvrt.Chi2_core, wrkvrt.ndof_core() );

      // Registering the vertex covariance matrix
      std::vector<float> fCov(wrkvrt.vertexCov.cbegin(), wrkvrt.vertexCov.cend());
      vertex->setCovariance(fCov);

      // Registering the vertex momentum and charge
      vertex->auxdata<float>("vtx_px")                   = wrkvrt.vertexMom.Px();
      vertex->auxdata<float>("vtx_py")                   = wrkvrt.vertexMom.Py();
      vertex->auxdata<float>("vtx_pz")                   = wrkvrt.vertexMom.Pz();

      vertex->auxdata<float>("vtx_mass")                 = wrkvrt.vertexMom.M();
      vertex->auxdata<float>("vtx_charge")               = wrkvrt.Charge;

      vertex->auxdata<float>("chi2_core")                = wrkvrt.Chi2_core;
      vertex->auxdata<float>("ndof_core")                = wrkvrt.ndof_core();
      vertex->auxdata<float>("chi2_assoc")               = wrkvrt.Chi2;
      vertex->auxdata<float>("ndof_assoc")               = wrkvrt.ndof();
      // Other SV properties
      vertex->auxdata<float> ("mass")                    = sumP4_pion.M();
      vertex->auxdata<float> ("mass_e")                  = sumP4_electron.M();
      vertex->auxdata<float> ("mass_selectedTracks")     = sumP4_selected.M();
      vertex->auxdata<float> ("minOpAng")                = minOpAng;
      vertex->auxdata<int>   ("num_trks")                = wrkvrt.nTracksTotal();
      vertex->auxdata<int>   ("num_selectedTracks")      = wrkvrt.selectedTrackIndices.size();
      vertex->auxdata<int>   ("num_associatedTracks")    = wrkvrt.associatedTrackIndices.size();
      vertex->auxdata<float> ("dCloseVrt")               = wrkvrt.closestWrkVrtValue;
      
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
    if( m_jp.doAugmentDVimpactParametersToMuons     ) { ATH_CHECK( augmentDVimpactParametersToLeptons<xAOD::Muon>    ( "Muons"     ) ); }
    if( m_jp.doAugmentDVimpactParametersToElectrons ) { ATH_CHECK( augmentDVimpactParametersToLeptons<xAOD::Electron>( "Electrons" ) ); }
    
    } catch (const std::out_of_range& e) {
      
      ATH_MSG_WARNING( " > " << __FUNCTION__ << ": out of range error is detected: " << e.what()  );
      
      return StatusCode::SUCCESS;
      
    } catch( ... ) {

      ATH_MSG_WARNING( " > " << __FUNCTION__ << ": some other error is detected."  );
      
      return StatusCode::SUCCESS;
      
    }
      
    return StatusCode::SUCCESS;
  }
  
  
  //____________________________________________________________________________________________________
  StatusCode VrtSecInclusive::monitorVertexingAlgorithmStep( std::vector<WrkVrt>* workVerticesContainer, const std::string name, bool final ) {
    
    if( m_jp.FillIntermediateVertices ) {
      
      const xAOD::TrackParticleContainer* trackParticleContainer ( nullptr );
      ATH_CHECK( evtStore()->retrieve( trackParticleContainer, m_jp.TrackLocation) );
      
      xAOD::VertexContainer* intermediateVertexContainer { nullptr };
      
      ATH_CHECK( evtStore()->retrieve( intermediateVertexContainer, "VrtSecInclusive_IntermediateVertices_" + name + m_jp.augVerString ) );
      
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
    
    if( m_vertexingAlgorithmStep == 0 ) {
      
      const auto compSize = m_selectedTracks->size()*(m_selectedTracks->size() - 1)/2 - m_incomp.size();
      m_hists["vertexYield"]->Fill( m_vertexingAlgorithmStep, compSize );
      
    } else {
      
      m_hists["vertexYield"]->Fill( m_vertexingAlgorithmStep, count );
      
    }
    
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
  
  //____________________________________________________________________________________________________
  bool VrtSecInclusive::getSVImpactParameters(const xAOD::TrackParticle* trk, Amg::Vector3D vertex,
                                              std::vector<double>& impactParameters,
                                              std::vector<double>& impactParErrors){

    if( m_jp.trkExtrapolator==1 ){
      m_fitSvc->VKalGetImpact(trk, vertex, static_cast<int>( trk->charge() ), impactParameters, impactParErrors);
    }
    else if( m_jp.trkExtrapolator==2 ){
      const Trk::Perigee* sv_perigee = m_trackToVertexTool->perigeeAtVertex( *trk, vertex );
      if( !sv_perigee ) return false;
      impactParameters[0] = sv_perigee->parameters() [Trk::d0];
      impactParameters[1] = sv_perigee->parameters() [Trk::z0];
      impactParErrors[0]  = (*sv_perigee->covariance())( Trk::d0, Trk::d0 );
      impactParErrors[1]  = (*sv_perigee->covariance())( Trk::z0, Trk::z0 );
    }
    else{
      ATH_MSG_WARNING( " > " << __FUNCTION__ << ": Unknown track extrapolator " << m_jp.trkExtrapolator   );
      return false;
    }

    return true;

  } // getSVImpactParameters

} // end of namespace VKalVrtAthena
