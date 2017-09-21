/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

using namespace std;

namespace Trk {
  extern 
  int  pgraphm_(
		long int *weit, long int *edges, long int *nodes,
		long int *set, long int *nptr,  long int *nth);
}

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
    
    // Services
    m_pixelCondSummarySvc          ( "PixelConditionsSummarySvc", "VrtSecInclusive" ),
    m_sctCondSummarySvc            ( "SCT_ConditionsSummarySvc", "VrtSecInclusive" ),
    
    m_checkPatternStrategy         ( "Classical" ),
    
    // Pointers of Ntuple variable vectors
    m_tree_Vert                    ( nullptr ),
    m_ntupleVars                   ( nullptr )
    
  {
    
    m_patternStrategyFuncs["Classical"]     = &VrtSecInclusive::checkTrackHitPatternToVertex;
    m_patternStrategyFuncs["Extrapolation"] = &VrtSecInclusive::checkTrackHitPatternToVertexByExtrapolation;

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
  StatusCode VrtSecInclusive::beginRun()  { 

    ATH_MSG_DEBUG("beginRun()");

    return StatusCode::SUCCESS;
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
    
    
    if( detStore()->retrieve(m_atlasId, "AtlasID").isFailure() ) return StatusCode::SUCCESS;
    if( detStore()->retrieve(m_pixelId, "PixelID").isFailure() ) return StatusCode::SUCCESS;
    if( detStore()->retrieve(m_sctId,   "SCT_ID") .isFailure() ) return StatusCode::SUCCESS;
    
    ATH_CHECK( m_extrapolator.retrieve() );
    
    // extract VertexMapper
    if( m_jp.doMapToLocal ) {
      ATH_CHECK( m_vertexMapper.retrieve() );
    }
    
    
    // Vertexing algorithm configuration
    m_vertexingAlgorithms.emplace_back( &VrtSecInclusive::reconstruct2TrackVertices         );
    m_vertexingAlgorithms.emplace_back( &VrtSecInclusive::reconstructNTrackVertices         );
    
    if( m_jp.doReassembleVertices ) {
      m_vertexingAlgorithms.emplace_back( &VrtSecInclusive::reassembleVertices              );
    }
    
    if( m_jp.doMergeByShuffling ) {
      m_vertexingAlgorithms.emplace_back( &VrtSecInclusive::mergeByShuffling                );
    }
    
    if ( m_jp.doMergeFinalVerticesDistance ) {
      m_vertexingAlgorithms.emplace_back( &VrtSecInclusive::mergeFinalVertices              );
    }
    
    if( m_jp.doAssociateNonSelectedTracks ) {
      m_vertexingAlgorithms.emplace_back( &VrtSecInclusive::associateNonSelectedTracks      );
    }
    
    m_vertexingAlgorithms.emplace_back( &VrtSecInclusive::refitAndSelectGoodQualityVertices );

    
    // now make histograms/ntuples
    
    ATH_CHECK( service( "THistSvc", hist_root ) );
    
    if( m_jp.FillHist ) {
      
      ATH_MSG_INFO("initialize: Filling Histograms");
      //
      m_hists["trkSelCuts"]       = new TH1F("trkSelCuts",";Cut Order;Trackss",10, -0.5, 10-0.5);
      m_hists["vertexYield"]      = new TH1F("vertexYield",";Algorithm Step;Events",10, -0.5, 10-0.5);
      m_hists["vertexYieldNtrk"]  = new TH2F("vertexYieldNtrk",";Ntrk;Algorithm Step;Events", 100, 0, 100, 10, -0.5, 10-0.5);
      m_hists["vertexYieldChi2"]  = new TH2F("vertexYieldChi2",";#chi^{2}/N_{dof};Algorithm Step;Events", 100, 0, 100, 10, -0.5, 10-0.5);
      m_hists["shuffleMinSignif"] = new TH1F("shuffleMinSignif", ";Min( log_{10}( Significance ) );Vertices", 100, -3, 5);
      m_hists["finalVtxNtrk"]     = new TH1F("finalVtxNtrk", ";N_{trk};Vertices", 100, 0, 100);
      m_hists["finalVtxR"]        = new TH1F("finalVtxR", ";r [mm];Vertices", 400, 0, 400);
      
      std::string histDir("/AANT/VrtSecInclusive/");
      
      for( auto& pair : m_hists ) {
        ATH_CHECK( hist_root->regHist( histDir + pair.first, pair.second ) );
      }
    }
    
    
    if( m_jp.FillNtuple ) {
      
      ATH_CHECK( setupNtupleVariables() );

      m_tree_Vert = new TTree("tree_VrtSec","TTree of VrtSecInclusive"); 
      ATH_CHECK( hist_root->regTree("/AANT/tree_VrtSec", m_tree_Vert) );
      
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
    
    
    ///////////////////////////////////////////////////////////////////////////
    //
    // Setup StoreGate Variables
    //
    auto *selectedBaseTracks          = new xAOD::TrackParticleContainer;
    auto *selectedBaseTracksAux       = new xAOD::TrackParticleAuxContainer;
    auto *associableTracks            = new xAOD::TrackParticleContainer;
    auto *associableTracksAux         = new xAOD::TrackParticleAuxContainer;
    auto *twoTrksVertexContainer      = new xAOD::VertexContainer;
    auto *twoTrksVertexAuxContainer   = new xAOD::VertexAuxContainer;
    auto *secondaryVertexContainer    = new xAOD::VertexContainer;
    auto *secondaryVertexAuxContainer = new xAOD::VertexAuxContainer;
    
    selectedBaseTracks       ->setStore( selectedBaseTracksAux );
    associableTracks         ->setStore( associableTracksAux );
    twoTrksVertexContainer   ->setStore( twoTrksVertexAuxContainer );
    secondaryVertexContainer ->setStore( secondaryVertexAuxContainer );
    
    
    ATH_CHECK( evtStore()->record( selectedBaseTracks,          "VrtSecInclusive_SelectedTrackParticles"       ) );
    ATH_CHECK( evtStore()->record( selectedBaseTracksAux,       "VrtSecInclusive_SelectedTrackParticlesAux."   ) );
    
    ATH_CHECK( evtStore()->record( associableTracks,            "VrtSecInclusive_AssociableParticles"          ) );
    ATH_CHECK( evtStore()->record( associableTracksAux,         "VrtSecInclusive_AssociableParticlesAux."      ) );
    
    ATH_CHECK( evtStore()->record( twoTrksVertexContainer,      "VrtSecInclusive_All2TrksVertices"             ) );
    ATH_CHECK( evtStore()->record( twoTrksVertexAuxContainer,   "VrtSecInclusive_All2TrksVerticesAux."         ) );
    
    ATH_CHECK( evtStore()->record( secondaryVertexContainer,    "VrtSecInclusive_SecondaryVertices"            ) );
    ATH_CHECK( evtStore()->record( secondaryVertexAuxContainer, "VrtSecInclusive_SecondaryVerticesAux."        ) );
    
    // Later use elsewhere in the algorithm
    m_selectedBaseTracks = selectedBaseTracks;
    m_associableTracks   = associableTracks;
    
    ///////////////////////////////////////////////////////////////////////////
    //
    // now start algorithm
    //
    
    //--------------------------------------------------------
    //  Extract tracks from xAOD::TrackParticle container
    //
    
    const xAOD::TrackParticleContainer* trackParticleContainer ( nullptr );
    ATH_CHECK( evtStore()->retrieve( trackParticleContainer, m_jp.TrackLocation) );
    
    ATH_MSG_DEBUG( "Extracted xAOD::TrackParticle number=" << trackParticleContainer->size() );
    
    //--------------------------------------------------------
    //  Primary vertex processing
    // 
    sc = this->processPrimaryVertices(); // fetch the 1st primary reconstructed vertex
    
    if( sc.isFailure() or !m_thePV ) {
      
      ATH_MSG_WARNING("processPrimaryVertices() failed");
      return StatusCode::SUCCESS;
    }
    

    if( m_jp.FillNtuple )
      m_ntupleVars->get<unsigned int>( "NumAllTrks" ) = static_cast<int>( trackParticleContainer->size() );
    
    
    //-----------------------------------------------------------
    //  Track selection
    //
    
    ATH_MSG_DEBUG("execute: Reco. Tracks in event = "<< static_cast<int>( trackParticleContainer->size() ) );

    // Perform track selection and store it to selectedBaseTracks
    ATH_CHECK( selectTracks( trackParticleContainer ) );
    
    ATH_MSG_DEBUG( "execute: Number of total tracks      = " << trackParticleContainer->size() );
    ATH_MSG_DEBUG( "execute: Number of selected tracks   = " << selectedBaseTracks->size() );
    
    
    if( m_jp.FillNtuple )
      m_ntupleVars->get<unsigned int>( "NumSelTrks" ) = static_cast<int>( selectedBaseTracks->size() );
    
    // fill information about selected tracks in AANT
    ATH_CHECK( fillAANT_SelectedBaseTracks() );
    
    //-------------------------------------------------------
    // Skip the event if the number of selected tracks is more than m_jp.SelTrkMaxCutoff
    if( selectedBaseTracks->size() < 2 ) {
      ATH_MSG_INFO( "execute: Too few (<2) selected reco tracks. Terminated reconstruction." );
      return StatusCode::SUCCESS;   
    }
      
    if( selectedBaseTracks->size() > m_jp.SelTrkMaxCutoff ) {
      ATH_MSG_INFO( "execute: Too many selected reco tracks. Terminated reconstruction." );
      return StatusCode::SUCCESS;   
    }
      
    //-------------------------------------------------------
    // Core part of Vertexing
    //
    
    // List of track indices which are not used for the 2-track vertices.
    m_incomp.clear();
    m_vertexingAlgorithmStep = 0;
    
    // Try to compose 2-track vertices for all combinations of selected tracks greather than the given chi2 cut.      
    ATH_CHECK( extractIncompatibleTracks() );
    
    ATH_MSG_VERBOSE( "execute: Found= "<<m_incomp.size()<<", "<< selectedBaseTracks->size() );
    if( m_jp.FillNtuple ) m_ntupleVars->get<unsigned int>( "SizeIncomp" ) = m_incomp.size();
    
    // set of vertices created in the following while loop.
    auto* workVerticesContainer = new std::vector<WrkVrt>;
    
    // the main sequence of the main vertexing algorithms
    // see initialize() what kind of algorithms exist.
    for( auto itr = m_vertexingAlgorithms.begin(); itr!=m_vertexingAlgorithms.end(); ++itr ) {
      
      auto alg = *itr;
      
      ATH_CHECK( (this->*alg)( workVerticesContainer ) );
      
      auto end = std::remove_if( workVerticesContainer->begin(), workVerticesContainer->end(), []( WrkVrt& wrkvrt ) { return wrkvrt.isGood == false || wrkvrt.selectedTrackIndices.size() < 2; } );
      workVerticesContainer->erase( end, workVerticesContainer->end() );

      monitorVertexingAlgorithmStep( workVerticesContainer, "", std::next( itr ) == m_vertexingAlgorithms.end() );
      
      m_vertexingAlgorithmStep++;
      
    }
    
    delete workVerticesContainer;
    
    
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
