/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBBLOBOBJS_TILEBCHSTATUS_H
#define TILECALIBBLOBOBJS_TILEBCHSTATUS_H

/** 
    @brief Class holding bad channel problems.
    @author Nils Gollub <nils.gollub@cern.ch>
*/

#include "TileCalibBlobObjs/TileBchPrbs.h"
#include <set>
#include <string>

class TileBchStatus
{
 public:
  typedef TileBchPrbs::Prb Prb;
  typedef std::set<Prb>    PrbSet;

  TileBchStatus();
  TileBchStatus(const PrbSet& prbSet);

  TileBchStatus& operator+=(const TileBchStatus& rhs);
  TileBchStatus& operator+=(const TileBchPrbs::Prb& prb);
  TileBchStatus& operator-=(const TileBchStatus& rhs);
  TileBchStatus& operator-=(const TileBchPrbs::Prb& prb);
  bool           operator==(const TileBchStatus &other) const;
  bool           operator!=(const TileBchStatus &other) const;

  const PrbSet& getPrbs() const;
  PrbSet        testFor(const PrbSet& prbSet) const;
  bool          contains(const Prb& prb) const;
  std::string   getString() const;

  static void initClassifierDefinitions();
  static void defineBad(  const TileBchStatus& status);
  static void defineNoisy(const TileBchStatus& status);
  static void defineNoGainL1(const TileBchStatus& status);
  static void defineBadTiming(const TileBchStatus& status);
  static TileBchStatus getDefinitionBad();
  static TileBchStatus getDefinitionNoisy();
  static TileBchStatus getDefinitionNoGainL1();
  static TileBchStatus getDefinitionBadTiming();

  bool isGood() const;
  bool isBad() const;
  bool isAffected() const;
  bool isNoisy() const;
  bool isIgnoredInDsp() const;
  bool isIgnoredInHlt() const;
  bool isNoGainL1() const;
  bool isHalfGainL1() const;
  bool isBadTiming() const;

 private:
  PrbSet m_prbSet;
  //=== reference sets
  static PrbSet m_refBad;
  static PrbSet m_refNoisy;
  static PrbSet m_refNoGainL1;
  static PrbSet m_refBadTiming;
};

//
//_________________________________________________________
inline const TileBchStatus::PrbSet&
TileBchStatus::getPrbs() const
{
  return m_prbSet;
}

//
//_________________________________________________________
inline bool
TileBchStatus::contains(const Prb& prb) const
{
  return (m_prbSet.find(prb) != m_prbSet.end());
}

//
//_________________________________________________________
inline bool
TileBchStatus::isGood() const
{
  return (m_prbSet.size() == 0);
}

//
//_________________________________________________________
inline bool
TileBchStatus::isAffected() const
{
  return (!isGood());
}

//
//_________________________________________________________
inline bool
TileBchStatus::isBad() const
{
  return m_prbSet.size() ? (testFor(m_refBad).size() != 0) : false;
}

//
//_________________________________________________________
inline bool
TileBchStatus::isNoisy() const
{
  return m_prbSet.size() ? (testFor(m_refNoisy).size() != 0) : false;
}

//
//_________________________________________________________
inline bool
TileBchStatus::isIgnoredInDsp() const
{
  return m_prbSet.size() ? contains(TileBchPrbs::IgnoredInDsp) : false; 
}

//
//_________________________________________________________
inline bool
TileBchStatus::isIgnoredInHlt() const
{
  return m_prbSet.size() ? contains(TileBchPrbs::IgnoredInHlt) : false; 
}

//
//_________________________________________________________
inline bool
TileBchStatus::isNoGainL1() const
{
  return m_prbSet.size() ? (testFor(m_refNoGainL1).size()!=0) : false;
}

//
//_________________________________________________________
inline bool
TileBchStatus::isHalfGainL1() const
{
  return m_prbSet.size() ? contains(TileBchPrbs::TrigHalfGain) : false; 
}

//
//_________________________________________________________
inline bool
TileBchStatus::isBadTiming() const
{
  return m_prbSet.size() ? (testFor(m_refBadTiming).size() != 0) : false;
}

#endif
