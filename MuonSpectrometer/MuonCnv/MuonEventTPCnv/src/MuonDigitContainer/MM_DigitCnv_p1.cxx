/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MM_DigitCnv_p3.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonDigitContainer/MmDigit.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonDigitContainer/MM_DigitCnv_p1.h"

void MM_DigitCnv_p1::persToTrans( const Muon::MM_Digit_p1 *persObj, MmDigit *transObj,MsgStream & /**log*/ ) 
{
  // std::cout << "MM_DigitCnv_p1::persToTrans" << std::endl;
  
  transObj->m_muonId                  = persObj->m_muonId;
  /** strip response */
  transObj->m_stripResponseTime       = persObj->m_stripResponseTime;
  transObj->m_stripResponseCharge     = persObj->m_stripResponseCharge;
  transObj->m_stripResponsePosition   = persObj->m_stripResponsePosition;
  /** VMM chip response */
  transObj->m_chipResponseTime       = persObj->m_chipResponseTime;
  transObj->m_chipResponseCharge     = persObj->m_chipResponseCharge;
  transObj->m_chipResponsePosition   = persObj->m_chipResponsePosition;
     
  transObj->m_stripForTrigger         = persObj->m_stripForTrigger;   
  transObj->m_stripTimeForTrigger     = persObj->m_stripTimeForTrigger;   
}

void MM_DigitCnv_p1::transToPers( const MmDigit *transObj, Muon::MM_Digit_p1 *persObj, MsgStream & /*log*/ )
{
  persObj->m_muonId                   = transObj->m_muonId.get_identifier32().get_compact();

  /** strip response */
  persObj->m_stripResponseTime       = transObj->m_stripResponseTime;
  persObj->m_stripResponseCharge     = transObj->m_stripResponseCharge;
  persObj->m_stripResponsePosition   = transObj->m_stripResponsePosition;
  /** VMM chip response */
  persObj->m_chipResponseTime       = transObj->m_chipResponseTime;
  persObj->m_chipResponseCharge     = transObj->m_chipResponseCharge;
  persObj->m_chipResponsePosition   = transObj->m_chipResponsePosition;

  persObj->m_stripForTrigger          = transObj->m_stripForTrigger;   
  persObj->m_stripTimeForTrigger      = transObj->m_stripTimeForTrigger;   
}


