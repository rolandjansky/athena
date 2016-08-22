/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/TgcRdo.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonEventAthenaPool/TgcRdo_p1.h"
#include "TgcRdoCnv_p1.h"
#include "TgcRawDataCnv_p1.h"

void
TgcRdoCnv_p1::transToPers(const TgcRdo* /*transColl*/, TgcRdo_p1* /*persColl*/, MsgStream &log) 
{
    log << MSG::ERROR << "TgcRdoCnv_p1::transToPers should not be called. _p2 exists!" << endmsg;
}

void
TgcRdoCnv_p1::persToTrans(const TgcRdo_p1* persColl, TgcRdo* transColl, MsgStream &log) 
{
    if (log.level() <= MSG::DEBUG)
        log << MSG::DEBUG  << " ***  Reading TgcRdo P1" << endmsg;
//    log << MSG::INFO  << " ***  Reading TgcRdo P1" << endmsg;

    // Invoke vector converter from the base template
    TgcRdoCnv_p1_basetype::persToTrans(persColl, transColl, log);

    transColl->setOnlineId(persColl->subDetectorId(),
                           persColl->rodId());
    transColl->setTriggerType(persColl->triggerType());
    transColl->setBcId(persColl->bcId());
    transColl->setL1Id(persColl->l1Id());
    transColl->setVersion(0);
    
    if (log.level() <= MSG::DEBUG)
        std::cout << "After conversion " << *transColl << std::endl;
}

