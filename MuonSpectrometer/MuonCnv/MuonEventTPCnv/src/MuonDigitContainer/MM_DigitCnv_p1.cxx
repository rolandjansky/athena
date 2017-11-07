/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MM_DigitCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "MuonDigitContainer/MmDigit.h"
#include "MuonEventTPCnv/MuonDigitContainer/MM_DigitCnv_p1.h"

void MM_DigitCnv_p1::persToTrans( const Muon::MM_Digit_p1 *persObj, MmDigit *transObj,MsgStream & /**log*/ )
{

  std::vector<float> tmp_trigtime(1, persObj->m_stripTimeForTrigger);
  std::vector<int>   tmp_trigpos(1, persObj->m_stripPositionForTrigger);
  std::vector<float> tmp_trigcharge(1, persObj->m_stripChargeForTrigger);
  std::vector<int>   tmp_trigmmfe(1, persObj->m_MMFE_VMM_idForTrigger);
  std::vector<int>   tmp_trigvmm(1, persObj->m_VMM_idForTrigger);

  *transObj = MmDigit (Identifier(persObj->m_muonId),
                       persObj->m_stripResponseTime,
                       persObj->m_stripResponsePosition,
                       persObj->m_stripResponseCharge,
                       persObj->m_chipResponseTime,
                       persObj->m_chipResponsePosition,
                       persObj->m_chipResponseCharge,
                       tmp_trigtime,
                       tmp_trigpos,
                       tmp_trigcharge,
                       tmp_trigmmfe,
                       tmp_trigvmm
                       );
}

void MM_DigitCnv_p1::transToPers( const MmDigit *transObj, Muon::MM_Digit_p1 *persObj, MsgStream & /*log*/ )
{
  persObj->m_muonId                  = transObj->identify().get_identifier32().get_compact();

  /** strip response */
  persObj->m_stripResponseTime       = transObj->stripResponseTime();
  persObj->m_stripResponseCharge     = transObj->stripResponseCharge();
  persObj->m_stripResponsePosition   = transObj->stripResponsePosition();
  /** VMM chip response */
  persObj->m_chipResponseTime        = transObj->chipResponseTime();
  persObj->m_chipResponseCharge      = transObj->chipResponseCharge();
  persObj->m_chipResponsePosition    = transObj->chipResponsePosition();

  persObj->m_stripTimeForTrigger     = (transObj->stripTimeForTrigger()).front();
  persObj->m_stripPositionForTrigger = (transObj->stripPositionForTrigger()).front();
  persObj->m_stripChargeForTrigger   = (transObj->stripChargeForTrigger()).front();
  persObj->m_MMFE_VMM_idForTrigger   = (transObj->MMFE_VMM_idForTrigger()).front();
  persObj->m_VMM_idForTrigger        = (transObj->VMM_idForTrigger()).front();
}


