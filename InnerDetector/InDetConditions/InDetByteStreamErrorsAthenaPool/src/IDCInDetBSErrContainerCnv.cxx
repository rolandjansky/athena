/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "IDCInDetBSErrContainerCnv.h"
#include <algorithm>    // std::min_element, std::max_element

void IDCInDetBSErrContainerCnv::transToPers(const IDCInDetBSErrContainer* transCont, InDetBSErrContainer_p2* persCont, MsgStream&)
{
  const auto vect = transCont->getAll();
  persCont->m_bsErrs.reserve(vect.size());
  for(const auto& v : vect) persCont->m_bsErrs.emplace_back(v.first, v.second);
  return;
}

void  IDCInDetBSErrContainerCnv::persToTrans(const InDetBSErrContainer_p2* persCont, IDCInDetBSErrContainer* transCont, MsgStream& /*log*/)
{
  for (const std::pair<IdentifierHash, int>& bsErr: persCont->m_bsErrs) {
    transCont->setOrDrop(bsErr.first, bsErr.second);
  }
  return;
}

//================================================================
IDCInDetBSErrContainer* IDCInDetBSErrContainerCnv::createTransient(const InDetBSErrContainer_p2* persObj, MsgStream& log) {
  auto maxhash = std::max_element(persObj->m_bsErrs.begin(), persObj->m_bsErrs.end());
  size_t g = maxhash->first;
  std::unique_ptr<IDCInDetBSErrContainer> trans = std::make_unique<IDCInDetBSErrContainer>(g + 1, std::numeric_limits<int>::min());
  persToTrans(persObj, trans.get(), log);
  return trans.release();
}

