/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventAthenaPool/src/HGTD_RDO_ContainerCnv.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @brief
 */

#include "HGTD_RDO_ContainerCnv.h"
#include "GaudiKernel/MsgStream.h"

#include <memory>

HGTD_RDO_ContainerCnv::HGTD_RDO_ContainerCnv(ISvcLocator* svcloc)
  : HGTD_RDO_ContainerCnvBase_t(svcloc) {}

HGTD_RDO_Container* HGTD_RDO_ContainerCnv::createTransient() {

  static const pool::Guid p1_guid(
    "C25315CC-F0A2-43D6-8F42-012BE34B0107"); // with HGTD_RDO_p1
  ATH_MSG_DEBUG("createTransient(): main converter");

  HGTD_RDO_Container* trans_cont(0);
  if (compareClassGuid(p1_guid)) {
    ATH_MSG_DEBUG("createTransient(): T/P version 1 detected");
    std::unique_ptr<HGTD_RDO_Container_PERS_t> pers_cont(
      poolReadObject<HGTD_RDO_Container_PERS_t>());

    trans_cont = m_converter.createTransient(pers_cont.get(), msg());

  } else {
    throw std::runtime_error(
      "Unsupported persistent version of HGTD_RDO_Container");
  }
  return trans_cont;
}

HGTD_RDO_Container_PERS_t*
HGTD_RDO_ContainerCnv::createPersistent(HGTD_RDO_Container* trans_cont) {

  HGTD_RDO_Container_PERS_t* pers_cont =
    m_converter.createPersistent(trans_cont, msg());

  return pers_cont;
}
