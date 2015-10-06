/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "ALFA_RawEv/ALFA_RawData_charge.h"
#undef private
#undef protected

#include "GaudiKernel/MsgStream.h"
#include "ALFA_EventTPCnv/ALFA_RawDataCnv_charge_p1.h"

void ALFA_RawDataCnv_charge_p1::persToTrans(const ALFA_RawData_charge_p1* persObj, 
ALFA_RawData_charge* transObj, MsgStream &log) {


  log << MSG::DEBUG << "In ALFA_RawDataCnv_charge_p1:persToTrans called" << endreq;
  
  transObj->ChannelNumId  = persObj->ChannelNumId;
  transObj->PMFId  = persObj->PMFId;
  transObj->MBId  = persObj->MBId;
  transObj->EventCount  = persObj->EventCount;
  transObj->ChargeChan.assign(persObj->m_ChargeChan.begin(), persObj->m_ChargeChan.end());
  transObj->p_dataWords->assign (persObj->m_dataWords.begin(), persObj->m_dataWords.end());
 
  }

void ALFA_RawDataCnv_charge_p1::transToPers(const ALFA_RawData_charge* transObj, ALFA_RawData_charge_p1* persObj, MsgStream &log) {

  
  log << MSG::DEBUG << "In ALFA_RawDataCnv_p1:transToPers called" << endreq;
  
  persObj->ChannelNumId  = transObj->ChannelNumId;
  persObj->PMFId  = transObj->PMFId;
  persObj->MBId  = transObj->MBId;
  persObj->EventCount  = transObj->EventCount;
  
  persObj->m_ChargeChan.resize(64);

  for ( int i = 0; i < 64; i++ ){
  persObj->m_ChargeChan[i] =  transObj->ChargeChan[i];
  }   
  

     if (transObj->dataWords()!=0){
      persObj->m_dataWords.reserve(transObj->dataWords()->size());
      std::vector<uint32_t>::const_iterator it    = transObj->dataWords()->begin();
      std::vector<uint32_t>::const_iterator itEnd = transObj->dataWords()->end();
      std::copy(it, itEnd, persObj->m_dataWords.begin() );
    }

 }
