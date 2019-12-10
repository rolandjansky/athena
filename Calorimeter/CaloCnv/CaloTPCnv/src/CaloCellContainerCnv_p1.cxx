/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloCellContainerCnv_p1.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCompactCellContainer.h"
#include "CaloCompactCellTool.h"
#include "AthenaKernel/getThinningCache.h"


CaloCellContainerCnv_p1::CaloCellContainerCnv_p1 () {}


void CaloCellContainerCnv_p1::persToTransWithKey(const CaloCompactCellContainer* pers,
                                                 CaloCellContainer* trans,
                                                 const std::string& /*key*/,
                                                 MsgStream& log) const
{
  trans->clear();
  CaloCompactCellTool compactCellTool;
  if (compactCellTool.getTransient(*pers, trans).isFailure()) {
    log << MSG::ERROR << " CaloCellContainerCnv_p1: Could not get transient" << endmsg;
  }
}


void CaloCellContainerCnv_p1::transToPersWithKey(const CaloCellContainer* trans,
                                                 CaloCompactCellContainer* pers,
                                                 const std::string& key,
                                                 MsgStream& log) const
{
  CaloCompactCellTool compactCellTool;

  if (compactCellTool.getPersistent (*trans,
                                     pers,
                                     SG::getThinningDecision (key),
                                     CaloCompactCellTool::VERSION_LATEST).isFailure()) {
    log << MSG::ERROR << " CaloCellContainerCnv_p1: Could not get persistent" << endmsg;
  }
}
