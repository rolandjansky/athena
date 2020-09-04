/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "IDCInDetBSErrContainerCnv_p1.h"
#include <algorithm>    // std::min_element, std::max_element

void IDCInDetBSErrContainerCnv_p1::transToPers(const IDCInDetBSErrContainer* transCont, InDetBSErrContainer64_p1* persCont, MsgStream&)
{
  const auto vect = transCont->getAll();
  persCont->m_bsErrs.reserve(vect.size());
  for(const auto& v : vect) persCont->m_bsErrs.emplace_back(v.first, v.second);
  return;
}

void  IDCInDetBSErrContainerCnv_p1::persToTrans(const InDetBSErrContainer64_p1* persCont, IDCInDetBSErrContainer* transCont, MsgStream& /*log*/)
{
  for (const std::pair<IdentifierHash, uint64_t>& bsErr: persCont->m_bsErrs) {
    transCont->setOrDrop(bsErr.first, bsErr.second);
  }
  return;
}

//================================================================
IDCInDetBSErrContainer* IDCInDetBSErrContainerCnv_p1::createTransient(const InDetBSErrContainer64_p1* persObj, MsgStream& log) {
  size_t g = 0;
  if (persObj->m_bsErrs.begin()!=persObj->m_bsErrs.end()) {
    auto maxhash = std::max_element(persObj->m_bsErrs.begin(), persObj->m_bsErrs.end());
    g = maxhash->first + 1;
  }
  std::unique_ptr<IDCInDetBSErrContainer> trans = std::make_unique<IDCInDetBSErrContainer>(g, std::numeric_limits<IDCInDetBSErrContainer::ErrorCode>::min());
  persToTrans(persObj, trans.get(), log);
  return trans.release();
}

