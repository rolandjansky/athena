/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloUtils/CaloClusterStoreHelper.h"

#include "CaloRec/CaloClusterSnapshot.h"
#include "AthenaKernel/errorcheck.h"
#include "CaloRec/CaloClusterMaker.h"

// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
CaloClusterSnapshot::CaloClusterSnapshot(const std::string& type, 
				   const std::string& name,
				   const IInterface* parent):
  AthAlgTool(type, name, parent),
  m_finalContName(nullptr)
{ 
  declareInterface<CaloClusterCollectionProcessor> (this);
  declareProperty("OutputName",m_outputName);
  declareProperty("SetCrossLinks",m_setCrossLinks=false);
  
}

// -------------------------------------------------------------
// Destructor 
// -------------------------------------------------------------
CaloClusterSnapshot::~CaloClusterSnapshot()
{ }

StatusCode CaloClusterSnapshot::initialize() {

  if (m_setCrossLinks) {
    const CaloClusterMaker* parentAlgo=dynamic_cast<const CaloClusterMaker*>(parent());
    if (!parentAlgo) {
      msg(MSG::WARNING) << "Configuration problem. Parent is not CaloClusterMaker. Can't set ElementLink to final cluster." << endreq;
    }
    m_finalContName=&(parentAlgo->getOutputContainerName());
  }
  
  return StatusCode::SUCCESS;

}


StatusCode CaloClusterSnapshot::execute(xAOD::CaloClusterContainer* clusColl) {
  ATH_MSG_DEBUG("Executing CaloClusterSnapshot");
  
  xAOD::CaloClusterContainer* outputColl=CaloClusterStoreHelper::makeContainer(&(*evtStore()),m_outputName,msg());
  CaloClusterStoreHelper::copyContainer(clusColl,outputColl);

  if (m_setCrossLinks) {
    const size_t nClusters=clusColl->size();
    if (nClusters!=outputColl->size()) {
      ATH_MSG_ERROR("After copy of container: Unequal number of entries! Original " << nClusters << ", copy " << outputColl->size());
      return StatusCode::FAILURE;
    }

    //final cluster to snapshot:
    for (size_t i=0;i<nClusters;++i) {
      (*clusColl)[i]->setSisterCluster(m_outputName,i);
    }

    //From snapshot to final cluster
    if (m_finalContName) {
       for (size_t i=0;i<nClusters;++i) {
	 (*outputColl)[i]->setSisterCluster(*m_finalContName,i);
	 //	 std::cout << "Setting link to " << *m_finalContName << ", index "<< i <<std::endl;
       }
    }
    else
      ATH_MSG_DEBUG("Can't set element link from snapshot to final cluster, see warning above");
  }

  
  CHECK(CaloClusterStoreHelper::finalizeClusters(&(*evtStore()),outputColl, m_outputName, msg()));

  
  return StatusCode::SUCCESS;
}

