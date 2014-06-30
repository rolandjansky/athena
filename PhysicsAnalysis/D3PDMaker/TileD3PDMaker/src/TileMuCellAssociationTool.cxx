/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileMuCellAssociationTool.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Description: Associate calocells to muons, used for calibration studies of TileCal using muons.
 *
 * Created in September 2012
 */

// INCLUDE HEADER
#include "TileMuCellAssociationTool.h"

// ADD TILEMUONMUCELLASSOCIATIONTOOL TO D3PD NAMESPACE
using namespace std;
namespace D3PD{

//====================================================================
TileMuCellAssociationTool::TileMuCellAssociationTool
    (const string& type,const string& name, const IInterface* parent):
    Base(type,name,parent),
    m_calocellContainerName( "SelectedCells" ),
    m_inCalo("TrackInCaloTools/extrapolMuonInCaloTool"){
//====================================================================
// CONSTRUCTOR
    declareProperty( "CellsName",m_calocellContainerName="SelectedCells" );
    declareProperty("TryStandAloneMuons",    m_standalone = "no");

    // TRACK EXTRAPOLATOR TOOL
    declareProperty("ExtrapolateInCaloTool", m_inCalo);
    declareProperty("TrackType",             m_trackType = 0);
} // TileMuCellAssociationTool::TileMuCellAssociationTool

//==============================================================
TileMuCellAssociationTool::~TileMuCellAssociationTool(){
//==============================================================
// DESTRUCTOR
} // TileMuCellAssociationTool::~TileMuCellAssociationTool()

//=====================================================
StatusCode TileMuCellAssociationTool::initialize(){
//=====================================================
// RETRIEVES NECESSARY SERVICES AND TOOLS, INSTANTIATES GLOBAL VARIABLES
    
    ATH_MSG_INFO(" in TileMuCellAssociationTool::initialize()");

    // RETRIEVE STOREGATE SERVICE
    CHECK( service("StoreGateSvc",m_storeGate) );
    m_muon = NULL;
    m_muon_CB_trk = NULL;

    return StatusCode::SUCCESS;
} // TileMuCellAssociationTool::initialize

//=======================================================================
StatusCode TileMuCellAssociationTool::reset(const Analysis::Muon& p){
//=======================================================================
    
    ATH_MSG_DEBUG(" in TileMuCellAssociationTool::reset()");

    if(not &p) ATH_MSG_ERROR("Muon associator tried an calocell container.");

    m_muon = &p;
    const Trk::Track* muon_trk           = m_muon->combinedMuonTrkTrack();
    //const Rec::TrackParticle* muon_track = m_muon->combinedMuonTrackParticle();
    switch(m_trackType){
        case 0:  muon_trk   = m_muon->combinedMuonTrkTrack(); 
                 //muon_track = m_muon->combinedMuonTrackParticle();
                 break;
        case 1:  muon_trk   = m_muon->inDetTrkTrack();
                 //muon_track = m_muon->inDetTrackParticle();
                 break;
        case 2:  muon_trk   = m_muon->muonSpectrometerTrkTrack();  
                 //muon_track = m_muon->muonSpectrometerTrackParticle();
                 break;
        case 3:  muon_trk   = m_muon->muonExtrapolatedTrkTrack();  
                 //muon_track = m_muon->muonExtrapolatedTrackParticle();
                 break;
        case 4:  muon_trk   = m_muon->innerExtrapolatedTrkTrack(); 
                 //muon_track = m_muon->innerExtrapolatedTrackParticle();
                 break;
        default: muon_trk   = m_muon->combinedMuonTrkTrack();      
                 //muon_track = m_muon->combinedMuonTrackParticle();
                 break;
    } // SWITCH

    m_listListCells = m_inCalo->getCellsAroundTrack(muon_trk,1.5,tile1,tile3,false);

    for(unsigned int il=0; il<m_listListCells.size(); ++il){
        std::vector<const CaloCell*> listOfCells = m_listListCells.at(il);
        for(unsigned int ic=0; ic<listOfCells.size(); ++ic){
            const CaloCell* cell = listOfCells.at(ic);
            if(std::find(m_calocellContainer.begin(),m_calocellContainer.end(),cell) == m_calocellContainer.end()){
                if(dynamic_cast<const TileCell*> (cell)){
                    m_calocellContainer.push_back( cell );
                } // IF
            } // IF
        } // FOR (CELLS)
    } // FOR (LAYERS)

    m_cellItr            = m_calocellContainer.begin();
    m_cellEnd            = m_calocellContainer.end();

    return StatusCode::SUCCESS;

} // TileMuCellAssociationTool::reset()

//====================================================
const CaloCell* TileMuCellAssociationTool::next(){
//====================================================
    
    ATH_MSG_DEBUG(" in TileMuCellAssociationTool::next()");

    while(m_cellItr != m_cellEnd){
        const CaloCell *cell = *m_cellItr;
        ++m_cellItr;
        return cell;
    } // WHILE

    return 0;
} // CaloCell* TileMuCellAssociationTool::next()

} // D3PD NAMESPACE
