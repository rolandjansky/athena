/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/MdtAmtHit.h"
#include "MdtAmtHitCnv_p1.h"


void
MdtAmtHitCnv_p1::transToPers(const MdtAmtHit* trans, MdtAmtHit_p1* pers, MsgStream &) 
{
    pers->m_tdcId       = trans->tdcId();
    pers->m_channelId   = trans->channelId();
    pers->m_leading   	= trans->leading(); 
    pers->m_coarse    	= trans->coarse();
    pers->m_fine	= trans->fine();
    pers->m_width      	= trans->width();
    pers->m_isMasked   	= trans->isMasked(); 
    pers->m_dataWords   = *trans->dataWords();
}


void
MdtAmtHitCnv_p1::persToTrans(const MdtAmtHit_p1* pers, MdtAmtHit* trans, MsgStream &) 
{
  *trans = MdtAmtHit (pers->m_tdcId, pers->m_channelId, pers->m_isMasked);
  trans->setValues (pers->m_coarse, pers->m_fine, pers->m_width);
  trans->setLeading (pers->m_leading);
  for (uint32_t w : pers->m_dataWords)
    trans->addData (w);
}
