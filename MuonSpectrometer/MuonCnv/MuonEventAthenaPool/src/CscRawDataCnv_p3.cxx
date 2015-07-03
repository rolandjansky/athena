/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "MuonRDO/CscRawData.h"
#undef private
#undef protected

#include "CscRawDataCnv_p3.h"

#include "GaudiKernel/GaudiException.h"

void
CscRawDataCnv_p3::transToPers(const CscRawData* trans, CscRawData_p3* pers, MsgStream &) 
{
    pers->m_amps        = trans->m_amps;
    pers->m_address     = trans->m_address;
    pers->m_id          = trans->m_id;
    pers->m_time        = trans->m_time;
    pers->m_rpuID       = trans->m_rpuID;
    pers->m_width       = trans->m_width;
    pers->m_isTimeComputed        = trans->m_isTimeComputed;
    pers->m_hashId        = trans->m_hashId;}

void
CscRawDataCnv_p3::persToTrans(const CscRawData_p3* pers, CscRawData* trans, MsgStream &) 
{
   trans->m_amps        = pers->m_amps;
   trans->m_address     = pers->m_address;
   trans->m_id          = pers->m_id;
   trans->m_time        = pers->m_time;
   trans->m_rpuID       = pers->m_rpuID;
   trans->m_width       = pers->m_width;
   trans->m_isTimeComputed        = pers->m_isTimeComputed;
   trans->m_hashId        = pers->m_hashId;
}
