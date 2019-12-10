/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloClusterCellLinkContainerCnv_p1.h" 

namespace {

  const unsigned INDEXBITS=18;
  const unsigned INDEXBIT_MASK = (0x1 << INDEXBITS) -1;
  const unsigned HAS_WEIGHT_BIT=0x1 << INDEXBITS;

} //anonymous namespace


void
CaloClusterCellLinkContainerCnv_p1::persToTransWithKey (const CaloClusterCellLinkContainer_p1* pers,
                                                        CaloClusterCellLinkContainer* trans,
                                                        const std::string& /*key*/,
                                                        MsgStream& msg) const
{
  const size_t nClusters=pers->m_nCellsPerCluster.size();
  size_t maxPersIdx=pers->m_indices.size();

  trans->reserve(nClusters);
  size_t persIdx=0;
  size_t weightIdx=0;
  for (size_t iCluster=0;iCluster<nClusters;++iCluster) {
    DataLink<CaloCellContainer> link;
    m_linkCnv.persToTrans(pers->m_cellCont,link,msg);
    CaloClusterCellLink* cccl=new CaloClusterCellLink(link);
    trans->push_back(cccl);
    size_t nextStep=persIdx+pers->m_nCellsPerCluster[iCluster];
    if (nextStep > maxPersIdx) {
      msg << MSG::ERROR << "Inconsistent persistent object: To few persistent values, expected at least " << nextStep
	  << " got only "<< maxPersIdx  << endmsg;
      nextStep=maxPersIdx;
    }
    cccl->reserve(nextStep-persIdx);
    for (;persIdx<nextStep;++persIdx) {
      const unsigned index=(pers->m_indices[persIdx] & INDEXBIT_MASK);
      if (pers->m_indices[persIdx] & HAS_WEIGHT_BIT) {
	cccl->addCell(index, pers->m_weights[weightIdx++]);
      }
      else {
	cccl->addCell(index,1.0);
      }
    }//end loop over cells in cluster
  }//end loop over clusters
}


void
CaloClusterCellLinkContainerCnv_p1::transToPersWithKey (const CaloClusterCellLinkContainer* trans,
                                                        CaloClusterCellLinkContainer_p1* pers,
                                                        const std::string& /*key*/,
                                                        MsgStream &msg) const
{
  const size_t nClusters=trans->size();
  if (nClusters>0) {
    //we assume here all clusters in a container are built from the same cell container
    m_linkCnv.transToPers((*trans)[0]->getCellContainerLink(),pers->m_cellCont,msg);
  }
 
  size_t minCapacity=0;
  pers->m_nCellsPerCluster.reserve(nClusters);
  for(const CaloClusterCellLink* cccl: *trans) {
    const size_t nCells=cccl->size();
    pers->m_nCellsPerCluster.push_back(nCells);
    minCapacity+=nCells;
    pers->m_indices.reserve(minCapacity);
    //pers->m_weights.reserve(minCapacity);
    CaloClusterCellLink::const_iterator it = cccl->begin();
    CaloClusterCellLink::const_iterator end = cccl->end();
    for (; it != end; ++it) {
      if (it.weight() == 1.0) { //standard weight 
	pers->m_indices.push_back(it.index() & INDEXBIT_MASK);
      }
      else {
	pers->m_indices.push_back((it.index() & INDEXBIT_MASK) | HAS_WEIGHT_BIT);
	pers->m_weights.push_back(it.weight());
      }
    }//end loop over cells in cellLink object
  }//end loop over transient CaloClusterCellLinkContainer
  
  return;
}
