/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigInDetEvent/TrigHisto1D.h"
#include "TrigInDetEventTPCnv/TrigHisto1D_p1.h"
#undef private
#undef protected

#include "TrigInDetEventTPCnv/TrigHisto1DCnv_p1.h"

void TrigHisto1DCnv_p1::persToTrans(const TrigHisto1D_p1 *persObj,
				    TrigHisto1D *transObj,
				    MsgStream& log) {
  log << MSG::DEBUG << "TrigHisto1DCnv_p1::persToTrans" << endreq;
  
  transObj->m_nbins_x = persObj->m_nbins_x;
  transObj->m_min_x = persObj->m_min_x;
  transObj->m_max_x = persObj->m_max_x;
  transObj->m_contents = persObj->m_contents;

  // Calculate values only present in the transient class
  if(persObj->m_nbins_x != 0) {
    transObj->m_binSize_x = (persObj->m_max_x - persObj->m_min_x)/((float)persObj->m_nbins_x); 
  }
  else {
    transObj->m_binSize_x = 0.;
  }
  transObj->m_underflowBin_x = 0;
  transObj->m_overflowBin_x = persObj->m_nbins_x+1;
}

//---------------------------------------------------------------

void TrigHisto1DCnv_p1::transToPers(const TrigHisto1D *transObj,
				    TrigHisto1D_p1 *persObj,
				    MsgStream& log) {
  log << MSG::DEBUG << "TrigHisto1DCnv_p1::transToPers" << endreq;
  
  persObj->m_nbins_x = transObj->m_nbins_x;
  persObj->m_min_x = transObj->m_min_x;
  persObj->m_max_x = transObj->m_max_x;
  persObj->m_contents = transObj->m_contents;
}
