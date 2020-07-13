/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetByteStreamErrorsAthenaPool/TRT_BSErrContainer_p2.h"
#include "InDetByteStreamErrors/TRT_BSErrContainer.h"
#include "TRT_BSErrContainerCnv_p2.h"

#include "Identifier/IdentifierHash.h"
#include "AthAllocators/DataPool.h"

#include <iostream>

void TRT_BSErrContainerCnv_p2::transToPers(const TRT_BSErrContainer* transCont, TRT_BSErrContainer_p2* persCont, MsgStream & /* log */) 
{
   for (const uint32_t id: transCont->getMissingErrorSet()) {
     persCont->m_missingError.push_back(id);
  }
  
  for (const uint32_t id: transCont->getErrorErrorSet()) {
    persCont->m_errorError.push_back(id);
  }

  for (const uint32_t id: transCont->getSidErrorSet()) {
    persCont->m_sidError.push_back(id);
  }
  
  return;
}

void  TRT_BSErrContainerCnv_p2::persToTrans(const TRT_BSErrContainer_p2* persCont, TRT_BSErrContainer* transCont, MsgStream & /* log */) 
{

  for (const uint32_t id : persCont->m_missingError) {
    transCont->add_missing_error(id);
  }

  for (const uint32_t id : persCont->m_errorError) {
    transCont->add_error_error(id);
  }
    
  for (const uint32_t id : persCont->m_sidError) {
    transCont->add_sid_error(id);
  }
  return;
}

//================================================================
TRT_BSErrContainer* TRT_BSErrContainerCnv_p2::createTransient(const TRT_BSErrContainer_p2* persObj, MsgStream& log) {
  std::unique_ptr<TRT_BSErrContainer> trans(std::make_unique<TRT_BSErrContainer>());
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}
