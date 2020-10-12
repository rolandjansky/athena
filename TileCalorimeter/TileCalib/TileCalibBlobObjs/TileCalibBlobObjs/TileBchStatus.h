/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBBLOBOBJS_TILEBCHSTATUS_H
#define TILECALIBBLOBOBJS_TILEBCHSTATUS_H

/** 
    @brief Class holding bad channel problems.
    @author Nils Gollub <nils.gollub@cern.ch>
*/

#include "TileCalibBlobObjs/TileBchPrbs.h"
#include "CxxUtils/checker_macros.h"
#include <set>
#include <string>
#include <mutex>

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
  static void defineTimingDmuBcOffset(const TileBchStatus& status);
  static void defineWrongBCID(const TileBchStatus& status);
  static TileBchStatus getDefinitionBad();
  static TileBchStatus getDefinitionNoisy();
  static TileBchStatus getDefinitionNoGainL1();
  static TileBchStatus getDefinitionBadTiming();
  static TileBchStatus getDefinitionTimingDmuBcOffset();
  static TileBchStatus getDefinitionWrongBCID();

  bool isGood() const;
  bool isBad() const;
  bool isAffected() const;
  bool isNoisy() const;
  bool isIgnoredInDsp() const;
  bool isIgnoredInHlt() const;
  bool isNoGainL1() const;
  bool isHalfGainL1() const;
  bool isBadTiming() const;
  bool isTimingDmuBcOffset() const;
  bool isWrongBCID() const;

 private:
  PrbSet m_prbSet;
  //=== reference sets
  struct LockedPrbSet
  {
    size_t size() const
    {
      std::lock_guard lock (m_mutex);
      return m_set.size();
    }
    void set (const PrbSet& s)
    {
      std::lock_guard lock (m_mutex);
      m_set = s;
    }
    void set (PrbSet&& s)
    {
      std::lock_guard lock (m_mutex);
      m_set = std::move(s);
    }
    operator TileBchStatus() const
    {
      std::lock_guard lock (m_mutex);
      return TileBchStatus (m_set, true);
    }
    bool test (const PrbSet& s) const;

  private:
    PrbSet m_set;
    mutable std::mutex m_mutex;
  };

  TileBchStatus (const PrbSet& s, bool) : m_prbSet (s) {}

  static LockedPrbSet s_refBad ATLAS_THREAD_SAFE;
  static LockedPrbSet s_refNoisy ATLAS_THREAD_SAFE;
  static LockedPrbSet s_refNoGainL1 ATLAS_THREAD_SAFE;
  static LockedPrbSet s_refBadTiming ATLAS_THREAD_SAFE;
  static LockedPrbSet s_refWrongBCID ATLAS_THREAD_SAFE;
  static LockedPrbSet s_refTimingDmuBcOffset ATLAS_THREAD_SAFE;
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
  return m_prbSet.size() ? (s_refBad.test (m_prbSet)) : false;
}

//
//_________________________________________________________
inline bool
TileBchStatus::isNoisy() const
{
  return m_prbSet.size() ? (s_refNoisy.test (m_prbSet)) : false;
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
  return m_prbSet.size() ? (s_refNoGainL1.test (m_prbSet)) : false;
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
  return m_prbSet.size() ? (s_refBadTiming.test (m_prbSet)) : false;
}

//
//_________________________________________________________
inline bool
TileBchStatus::isTimingDmuBcOffset() const
{
  return m_prbSet.size() ? (s_refTimingDmuBcOffset.test (m_prbSet)) : false;
}

//
//_________________________________________________________
inline bool
TileBchStatus::isWrongBCID() const
{
  return m_prbSet.size() ? (s_refWrongBCID.test (m_prbSet)) : false;
}

#endif
