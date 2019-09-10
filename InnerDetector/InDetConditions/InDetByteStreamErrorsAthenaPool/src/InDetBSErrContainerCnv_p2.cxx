/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetBSErrContainerCnv_p2.h"

void InDetBSErrContainerCnv_p2::transToPers(const InDetBSErrContainer* transCont, InDetBSErrContainer_p2* persCont, MsgStream& log)
{
  (persCont->m_bsErrs).reserve(transCont->size());
  for (const std::pair<IdentifierHash, int>* bsErr: *transCont) {
    if (bsErr==nullptr) {
      log << MSG::WARNING << "const std::pair<IdentifierHash, int>* is nullptr and skipped." << endmsg;
      continue;
    }
    (persCont->m_bsErrs).push_back(*bsErr);
  }

  return;
}

void  InDetBSErrContainerCnv_p2::persToTrans(const InDetBSErrContainer_p2* persCont, InDetBSErrContainer* transCont, MsgStream& /*log*/)
{
  transCont->reserve((persCont->m_bsErrs).size());
  for (const std::pair<IdentifierHash, int>& bsErr: persCont->m_bsErrs) {  
    transCont->push_back(std::make_unique<std::pair<IdentifierHash, int>>(bsErr));
  }

  return;
}

//================================================================
InDetBSErrContainer* InDetBSErrContainerCnv_p2::createTransient(const InDetBSErrContainer_p2* persObj, MsgStream& log) {
  std::unique_ptr<InDetBSErrContainer> trans{std::make_unique<InDetBSErrContainer>()};
  persToTrans(persObj, trans.get(), log);
  return trans.release();
}
