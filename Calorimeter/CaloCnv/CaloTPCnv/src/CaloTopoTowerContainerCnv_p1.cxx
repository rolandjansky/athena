/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloTopoTowerContainerCnv_p1.h"

#include "CaloTPCnv/CaloTopoTowerContainer_p1.h"
#include "CaloEvent/CaloTopoTowerContainer.h"

#include <iostream>

void CaloTopoTowerContainerCnv_p1::persToTrans (const CaloTopoTowerContainer_p1* pers,
                                                CaloTopoTowerContainer* trans,
                                                MsgStream& msg) const
{
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
  trans->m_noiseSigma=pers->m_noiseSigma;
  trans->m_cellESignificanceThreshold=pers->m_cellESignificanceThreshold;
  trans->m_caloSelection=pers->m_caloSelection;
}


void CaloTopoTowerContainerCnv_p1::transToPers (const CaloTopoTowerContainer* trans,
                                                CaloTopoTowerContainer_p1* pers,
                                                MsgStream& msg) const
{
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
  pers->m_noiseSigma=trans->m_noiseSigma;
  pers->m_cellESignificanceThreshold=trans->m_cellESignificanceThreshold;
  pers->m_caloSelection=trans->m_caloSelection;
}
