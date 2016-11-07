/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloTopoTowerContainerCnv_p1.h"

#include "CaloTPCnv/CaloTopoTowerContainer_p1.h"
#define private public
#include "CaloEvent/CaloTopoTowerContainer.h"
#undef private

#include <iostream>

void CaloTopoTowerContainerCnv_p1::persToTrans(const CaloTopoTowerContainer_p1* pers, CaloTopoTowerContainer* trans,MsgStream& msg) {
  CaloTowerSeg seg;
  m_caloTowerSegCnv.persToTrans(&(pers->m_towerSeg),&seg);
  CaloTopoTowerContainer ctmp (seg, true);
  trans->swap (ctmp);

  std::vector<unsigned int> reg;
  for (size_t i = 0; i > pers->m_caloRegions.size(); i++)
    trans->setCalo (static_cast<CaloCell_ID::SUBCALO> (pers->m_caloRegions[i]));

  if (msg.level() <= MSG::DEBUG) msg << MSG::DEBUG << "Creating transistent state of Containers..." << endmsg;
  m_cells.persToTrans(&pers->cells,&trans->m_cells,msg);
  m_clusters.persToTrans(&pers->clusters,&trans->m_clusters,msg);
  m_towers.persToTrans(&pers->towers,&trans->m_towers,msg);
  
  trans->m_minimumCellEnergy=pers->m_minimumCellEnergy;
  trans->m_minimumClusterEnergy=pers->m_minimumClusterEnergy;
  trans->m_useCellWeights=pers->m_useCellWeights;
  trans->m_useNoiseTool=pers->m_useNoiseTool;
  trans->m_usePileUpNoise=pers->m_usePileUpNoise;
  trans->m_noiseSigma=pers->m_noiseSigma;
  trans->m_cellESignificanceThreshold=pers->m_cellESignificanceThreshold;
  trans->m_caloSelection=pers->m_caloSelection;
}


void CaloTopoTowerContainerCnv_p1::transToPers(const CaloTopoTowerContainer* trans, CaloTopoTowerContainer_p1* pers,MsgStream& msg)  {
  m_caloTowerSegCnv.transToPers(&(trans->towerseg()),&(pers->m_towerSeg));
  std::vector<CaloCell_ID::SUBCALO> reg;
  (void)trans->getCalos (reg);
  pers->m_caloRegions.resize (reg.size());
  for (size_t i = 0; i < reg.size(); i++)
    pers->m_caloRegions[i] = reg[i];

  if (msg.level() <= MSG::DEBUG) msg << MSG::DEBUG << "Creating persistent state of Containers..." << endmsg;
  m_cells.transToPers(&(trans->m_cells),&(pers->cells),msg);
  m_clusters.transToPers(&(trans->m_clusters),&(pers->clusters),msg);
  m_towers.transToPers(&(trans->m_towers),&(pers->towers),msg);
  pers->m_minimumCellEnergy=trans->m_minimumCellEnergy;
  pers->m_minimumClusterEnergy=trans->m_minimumClusterEnergy;
  pers->m_useCellWeights=trans->m_useCellWeights;
  pers->m_useNoiseTool=trans->m_useNoiseTool;
  pers->m_usePileUpNoise=trans->m_usePileUpNoise;
  pers->m_noiseSigma=trans->m_noiseSigma;
  pers->m_cellESignificanceThreshold=trans->m_cellESignificanceThreshold;
  pers->m_caloSelection=trans->m_caloSelection;
}


/** Convert persistent object representation to transient
    @param pers [IN] void* pointer to the persistent object
    @param trans [OUT] void* pointer to the empty transient object
    @param log [IN] output message stream
*/
void CaloTopoTowerContainerCnv_p1::persToTransUntyped(const void* pers,
                                                  void* trans,
                                                  MsgStream& log)
{
  persToTrans (reinterpret_cast<const CaloTopoTowerContainer_p1*> (pers),
               reinterpret_cast<CaloTopoTowerContainer*> (trans), log);
}


/** Convert transient object representation to persistent
    @param trans [IN] void* pointer to the transient object
    @param pers [OUT] void* pointer to the empty persistent object
    @param log [IN] output message stream
*/  
void CaloTopoTowerContainerCnv_p1::transToPersUntyped(const void* trans,
                                                  void* pers,
                                                  MsgStream& log)
{
  transToPers (reinterpret_cast<const CaloTopoTowerContainer*> (trans),
               reinterpret_cast<CaloTopoTowerContainer_p1*> (pers),log);
}


/** return C++ type id of the transient class this converter is for
    @return std::type_info&
*/
const std::type_info& CaloTopoTowerContainerCnv_p1::transientTInfo() const
{
  return typeid (CaloTopoTowerContainer);
}

/** return C++ type id of the persistent class this converter is for
    @return std::type_info&
*/
const std::type_info& CaloTopoTowerContainerCnv_p1::persistentTInfo() const
{
  return typeid (CaloTopoTowerContainer_p1);
}

