/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetLowBetaContainerCnv.h"
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaContainerCnv_tlp1.h"

InDetLowBetaContainerCnv::InDetLowBetaContainerCnv(ISvcLocator *svcloc)
  : InDetLowBetaContainerCnvBase(svcloc, "InDetLowBetaContainerCnv"),
    m_TPConverter_tlp1 (new InDetLowBetaContainerCnv_tlp1)
{ }

InDetLowBetaContainerCnv::~InDetLowBetaContainerCnv()
{
  delete m_TPConverter_tlp1;
}
 

// createPersistent
InDetLowBetaContainer_PERS *InDetLowBetaContainerCnv::createPersistent(InDet::InDetLowBetaContainer *transObj) {
  ATH_MSG_DEBUG("InDetLowBetaContainerCnv::createPersistent called");

  InDetLowBetaContainer_PERS *p_InDetLowBetaCont = m_TPConverter_tlp1->createPersistent(transObj, msg());
 
  return p_InDetLowBetaCont;
 
}
 
// createTransient
InDet::InDetLowBetaContainer *InDetLowBetaContainerCnv::createTransient() {
  ATH_MSG_DEBUG("InDetLowBetaContainerCnv::createTransient called");

  const pool::Guid tlp1_guid("2EBE2034-8157-477B-B327-D37BE8A0317D");

  InDet::InDetLowBetaContainer *p_collection = 0;
  if (compareClassGuid(tlp1_guid)) {
    poolReadObject<InDetLowBetaContainer_PERS>(*m_TPConverter_tlp1);
    p_collection = m_TPConverter_tlp1->createTransient(msg());
  } else
    throw std::runtime_error("Unsupported persistent version of InDetLowBetaContainer");

  return p_collection;
}


AthenaPoolTopLevelTPCnvBase*
InDetLowBetaContainerCnv::getTopLevelTPCnv()
{
  return m_TPConverter_tlp1;
} 
