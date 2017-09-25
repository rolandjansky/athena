/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_RawEv/AFP_RawData.h"
#include "GaudiKernel/MsgStream.h"
#include "AFP_EventTPCnv/AFP_RawDataCnv_p1.h"

void AFP_RawDataCnv_p1::persToTrans(const AFP_RawData_p1* persObj,
AFP_RawData* transObj, MsgStream &log) {


  log << MSG::DEBUG << "In AFP_RawDataCnv_p1:persToTrans called" << endreq;

  transObj->SetZero_PMF();
  transObj->Set_DiscConf (persObj->HitDiscConfig);
  transObj->Set_link (persObj->Link);
  transObj->Set_column (persObj->Column);
  transObj->Set_row (persObj->Row);
  transObj->Set_ToT (persObj->ToT);
  transObj->Set_lvl1 (persObj->lvl1);
  for (uint32_t w : persObj->m_dataWords)
  transObj->addData (w);
  transObj->Set_error_bit17 (persObj->m_error_bit17);

	 
	  }

void AFP_RawDataCnv_p1::transToPers(const AFP_RawData* transObj, AFP_RawData_p1* persObj, MsgStream &log) {

 
  log << MSG::DEBUG << "In AFP_RawDataCnv_p1:transToPers called" << endreq;
 
  persObj->HitDiscConfig  = transObj->Get_DiscConf();
  persObj->Link  = transObj->Get_link();
  persObj->Column  = transObj->Get_column();
  persObj->Row  = transObj->Get_row();
  persObj->m_error_bit17  = transObj->Get_error_bit17();
  persObj->ToT = transObj->Get_ToT();
  persObj->lvl1 = transObj->Get_lvl1();
//  const std::vector<uint16_t>& v = transObj->HitChan();
 // for ( int i = 0; i < 16; i++ ){
//  persObj->m_ToT[i] =  v[i];
// }   
 

  if (transObj->dataWords()!=0)
    persObj->m_dataWords = *transObj->dataWords();
}
