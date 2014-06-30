/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileTrackClusterAssociationTool.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>, Gang Qin <gang.qin@cern.ch>
 * Description: Associate tracks to caloclusters 
 *
 * Created in June 2012 
 */

// INCLUDE HEADER
#include "TileTrackClusterAssociationTool.h"

// ADD TILEMUONMUCELLASSOCIATIONTOOL TO D3PD NAMESPACE
using namespace std;

namespace D3PD{

//====================================================================
TileTrackClusterAssociationTool::TileTrackClusterAssociationTool
    (const string& type,const string& name, const IInterface* parent):
    Base(type,name,parent),
    m_caloclusterContainerName( "CaloCalTopoCluster" ),
    m_inCalo("TrackInCaloTools/extrapolMuonInCaloTool"){
//====================================================================
    declareProperty( "ClustersName",m_caloclusterContainerName="CaloCalTopoCluster" );
    declareProperty("ExtrapolateInCaloTool", m_inCalo);
    declareProperty("cut_sizeofcone", m_cutSizeOfCone = 0.4); // CUT IN CONE SIZE

} // TileTrackClusterAssociationTool::TileTrackClusterAssociationTool

//==============================================================
TileTrackClusterAssociationTool::~TileTrackClusterAssociationTool(){
//==============================================================
// DESTRUCTOR
} // TileTrackClusterAssociationTool::~TileTrackClusterAssociationTool()

//=====================================================
StatusCode TileTrackClusterAssociationTool::initialize(){
//=====================================================
// RETRIEVES NECESSARY SERVICES AND TOOLS, INSTANTIATES GLOBAL VARIABLES
    
    ATH_MSG_INFO(" in TileTrackClusterAssociationTool::initialize()");

    // RETRIEVE STOREGATE SERVICE
    CHECK( service("StoreGateSvc",m_storeGate) );

    return StatusCode::SUCCESS;
} // TileTrackClusterAssociationTool::initialize

//=======================================================================
StatusCode TileTrackClusterAssociationTool::reset(const Rec::TrackParticle& p){
//=======================================================================
    
    ATH_MSG_DEBUG(" in TileTrackClusterAssociationTool::reset()");

    if(not &p) ATH_MSG_ERROR("Cluster associator tried an calocluster container.");

    const Rec::TrackParticle* trackPointer = &p;
    double cone = 0.4;

    m_listOfClusters = m_inCalo->getClustersAroundTrack(cone, trackPointer);
    m_caloclusterContainer.clear();
    m_caloclusterContainer.clear();
    
    // LOOP OVER CLUSTERS
    for(unsigned int ic = 0; ic<m_listOfClusters.size(); ++ic){
        const CaloCluster* cluster = m_listOfClusters.at(ic);
        m_caloclusterContainer.push_back(cluster);	
    } // FOR

    m_clusterItr            = m_caloclusterContainer.begin(); 
    m_clusterEnd            = m_caloclusterContainer.end(); 

    return StatusCode::SUCCESS;

} // TileTrackClusterAssociationTool::reset()

//====================================================
const CaloCluster* TileTrackClusterAssociationTool::next(){
//====================================================
    ATH_MSG_DEBUG(" in TileTrackClusterAssociationTool::next()");	

    // RETURN CLUSTER IF IT CAN BE FOUND
    while(m_clusterItr != m_clusterEnd){
        const CaloCluster *cluster = *m_clusterItr;
        ++m_clusterItr;
        return cluster;
    } // WHILE

    return 0;
} // CaloCluster* TileTrackClusterAssociationTool::next()

} // D3PD NAMESPACE
