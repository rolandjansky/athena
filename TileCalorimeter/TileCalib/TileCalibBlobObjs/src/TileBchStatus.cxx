/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCalibBlobObjs/TileBchStatus.h"
#include <algorithm>

//=== static memebers
TileBchStatus::PrbSet TileBchStatus::m_refBad;
TileBchStatus::PrbSet TileBchStatus::m_refNoisy;
TileBchStatus::PrbSet TileBchStatus::m_refNoGainL1;
TileBchStatus::PrbSet TileBchStatus::m_refBadTiming;
TileBchStatus::PrbSet TileBchStatus::m_refWrongBCID;
TileBchStatus::PrbSet TileBchStatus::m_refTimingDmuBcOffset;

//
//_________________________________________________________
TileBchStatus::TileBchStatus()
{
  if(!m_refBad.size()) initClassifierDefinitions();
}

//
//_________________________________________________________
TileBchStatus::TileBchStatus(const PrbSet& prbSet) : 
  m_prbSet(prbSet)
{
  if(!m_refBad.size()) initClassifierDefinitions();
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
  //=== adc
  m_refBad.insert(TileBchPrbs::GeneralMaskAdc);
  m_refBad.insert(TileBchPrbs::AdcDead);
  m_refBad.insert(TileBchPrbs::SevereStuckBit);
  m_refBad.insert(TileBchPrbs::SevereDataCorruption);
  m_refBad.insert(TileBchPrbs::VeryLargeHfNoise);
  m_refBad.insert(TileBchPrbs::NoData);
  m_refBad.insert(TileBchPrbs::WrongDspConfig);
  //=== channel
  m_refBad.insert(TileBchPrbs::GeneralMaskChannel);
  m_refBad.insert(TileBchPrbs::NoPmt);
  m_refBad.insert(TileBchPrbs::NoHV);
  m_refBad.insert(TileBchPrbs::WrongHV);
  //=== online (adc)
  m_refBad.insert(TileBchPrbs::OnlineGeneralMaskAdc);
  
  //=== define which problems trigger a noisy state
  m_refNoisy.insert(TileBchPrbs::LargeHfNoise);
  m_refNoisy.insert(TileBchPrbs::CorrelatedNoise);
  m_refNoisy.insert(TileBchPrbs::LargeLfNoise);

  //=== define which problems trigger a NoGainL1 state
  m_refNoGainL1.insert(TileBchPrbs::AdcDead);
  m_refNoGainL1.insert(TileBchPrbs::NoPmt);
  m_refNoGainL1.insert(TileBchPrbs::NoHV);
  m_refNoGainL1.insert(TileBchPrbs::TrigGeneralMask);
  m_refNoGainL1.insert(TileBchPrbs::TrigNoGain);
  m_refNoGainL1.insert(TileBchPrbs::TrigNoisy);
  m_refNoGainL1.insert(TileBchPrbs::DisableForL1);

  //=== define which problems trigger a bad timing
  m_refBadTiming.insert(TileBchPrbs::BadTiming);
  //=== online
  m_refBadTiming.insert(TileBchPrbs::OnlineBadTiming);

  //=== define which problems trigger an affected timing
  m_refTimingDmuBcOffset.insert(TileBchPrbs::TimingDmuBcOffset);
  //=== online
  m_refTimingDmuBcOffset.insert(TileBchPrbs::OnlineTimingDmuBcOffset);

  //=== define which problems trigger a wrong BCID
  m_refWrongBCID.insert(TileBchPrbs::WrongBCID);
  //=== online
  m_refWrongBCID.insert(TileBchPrbs::OnlineWrongBCID);

}

//
//_________________________________________________________
void 
TileBchStatus::defineBad(const TileBchStatus& status)
{
  m_refBad = status.getPrbs();
}

//
//_________________________________________________________
void 
TileBchStatus::defineNoisy(const TileBchStatus& status)
{
  m_refNoisy = status.getPrbs();
}

//
//_________________________________________________________
void 
TileBchStatus::defineNoGainL1(const TileBchStatus& status)
{
  m_refNoGainL1 = status.getPrbs();
}

//
//_________________________________________________________
void 
TileBchStatus::defineBadTiming(const TileBchStatus& status)
{
  m_refBadTiming = status.getPrbs();
}

//
//_________________________________________________________
void
TileBchStatus::defineTimingDmuBcOffset(const TileBchStatus& status)
{
  m_refTimingDmuBcOffset = status.getPrbs();
}

//
//_________________________________________________________
void
TileBchStatus::defineWrongBCID(const TileBchStatus& status)
{
  m_refWrongBCID = status.getPrbs();
}


//
//_________________________________________________________
TileBchStatus 
TileBchStatus::getDefinitionBad()
{
  return TileBchStatus(m_refBad);
}

//
//_________________________________________________________
TileBchStatus
TileBchStatus::getDefinitionNoisy()
{
  return TileBchStatus(m_refNoisy);
}

//
//_________________________________________________________
TileBchStatus
TileBchStatus::getDefinitionNoGainL1()
{
  return TileBchStatus(m_refNoGainL1);
}

//
//_________________________________________________________
TileBchStatus
TileBchStatus::getDefinitionBadTiming()
{
  return TileBchStatus(m_refBadTiming);
}

//
//_________________________________________________________
TileBchStatus
TileBchStatus::getDefinitionTimingDmuBcOffset()
{
  return TileBchStatus(m_refTimingDmuBcOffset);
}

//
//_________________________________________________________
TileBchStatus
TileBchStatus::getDefinitionWrongBCID()
{
  return TileBchStatus(m_refWrongBCID);
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
