/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef PAT_INTERFACES__SYSTEMATICS_CACHE_H
#define PAT_INTERFACES__SYSTEMATICS_CACHE_H

#include <AsgMessaging/AsgMessagingForward.h>
#include <AsgMessaging/MessageCheck.h>
#include <CxxUtils/AthUnlikelyMacros.h>
#include <CxxUtils/checker_macros.h>
#include <PATInterfaces/SystematicSet.h>
#include <functional>
#include <tbb/concurrent_unordered_map.h>

namespace CP
{
  /// \brief helper class to cache systematics varied calibration
  /// constants inside a \ref ISystematicsTool or \ref
  /// IRentrantSystematicsTool
  ///
  /// The general pattern for how systematics are supposed to be
  /// implemented inside systematics tools is that the first time a
  /// specific systematic variation is requested the tool should
  /// calculate the calibration constants for that particular
  /// systematic, store it in an (unordered) map and look it up from
  /// that map in all subsequent uses.  This class tries to implement
  /// that particular logic in a thread-safe and (reasonably)
  /// fool-proof way.
  ///
  /// The basic way to use this class is:
  /// ```
  /// class MyTool : public asg::AsgTool, virtual public IMyTool {
  ///    ...
  ///    struct ConfigData {...};
  ///    CP::SystematicsCache<ConfigData> m_calibCache {this};
  ///    const ConfigData *m_currentConfig {nullptr};
  ///  };
  ///
  ///  StatusCode MyTool :: initialize () {
  ///    ...
  ///    CP::SystematicSet affectingSystematics = ...;
  ///    ANA_CHECK (m_calibCache.initialize (std::move (affectingSystematics),
  ///                                        [this] (const CP::SystematicSet& sys,
  ///                                                ConfigData& myconfig) {
  ///        myconfig = ...;
  ///        return StatusCode;});
  ///    ANA_CHECK (applySystematicVariation (CP::SystematicSet()));
  ///    return StatusCode::SUCCESS;
  ///  }
  ///
  /// StatusCode MyTool :: applySystematicVariation (const CP::SystematicSet& sys, ...) {
  ///   return m_calibCache.get (sys, m_currentConfig);
  /// }
  ///
  /// CP::SystematicSet MyTool :: affectingSystematics () const {
  ///   return m_calibCache.affectingSystematics(); }
  /// ```
  ///
  /// There are not too many constraints on the structure and lambda
  /// function, just make sure the structure is default constructable
  /// and the lambda function is safe to execute while a lock is held
  /// (i.e. if you acquire a lock make sure not to create a
  /// dead-lock).  It is currently not anticipated that this lambda
  /// function needs to be thread-safe itself (even if multiple
  /// threads use the same tool, they would still just run through the
  /// same systematics in the same order).  It is guaranteed that the
  /// function is called exactly once for each unique set of affecting
  /// systematics, and that only a single copy of the calibration data
  /// is stored for each unique set of affecting systematics.
  ///
  /// Note that while the above example uses a lambda function it is
  /// probably a good idea to write this as a separate helper function
  /// and have the lambda function wrap that helper.  The rationale
  /// for that recommendation is that this is usually a rather long
  /// function with tedious logic and making it part of initialize can
  /// affect the readability of either one.

  template<typename CalibData>
  class SystematicsCache final : public asg::AsgMessagingForward
  {
    /// Public Members
    /// ==============

  public:

    using asg::AsgMessagingForward::msg;

    template<typename T2>
    SystematicsCache (T2 *val_parent);

    /// initialize with the list of affecting systematics and a
    /// function to calculate the calibration data for each systematic
    void initialize (CP::SystematicSet val_affectingSystematics,
                     std::function<StatusCode (const CP::SystematicSet& sys, CalibData& result)> val_calc) noexcept;

    /// the list of affecting systematics
    const CP::SystematicSet& affectingSystematics () const noexcept;


    /// whether we are affected by the given systematic variation
    ///
    /// this is mostly for implementing the \ref ISystematicsTool
    /// interface and should probably not be used by anyone.
    bool isAffectedBySystematic(const SystematicVariation& systematic) const;


    /// get the pointer to the cached calibration data for the given
    /// systematic, calculating it if necessary
    StatusCode get (const CP::SystematicSet& sys, const CalibData*& result) const;



    /// Private Members
    /// ===============

  private:

    /// the list of affecting systematics
    CP::SystematicSet m_affectingSystematics;

    /// the function to calculate the calibration data for a given
    /// systematics
    std::function<StatusCode (const CP::SystematicSet& sys, CalibData& result)> m_calc;

    /// the cache of previously calculated calibration data
    mutable tbb::concurrent_unordered_map<CP::SystematicSet,std::shared_ptr<const CalibData>,SystematicSetHash> m_cache ATLAS_THREAD_SAFE;

    /// a mutex to protext against concurrent execution of \ref m_calc
    ///
    /// see the comment inside \ref get for a detailed explanation of
    /// why this is used/needed.
    mutable std::mutex m_calcMutex;
  };



  /// Function Implementations
  /// ========================

  template<typename CalibData> template<typename T2>
  SystematicsCache<CalibData> ::
  SystematicsCache (T2 *val_parent)
    : AsgMessagingForward (val_parent)
  {}



  template<typename CalibData> void SystematicsCache<CalibData> ::
  initialize (CP::SystematicSet val_affectingSystematics,
              std::function<StatusCode (const CP::SystematicSet& sys, CalibData& result)> val_calc) noexcept
  {
    m_calc = std::move (val_calc);
    m_affectingSystematics = std::move (val_affectingSystematics);
    m_cache.clear ();
  }



  template<typename CalibData>
  const CP::SystematicSet& SystematicsCache<CalibData> ::
  affectingSystematics () const noexcept
  {
    return m_affectingSystematics;
  }



  template<typename CalibData>
  bool SystematicsCache<CalibData> ::
  isAffectedBySystematic(const SystematicVariation& sys) const
  {
    // this is not technically correct, but what a lot of people do
    return m_affectingSystematics.find (sys) != m_affectingSystematics.end();
  }



  template<typename CalibData>
  StatusCode SystematicsCache<CalibData> ::
  get (const CP::SystematicSet& sys, const CalibData*& result) const
  {
    // fast-path, check if we already calculated this
    auto iter = m_cache.find (sys);
    if (ATH_LIKELY (iter != m_cache.end())) //[[likely]], removed until C++20 support
    {
      result = iter->second.get();
      return StatusCode::SUCCESS;
    }

    // create a lock to prevent the concurrent (or duplicate)
    // execution of the \ref m_calc function.  if that function is
    // thread-safe this lock is not strictly necessary.  however,
    // given that it is hard to enforce that the function is indeed
    // thread-safe and there are essentially no performance gains to
    // be had, this seems safer (unless \ref m_calc acquires mutexes
    // in a way that causes a dead-lock).
    std::lock_guard<std::mutex> lock (m_calcMutex);

    // check if another thread already calculated this value while we
    // acquired the lock
    iter = m_cache.find (sys);
    if (iter != m_cache.end())
    {
      result = iter->second.get();
      return StatusCode::SUCCESS;
    }

    // we will only ever calculate the systematics for our set of
    // affecting systematics, any systematics that are not affecting
    // us will be ignored
    CP::SystematicSet mysys;
    ANA_CHECK (SystematicSet::filterForAffectingSystematics (sys, m_affectingSystematics, mysys));

    std::shared_ptr<const CalibData> mycalib;
    {
      // check if our affecting systematics are known already
      auto iter = m_cache.find (mysys);
      if (iter != m_cache.end())
        mycalib = iter->second;
      else
      {
        // this will now actually calculate the systematics.  note
        // that if this fails, nothing gets cached, but we should also
        // be aborting
        auto value = std::make_shared<CalibData> ();
        ANA_CHECK (m_calc (mysys, *value));

        // store for the affecting systematic
        auto emplace_result = m_cache.emplace (mysys, value);
        mycalib = emplace_result.first->second;
      }
    }

    // store for user requested systematics as well, which may or may
    // not be the same as the affecting systematics, but if it is
    // already there, this will do nothing.
    m_cache.emplace (sys, mycalib);
    result = mycalib.get();
    return StatusCode::SUCCESS;
  }
}

#endif
