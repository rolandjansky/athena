#ifndef EVENT_LOOP_COMPS__ANA_ALGORITHM_H
#define EVENT_LOOP_COMPS__ANA_ALGORITHM_H

//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <EventLoopComps/Global.h>

#ifdef ROOTCORE
#include <AsgTools/AsgMessaging.h>
#include <AsgTools/SgTEvent.h>
#else
#include <AthenaBaseComps/AthHistogramAlgorithm.h>
#endif

class TH1;
class ISvcLocator;
#ifdef ROOTCORE
class Property;
class PropertyMgr;
#else
#endif

namespace EL
{
#ifdef ROOTCORE
  class Worker;
#endif

  /// \brief the (new) base class for EventLoop algorithms
  ///
  /// This is meant as a replacement for the old EventLoop algorithms
  /// (\ref EL::Algorithm) to address a number of problems:
  /// * The old algorithm class relied on streamers for configuration.
  ///   This is actually pretty bad in practice, as it leads to all
  ///   kinds of errors.  The new class instead uses the
  ///   `setProperty()` mechanism that is used for other components.
  /// * The old algorithm class was tied very closely to the \ref
  ///   EL::Worker class, making it difficult to run standalone,
  ///   e.g. for testing.
  /// * The old algorithm class had different signatures and calling
  ///   conventions from the corresponding methods in Athena
  ///   algorithms, making it near-impossible to write dual-use
  ///   algorithms.
  /// * The old algorithm class was tied in closely with the rest of
  ///   the EventLoop package, meaning every library implementing an
  ///   algorithm had to depend on the EventLoop package.
  ///
  /// \todo This class does not allow to do everything that can
  /// currently be done with a \ref `EL::Algorithm` since it neither
  /// knows about incidents, nor has all the virtual methods that were
  /// available before.
  ///
  /// \todo It might be nice to make this actually dual-use at some
  /// point, but since there seems little interest in a dual-use
  /// algorithm that is on the back-burner.  There are some
  /// conditional compilation directives here and there, but it was
  /// never tested for dual-use (or even in-athena compilation).

  class AnaAlgorithm
#ifdef ROOTCORE
    : public asg::AsgMessaging, public INamedInterface
#else
    : public AthHistogramAlgorithm
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
    AnaAlgorithm (const std::string& name, 
                  ISvcLocator* pSvcLocator);

    /// \brief standard (virtual) destructor
    /// \par Guarantee
    ///   no-fail
  public:
    virtual ~AnaAlgorithm() noexcept;



    //
    // services interface
    //

#ifdef ROOTCORE
    /// \brief get the (main) event store for this algorithm
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   job not configured for xAODs
    /// \post result != nullptr
  public:
    asg::SgTEvent *evtStore() const;


    /// \brief book the given histogram
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   histogram booking error
  public:
    ::StatusCode book (const TH1& hist);


    /// \brief get the histogram with the given name
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   histogram not found
  public:
    TH1 *hist (const std::string& name) const;


    /// \brief the histogram worker interface
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   no histogram worker configured
    /// \post result != nullptr
  public:
    IHistogramWorker *histogramWorker () const;


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
    Worker *wk () const;
#endif



    //
    // properties interface
    //

#ifdef ROOTCORE
    /// \brief declare an algorithm property
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    template<typename T> Property *
    declareProperty (const std::string& name, T& loc,
                     const std::string& doc = "");

    /// \brief set a property with a given value
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   invalid property type
  public:
    template<typename T> ::StatusCode
    setProperty (const std::string& name, T&& value);
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
    virtual ::StatusCode execute ();

    /// \brief finalize this algorithm
    ///
    /// This gets called after event processing has finished.  The
    /// last event may no longer be in memory, and the code should not
    /// try to access it.
  protected:
    virtual ::StatusCode finalize ();

    /// \brief print the state of the algorithm
    ///
    /// This is mostly to allow algorithms to add a little debugging
    /// information if they feel like it.
  protected:
    virtual void print () const;



    //
    // framework interface
    //

#ifdef ROOTCORE
    /// \brief call \ref initialize
  public:
    ::StatusCode sysInitialize ();

    /// \brief call \ref execute
  public:
    ::StatusCode sysExecute ();

    /// \brief call \ref finalize
  public:
    ::StatusCode sysFinalize ();

    /// \brief call \ref print
  public:
    void sysPrint ();


    /// \brief set the value of \ref evtStore
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   service already configured
  public:
    void setEvtStore (asg::SgTEvent *val_evtStore);

    /// \brief set the value of \ref histogramWorker
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   service already configured
  public:
    void setHistogramWorker (IHistogramWorker *val_histogramWorker);

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
    void setWk (Worker *val_wk);
#endif


    //
    // inherited interface
    //

#ifdef ROOTCORE
  public:
    virtual const std::string& name () const final;
#endif



    //
    // private interface
    //

#ifdef ROOTCORE
    /// \brief the name of the algorithm
  private:
    std::string m_name;
#endif

#ifdef ROOTCORE
    /// \brief the property manager
  private:
    std::unique_ptr<PropertyMgr> m_properties;
#endif

#ifdef ROOTCORE
    /// \brief the value of \ref evtStore
  private:
    asg::SgTEvent *m_evtStore = nullptr;
#endif

#ifdef ROOTCORE
    /// \brief the value of \ref histogramWorker
  private:
    IHistogramWorker *m_histogramWorker = nullptr;
#endif

#ifdef ROOTCORE
    /// \brief the value of \ref filterWorker
  private:
    IFilterWorker *m_filterWorker = nullptr;
#endif

#ifdef ROOTCORE
    /// \brief the value of \ref wk
  private:
    Worker *m_wk = nullptr;
#endif
  };
}

#include "AnaAlgorithm.icc"

#endif
