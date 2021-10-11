/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawEvent/LArFebErrorSummary.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArTPCnv/LArFebErrorSummaryCnv_p1.h"

// LArFebErrorSummaryCnv_p1, used for T/P separation
// author H.Ma

void LArFebErrorSummaryCnv_p1::transToPers(const LArFebErrorSummary* trans, LArFebErrorSummary_p1* pers, MsgStream &/*log*/) const
{
  unsigned int sz = trans->get_all_febs().size();

  pers->m_feb_errors.clear();
  pers->m_feb_ids.clear();
  pers->m_feb_errors.reserve(sz);
  pers->m_feb_ids.reserve(sz);

  for (const std::pair<const unsigned int, uint16_t>& p :
         trans->get_all_febs())
  {
    pers->m_feb_ids.push_back (p.first);
    pers->m_feb_errors.push_back (p.second);
  }
}

void LArFebErrorSummaryCnv_p1::persToTrans(const LArFebErrorSummary_p1* pers, LArFebErrorSummary* trans, MsgStream & log) const
{

  if( pers->m_feb_errors.size()!= pers->m_feb_ids.size() )
    {
      log<<MSG::ERROR<<" In LArFebErrorSummaryCnv_p1::persToTrans, size of ids and errors are not the same "<<endmsg;
      return ; 
    }

  unsigned int sz = pers->m_feb_ids.size(); 

  // clear the internal map 
  trans->clear(); 

  for (unsigned int i = 0;i<sz;++i){
    if ( ! trans->set_feb_error(pers->m_feb_ids[i],pers->m_feb_errors[i]) )
      {
	log<<MSG::ERROR<<" In LArFebErrorSummaryCnv_p1::persToTrans, failed to add a feb_id, and error "<<pers->m_feb_ids[i]
	   <<" " << pers->m_feb_errors[i] <<endmsg;
      }
  }


}

