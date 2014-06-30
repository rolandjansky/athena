/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileClusterCellAssociationTool.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>, Gang Qin <gang.qin@cern.ch>
 * Description: Associate caloclusters to calocells
 *
 * Created in June 2013 
 */

// INCLUDE HEADER
#include "TileClusterCellAssociationTool.h"

// ADD TILEMUONMUCELLASSOCIATIONTOOL TO D3PD NAMESPACE
using namespace std;
namespace D3PD{

//====================================================================
TileClusterCellAssociationTool::TileClusterCellAssociationTool
    (const string& type,const string& name, const IInterface* parent):
    Base(type,name,parent),
    m_calocellContainerName( "SelectedCells" ),
    m_inCalo("TrackInCaloTools/extrapolMuonInCaloTool"){
//====================================================================
// CONSTRUCTOR

    declareProperty( "CellsName",m_calocellContainerName="SelectedCells" );
    declareProperty("ExtrapolateInCaloTool", m_inCalo);

} // TileClusterCellAssociationTool::TileClusterCellAssociationTool

//==============================================================
TileClusterCellAssociationTool::~TileClusterCellAssociationTool(){
//==============================================================
// DESTRUCTOR
} // TileClusterCellAssociationTool::~TileClusterCellAssociationTool()

//=====================================================
StatusCode TileClusterCellAssociationTool::initialize(){
//=====================================================
// RETRIEVES NECESSARY SERVICES AND TOOLS, INSTANTIATES GLOBAL VARIABLES
    
    ATH_MSG_INFO(" in TileClusterCellAssociationTool::initialize()");

    // RETRIEVE STOREGATE SERVICE
    CHECK( service("StoreGateSvc",m_storeGate) );

    return StatusCode::SUCCESS;
} // TileClusterCellAssociationTool::initialize

//=====================================================================
StatusCode TileClusterCellAssociationTool::reset(const CaloCluster& p){
//=====================================================================
    
    ATH_MSG_DEBUG(" in TileClusterCellAssociationTool::reset()");

    if(not &p) ATH_MSG_ERROR("Cluster associator tried an calocell container.");

    if(p.isCellLinkValid() != 0){
        m_cellItr            = p.cell_begin(); 
        m_cellEnd            = p.cell_end(); 
    } // IF  
    else{
        m_cellItr = m_cellEnd = CaloCluster::cell_iterator();
    } // ELSE

    return StatusCode::SUCCESS;

} // TileClusterCellAssociationTool::reset()

//====================================================
const CaloCell* TileClusterCellAssociationTool::next(){
//====================================================
    ATH_MSG_DEBUG(" in TileClusterCellAssociationTool::next()");	 	

    while(m_cellItr != m_cellEnd){
        const CaloCell *cell = *m_cellItr;
        ++m_cellItr;
        return cell;
    } // WHILE

    return 0;
} // CaloCell* TileClusterCellAssociationTool::next()

} // D3PD NAMESPACE
