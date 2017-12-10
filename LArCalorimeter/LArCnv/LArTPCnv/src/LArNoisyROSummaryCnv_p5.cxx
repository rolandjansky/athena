/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecEvent/LArNoisyROSummary.h"
#include "LArTPCnv/LArNoisyROSummaryCnv_p5.h"

void LArNoisyROSummaryCnv_p5::transToPers(const LArNoisyROSummary* trans, LArNoisyROSummary_p5* pers, MsgStream & log) 
{
  log << MSG::DEBUG << " in LArNoisyROSummaryCnv_p5::transToPers " << endreq;

  size_t sfebs = trans->get_noisy_febs().size();
  pers->m_noisy_febs.reserve(sfebs);
  for (const HWIdentifier& hwid : trans->get_noisy_febs())
  {
    pers->m_noisy_febs.push_back (hwid.get_identifier32().get_compact());
  }

  size_t smnbtightfebs = trans->get_MNBTight_febs().size();
  pers->m_MNBTight_febs.reserve(smnbtightfebs);
  for (const HWIdentifier& hwid : trans->get_MNBTight_febs())
  {
    pers->m_MNBTight_febs.push_back (hwid.get_identifier32().get_compact());
  }

  size_t smnbloosefebs = trans->get_MNBLoose_febs().size();
  pers->m_MNBLoose_febs.reserve(smnbloosefebs);
  for (const HWIdentifier& hwid : trans->get_MNBLoose_febs())
  {
    pers->m_MNBLoose_febs.push_back (hwid.get_identifier32().get_compact());
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
  pers->m_BadFEB_WFlaggedPartitions = trans->BadFEB_WFlaggedPartitions() ;
  pers->m_MNBLooseFlaggedPartitions = trans->MNBLooseFlaggedPartitions();
  pers->m_MNBTightFlaggedPartitions = trans->MNBTightFlaggedPartitions();
  pers->m_MNBTightFlaggedPartitions_PsVeto = trans->MNBTightFlaggedPartitions_PsVeto();
  
}

void LArNoisyROSummaryCnv_p5::persToTrans(const LArNoisyROSummary_p5* pers, LArNoisyROSummary* trans, MsgStream & log ) 
{
  log << MSG::DEBUG << " in  LArNoisyROSummaryCnv_p5::persToTrans " << endreq;
  trans->clear();

  size_t sfebs =  pers->m_noisy_febs.size();
  for ( size_t i = 0; i < sfebs; i++)
  {
    HWIdentifier febId(Identifier32(pers->m_noisy_febs[i]));
    trans->add_noisy_feb(febId);
  }

  size_t smnbtightfebs =  pers->m_MNBTight_febs.size();
  for ( size_t i = 0; i < smnbtightfebs; i++)
    {
      HWIdentifier febId(Identifier32(pers->m_MNBTight_febs[i]));
      trans->add_MNBTight_feb(febId);
    }

  size_t smnbloosefebs =  pers->m_MNBLoose_febs.size();
  for ( size_t i = 0; i < smnbloosefebs; i++)
    {
      HWIdentifier febId(Identifier32(pers->m_MNBLoose_febs[i]));
      trans->add_MNBLoose_feb(febId);
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
  trans->SetMNBTightFlaggedPartitions_PsVeto (pers->m_MNBTightFlaggedPartitions_PsVeto);

}
