/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AnaAlgorithm/AnaReentrantAlgorithm.h>

#include <AsgMessaging/MessageCheck.h>
#include <RootCoreUtils/Assert.h>
#include <stdexcept>

#ifdef XAOD_STANDALONE
#include <AnaAlgorithm/IFilterWorker.h>
#endif

//
// method implementations
//

namespace EL
{
  AnaReentrantAlgorithm ::
  AnaReentrantAlgorithm (const std::string& name,
                         [[maybe_unused]] ISvcLocator *pSvcLocator)
#ifdef XAOD_STANDALONE
    : AsgComponent (name)
#else
    : AthReentrantAlgorithm (name, pSvcLocator)
#endif
  {
    ANA_MSG_DEBUG ("AnaReentrantAlgorithm: " << name);
  }



  AnaReentrantAlgorithm ::
  ~AnaReentrantAlgorithm () noexcept
  {}



#ifdef XAOD_STANDALONE
  bool AnaReentrantAlgorithm ::
  filterPassed() const
  {
    return filterWorker()->filterPassed();
  }



  void AnaReentrantAlgorithm ::
  setFilterPassed (bool val_filterPassed)
  {
    filterWorker()->setFilterPassed (val_filterPassed);
  }



  IFilterWorker *AnaReentrantAlgorithm ::
  filterWorker () const
  {
    if (!m_filterWorker)
      throw std::logic_error ("no filter worker set on algorithm " + name());
    return m_filterWorker;
  }



  IWorker *AnaReentrantAlgorithm ::
  wk () const
  {
    if (!m_wk)
      throw std::logic_error ("no worker set on algorithm " + name());
    return m_wk;
  }
#endif



  ::StatusCode AnaReentrantAlgorithm ::
  initialize ()
  {
    return StatusCode::SUCCESS;
  }



  ::StatusCode AnaReentrantAlgorithm ::
  execute (const EventContext& /*ctx*/)
  {
    return StatusCode::SUCCESS;
  }



  ::StatusCode AnaReentrantAlgorithm ::
  finalize ()
  {
    return StatusCode::SUCCESS;
  }



#ifdef XAOD_STANDALONE
  ::StatusCode AnaReentrantAlgorithm ::
  sysInitialize ()
  {
    return initialize ();
  }



  ::StatusCode AnaReentrantAlgorithm ::
  sysExecute (const EventContext& ctx)
  {
    return execute (ctx);
  }



  ::StatusCode AnaReentrantAlgorithm ::
  sysFinalize ()
  {
    return finalize ();
  }



  asg::SgTEvent *AnaReentrantAlgorithm ::
  evtStore () const
  {
    if (!m_evtStore)
      throw std::logic_error ("no evtStore set on algorithm " + name());
    return m_evtStore;
  }



  void AnaReentrantAlgorithm ::
  setEvtStore (asg::SgTEvent *val_evtStore)
  {
    if (m_evtStore)
      throw std::logic_error ("set evtStore twice on algorithm " + name());
    m_evtStore = val_evtStore;
  }



  void AnaReentrantAlgorithm ::
  setFilterWorker (IFilterWorker *val_filterWorker)
  {
    if (m_filterWorker)
      throw std::logic_error ("set filter worker twice on algorithm " + name());
    m_filterWorker = val_filterWorker;
  }



  void AnaReentrantAlgorithm ::
  setWk (IWorker *val_wk)
  {
    if (m_wk)
      throw std::logic_error ("set wk twice on algorithm " + name());
    m_wk = val_wk;
  }
#endif
}
