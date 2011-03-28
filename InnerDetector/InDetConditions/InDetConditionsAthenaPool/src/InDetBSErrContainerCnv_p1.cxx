/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "InDetConditionsAthenaPool/InDetBSErrContainer_p1.h"
#include "InDetByteStreamErrors/InDetBSErrContainer.h"
#undef private
#undef protected
#include "InDetBSErrContainerCnv_p1.h"

#include "Identifier/IdentifierHash.h"
#include "DataModel/DataPool.h"

void InDetBSErrContainerCnv_p1::transToPers(const InDetBSErrContainer* transCont, InDetBSErrContainer_p1* persCont, MsgStream & /*log */) 
{
  std::vector<std::pair<IdentifierHash, int>* >::const_iterator it = transCont->begin();
  std::vector<std::pair<IdentifierHash, int>* >::const_iterator itEnd = transCont->end();
  (persCont->m_bsErrs).reserve(transCont->size());

  for (; it != itEnd; ++it) {
    (persCont->m_bsErrs).push_back(*it);
  }
  return;
}

void  InDetBSErrContainerCnv_p1::persToTrans(const InDetBSErrContainer_p1* persCont, InDetBSErrContainer* transCont, MsgStream & /*log */) 
{

  std::vector<std::pair<IdentifierHash, int>* >::const_iterator it = (persCont->m_bsErrs).begin();
  std::vector<std::pair<IdentifierHash, int>* >::const_iterator itEnd = (persCont->m_bsErrs).end();
  transCont->reserve((persCont->m_bsErrs).size());

  for (; it != itEnd; ++it) {
    transCont->push_back(*it);
  }
  return;

}

//================================================================
InDetBSErrContainer* InDetBSErrContainerCnv_p1::createTransient(const InDetBSErrContainer_p1* persObj, MsgStream& log) {
  std::auto_ptr<InDetBSErrContainer> trans(new InDetBSErrContainer());
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}
