/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "InDetConditionsAthenaPool/TRT_BSErrContainer_p1.h"
#include "InDetByteStreamErrors/TRT_BSErrContainer.h"
#undef private
#undef protected
#include "TRT_BSErrContainerCnv_p1.h"

#include "Identifier/IdentifierHash.h"
#include "DataModel/DataPool.h"

#include <iostream>

void TRT_BSErrContainerCnv_p1::transToPers(const TRT_BSErrContainer* transCont, TRT_BSErrContainer_p1* persCont, MsgStream & /* log */) 
{
  std::vector<std::pair<uint8_t, uint32_t>* >::const_iterator it = transCont->begin();
  std::vector<std::pair<uint8_t, uint32_t>* >::const_iterator itEnd = transCont->end();
  (persCont->m_bsErrs).reserve(transCont->size());

  for (; it != itEnd; ++it) {
    (persCont->m_bsErrs).push_back(*it);
  }
  return;
}

void  TRT_BSErrContainerCnv_p1::persToTrans(const TRT_BSErrContainer_p1* persCont, TRT_BSErrContainer* transCont, MsgStream & /* log */) 
{

  std::vector<std::pair<uint8_t, uint32_t>* >::const_iterator it = (persCont->m_bsErrs).begin();
  std::vector<std::pair<uint8_t, uint32_t>* >::const_iterator itEnd = (persCont->m_bsErrs).end();
  transCont->reserve((persCont->m_bsErrs).size());

  for (; it != itEnd; ++it) {
    transCont->push_back(*it);
  }
  return;

}

//================================================================
TRT_BSErrContainer* TRT_BSErrContainerCnv_p1::createTransient(const TRT_BSErrContainer_p1* persObj, MsgStream& log) {
  std::auto_ptr<TRT_BSErrContainer> trans(new TRT_BSErrContainer());
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}
