///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ZdcDigitsCnv_p1.cxx 
// Implementation file for class ZdcDigitsCnv_p1
// Author: Peter Steinberg <peter.steinberg@bnl.gov>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 

#include "ZdcEvent/ZdcDigits.h"
#include "ZdcEventTPCnv/ZdcDigitsCnv_p1.h"


void
ZdcDigitsCnv_p1::persToTrans(const ZdcDigits_p1* persObj, ZdcDigits* transObj, MsgStream &log)
{
  //log << MSG::INFO << "ZdcDigitsCnv_p1::persToTrans called " << endmsg;
  log << MSG::VERBOSE << "ZdcDigitsCnv_p1::persToTrans called " << endmsg;

  *static_cast<ZdcRawData*>(transObj) =
    ZdcRawData (Identifier(Identifier32(persObj->m_id)));

  //log << MSG::INFO << "ZdcDigitsCnv_p1::reading in... " << endmsg;
  transObj->set_digits_gain0_delay0 (persObj->m_digits_gain0_delay0);
  transObj->set_digits_gain0_delay1 (persObj->m_digits_gain0_delay1);
  transObj->set_digits_gain1_delay0 (persObj->m_digits_gain1_delay0);
  transObj->set_digits_gain1_delay1 (persObj->m_digits_gain1_delay1);
   
  //log << MSG::INFO << "ZdcDigitsCnv_p1::finished reading in... " << endmsg;

}


void
ZdcDigitsCnv_p1::transToPers(const ZdcDigits* transObj, ZdcDigits_p1* persObj, MsgStream &log)
{
  log << MSG::VERBOSE << "ZdcDigitsCnv_p1::transToPers called " << endmsg;

  persObj->m_id = transObj->identify().get_identifier32().get_compact();

  persObj->m_digits_gain0_delay0 = transObj->get_digits_gain0_delay0();
  persObj->m_digits_gain0_delay1 = transObj->get_digits_gain0_delay1();
  persObj->m_digits_gain1_delay0 = transObj->get_digits_gain1_delay0();
  persObj->m_digits_gain1_delay1 = transObj->get_digits_gain1_delay1();

}
