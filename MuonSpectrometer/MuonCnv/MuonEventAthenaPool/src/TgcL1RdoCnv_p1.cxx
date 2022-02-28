/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/TgcL1Rdo.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonEventAthenaPool/TgcL1Rdo_p1.h"
#include "TgcL1RdoCnv_p1.h"
#include "TgcL1RawDataCnv_p1.h"

namespace {

union Errors_u {
  TgcL1Rdo::Errors errors;
  uint32_t i;
};

union SRodStatus_u {
  TgcL1Rdo::SRodStatus srodStatus;
  uint32_t i;
};

union LocalStatus_u {
  TgcL1Rdo::LocalStatus localStatus;
  uint32_t i;
};

}

void
TgcL1RdoCnv_p1::transToPers(const TgcL1Rdo* transColl, TgcL1Rdo_p1* persColl, MsgStream &log) 
{
    if (log.level() <= MSG::DEBUG)
        log <<  MSG::DEBUG << " ***  Writing out TgcL1Rdo P1" << endmsg;

    // Invoke vector converter from the base template
    TgcL1RdoCnv_p1_basetype::transToPers(transColl, persColl, log);

    persColl->m_version       = transColl->version();
    persColl->m_id            = transColl->identify();
    persColl->m_subDetectorId = transColl->subDetectorId();
    persColl->m_srodId        = transColl->srodId();
    persColl->m_triggerType   = transColl->triggerType();
    persColl->m_bcId          = transColl->bcId();
    persColl->m_l1Id          = transColl->l1Id();

    Errors_u errors_u{};
    errors_u.i = 0;
    errors_u.errors = transColl->errors();
    persColl->m_errors        = errors_u.i;

    SRodStatus_u srodStatus_u{};
    srodStatus_u.srodStatus = transColl->srodStatus();
    persColl->m_srodStatus        = srodStatus_u.i;

    LocalStatus_u localStatus_u{};
    localStatus_u.localStatus = transColl->localStatus();
    persColl->m_localStatus        = localStatus_u.i;

    persColl->m_orbit         = transColl->orbit();
}

void
TgcL1RdoCnv_p1::persToTrans(const TgcL1Rdo_p1* persColl, TgcL1Rdo* transColl, MsgStream &log)
{
    if (log.level() <= MSG::DEBUG)
        log << MSG::DEBUG  << " ***  Reading TgcL1Rdo P1" << endmsg;

    // Invoke vector converter from the base template
    TgcL1RdoCnv_p1_basetype::persToTrans(persColl, transColl, log);

    transColl->setVersion      (persColl->m_version);
    transColl->setOnlineId(persColl->m_subDetectorId,
                           persColl->m_srodId);
    transColl->setTriggerType  (persColl->m_triggerType);
    transColl->setBcId         (persColl->m_bcId);
    transColl->setL1Id         (persColl->m_l1Id);
    transColl->setOrbit        (persColl->m_orbit);

    if(persColl->m_errors!=0) {transColl->setErrors       (persColl->m_errors);}
    transColl->setSRodStatus    (persColl->m_srodStatus);
    transColl->setLocalStatus  (persColl->m_localStatus);
}

