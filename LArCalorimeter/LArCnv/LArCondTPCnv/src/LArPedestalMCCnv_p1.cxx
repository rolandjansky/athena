/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArPedestalMC.h"
#include "LArCondTPCnv/LArPedestalMCCnv_p1.h"

void
LArPedestalMCCnv_p1::persToTrans(const LArPedMCPersType* persObj, 
                                 LArPedMCTransType* transObj, 
                                 MsgStream & /*log*/)
{
    // Copy conditions
    transObj->set (persObj->m_vPedestal, persObj->m_vPedestalRMS);
}


        
    
void
LArPedestalMCCnv_p1::transToPers(const LArPedMCTransType* transObj, 
                                 LArPedMCPersType* persObj, 
                                 MsgStream & /*log*/) 
{
    // Copy conditions
    persObj->m_vPedestal          = transObj->m_vPedestal;
    persObj->m_vPedestalRMS       = transObj->m_vPedestalRMS;
}
