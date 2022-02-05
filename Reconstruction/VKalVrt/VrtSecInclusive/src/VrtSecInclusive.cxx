/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecInclusive/VrtSecInclusive.h"
#include "VrtSecInclusive/NtupleVars.h"

#include "VxVertex/VxTrackAtVertex.h"
#include "GeneratorObjects/HepMcParticleLink.h"

// ROOT Classes
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TROOT.h"

#include <string>
#include <new>
#include <iostream>
#include <tuple>
#include <functional>
#include <chrono>
#include <exception> 

using namespace std;


namespace VKalVrtAthena {
  
  //Constructor and destructor
  //__________________________________________________________________________
  VrtSecInclusive::VrtSecInclusive(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm                   (name,pSvcLocator),
    
    m_primaryVertices              ( nullptr ),
    m_thePV                        ( nullptr ),
  
    // ToolsHandles
    m_fitSvc                       ( "Trk::TrkVKalVrtFitter", this ),
    m_truthToTrack                 ( "Trk::TruthToTrack/InDetTruthToTrack" ),
    m_trackToVertexTool            ( "Reco::TrackToVertex" ),
    m_trackToVertexIPEstimatorTool ( "Trk::TrackToVertexIPEstimator/TrackToVertexIPEstimator" ),
    m_extrapolator                 ( "Trk::Extrapolator/AtlasExtrapolator" ),
    m_vertexMapper                 ( "" ),
    
    m_checkPatternStrategy         ( "Classical" ),
    
    // Pointers of Ntuple variable vectors
    m_tree_Vert                    ( nullptr ),
    m_ntupleVars                   ( nullptr )
    
  {
    
    m_patternStrategyFuncs["Classical"]           = &VrtSecInclusive::checkTrackHitPatternToVertex;
    m_patternStrategyFuncs["ClassicalOuter"]      = &VrtSecInclusive::checkTrackHitPatternToVertexOuterOnly;
    m_patternStrategyFuncs["Extrapolation"]       = &VrtSecInclusive::checkTrackHitPatternToVertexByExtrapolation;
    m_patternStrategyFuncs["ExtrapolationAssist"] = &VrtSecInclusive::checkTrackHitPatternToVertexByExtrapolationAssist;

    this->declareProperties();
    
    if( m_jp.FillNtuple ) {
      m_ntupleVars.reset( new NtupleVars );
    }
    
  }
  
  

  //__________________________________________________________________________
  VrtSecInclusive::~VrtSecInclusive()
  { 
    ATH_MSG_DEBUG("destructor called");
  }
  
  
  //__________________________________________________________________________
  StatusCode VrtSecInclusive::initialize()
  {
    //---------------- HBOOK
    ITHistSvc*     hist_root=0;
    ATH_MSG_INFO("initialize: begin");
    //
    // first instantiate tools

    //  VKalVrt vertex fitter
    if (m_fitSvc.retrieve().isFailure()) {
      ATH_MSG_ERROR("initialize: Can't find Trk::TrkVKalVrtFitter");
      return StatusCode::SUCCESS; 
    } else {
      ATH_MSG_INFO("initialize: Trk::TrkVKalVrtFitter found");
    }

    //
    // retreive tool to get trackParameters of generated Particles
    if(m_truthToTrack.retrieve().isFailure()) {
      ATH_MSG_INFO("initialize: Cannot retrieve Trk::TruthToTrack Tool!");
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_INFO("initialize: Retrieved Trk::TruthToTrack Tool" << m_truthToTrack);

    }
    // extract TrackToVertex extrapolator tool
    if ( m_trackToVertexTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR("initialize: failed to retrieve trackToVertex tool ");
      return StatusCode::SUCCESS;
    }
    else {
      ATH_MSG_INFO("initialize: Retrieved Reco::TrackToVertex Tool" << m_trackToVertexTool);
    }
    // extract TrackToVertexIPEstimator extrapolator tool
    if ( m_trackToVertexIPEstimatorTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR("initialize: failed to retrieve trackToVertexIPEstimator tool ");
      return StatusCode::SUCCESS;
    }
    else {
      ATH_MSG_INFO("initialize: Retrieved Trk::TrackToVertexIPEstimator Tool" << m_trackToVertexIPEstimatorTool);
    }
    
    if( detStore()->retrieve(m_atlasId, "AtlasID").isFailure() ) return StatusCode::FAILURE;
    if( detStore()->retrieve(m_pixelId, "PixelID").isFailure() ) return StatusCode::FAILURE;
    if( detStore()->retrieve(m_sctId,   "SCT_ID") .isFailure() ) return StatusCode::FAILURE;

    if ( m_pixelCondSummaryTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR("initialize: failed to retrieve PixelConditionsSummaryTool");
      return StatusCode::SUCCESS;
    }
    else {
      ATH_MSG_INFO("initialize: Retrieved PixelConditionsSummaryTool" << m_pixelCondSummaryTool);
    }
    if ( m_sctCondSummaryTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR("initialize: failed to retrieve SCTConditionsSummaryTool");
      return StatusCode::SUCCESS;
    }
    else {
      ATH_MSG_INFO("initialize: Retrieved SCTConditionsSummaryTool" << m_sctCondSummaryTool);
    }
    
    ATH_CHECK( m_extrapolator.retrieve() );
    
    // extract VertexMapper
    if( m_jp.doMapToLocal ) {
      ATH_CHECK( m_vertexMapper.retrieve() );
    }
    
    // Track selection algorithm configuration
    if( m_jp.doSelectTracksFromMuons )     { m_trackSelectionAlgs.emplace_back( &VrtSecInclusive::selectTracksFromMuons );     }
    if( m_jp.doSelectTracksFromElectrons ) { m_trackSelectionAlgs.emplace_back( &VrtSecInclusive::selectTracksFromElectrons ); }
    
    // if none of the above two flags are activated, use ID tracks (default)
    if( !m_jp.doSelectTracksFromMuons && !m_jp.doSelectTracksFromElectrons ) {
      
      m_trackSelectionAlgs.emplace_back( &VrtSecInclusive::selectTracksInDet );
      
    }
    
    
    // Vertexing algorithm configuration
    m_vertexingAlgorithms.emplace_back( std::pair<std::string, vertexingAlg>( "extractIncompatibleTrackPairs", &VrtSecInclusive::extractIncompatibleTrackPairs )     );
    m_vertexingAlgorithms.emplace_back( std::pair<std::string, vertexingAlg>( "findNtrackVertices",            &VrtSecInclusive::findNtrackVertices )                );
    m_vertexingAlgorithms.emplace_back( std::pair<std::string, vertexingAlg>( "rearrangeTracks",               &VrtSecInclusive::rearrangeTracks)                    );
    
    if( m_jp.doReassembleVertices ) {
      m_vertexingAlgorithms.emplace_back( std::pair<std::string, vertexingAlg>( "reassembleVertices",          &VrtSecInclusive::reassembleVertices )                );
    }
      
    if( m_jp.doMergeByShuffling ) {
      m_vertexingAlgorithms.emplace_back( std::pair<std::string, vertexingAlg>( "mergeByShuffling",           &VrtSecInclusive::mergeByShuffling )                   );
    }
      
    if ( m_jp.doMergeFinalVerticesDistance ) {
      m_vertexingAlgorithms.emplace_back( std::pair<std::string, vertexingAlg>( "mergeFinalVertices",          &VrtSecInclusive::mergeFinalVertices )                );
    }
    
    if( m_jp.doAssociateNonSelectedTracks ) {
      m_vertexingAlgorithms.emplace_back( std::pair<std::string, vertexingAlg>( "associateNonSelectedTracks",  &VrtSecInclusive::associateNonSelectedTracks )        );
    }
    
    m_vertexingAlgorithms.emplace_back( std::pair<std::string, vertexingAlg>( "refitAndSelect",                &VrtSecInclusive::refitAndSelectGoodQualityVertices ) );

    
    // now make histograms/ntuples
    
    ATH_CHECK( service( "THistSvc", hist_root ) );
    
    if( m_jp.FillHist ) {
      
      std::vector<double> rbins = { 0.1, 0.3, 0.5, 1, 2, 3, 5, 7, 10, 14, 20, 28, 38, 50, 64, 80, 100, 130, 170, 220, 280, 350, 450, 600 };
      std::vector<double> nbins = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 20, 24, 28, 38, 50, 70, 100, 150 };
      
      const size_t& nAlgs = m_vertexingAlgorithms.size();
      
      ATH_MSG_INFO("initialize: Filling Histograms");
      //
      m_hists["trkSelCuts"]        = new TH1F("trkSelCuts",        ";Cut Order;Tracks",                         10, -0.5, 10-0.5                                         );
      m_hists["selTracksDist"]     = new TH1F("selTracksDist",     ";Selected Tracks;Events",                   2000, -0.5, 2000-0.5                                     );
      m_hists["initVertexDispD0"]  = new TH2F("initVertexDispD0",  ";Rough d0 wrt init [mm];r [mm];Vertices",   1000, -100, 100, rbins.size()-1, &(rbins[0])             ); 
      m_hists["initVertexDispZ0"]  = new TH2F("initVertexDispZ0",  ";Rough z0 wrt init [mm];z [mm];Vertices",   1000, -100, 100, 100, -1000, 1000                        ); 
      m_hists["incompMonitor"]     = new TH1F("incompMonitor",     ";Setp;Track Pairs",                         10, -0.5, 10-0.5                                         );
      m_hists["2trkVerticesDist"]  = new TH1F("2trkVerticesDist",  ";2-track Vertices;Events",                  1000, -0.5, 1000-0.5                                     );
      m_hists["2trkChi2Dist"]      = new TH1F("2trkChi2Dist",      ";log10(#chi^{2}/N_{dof});Entries",          100, -3, 7                                               );
      m_hists["NtrkChi2Dist"]      = new TH1F("NtrkChi2Dist",      ";log10(#chi^{2}/N_{dof});Entries",          100, -3, 7                                               );
      m_hists["vPosDist"]          = new TH2F("vPosDist",          ";r;#vec{x}*#vec{p}/p_{T} [mm]",             rbins.size()-1, &(rbins[0]), 200, -1000, 1000            );
      m_hists["vPosMomAngTDist"]   = new TH2F("vPosMomAngDistT",   ";r;cos(#vec{r},#vec{p}_{T})",               rbins.size()-1, &(rbins[0]), 200, -1.0, 1.0              );
      m_hists["disabledCount"]     = new TH1F("disabledCount",     ";N_{modules};Tracks",                       20, -0.5, 10-0.5                                         );
      m_hists["vertexYield"]       = new TH1F("vertexYield",       ";Algorithm Step;Vertices",                  nAlgs, -0.5, nAlgs-0.5                                   );
      m_hists["vertexYieldNtrk"]   = new TH2F("vertexYieldNtrk",   ";Ntrk;Algorithm Step;Vertices",             100, 0, 100, nAlgs, -0.5, nAlgs-0.5                      );
      m_hists["vertexYieldChi2"]   = new TH2F("vertexYieldChi2",   ";#chi^{2}/N_{dof};Algorithm Step;Vertices", 100, 0, 100, nAlgs, -0.5, nAlgs-0.5                      );
      m_hists["mergeType"]         = new TH1F("mergeType",         ";Merge Algorithm Type;Entries",             10, -0.5, 10-0.5                                         );
      m_hists["associateMonitor"]  = new TH1F("associateMonitor",  ";Step;Vertices",                            10, -0.5, 10-0.5                                         );
      m_hists["shuffleMinSignif1"] = new TH1F("shuffleMinSignif1", ";Min( log_{10}( Significance ) );Vertices", 100, -3, 5                                               );
      m_hists["shuffleMinSignif2"] = new TH1F("shuffleMinSignif2", ";Min( log_{10}( Significance ) );Vertices", 100, -3, 5                                               );
      m_hists["shuffleMinSignif3"] = new TH1F("shuffleMinSignif3", ";Min( log_{10}( Significance ) );Vertices", 100, -3, 5                                               );
      m_hists["finalCutMonitor"]   = new TH1F("finalCutMonitor",   ";Step;Vertices",                            6, -0.5, 6-0.5                                           );
      m_hists["finalVtxNtrk"]      = new TH1F("finalVtxNtrk",      ";N_{trk};Vertices",                         nbins.size()-1, &(nbins[0])                              );
      m_hists["finalVtxR"]         = new TH1F("finalVtxR",         ";r [mm];Vertices",                          600, 0, 600                                              );
      m_hists["finalVtxNtrkR"]     = new TH2F("finalVtxNtrkR",     ";N_{trk};r [mm];Vertices",                  nbins.size()-1, &(nbins[0]), rbins.size()-1, &(rbins[0]) );
      m_hists["CPUTime"]           = new TH1F("CPUTime",           ";Step;Accum. CPU Time [s]",                 10, -0.5, 10-0.5                                         );
      m_hists["nMatchedTruths"]    = new TH2F("nMatchedTruths",    ";Step;;r [mm];Matched truth vertices",      11, -0.5, 11-0.5, rbins.size()-1, &(rbins[0])            );
      m_hists["vPosMomAngT"]       = new TH1F("vPosMomAngT",       ";cos(#vec{r},#vec{p}_{T}",                  200, -1.0, 1.0                                           );
      m_hists["vPosMomAng3D"]      = new TH1F("vPosMomAng3D",      ";cos(#vec{r},#vec{p})",                     200, -1.0, 1.0                                           );
      m_hists["2trkVtxDistFromPV"] = new TH1F("2trkVtDistFromPV",  ";2tr vertex distance from PV;Events",       100, 0, 3                                                );

      
      std::string histDir("/AANT/VrtSecInclusive" + m_jp.augVerString + "/");
      
      for( auto& pair : m_hists ) {
        ATH_CHECK( hist_root->regHist( histDir + pair.first, pair.second ) );
      }
    }
    
    
    if( m_jp.FillNtuple ) {
      
      ATH_CHECK( setupNtupleVariables() );

      m_tree_Vert = new TTree("tree_VrtSecInclusive","TTree of VrtSecInclusive"); 
      ATH_CHECK( hist_root->regTree("/AANT/tree_VrtSecInclusive", m_tree_Vert) );
      
      ATH_CHECK( setupNtuple() );
      
    }
    // 
    ATH_MSG_INFO("initialize: Exit VrtSecInclusive::initialize()");
    return StatusCode::SUCCESS; 
  }



  //__________________________________________________________________________
  StatusCode VrtSecInclusive::finalize()
  {
    
    ATH_MSG_INFO("finalize: VrtSecInclusive finalize()");
    return StatusCode::SUCCESS; 
  }

  //__________________________________________________________________________
  StatusCode VrtSecInclusive::initEvent()
  {

    ATH_MSG_DEBUG("initEvent: begin");
    
    // Clear all variables to be stored to the AANT
    if( m_jp.FillNtuple ) {
      ATH_CHECK( clearNtupleVariables() );
    }

    
    ATH_MSG_DEBUG("initEvent: from initEvent ");
    return StatusCode::SUCCESS;
   
  }
  
  
  //__________________________________________________________________________
  StatusCode VrtSecInclusive::execute()
  {
    //
    ATH_MSG_DEBUG("VrtSecInclusive execute()");

    // clear ntuple variables
    StatusCode sc = this->initEvent();
    if(sc.isFailure()) {
      ATH_MSG_WARNING("Problem in initEvent ");
      return StatusCode::SUCCESS;
    }
    
    // add event level info to ntuple
    if( m_jp.FillNtuple ) sc = addEventInfo();

    if (sc.isFailure() ) {
      ATH_MSG_WARNING("Failure in getEventInfo() ");
      return StatusCode::SUCCESS;
    }

    m_vertexingStatus = 0;
    
    ///////////////////////////////////////////////////////////////////////////
    //
    // Setup StoreGate Variables
    //
    
    // Check Return StatusCode::Failure if the user-specified container names have duplication.
    {
      std::vector<std::string> userContainerNames { m_jp.secondaryVerticesContainerName, m_jp.all2trksVerticesContainerName };
      std::set<std::string> userContainerNamesSet;
      for( auto& name : userContainerNames ) userContainerNamesSet.insert( name );
      if( userContainerNamesSet.size() != userContainerNames.size() ) {
        ATH_MSG_ERROR( " > " << __FUNCTION__ << ": detected duplicated user-specified container name. Please check your job property" );
        return StatusCode::FAILURE;
      }
    }
    
    auto *secondaryVertexContainer    = new xAOD::VertexContainer;
    auto *secondaryVertexAuxContainer = new xAOD::VertexAuxContainer;
    
    secondaryVertexContainer ->setStore( secondaryVertexAuxContainer );
    
    ATH_CHECK( evtStore()->record( secondaryVertexContainer,    "VrtSecInclusive_" + m_jp.secondaryVerticesContainerName + m_jp.augVerString          ) );
    ATH_CHECK( evtStore()->record( secondaryVertexAuxContainer, "VrtSecInclusive_" + m_jp.secondaryVerticesContainerName + m_jp.augVerString + "Aux." ) );
    
    if( m_jp.FillIntermediateVertices ) {
      auto *twoTrksVertexContainer      = new xAOD::VertexContainer;
      auto *twoTrksVertexAuxContainer   = new xAOD::VertexAuxContainer;
      
      twoTrksVertexContainer   ->setStore( twoTrksVertexAuxContainer );
      
      ATH_CHECK( evtStore()->record( twoTrksVertexContainer,      "VrtSecInclusive_" + m_jp.all2trksVerticesContainerName + m_jp.augVerString          ) );
      ATH_CHECK( evtStore()->record( twoTrksVertexAuxContainer,   "VrtSecInclusive_" + m_jp.all2trksVerticesContainerName + m_jp.augVerString + "Aux."  ) );
    
      for( auto itr = m_vertexingAlgorithms.begin(); itr!=m_vertexingAlgorithms.end(); ++itr ) {
      
        auto& name = itr->first;
      
        auto *intermediateVertexContainer      = new xAOD::VertexContainer;
        auto *intermediateVertexAuxContainer   = new xAOD::VertexAuxContainer;
      
        intermediateVertexContainer   ->setStore( intermediateVertexAuxContainer );
      
        ATH_CHECK( evtStore()->record( intermediateVertexContainer,      "VrtSecInclusive_IntermediateVertices_" + name + m_jp.augVerString           ) );
        ATH_CHECK( evtStore()->record( intermediateVertexAuxContainer,   "VrtSecInclusive_IntermediateVertices_" + name + m_jp.augVerString + "Aux."  ) );
      }
    
    }
    
    dumpTruthInformation();

    
    // Later use elsewhere in the algorithm
    m_selectedTracks.reset  ( new std::vector<const xAOD::TrackParticle*> );
    m_associatedTracks.reset( new std::vector<const xAOD::TrackParticle*> );
    
    m_extrapolatedPatternBank.clear();
    
    ///////////////////////////////////////////////////////////////////////////
    //
    // now start algorithm
    //
    
    //--------------------------------------------------------
    //  Primary vertex processing
    // 
    sc = this->processPrimaryVertices(); // fetch the 1st primary reconstructed vertex
    
    if( sc.isFailure() or !m_thePV ) {
      
      ATH_MSG_WARNING("processPrimaryVertices() failed");
      return StatusCode::SUCCESS;
    }    

    // Perform track selection and store it to selectedBaseTracks
    try {

      for( auto alg : m_trackSelectionAlgs ) {
        ATH_CHECK( (this->*alg)() );
      }
      
    } catch( ... ) {
      
      ATH_MSG_WARNING( " > " << __FUNCTION__ << ": some other error is detected in the track selection scope."  );
      
      return StatusCode::SUCCESS;
      
    }
    
    
    if( m_jp.FillNtuple )
      m_ntupleVars->get<unsigned int>( "NumSelTrks" ) = static_cast<int>( m_selectedTracks->size() );
    
    // fill information about selected tracks in AANT
    ATH_CHECK( fillAANT_SelectedBaseTracks() );
    
    //-------------------------------------------------------
    // Skip the event if the number of selected tracks is more than m_jp.SelTrkMaxCutoff
    if( m_selectedTracks->size() < 2 ) {
      ATH_MSG_DEBUG( "execute: Too few (<2) selected reco tracks. Terminated reconstruction." );
      m_vertexingStatus = 1;
      return StatusCode::SUCCESS;   
    }
      
    if( m_selectedTracks->size() > m_jp.SelTrkMaxCutoff ) {
      ATH_MSG_INFO( "execute: Too many selected reco tracks. Terminated reconstruction." );
      m_vertexingStatus = 2;
      return StatusCode::SUCCESS;   
    }
      
    //-------------------------------------------------------
    // Core part of Vertexing
    //
    
    try {

      m_vertexingAlgorithmStep = 0;
    
      // set of vertices created in the following while loop.
      auto* workVerticesContainer = new std::vector<WrkVrt>;
    
      // the main sequence of the main vertexing algorithms
      // see initialize() what kind of algorithms exist.
      for( auto itr = m_vertexingAlgorithms.begin(); itr!=m_vertexingAlgorithms.end(); ++itr ) {
      
        auto& name = itr->first;
        auto alg   = itr->second;
      
        auto t_start = std::chrono::system_clock::now();
      
        ATH_CHECK( (this->*alg)( workVerticesContainer ) );
      
        auto t_end = std::chrono::system_clock::now();
      
        if( m_jp.FillHist ) {
          auto sec = std::chrono::duration_cast<std::chrono::microseconds>( t_end - t_start ).count();
          m_hists["CPUTime"]->Fill( m_vertexingAlgorithmStep, sec/1.e6 );
        }
      
        auto end = std::remove_if( workVerticesContainer->begin(), workVerticesContainer->end(),
                                   []( WrkVrt& wrkvrt ) {
                                     return ( wrkvrt.isGood == false || wrkvrt.nTracksTotal() < 2 ); }
                                   );
      
        workVerticesContainer->erase( end, workVerticesContainer->end() );

        ATH_CHECK( monitorVertexingAlgorithmStep( workVerticesContainer, name, std::next( itr ) == m_vertexingAlgorithms.end() ) );
      
        m_vertexingAlgorithmStep++;
      
      }
    
      delete workVerticesContainer;
    
    } catch(std::exception& e) {
      
      ATH_MSG_WARNING( " > " << __FUNCTION__ << ": exception detected in the vertexing scope: " << e.what() );
      m_vertexingStatus = 4;
            
      return StatusCode::SUCCESS;
      
    }
    
    const xAOD::EventInfo* eventInfo { nullptr };
    sc = evtStore()->retrieve(eventInfo, "EventInfo");
    if( sc.isFailure() ) { 
      ATH_MSG_WARNING("Failure to retrieve EventInfo ");
      return StatusCode::SUCCESS;
    }
    eventInfo->auxdecor<int>("VrtSecInclusive_"+ m_jp.secondaryVerticesContainerName + m_jp.augVerString + "_status")=m_vertexingStatus;
    
    // Fill AANT
    if( m_jp.FillNtuple ) {
      m_tree_Vert->Fill();
      ATH_CHECK( clearNtupleVariables() );
    }
    
    
    ATH_MSG_VERBOSE( "execute: process done." );
    // end
    return StatusCode::SUCCESS;   
    
  }
    
} // end of namespace bracket
