/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoop/D3PDReaderSvc.h>

#ifdef ROOTCORE_PACKAGE_D3PDReader

#include <memory>
#include <D3PDReader/Event.h>
#include <D3PDReader/D3PDPerfStats.h>
#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/MetaObject.h>

//
// method implementations
//

ClassImp (EL::D3PDReaderSvc)

namespace EL
{
  const std::string D3PDReaderSvc::name = "D3PDReader";



  void D3PDReaderSvc ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
    RCU_INVARIANT (m_cacheStats != 0 || m_cacheMode == 0);
    RCU_INVARIANT (m_cacheMode < 5);
  }



  D3PDReaderSvc ::
  D3PDReaderSvc ()
    : m_event (0), m_useStats (false),
      m_cacheStats (0), m_cacheMode (0), m_cacheArg (0)
  {
    RCU_NEW_INVARIANT (this);
  }



  D3PDReaderSvc ::
  ~D3PDReaderSvc ()
  {
    RCU_DESTROY_INVARIANT (this);

    delete m_event;
  }



  D3PDReader::Event *D3PDReaderSvc ::
  event () const
  {
    RCU_DESTROY_INVARIANT (this);

    if (m_event == 0)
      RCU_THROW_MSG ("D3PDReaderSvc not yet configured");
    D3PDReader::Event *const result = m_event;
    RCU_PROVIDE (result != 0);
    return result;
  }



  const char *D3PDReaderSvc ::
  GetName() const
  {
    RCU_READ_INVARIANT (this);
    return name.c_str();
  }



  StatusCode D3PDReaderSvc ::
  changeInput (bool firstFile)
  {
    RCU_CHANGE_INVARIANT (this);

    if (firstFile)
    {
      m_useStats = wk()->metaData()->getDouble (Job::optD3PDPerfStats, 0) != 0;
      if (m_useStats)
	D3PDReader::D3PDPerfStats::Instance()->Start();

      const TObject *object = wk()->metaData()->get (Job::optD3PDReadStats);
      m_cacheStats = dynamic_cast<const D3PDReader::D3PDReadStats*>(object);
      if (object != m_cacheStats)
	RCU_THROW_MSG (("job meta-data " + Job::optD3PDReadStats + " not of type D3PDReader::D3PDReadStats"));
      if (m_cacheStats)
      {
	double arg;
	if ((arg = wk()->metaData()->getDouble (Job::optD3PDCacheMinEvent,0)) > 0)
	  m_cacheMode = 1, m_cacheArg = arg;
	if ((arg = wk()->metaData()->getDouble (Job::optD3PDCacheMinEventFraction,0)) > 0)
	  m_cacheMode = 2, m_cacheArg = arg;
	if ((arg = wk()->metaData()->getDouble (Job::optD3PDCacheMinByte,0)) > 0)
	  m_cacheMode = 3, m_cacheArg = arg;
	if ((arg = wk()->metaData()->getDouble (Job::optD3PDCacheMinByteFraction,0)) > 0)
	  m_cacheMode = 4, m_cacheArg = arg;
      }
    }

    if (m_event == 0)
      m_event = new D3PDReader::Event;
    if (m_cacheStats)
    {
      switch (m_cacheMode)
      {
      case 0:
	m_cacheStats->AddToTreeCache (wk()->tree());
	break;
      case 1:
	m_cacheStats->AddToTreeCacheByEntries (wk()->tree(), Long64_t (m_cacheArg));
	break;
      case 2:
	m_cacheStats->AddToTreeCacheByEntryFrac (wk()->tree(), m_cacheArg);
	break;
      case 3:
	m_cacheStats->AddToTreeCacheByBytes (wk()->tree(), Long64_t (m_cacheArg));
	break;
      case 4:
	m_cacheStats->AddToTreeCacheByByteFrac (wk()->tree(), m_cacheArg);
	break;
      }
    }

    m_event->ReadFrom (wk()->tree());
    m_event->GetEntry (wk()->treeEntry());
    return StatusCode::SUCCESS;
  }



  StatusCode D3PDReaderSvc ::
  execute ()
  {
    RCU_CHANGE_INVARIANT (this);

    m_event->GetEntry (wk()->treeEntry());
    return StatusCode::SUCCESS;
  }



  EL::StatusCode D3PDReaderSvc ::
  finalize ()
  {
    if (m_useStats)
    {
      D3PDReader::D3PDPerfStats::Instance()->Stop();
      std::auto_ptr<D3PDReader::D3PDReadStats> stats
	(new D3PDReader::D3PDReadStats (m_event->GetStatistics()));
      stats->SetName (Job::optD3PDReadStats.c_str());
      wk()->addOutput (stats.release());
    }
    return StatusCode::SUCCESS;
  }
}

#endif
