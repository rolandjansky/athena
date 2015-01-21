/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * File:   TileCosmicMuonFilterAlg.cxx
 * Author: Carlos.Solans@cern.ch
 * Author: Archil.Durglishvili@cern.ch
 */

#include "TileCosmicMuonFilterAlg.h"
#include "CaloGeoHelpers/CaloSampling.h" 
#include "AthContainers/ConstDataVector.h"
#include "xAODTracking/TrackingPrimitives.h" 

using namespace std;
using namespace xAOD;

//=======================================
TileCosmicMuonFilterAlg::TileCosmicMuonFilterAlg( const std::string& name, ISvcLocator* pSvcLocator ): 
  AthAlgorithm( name, pSvcLocator ),  m_trackInCalo("TrackExtrapolator"){
//=======================================
  declareProperty("InputTracks",     m_inputTracks     = "CombinedMuonTrackParticles");
  declareProperty("InputMuons",      m_inputMuons      = "Muons");
  declareProperty("OutputMuons",     m_outputMuons     = "SelectedMuons");
  declareProperty("OutputTracks",    m_outputTracks    = "SelectedTracks");
  declareProperty("InputCells",      m_inputCells      = "AODCellContainer");
  declareProperty("OutputCells",     m_outputCells     = "SelectedCells");
  declareProperty("MuonCells",       m_muonCells       = "muonCells");
  declareProperty("MuonPtCut",       m_muonPt          = 10000.);
  declareProperty("MuonEtaCut",      m_muonEta         = 1.7);
  declareProperty("MuonCellR",       m_muonCellR       = 1.0);  
  declareProperty("DumpLArCells",    m_dumpLarCells    = false);
  declareProperty("UseCuts",         m_useCuts         = false);
  declareProperty("TrackTools",      m_trackInCalo);
  declareProperty("TrackType",       m_trackType       = 1);
  m_storeGate =	0;
} 

//=======================================
StatusCode TileCosmicMuonFilterAlg::initialize(){
//=======================================
  ATH_MSG_INFO("TileCosmicMuonFilterAlg::initialize()");

  CHECK(service("StoreGateSvc",m_storeGate));
  CHECK(m_trackInCalo.retrieve());
  
  return StatusCode::SUCCESS;
} 

//=======================================
StatusCode TileCosmicMuonFilterAlg::execute(){
//=======================================
   switch(m_trackType)
   {
     case 0:  m_inputTracks = "CombinedMuonTrackParticles";       break;
     case 1:  m_inputTracks = "InDetTrackParticles" ;             break;
     case 2:  m_inputTracks = "MuonSpectrometerTrackParticles" ;  break;
     default: m_inputTracks = "CombinedMuonTrackParticles";       break;
   } // SWITCH


  //Get the input tracks
  const TRACKCONTAINER* inputTracks = 0;
  CHECK( m_storeGate->retrieve( inputTracks, m_inputTracks ) );

  //Get the input Muons
  const MUONCONTAINER* inputMuons = 0;
  CHECK( m_storeGate->retrieve( inputMuons, m_inputMuons ) );
  ATH_MSG_INFO("Number of Muons: " << inputMuons->size());

  //Allocate the output Muons container
  MUONCONTAINER* outputMuons = new MUONCONTAINER;
  MUONAUXCONTAINER* outputAuxMuons = new MUONAUXCONTAINER;
  outputMuons->setStore( outputAuxMuons );


  //Allocate the output tracks container
  TRACKCONTAINER * outputTracks = new TRACKCONTAINER;
  TRACKAUXCONTAINER* outputAuxTracks = new TRACKAUXCONTAINER;
  outputTracks->setStore( outputAuxTracks );


  //Get input cells
  const CELLCONTAINER* inputCells = 0;
  CHECK( evtStore()->retrieve( inputCells, m_inputCells ) );

  //Allocate output cells container
  ConstDataVector<CELLCONTAINER>* outputCells = new ConstDataVector<CELLCONTAINER>( SG::VIEW_ELEMENTS );
  CHECK( m_storeGate->record( outputCells, m_outputCells ) );

  //Allocate cells container which will be refilled for every muon; temporary use only
  ConstDataVector<CELLCONTAINER>* tmpCells = new ConstDataVector<CELLCONTAINER>( SG::VIEW_ELEMENTS );


  //===============================================
  const int Ncut=7;
  int cut[Ncut];
  for( int i = 0; i < Ncut; ++i) cut[i] = 0;
  //===============================================

  //Select Muons and corresponding tracks
  MUONCONTAINER::const_iterator muonItr = inputMuons->begin();
  MUONCONTAINER::const_iterator muonEnd = inputMuons->end();
  for(; muonItr != muonEnd; ++muonItr ){
    const MUON* muon = *muonItr;
    if(!muon){ ATH_MSG_INFO("Not a valid muon"); continue; }
    //Cut 0:  combined muon // probably it is better to use this cut
    if( m_useCuts && muon->muonType()!=xAOD::Muon::Combined ) continue;
    cut[0]++;
    //Cut 1: transverse momentum
    if( m_useCuts && muon->pt() < m_muonPt ) continue;
    cut[1]++;
    //Cut 2: Eta
    if( m_useCuts && fabs(muon->eta()) > m_muonEta ) continue;
    cut[2]++;
    if( m_useCuts )
    {
      //============Cut 3: Criteria for the associated inner detector track (all muon collections)=========
      /// Returns true if this Muon passes the MCP ID hit cuts (see the MCP twiki for definitions:
      /// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MuonPerformance)
      if( !muon->passesIDCuts() ) continue;
    }
    cut[3]++;
    //=====================================================================================================

    //===============save only muons which have the xAOD::TrackParticle of m_trackType===========
    /// primaryTrackParticle() Returns a pointer (which should not usually be NULL, but might be if the muon has been stripped of information) to the
    /// primary TrackParticle corresponding to the MuonType of this muon.
    /// This is determined in the following order:
    ///  1. CombinedTrackParticle
    ///  2. InnerDetectorTrackParticle
    ///  3. MuonSpectrometerTrackParticle
    const TRACK* muon_track = 0;
    switch(m_trackType)
    {
      case 0:  muon_track = muon->trackParticle(xAOD::Muon::CombinedTrackParticle);          break;
      case 1:  muon_track = muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);     break;
      case 2:  muon_track = muon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);  break;
      default: muon_track = muon->primaryTrackParticle();                                    break;
    } // SWITCH
    if(!muon_track) continue;
    cut[4]++;
    //====================================================================
    //Fill the muon container
    MUON* tmpMuon = new MUON();
    tmpMuon->makePrivateStore( muon );
    outputMuons->push_back(tmpMuon);
  }// Muons loop
 

  //Allocate output association between muons and cells
  ASSOCCONTAINER* muonCells = new ASSOCCONTAINER_CONSTRUCTOR(outputMuons->size());
  CHECK( m_storeGate->record( muonCells, m_muonCells ) );
  ASSOCCONTAINER::iterator assocItr = muonCells->begin();


  muonItr = outputMuons->begin();
  muonEnd = outputMuons->end();
  for(; muonItr != muonEnd; ++muonItr ){
    const MUON* muon = *muonItr;

    /// primaryTrackParticle() Returns a pointer (which should not usually be NULL, but might be if the muon has been stripped of information) to the
    /// primary TrackParticle corresponding to the MuonType of this muon.
    /// This is determined in the following order:
    ///  1. CombinedTrackParticle
    ///  2. InnerDetectorTrackParticle
    ///  3. MuonSpectrometerTrackParticle
    const TRACK* muon_track = 0;
    switch(m_trackType)
    {
      case 0:  muon_track = muon->trackParticle(xAOD::Muon::CombinedTrackParticle);          break;
      case 1:  muon_track = muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);     break;
      case 2:  muon_track = muon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);  break;
      default: muon_track = muon->primaryTrackParticle();                                    break;
    } // SWITCH
    if(!muon_track) continue;  

    //save the track associated to muon
    TRACK* tmpTrack = new TRACK();
    tmpTrack->makePrivateStore( muon_track );
    outputTracks->push_back(tmpTrack);


    tmpCells->clear();
    m_trackInCalo->getCellsWithinConeAroundTrack(muon_track,inputCells, tmpCells, m_muonCellR , m_dumpLarCells );

    //loop over the selected cells in order to associat to muon
    CELLCONTAINER::const_iterator cellItr = tmpCells->begin();
    CELLCONTAINER::const_iterator cellEnd = tmpCells->end();
    for( ;cellItr != cellEnd; ++cellItr){
      const CELL* cell = *cellItr;
      unsigned int i=0;
      while(i!=outputCells->size()){if(outputCells->at(i)==cell){break;}i++;}
      if(i==outputCells->size()){
         outputCells->push_back(cell);
      }
      assocItr->push_back(i);//FIXME
    }
    ++assocItr;
  }// output Muons loop

  //for( int i = 0; i < Ncut; ++i) ATH_MSG_INFO(" cut["<<i<<"] = "<< cut[i] );

  ATH_MSG_DEBUG("Number of selected Muons: "<< outputMuons->size() );
  ATH_MSG_DEBUG("Number of selected cells: " << outputCells->size());

  CHECK( evtStore()->record(outputMuons, m_outputMuons) );
  CHECK( evtStore()->record(outputAuxMuons,m_outputMuons+"Aux.") );
  CHECK( evtStore()->record(outputTracks, m_outputTracks) );
  CHECK( evtStore()->record(outputAuxTracks,m_outputTracks+"Aux.") );

  delete tmpCells; // clean up memory 
  return StatusCode::SUCCESS;
}

