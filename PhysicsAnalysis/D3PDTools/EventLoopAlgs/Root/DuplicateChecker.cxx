/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoopAlgs/DuplicateChecker.h>
#include <EventLoopAlgs/Global.h>

#include <AsgTools/MsgStream.h>
#include <EventLoop/Job.h>
#include <EventLoop/IWorker.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/MetaNames.h>
#include <SampleHandler/Sample.h>
#include <TChain.h>
#include <TTree.h>
#include <fstream>
#include <xAODEventInfo/EventInfo.h>
#include <xAODRootAccess/TEvent.h>

//
// method implementations
//

ClassImp (EL::DuplicateChecker)

namespace EL
{
  void DuplicateChecker :: 
  testInvariant () const
  {
    RCU_INVARIANT (this != nullptr);
  }



  DuplicateChecker :: 
  DuplicateChecker ()
    : m_eventInfoName ("EventInfo")
  {
    RCU_NEW_INVARIANT (this);
  }



  const std::string& DuplicateChecker ::
  eventInfoName () const
  {
    RCU_READ_INVARIANT (this);
    return m_eventInfoName;
  }



  void DuplicateChecker ::
  setEventInfoName (const std::string& val_eventInfoName)
  {
    RCU_CHANGE_INVARIANT (this);
    m_eventInfoName = val_eventInfoName;
  }



  const std::string& DuplicateChecker ::
  outputTreeName () const
  {
    RCU_READ_INVARIANT (this);
    return m_outputTreeName;
  }



  void DuplicateChecker ::
  setOutputTreeName (const std::string& val_outputTreeName)
  {
    RCU_CHANGE_INVARIANT (this);
    m_outputTreeName = val_outputTreeName;
  }



  void DuplicateChecker ::
  addKnownDuplicate (const std::string& sampleName,
		     const std::string& fileName,
		     Long64_t entry, number_type runNumber,
		     number_type eventNumber)
  {
    RCU_CHANGE_INVARIANT (this);
    m_duplicates[std::make_pair (sampleName, fileName)].insert
      (std::make_pair (entry, std::make_pair (runNumber, eventNumber)));
  }



  void DuplicateChecker ::
  addKnownDuplicatesFile (const std::string& duplicatesFile)
  {
    std::ifstream file (duplicatesFile);
    std::string line;
    while (std::getline (file, line))
    {
      if (!line.empty() && line[0] != '#')
      {
	std::string sampleName, fileName;
	Long64_t entry;
	number_type runNumber, eventNumber;
	std::istringstream str (line);
	if (!(str >> sampleName >> fileName >> entry >> runNumber >> eventNumber))
	  RCU_THROW_MSG ("failed to parse line: " + line);
	addKnownDuplicate (sampleName, fileName, entry, runNumber, eventNumber);
      }
    }
  }



  bool DuplicateChecker ::
  processSummary (const std::string& submitdir, const std::string& treeName)
  {
    SH::SampleHandler sh;
    sh.load (submitdir + "/hist");
    sh.setMetaString (SH::MetaFields::treeName, treeName);
    return processSummary (sh, submitdir + "/duplicates");
  }



  bool DuplicateChecker ::
  processSummary (const SH::SampleHandler& sh, const std::string& outputFile)
  {
    std::ofstream file (outputFile.c_str());
    bool success = true;
    for (SH::Sample *sample : sh)
    {
      std::set<std::pair<number_type,number_type>> known_list;
      std::set<std::pair<number_type,number_type>> processed_list;

      std::unique_ptr<TChain> tree (sample->makeTChain());
      std::string *inputFileName = nullptr;
      tree->SetBranchAddress ("fileName", &inputFileName);
      Long64_t inputFileIndex;
      tree->SetBranchAddress ("fileIndex", &inputFileIndex);
      number_type runNumber;
      tree->SetBranchAddress ("runNumber", &runNumber);
      number_type eventNumber;
      tree->SetBranchAddress ("eventNumber", &eventNumber);
      Bool_t processed;
      tree->SetBranchAddress ("processed", &processed);
      for (Long64_t entry = 0, entries = tree->GetEntries();
	   entry < entries; ++ entry)
      {
	if (tree->GetEntry (entry) < 0)
	  RCU_THROW_MSG ("failed to read tree entry");
	std::pair<number_type,number_type> id (runNumber, eventNumber);

	if (known_list.find (id) == known_list.end())
	  known_list.insert (id);
	else
	{
	  file << sample->name() << " " << *inputFileName << " "
	       << inputFileIndex << " " << runNumber << " "
	       << eventNumber << std::endl;
	}

	if (processed)
	{
	  if (processed_list.find (id) == processed_list.end())
	    processed_list.insert (id);
	  else
	  {
	    success = false;
	    std::cout << "ERROR: duplicate event processed: "
		      << sample->name() << " "
		      << runNumber << " " << eventNumber << std::endl;
	  }
	}
      }
      if (known_list.size() != processed_list.size())
      {
	RCU_ASSERT (processed_list.size() < known_list.size());
	success = false;
	for (auto& id : known_list)
	{
	  if (processed_list.find (id) == processed_list.end())
	  {
	    std::cout << "ERROR: event never processed: "
		      << sample->name() << " "
		      << runNumber << " " << eventNumber << std::endl;
	  }
	}
      }
    }
    return success;
  }



  StatusCode DuplicateChecker ::
  setupJob (Job& job)
  {
    RCU_CHANGE_INVARIANT (this);
    job.useXAOD ();
    return StatusCode::SUCCESS;
  }



  StatusCode DuplicateChecker ::
  changeInput (bool /*firstFile*/)
  {
    RCU_CHANGE_INVARIANT (this);
    m_inputFileName = wk()->inputFileName();

    auto iter = m_duplicates.find
      (std::make_pair (wk()->metaData()->castString (SH::MetaNames::sampleName()), m_inputFileName));
    if (iter != m_duplicates.end())
      m_currentDuplicates = &iter->second;
    else
      m_currentDuplicates = nullptr;
    return StatusCode::SUCCESS;
  }



  StatusCode DuplicateChecker ::
  initialize ()
  {
    RCU_CHANGE_INVARIANT (this);

    m_event = wk()->xaodEvent();

    if (!m_outputTreeName.empty())
    {
      m_outputTree = new TTree (m_outputTreeName.c_str(), "DuplicateChecker tree");
      wk()->addOutput (m_outputTree);
      m_outputTree->Branch ("fileName", &m_inputFileName);
      m_outputTree->Branch ("fileIndex", &m_inputFileIndex, "inputFileIndex/L");
      m_outputTree->Branch ("runNumber", &m_runNumber, "runNumber/i");
      m_outputTree->Branch ("eventNumber", &m_eventNumber, "eventNumber/i");
      m_outputTree->Branch ("processed", &m_processEvent, "processed/O");
    }

    return StatusCode::SUCCESS;
  }



  StatusCode DuplicateChecker ::
  execute ()
  {
    RCU_CHANGE_INVARIANT (this);

    read_run_event_number ();
    m_processEvent = true;

    const std::pair<number_type,number_type> id (m_runNumber, m_eventNumber);

    if (m_currentDuplicates != nullptr)
    {
      auto iter = m_currentDuplicates->find (wk()->treeEntry());
      if (iter != m_currentDuplicates->end())
      {
	if (id != iter->second)
	{
	  ATH_MSG_FATAL ("expected duplicate event at entry " << iter->first
			 << " to be run=" << iter->second.first << ",event="
			 << iter->second.second << " but found run="
			 << id.first << ",event=" << id.second);
	  return StatusCode::FAILURE;
	}
	m_processEvent = false;
      }
    }

    // implicitly skip duplicates in same file
    if (m_processEvent)
    {
      if (m_processed.find (id) != m_processed.end())
      {
	ATH_MSG_WARNING ("skipping duplicate event run=" << id.first
			 << " event=" << id.second << " in file "
			 << wk()->inputFileName() << " at event "
			 << wk()->treeEntry());
	m_processEvent = false;
      } else
	m_processed.insert (id);
    }
    if (m_processEvent == false)
      wk()->skipEvent ();
    if (m_outputTree)
    {
      m_inputFileIndex = wk()->treeEntry ();
      m_outputTree->Fill ();
      // m_inputFileName.clear ();
    }

    return StatusCode::SUCCESS;
  }



  void DuplicateChecker ::
  read_run_event_number ()
  {
    const xAOD::EventInfo *info = nullptr;
    if (m_event->retrieve (info, m_eventInfoName).isFailure())
      RCU_THROW_MSG ("didn't find event info");

    m_runNumber = info->runNumber();
    m_eventNumber = info->eventNumber();
  }
}
