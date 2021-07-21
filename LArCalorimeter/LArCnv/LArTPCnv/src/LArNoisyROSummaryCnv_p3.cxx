/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecEvent/LArNoisyROSummary.h"
#include "LArTPCnv/LArNoisyROSummaryCnv_p3.h"

void LArNoisyROSummaryCnv_p3::transToPers(const LArNoisyROSummary* trans, LArNoisyROSummary_p3* pers, MsgStream & log) const
{
  log << MSG::DEBUG << " in LArNoisyROSummaryCnv_p3::transToPers " << endmsg;

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

  pers->m_BadFEBFlaggedPartitions = trans->BadFEBFlaggedPartitions();
  pers->m_SatMediumFlaggedPartitions = trans->SatMediumFlaggedPartitions();
  pers->m_SatTightFlaggedPartitions = trans->SatTightFlaggedPartitions();
  pers->m_BadFEB_WFlaggedPartitions = trans->BadFEB_WFlaggedPartitions();  
  pers->m_MNBLooseFlaggedPartitions = trans->MNBLooseFlaggedPartitions();
  pers->m_MNBTightFlaggedPartitions = trans->MNBTightFlaggedPartitions();
  
}

void LArNoisyROSummaryCnv_p3::persToTrans(const LArNoisyROSummary_p3* pers, LArNoisyROSummary* trans, MsgStream & log ) const
{
  log << MSG::DEBUG << " in  LArNoisyROSummaryCnv_p3::persToTrans " << endmsg;
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

  trans->SetBadFEBFlaggedPartitions (pers->m_BadFEBFlaggedPartitions);
  trans->SetSatMediumFlaggedPartitions (pers->m_SatMediumFlaggedPartitions);
  trans->SetSatTightFlaggedPartitions (pers->m_SatTightFlaggedPartitions);
  trans->SetBadFEB_WFlaggedPartitions (pers->m_BadFEB_WFlaggedPartitions);
  trans->SetMNBLooseFlaggedPartitions (pers->m_MNBLooseFlaggedPartitions);
  trans->SetMNBTightFlaggedPartitions (pers->m_MNBTightFlaggedPartitions);

}
