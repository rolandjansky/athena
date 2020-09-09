/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  msg << MSG::WARNING << "CaloClusterStoreHelper::makeContainer(StoreGateSvc* pStoreGate, ...) is deprecated. Use DataHandles!" << endmsg;
  // Create the xAOD container and its auxiliary store:
  xAOD::CaloClusterContainer* clusColl = new xAOD::CaloClusterContainer();
  if (pStoreGate->overwrite(clusColl, clusCollKey).isFailure()) {
    msg << MSG::ERROR << "Failed to record xAOD::CaloClusterContainer with key" << clusCollKey <<endmsg;
    delete clusColl;
    return nullptr;
  }

  xAOD::CaloClusterAuxContainer* aux = new xAOD::CaloClusterAuxContainer();
  if(pStoreGate->overwrite(aux, clusCollKey+"Aux.").isFailure()) {
    msg << MSG::ERROR << "Failed to record xAOD::CaloClusterAuxContainer with key " << clusCollKey+"Aux." << endmsg;
    delete aux;
    delete clusColl;
    return nullptr;
  }
  clusColl->setStore( aux );
  
  return clusColl;
}

StatusCode CaloClusterStoreHelper::AddContainerWriteHandle(StoreGateSvc* /*pStoreGate*/,
                                                           SG::WriteHandle<xAOD::CaloClusterContainer> &clusColl,
                                                           MsgStream& /*msg*/) {
  // Create the xAOD container and its auxiliary store:
  StatusCode sc = 
    clusColl.record (std::make_unique<xAOD::CaloClusterContainer>(),
                     std::make_unique<xAOD::CaloClusterAuxContainer>());
  return sc;
}


StatusCode CaloClusterStoreHelper::finalizeClusters(StoreGateSvc* pStoreGate,
						    xAOD::CaloClusterContainer* pClusterColl,
						    const std::string& clusCollKey,
						    MsgStream& msg)
{

  msg << MSG::WARNING << "CaloClusterStoreHelper::finalizeClusters(StoreGateSvc* pStoreGate, ...) is deprecated. Use DataHandles!" << endmsg;
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


StatusCode
CaloClusterStoreHelper::finalizeClusters (SG::WriteHandle<CaloClusterCellLinkContainer>& h,
                                          xAOD::CaloClusterContainer* pClusterColl)
{
  auto cellLinks = std::make_unique<CaloClusterCellLinkContainer>();

  //Loop on clusters and call setLink to transfer ownership of CaloClusterCellLink object to 
  //CaloClusterCellLinkContainer
  IProxyDict* sg = SG::CurrentEventStore::store();
  for (xAOD::CaloCluster* cl : *pClusterColl) {
    cl->setLink(cellLinks.get(), sg);
  }

  return h.record (std::move (cellLinks));
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
