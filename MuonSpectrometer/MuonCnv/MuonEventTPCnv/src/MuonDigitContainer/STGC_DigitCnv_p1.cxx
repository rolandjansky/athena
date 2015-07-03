/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   STGC_DigitCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonDigitContainer/sTgcDigit.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonDigitContainer/STGC_DigitCnv_p1.h"
/*******************************************************************************/
void STGC_DigitCnv_p1::persToTrans( const Muon::STGC_Digit_p1 *persObj, sTgcDigit *transObj,MsgStream & /**log*/ ) 
{
  transObj->m_muonId                  = persObj->m_muonId;
  transObj->m_bcTag                   = persObj->m_bcTag;
  transObj->m_charge                  = persObj->m_charge;
}
/*******************************************************************************/
void STGC_DigitCnv_p1::transToPers( const sTgcDigit *transObj, Muon::STGC_Digit_p1 *persObj, MsgStream & /*log*/ )
{
  persObj->m_muonId                   = transObj->m_muonId.get_identifier32().get_compact();
  persObj->m_bcTag                    = (uint16_t)(transObj->m_bcTag);
  persObj->m_charge                   = transObj->m_charge;
}
/*******************************************************************************/

