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
#include "TH1D.h"
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
    AthAlgorithm                  (name,pSvcLocator),
    
    // Physics/Math Constants
    m_pi      ( 139.57018 ),    // Pion mass [MeV]
    m_e       ( 0.511     ),    // Electron mass [MeV]
    m_proton  ( 938.27205 ),    // Proton mass [MeV]
    m_PI      ( 4.*atan(1.) ),  // pi
    
    // Conversion units
    m_cnv_xyz ( 0.1 ),     /* conversion units for distance mm*/ 
    m_cnv_mom ( 1000. ),   /* conversion units for energy MeV*/   
    
    // Detector radius constants
    // Hide: these constants requires to be adapted depending on geometry. Now for Run2
    m_avRad_bp   ( 23.9 ),
    m_avRad_pix0 ( 33.0 ),
    m_avRad_pix1 ( 52.5 ),
    m_avRad_pix2 ( 90.5 ),
    m_avRad_pix3 ( 125  ), // use just MC, since I don't have enough in data
    m_avRad_sct0 ( 302  ),
    m_avRad_sct1 ( 373  ),
  
    m_ec_pix_rmin (  88.8 ),
    m_ec_pix_rmax ( 149.6 ),
    m_ec_pix0     ( 495   ),
    m_ec_pix1     ( 580   ),
    m_ec_pix2     ( 650   ),

    m_ec_sct_rmin  ( 275    ),
    m_ec_sct_rmax  ( 560    ),
    m_ec_sct0_rmin ( 337.6  ),
    m_ec_sct0      ( 853.8  ),
    m_ec_sct1      ( 934    ),
    m_ec_sct2      ( 1091.5 ),
    m_ec_sct3      ( 1299.9 ),
    m_ec_sct4      ( 1399.8 ),
  
    m_vertexTES ( nullptr ),
  
    // String constants
    m_TrackLocation                ( "InDetTrackParticles" ),
    m_PrimVrtLocation              ( "VKalPrimVrtCollection" ),
    m_SecVrtLocation               ( "VKalSecVrtCollection" ),
    m_truthParticleContainerName   ( "TruthParticles" ),
    m_mcEventContainerName         ( "TruthEvent" ),
    
    // GeoModel
    m_geoModel                     ( VKalVrtAthena::GeoModel::Run2 ),
    
    // Analysis cut constants
    m_ImpactWrtBL                  ( false ),
    m_d0TrkPVDstMinCut             ( 0.   ),
    m_d0TrkPVDstMaxCut             ( 1000.   ),
    m_zTrkPVDstMinCut              ( 0.   ),
    m_zTrkPVDstMaxCut              ( 1000.   ),
    m_d0TrkPVSignifCut             ( 0.   ),
    m_zTrkPVSignifCut              ( 0.   ),
    
    m_TrkChi2Cut                   ( 3.   ),
    m_SelVrtChi2Cut                ( 4.5   ),
    m_TrkPtCut                     ( 500.   ),
    m_CutSctHits                   ( 0   ),
    m_CutPixelHits                 ( 0   ),
    m_CutSiHits                    ( 0   ),
    m_CutBLayHits                  ( 0   ),
    m_CutSharedHits                ( 0   ),
    m_CutTRTHits                   ( 0   ),
    m_VertexMergeFinalDistCut      ( 1.  ),
    m_D0TrkErrorCut                ( 10.   ),
    m_ZTrkErrorCut                 ( 20.   ),
    m_VertexMergeCut               ( 3.   ),
    m_TrackDetachCut               ( 6.   ),
    m_FillHist                     ( false ),
    m_FillNtuple                   ( false ),
    m_LoPtCut                      ( 500.   ),
    m_SelTrkMaxCutoff              ( 50  ),
    
    // Internal statuses of the algorithm
    m_SAloneTRT                    ( false ),
    m_doTRTPixCut                  ( false ),
    m_mergeFinalVerticesDistance   ( false ),
    m_doTruth                      ( false ),
    m_removeFakeVrt                ( false ),
    m_mcTrkResolution              ( 0.    ),
    m_TruthTrkLen                  ( 0.    ),
    m_extrapPV                     ( false ),
    m_thePV                        ( nullptr ),
    
    // Tools
    m_fitSvc                       ( "Trk::TrkVKalVrtFitter", this ),
    m_truthToTrack                 ( "Trk::TruthToTrack/InDetTruthToTrack" ),
    m_trackToVertexTool            ( "Reco::TrackToVertex" ),
    m_trackToVertexIPEstimatorTool ( "Trk::TrackToVertexIPEstimator/TrackToVertexIPEstimator" ),
    m_vertexMapper                 ( "" ),
    
    // Histograms for stats
    // (Hide: from C++11 "nullptr" is recommended for NULL)
    m_hb_massPiPi        ( nullptr ),
    m_hb_2Ddist          ( nullptr ),
    m_hb_massEE          ( nullptr ),
    m_hb_nvrt2           ( nullptr ),
    m_hb_ratio           ( nullptr ),
    m_trkSelCuts         ( nullptr ),
    
    // Pointers of Ntuple variable vectors
    m_tree_Vert ( nullptr ),
    m_ntupleVars( new NtupleVars() ),
    
    m_importedTrkTruthColl     ( nullptr ),
    m_importedFullTruthColl    ( nullptr )
    
  {

    this->declareProperties();
    
  }
  
  

  //__________________________________________________________________________
  VrtSecInclusive::~VrtSecInclusive()
  { 
    delete m_ntupleVars;
    
    ATH_MSG_DEBUG("destructor called");
  }



  //__________________________________________________________________________
  StatusCode VrtSecInclusive::beginRun()  { 

    ATH_MSG_INFO("beginRun()");
    //
    ATH_MSG_VERBOSE("Charged electron/pion mass " << m_e<<","<<m_pi);
    //
    // average radii of various material layers - use both MC and data
    //

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
    
    // extract VertexMapper
    if( m_doMapToLocal ) {
      ATH_CHECK( m_vertexMapper.retrieve() );
    }
    
    //

    
    // now make histograms/ntuples
    
    ATH_CHECK( service( "THistSvc", hist_root ) );
    
    if( m_FillHist ) {
      
      ATH_MSG_INFO("initialize: VrtSecInclusiv Histogram found");
      //
      m_hb_massPiPi   = new TH1D("91"," massPiPi", 200, 200., 800.);
      m_hb_2Ddist     = new TH1D("92"," 2Ddist",   200,   0., 500.);
      m_hb_massEE     = new TH1D("93"," massEE",   100,   0., 200.);
      m_hb_nvrt2      = new TH1D("94"," vertices2", 50,   0.,  50.);
      m_hb_ratio      = new TH1D("95"," ratio",     50,   0.,   1.);
      m_trkSelCuts    = new TH1D("TrkSelCuts"," TrkSelCuts ",35, 0., 35.);
      
      std::string histDir("/AANT/stat/SecVrtInclusive/");
      ATH_CHECK( hist_root->regHist(histDir+"91", m_hb_massPiPi) );
      ATH_CHECK( hist_root->regHist(histDir+"92", m_hb_2Ddist) );
      ATH_CHECK( hist_root->regHist(histDir+"93", m_hb_massEE ) );
      ATH_CHECK( hist_root->regHist(histDir+"94", m_hb_nvrt2) );
      ATH_CHECK( hist_root->regHist(histDir+"95", m_hb_ratio) );
      ATH_CHECK( hist_root->regHist(histDir+"TrkSelCuts", m_trkSelCuts) );
    }
    
    
    if( m_FillNtuple ) {
      
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
    if( m_FillNtuple ) {
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
    if( m_FillNtuple ) sc = addEventInfo();
    
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
    
    
    ///////////////////////////////////////////////////////////////////////////
    //
    // now start algorithm
    //
    
    //--------------------------------------------------------
    //  Extract tracks from xAOD::TrackParticle container
    //
    
    const xAOD::TrackParticleContainer* trackParticleContainer ( nullptr );
    ATH_CHECK( evtStore()->retrieve( trackParticleContainer, m_TrackLocation) );
    
    // Truth matching. need this later for getting TrackParticle Truth
    if( m_doTruth ) setTrackParticleContainer( trackParticleContainer );
    
    ATH_MSG_DEBUG( "Extracted xAOD::TrackParticle number=" << trackParticleContainer->size() );
    
    //--------------------------------------------------------
    //  Primary vertex processing
    // 
    sc = this->processPrimaryVertices(); // fetch the 1st primary reconstructed vertex
    
    if( sc.isFailure() or !m_thePV ) {
      
      ATH_MSG_WARNING("processPrimaryVertices() failed");
      return StatusCode::SUCCESS;
    }
    

    if( m_FillNtuple )
      m_ntupleVars->get<unsigned int>( "NumAllTrks" ) = static_cast<int>( trackParticleContainer->size() );
    
    //--------------------------------------------------------
    //
    // set up truth information
    //
    if( m_doTruth ) {

      // we need the following to look for Truth tracks matching RecoParticle
      const xAOD::TruthEventContainer* gen( nullptr );
      ATH_CHECK( evtStore()->retrieve(gen, m_mcEventContainerName) );
      ATH_MSG_VERBOSE("execute: MC Event " << m_mcEventContainerName << " found with " << gen->size() << " entries."); 
      setMcEventCollection( gen ); // for later use
      
      // TrackTruthParticle Container
      const xAOD::TruthParticleContainer* trackTruthColl( nullptr );
      
      ATH_CHECK( evtStore()->retrieve(trackTruthColl, m_truthParticleContainerName) );
      
      if ( !trackTruthColl ) {
	ATH_MSG_ERROR("execute: No " << m_truthParticleContainerName << " found in StoreGate");
	trackTruthColl=nullptr; 
      }
      
      setTrackParticleTruthCollection( trackTruthColl );
    
    } // endif( m_doTruth )
    

    //-----------------------------------------------------------
    //  Track selection
    //
    
    ATH_MSG_DEBUG("execute: Reco. Tracks in event = "<< static_cast<int>( trackParticleContainer->size() ) );

    // Perform track selection and store it to selectedBaseTracks
    ATH_CHECK( SelGoodTrkParticle( trackParticleContainer ) );
    
    ATH_MSG_DEBUG( "execute: Number of total tracks      = " << trackParticleContainer->size() );
    ATH_MSG_DEBUG( "execute: Number of selected tracks   = " << selectedBaseTracks->size() );
    
    if( m_FillNtuple )
      m_ntupleVars->get<unsigned int>( "NumSelTrks" ) = static_cast<int>( selectedBaseTracks->size() );
    
    // look at truth vertices
    // do this after I have filled m_RecoTrk_barcode in Utilities.cxx
    //
    if( m_FillNtuple && m_doTruth ) {
      
      // this method runs on MCEventCollection, i.e., GEN_AOD or TruthEvent in ESD file
      sc = getNewTruthInfo();
      
      if( sc.isFailure() ) ATH_MSG_INFO("execute: Couldn't get truth info");
    }
    
    
    // fill information about selected tracks in AANT
    ATH_CHECK( fillAANT_SelectedBaseTracks() );
    
    //-------------------------------------------------------
    // Skip the event if the number of selected tracks is more than m_SelTrkMaxCutoff
    
    if(  selectedBaseTracks->size() > m_SelTrkMaxCutoff ) {
      
      if( selectedBaseTracks->size() < 2 ) {
        ATH_MSG_INFO( "execute: Too few (<2) selected reco tracks. Terminated reconstruction." );
      } else {
        ATH_MSG_INFO( "execute: Too many selected reco tracks. Terminated reconstruction." );
      }
      
      return StatusCode::SUCCESS;   
    }
    
    //-------------------------------------------------------
    // Core part of Vertexing
    //
    
    // List of track indices which are not used for the 2-track vertices.
    std::vector<int> Incomp;
    
    // Try to compose 2-track vertices for all combinations of selected tracks greather than the given chi2 cut.
      
    ATH_CHECK( extractIncompatibleTracks( Incomp ) );
    
    ATH_MSG_VERBOSE( "execute: Found= "<<Incomp.size()<<", "<< selectedBaseTracks->size() );
    if( m_FillNtuple ) m_ntupleVars->get<unsigned int>( "SizeIncomp" ) = Incomp.size();
    
    // set of vertices created in the following while loop.
    auto workVerticesContainer = new std::vector<WrkVrt>;
    
    // Reconstruction of initial solution set (2-track vertices)
    ATH_CHECK( reconstruct2TrackVertices( Incomp, workVerticesContainer ) );
    
    // No need to use Incomp anymore.
    Incomp.clear();
    
    // Reconstruction of N-track vertices from 2-track vertices
    ATH_CHECK( reconstructNTrackVertices( workVerticesContainer ) );
    
    if ( m_mergeFinalVerticesDistance ) {
      ATH_MSG_DEBUG("execute: trying to merge vertices within " << m_VertexMergeFinalDistCut << " mm.");
      ATH_CHECK( mergeFinalVertices( workVerticesContainer ) );
    } // end if m_mergeFinalVerticesDistance
    
    
    // Attempt to associate more tracks other than selected tracks to the final vertices
    ATH_CHECK( associateNonSelectedTracks( workVerticesContainer ) );
    
    // Refitting and selection of good-quality vertices
    ATH_CHECK( refitAndSelectGoodQualityVertices( workVerticesContainer ) );
    
    delete workVerticesContainer;
    
    // Fill AANT
    if( m_FillNtuple ) {
      m_tree_Vert->Fill();
      ATH_CHECK( clearNtupleVariables() );
    }
    
    
    ATH_MSG_VERBOSE( "execute: process done." );
    // end
    return StatusCode::SUCCESS;   
    
  }
    
} // end of namespace bracket
