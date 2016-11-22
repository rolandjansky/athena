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
#include "EventLoop/Job.h"
#include "EventLoop/OutputStream.h"

ClassImp(EL::GridWorker)

namespace EL
{

  void GridWorker::testInvariant() const {
    RCU_INVARIANT (this != 0);
  }

  GridWorker::GridWorker (const SH::MetaObject *meta, 
			  TList *output,
			  const TList& bigOutputs, 
			  const TList& algs, 
			  const std::string& location,
			  PandaRootTools& pandaTools)
    : Worker(meta, output), 
      m_meta(meta), m_location(location), m_pandaTools(pandaTools) {
        
    {//Create and register the "big" output files with base class
      TIter itr(&bigOutputs);
      TObject *obj = 0;
      while ((obj = itr())) {
	EL::OutputStream *os = dynamic_cast<EL::OutputStream*>(obj);
	if (os) {
	  const std::string name
	    = location + "/" + os->label() + ".root";
	  addOutputFile (os->label(), new TFile (name.c_str(), "RECREATE"));
	}
	else {
	  throw "ERROR: Bad input"; 
	}
      }
    }
    
    {//Register the algorithms with base class
      TIter itr(&algs);
      TObject *obj = 0;
      while ((obj = itr())) {
	if (obj->InheritsFrom("EL::Algorithm")) {
	  EL::Algorithm * alg = dynamic_cast<EL::Algorithm*>(obj);
	  if (alg) {
	    addAlg(alg);
	  }
	  else {
	    throw "ERROR: Bad input"; 
	  }
	}
	else {
	  throw "ERROR: Bad input"; 
	}
      }    
    }
    RCU_NEW_INVARIANT (this);
  }


  void GridWorker::run() {
    using namespace std;
    RCU_CHANGE_INVARIANT (this);

    for (TFile* f = m_pandaTools.OpenNextFile();
	 f != 0;
	 f = m_pandaTools.OpenNextFile()) {

      setInputFile (f);
      treeEntry(0);
      algsChangeInput();


      Long64_t nEvents = inputFileNumEntries();
      std::cout << "Running loop over " << nEvents << " events\n"; 
      for (Long64_t i = 0; i < nEvents; i++) {
	m_pandaTools.NotifyNewEvent();
	treeEntry(i);
	
	try {

	  algsExecute();

	}
	catch (exception& e) {
	  cout << "Caught exception while executing algorithm:\n"
	       << e.what() << "\n";
	  m_pandaTools.Fail();
	}
	catch (char const* e) {
	  cout << "Caught exception while executing algorithm:\n"
	       << e << "\n";
	  m_pandaTools.Fail();
	}
	catch (...) {
	  cout << "Caught unknown exception while executing algorithm";
	  m_pandaTools.Fail();
	}

      }    
      f->Close();    
    }

    algsFinalize();

    int nEvents = m_pandaTools.GetEventsRead();
    int nFiles = m_pandaTools.GetFilesRead();
    cout << "\nLoop finished. ";
    cout << "Read " << nEvents << " events in " << nFiles << " files.\n";

    m_pandaTools.NotifyJobFinished();
  }

}
