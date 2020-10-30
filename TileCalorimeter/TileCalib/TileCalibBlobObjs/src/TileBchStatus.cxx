/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCalibBlobObjs/TileBchStatus.h"
#include <algorithm>

//=== static memebers
TileBchStatus::LockedPrbSet TileBchStatus::s_refBad ATLAS_THREAD_SAFE;
TileBchStatus::LockedPrbSet TileBchStatus::s_refNoisy ATLAS_THREAD_SAFE;
TileBchStatus::LockedPrbSet TileBchStatus::s_refNoGainL1 ATLAS_THREAD_SAFE;
TileBchStatus::LockedPrbSet TileBchStatus::s_refBadTiming ATLAS_THREAD_SAFE;
TileBchStatus::LockedPrbSet TileBchStatus::s_refWrongBCID ATLAS_THREAD_SAFE;
TileBchStatus::LockedPrbSet TileBchStatus::s_refTimingDmuBcOffset ATLAS_THREAD_SAFE;

//
//_________________________________________________________
TileBchStatus::TileBchStatus()
{
  if(!s_refBad.size()) initClassifierDefinitions();
}

//
//_________________________________________________________
TileBchStatus::TileBchStatus(const PrbSet& prbSet) : 
  m_prbSet(prbSet)
{
  if(!s_refBad.size()) initClassifierDefinitions();
}

//
//_________________________________________________________
TileBchStatus& 
TileBchStatus::operator+=(const TileBchStatus& rhs)
{
  //  PrbSet overlapp;
  std::insert_iterator<PrbSet> insItr(m_prbSet, m_prbSet.begin()); 
  std::set_union(m_prbSet.begin(),m_prbSet.end(), rhs.getPrbs().begin(), rhs.getPrbs().end(), insItr);
  return *this;
}

//
//_________________________________________________________
TileBchStatus& 
TileBchStatus::operator+=(const TileBchPrbs::Prb& prb)
{
  m_prbSet.insert(prb);
  return *this;
}

//
//_________________________________________________________
TileBchStatus& 
TileBchStatus::operator-=(const TileBchStatus& rhs)
{
  PrbSet::const_iterator iPrb = rhs.getPrbs().begin();
  for(; iPrb!=rhs.getPrbs().end(); ++iPrb){
    m_prbSet.erase(*iPrb);
  }
  return *this;
}

//
//_________________________________________________________
TileBchStatus& 
TileBchStatus::operator-=(const TileBchPrbs::Prb& prb)
{
  m_prbSet.erase(prb);
  return *this;
}

//
//_________________________________________________________
bool 
TileBchStatus::operator==(const TileBchStatus &other) const
{
  return (m_prbSet == other.m_prbSet);
}

//
//_________________________________________________________
bool 
TileBchStatus::operator!=(const TileBchStatus &other) const
{
  return !(m_prbSet == other.m_prbSet);
}

//
//_________________________________________________________
void 
TileBchStatus::initClassifierDefinitions()
{
  //=== define which problems trigger a bad state
  PrbSet refBad;
  //=== adc
  refBad.insert(TileBchPrbs::GeneralMaskAdc);
  refBad.insert(TileBchPrbs::AdcDead);
  refBad.insert(TileBchPrbs::SevereStuckBit);
  refBad.insert(TileBchPrbs::SevereDataCorruption);
  refBad.insert(TileBchPrbs::VeryLargeHfNoise);
  refBad.insert(TileBchPrbs::NoData);
  refBad.insert(TileBchPrbs::WrongDspConfig);
  //=== channel
  refBad.insert(TileBchPrbs::GeneralMaskChannel);
  refBad.insert(TileBchPrbs::NoPmt);
  refBad.insert(TileBchPrbs::NoHV);
  refBad.insert(TileBchPrbs::WrongHV);
  //=== online (adc)
  refBad.insert(TileBchPrbs::OnlineGeneralMaskAdc);
  s_refBad.set (std::move (refBad));
  
  PrbSet refNoisy;
  //=== define which problems trigger a noisy state
  refNoisy.insert(TileBchPrbs::LargeHfNoise);
  refNoisy.insert(TileBchPrbs::CorrelatedNoise);
  refNoisy.insert(TileBchPrbs::LargeLfNoise);
  s_refNoisy.set (std::move (refNoisy));

  PrbSet refNoGainL1;
  //=== define which problems trigger a NoGainL1 state
  refNoGainL1.insert(TileBchPrbs::AdcDead);
  refNoGainL1.insert(TileBchPrbs::NoPmt);
  refNoGainL1.insert(TileBchPrbs::NoHV);
  refNoGainL1.insert(TileBchPrbs::TrigGeneralMask);
  refNoGainL1.insert(TileBchPrbs::TrigNoGain);
  refNoGainL1.insert(TileBchPrbs::TrigNoisy);
  refNoGainL1.insert(TileBchPrbs::DisableForL1);
  s_refNoGainL1.set (std::move (refNoGainL1));

  PrbSet refBadTiming;
  //=== define which problems trigger a bad timing
  refBadTiming.insert(TileBchPrbs::BadTiming);
  //=== online
  refBadTiming.insert(TileBchPrbs::OnlineBadTiming);
  s_refBadTiming.set (std::move (refBadTiming));

  PrbSet refTimingDmuBcOffset;
  //=== define which problems trigger an affected timing
  refTimingDmuBcOffset.insert(TileBchPrbs::TimingDmuBcOffset);
  //=== online
  refTimingDmuBcOffset.insert(TileBchPrbs::OnlineTimingDmuBcOffset);
  s_refTimingDmuBcOffset.set (std::move (refTimingDmuBcOffset));

  PrbSet refWrongBCID;
  //=== define which problems trigger a wrong BCID
  refWrongBCID.insert(TileBchPrbs::WrongBCID);
  //=== online
  refWrongBCID.insert(TileBchPrbs::OnlineWrongBCID);
  s_refWrongBCID.set (std::move (refWrongBCID));
}

//
//_________________________________________________________
void 
TileBchStatus::defineBad(const TileBchStatus& status)
{
  s_refBad.set (status.getPrbs());
}

//
//_________________________________________________________
void 
TileBchStatus::defineNoisy(const TileBchStatus& status)
{
  s_refNoisy.set (status.getPrbs());
}

//
//_________________________________________________________
void 
TileBchStatus::defineNoGainL1(const TileBchStatus& status)
{
  s_refNoGainL1.set (status.getPrbs());
}

//
//_________________________________________________________
void 
TileBchStatus::defineBadTiming(const TileBchStatus& status)
{
  s_refBadTiming.set (status.getPrbs());
}

//
//_________________________________________________________
void
TileBchStatus::defineTimingDmuBcOffset(const TileBchStatus& status)
{
  s_refTimingDmuBcOffset.set (status.getPrbs());
}

//
//_________________________________________________________
void
TileBchStatus::defineWrongBCID(const TileBchStatus& status)
{
  s_refWrongBCID.set (status.getPrbs());
}

//
//_________________________________________________________
TileBchStatus 
TileBchStatus::getDefinitionBad()
{
  return s_refBad;
}

//
//_________________________________________________________
TileBchStatus
TileBchStatus::getDefinitionNoisy()
{
  return s_refNoisy;
}

//
//_________________________________________________________
TileBchStatus
TileBchStatus::getDefinitionNoGainL1()
{
  return s_refNoGainL1;
}

//
//_________________________________________________________
TileBchStatus
TileBchStatus::getDefinitionBadTiming()
{
  return s_refBadTiming;
}

//
//_________________________________________________________
TileBchStatus
TileBchStatus::getDefinitionTimingDmuBcOffset()
{
  return s_refTimingDmuBcOffset;
}

//
//_________________________________________________________
TileBchStatus
TileBchStatus::getDefinitionWrongBCID()
{
  return s_refWrongBCID;
}

//
//_________________________________________________________
TileBchStatus::PrbSet
TileBchStatus::testFor(const PrbSet& prbSet) const
{
  PrbSet overlapp;
  std::insert_iterator<PrbSet> insItr(overlapp, overlapp.begin()); 
  std::set_intersection(m_prbSet.begin(),m_prbSet.end(), prbSet.begin(),  prbSet.end(), insItr);
  return overlapp;
}

//
//_________________________________________________________
std::string
TileBchStatus::getString() const
{
  std::string prbStr("");
  for(PrbSet::const_iterator iPrb = m_prbSet.begin(); iPrb != m_prbSet.end(); ++iPrb){
    prbStr += TileBchPrbs::getDescription(*iPrb) + "; ";
  } 
  return prbStr;
}

//
//_________________________________________________________
bool TileBchStatus::LockedPrbSet::test (const PrbSet& s) const
{
  std::lock_guard lock (m_mutex);
  PrbSet overlapp;
  std::insert_iterator<PrbSet> insItr(overlapp, overlapp.begin()); 
  std::set_intersection(m_set.begin(),m_set.end(), s.begin(),  s.end(), insItr);
  return !overlapp.empty();
}
