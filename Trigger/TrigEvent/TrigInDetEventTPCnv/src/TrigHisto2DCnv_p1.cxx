/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigInDetEvent/TrigHisto2D.h"
#include "TrigInDetEventTPCnv/TrigHisto2D_p1.h"
#undef private
#undef protected

#include "TrigInDetEventTPCnv/TrigHisto2DCnv_p1.h"

void TrigHisto2DCnv_p1::persToTrans(const TrigHisto2D_p1 *persObj,
				    TrigHisto2D *transObj,
				    MsgStream& log) {
  log << MSG::DEBUG << "TrigHisto2DCnv_p1::persToTrans" << endreq;

  transObj->m_nbins_x = persObj->m_nbins_x;
  transObj->m_min_x = persObj->m_min_x;
  transObj->m_max_x = persObj->m_max_x;
  transObj->m_nbins_y = persObj->m_nbins_y;
  transObj->m_min_y = persObj->m_min_y;
  transObj->m_max_y = persObj->m_max_y; 
  transObj->m_contents = persObj->m_contents;

  // Calculate values only present in the transient class
  if(persObj->m_nbins_x != 0) {
    transObj->m_binSize_x = (persObj->m_max_x - persObj->m_min_x)/((float)persObj->m_nbins_x);
  }
  else {
    transObj->m_binSize_x = 0.;
    log << MSG::DEBUG << "m_nbins_x is zero, setting m_binSize_x to zero." << endreq;
  }

  transObj->m_underflowBin_x = 0;
  transObj->m_overflowBin_x = persObj->m_nbins_x+1;

  if(persObj->m_nbins_y != 0) {
    transObj->m_binSize_y = (persObj->m_max_y - persObj->m_min_y)/((float)persObj->m_nbins_y);
  }
  else {
    transObj->m_binSize_y = 0.;
    log << MSG::DEBUG << "m_nbins_y is zero, setting m_binSize_y to zero." << endreq;
  }

  transObj->m_underflowBin_y = 0;
  transObj->m_overflowBin_y = persObj->m_nbins_y+1;

}

//---------------------------------------------------------------

void TrigHisto2DCnv_p1::transToPers(const TrigHisto2D *transObj,
				    TrigHisto2D_p1 *persObj,
				    MsgStream& log) {
  log << MSG::DEBUG << "TrigHisto2DCnv_p1::transToPers" << endreq;

  persObj->m_nbins_x = transObj->m_nbins_x;
  persObj->m_min_x = transObj->m_min_x;
  persObj->m_max_x = transObj->m_max_x;
  persObj->m_nbins_y = transObj->m_nbins_y;
  persObj->m_min_y = transObj->m_min_y;
  persObj->m_max_y = transObj->m_max_y;
  persObj->m_contents = transObj->m_contents;
}
