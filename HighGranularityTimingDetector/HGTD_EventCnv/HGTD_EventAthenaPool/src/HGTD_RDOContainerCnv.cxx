/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventAthenaPool/src/HGTD_RDOContainerCnv.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 * @brief
 */

#include "HGTD_RDOContainerCnv.h"
#include "GaudiKernel/MsgStream.h"

#include <memory>

HGTD_RDOContainerCnv::HGTD_RDOContainerCnv(ISvcLocator* svcloc)
    : HGTD_RDOContainerCnvBase_t(svcloc) {}

HGTD_RDOContainer* HGTD_RDOContainerCnv::createTransient() {

  static pool::Guid p1_guid(
      "C25315CC-F0A2-43D6-8F42-012BE34B0107"); // with HGTD_RDO_p1
  ATH_MSG_DEBUG("createTransient(): main converter");

  HGTD_RDOContainer* trans_cont(0);
  if (compareClassGuid(p1_guid)) {
    ATH_MSG_DEBUG("createTransient(): T/P version 1 detected");
    std::auto_ptr<HGTD_RDOContainer_PERS_t> pers_cont(
        poolReadObject<HGTD_RDOContainer_PERS_t>());

    trans_cont = m_converter.createTransient(pers_cont.get(), msg());

  } else {
    throw std::runtime_error(
        "Unsupported persistent version of HGTD_RDOContainer");
  }
  return trans_cont;
}

HGTD_RDOContainer_PERS_t*
HGTD_RDOContainerCnv::createPersistent(HGTD_RDOContainer* trans_cont) {

  HGTD_RDOContainer_PERS_t* pers_cont =
      m_converter.createPersistent(trans_cont, msg());

  return pers_cont;
}
