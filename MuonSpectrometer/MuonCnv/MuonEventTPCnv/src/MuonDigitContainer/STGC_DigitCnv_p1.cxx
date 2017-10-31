/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   STGC_DigitCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "MuonDigitContainer/sTgcDigit.h"
#include "MuonEventTPCnv/MuonDigitContainer/STGC_DigitCnv_p1.h"
/*******************************************************************************/
void STGC_DigitCnv_p1::persToTrans( const Muon::STGC_Digit_p1 *persObj, sTgcDigit *transObj,MsgStream & /**log*/ ) 
{
  transObj->setID (Identifier (persObj->m_muonId));
  transObj->set_bcTag  (persObj->m_bcTag);
  transObj->set_charge (persObj->m_charge);
  transObj->set_time(persObj->m_time);
  transObj->set_isDead(persObj->m_isDead);
  transObj->set_isPileup(persObj->m_isPileup);
}
/*******************************************************************************/
void STGC_DigitCnv_p1::transToPers( const sTgcDigit *transObj, Muon::STGC_Digit_p1 *persObj, MsgStream & /*log*/ )
{
  persObj->m_muonId                   = transObj->identify().get_identifier32().get_compact();
  persObj->m_bcTag                    = (uint16_t)(transObj->bcTag());
  persObj->m_charge                   = transObj->charge();
  persObj->m_time                     = transObj->time();
  persObj->m_isDead                   = transObj->isDead();
  persObj->m_isPileup                 = transObj->isPileup();
}
/*******************************************************************************/

