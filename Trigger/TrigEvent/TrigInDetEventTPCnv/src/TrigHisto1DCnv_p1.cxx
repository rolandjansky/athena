/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigHisto1D.h"
#include "TrigInDetEventTPCnv/TrigHisto1D_p1.h"
#include "TrigInDetEventTPCnv/TrigHisto1DCnv_p1.h"

void TrigHisto1DCnv_p1::persToTrans(const TrigHisto1D_p1 *persObj,
				    TrigHisto1D *transObj,
				    MsgStream& log) {
  log << MSG::DEBUG << "TrigHisto1DCnv_p1::persToTrans" << endreq;

  *transObj = TrigHisto1D (persObj->m_nbins_x,
                           persObj->m_min_x,
                           persObj->m_max_x,
                           persObj->m_contents);
}

//---------------------------------------------------------------

void TrigHisto1DCnv_p1::transToPers(const TrigHisto1D *transObj,
				    TrigHisto1D_p1 *persObj,
				    MsgStream& log) {
  log << MSG::DEBUG << "TrigHisto1DCnv_p1::transToPers" << endreq;
  
  persObj->m_nbins_x = transObj->nbins_x();
  persObj->m_min_x = transObj->min_x();
  persObj->m_max_x = transObj->max_x();
  persObj->m_contents = transObj->contents();
}
