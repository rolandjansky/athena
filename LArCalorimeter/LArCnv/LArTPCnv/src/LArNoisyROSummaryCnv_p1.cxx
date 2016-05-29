/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecEvent/LArNoisyROSummary.h"
#include "LArTPCnv/LArNoisyROSummaryCnv_p1.h"

void LArNoisyROSummaryCnv_p1::transToPers(const LArNoisyROSummary* trans, LArNoisyROSummary_p1* pers, MsgStream & log) 
{
  log << MSG::DEBUG << " in LArNoisyROSummaryCnv_p1::transToPers " << endreq;

  size_t sfebs = trans->get_noisy_febs().size();
  pers->m_noisy_febs.reserve(sfebs);
  for (const HWIdentifier& hwid : trans->get_noisy_febs())
  {
    pers->m_noisy_febs.push_back (hwid.get_identifier32().get_compact());
  }

  pers->m_noisy_preamps.clear();
  pers->m_noisy_preamps.reserve(trans->get_noisy_preamps().size());
  for (const std::pair<HWIdentifier, std::vector<int> >& p :
         trans->get_noisy_preamps())
  {
    pers->m_noisy_preamps.emplace_back (p.first.get_identifier32().get_compact(),
                                        p.second);
  }
}

void LArNoisyROSummaryCnv_p1::persToTrans(const LArNoisyROSummary_p1* pers, LArNoisyROSummary* trans, MsgStream & log ) 
{
  log << MSG::DEBUG << " in  LArNoisyROSummaryCnv_p1::persToTrans " << endreq;
  trans->clear();

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
}
