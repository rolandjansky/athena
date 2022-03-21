/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloEvent/CaloClusterCellLink.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/CurrentEventStore.h"
#include "AthenaKernel/errorcheck.h"


std::unique_ptr<xAOD::CaloCluster> CaloClusterStoreHelper::makeCluster(const CaloCellContainer* cellCont) {
  std::unique_ptr<xAOD::CaloCluster> cluster = std::make_unique<xAOD::CaloCluster>();
  cluster->makePrivateStore();
  if (cellCont) cluster->addCellLink(std::make_unique<CaloClusterCellLink>(cellCont));
  return cluster;
}
 

std::unique_ptr<xAOD::CaloCluster> CaloClusterStoreHelper::makeCluster(const CaloCellContainer* cellCont,
								       const double eta0, const double phi0,
								       const xAOD::CaloCluster_v1::ClusterSize clusterSize) {
  std::unique_ptr<xAOD::CaloCluster> cluster=CaloClusterStoreHelper::makeCluster(cellCont);
  cluster->setEta0(eta0);
  cluster->setPhi0(phi0);
  cluster->setClusterSize(clusterSize);
  return cluster;
}


xAOD::CaloCluster* CaloClusterStoreHelper::makeCluster(xAOD::CaloClusterContainer* cont, 
						       const CaloCellContainer* cellCont) {

  xAOD::CaloCluster* cluster=cont->push_back(std::make_unique<xAOD::CaloCluster>());
  if (cellCont) cluster->addCellLink(std::make_unique<CaloClusterCellLink>(cellCont));
  return cluster;
}


StatusCode CaloClusterStoreHelper::AddContainerWriteHandle(SG::WriteHandle<xAOD::CaloClusterContainer> &clusColl) {
                                                           
  // Create the xAOD container and its auxiliary store:
  StatusCode sc = 
    clusColl.record (std::make_unique<xAOD::CaloClusterContainer>(),
                     std::make_unique<xAOD::CaloClusterAuxContainer>());
  return sc;
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
    xAOD::CaloCluster* newClu=newColl->push_back(std::make_unique<xAOD::CaloCluster>());
    *newClu=*oldCluster;
    //new xAOD::CaloCluster (*oldCluster)); //Copy c'tor creates a private AuxStore and a private ClusterCellLink obj
  }
  return;
}
