//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoopComps/AnaAlgorithm.h>

#include <AsgTools/MessageCheck.h>
#include <RootCoreUtils/Assert.h>
#include <TH1.h>

#ifdef ROOTCORE
#include <EventLoopComps/IFilterWorker.h>
#include <EventLoopComps/IHistogramWorker.h>
#endif

//
// method implementations
//

namespace EL
{
  AnaAlgorithm ::
  AnaAlgorithm (const std::string& name, 
                ISvcLocator*
#ifndef ROOTCORE
                pSvcLocator
#endif
                )
#ifdef ROOTCORE
    : AsgMessaging (name)
    , m_name (name)
    , m_properties (new PropertyMgr)
#else
    : AthHistogramAlgorithm (name, pSvcLocator)
#endif
  {
    ANA_MSG_INFO ("AnaAlgorithm: " << name);
  }



  AnaAlgorithm ::
  ~AnaAlgorithm () noexcept
  {}



#ifdef ROOTCORE
  asg::SgTEvent *AnaAlgorithm ::
  evtStore () const
  {
    if (!m_evtStore)
      throw std::logic_error ("no evtStore set on algorithm " + name());
    return m_evtStore;
  }



  ::StatusCode AnaAlgorithm ::
  book (const TH1& hist)
  {
    RCU_CHANGE_INVARIANT (this);
    histogramWorker()->addOutput (hist.Clone());
    return ::StatusCode::SUCCESS;
  }



  TH1 *AnaAlgorithm ::
  hist (const std::string& name) const
  {
    RCU_READ_INVARIANT (this);
    return histogramWorker()->getOutputHist (name);
  }



  IHistogramWorker *AnaAlgorithm ::
  histogramWorker () const
  {
    if (!m_histogramWorker)
      throw std::logic_error ("no histogram worker set on algorithm " + name());
    return m_histogramWorker;
  }



  bool AnaAlgorithm ::
  filterPassed() const
  {
    return filterWorker()->filterPassed();
  }



  void AnaAlgorithm ::
  setFilterPassed (bool val_filterPassed)
  {
    filterWorker()->setFilterPassed (val_filterPassed);
  }



  IFilterWorker *AnaAlgorithm ::
  filterWorker () const
  {
    if (!m_filterWorker)
      throw std::logic_error ("no filter worker set on algorithm " + name());
    return m_filterWorker;
  }



  Worker *AnaAlgorithm ::
  wk () const
  {
    if (!m_wk)
      throw std::logic_error ("no worker set on algorithm " + name());
    return m_wk;
  }
#endif



  ::StatusCode AnaAlgorithm ::
  initialize ()
  {
    return StatusCode::SUCCESS;
  }



  ::StatusCode AnaAlgorithm ::
  execute ()
  {
    return StatusCode::SUCCESS;
  }



  ::StatusCode AnaAlgorithm ::
  finalize ()
  {
    return StatusCode::SUCCESS;
  }



  void AnaAlgorithm ::
  print () const
  {}



#ifdef ROOTCORE
  ::StatusCode AnaAlgorithm ::
  sysInitialize ()
  {
    return initialize ();
  }



  ::StatusCode AnaAlgorithm ::
  sysExecute ()
  {
    return execute ();
  }



  ::StatusCode AnaAlgorithm ::
  sysFinalize ()
  {
    return finalize ();
  }



  void AnaAlgorithm ::
  sysPrint ()
  {
    print ();
  }



  void AnaAlgorithm ::
  setEvtStore (asg::SgTEvent *val_evtStore)
  {
    if (m_evtStore)
      throw std::logic_error ("set evtStore twice on algorithm " + name());
    m_evtStore = val_evtStore;
  }



  void AnaAlgorithm ::
  setHistogramWorker (IHistogramWorker *val_histogramWorker)
  {
    if (m_histogramWorker)
      throw std::logic_error ("set histogram worker twice on algorithm " + name());
    m_histogramWorker = val_histogramWorker;
  }



  void AnaAlgorithm ::
  setFilterWorker (IFilterWorker *val_filterWorker)
  {
    if (m_filterWorker)
      throw std::logic_error ("set filter worker twice on algorithm " + name());
    m_filterWorker = val_filterWorker;
  }



  void AnaAlgorithm ::
  setWk (Worker *val_wk)
  {
    if (m_wk)
      throw std::logic_error ("set wk twice on algorithm " + name());
    m_wk = val_wk;
  }



  const std::string& AnaAlgorithm ::
  name () const
  {
    return m_name;
  }
#endif
}
