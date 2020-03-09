/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTPCnv/TBTDCCnv_p1.h"
#include "TBEvent/TBTDC.h"
#include "Identifier/Identifier.h"
#include "TBTPCnv/TBTDC_p1.h"



void
TBTDCCnv_p1::persToTrans(const TBTDC_p1* pers, 
                                     TBTDC* trans, MsgStream &/*log*/) const
{
  *trans = TBTDC (pers->m_tdc,
                  pers->m_tdcmin,
                  pers->m_scale,
                  pers->m_phase);
}


void
TBTDCCnv_p1::transToPers(const TBTDC* trans, 
                                     TBTDC_p1* pers, MsgStream &/*log*/) const
{
  pers->m_tdc    = trans->tdc();
  pers->m_tdcmin = trans->tdcmin();
  pers->m_scale  = trans->scale();
  pers->m_phase  = trans->phase();
}

