/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloEvent/CaloClusterCellLink.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/CurrentEventStore.h"
#include "AthenaKernel/errorcheck.h"


xAOD::CaloCluster* CaloClusterStoreHelper::makeCluster(const CaloCellContainer* cellCont) {
  xAOD::CaloCluster* cluster=new xAOD::CaloCluster();
  cluster->makePrivateStore();
  if (cellCont) cluster->addCellLink(new CaloClusterCellLink(cellCont));
  return cluster;
}
 

xAOD::CaloCluster* CaloClusterStoreHelper::makeCluster(const CaloCellContainer* cellCont,
						       const double eta0, const double phi0,
						       const xAOD::CaloCluster_v1::ClusterSize clusterSize) {
  xAOD::CaloCluster* cluster=CaloClusterStoreHelper::makeCluster(cellCont);
  cluster->setEta0(eta0);
  cluster->setPhi0(phi0);
  cluster->setClusterSize(clusterSize);
  return cluster;
}


xAOD::CaloCluster* CaloClusterStoreHelper::makeCluster(xAOD::CaloClusterContainer* cont, 
						       const CaloCellContainer* cellCont) {
  xAOD::CaloCluster* cluster=new xAOD::CaloCluster();
  cont->push_back(cluster);
  if (cellCont) cluster->addCellLink(new CaloClusterCellLink(cellCont));
  return cluster;
}

xAOD::CaloClusterContainer* CaloClusterStoreHelper::makeContainer(StoreGateSvc* pStoreGate,
								  const std::string& clusCollKey,
								  MsgStream& msg) {
  // Create the xAOD container and its auxiliary store:
  xAOD::CaloClusterContainer* clusColl = new xAOD::CaloClusterContainer();
  if (pStoreGate->overwrite(clusColl, clusCollKey).isFailure()) {
    msg << MSG::ERROR << "Failed to record xAOD::CaloClusterContainer with key" << clusCollKey <<endmsg;
    delete clusColl;
    return NULL;
  }

  xAOD::CaloClusterAuxContainer* aux = new xAOD::CaloClusterAuxContainer();
  if(pStoreGate->overwrite(aux, clusCollKey+"Aux.").isFailure()) {
    msg << MSG::ERROR << "Failed to record xAOD::CaloClusterAuxContainer with key " << clusCollKey+"Aux." << endmsg;
    delete aux;
    delete clusColl;
    return NULL;
  }
  clusColl->setStore( aux );
  
  return clusColl;
}

StatusCode CaloClusterStoreHelper::AddContainerWriteHandle(StoreGateSvc* pStoreGate,
								  SG::WriteHandle<xAOD::CaloClusterContainer> &clusColl,
								  MsgStream& msg) {
  // Create the xAOD container and its auxiliary store:
  clusColl = std::unique_ptr<xAOD::CaloClusterContainer>(new xAOD::CaloClusterContainer());
  if (!clusColl.isValid()) return StatusCode::FAILURE;
  xAOD::CaloClusterAuxContainer* aux = new xAOD::CaloClusterAuxContainer();
  if(pStoreGate->overwrite(aux, clusColl.name()+"Aux.").isFailure()) {
    msg << MSG::ERROR << "Failed to record xAOD::CaloClusterAuxContainer with key " << clusColl.name()+"Aux." << endmsg;
    delete aux;
    return StatusCode::FAILURE;
  }
  clusColl->setStore( aux );     
  return StatusCode::SUCCESS;

}


StatusCode CaloClusterStoreHelper::finalizeClusters(StoreGateSvc* pStoreGate,
						    xAOD::CaloClusterContainer* pClusterColl,
						    const std::string& clusCollKey,
						    MsgStream& msg) {
  //Check if object is already locked:
  SG::DataProxy* proxy=pStoreGate->proxy(pClusterColl);
  //Could also check CLID and name at this point ....
  if (!proxy) {
    msg << MSG::ERROR << "Can't get DataProxy for object at address " << pClusterColl << endmsg;    
  }
  else {
    if (proxy->isConst()) {
      msg << MSG::ERROR << "Object is already locked! Do nothing." << endmsg;
      return StatusCode::FAILURE;
    }
  }


  CaloClusterCellLinkContainer* cellLinks= new CaloClusterCellLinkContainer();
  if(pStoreGate->overwrite(cellLinks, clusCollKey + "_links").isFailure()) {
    msg << MSG::ERROR << "Failed to record CaloClusterCellLinkContainer with key " << clusCollKey + "Links" << endmsg;
    return StatusCode::FAILURE;
  }

  //Loop on clusters and call setLink to transfer ownership of CaloClusterCellLink object to 
  //CaloClusterCellLinkContainer
  IProxyDict* sg = SG::CurrentEventStore::store();
  for (xAOD::CaloCluster* cl : *pClusterColl) {
    cl->setLink(cellLinks, sg);
  }


  if (pStoreGate->setConst(pClusterColl).isFailure()) {
    msg << MSG::ERROR << "Failed to lock CaloClusterContainer" << endmsg;
  }
  if (pStoreGate->setConst(cellLinks).isFailure())  {
    msg << MSG::ERROR << "Failed to lock CaloClusterCellLinkContainer" << endmsg;
  }
  return StatusCode::SUCCESS;
}


 //Moved to here from CaloRunClusterCorrection
void CaloClusterStoreHelper::copyContainer (const xAOD::CaloClusterContainer* oldColl, xAOD::CaloClusterContainer* newColl) {
  // make a Cluster Container 
  newColl->clear();
  newColl->reserve (oldColl->size());
  for (const xAOD::CaloCluster* oldCluster : *oldColl) { 
    xAOD::CaloCluster* newClu=new xAOD::CaloCluster();
    newColl->push_back (newClu);
    *newClu=*oldCluster;
    //new xAOD::CaloCluster (*oldCluster)); //Copy c'tor creates a private AuxStore and a private ClusterCellLink obj
  }
  return;
}
