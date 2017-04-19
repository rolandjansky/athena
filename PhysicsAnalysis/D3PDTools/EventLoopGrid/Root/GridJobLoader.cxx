/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <EventLoopGrid/GridJobLoader.h>

#include <iostream>
#include <string>
#include <exception>

#include <TROOT.h>
#include <TList.h>
#include <TFile.h>
#include <TObject.h>

#include "RootCoreUtils/Assert.h"
#include "SampleHandler/SampleGrid.h"
#include "SampleHandler/MetaObject.h"
#include "SampleHandler/SampleHandler.h"
#include "EventLoop/Algorithm.h"
#include "EventLoopGrid/GridWorker.h"
#include "EventLoopGrid/PandaRootTools.h"
#include "EventLoop/Driver.h"
#include "EventLoop/OutputStream.h"

ClassImp(EL::GridJobLoader)

namespace EL {

  void GridJobLoader::Run(const std::string& sampleName) {
    using namespace std;
    TList output;
    TList bigOutputs;
    TList algorithms;
    SH::MetaObject *mo = 0;
    PandaRootTools pandaTools;

    try {
      TFile *f = TFile::Open("jobdef.root");  
      if (f && !f->IsZombie()) {
	
	mo = dynamic_cast<SH::MetaObject*>(f->Get(sampleName.c_str()));
	if (!mo) {
	  throw "Could not read in sample meta object";
	}
	
	TList *algs = (TList*)f->Get("algorithms");
	if (algs) {  
	  TIter itr(algs);
	  TObject *obj = 0;
	  while ((obj = itr())) {
	    if (obj->InheritsFrom("EL::Algorithm")) {
	      EL::Algorithm * alg = dynamic_cast<EL::Algorithm*>(obj);
	      if (alg) {
		algorithms.Add(alg->Clone());
	      }
	      else {
		throw "Encountered unexpected entry in list of algorithms"; 
	      }
	    }
	    else {
	      throw "Encountered unexpected entry in list of algorithms"; 
	    }
	    //delete obj;
	  }
	}
	else {throw "Could not read list of algorithms"; }
	delete algs;


	TList *outs = (TList*)f->Get("outputs");
	if (outs) {  
	  TIter itr(outs);
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
	delete outs;
      }
      else {
	throw "Could not read jobdef"; 
      }

      f->Close();
      delete f; f=0;
    
      const std::string location = ".";

      EL::GridWorker worker(mo, 
			    &output, 
			    bigOutputs, 
			    algorithms, 
			    location, 
			    pandaTools);

      std::cout << "Starting EventLoop Grid worker\n";

      worker.run ();

      std::cout << "EventLoop Grid worker finished\n";
      std::cout << "Saving output\n";

      EL::Driver::saveOutput(location, "output", output);    
    }
    catch (exception& e) {
      cout << "Aborting job due to internal GridWorker error:\n" << e.what() 
	   << "\nThe cause of this is probably a misconfigured job\n";
      pandaTools.Abort();
    }
    catch (char const* e) {
      cout << "Aborting job due to internal GridWorker error:\n"
	   << e << "\nThe cause of this is probably a misconfigured job\n";
      pandaTools.Abort();
    }
    catch (...) {
      cout << "Unknown exception caught by EventLoop GridWorker.\n";
      pandaTools.Abort();
    }

  }
}
