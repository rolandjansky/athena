/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileTrackCellAssociationTool.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>, Gang Qin <gang.qin@cern.ch> 
 * Description: Associate tracks to calocells 
 *
 * Created in June 2013 
 */

// INCLUDE HEADER
#include "TileTrackCellAssociationTool.h"

// ADD TILEMUONMUCELLASSOCIATIONTOOL TO D3PD NAMESPACE
using namespace std;
namespace D3PD{

//====================================================================
TileTrackCellAssociationTool::TileTrackCellAssociationTool
    (const string& type,const string& name, const IInterface* parent):
    Base(type,name,parent),
    m_calocellContainerName( "SelectedCells" ),
    m_inCalo("TrackInCaloTools/extrapolMuonInCaloTool"){
//====================================================================
// CONSTRUCTOR
    declareProperty( "CellsName",m_calocellContainerName="SelectedCells" );

    // TRACK EXTRAPOLATOR TOOL
    declareProperty("ExtrapolateInCaloTool", m_inCalo);

} // TileTrackCellAssociationTool::TileTrackCellAssociationTool

//==============================================================
TileTrackCellAssociationTool::~TileTrackCellAssociationTool(){
//==============================================================
// DESTRUCTOR
} // TileTrackCellAssociationTool::~TileTrackCellAssociationTool()

//=====================================================
StatusCode TileTrackCellAssociationTool::initialize(){
//=====================================================
// RETRIEVES NECESSARY SERVICES AND TOOLS, INSTANTIATES GLOBAL VARIABLES
    
    ATH_MSG_INFO(" in TileTrackCellAssociationTool::initialize()");

    // RETRIEVE STOREGATE SERVICE
    CHECK( service("StoreGateSvc",m_storeGate) );

    return StatusCode::SUCCESS;
} // TileTrackCellAssociationTool::initialize



//=======================================================================
StatusCode TileTrackCellAssociationTool::reset(const Rec::TrackParticle& p){
//=======================================================================
    
    ATH_MSG_DEBUG(" in TileTrackCellAssociationTool::reset()");

    if(not &p) ATH_MSG_ERROR("Cluster associator tried an calocell container.");
    const Rec::TrackParticle* trackPointer = &p;

    m_listListCells = m_inCalo->getCellsAroundTrack(trackPointer,1.5,tile1,tile3,false);
    m_calocellContainer.clear();
    for(unsigned int il=0; il<m_listListCells.size(); ++il){
        std::vector<const CaloCell*> listOfCells = m_listListCells.at(il);
        for(unsigned int ic=0; ic<listOfCells.size(); ++ic){
            const CaloCell* cell = listOfCells.at(ic);
	    if(std::find(m_calocellContainer.begin(),m_calocellContainer.end(),cell) == m_calocellContainer.end()){	
		if (dynamic_cast<const TileCell*> (cell)){
	    	    m_calocellContainer.push_back( cell );	
		}  // IF
	    } // IF
	} // FOR (CELLS)
    } // FOR(LAYERS)

    m_cellItr            = m_calocellContainer.begin(); 
    m_cellEnd            = m_calocellContainer.end(); 

    return StatusCode::SUCCESS;

} // TileTrackCellAssociationTool::reset()

//====================================================
const CaloCell* TileTrackCellAssociationTool::next(){
//====================================================
    ATH_MSG_DEBUG(" in TileTrackCellAssociationTool::next()");

    while(m_cellItr != m_cellEnd){
        const CaloCell *cell = *m_cellItr;
        ++m_cellItr;
        return cell;
    } // WHILE

    return 0;
} // CaloCell* TileTrackCellAssociationTool::next()

} // D3PD NAMESPACE
