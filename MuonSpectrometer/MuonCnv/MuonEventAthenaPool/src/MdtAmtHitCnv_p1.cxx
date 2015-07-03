/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "MuonRDO/MdtAmtHit.h"
#undef private
#undef protected

#include "MdtAmtHitCnv_p1.h"


void
MdtAmtHitCnv_p1::transToPers(const MdtAmtHit* trans, MdtAmtHit_p1* pers, MsgStream &) 
{
    pers->m_tdcId       = trans->tdcId();
    pers->m_channelId   = trans->channelId();
    pers->m_leading   	= trans->leading(); 
    pers->m_coarse    	= trans->coarse();
    pers->m_fine	    = trans->fine();
    pers->m_width      	= trans->width();
    pers->m_isMasked   	= trans->isMasked(); 

    //check that vector exists
    if (trans->dataWords()!=0){
        pers->m_dataWords.reserve(trans->dataWords()->size());
        std::vector<uint32_t>::const_iterator it    = trans->dataWords()->begin();
        std::vector<uint32_t>::const_iterator itEnd = trans->dataWords()->end();
        std::copy(it, itEnd, pers->m_dataWords.begin() );
    }
}


void
MdtAmtHitCnv_p1::persToTrans(const MdtAmtHit_p1* pers, MdtAmtHit* trans, MsgStream &) 
{
   trans->m_tdcId 	    = pers->m_tdcId;
   trans->m_channelId  	= pers->m_channelId;
   trans->m_leading   	= pers->m_leading;
   trans->m_coarse    	= pers->m_coarse;
   trans->m_fine    	= pers->m_fine;
   trans->m_width    	= pers->m_width;
   trans->m_isMasked    = pers->m_isMasked;
   trans->p_dataWords->assign (pers->m_dataWords.begin(), pers->m_dataWords.end());
}
