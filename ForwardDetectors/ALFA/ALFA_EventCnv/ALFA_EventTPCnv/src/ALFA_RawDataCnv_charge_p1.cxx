/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawEv/ALFA_RawData_charge.h"
#include "GaudiKernel/MsgStream.h"
#include "ALFA_EventTPCnv/ALFA_RawDataCnv_charge_p1.h"

void ALFA_RawDataCnv_charge_p1::persToTrans(const ALFA_RawData_charge_p1* persObj, 
ALFA_RawData_charge* transObj, MsgStream &log) {


  log << MSG::DEBUG << "In ALFA_RawDataCnv_charge_p1:persToTrans called" << endreq;

  transObj->SetZero_PMF();
  transObj->SetChannelNum_PMF (persObj->ChannelNumId);
  transObj->SetPMFId_PMF (persObj->PMFId);
  transObj->SetMBId_PMF (persObj->MBId);
  transObj->SetEventCount_PMF (persObj->EventCount);
  transObj->SetChargeChanVect_PMF (persObj->m_ChargeChan);
  for (uint32_t w : persObj->m_dataWords)
    transObj->addData(w);
  }

void ALFA_RawDataCnv_charge_p1::transToPers(const ALFA_RawData_charge* transObj, ALFA_RawData_charge_p1* persObj, MsgStream &log) {

  
  log << MSG::DEBUG << "In ALFA_RawDataCnv_p1:transToPers called" << endreq;
  
  persObj->ChannelNumId  = transObj->GetChannelNum_PMF();
  persObj->PMFId  = transObj->GetPMFId_PMF();
  persObj->MBId  = transObj->GetMBId_PMF();
  persObj->EventCount  = transObj->GetEventCount_PMF();
  
  persObj->m_ChargeChan.resize(64);

  const std::vector<uint16_t>&v =  transObj->ChargeChanVect_number();
  for ( int i = 0; i < 64; i++ ){
    persObj->m_ChargeChan[i] =  v[i];
  }   
  

  if (transObj->dataWords()!=0)
    persObj->m_dataWords = *transObj->dataWords();
}
