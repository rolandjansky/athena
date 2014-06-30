/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileCellMuAssociationTool.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Description: Associate muons to calocells, used in calibration studies of TileCal using muons.
 *
 * Created in September 2012
 */

#include "TileCellMuAssociationTool.h"

// USE STANDARD NAMESPACE
using namespace std;

// ADD TILEMUONCELLMUASSOCIATIONTOOL TO D3PD NAMESPACE
namespace D3PD{

//====================================================================
TileCellMuAssociationTool::TileCellMuAssociationTool
    (const string& type,const string& name, const IInterface* parent):
      Base(type,name,parent),
    m_inCalo("TrackInCaloTools/extrapolMuonInCaloTool"){
//====================================================================
    // TRACK EXTRAPOLATOR TOOL
    declareProperty("ExtrapolateInCaloTool", m_inCalo);
    declareProperty("TryStandAloneMuons",    m_standalone = "no");
    declareProperty("TrackType",             m_trackType = 0);
} // TileCellMuAssociationTool::TileCellMuAssociationTool

//==============================================================
TileCellMuAssociationTool::~TileCellMuAssociationTool(){
//==============================================================
// DESTRUCTOR
} // TileCellMuAssociationTool::~TileCellMuAssociationTool()

//=====================================================
StatusCode TileCellMuAssociationTool::initialize(){
//=====================================================
// RETRIEVES NECESSARY SERVICES AND TOOLS, INSTANTIATES GLOBAL VARIABLES
    ATH_MSG_DEBUG(" in TileCellMuAssociationTool::initialize()");

    // RETRIEVE STOREGATE SERVICE
    CHECK( service("StoreGateSvc",m_storeGate) );
    m_muonIndex = 0;

    return StatusCode::SUCCESS;
} // TileCellMuAssociationTool::initialize

//=================================================================
StatusCode TileCellMuAssociationTool::reset(const CaloCell& p){
//=================================================================
    ATH_MSG_DEBUG(" in TileCellMuAssociationTool::reset()");

    m_cell = &p;

    // RETRIEVE ALL MUON OBJECTS FROM STOREGATE
    CHECK( m_storeGate->retrieve( m_muons, "StacoMuonCollection" ) );

    ATH_MSG_DEBUG("NUMBER OF MUONS:" << m_muons->size() );

    Analysis::MuonContainer::const_iterator f_mu = m_muons->begin();
    Analysis::MuonContainer::const_iterator l_mu = m_muons->end();
    for( ; f_mu != l_mu ; ++f_mu ){
        const Analysis::Muon* muonPointer       = *f_mu;
        if(!muonPointer) break;
        const Trk::Track* muon_trk = NULL;
        switch(m_trackType){
            case 0:  muon_trk = muonPointer->combinedMuonTrkTrack();      break;
            case 1:  muon_trk = muonPointer->inDetTrkTrack();             break;
            case 2:  muon_trk = muonPointer->muonSpectrometerTrkTrack();  break;
            case 3:  muon_trk = muonPointer->muonExtrapolatedTrkTrack();  break;
            case 4:  muon_trk = muonPointer->innerExtrapolatedTrkTrack(); break;
            default: muon_trk = muonPointer->combinedMuonTrkTrack();      break;
        } // SWITCH
        if( !muon_trk ){ continue; }
        m_allMuons.push_back(muonPointer);
        m_allTracks.push_back(muon_trk);
    } // FOR (MUONS)

    return StatusCode::SUCCESS;

} // TileCellMuAssociationTool::reset(const CaloCell& p)

//==========================================================
const Analysis::Muon* TileCellMuAssociationTool::next(){
//==========================================================
    ATH_MSG_DEBUG(" in TileCellMuAssociationTool::next()");

    // RETURN POINTER TO MUON AS SOON AS MATCHING CELL AROUND TRACK HAS BEEN FOUND
    for(unsigned int i=m_muonIndex;i<m_allTracks.size();++i){
        ++m_muonIndex;
        const Analysis::Muon* muonPointer       = m_allMuons.at(i);
        const Trk::Track*     muon_CB_trk       = m_allTracks.at(i);

        std::vector<std::vector<const CaloCell* > > listListCells = m_inCalo->getCellsAroundTrack(muon_CB_trk,1.5,tile1,tile3,false);
        for(unsigned int il=0; il<listListCells.size(); ++il){
            std::vector<const CaloCell*> listOfCells = listListCells.at(il);
            if(std::find(listOfCells.begin(),listOfCells.end(),m_cell) != listOfCells.end()){
                if(dynamic_cast<const TileCell*> (m_cell))
                    return muonPointer;
            } // FOR (LAYERS)
        } // FOR (MUONS)
    } // FOR (MUON TYPES)

    return 0;

} // Analysis::Muon* TileCellMuAssociationTool::next()

} // D3PD NAMESPACE
