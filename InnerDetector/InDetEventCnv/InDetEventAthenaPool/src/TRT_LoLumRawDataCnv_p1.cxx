/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetRawData/TRT_LoLumRawData.h"

// Persistent class and converter header file
#include "InDetEventAthenaPool/InDetRawData_p1.h"
#include "TRT_LoLumRawDataCnv_p1.h"

// Other stuff
#include "Identifier/Identifier.h"
#include "MsgUtil.h"


void
TRT_LoLumRawDataCnv_p1::persToTrans(const InDetRawData_p1* persObj, TRT_LoLumRawData* transObj, MsgStream &log)
{
   MSG_VERBOSE(log,"TRT_LoLumRawData_p1::persToTrans called ");

   *transObj = TRT_LoLumRawData (Identifier(persObj->m_rdoId),
                                 persObj->m_word);
}

void
TRT_LoLumRawDataCnv_p1::transToPers(const TRT_LoLumRawData* transObj, InDetRawData_p1* persObj, MsgStream &log) 
{
   MSG_VERBOSE(log,"TRT_LoLumRawData_p1::transToPers called ");
   persObj->m_rdoId = transObj->identify().get_identifier32().get_compact();
   persObj->m_word = transObj->getWord();
}
