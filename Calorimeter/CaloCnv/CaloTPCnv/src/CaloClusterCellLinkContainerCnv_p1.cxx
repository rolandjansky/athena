/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloClusterCellLinkContainerCnv_p1.h" 

namespace {

  const unsigned INDEXBITS=18;
  const unsigned INDEXBIT_MASK = (0x1 << INDEXBITS) -1;
  const unsigned HAS_WEIGHT_BIT=0x1 << INDEXBITS;

} //anonymous namespace


void  CaloClusterCellLinkContainerCnv_p1::persToTrans(const CaloClusterCellLinkContainer_p1* pers,
						      CaloClusterCellLinkContainer* trans,
						      MsgStream& msg) {

  const size_t nClusters=pers->m_nCellsPerCluster.size();
  size_t maxPersIdx=pers->m_indices.size();

  trans->reserve(nClusters);
  size_t persIdx=0;
  size_t weightIdx=0;
  for (size_t iCluster=0;iCluster<nClusters;++iCluster) {
    CaloClusterCellLink* cccl=new CaloClusterCellLink();
    trans->push_back(cccl);
    m_linkCnv.persToTrans(pers->m_cellCont,cccl->m_cellCont,msg);
    size_t nextStep=persIdx+pers->m_nCellsPerCluster[iCluster];
    if (nextStep > maxPersIdx) {
      msg << MSG::ERROR << "Inconsistent persistent object: To few persistent values, expected at least " << nextStep
	  << " got only "<< maxPersIdx  << endmsg;
      nextStep=maxPersIdx;
    }
    for (;persIdx<nextStep;++persIdx) {
      const unsigned index=(pers->m_indices[persIdx] & INDEXBIT_MASK);
      if (pers->m_indices[persIdx] & HAS_WEIGHT_BIT) {
	cccl->m_indicesAndWeights.emplace_back(index, pers->m_weights[weightIdx++]);
      }
      else {
	cccl->m_indicesAndWeights.emplace_back(index,1.0);
      }
    }//end loop over cells in cluster
  }//end loop over clusters
}


void  CaloClusterCellLinkContainerCnv_p1::transToPers(const CaloClusterCellLinkContainer* trans,
						      CaloClusterCellLinkContainer_p1* pers,
						      MsgStream &msg) {
  const size_t nClusters=trans->size();
  if (nClusters>0) {
    //we assume here all clusters in a container are built from the same cell container
    m_linkCnv.transToPers((*trans)[0]->m_cellCont,pers->m_cellCont,msg);
  }
 
  size_t minCapacity=0;
  pers->m_nCellsPerCluster.reserve(nClusters);
  for(const CaloClusterCellLink* cccl: *trans) {
    const size_t nCells=cccl->size();
    pers->m_nCellsPerCluster.push_back(nCells);
    minCapacity+=nCells;
    pers->m_indices.reserve(minCapacity);
    //pers->m_weights.reserve(minCapacity);
    for (const auto& weightIndex : cccl->m_indicesAndWeights) {
      if (weightIndex.second == 1.0) { //standard weight 
	pers->m_indices.push_back(weightIndex.first & INDEXBIT_MASK);
      }
      else {
	pers->m_indices.push_back((weightIndex.first & INDEXBIT_MASK) | HAS_WEIGHT_BIT);
	pers->m_weights.push_back(weightIndex.second);
      }
    }//end loop over cells in cellLink object
  }//end loop over transient CaloClusterCellLinkContainer
  
  return;
}


void CaloClusterCellLinkContainerCnv_p1::persToTransUntyped(const void* pers,
                                                     void* trans,
                                                     MsgStream& log)
{
  persToTrans (reinterpret_cast<const CaloClusterCellLinkContainer_p1*> (pers),
               reinterpret_cast<CaloClusterCellLinkContainer*> (trans),
               log);
}


/**
 * @brief Convert from transient to persistent object, with untyped pointers.
 * @param trans The transient object to convert.
 * @param pers The persistent object to which to convert.
 * @param log Error logging stream.
 */
void CaloClusterCellLinkContainerCnv_p1::transToPersUntyped(const void* trans,
							    void* pers,
							    MsgStream& log)
{
  transToPers (reinterpret_cast<const CaloClusterCellLinkContainer*> (trans),
               reinterpret_cast<CaloClusterCellLinkContainer_p1*> (pers),
               log);
}


/**
 * @brief Return the @c std::type_info for the transient type.
 */
const std::type_info& CaloClusterCellLinkContainerCnv_p1::transientTInfo() const
{
  return typeid (CaloClusterCellLinkContainer);
}

/** return C++ type id of the persistent class this converter is for
    @return std::type_info&
*/
const std::type_info& CaloClusterCellLinkContainerCnv_p1::persistentTInfo() const
{
  return typeid (CaloClusterCellLinkContainer_p1);
}


