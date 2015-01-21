/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * File: TileCaloClusterFillerTool.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>, Gang Qin <gang.qin@cern.ch>, Archil Durglishvili <archil.durglishvili@cern.ch>
 * Date: Nov 2014 
 *
 */

// INCLUDE HEADER FILE
#include "TileClusterFillerTool.h"

using namespace D3PD;

//=========================================================================
TileClusterFillerTool::TileClusterFillerTool (const std::string& type,
					      const std::string& name,
					      const IInterface* parent):
  BlockFillerTool<xAOD::CaloCluster> (type, name, parent){
//=========================================================================
  
  book().ignore(); // AVOID COVERITY WARNINGS
  declareProperty("LevelOfDetails", m_LevelOfDetails = 2);
  
} // TileClusterFillerTool::TileCaloClusterFillerTool

//=================================================
StatusCode TileClusterFillerTool::initialize(){
//=================================================

    ATH_MSG_DEBUG("TileClusterFillerTool::initialize");

    CHECK( D3PD::BlockFillerTool<xAOD::CaloCluster>::initialize() );
    //CHECK( service("StoreGateSvc",m_storeGate));

    return StatusCode::SUCCESS;
} // TileClusterFillerTool::initialize

//===========================================
StatusCode TileClusterFillerTool::book(){
//===========================================

  if(m_LevelOfDetails > 0){
    CHECK( addVariable("E",                 m_e,                           "", m_defaultValue));
    CHECK( addVariable("eta",               m_eta,                         "", m_defaultValue));
    CHECK( addVariable("phi",               m_phi,                         "", m_defaultValue));
    CHECK( addVariable("NSamples",          m_nSamples,                    "", m_defaultValue));
    CHECK( addVariable("time",              m_time,                        "", m_defaultValue));
    CHECK( addVariable("size",              m_size,                        "", m_defaultValue));
    CHECK( addVariable("etasize",           m_etaSize,                     "", m_defaultValue));
    CHECK( addVariable("phisize",           m_phiSize,                     "", m_defaultValue));
    CHECK( addVariable("eta0",              m_eta0,                        "", m_defaultValue));
    CHECK( addVariable("phi0",              m_phi0,                        "", m_defaultValue));
  }
  
  if(m_LevelOfDetails > 1){
    CHECK( addVariable("sample_E",          m_eSample                      ));
    CHECK( addVariable("sample_eta",        m_etaSample                    ));
    CHECK( addVariable("sample_phi",        m_phiSample                    ));
  }
  
  return StatusCode::SUCCESS;

} // TileClusterFillerTool::book

//===============================================================
StatusCode TileClusterFillerTool::fill(const xAOD::CaloCluster& p){
//===============================================================
  ATH_MSG_DEBUG("TileClusterFillerTool::fill()"); 

  // RETRIEVE APPROPRIATE POINTERS
  const xAOD::CaloCluster* clusterPointer = &p;

  if(!clusterPointer) { ATH_MSG_INFO("NULL POINTER"); return StatusCode::RECOVERABLE; }
    
  if(m_LevelOfDetails > 0){
    *m_e        = clusterPointer->e();
    *m_eta      = clusterPointer->eta();
    *m_phi      = clusterPointer->phi();
    *m_nSamples = clusterPointer->nSamples();
    //*m_time     = clusterPointer->getTime();
    //*m_size     = clusterPointer->getClusterSize();
    *m_etaSize  = clusterPointer->getClusterEtaSize();
    *m_phiSize  = clusterPointer->getClusterPhiSize();
    *m_eta0     = clusterPointer->eta0();
    *m_phi0     = clusterPointer->phi0();
  } 
    
  if(m_LevelOfDetails > 1){
    for(int i=0;i<(int)CaloSampling::Unknown;i++) {
      const CaloSampling::CaloSample sam = (CaloSampling::CaloSample)i;
      m_eSample->push_back(clusterPointer->eSample(sam));
      m_etaSample->push_back(clusterPointer->etaSample(sam));
      m_phiSample->push_back(clusterPointer->phiSample(sam));
    }
  } 

  return StatusCode::SUCCESS;

} // TileClusterFillerTool::fill

