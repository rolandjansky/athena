/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetByteStreamErrorsAthenaPool/SCT_ByteStreamFractionContainer_p1.h"
#include "InDetByteStreamErrors/SCT_ByteStreamFractionContainer.h"
#include "SCT_ByteStreamFractionContainerCnv_p1.h"

#include "AthAllocators/DataPool.h"

void SCT_ByteStreamFractionContainerCnv_p1::transToPers(const SCT_ByteStreamFractionContainer* transCont, SCT_ByteStreamFractionContainer_p1* persCont, MsgStream & /*log */) 
{
  (persCont->m_trueSets).reserve(SCT_ByteStreamFractionContainer::NumberOfTypes);
  (persCont->m_falseSets).reserve(SCT_ByteStreamFractionContainer::NumberOfTypes);

  for (unsigned int i=SCT_ByteStreamFractionContainer::FirstIndex; i<SCT_ByteStreamFractionContainer::NumberOfTypes; i++) {
    SCT_ByteStreamFractionContainer::Type type = static_cast<SCT_ByteStreamFractionContainer::Type>(i);
    (persCont->m_trueSets).push_back(transCont->getFraction(type).getTrueSet());
    (persCont->m_falseSets).push_back(transCont->getFraction(type).getFalseSet());
  }
  return;
}

void SCT_ByteStreamFractionContainerCnv_p1::persToTrans(const SCT_ByteStreamFractionContainer_p1* persCont, SCT_ByteStreamFractionContainer* transCont, MsgStream & /*log */) 
{
  std::vector<std::set<uint32_t> >::const_iterator it = (persCont->m_trueSets).begin();
  std::vector<std::set<uint32_t> >::const_iterator itEnd = (persCont->m_trueSets).end();
  for (unsigned int i=SCT_ByteStreamFractionContainer::FirstIndex; i<SCT_ByteStreamFractionContainer::NumberOfTypes and it!=itEnd; i++, it++) {
    SCT_ByteStreamFractionContainer::Type type = static_cast<SCT_ByteStreamFractionContainer::Type>(i);
    transCont->setTrueSet(type, *it);
  }
  std::vector<std::set<uint32_t> >::const_iterator jt = (persCont->m_falseSets).begin();
  std::vector<std::set<uint32_t> >::const_iterator jtEnd = (persCont->m_falseSets).end();
  for (unsigned int j=SCT_ByteStreamFractionContainer::FirstIndex; j<SCT_ByteStreamFractionContainer::NumberOfTypes and jt!=jtEnd; j++, jt++) {
    SCT_ByteStreamFractionContainer::Type type = static_cast<SCT_ByteStreamFractionContainer::Type>(j);
    transCont->setFalseSet(type, *jt);
  }
  return;
}

//================================================================
SCT_ByteStreamFractionContainer* SCT_ByteStreamFractionContainerCnv_p1::createTransient(const SCT_ByteStreamFractionContainer_p1* persObj, MsgStream& log) {
  std::unique_ptr<SCT_ByteStreamFractionContainer> trans(std::make_unique<SCT_ByteStreamFractionContainer>());
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}
