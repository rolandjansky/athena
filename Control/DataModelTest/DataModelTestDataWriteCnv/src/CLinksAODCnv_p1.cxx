/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataWriteCnv/CLinksAODCnv_p1.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief TP converter for CLinksAOD_p1.
 */


#include "CLinksAODCnv_p1.h"


namespace DMTest {


void CLinksAODCnv_p1::persToTrans (const CLinksAOD_p1* persObj,
                                   CLinksAOD* transObj,
                                   MsgStream &log) const
{
  std::vector<ElementLink<CVec> > vel;
  vel.resize (persObj->m_vel.size());
  size_t i = 0;
  for (const ElementLinkInt_p3& el_p : persObj->m_vel) {
    m_velCnv.persToTrans (el_p, vel[i], log);
    ++i;
  }
  transObj->setVel (std::move (vel));

  m_elvCnv.persToTrans (persObj->m_elv, transObj->elv(), log);
}


void CLinksAODCnv_p1::transToPers (const CLinksAOD* transObj,
                                   CLinksAOD_p1* persObj,
                                   MsgStream &log) const
{
  persObj->m_vel.resize (transObj->vel().size());
  size_t i = 0;
  for (const ElementLink<CVec>& el : transObj->vel()) {
    m_velCnv.transToPers (el, persObj->m_vel[i], log);
    ++i;
  }

  m_elvCnv.transToPers (transObj->elv(), persObj->m_elv, log);
}


} // namespace DMTest
