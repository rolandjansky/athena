/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MM_DigitCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#include "MuonDigitContainer/MmDigit.h"
#include "MuonEventTPCnv/MuonDigitContainer/MM_DigitCnv_p2.h"

void MM_DigitCnv_p2::persToTrans( const Muon::MM_Digit_p2 *persObj, MmDigit *transObj,MsgStream & /**log*/ ) 
{
  *transObj = MmDigit (Identifier(persObj->m_muonId),
                       persObj->m_stripResponseTime,
                       persObj->m_stripResponsePosition,
                       persObj->m_stripResponseCharge,
                       persObj->m_chipResponseTime,
                       persObj->m_chipResponsePosition,
                       persObj->m_chipResponseCharge,
                       persObj->m_stripTimeForTrigger,
                       persObj->m_stripPositionForTrigger,
                       persObj->m_stripChargeForTrigger,
                       persObj->m_MMFE_VMM_idForTrigger,
                       persObj->m_VMM_idForTrigger);
}

void MM_DigitCnv_p2::transToPers( const MmDigit *transObj, Muon::MM_Digit_p2 *persObj, MsgStream & /*log*/ )
{
  persObj->m_muonId                   = transObj->identify().get_identifier32().get_compact();

  /** strip response */
  persObj->m_stripResponseTime       = transObj->stripResponseTime();
  persObj->m_stripResponseCharge     = transObj->stripResponseCharge();
  persObj->m_stripResponsePosition   = transObj->stripResponsePosition();
  /** VMM chip response */
  persObj->m_chipResponseTime       = transObj->chipResponseTime();
  persObj->m_chipResponseCharge     = transObj->chipResponseCharge();
  persObj->m_chipResponsePosition   = transObj->chipResponsePosition();

  persObj->m_stripTimeForTrigger    = transObj->stripTimeForTrigger();
  persObj->m_stripPositionForTrigger= transObj->stripPositionForTrigger();
  persObj->m_stripChargeForTrigger  = transObj->stripChargeForTrigger();

  persObj->m_MMFE_VMM_idForTrigger  = transObj->MMFE_VMM_idForTrigger();
  persObj->m_VMM_idForTrigger       = transObj->VMM_idForTrigger();   
}


