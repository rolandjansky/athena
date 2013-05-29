/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "ALFA_RawEv/ALFA_RawData.h"
#undef private
#undef protected

#include "GaudiKernel/MsgStream.h"
#include "ALFA_EventTPCnv/ALFA_RawDataCnv_p1.h"

void ALFA_RawDataCnv_p1::persToTrans(const ALFA_RawData_p1* persObj, 
ALFA_RawData* transObj, MsgStream &log) {


  log << MSG::DEBUG << "In ALFA_RawDataCnv_p1:persToTrans called" << endreq;
  
  transObj->WordId  = persObj->WordId;
  transObj->PMFId  = persObj->PMFId;
  transObj->MBId  = persObj->MBId;
  transObj->EventCount  = persObj->EventCount;
  transObj->MarocChan.assign(persObj->m_MarocChan.begin(), persObj->m_MarocChan.end());
  transObj->p_dataWords->assign (persObj->m_dataWords.begin(), persObj->m_dataWords.end());
  transObj->m_error_bit17  = persObj->m_error_bit17;

 
  }

void ALFA_RawDataCnv_p1::transToPers(const ALFA_RawData* transObj, ALFA_RawData_p1* persObj, MsgStream &log) {

  
  log << MSG::DEBUG << "In ALFA_RawDataCnv_p1:transToPers called" << endreq;
  
  persObj->WordId  = transObj->WordId;
  persObj->PMFId  = transObj->PMFId;
  persObj->MBId  = transObj->MBId;
  persObj->EventCount  = transObj->EventCount;
  persObj->m_error_bit17  = transObj->m_error_bit17;

  persObj->m_MarocChan.resize(16);

  for ( int i = 0; i < 16; i++ ){
  persObj->m_MarocChan[i] =  transObj->MarocChan[i];
  }   
  

     if (transObj->dataWords()!=0){
      persObj->m_dataWords.reserve(transObj->dataWords()->size());
      std::vector<uint32_t>::const_iterator it    = transObj->dataWords()->begin();
      std::vector<uint32_t>::const_iterator itEnd = transObj->dataWords()->end();
      std::copy(it, itEnd, persObj->m_dataWords.begin() );
    }

 }
