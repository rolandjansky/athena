/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloClusterCellLinkContainerCnv_p1.h" 
#include "AthenaKernel/getThinningCache.h"
#include "AthenaKernel/ThinningCache.h"

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
                                                        const std::string& key,
                                                        MsgStream &msg) const
{
  const SG::ThinningCache* tcache = SG::getThinningCache();
  
  const SG::ThinningDecisionBase* dec_cells = nullptr;
  const SG::ThinningDecisionBase* dec_clusts = tcache ? tcache->thinning (key) : nullptr;

  const size_t nClusters=trans->size();
  if (nClusters>0) {
    //we assume here all clusters in a container are built from the same cell container
    m_linkCnv.transToPers((*trans)[0]->getCellContainerLink(),pers->m_cellCont,msg);
    if (tcache) {
      dec_cells = SG::getThinningDecision ((*trans)[0]->getCellContainerLink().dataID());
    }
  }
 
  size_t minCapacity=0;
  pers->m_nCellsPerCluster.reserve(nClusters);
  size_t icluster = 0;
  for(const CaloClusterCellLink* cccl: *trans) {
    if (!dec_clusts || !dec_clusts->thinned (icluster)) {
      const size_t nCells=cccl->size();
      pers->m_nCellsPerCluster.push_back(nCells);
      minCapacity+=nCells;
      pers->m_indices.reserve(minCapacity);
      //pers->m_weights.reserve(minCapacity);
      CaloClusterCellLink::const_iterator it = cccl->begin();
      CaloClusterCellLink::const_iterator end = cccl->end();
      for (; it != end; ++it) {
        unsigned ndx = it.index();
        if (dec_cells) ndx = dec_cells->index (ndx);
        // Don't save weights that are exactly 1.0.
        // But the weights we save are single-precision, while the weights
        // in the cluster are double-precision.  Convert to single-precision
        // before doing the comparision with 1.0, otherwise the contents
        // of this object can change if we read and rewrite.
        // See ATLASRECTS-7129.
        float fweight = it.weight();
        if (fweight == 1.0) { //standard weight 
          pers->m_indices.push_back(ndx & INDEXBIT_MASK);
        }
        else {
          pers->m_indices.push_back((ndx & INDEXBIT_MASK) | HAS_WEIGHT_BIT);
          pers->m_weights.push_back(fweight);
        }
      }//end loop over cells in cellLink object
    }
    ++icluster;
  }//end loop over transient CaloClusterCellLinkContainer
  
  return;
}
