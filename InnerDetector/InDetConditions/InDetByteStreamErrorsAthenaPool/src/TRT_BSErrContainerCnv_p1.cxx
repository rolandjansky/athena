/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetByteStreamErrorsAthenaPool/TRT_BSErrContainer_p1.h"
#include "InDetByteStreamErrors/TRT_BSErrContainer.h"
#include "TRT_BSErrContainerCnv_p1.h"

#include "Identifier/IdentifierHash.h"
#include "AthAllocators/DataPool.h"

#include <iostream>

void TRT_BSErrContainerCnv_p1::transToPers(const TRT_BSErrContainer*, TRT_BSErrContainer_p1* , MsgStream & /* log */) 
{
  throw std::runtime_error("Obsolete TRT_BSErrContainerCnv_p1::transToPers() not implemented");
  return; 
}

void  TRT_BSErrContainerCnv_p1::persToTrans(const TRT_BSErrContainer_p1* , TRT_BSErrContainer* , MsgStream & /* log */) 
{
  throw std::runtime_error("Obsolete TRT_BSErrContainerCnv_p1::persToTrans() not implemented");
  return;

}

//================================================================
TRT_BSErrContainer* TRT_BSErrContainerCnv_p1::createTransient(const TRT_BSErrContainer_p1* persObj, MsgStream& log) {
  std::unique_ptr<TRT_BSErrContainer> trans(std::make_unique<TRT_BSErrContainer>());
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}
