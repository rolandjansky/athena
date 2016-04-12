/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloClusterCopier.h 
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  D. Rousseau, copied from LArClusterCopier from H. Ma
CREATED:  Sep 2005
	   See header file. 

********************************************************************/

//#include "CaloUtils/CaloClusterStoreHelper.h"

#include "CaloRec/CaloClusterCopier.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

#include "AthenaKernel/errorcheck.h"


// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
CaloClusterCopier::CaloClusterCopier(const std::string& type, 
				   const std::string& name,
				   const IInterface* parent):
  AthAlgTool(type, name, parent),
  m_clustersName(""),
  m_copyCells(true),m_useClusterPosition(false),m_etCut(-1.), m_clusterSize(xAOD::CaloCluster_v1::CSize_Unknown)
{ 
  declareInterface<CaloClusterCollectionProcessor> (this);
  declareProperty("ClustersName",m_clustersName);
  declareProperty("CopyCells",m_copyCells);
  declareProperty("UseClusterPosition",m_useClusterPosition);
  declareProperty("etCut",m_etCut);
}

// -------------------------------------------------------------
// Destructor 
// -------------------------------------------------------------
CaloClusterCopier::~CaloClusterCopier()
{ }

StatusCode CaloClusterCopier::initialize() {

  msg() << MSG::INFO << " Careful: CaloClusterCopier only copy eta0 phi0" << endreq ;
  
  if (!m_copyCells){
    msg() << MSG::INFO << " Do not copy cells " << endreq ;
//    log << MSG::ERROR << " CopyCell=False : Possibility to copy cluster without cells not implemented yet. Ignored." << endreq ;
    
  } else {
    msg() << MSG::WARNING << " Copy of cluster with cells not well tested " << endreq ;
  }
  
  
  return StatusCode::SUCCESS;

}

StatusCode CaloClusterCopier::execute(xAOD::CaloClusterContainer* clusColl) {
  ATH_MSG_DEBUG("Executing CaloClusterCopier");

  const xAOD::CaloClusterContainer* inputContainer; 
  CHECK(evtStore()->retrieve(inputContainer,m_clustersName));

  xAOD::CaloClusterContainer::const_iterator it = inputContainer->begin();
  xAOD::CaloClusterContainer::const_iterator it_e = inputContainer->end();
  for(; it!=it_e;++it) {
    const xAOD::CaloCluster* old = *it; 
    if (m_etCut < 0 || std::fabs(old->et()) > m_etCut) {
      // make the cluster
      xAOD::CaloCluster* cluster_ptr = CaloClusterStoreHelper::makeCluster(clusColl,old->getCellLinks()->getCellContainer());
      if (m_useClusterPosition) {
	cluster_ptr->setEta0(old->eta());
	cluster_ptr->setPhi0(old->phi());
      } else {
	cluster_ptr->setEta0(old->eta0());
	cluster_ptr->setPhi0(old->phi0());
      }

      cluster_ptr->setClusterSize(m_clusterSize);

      if (m_copyCells){
	//Copy constructor for cell-link object??
	//CaloClusterCellLink* links=new CaloClusterCellLink(oldCellLinks->getCellContainer());
	//cluster_ptr->addCellLink(links);
	// get all the cells 
	xAOD::CaloCluster::const_cell_iterator it_cell=old->cell_begin(); 
	xAOD::CaloCluster::const_cell_iterator it_cell_e=old->cell_end(); 
	// put the cell in cluster	
	int nhit = 0; 
	for(; it_cell!=it_cell_e; ++it_cell)  {
	  cluster_ptr->addCell(it_cell.index(),1.0); 
	  ++nhit; 
	}
	//update kine!
	CaloClusterKineHelper::calculateKine(cluster_ptr,false,true); //No weight at this point! 
	ATH_MSG_DEBUG(" Number of cells = " <<nhit);
      }// end if m_copyCells
    }//end if et cut
  }//end loop over input clusters

  ATH_MSG_DEBUG("Done with copying clusters, number of clusters= " << clusColl->size());
  return StatusCode::SUCCESS;
}


void CaloClusterCopier::getClusterSize(){

  if(name()=="LArgamClusterMaker.larClusterCopier"){
    m_clusterSize = xAOD::CaloCluster::SW_55ele; 
  }else if(name()=="LAr35ClusterMaker.larClusterCopier35"){
    m_clusterSize = xAOD::CaloCluster::SW_35ele; 
  }else if(name()=="LAr37ClusterMaker.larClusterCopier37"){
    m_clusterSize = xAOD::CaloCluster::SW_37ele; 
  }else if(name()=="LArgamClusterMaker.larClusterCopiergam"){
    m_clusterSize = xAOD::CaloCluster::SW_55gam; 
  }else if(name()=="LArgam35ClusterMaker.larClusterCopiergam35"){
    m_clusterSize = xAOD::CaloCluster::SW_35gam; 
  }else if(name()=="LArgam37ClusterMaker.larClusterCopiergam37"){
    m_clusterSize = xAOD::CaloCluster::SW_37gam; 
  }else if (name()=="EMTopo35ClusterMaker.TopoEMCopier35") {
    m_clusterSize = xAOD::CaloCluster::SW_35gam;
  }

  ATH_MSG_DEBUG("Name, Cluster size = " << name() << " " << m_clusterSize);

  return;

}
//xAOD::CaloCluster_v1::
