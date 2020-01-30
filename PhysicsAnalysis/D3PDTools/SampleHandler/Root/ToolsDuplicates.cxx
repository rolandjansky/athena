/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <SampleHandler/ToolsDuplicates.h>

#include <memory>
#include <set>
#include <sstream>
#include <utility>
#include <TChain.h>
#include <TFile.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/Sample.h>
#include <SampleHandler/SampleHandler.h>

//
// method implementations
//

namespace SH
{
  namespace
  {
    /// description: the type for run-event number pairs
    typedef std::pair<UInt_t,UInt_t> RunEvent;

    /// description: the type for lists of run-event numbers
    typedef std::set<RunEvent> RunEventList;



    /// description: the different names for run numbers
    /// guarantee: strong
    /// failures: out of memory II
    const std::set<std::string>& runNames ()
    {
      static std::set<std::string> result;
      if (result.empty())
      {
	result.insert ("RunNumber");
	result.insert ("runNumber");
      }
      return result;
    }



    /// description: the different names for event numbers
    /// guarantee: strong
    /// failures: out of memory II
    const std::set<std::string>& eventNames ()
    {
      static std::set<std::string> result;
      if (result.empty())
      {
	result.insert ("EventNumber");
	result.insert ("eventNumber");
      }
      return result;
    }



    /// effects: find which of the names is present in the n-tuple
    /// guarantee: strong
    /// failures: out of memory II
    /// failures: names not found
    std::string
    findBranch (const TTree& tree, const std::set<std::string>& names)
    {
      TObjArray *branches = const_cast<TTree&>(tree).GetListOfBranches();

      for (std::set<std::string>::const_iterator name = names.begin(),
	     end = names.end(); name != end; ++ name)
      {
	if (branches->FindObject (name->c_str()) != 0)
	  return *name;
      }
      RCU_THROW_MSG ("failed to find branch of valid name");
      return std::string (); // compiler dummy
    }



    /// effects: check the given tree for duplicate events and then
    ///   print them out, as well as accumulating them
    /// guarantee: basic, may print partially
    /// failures: out of memory III
    /// failures: i/o errors
    void printDuplicateEvents (TTree& tree, RunEventList& list)
    {
      const Long64_t nentries = tree.GetEntries();
      if (nentries < 0)
	RCU_THROW_MSG ("failed to read number of events from n-tuple");
      if (nentries == 0)
	return;

      const std::string runName = findBranch (tree, runNames());
      const std::string eventName = findBranch (tree, eventNames());

      tree.SetBranchStatus ("*", 0);
      UInt_t run = 0;
      tree.SetBranchStatus (runName.c_str(), 1);
      tree.SetBranchAddress (runName.c_str(), static_cast<void*>(&run));
      UInt_t event = 0;
      tree.SetBranchStatus (eventName.c_str(), 1);
      tree.SetBranchAddress (eventName.c_str(), static_cast<void*>(&event));

      tree.SetCacheSize (10 * 1024 * 1024);
      for (Long64_t entry = 0;
	   entry < nentries; ++ entry)
      {
	if (tree.GetEntry (entry) < 0)
	  RCU_THROW_MSG ("failed to read event");
	RunEvent runEvent (run, event);
	if (list.find (runEvent) == list.end())
	{
	  list.insert (runEvent);
	} else
	{
	  std::ostringstream message;
	  message << "duplicate event run=" << run << " event=" << event
		  << " file=" << tree.GetCurrentFile()->GetName();
	  
	  RCU_WARN_MSG (message.str());
	}
      }
    }
  }



  void printDuplicateEvents (const Sample& sample)
  {
    RunEventList list;
    std::unique_ptr<TChain> chain (sample.makeTChain ());
    printDuplicateEvents (*chain, list);
  }



  void printDuplicateEventsSplit (const SampleHandler& sh)
  {
    for (SampleHandler::iterator sample = sh.begin(),
	   end = sh.end(); sample != end; ++ sample)
    {
      printDuplicateEvents (**sample);
    }
  }



  void printDuplicateEventsJoint (const SampleHandler& sh)
  {
    RunEventList list;
    for (SampleHandler::iterator sample = sh.begin(),
	   end = sh.end(); sample != end; ++ sample)
    {
      std::unique_ptr<TChain> chain ((*sample)->makeTChain ());
      printDuplicateEvents (*chain, list);
    }
  }
}
