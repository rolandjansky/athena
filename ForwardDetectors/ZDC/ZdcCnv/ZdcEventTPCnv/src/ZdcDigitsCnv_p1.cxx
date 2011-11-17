///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ZdcDigitsCnv_p1.cxx 
// Implementation file for class ZdcDigitsCnv_p1
// Author: Peter Steinberg <peter.steinberg@bnl.gov>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 

#define private public
#define protected public
#include "ZdcEvent/ZdcDigits.h"
#undef private
#undef protected

#include "ZdcEventTPCnv/ZdcDigitsCnv_p1.h"


void
ZdcDigitsCnv_p1::persToTrans(const ZdcDigits_p1* persObj, ZdcDigits* transObj, MsgStream &log)
{
  //log << MSG::INFO << "ZdcDigitsCnv_p1::persToTrans called " << endreq;
  log << MSG::VERBOSE << "ZdcDigitsCnv_p1::persToTrans called " << endreq;

  transObj->m_id = Identifier(Identifier32(persObj->m_id));

  //log << MSG::INFO << "ZdcDigitsCnv_p1::reading in... " << endreq;
   
  transObj->m_digits_gain0_delay0.reserve(persObj->m_digits_gain0_delay0.size());
  transObj->m_digits_gain0_delay1.reserve(persObj->m_digits_gain0_delay1.size());
  transObj->m_digits_gain1_delay0.reserve(persObj->m_digits_gain1_delay0.size());
  transObj->m_digits_gain1_delay1.reserve(persObj->m_digits_gain1_delay1.size());

  for (std::vector<int>::const_iterator it = persObj->m_digits_gain0_delay0.begin(); it != persObj->m_digits_gain0_delay0.end(); ++it) {
    int val = (*it);
    transObj->m_digits_gain0_delay0.push_back( val );
  }

  for (std::vector<int>::const_iterator it = persObj->m_digits_gain0_delay1.begin(); it != persObj->m_digits_gain0_delay1.end(); ++it) {
    int val = (*it);
    transObj->m_digits_gain0_delay1.push_back( val );
  }

  for (std::vector<int>::const_iterator it = persObj->m_digits_gain1_delay0.begin(); it != persObj->m_digits_gain1_delay0.end(); ++it) {
    int val = (*it);
    transObj->m_digits_gain1_delay0.push_back( val );
  }

  for (std::vector<int>::const_iterator it = persObj->m_digits_gain1_delay1.begin(); it != persObj->m_digits_gain1_delay1.end(); ++it) {
    int val = (*it);
    transObj->m_digits_gain1_delay1.push_back( val );
  }

  //log << MSG::INFO << "ZdcDigitsCnv_p1::finished reading in... " << endreq;

}


void
ZdcDigitsCnv_p1::transToPers(const ZdcDigits* transObj, ZdcDigits_p1* persObj, MsgStream &log)
{
  log << MSG::VERBOSE << "ZdcDigitsCnv_p1::transToPers called " << endreq;

  persObj->m_id = transObj->m_id.get_identifier32().get_compact();

  persObj->m_digits_gain0_delay0.reserve(transObj->m_digits_gain0_delay0.size());
  for (std::vector<int>::const_iterator it = transObj->m_digits_gain0_delay0.begin(); it != transObj->m_digits_gain0_delay0.end(); ++it) {
    int val = (*it);
    persObj->m_digits_gain0_delay0.push_back( val );
  }

  persObj->m_digits_gain0_delay1.reserve(transObj->m_digits_gain0_delay1.size());
  for (std::vector<int>::const_iterator it = transObj->m_digits_gain0_delay1.begin(); it != transObj->m_digits_gain0_delay1.end(); ++it) {
    int val = (*it);
    persObj->m_digits_gain0_delay1.push_back( val );
  }

  persObj->m_digits_gain1_delay0.reserve(transObj->m_digits_gain1_delay0.size());
  for (std::vector<int>::const_iterator it = transObj->m_digits_gain1_delay0.begin(); it != transObj->m_digits_gain1_delay0.end(); ++it) {
    int val = (*it);
    persObj->m_digits_gain1_delay0.push_back( val );
  }

  persObj->m_digits_gain1_delay1.reserve(transObj->m_digits_gain1_delay1.size());
  for (std::vector<int>::const_iterator it = transObj->m_digits_gain1_delay1.begin(); it != transObj->m_digits_gain1_delay1.end(); ++it) {
    int val = (*it);
    persObj->m_digits_gain1_delay1.push_back( val );
  }

}
