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

#include <iostream>
#include <string>
#include <exception>

#include <TROOT.h>
#include <TList.h>
#include <TFile.h>
#include <TObject.h>

#include "RootCoreUtils/Assert.h"
#include "RootCoreUtils/ThrowMsg.h"
#include "SampleHandler/SampleGrid.h"
#include "SampleHandler/MetaObject.h"
#include "SampleHandler/SampleHandler.h"
#include "EventLoop/Algorithm.h"
#include "EventLoop/JobConfig.h"
#include "EventLoopGrid/GridWorker.h"
#include "EventLoopGrid/PandaRootTools.h"
#include "EventLoop/Driver.h"
#include "EventLoop/OutputStream.h"

ClassImp(EL::GridWorker)

namespace EL
{

  void GridWorker::testInvariant() const {
    RCU_INVARIANT (this != 0);
  }

  GridWorker::GridWorker (const SH::MetaObject *meta, 
			  TList *output,
			  const std::string& location,
			  PandaRootTools& pandaTools)
    : m_meta(meta), m_location(location), m_pandaTools(pandaTools) {

    RCU_NEW_INVARIANT (this);
  }


  void GridWorker::run(JobConfig&& jobConfig, const TList& bigOutputs, const std::string& location) {
    using namespace std;
    using namespace msgEventLoop;
    RCU_CHANGE_INVARIANT (this);

    {//Create and register the "big" output files with base class
      TIter itr(&bigOutputs);
      TObject *obj = 0;
      while ((obj = itr())) {
	EL::OutputStream *os = dynamic_cast<EL::OutputStream*>(obj);
	if (os) {
	  const std::string name
	    = location + "/" + os->label() + ".root";
          Detail::OutputStreamData data;
          data.m_file = std::make_unique<TFile> (name.c_str(), "RECREATE");
          ANA_CHECK_THROW (addOutputStream (os->label(), std::move (data)));
	}
	else {
	  throw "ERROR: Bad input"; 
	}
      }
    }

    setJobConfig (std::move (jobConfig));

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



  void GridWorker ::
  execute (const std::string& sampleName)
  {
    using namespace msgEventLoop;

    ANA_MSG_INFO ("Running with ROOT version " << gROOT->GetVersion()
                  << " (" << gROOT->GetVersionDate() << ")");

    ANA_MSG_INFO ("Loading EventLoop grid job");


    TList bigOutputs;
    std::unique_ptr<JobConfig> jobConfig;
    SH::MetaObject *mo = 0;
    PandaRootTools pandaTools;

    try {
      std::unique_ptr<TFile> f (TFile::Open("jobdef.root"));
      if (f && !f->IsZombie()) {
	
	mo = dynamic_cast<SH::MetaObject*>(f->Get(sampleName.c_str()));
	if (!mo) {
	  throw "Could not read in sample meta object";
	}
	
        jobConfig.reset (dynamic_cast<JobConfig*>(f->Get("jobConfig")));
        if (jobConfig == nullptr)
          RCU_THROW_MSG ("failed to read jobConfig object");

        std::unique_ptr<TList> outs ((TList*)f->Get("outputs"));
	if (outs) {  
	  TIter itr(outs.get());
	  TObject *obj = 0;
	  while ((obj = itr())) {
	    EL::OutputStream * out = dynamic_cast<EL::OutputStream*>(obj);
	    if (out) {
	      bigOutputs.Add(out);
	    }
	    else {
	      throw "Encountered unexpected entry in list of outputs"; 
	    }
	  }
	}
	else {
	  throw "Could not read list of outputs"; 
	}
      }
      else {
	throw "Could not read jobdef"; 
      }

      f->Close();
      f.reset ();
    
      const std::string location = ".";

      TList output;
      EL::GridWorker worker(mo, 
			    &output, 
                            location, 
			    pandaTools);
      worker.setMetaData (mo);
      worker.setOutputHist (&output);
      
      ANA_MSG_INFO ("Starting EventLoop Grid worker");

      worker.run (std::move (*jobConfig), bigOutputs, location);

      ANA_MSG_INFO ("EventLoop Grid worker finished");
      ANA_MSG_INFO ("Saving output");

      EL::Driver::saveOutput (location, "output", output);    
    }
    catch (std::exception& e) {
      std::cout << "Aborting job due to internal GridWorker error:\n" << e.what() 
	   << "\nThe cause of this is probably a misconfigured job\n";
      pandaTools.Abort();
    }
    catch (char const* e) {
      std::cout << "Aborting job due to internal GridWorker error:\n"
	   << e << "\nThe cause of this is probably a misconfigured job\n";
      pandaTools.Abort();
    }
    catch (...) {
      std::cout << "Unknown exception caught by EventLoop GridWorker.\n";
      pandaTools.Abort();
    }
  }
}
