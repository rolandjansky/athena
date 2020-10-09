/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AnaAlgorithm/AnaAlgorithm.h>

#include <AsgMessaging/MessageCheck.h>
#include <RootCoreUtils/Assert.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <stdexcept>

#ifdef XAOD_STANDALONE
#include <AnaAlgorithm/IFilterWorker.h>
#include <AnaAlgorithm/IHistogramWorker.h>
#include <AnaAlgorithm/ITreeWorker.h>
#endif

#ifndef XAOD_STANDALONE
#include <GaudiKernel/IIncidentSvc.h>
#include <GaudiKernel/ServiceHandle.h>
#endif

//
// method implementations
//

namespace EL
{
  AnaAlgorithm ::
  AnaAlgorithm (const std::string& name, 
                ISvcLocator*
#ifndef XAOD_STANDALONE
                pSvcLocator
#endif
                )
#ifdef XAOD_STANDALONE
    : AsgComponent (name)
    , m_inputMetaStore  (asg::SgTEventMeta::InputStore, nullptr)
    , m_outputMetaStore (asg::SgTEventMeta::OutputStore, nullptr)
#else
    : AthHistogramAlgorithm (name, pSvcLocator)
    , m_inputMetaStore  ("StoreGateSvc/InputMetaDataStore", name)
    , m_outputMetaStore ("StoreGateSvc/MetaDataStore", name)
#endif
  {
#ifdef XAOD_STANDALONE
    declareProperty ("RootStreamName", m_treeStreamName = "ANALYSIS",
                     "Name of the stream to put trees into");
#endif

    ANA_MSG_DEBUG ("AnaAlgorithm: " << name);
  }



  AnaAlgorithm ::
  ~AnaAlgorithm () noexcept
  {}



  AnaAlgorithm::MetaStorePtr_t AnaAlgorithm::inputMetaStore() const
  {
#ifdef XAOD_STANDALONE
     return &m_inputMetaStore;
#else
     return m_inputMetaStore;
#endif // XAOD_STANDALONE
  }



  AnaAlgorithm::MetaStorePtr_t AnaAlgorithm::outputMetaStore() const
  {
#ifdef XAOD_STANDALONE
     return &m_outputMetaStore;
#else
     return m_outputMetaStore;
#endif // XAOD_STANDALONE
  }



#ifdef XAOD_STANDALONE
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
    histogramWorker()->addOutput (hist.Clone());
    return ::StatusCode::SUCCESS;
  }



  TH1 *AnaAlgorithm ::
  hist (const std::string& name) const
  {
    return histogramWorker()->getOutputHist (name);
  }



  TH2 *AnaAlgorithm ::
  hist2d (const std::string& name) const
  {
    TH2 *hist = dynamic_cast<TH2*>(histogramWorker()->getOutputHist (name));
    if (hist == nullptr)
      throw std::runtime_error ("histogram not a 2d-histogram: " + name);
    return hist;
  }



  TH3 *AnaAlgorithm ::
  hist3d (const std::string& name) const
  {
    TH3 *hist = dynamic_cast<TH3*>(histogramWorker()->getOutputHist (name));
    if (hist == nullptr)
      throw std::runtime_error ("histogram not a 3d-histogram: " + name);
    return hist;
  }



  IHistogramWorker *AnaAlgorithm ::
  histogramWorker () const
  {
    if (!m_histogramWorker)
      throw std::logic_error ("no histogram worker set on algorithm " + name());
    return m_histogramWorker;
  }



  ::StatusCode AnaAlgorithm ::
  book (const TTree& tree)
  {
     ANA_CHECK_SET_TYPE( ::StatusCode );
     ANA_CHECK( treeWorker()->addTree( tree, m_treeStreamName ) );
     return ::StatusCode::SUCCESS;
  }



  TTree *AnaAlgorithm ::
  tree (const std::string& name) const
  {
     return treeWorker()->getOutputTree( name, m_treeStreamName );
  }



  ITreeWorker *AnaAlgorithm ::
  treeWorker () const
  {
     if( ! m_treeWorker ) {
        throw std::logic_error( "no tree worker set on algorithm " + name() );
     }
     return m_treeWorker;
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



  IWorker *AnaAlgorithm ::
  wk () const
  {
    if (!m_wk)
      throw std::logic_error ("no worker set on algorithm " + name());
    return m_wk;
  }
#endif



  StatusCode AnaAlgorithm ::
  requestFileExecute ()
  {
    m_hasFileExecute = true;

#ifndef XAOD_STANDALONE
    // Connect to the IncidentSvc:
    ServiceHandle< IIncidentSvc > incSvc( "IncidentSvc", name() );
    ATH_CHECK( incSvc.retrieve() );

    // Set up the right callback, but ensure we don't double-register
    // if we are called twice
    incSvc->removeListener( this, IncidentType::BeginInputFile );
    incSvc->addListener( this, IncidentType::BeginInputFile, 0, true );
#endif

    return StatusCode::SUCCESS;
  }



  StatusCode AnaAlgorithm ::
  requestBeginInputFile ()
  {
    m_hasBeginInputFile = true;

#ifndef XAOD_STANDALONE
    // Connect to the IncidentSvc:
    ServiceHandle< IIncidentSvc > incSvc( "IncidentSvc", name() );
    ATH_CHECK( incSvc.retrieve() );

    // Set up the right callback, but ensure we don't double-register
    // if we are called twice
    incSvc->removeListener( this, IncidentType::BeginInputFile );
    incSvc->addListener( this, IncidentType::BeginInputFile, 0, true );
#endif

    return StatusCode::SUCCESS;
  }



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



  ::StatusCode AnaAlgorithm ::
  fileExecute ()
  {
    return StatusCode::SUCCESS;
  }



  ::StatusCode AnaAlgorithm ::
  beginInputFile ()
  {
    return StatusCode::SUCCESS;
  }



#ifdef XAOD_STANDALONE
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



  ::StatusCode AnaAlgorithm ::
  sysFileExecute ()
  {
    if (m_hasFileExecute == false)
    {
      ANA_MSG_FATAL ("called fileExecute(), though it was not registered");
      return StatusCode::FAILURE;
    }
    return fileExecute ();
  }



  ::StatusCode AnaAlgorithm ::
  sysBeginInputFile ()
  {
    if (m_hasBeginInputFile == false)
    {
      ANA_MSG_FATAL ("called beginInputFile(), though it was not registered");
      return StatusCode::FAILURE;
    }
    return beginInputFile ();
  }



  void AnaAlgorithm ::
  setEvtStore (asg::SgTEvent *val_evtStore)
  {
    if (m_evtStore)
      throw std::logic_error ("set evtStore twice on algorithm " + name());
    m_evtStore = val_evtStore;
    m_inputMetaStore = asg::SgTEventMeta (asg::SgTEventMeta::InputStore,
                                          val_evtStore->event());
    m_outputMetaStore = asg::SgTEventMeta (asg::SgTEventMeta::OutputStore,
                                           val_evtStore->event());
  }



  void AnaAlgorithm ::
  setHistogramWorker (IHistogramWorker *val_histogramWorker)
  {
    if (m_histogramWorker)
      throw std::logic_error ("set histogram worker twice on algorithm " + name());
    m_histogramWorker = val_histogramWorker;
  }



  void AnaAlgorithm ::
  setTreeWorker (ITreeWorker *val_treeWorker)
  {
     if( m_treeWorker ) {
        throw std::logic_error( "set tree worker twice on algorithm " +
                                name() );
     }
     m_treeWorker = val_treeWorker;
  }



  void AnaAlgorithm ::
  setFilterWorker (IFilterWorker *val_filterWorker)
  {
    if (m_filterWorker)
      throw std::logic_error ("set filter worker twice on algorithm " + name());
    m_filterWorker = val_filterWorker;
  }



  void AnaAlgorithm ::
  setWk (IWorker *val_wk)
  {
    if (m_wk)
      throw std::logic_error ("set wk twice on algorithm " + name());
    m_wk = val_wk;
  }



  bool AnaAlgorithm ::
  hasFileExecute () const noexcept
  {
    return m_hasFileExecute;
  }



  bool AnaAlgorithm ::
  hasBeginInputFile () const noexcept
  {
    return m_hasBeginInputFile;
  }
#endif



#ifndef XAOD_STANDALONE
  void AnaAlgorithm ::
  handle (const Incident& inc)
  {
    if (inc.type() == IncidentType::BeginInputFile)
    {
      if (m_hasBeginInputFile)
        ANA_CHECK_THROW (beginInputFile ());
      if (m_hasFileExecute)
        ANA_CHECK_THROW (fileExecute ());
    } else
    {
      ATH_MSG_WARNING( "Unknown incident type received: " << inc.type() );
    }
  }
#endif
}
