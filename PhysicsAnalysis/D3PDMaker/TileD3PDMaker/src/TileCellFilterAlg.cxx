/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileCellFilterAlg.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>, Gang qin <gang.qin@cern.ch>
 * 
 * Created in September 2012
 * Updated in August 2013
 */

// INCLUDE HEADER
#include "TileCellFilterAlg.h"

//===============================================================================================
TileCellFilterAlg::TileCellFilterAlg( const std::string& name,ISvcLocator* pSvcLocator ): 
    AthAlgorithm( name, pSvcLocator ),
    m_inCalo("TrackInCaloTools/extrapolMuonInCaloTool"){
//==============================================================================================

    // INCLUDE LIQUID ARGON
    declareProperty("IncludeLAr", m_includeLAr="no");
    declareProperty("TryStandAloneMuons", m_standalone = "no");
    declareProperty("TrackType", m_trackType = 0);
    
    // FILTER SETTING: 0=Muon, 1=Tracks
    declareProperty("FilterConfiguration", m_filterConfiguration = 0);

    // TRACK EXTRAPOLATOR TOOL
    declareProperty("ExtrapolateInCalo", m_inCalo);
    declareProperty("ConeSize", m_coneSize = 1.5);

    // CONTAINER NAMES
    declareProperty("InputContainer", m_inputContainerName="SelectedTracks");
    declareProperty("CellsName",m_calocellContainerName="AllCalo");
    declareProperty("OutputCellsName", m_outputCellsName="SelectedCells");

} // TileCellFilterAlg::TileCellFilterAlg

//=============================================
StatusCode TileCellFilterAlg::initialize(){
//=============================================
    // INITIALISE THE CALOCELL FILTER ALGORITHM
    
    ATH_MSG_INFO(" NOW IN TileCellFilterAlg::initialize() ");

    if (m_inCalo.retrieve().isFailure() ) {
        ATH_MSG_ERROR(" Cannot retrieve extrapolator in calorimeter properly. ");
    } // IF

    // RETRIEVE STOREGATE SERVICE
    if(service("StoreGateSvc",m_storeGate).isFailure()) {
        ATH_MSG_ERROR("StoreGate service not found !");
        return StatusCode::FAILURE;
    } // IF

    return StatusCode::SUCCESS;
} // TileCellFilterAlg::initialize()

//==========================================
StatusCode TileCellFilterAlg::execute(){
//==========================================

    // CALOCELL OUTPUT CONTAINER
    m_outputCont = new ConstDataVector<CaloCellContainer>( SG::VIEW_ELEMENTS );
    CHECK( m_storeGate->record( m_outputCont, m_outputCellsName ) );

    // NUMBER OF ASSOCIATED CELLS
    //int counter = 0;
    //int cell_counter = 0;
    //int tilecell_counter = 0;
    bool debug = false;

    // GET HANDLE ON CALORIMETER CELL CONTAINER
    const CONTAINER* cellcoll;
    CHECK( evtStore()->retrieve( cellcoll, m_calocellContainerName ) );

    // SELECT TO FILTER BASED ON WHICH CONFIGURATION:
    // CONFIGURATION 0 IMPLIES CELLS ARE FILTERED BASED ON PROXIMITY TO MUONS
    // CONFIGURATION 1 IMPLIES CELLS ARE FILTERED BASED ON PROXIMITY TO TRACKS
    
    if(m_filterConfiguration == 0){
      // GET HANDLE ON MUON CONTAINER
        const MUONCONTAINER* stac_muons;

        CHECK( m_storeGate->retrieve( stac_muons, "StacoMuonCollection" ) );

        if(debug) ATH_MSG_INFO("FILTERING OUT IRRELEVANT CALOCELLS USING MUON TRACKS... ");
        if(debug) ATH_MSG_INFO("NUMBER OF MUONS: " << ":: " << "StacoMuonCollection" << " | " << stac_muons->size());
        if(debug) ATH_MSG_INFO("NUMBER OF CALOCELLS: " << ":: " << m_calocellContainerName << " | " << cellcoll->size());

        // RETRIEVE RELEVANT CALOCELLS FOR EACH MUON
        std::vector<MUONCONTAINER::const_iterator> start;
        std::vector<MUONCONTAINER::const_iterator> stop;
        start.push_back(stac_muons->begin());
        stop.push_back(stac_muons->end());

        // LOOP OVER MUON TYPES (STACO/MUID)
        for(unsigned int i=0;i<start.size();++i){
            MUONCONTAINER::const_iterator f_mu = start[i];
            MUONCONTAINER::const_iterator l_mu = stop[i];

            // LOOP OVER MUONS
            for( ; f_mu != l_mu; ++f_mu ){
                // RETRIEVE EXTRAPOLATED MUON TRACK
                const Analysis::Muon* muonPointer = ( *f_mu );
                const Trk::Track* muon_trk = muonPointer->combinedMuonTrkTrack();
                switch(m_trackType){
                    case 0:  muon_trk = muonPointer->combinedMuonTrkTrack();      break;
                    case 1:  muon_trk = muonPointer->inDetTrkTrack();             break;
                    case 2:  muon_trk = muonPointer->muonSpectrometerTrkTrack();  break;
                    case 3:  muon_trk = muonPointer->muonExtrapolatedTrkTrack();  break;
                    case 4:  muon_trk = muonPointer->innerExtrapolatedTrkTrack(); break;
                    default: muon_trk = muonPointer->combinedMuonTrkTrack();      break;
                } // SWITCH
                if( !muon_trk ){ continue; }
                m_inCalo->getCellsWithinConeAroundTrack(muon_trk,cellcoll,m_outputCont,m_coneSize,bool(m_includeLAr=="yes"));
            } // FOR (MUONS)
        } // FOR (MUON TYPES)
    } // IF (CONFIGURATION == 0)

    
    else if(m_filterConfiguration == 1){
      cout<<"using track collection: "<<m_inputContainerName<<endl;
      CHECK( m_storeGate->retrieve( trackcoll, m_inputContainerName ) );
      TRACKCONTAINER::const_iterator it = trackcoll->begin();

        // LOOP OVER TRACKS
      for( ; it != trackcoll->end(); ++it ){
	const TRACKPARTICLE* trackPointer = ( *it );
	m_inCalo->getCellsWithinConeAroundTrack(trackPointer,cellcoll,m_outputCont,m_coneSize,bool(m_includeLAr=="yes"));
      } // FOR (TRACKS)
    } // ELSE IF (CONFIGURATION == 1)

    
    if(debug) ATH_MSG_INFO("NUMBER OF ASSOCIATED CELLS: " << m_outputCont->size() << " / " << cellcoll->size() );

    return StatusCode::SUCCESS;
} // TileCellFilterAlg::execute()

//===========================================
StatusCode TileCellFilterAlg::finalize(){
//===========================================
    return StatusCode::SUCCESS;
} // TileCellFilterAlg::finalize()
