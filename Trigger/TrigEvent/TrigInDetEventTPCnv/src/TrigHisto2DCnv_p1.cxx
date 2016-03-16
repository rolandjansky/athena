/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigHisto2D.h"
#include "TrigInDetEventTPCnv/TrigHisto2D_p1.h"
#include "TrigInDetEventTPCnv/TrigHisto2DCnv_p1.h"

void TrigHisto2DCnv_p1::persToTrans(const TrigHisto2D_p1 *persObj,
				    TrigHisto2D *transObj,
				    MsgStream& log) {
  log << MSG::DEBUG << "TrigHisto2DCnv_p1::persToTrans" << endreq;

  *transObj = TrigHisto2D (persObj->m_nbins_x,
                           persObj->m_min_x,
                           persObj->m_max_x,
                           persObj->m_nbins_y,
                           persObj->m_min_y,
                           persObj->m_max_y,
                           persObj->m_contents);
}

//---------------------------------------------------------------

void TrigHisto2DCnv_p1::transToPers(const TrigHisto2D *transObj,
				    TrigHisto2D_p1 *persObj,
				    MsgStream& log) {
  log << MSG::DEBUG << "TrigHisto2DCnv_p1::transToPers" << endreq;

  persObj->m_nbins_x = transObj->nbins_x();
  persObj->m_min_x = transObj->min_x();
  persObj->m_max_x = transObj->max_x();
  persObj->m_nbins_y = transObj->nbins_y();
  persObj->m_min_y = transObj->min_y();
  persObj->m_max_y = transObj->max_y();
  persObj->m_contents = transObj->contents();
}
