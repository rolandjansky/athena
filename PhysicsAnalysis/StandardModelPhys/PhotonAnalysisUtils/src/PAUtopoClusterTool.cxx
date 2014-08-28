/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PhotonAnalysisUtils/PAUtopoClusterTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"

#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

#include "PhotonAnalysisUtils/PAUutils.h"

// constructor
PAUtopoClusterTool::PAUtopoClusterTool(const std::string& t, const std::string& n, const IInterface* p) : AthAlgTool(t,n,p) {
  declareInterface<IPAUtopoClusterTool>(this);
  
  declareProperty("EtThreshold",m_EtThreshold=2000) ;
  declareProperty("DeltaRMatch",m_DeltaRMatch=0.1) ;
  declareProperty("EtaMax",m_EtaMax=4.8);
}

// destructor
PAUtopoClusterTool::~PAUtopoClusterTool() {}

// Athena standard methods

// initialize
StatusCode PAUtopoClusterTool::initialize() {
  MsgStream log(msgSvc(), name());
  log <<  MSG::INFO  << name() <<"in initialize()..." << endreq;

  //  std::cout <<"Initializing PAUtopoClusterTool ..." << std::endl ;
  return StatusCode::SUCCESS;
}

// finalize
StatusCode PAUtopoClusterTool::finalize() {
  MsgStream log(msgSvc(), name());
  log <<  MSG::VERBOSE  << "... in finalize() ..." << endreq ;
  return StatusCode::SUCCESS;
}

const CaloCluster* PAUtopoClusterTool::getCluster(const CaloCluster* refClus, const CaloClusterContainer* clusContainer, const std::vector<float>& dRcone, 
						  std::vector<float>& EtCone) {
  unsigned Ncones = dRcone.size() ;
  EtCone = std::vector<float>(Ncones) ;
  //
  CaloClusterContainer::const_iterator itr   = clusContainer->begin();
  CaloClusterContainer::const_iterator itr_e = clusContainer->end();
  float dRbest = m_DeltaRMatch ;
  const CaloCluster* bestClus = 0 ;
  //
  for ( ; itr != itr_e ; ++itr ) {
    const CaloCluster* clus = (*itr);
    float clus_et = clus->et() ;
    if ( clus_et < m_EtThreshold )  continue ;
    if ( fabs(clus->eta()) > m_EtaMax ) continue ;
    //
    float dR = PAUutils::deltaR(refClus,clus) ;
    if ( dR < dRbest ) {
      dRbest = dR ;
      bestClus = clus ;
      //std::cout << "topoClusterTool: dR\t" << dR << std::endl ;
    }
    for ( unsigned i=0 ; i<Ncones ; ++i ) {
      if ( dR < dRcone[i] )  EtCone[i] += clus_et ;
    }
  }
  //
  return bestClus ;
}
