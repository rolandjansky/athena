/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecFuzzy/VrtSecFuzzy.h"

#include "VxVertex/VxTrackAtVertex.h"
#include "GeneratorObjects/HepMcParticleLink.h"

// ROOT Classes
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TROOT.h"
#include "TFile.h"

#include <string>
#include <new>
#include <iostream>
#include <tuple>
#include <functional>
#include <chrono>

#include "TMVA/Reader.h"
#include "TMVA/MethodBDT.h"
#include "MVAUtils/BDT.h"
#include "PathResolver/PathResolver.h"
#include <memory>

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
  VrtSecFuzzy::VrtSecFuzzy(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm                   (name,pSvcLocator),
    
    m_primaryVertices              ( nullptr ),
    m_thePV                        ( nullptr ),
  
    // ToolsHandles
    m_fitSvc                       ( "Trk::TrkVKalVrtFitter", this ),
    m_truthToTrack                 ( "Trk::TruthToTrack/InDetTruthToTrack" ),
    m_trackToVertexTool            ( "Reco::TrackToVertex" ),
    m_trackToVertexIPEstimatorTool ( "Trk::TrackToVertexIPEstimator/TrackToVertexIPEstimator" ),
    m_extrapolator                 ( "Trk::Extrapolator/AtlasExtrapolator" ),
    
    // Services
    m_pixelCondSummarySvc          ( "PixelConditionsSummarySvc", "VrtSecFuzzy" ),
    m_sctCondSummarySvc            ( "SCT_ConditionsSummarySvc", "VrtSecFuzzy" ),
    
    m_checkPatternStrategy         ( "Classical" )    
  {
    
    m_patternStrategyFuncs["Classical"]           = &VrtSecFuzzy::checkTrackHitPatternToVertex;
    m_patternStrategyFuncs["ClassicalOuter"]      = &VrtSecFuzzy::checkTrackHitPatternToVertexOuterOnly;
    m_patternStrategyFuncs["Extrapolation"]       = &VrtSecFuzzy::checkTrackHitPatternToVertexByExtrapolation;
    m_patternStrategyFuncs["ExtrapolationAssist"] = &VrtSecFuzzy::checkTrackHitPatternToVertexByExtrapolationAssist;

    this->declareProperties();
    
  }
  
  

  //__________________________________________________________________________
  VrtSecFuzzy::~VrtSecFuzzy()
  { 
    ATH_MSG_DEBUG("destructor called");
  }



  //__________________________________________________________________________
  StatusCode VrtSecFuzzy::beginRun()  { 

    ATH_MSG_DEBUG("beginRun()");

    return StatusCode::SUCCESS;
  }
  
  
  
  //__________________________________________________________________________
  StatusCode VrtSecFuzzy::initialize()
  {
    //---------------- HBOOK
    ATH_MSG_INFO("initialize: begin");
    //
    // first instantiate tools

    // VKalVrt vertex fitter
    if(m_fitSvc.retrieve().isFailure()) {
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
    
    // Track selection algorithm configuration
    m_trackSelectionAlgs.emplace_back( &VrtSecFuzzy::selectInDetTracks );
    
    // Vertexing algorithm configuration
    m_vertexingAlgorithms.emplace_back( std::pair<std::string, vertexingAlg>( "form2trackVertices", &VrtSecFuzzy::form2trackVertices )     );
    m_vertexingAlgorithms.emplace_back( std::pair<std::string, vertexingAlg>( "merge2trackVertices", &VrtSecFuzzy::merge2trackVertices )     );

    for(auto file : m_jp.BDTFilesName){

//      TFile* rootFile = TFile::Open(Form("%s", m_jp.BDTFilesName.at(ifile).c_str()), "READ"); // for local testing
      std::string rootFilePath = PathResolver::find_calib_file("VrtSecFuzzy/" + file);
      TFile* rootFile = TFile::Open(rootFilePath.c_str(), "READ");    

      if (!rootFile) {
        ATH_MSG_FATAL("Could not retrieve root file: " << file);
        return StatusCode::FAILURE;
      }
      m_tfile.reset(rootFile);

      TTree *training = (TTree*)m_tfile->Get("BDT");
      if (training) {
        MVAUtils::BDT *tmpBDT = new MVAUtils::BDT(training);
        delete training; 
        m_bdt.push_back(tmpBDT);
      }else {
        ATH_MSG_FATAL("Could not retrieve tree: BDT");
        return StatusCode::FAILURE;
      }
    }
/*
    for(unsigned int ifile = 0; ifile < m_jp.BDTFilesName.size(); ifile++){

//      TFile* rootFile = TFile::Open(Form("%s", m_jp.BDTFilesName.at(ifile).c_str()), "READ"); // for local testing
      std::string rootFilePath = PathResolver::find_calib_file("VrtSecFuzzy/"+m_jp.BDTFilesName.at(ifile));
      TFile* rootFile = TFile::Open(rootFilePath.c_str(), "READ");    

      if (!rootFile) {
        ATH_MSG_FATAL("Could not retrieve root file: " << m_jp.BDTFilesName.at(ifile));
        return StatusCode::FAILURE;
      }
      m_tfile.reset(rootFile);

      TTree *training = (TTree*)m_tfile->Get("BDT");
      if (training) {
        MVAUtils::BDT *tmpBDT = new MVAUtils::BDT(training);
        delete training; 
        m_bdt.push_back(tmpBDT);
      }else {
        ATH_MSG_FATAL("Could not retrieve tree: BDT");
        return StatusCode::FAILURE;
      }
    }
*/

    ATH_MSG_INFO("initialize: Exit VrtSecFuzzy::initialize()");
    return StatusCode::SUCCESS; 
  }


  //__________________________________________________________________________
  StatusCode VrtSecFuzzy::finalize()
  {
    
    ATH_MSG_INFO("finalize: VrtSecFuzzy finalize()");
    return StatusCode::SUCCESS; 
  }

  //__________________________________________________________________________
  StatusCode VrtSecFuzzy::initEvent()
  {

    ATH_MSG_DEBUG("initEvent: begin");
    
    ATH_MSG_DEBUG("initEvent: from initEvent ");
    return StatusCode::SUCCESS;
   
  }
  
  
  //__________________________________________________________________________
  StatusCode VrtSecFuzzy::execute()
  {
    //
    ATH_MSG_DEBUG("VrtSecFuzzy execute()");

    // clear ntuple variables
    StatusCode sc = this->initEvent();
    if(sc.isFailure()) {
      ATH_MSG_WARNING("Problem in initEvent ");
      return StatusCode::SUCCESS;
    }
    
    if (sc.isFailure() ) {
      ATH_MSG_WARNING("Failure in getEventInfo() ");
      return StatusCode::SUCCESS;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    //
    // Setup StoreGate Variables
    //
    
    // Check Return StatusCode::Failure if the user-specified container names have duplication.
    {
      std::vector<std::string> userContainerNames { m_jp.secondaryVerticesContainerName, m_jp.all2trksVerticesContainerName, m_jp.twoTrksVerticesInDVContainerName };
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
    
    ATH_CHECK( evtStore()->record( secondaryVertexContainer,    "VrtSecFuzzy_" + m_jp.secondaryVerticesContainerName ) );
    ATH_CHECK( evtStore()->record( secondaryVertexAuxContainer, "VrtSecFuzzy_" + m_jp.secondaryVerticesContainerName + "Aux." ) );
 
    auto *twoTrksVertexInDVContainer    = new xAOD::VertexContainer;
    auto *twoTrksVertexInDVAuxContainer = new xAOD::VertexAuxContainer;
    
    twoTrksVertexInDVContainer ->setStore( twoTrksVertexInDVAuxContainer );
    
    ATH_CHECK( evtStore()->record( twoTrksVertexInDVContainer,    "VrtSecFuzzy_" + m_jp.twoTrksVerticesInDVContainerName ) );
    ATH_CHECK( evtStore()->record( twoTrksVertexInDVAuxContainer, "VrtSecFuzzy_" + m_jp.twoTrksVerticesInDVContainerName + "Aux." ) );
    
    if( m_jp.FillIntermediateVertices ) {
      auto *twoTrksVertexContainer      = new xAOD::VertexContainer;
      auto *twoTrksVertexAuxContainer   = new xAOD::VertexAuxContainer;
      
      twoTrksVertexContainer   ->setStore( twoTrksVertexAuxContainer );
      
      ATH_CHECK( evtStore()->record( twoTrksVertexContainer,      "VrtSecFuzzy_" + m_jp.all2trksVerticesContainerName ) );
      ATH_CHECK( evtStore()->record( twoTrksVertexAuxContainer,   "VrtSecFuzzy_" + m_jp.all2trksVerticesContainerName + "Aux." ) );
    
    }
    
    // Later use elsewhere in the algorithm
    m_selectedTracks.reset  ( new std::vector<const xAOD::TrackParticle*> );
    
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
    for( auto alg : m_trackSelectionAlgs ) {
      ATH_CHECK( (this->*alg)() );
    }
    
    //-------------------------------------------------------
    // Skip the event if the number of selected tracks is more than m_jp.SelTrkMaxCutoff
    ATH_MSG_INFO("execute(): # selected tracks = " << m_selectedTracks->size());
    if( m_selectedTracks->size() < 2 ) {
      ATH_MSG_DEBUG( "execute: Too few (<2) selected reco tracks. Terminated reconstruction." );
      return StatusCode::SUCCESS;   
    }
      
    if( m_selectedTracks->size() > m_jp.SelTrkMaxCutoff ) {
      ATH_MSG_INFO( "execute: Too many selected reco tracks. Terminated reconstruction." );
      return StatusCode::SUCCESS;   
    }
      
    //-------------------------------------------------------
    // Core part of Vertexing
    //
    
    m_vertexingAlgorithmStep = 0;
    
    // set of vertices created in the following while loop.
    auto* workVerticesContainer = new std::vector<WrkVrt>;
    
    // the main sequence of the main vertexing algorithms
    // see initialize() what kind of algorithms exist.
    for( auto vertAlgo : m_vertexingAlgorithms ) {
      
      auto& alg = vertAlgo.second;
     
      ATH_CHECK( (this->*alg)( workVerticesContainer ) );
      
      auto end = std::remove_if( workVerticesContainer->begin(), workVerticesContainer->end(),
                                 []( WrkVrt& wrkvrt ) {
                                   return ( wrkvrt.isGood == false || wrkvrt.ntrks < 2 ); }
                               );
      
      workVerticesContainer->erase( end, workVerticesContainer->end() );

      m_vertexingAlgorithmStep++;
      
    }
    
    delete workVerticesContainer;
    
    ATH_MSG_VERBOSE( "execute: process done." );
    // end
    return StatusCode::SUCCESS;   
    
  }
    
} // end of namespace bracket
