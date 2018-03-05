/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecEvent/LArNoisyROSummary.h"


LArNoisyROSummary::LArNoisyROSummary():
  m_BadFEBFlaggedPartitions(0),
  m_BadFEB_WFlaggedPartitions(0),
  m_SatMediumFlaggedPartitions(0), 
  m_SatTightFlaggedPartitions(0),
  m_MNBLooseFlaggedPartitions(0),
  m_MNBTightFlaggedPartitions(0),
  m_MNBTightFlaggedPartitions_PsVeto(0)
{
}

LArNoisyROSummary::~LArNoisyROSummary()
{
}

void LArNoisyROSummary::clear()
{
  m_noisy_febs.clear();
  m_MNBTight_febs.clear();
  m_MNBLoose_febs.clear();
  m_noisy_preamps.clear();
  m_BadFEBFlaggedPartitions = 0; 
  m_BadFEB_WFlaggedPartitions = 0;
  m_SatMediumFlaggedPartitions = 0;
  m_SatTightFlaggedPartitions = 0;
  m_MNBLooseFlaggedPartitions = 0;
  m_MNBTightFlaggedPartitions = 0;
  m_MNBTightFlaggedPartitions_PsVeto = 0;
}


void LArNoisyROSummary::set_noisy_febs(const std::vector<HWIdentifier>& badfebs)
{
  m_noisy_febs = badfebs;
}


void LArNoisyROSummary::add_noisy_feb(HWIdentifier febid)
{
  m_noisy_febs.push_back(febid);
}

void LArNoisyROSummary::set_MNBTight_febs(const std::vector<HWIdentifier>& badfebs)
{
     m_MNBTight_febs = badfebs;
}

void LArNoisyROSummary::add_MNBTight_feb(HWIdentifier febid)
{
     m_MNBTight_febs.push_back(febid);
}

void LArNoisyROSummary::set_MNBLoose_febs(const std::vector<HWIdentifier>& badfebs)
{
     m_MNBLoose_febs = badfebs;
}

void LArNoisyROSummary::add_MNBLoose_feb(HWIdentifier febid)
{
     m_MNBLoose_febs.push_back(febid);
}

void LArNoisyROSummary::set_noisy_preamps(const std::vector< std::pair<HWIdentifier, std::vector<int> > >& badPAs)
{
  m_noisy_preamps = badPAs;
}

void LArNoisyROSummary::add_noisy_preamp(HWIdentifier febid, int channel)
{
  // search the vector for the corresponding FEB id NB: the vector should be short
  // so this should not be slow
  bool found = false;
  for (size_t i=0; i<m_noisy_preamps.size(); i++)
  {
    if ( m_noisy_preamps[i].first == febid )
    {
      found = true;
      m_noisy_preamps[i].second.push_back(channel);
    }
  }
  if ( ! found )  // create entry for that FEB
  {
    std::vector<int> PAnbs;
    PAnbs.push_back(channel);

    m_noisy_preamps.push_back(std::make_pair(febid,PAnbs));
  }
}


const std::vector<HWIdentifier>& LArNoisyROSummary::get_noisy_febs() const
{
  return m_noisy_febs;
}

const std::vector<HWIdentifier>& LArNoisyROSummary::get_MNBTight_febs() const
{
     return m_MNBTight_febs;
}

const std::vector<HWIdentifier>& LArNoisyROSummary::get_MNBLoose_febs() const
{
     return m_MNBLoose_febs;
}


const std::vector<std::pair<HWIdentifier,std::vector<int> > >& LArNoisyROSummary::get_noisy_preamps() const
{
  return m_noisy_preamps;
}
