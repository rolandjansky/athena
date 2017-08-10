/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetConditionsAthenaPool/TRT_BSIdErrContainer_p1.h"
#include "InDetByteStreamErrors/TRT_BSIdErrContainer.h"
#include "TRT_BSIdErrContainerCnv_p1.h"

#include "Identifier/IdentifierHash.h"
#include "AthAllocators/DataPool.h"

#include <iostream>

void TRT_BSIdErrContainerCnv_p1::transToPers(const TRT_BSIdErrContainer* transCont, TRT_BSIdErrContainer_p1* persCont, MsgStream & /* log */) 
{
  TRT_BSIdErrContainer::const_iterator it = transCont->begin();
  TRT_BSIdErrContainer::const_iterator itEnd = transCont->end();
  (persCont->m_bsErrs).reserve(transCont->size());

  for (; it != itEnd; ++it) {
    // FIXME: Should change type of m_bsErrs, but don't want to cause possible
    // back-compatibility problems.
    std::pair<uint8_t, std::pair<uint32_t, uint8_t> >* ptr = const_cast<std::pair<uint8_t, std::pair<uint32_t, uint8_t> >*> (*it);
    (persCont->m_bsErrs).push_back(ptr);
  }
  return;
}

void  TRT_BSIdErrContainerCnv_p1::persToTrans(const TRT_BSIdErrContainer_p1* persCont, TRT_BSIdErrContainer* transCont, MsgStream & /* log */) 
{

  std::vector<std::pair<uint8_t, std::pair<uint32_t, uint8_t> >* >::const_iterator it = (persCont->m_bsErrs).begin();
  std::vector<std::pair<uint8_t, std::pair<uint32_t, uint8_t> >* >::const_iterator itEnd = (persCont->m_bsErrs).end();
  transCont->reserve((persCont->m_bsErrs).size());

  for (; it != itEnd; ++it) {
    transCont->push_back(*it);
  }
  return;

}

//================================================================
TRT_BSIdErrContainer* TRT_BSIdErrContainerCnv_p1::createTransient(const TRT_BSIdErrContainer_p1* persObj, MsgStream& log) {
  std::auto_ptr<TRT_BSIdErrContainer> trans(new TRT_BSIdErrContainer());
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}
