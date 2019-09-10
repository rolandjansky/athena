/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetBSErrContainerCnv.cxx
 * @brief Generated implementation file which includes header files needed by InDetBSErrContainerCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "InDetBSErrContainerCnv.h"

InDetBSErrContainer_PERS* 
InDetBSErrContainerCnv::createPersistent(InDetBSErrContainer* transCont) {
  MsgStream log{msgSvc(), "InDetBSErrContainerCnv"};
  InDetBSErrContainer_PERS* persObj{m_TPConverter_PERS.createPersistent(transCont, log)};
  return persObj; 
}

InDetBSErrContainer* 
InDetBSErrContainerCnv::createTransient() {
  MsgStream log{msgSvc(), "InDetBSErrContainerCnv"};
  static const pool::Guid p1_guid{"FA64DC17-D07E-4305-9B21-18C64F1B4C47"};
  static const pool::Guid p2_guid{"ECBBC13B-0A44-4694-AD24-574A7598C627"};
  if (compareClassGuid(p1_guid)) {
    return m_TPConverter_p1.createTransient(poolReadObject<InDetBSErrContainer_p1>(), log);
  } else if (compareClassGuid(p2_guid)) {
    return m_TPConverter_PERS.createTransient(poolReadObject<InDetBSErrContainer_p2>(), log);
  }
  throw std::runtime_error("Unsupported persistent version of Data Collection");
}
