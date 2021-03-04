/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef ANA_ALGORITHM__ANA_REENTRANT_ALGORITHM_H
#define ANA_ALGORITHM__ANA_REENTRANT_ALGORITHM_H

#include <AnaAlgorithm/Global.h>

#ifdef XAOD_STANDALONE
#include <AsgTools/AsgComponent.h>
#include <AsgTools/SgTEvent.h>
#include <memory>
#include <vector>
#else
#include <AthenaBaseComps/AthReentrantAlgorithm.h>
#include <AsgMessaging/MessageCheck.h>
#endif

class EventContext;
class ISvcLocator;

namespace EL
{
#ifdef XAOD_STANDALONE
  class IWorker;
#endif

  /// \brief the base class for EventLoop reentrant algorithms
  ///
  /// Technically EventLoop doesn't use that algorithms are reentrant,
  /// but when writing a dual-use algorithm it is preferable for it to
  /// be reentrant in Athena, and for that it needs its own interface
  /// class.
  ///
  /// Please note that if you use this algorithm base class you
  /// absolutely must use data handles for accessing the event store.
  /// While it makes zero difference in EventLoop, it doesn't work
  /// otherwise in AthenaMT, which defeats the whole purpose of using
  /// this class.

  class AnaReentrantAlgorithm
#ifdef XAOD_STANDALONE
    : public asg::AsgComponent
#else
    : public AthReentrantAlgorithm
#endif
  {
    //
    // public interface
    //

    /// \brief constructor with parameters
    ///
    /// This matches the Athena algorithm constructor (for dual-use
    /// purposes).  Within EventLoop the `pSvcLocator` will always be
    /// `nullptr` (unless we ever have dual-use services).
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    AnaReentrantAlgorithm (const std::string& name,
                           ISvcLocator* pSvcLocator);

    /// \brief standard (virtual) destructor
    /// \par Guarantee
    ///   no-fail
  public:
    virtual ~AnaReentrantAlgorithm() noexcept;



    //
    // services interface
    //

#ifdef XAOD_STANDALONE
    /// \brief the filter worker interface
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   no filter worker configured
    /// \post result != nullptr
  public:
    IFilterWorker *filterWorker () const;


    /// \brief whether the current algorithm passed its filter
    /// criterion for the current event
    /// \par Guarantee
    ///   no-fail
  public:
    bool filterPassed() const;

    /// \brief set the value of \ref filterPassed
    /// \par Guarantee
    ///   no-fail
  public:
    void setFilterPassed (bool val_filterPassed);


    /// \brief the worker that is controlling us (if working in
    /// EventLoop)
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   job not running in EventLoop
    /// \post result != nullptr
  public:
    IWorker *wk () const;
#endif



    //
    // virtual interface
    //

    /// \brief initialize this algorithm
    ///
    /// Note that unlike the original EventLoop algorithms, this gets
    /// called before any events are in memory (or at least it can
    /// be).  As such you should *not* try to access the current event
    /// in here.
  protected:
    virtual ::StatusCode initialize ();

    /// \brief execute this algorithm
    ///
    /// This gets called once on every event and is where the bulk of
    /// the processing ought to be happening.
  protected:
    virtual ::StatusCode execute (const EventContext& ctx) const;

    /// \brief finalize this algorithm
    ///
    /// This gets called after event processing has finished.  The
    /// last event may no longer be in memory, and the code should not
    /// try to access it.
  protected:
    virtual ::StatusCode finalize ();



    //
    // framework interface
    //

#ifdef XAOD_STANDALONE
    /// \brief call \ref initialize
  public:
    ::StatusCode sysInitialize ();

    /// \brief call \ref execute
  public:
    ::StatusCode sysExecute (const EventContext& ctx);

    /// \brief call \ref finalize
  public:
    ::StatusCode sysFinalize ();


    /// \brief get the (main) event store for this algorithm
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   job not configured for xAODs
    /// \post result != nullptr
    ///
    /// \warn The user should *not* be calling this function directly,
    /// ever, and it may very well disappear at any point without
    /// notice, and it does *not* exist in Athena which defeats the
    /// whole purpose of having a reentrant algorithm.
    ///
    /// It is essentially here because the stand-alone data handles
    /// need it.  Basically there are three solutions I considered:
    /// * make it a public member function for compatibility and warn all
    ///   users against using it directly
    /// * make it a private member function, and make all data handles
    ///   friends, but that would be a lot of friends
    /// * change the implementation of all data handles to no longer require
    ///   this function, which would be quite an intrusive change
  public:
    asg::SgTEvent *evtStore() const;

    /// \brief set the value of \ref evtStore
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   service already configured
  public:
    void setEvtStore (asg::SgTEvent *val_evtStore);

    /// \brief set the value of \ref filterWorker
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   service already configured
  public:
    void setFilterWorker (IFilterWorker *val_filterWorker);

    /// \brief set the value of \ref wk
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   service already configured
  public:
    void setWk (IWorker *val_wk);
#endif



    //
    // private interface
    //

#ifdef XAOD_STANDALONE
    /// \brief the value of \ref evtStore
  private:
    asg::SgTEvent *m_evtStore = nullptr;
#endif

#ifdef XAOD_STANDALONE
    /// \brief the value of \ref filterWorker
  private:
    IFilterWorker *m_filterWorker = nullptr;
#endif

#ifdef XAOD_STANDALONE
    /// \brief the value of \ref wk
  private:
    IWorker *m_wk = nullptr;
#endif
  };
}

#endif
