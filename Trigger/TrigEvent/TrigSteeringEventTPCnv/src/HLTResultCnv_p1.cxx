/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/HLTResult.h"
#include "TrigSteeringEventTPCnv/HLTResult_p1.h"
#include "TrigSteeringEventTPCnv/HLTResultCnv_p1.h"
#include "AthenaKernel/getThinningCache.h"
#include "AthenaKernel/ThinningCache.h"
#include "AthenaKernel/ITrigNavigationThinningSvc.h"
#include "GaudiKernel/ThreadLocalContext.h"

using namespace HLT;

void HLTResultCnv_p1::transToPers(const HLT::HLTResult* trans, 
                                     HLTResult_p1* pers, MsgStream &log )
{

  log << MSG::DEBUG << "HLTResultCnv_p1::transToPers called " << endmsg;
  
  pers->m_headerResult         = trans->m_headerResult         ;
  pers->m_chainsResult         = trans->m_chainsResult         ;

  const SG::ThinningCache* cache = SG::getThinningCache();
  if (cache && cache->trigNavigationThinningSvc()) {
    const EventContext& ctx = Gaudi::Hive::currentContext();
    std::vector< unsigned int > temp;
    if (cache->trigNavigationThinningSvc()->doSlimming (ctx, temp).isFailure()) {
      log << MSG::ERROR << "HLTResultCnv_p1::transToPers: doSlimming failed." << endmsg;
    }
    pers->m_navigationResult = temp;
  }
  else {
    pers->m_navigationResult     = trans->m_navigationResult;
  }

}

void HLTResultCnv_p1::persToTrans(const HLT::HLTResult_p1* pers, 
                                     HLTResult* trans, MsgStream &log )
{

  log << MSG::DEBUG << "HLTResultCnv_p1::persToTrans called " << endmsg;

  trans->m_headerResult         = pers->m_headerResult         ;
  trans->m_chainsResult         = pers->m_chainsResult         ;
  trans->m_navigationResult     = pers->m_navigationResult     ;

}
