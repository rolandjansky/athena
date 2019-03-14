/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoop/Worker.h>

#include <EventLoop/Algorithm.h>
#include <EventLoop/AlgorithmStateModule.h>
#include <EventLoop/Driver.h>
#include <EventLoop/EventCountModule.h>
#include <EventLoop/EventRange.h>
#include <EventLoop/FileExecutedModule.h>
#include <EventLoop/Job.h>
#include <EventLoop/LeakCheckModule.h>
#include <EventLoop/MessageCheck.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/StopwatchModule.h>
#include <EventLoop/TEventModule.h>
#include <EventLoop/OutputStreamData.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/RootUtils.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/DiskWriter.h>
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/MetaObject.h>
#include <SampleHandler/ToolsOther.h>
#include <TFile.h>
#include <TH1.h>
#include <TTree.h>

//
// method implementations
//

namespace EL
{
  void Worker ::
  testInvariant () const
  {
    RCU_INVARIANT (this != nullptr);
    for (std::size_t iter = 0, end = m_algs.size(); iter != end; ++ iter)
    {
      RCU_INVARIANT (m_algs[iter].m_algorithm != nullptr);
      RCU_INVARIANT (m_algs[iter]->wk() == this);
    }
  }



  Worker ::
  ~Worker ()
  {
    RCU_DESTROY_INVARIANT (this);
  }



  void Worker ::
  addOutput (TObject *output_swallow)
  {
    std::auto_ptr<TObject> output (output_swallow);

    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE_SOFT (output_swallow != 0);

    RCU::SetDirectory (output_swallow, 0);
    ModuleData::addOutput (std::move (output));
  }



  void Worker ::
  addOutputList (const std::string& name, TObject *output_swallow)
  {
    std::auto_ptr<TObject> output (output_swallow);

    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE_SOFT (output_swallow != 0);

    RCU::SetDirectory (output_swallow, 0);
    std::auto_ptr<TList> list (new TList);
    list->SetName (name.c_str());
    list->Add (output.release());
    addOutput (list.release());
  }



  TH1 *Worker ::
  getOutputHist (const std::string& name) const
  {
    RCU_READ_INVARIANT (this);

    TH1 *result = m_histOutput->getOutputHist (name);
    if (result == nullptr) RCU_THROW_MSG ("unknown output histogram: " + name);
    return result;
  }



  TFile *Worker ::
  getOutputFile (const std::string& label) const
  {
    RCU_READ_INVARIANT (this);
    TFile *result = getOutputFileNull (label);
    if (result == 0)
      RCU_THROW_MSG ("no output dataset defined with label: " + label);
    return result;
  }



  TFile *Worker ::
  getOutputFileNull (const std::string& label) const
  {
    RCU_READ_INVARIANT (this);
    auto iter = m_outputs.find (label);
    if (iter == m_outputs.end())
      return 0;
    return iter->second.file();
  }



  ::StatusCode Worker::
  addTree( const TTree& tree, const std::string& stream )
  {
    using namespace msgEventLoop;
    RCU_READ_INVARIANT( this );

    auto outputIter = m_outputs.find (stream);
    if (outputIter == m_outputs.end())
    {
      ANA_MSG_ERROR ( "No output file with stream name \"" + stream +
                      "\" found" );
      return ::StatusCode::FAILURE;
    }

    outputIter->second.addClone (tree);

    // Return gracefully:
    return ::StatusCode::SUCCESS;
  }



  TTree *Worker::
  getOutputTree( const std::string& name, const std::string& stream ) const
  {
    using namespace msgEventLoop;
    RCU_READ_INVARIANT( this );

    auto outputIter = m_outputs.find (stream);
    if (outputIter == m_outputs.end())
    {
      RCU_THROW_MSG ( "No output file with stream name \"" + stream
                      + "\" found" );
    }

    TTree *result = outputIter->second.getOutputTree( name );
    if( result == nullptr ) {
      RCU_THROW_MSG ( "No tree with name \"" + name + "\" in stream \"" +
                      stream + "\"" );
    }
    return result;
  }



  const SH::MetaObject *Worker ::
  metaData () const
  {
    RCU_READ_INVARIANT (this);
    return m_metaData;
  }



  TTree *Worker ::
  tree () const
  {
    RCU_READ_INVARIANT (this);
    return m_inputTree;
  }



  Long64_t Worker ::
  treeEntry () const
  {
    RCU_READ_INVARIANT (this);
    return m_inputTreeEntry;
  }



  TFile *Worker ::
  inputFile () const
  {
    RCU_READ_INVARIANT (this);
    return m_inputFile.get();
  }



  std::string Worker ::
  inputFileName () const
  {
    // no invariant used
    std::string path = inputFile()->GetName();
    auto split = path.rfind ('/');
    if (split != std::string::npos)
      return path.substr (split + 1);
    else
      return path;
  }



  TTree *Worker ::
  triggerConfig () const
  {
    RCU_READ_INVARIANT (this);
    return dynamic_cast<TTree*>(inputFile()->Get("physicsMeta/TrigConfTree"));
  }



  xAOD::TEvent *Worker ::
  xaodEvent () const
  {
    RCU_READ_INVARIANT (this);

    if (m_tevent == nullptr)
      RCU_THROW_MSG ("Job not configured for xAOD support");
    return m_tevent;
  }



  xAOD::TStore *Worker ::
  xaodStore () const
  {
    RCU_READ_INVARIANT (this);

    if (m_tstore == nullptr)
      RCU_THROW_MSG ("Job not configured for xAOD support");
    return m_tstore;
  }



  Algorithm *Worker ::
  getAlg (const std::string& name) const
  {
    RCU_READ_INVARIANT (this);
    for (auto& alg : m_algs)
    {
      if (alg->hasName (name))
	return alg.m_algorithm.get();
    }
    return 0;
  }



  void Worker ::
  skipEvent ()
  {
    RCU_CHANGE_INVARIANT (this);
    m_skipEvent = true;
  }



  bool Worker ::
  filterPassed () const noexcept
  {
    RCU_READ_INVARIANT (this);
    return !m_skipEvent;
  }



  void Worker ::
  setFilterPassed (bool val_filterPassed) noexcept
  {
    RCU_CHANGE_INVARIANT (this);
    m_skipEvent = !val_filterPassed;
  }



  Worker ::
  Worker ()
  {
    RCU_NEW_INVARIANT (this);
  }



  void Worker ::
  setMetaData (const SH::MetaObject *val_metaData)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (val_metaData != 0);

    m_metaData = val_metaData;
  }



  void Worker ::
  setOutputHist (const std::string& val_outputTarget)
  {
    RCU_CHANGE_INVARIANT (this);

    m_outputTarget = val_outputTarget;
  }



  void Worker ::
  setSegmentName (const std::string& val_segmentName)
  {
    RCU_CHANGE_INVARIANT (this);

    m_segmentName = val_segmentName;
  }



  void Worker ::
  setJobConfig (JobConfig&& jobConfig)
  {
    RCU_CHANGE_INVARIANT (this);
    for (std::unique_ptr<Algorithm>& alg : jobConfig.extractAlgorithms())
    {
      alg->m_wk = this;
      m_algs.push_back (std::move (alg));
    }
  }



  ::StatusCode Worker ::
  initialize ()
  {
    using namespace msgEventLoop;
    RCU_CHANGE_INVARIANT (this);

    const bool xAODInput = m_metaData->castBool (Job::optXAODInput, false);

    ANA_MSG_INFO ("xAODInput = " << xAODInput);
    if (xAODInput)
      m_modules.push_back (std::make_unique<Detail::TEventModule> ());
    m_modules.push_back (std::make_unique<Detail::LeakCheckModule> ());
    m_modules.push_back (std::make_unique<Detail::StopwatchModule> ());
    m_modules.push_back (std::make_unique<Detail::FileExecutedModule> ());
    m_modules.push_back (std::make_unique<Detail::EventCountModule> ());
    m_modules.push_back (std::make_unique<Detail::AlgorithmStateModule> ());

    if (m_outputs.find (Job::histogramStreamName) == m_outputs.end())
    {
      Detail::OutputStreamData data {
        m_outputTarget + "/hist-" + m_segmentName + ".root", "RECREATE"};
      ANA_CHECK (addOutputStream (Job::histogramStreamName, std::move (data)));
    }
    RCU_ASSERT (m_outputs.find (Job::histogramStreamName) != m_outputs.end());
    m_histOutput = &m_outputs.at(Job::histogramStreamName);

    m_jobStats = std::make_unique<TTree>
      ("EventLoop_JobStats", "EventLoop job statistics");
    m_jobStats->SetDirectory (nullptr);

    for (auto& module : m_modules)
      ANA_CHECK (module->preInitialize (*this));

    for (auto& module : m_modules)
      ANA_CHECK (module->onInitialize (*this));
    for (auto& module : m_modules)
      ANA_CHECK (module->postInitialize (*this));
    
    return ::StatusCode::SUCCESS;
  }



  ::StatusCode Worker ::
  finalize ()
  {
    using namespace msgEventLoop;

    RCU_CHANGE_INVARIANT (this);

    ANA_CHECK (openInputFile (""));
    for (auto& module : m_modules)
      ANA_CHECK (module->onFinalize (*this));
    for (auto& output : m_outputs)
    {
      if (output.first != Job::histogramStreamName)
      {
        output.second.saveOutput ();
        output.second.close ();
        std::string path = output.second.finalFileName ();
        if (!path.empty())
          addOutputList ("EventLoop_OutputStream_" + output.first, new TObjString (path.c_str()));
      }
    }
    for (auto& module : m_modules)
      ANA_CHECK (module->postFinalize (*this));
    if (m_jobStats->GetListOfBranches()->GetEntries() > 0)
    {
      if (m_jobStats->Fill() <= 0)
      {
        ANA_MSG_ERROR ("failed to fill the job statistics tree");
        return ::StatusCode::FAILURE;
      }
      ModuleData::addOutput (std::move (m_jobStats));
    }
    m_histOutput->saveOutput ();
    for (auto& module : m_modules)
      ANA_CHECK (module->onWorkerEnd (*this));
    m_histOutput->saveOutput ();
    m_histOutput->close ();
    ANA_MSG_INFO ("worker finished successfully");
    return ::StatusCode::SUCCESS;
  }



  ::StatusCode Worker ::
  processEvents (EventRange& eventRange)
  {
    using namespace msgEventLoop;

    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (!eventRange.m_url.empty());
    RCU_REQUIRE (eventRange.m_beginEvent >= 0);
    RCU_REQUIRE (eventRange.m_endEvent == EventRange::eof || eventRange.m_endEvent >= eventRange.m_beginEvent);

    ANA_CHECK (openInputFile (eventRange.m_url));

    if (eventRange.m_beginEvent > inputFileNumEntries())
    {
      ANA_MSG_ERROR ("first event (" << eventRange.m_beginEvent << ") points beyond last event in file (" << inputFileNumEntries() << ")");
      return ::StatusCode::FAILURE;
    }
    if (eventRange.m_endEvent == EventRange::eof)
    {
      eventRange.m_endEvent = inputFileNumEntries();
    } else if (eventRange.m_endEvent > inputFileNumEntries())
    {
      ANA_MSG_ERROR ("end event (" << eventRange.m_endEvent << ") points beyond last event in file (" << inputFileNumEntries() << ")");
      return ::StatusCode::FAILURE;
    }

    m_inputTreeEntry = eventRange.m_beginEvent;
    if (m_newInputFile)
    {
      m_newInputFile = false;
      for (auto& module : m_modules)
        ANA_CHECK (module->onNewInputFile (*this));
    }

    if (eventRange.m_beginEvent == 0)
    {
      for (auto& module : m_modules)
        ANA_CHECK (module->onFileExecute (*this));
    }

    ANA_MSG_INFO ("Processing events " << eventRange.m_beginEvent << "-" << eventRange.m_endEvent << " in file " << eventRange.m_url);

    for (uint64_t event = eventRange.m_beginEvent;
         event != uint64_t (eventRange.m_endEvent);
         ++ event)
    {
      m_inputTreeEntry = event;
      for (auto& module : m_modules)
        ANA_CHECK (module->onExecute (*this));
      ANA_CHECK (algsExecute ());
      m_eventsProcessed += 1;
      if (m_eventsProcessed % 10000 == 0)
        ANA_MSG_INFO ("Processed " << m_eventsProcessed << " events");
    }
    return ::StatusCode::SUCCESS;
  }



  ::StatusCode Worker ::
  openInputFile (std::string inputFileUrl)
  {
    using namespace msgEventLoop;

    RCU_CHANGE_INVARIANT (this);

    if (m_inputFileUrl == inputFileUrl)
      return ::StatusCode::SUCCESS;

    if (!m_inputFileUrl.empty())
    {
      if (m_newInputFile == false)
      {
        for (auto& module : m_modules)
          ANA_CHECK (module->onCloseInputFile (*this));
        for (auto& module : m_modules)
          ANA_CHECK (module->postCloseInputFile (*this));
      }
      m_newInputFile = false;
      m_inputTree = nullptr;
      m_inputFile.reset ();
      m_inputFileUrl.clear ();
    }

    if (inputFileUrl.empty())
      return ::StatusCode::SUCCESS;

    ANA_MSG_INFO ("Opening file " << inputFileUrl);
    std::unique_ptr<TFile> inputFile;
    try
    {
      inputFile = SH::openFile (inputFileUrl, *metaData());
    } catch (...)
    {
      Detail::report_exception ();
    }
    if (inputFile.get() == 0)
    {
      ANA_MSG_ERROR ("failed to open file " << inputFileUrl);
      for (auto& module : m_modules)
        module->reportInputFailure (*this);
      return ::StatusCode::FAILURE;
    }
    if (inputFile->IsZombie())
    {
      ANA_MSG_ERROR ("input file is a zombie: " << inputFileUrl);
      for (auto& module : m_modules)
        module->reportInputFailure (*this);
      return ::StatusCode::FAILURE;
    }

    TTree *tree = 0;
    const std::string treeName
      = m_metaData->castString (SH::MetaFields::treeName, SH::MetaFields::treeName_default);
    tree = dynamic_cast<TTree*>(inputFile->Get (treeName.c_str()));
    if (tree == nullptr)
    {
      ANA_MSG_INFO ("tree " << treeName << " not found in input file: " << inputFileUrl);
      ANA_MSG_INFO ("treating this like a tree with no events");
    }

    m_newInputFile = true;
    m_inputTree = tree;
    m_inputTreeEntry = 0;
    m_inputFile = std::move (inputFile);
    m_inputFileUrl = std::move (inputFileUrl);

    return ::StatusCode::SUCCESS;
  }



  ::StatusCode Worker ::
  addOutputStream (const std::string& label,
                   Detail::OutputStreamData data)
  {
    using namespace msgEventLoop;
    RCU_CHANGE_INVARIANT (this);

    if (m_outputs.find (label) != m_outputs.end())
    {
      ANA_MSG_ERROR ("output file already defined for label: " + label);
      return ::StatusCode::FAILURE;
    }
    m_outputs.insert (std::make_pair (label, std::move (data)));
    return ::StatusCode::SUCCESS;
  }



  ::StatusCode Worker ::
  algsExecute ()
  {
    using namespace msgEventLoop;
    RCU_CHANGE_INVARIANT (this);

    m_skipEvent = false;
    auto iter = m_algs.begin();
    try
    {
      for (auto end = m_algs.end();
           iter != end; ++ iter)
      {
        iter->m_executeCount += 1;
        if (iter->m_algorithm->execute() == StatusCode::FAILURE)
        {
          ANA_MSG_ERROR ("while calling execute() on algorithm " << iter->m_algorithm->GetName());
          return ::StatusCode::FAILURE;
        }

        if (m_skipEvent)
        {
          iter->m_skipCount += 1;
          return ::StatusCode::SUCCESS;
        }
      }
    } catch (...)
    {
      Detail::report_exception ();
      ANA_MSG_ERROR ("while calling execute() on algorithm " << iter->m_algorithm->GetName());
      return ::StatusCode::FAILURE;
    }

    /// rationale: this will make sure that the post-processing runs
    ///   for all algorithms for which the regular processing was run
    try
    {
      for (auto jter = m_algs.begin(), end = iter;
           jter != end && !m_skipEvent; ++ jter)
      {
        if (jter->m_algorithm->postExecute() == StatusCode::FAILURE)
        {
          ANA_MSG_ERROR ("while calling postExecute() on algorithm " << iter->m_algorithm->GetName());
          return ::StatusCode::FAILURE;
        }
      }
    } catch (...)
    {
      Detail::report_exception ();
      ANA_MSG_ERROR ("while calling postExecute() on algorithm " << iter->m_algorithm->GetName());
      return ::StatusCode::FAILURE;
    }

    if (m_firstEvent)
    {
      m_firstEvent = false;
      for (auto& module : m_modules)
        ANA_CHECK (module->postFirstEvent (*this));
    }

    return ::StatusCode::SUCCESS;
  }



  Long64_t Worker ::
  inputFileNumEntries () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE (inputFile() != 0);

    if (m_inputTree != 0)
      return m_inputTree->GetEntries();
    else
      return 0;
  }



  uint64_t Worker ::
  eventsProcessed () const noexcept
  {
    RCU_READ_INVARIANT (this);
    return m_eventsProcessed;
  }



  void Worker ::
  addModule (std::unique_ptr<Detail::Module> module)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (module != nullptr);
    m_modules.push_back (std::move (module));
  }
}
