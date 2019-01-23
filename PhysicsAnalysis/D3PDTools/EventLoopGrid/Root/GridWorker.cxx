/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "EventLoopGrid/GridWorker.h"

#include <memory>
#include <iostream>
#include <exception>

#include <TFile.h>
#include <TTree.h>
#include <TCollection.h>

#include "RootCoreUtils/Assert.h"
#include "RootCoreUtils/ThrowMsg.h"
#include "SampleHandler/Sample.h"
#include "EventLoop/Algorithm.h"
#include "EventLoop/EventRange.h"
#include "EventLoop/Job.h"
#include "EventLoop/MessageCheck.h"
#include "EventLoop/OutputStream.h"
#include "EventLoopGrid/GridReportingModule.h"

ClassImp(EL::GridWorker)

namespace EL
{

  void GridWorker::testInvariant() const {
    RCU_INVARIANT (this != 0);
  }

  GridWorker::GridWorker (const SH::MetaObject *meta, 
			  TList *output,
			  const TList& bigOutputs, 
                          JobConfig&& jobConfig, 
			  const std::string& location,
			  PandaRootTools& pandaTools)
    : Worker(meta, output), 
      m_meta(meta), m_location(location), m_pandaTools(pandaTools) {

    using namespace msgEventLoop;

    {//Create and register the "big" output files with base class
      TIter itr(&bigOutputs);
      TObject *obj = 0;
      while ((obj = itr())) {
	EL::OutputStream *os = dynamic_cast<EL::OutputStream*>(obj);
	if (os) {
	  const std::string name
	    = location + "/" + os->label() + ".root";
	  ANA_CHECK_THROW (addOutputFile (os->label(), std::make_unique<TFile> (name.c_str(), "RECREATE")));
	}
	else {
	  throw "ERROR: Bad input"; 
	}
      }
    }

    setJobConfig (std::move (jobConfig));

    RCU_NEW_INVARIANT (this);
  }


  void GridWorker::run() {
    using namespace std;
    using namespace msgEventLoop;
    RCU_CHANGE_INVARIANT (this);

    addModule (std::make_unique<Detail::GridReportingModule> ());
    ANA_CHECK_THROW (initialize());

    for (TString fileUrl = m_pandaTools.getNextFile();
	 fileUrl != "";
	 fileUrl = m_pandaTools.getNextFile()) {

      EventRange eventRange;
      eventRange.m_url = fileUrl;

      try {
        if (processEvents (eventRange).isFailure()) {
          m_pandaTools.Fail(eventsProcessed());
        }

      }
      catch (exception& e) {
        cout << "Caught exception while executing algorithm:\n"
             << e.what() << "\n";
        m_pandaTools.Fail(eventsProcessed());
      }
      catch (char const* e) {
        cout << "Caught exception while executing algorithm:\n"
             << e << "\n";
        m_pandaTools.Fail(eventsProcessed());
      }
      catch (...) {
        cout << "Caught unknown exception while executing algorithm";
        m_pandaTools.Fail(eventsProcessed());
      }
    }    

    ANA_CHECK_THROW (finalize ());

    int nEvents = eventsProcessed();
    int nFiles = m_pandaTools.GetFilesRead();
    cout << "\nLoop finished. ";
    cout << "Read " << nEvents << " events in " << nFiles << " files.\n";

    m_pandaTools.NotifyJobFinished(eventsProcessed());
  }

}
