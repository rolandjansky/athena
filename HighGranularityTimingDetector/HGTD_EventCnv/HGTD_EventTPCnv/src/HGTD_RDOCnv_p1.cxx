/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @fileHGTD_EventTPCnv/src/HGTD_RDOCnv_p1.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 * @brief
 */

#include "HGTD_EventTPCnv/HGTD_RDOCnv_p1.h"

void HGTD::HGTD_RDOCnv_p1::transToPers(const HGTD::HGTD_RDO* trans_obj,
                                       HGTD::HGTD_RDO_p1* pers_obj,
                                       MsgStream& log) {
  log << MSG::VERBOSE << "calling HGTD_RDOCnv_p1::transToPers" << endreq;

  pers_obj->m_rdo_id = trans_obj->identify().get_compact();
  pers_obj->m_toa = trans_obj->getTOA();
  pers_obj->m_tot = trans_obj->getTOT();
  pers_obj->m_bcid = trans_obj->getBCID();
  pers_obj->m_l1_id = trans_obj->getL1ID();
  pers_obj->m_l1_a = trans_obj->getL1A();
}

void HGTD::HGTD_RDOCnv_p1::persToTrans(const HGTD::HGTD_RDO_p1* pers_obj,
                                       HGTD::HGTD_RDO* trans_obj,
                                       MsgStream& log) {
  log << MSG::VERBOSE << "calling HGTD_RDOCnv_p1::persToTrans" << endreq;
  *trans_obj = HGTD::HGTD_RDO(Identifier(pers_obj->m_rdo_id), pers_obj->m_toa,
                              pers_obj->m_tot, pers_obj->m_bcid,
                              pers_obj->m_l1_id, pers_obj->m_l1_a);
}
