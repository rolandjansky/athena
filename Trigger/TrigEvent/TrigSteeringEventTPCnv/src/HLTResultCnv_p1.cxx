/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/HLTResult.h"
#include "TrigSteeringEventTPCnv/HLTResult_p1.h"
#include "TrigSteeringEventTPCnv/HLTResultCnv_p1.h"

using namespace HLT;

void HLTResultCnv_p1::transToPers(const HLT::HLTResult* trans, 
                                     HLTResult_p1* pers, MsgStream &log )
{

  log << MSG::DEBUG << "HLTResultCnv_p1::transToPers called " << endreq;
  
  pers->m_headerResult         = trans->m_headerResult         ;
  pers->m_chainsResult         = trans->m_chainsResult         ;
  pers->m_navigationResult     = trans->m_navigationResult     ;

}

void HLTResultCnv_p1::persToTrans(const HLT::HLTResult_p1* pers, 
                                     HLTResult* trans, MsgStream &log )
{

  log << MSG::DEBUG << "HLTResultCnv_p1::persToTrans called " << endreq;

  trans->m_headerResult         = pers->m_headerResult         ;
  trans->m_chainsResult         = pers->m_chainsResult         ;
  trans->m_navigationResult     = pers->m_navigationResult     ;

}
