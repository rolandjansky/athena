/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "MuonRDO/TgcRdo.h"
#undef private
#undef protected

#include "GaudiKernel/MsgStream.h"
#include "MuonEventAthenaPool/TgcRdo_p2.h"
#include "TgcRdoCnv_p2.h"
#include "TgcRawDataCnv_p2.h"

void
TgcRdoCnv_p2::transToPers(const TgcRdo* transColl, TgcRdo_p2* persColl, MsgStream &log) 
{
    if (log.level() <= MSG::DEBUG)
        log <<  MSG::DEBUG << " ***  Writing out TgcRdo P2" << endreq;

    // Invoke vector converter from the base template
    TgcRdoCnv_p2_basetype::transToPers(transColl, persColl, log);

    persColl->m_version       = transColl->m_version;
    persColl->m_id            = transColl->m_id;
    persColl->m_subDetectorId = transColl->m_subDetectorId;
    persColl->m_rodId         = transColl->m_rodId;
    persColl->m_triggerType   = transColl->m_triggerType;
    persColl->m_bcId          = transColl->m_bcId;
    persColl->m_l1Id          = transColl->m_l1Id;
    persColl->m_errors        = *(TgcRdo_const_pointer_cast<uint32_t>(&(transColl->m_errors))); 
    persColl->m_rodStatus     = *(TgcRdo_const_pointer_cast<uint32_t>(&(transColl->m_rodStatus)));
    persColl->m_localStatus   = *(TgcRdo_const_pointer_cast<uint32_t>(&(transColl->m_localStatus)));
    persColl->m_orbit         = transColl->m_orbit;
}

void
TgcRdoCnv_p2::persToTrans(const TgcRdo_p2* persColl, TgcRdo* transColl, MsgStream &log)
{
    if (log.level() <= MSG::DEBUG)
        log << MSG::DEBUG  << " ***  Reading TgcRdo P2" << endreq;

    // Invoke vector converter from the base template
    TgcRdoCnv_p2_basetype::persToTrans(persColl, transColl, log);

    transColl->m_version       = persColl->m_version;
    transColl->m_id            = persColl->m_id;
    transColl->m_subDetectorId = persColl->m_subDetectorId;
    transColl->m_rodId         = persColl->m_rodId;
    transColl->m_triggerType   = persColl->m_triggerType;
    transColl->m_bcId          = persColl->m_bcId;
    transColl->m_l1Id          = persColl->m_l1Id;
    transColl->m_errors        = *(TgcRdo_const_pointer_cast<TgcRdo::Errors>(&(persColl->m_errors)));
    transColl->m_rodStatus     = *(TgcRdo_const_pointer_cast<TgcRdo::RodStatus>(&(persColl->m_rodStatus)));
    transColl->m_localStatus   = *(TgcRdo_const_pointer_cast<TgcRdo::LocalStatus>(&(persColl->m_localStatus)));
    transColl->m_orbit         = persColl->m_orbit;
}

