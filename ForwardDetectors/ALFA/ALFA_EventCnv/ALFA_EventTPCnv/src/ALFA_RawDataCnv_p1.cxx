/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawEv/ALFA_RawData.h"
#include "GaudiKernel/MsgStream.h"
#include "ALFA_EventTPCnv/ALFA_RawDataCnv_p1.h"

void ALFA_RawDataCnv_p1::persToTrans(const ALFA_RawData_p1* persObj, 
ALFA_RawData* transObj, MsgStream &log) {


  log << MSG::DEBUG << "In ALFA_RawDataCnv_p1:persToTrans called" << endreq;

  transObj->SetZero_PMF();
  transObj->SetWordId_PMF (persObj->WordId);
  transObj->SetPMFId_PMF (persObj->PMFId);
  transObj->SetMBId_PMF (persObj->MBId);
  transObj->SetEventCount_PMF (persObj->EventCount);
  transObj->SetMarocChan_PMF (persObj->m_MarocChan);
  for (uint32_t w : persObj->m_dataWords)
    transObj->addData (w);
  transObj->Set_error_bit17 (persObj->m_error_bit17);

 
  }

void ALFA_RawDataCnv_p1::transToPers(const ALFA_RawData* transObj, ALFA_RawData_p1* persObj, MsgStream &log) {

  
  log << MSG::DEBUG << "In ALFA_RawDataCnv_p1:transToPers called" << endreq;
  
  persObj->WordId  = transObj->GetWordId_PMF();
  persObj->PMFId  = transObj->GetPMFId_PMF();
  persObj->MBId  = transObj->GetMBId_PMF();
  persObj->EventCount  = transObj->GetEventCount_PMF();
  persObj->m_error_bit17  = transObj->Get_error_bit17();

  persObj->m_MarocChan.resize(16);

  const std::vector<uint16_t>& v = transObj->HitChan();
  for ( int i = 0; i < 16; i++ ){
    persObj->m_MarocChan[i] =  v[i];
  }   
  

  if (transObj->dataWords()!=0)
    persObj->m_dataWords = *transObj->dataWords();
}
