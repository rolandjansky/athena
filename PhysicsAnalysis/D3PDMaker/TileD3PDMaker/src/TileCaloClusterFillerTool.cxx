/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * File: TileCaloClusterFillerTool.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>, Gang Qin <gang.qin@cern.ch>
 * Date: May 2013 
 *
 */

// INCLUDE HEADER FILE
#include "TileCaloClusterFillerTool.h"

// ADD TILEMUONTRACKFILLERTOOL TO D3PD NAMESPACE
namespace D3PD{
//=========================================================================
TileCaloClusterFillerTool::TileCaloClusterFillerTool (const std::string& type,const std::string& name,const IInterface* parent):
BlockFillerTool<CaloCluster> (type, name, parent),
m_inCalo("TrackInCaloTools/extrapolMuonInCaloTool"){
//=========================================================================

    book().ignore(); // AVOID COVERITY WARNINGS

    declareProperty("CaloClustersName",             m_caloclusterContainerName="CaloCalTopoCluster" );
    declareProperty("LevelOfDetails",        m_LevelOfDetails = 1);

} // TileCaloClusterFillerTool::TileCaloClusterFillerTool

//=================================================
StatusCode TileCaloClusterFillerTool::initialize(){
//=================================================

    ATH_MSG_DEBUG("in TileCaloClusterFillerTool::initialize");

    CHECK( D3PD::BlockFillerTool<CaloCluster>::initialize() );
    CHECK( service("StoreGateSvc",m_storeGate));

    return StatusCode::SUCCESS;
} // TileCaloClusterFillerTool::initialize

//===========================================
StatusCode TileCaloClusterFillerTool::book(){
//===========================================

    // VARIABLES
 if(m_LevelOfDetails > 0){
 CHECK( addVariable("E",                 m_e,                           "", m_defaultValue));
        CHECK( addVariable("eta",               m_eta,                         "", m_defaultValue));
        CHECK( addVariable("phi",               m_phi,                         "", m_defaultValue));
        CHECK( addVariable("NSamples",          m_nSamples,                    "", m_defaultValue));
        CHECK( addVariable("time",              m_getTime,                     "", m_defaultValue));
        CHECK( addVariable("size",              m_getClusterSize,              "", m_defaultValue));
	 CHECK( addVariable("eta0",              m_eta0,                        "", m_defaultValue));
        CHECK( addVariable("phi0",              m_phi0,                        "", m_defaultValue));
 }
  /*
    if(m_LevelOfDetails > 0){
        CHECK( addVariable("E",                 m_e,                           "", m_defaultValue));
        CHECK( addVariable("eta",               m_eta,                         "", m_defaultValue));
        CHECK( addVariable("phi",               m_phi,                         "", m_defaultValue));
        CHECK( addVariable("NSamples",          m_nSamples,                    "", m_defaultValue));
        CHECK( addVariable("time",              m_getTime,                     "", m_defaultValue));
        CHECK( addVariable("size",              m_getClusterSize,              "", m_defaultValue));
        CHECK( addVariable("etasize",           m_getClusterEtaSize,           "", m_defaultValue));
        CHECK( addVariable("phisize",           m_getClusterPhiSize,           "", m_defaultValue));
        CHECK( addVariable("eta0",              m_eta0,                        "", m_defaultValue));
        CHECK( addVariable("phi0",              m_phi0,                        "", m_defaultValue));
    } // IF

    if(m_LevelOfDetails > 5){
        CHECK( addVariable("sample_E",          m_eSample                      ));
        CHECK( addVariable("sample_eta",        m_etaSample                    ));
        CHECK( addVariable("sample_phi",        m_phiSample                    ));
    } // IF
  */

    return StatusCode::SUCCESS;
} // TileCaloClusterFillerTool::book

//===============================================================
StatusCode TileCaloClusterFillerTool::fill(const CaloCluster& p){
//===============================================================

    ATH_MSG_DEBUG("in TileCaloClusterFillerTool::fill");
    CHECK( m_storeGate->retrieve(m_caloclusterContainer, m_caloclusterContainerName));

    // RETRIEVE APPROPRIATE POINTERS
    const CaloCluster* clusterPointer = &p;
    
    if(m_LevelOfDetails > 0){
        *m_e                 = clusterPointer->e();
        *m_eta               = clusterPointer->eta();
        *m_phi               = clusterPointer->phi();
        *m_nSamples          = clusterPointer->nSamples();
        *m_getTime           = clusterPointer->getTime();
        *m_getClusterSize    = clusterPointer->getClusterSize();
	// *m_getClusterEtaSize = clusterPointer->getClusterEtaSize();
	// *m_getClusterPhiSize = clusterPointer->getClusterPhiSize();
        *m_eta0              = clusterPointer->eta0();
        *m_phi0              = clusterPointer->phi0();
    } // IF
    /*
    // LOOP OVER ALL CALORIMETER SAMPLES
    if(m_LevelOfDetails > 5){
        for(int i=0;i<(int)CaloSampling::Unknown;i++) {
            const CaloSampling::CaloSample sam = (CaloSampling::CaloSample)i;
            m_eSample->push_back(clusterPointer->eSample(sam));
            m_etaSample->push_back(clusterPointer->etaSample(sam));
            m_phiSample->push_back(clusterPointer->phiSample(sam));
        } // FOR
    } // IF
    */
    return StatusCode::SUCCESS;
} // TileCaloClusterFillerTool::fill

} // namespace D3PD
