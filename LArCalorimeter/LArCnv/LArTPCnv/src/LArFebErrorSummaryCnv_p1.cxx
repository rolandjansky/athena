/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "LArRawEvent/LArFebErrorSummary.h"
#undef private
#undef protected


#include "CaloIdentifier/CaloGain.h"
#include "LArTPCnv/LArFebErrorSummaryCnv_p1.h"

// LArFebErrorSummaryCnv_p1, used for T/P separation
// author H.Ma

void LArFebErrorSummaryCnv_p1::transToPers(const LArFebErrorSummary* trans, LArFebErrorSummary_p1* pers, MsgStream &/*log*/) 
{

  unsigned int sz = trans->m_feb_errors.size();

  pers->m_feb_errors.resize(sz);
  pers->m_feb_ids.resize(sz);

  std::map<unsigned int, uint16_t>::const_iterator it = trans->m_feb_errors.begin();
  std::map<unsigned int, uint16_t>::const_iterator it_e = trans->m_feb_errors.end();
  unsigned int i=0 ; 
  for (; it!=it_e;++it){
    pers->m_feb_ids[i]=(*it).first      ;
    pers->m_feb_errors[i]=(*it).second    ;
    ++i; 
  }

}

void LArFebErrorSummaryCnv_p1::persToTrans(const LArFebErrorSummary_p1* pers, LArFebErrorSummary* trans, MsgStream & log) 
{

  if( pers->m_feb_errors.size()!= pers->m_feb_ids.size() )
    {
      log<<MSG::ERROR<<" In LArFebErrorSummaryCnv_p1::persToTrans, size of ids and errors are not the same "<<endreq;
      return ; 
    }

  unsigned int sz = pers->m_feb_ids.size(); 

  // clear the internal map 
  trans->clear(); 

  for (unsigned int i = 0;i<sz;++i){
    if ( ! trans->set_feb_error(pers->m_feb_ids[i],pers->m_feb_errors[i]) )
      {
	log<<MSG::ERROR<<" In LArFebErrorSummaryCnv_p1::persToTrans, failed to add a feb_id, and error "<<pers->m_feb_ids[i]
	   <<" " << pers->m_feb_errors[i] <<endreq;
      }
  }


}

