#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/PFClusterSelector.h"
#include "xAODCaloEvent/CaloCluster.h"

PFClusterSelector::PFClusterSelector(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_caloClustersReadHandle("CaloTopoCluster"),
  m_caloCalClustersReadHandle("CaloCalTopoCluster"),
  m_eflowRecClustersWriteHandle("eflowRecClusters")
{
  declareProperty("clustersName",m_caloClustersReadHandle );
  declareProperty("calClustersName",m_caloCalClustersReadHandle  );
  declareProperty("eflowRecClustersOutputName", m_eflowRecClustersWriteHandle);
}

StatusCode PFClusterSelector::initialize(){

  ATH_CHECK(m_caloClustersReadHandle.initialize());
  ATH_CHECK(m_caloCalClustersReadHandle.initialize());

  ATH_CHECK(m_eflowRecClustersWriteHandle.initialize());

  return StatusCode::SUCCESS;
}

StatusCode PFClusterSelector::execute(){

  /* Verify the read handle has a valid pointer, and if not return */
  if (!m_caloClustersReadHandle.isValid()){
    msg(MSG::WARNING) << " Invalid ReadHandle for xAOD::CaloCluster with key: " <<  m_caloClustersReadHandle.key()  << endmsg;
    return StatusCode::SUCCESS;
  }
  /* Record the eflowRecCluster output container */
  ATH_CHECK(m_eflowRecClustersWriteHandle.record(std::make_unique<eflowRecClusterContainer>()));

  /* Fill the vector of eflowRecClusters */
  unsigned int nClusters = m_caloClustersReadHandle->size();
  for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster) {
    /* Create the eflowRecCluster and put it in the container */
    std::unique_ptr<eflowRecCluster> thisEFRecCluster  = std::make_unique<eflowRecCluster>(ElementLink<xAOD::CaloClusterContainer>(*m_caloClustersReadHandle, iCluster));
    
    if (m_caloCalClustersReadHandle.isValid()){
      std::map<IdentifierHash,double> cellsWeightMap;
      retrieveLCCalCellWeight(m_caloClustersReadHandle->at(iCluster)->e(), iCluster, cellsWeightMap);

      if (msgLvl(MSG::DEBUG)) {
        //zhangr
        std::map<IdentifierHash, double>::iterator it = cellsWeightMap.begin();
        for (; it != cellsWeightMap.end(); ++it) {
           msg(MSG::DEBUG) << "zhangrui eflowPreparation " << iCluster << "/" << nClusters << ": e="
                    << m_caloClustersReadHandle->at(iCluster)->e() << " (" << it->first << "  "
                    << it->second << ")" << endmsg;
        }
      }

      thisEFRecCluster->setCellsWeight(cellsWeightMap);
    }

    thisEFRecCluster->setClusterId(iCluster);
    m_eflowRecClustersWriteHandle->push_back(std::move(thisEFRecCluster));

    if (msgLvl(MSG::DEBUG)) {
      const xAOD::CaloCluster* thisCluster = m_caloClustersReadHandle->at(iCluster);
      msg(MSG::DEBUG) << "eflowPreparation clus = " << thisCluster->eta() << " "
		      << thisCluster->phi() << " " << thisCluster->e()/cosh(thisCluster->eta()) << " " << endmsg;
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode PFClusterSelector::finalize(){
  return StatusCode::SUCCESS;
}

void PFClusterSelector::retrieveLCCalCellWeight(const double& energy, const unsigned& index, std::map<IdentifierHash,double>& cellsWeight) {
  
  /* match CaloCluster with CaloCalCluster to obtain cell weight */
  /* first try the position at 'index'. If we are lucky, the loop can be avoided. */
  /* Note the read handle has been tested to be valid prior to the call of this function */
  const xAOD::CaloCluster* matchedCalCluster = m_caloCalClustersReadHandle->at(index);
  if (!(fabs(energy - matchedCalCluster->rawE()) < 0.001)) {
    matchedCalCluster = nullptr;
    for (unsigned iCalCalCluster = 0; iCalCalCluster < m_caloCalClustersReadHandle->size();
        ++iCalCalCluster) {
      matchedCalCluster = m_caloCalClustersReadHandle->at(iCalCalCluster);
      if (fabs(energy - matchedCalCluster->rawE()) < 0.001) {
        break;
      }
    }
  }
  assert (matchedCalCluster);

  /* obtain cell index and cell weight */
  const CaloDetDescrManager*   calo_dd_man  = CaloDetDescrManager::instance();
  const CaloCell_ID*               calo_id  = calo_dd_man->getCaloCell_ID();
  xAOD::CaloCluster::const_cell_iterator itCell = matchedCalCluster->cell_begin();
  xAOD::CaloCluster::const_cell_iterator endCell = matchedCalCluster->cell_end();
  for (; itCell != endCell; ++itCell) {
    const CaloCell* pCell = *itCell;
    Identifier myId = pCell->ID();
    IdentifierHash myHashId = calo_id->calo_cell_hash(myId);
    cellsWeight[myHashId] = itCell.weight();
  }

  return ;
}
