/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "LArRecEvent/LArNoisyROSummary.h"
#undef private
#undef protected

#include "LArTPCnv/LArNoisyROSummaryCnv_p1.h"

void LArNoisyROSummaryCnv_p1::transToPers(const LArNoisyROSummary* trans, LArNoisyROSummary_p1* pers, MsgStream & log) 
{
  typedef std::vector< std::pair<HWIdentifier, std::vector<int> > > PAcont;
  log << MSG::DEBUG << " in LArNoisyROSummaryCnv_p1::transToPers " << endreq;

  size_t sfebs = trans->m_noisy_febs.size();
  pers->m_noisy_febs.resize(sfebs);
  for ( size_t i = 0; i < sfebs; i++ )
  {
    pers->m_noisy_febs[i] = trans->m_noisy_febs[i].get_identifier32().get_compact();
  }

  pers->m_noisy_preamps.clear();
  pers->m_noisy_preamps.reserve(trans->m_noisy_preamps.size());
  for ( PAcont::const_iterator it = trans->m_noisy_preamps.begin(); it != trans->m_noisy_preamps.end(); it++)
  {
    pers->m_noisy_preamps.push_back(std::make_pair(it->first.get_identifier32().get_compact(), it->second));
  }


}

void LArNoisyROSummaryCnv_p1::persToTrans(const LArNoisyROSummary_p1* pers, LArNoisyROSummary* trans, MsgStream & log ) 
{
  log << MSG::DEBUG << " in  LArNoisyROSummaryCnv_p1::persToTrans " << endreq;
  trans->m_noisy_febs.clear();
  trans->m_noisy_preamps.clear();

  size_t sfebs =  pers->m_noisy_febs.size();
  for ( size_t i = 0; i < sfebs; i++)
  {
    HWIdentifier febId(Identifier32(pers->m_noisy_febs[i]));
    trans->add_noisy_feb(febId);
  }

  size_t spas =  pers->m_noisy_preamps.size();
  for ( size_t i = 0; i < spas; i++)
  {
    size_t schan = pers->m_noisy_preamps[i].second.size();
    for ( size_t j = 0; j < schan; j++ )
    {
      HWIdentifier febId(Identifier32(pers->m_noisy_preamps[i].first));
      trans->add_noisy_preamp(febId,pers->m_noisy_preamps[i].second[j]);
    }
  }

  trans->m_BadFEBFlaggedPartitions = 0;
  trans->m_SatMediumFlaggedPartitions = 0;
  trans->m_SatTightFlaggedPartitions = 0;
  trans->m_BadFEB_WFlaggedPartitions = 0;


}
