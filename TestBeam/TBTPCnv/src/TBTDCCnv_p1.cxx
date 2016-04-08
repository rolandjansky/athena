/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTPCnv/TBTDCCnv_p1.h"
#define private public
#define protected public
#include "TBEvent/TBTDC.h"

#undef private
#undef protected
#include "Identifier/Identifier.h"
#include "TBTPCnv/TBTDC_p1.h"



void
TBTDCCnv_p1::persToTrans(const TBTDC_p1* pers, 
                                     TBTDC* trans, MsgStream &/*log*/)
{

  trans->m_tdc    = pers->m_tdc;
  trans->m_tdcmin = pers->m_tdcmin;
  trans->m_scale  = pers->m_scale;
  trans->m_phase  = pers->m_phase;

}


void
TBTDCCnv_p1::transToPers(const TBTDC* trans, 
                                     TBTDC_p1* pers, MsgStream &/*log*/)
{

  pers->m_tdc    = trans->m_tdc;
  pers->m_tdcmin = trans->m_tdcmin;
  pers->m_scale  = trans->m_scale;
  pers->m_phase  = trans->m_phase;

}

