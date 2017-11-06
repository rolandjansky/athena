/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/TgcRdo.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonEventAthenaPool/TgcRdo_p3.h"
#include "TgcRdoCnv_p3.h"
#include "TgcRawDataCnv_p3.h"

namespace {

union Errors_u {
  TgcRdo::Errors errors;
  uint32_t i;
};

union RodStatus_u {
  TgcRdo::RodStatus rodStatus;
  uint32_t i;
};

union LocalStatus_u {
  TgcRdo::LocalStatus localStatus;
  uint32_t i;
};

}

void
TgcRdoCnv_p3::transToPers(const TgcRdo* transColl, TgcRdo_p3* persColl, MsgStream &log) 
{
    if (log.level() <= MSG::DEBUG)
        log <<  MSG::DEBUG << " ***  Writing out TgcRdo P3" << endmsg;

    // Invoke vector converter from the base template
    TgcRdoCnv_p3_basetype::transToPers(transColl, persColl, log);

    persColl->m_version       = transColl->version();
    persColl->m_id            = transColl->identify();
    persColl->m_subDetectorId = transColl->subDetectorId();
    persColl->m_rodId         = transColl->rodId();
    persColl->m_triggerType   = transColl->triggerType();
    persColl->m_bcId          = transColl->bcId();
    persColl->m_l1Id          = transColl->l1Id();

    Errors_u errors_u;
    errors_u.i = 0;
    errors_u.errors = transColl->errors();
    persColl->m_errors        = errors_u.i;

    RodStatus_u rodStatus_u;
    rodStatus_u.rodStatus = transColl->rodStatus();
    persColl->m_rodStatus        = rodStatus_u.i;

    LocalStatus_u localStatus_u;
    localStatus_u.localStatus = transColl->localStatus();
    persColl->m_localStatus        = localStatus_u.i;

    persColl->m_orbit         = transColl->orbit();
}

void
TgcRdoCnv_p3::persToTrans(const TgcRdo_p3* persColl, TgcRdo* transColl, MsgStream &log)
{
    if (log.level() <= MSG::DEBUG)
        log << MSG::DEBUG  << " ***  Reading TgcRdo P3" << endmsg;

    // Invoke vector converter from the base template
    TgcRdoCnv_p3_basetype::persToTrans(persColl, transColl, log);

    transColl->setVersion      (persColl->m_version);
    transColl->setOnlineId(persColl->m_subDetectorId,
                           persColl->m_rodId);
    transColl->setTriggerType  (persColl->m_triggerType);
    transColl->setBcId         (persColl->m_bcId);
    transColl->setL1Id         (persColl->m_l1Id);
    transColl->setOrbit        (persColl->m_orbit);

    if(persColl->m_errors!=0) {transColl->setErrors       (persColl->m_errors);}
    transColl->setRodStatus    (persColl->m_rodStatus);
    transColl->setLocalStatus  (persColl->m_localStatus);
}

