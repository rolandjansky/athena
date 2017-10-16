/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloCellContainerCnv_p1.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCompactCellContainer.h"
#include "CaloCompactCellTool.h"

CaloCellContainerCnv_p1::CaloCellContainerCnv_p1 () {}

void CaloCellContainerCnv_p1::persToTrans(const CaloCompactCellContainer* pers, CaloCellContainer* trans) {
  trans->clear();
  CaloCompactCellTool compactCellTool;
  StatusCode sc = compactCellTool.getTransient(*pers, trans);
  if (sc.isFailure())  std::cout << " CaloCellContainerCnv_p1: Could not get transient" << std::endl;
}



void CaloCellContainerCnv_p1::transToPers(const CaloCellContainer* trans, CaloCompactCellContainer* pers)  {
  CaloCompactCellTool compactCellTool;
  StatusCode sc = compactCellTool.getPersistent(*trans,pers,CaloCompactCellTool::VERSION_LATEST);
  if (sc.isFailure()) std::cout << " CaloCellContainerCnv_p1: Could not get persistent" << std::endl;
}


/** Convert persistent object representation to transient
    @param pers [IN] void* pointer to the persistent object
    @param trans [OUT] void* pointer to the empty transient object
    @param log [IN] output message stream
*/
void CaloCellContainerCnv_p1::persToTransUntyped(const void* pers,
                                                  void* trans,
                                                  MsgStream& /*log*/)
{
  persToTrans (reinterpret_cast<const CaloCompactCellContainer*> (pers),
               reinterpret_cast<CaloCellContainer*> (trans));
}


/** Convert transient object representation to persistent
    @param trans [IN] void* pointer to the transient object
    @param pers [OUT] void* pointer to the empty persistent object
    @param log [IN] output message stream
*/
void CaloCellContainerCnv_p1::transToPersUntyped(const void* trans,
                                                  void* pers,
                                                  MsgStream& /*log*/)
{
  transToPers (reinterpret_cast<const CaloCellContainer*> (trans),
               reinterpret_cast<CaloCompactCellContainer*> (pers));
}


/** return C++ type id of the transient class this converter is for
    @return std::type_info&
*/
const std::type_info& CaloCellContainerCnv_p1::transientTInfo() const
{
  return typeid (CaloCellContainer);
}

/** return C++ type id of the persistent class this converter is for
    @return std::type_info&
*/
const std::type_info& CaloCellContainerCnv_p1::persistentTInfo() const
{
  return typeid (CaloCompactCellContainer);
}

