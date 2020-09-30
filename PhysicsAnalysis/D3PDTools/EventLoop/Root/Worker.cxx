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
#include <EventLoop/BatchJob.h>
#include <EventLoop/BatchSample.h>
#include <EventLoop/BatchSegment.h>
#include <EventLoop/Driver.h>
#include <EventLoop/EventCountModule.h>
#include <EventLoop/EventRange.h>
#include <EventLoop/FileExecutedModule.h>
#include <EventLoop/GridReportingModule.h>
#include <EventLoop/Job.h>
#include <EventLoop/LeakCheckModule.h>
#include <EventLoop/MessageCheck.h>
#include <EventLoop/OutputStream.h>
#include <EventLoop/OutputStreamData.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/StopwatchModule.h>
#include <EventLoop/TEventModule.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/RootUtils.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/DiskOutput.h>
#include <SampleHandler/DiskWriter.h>
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/MetaObject.h>
#include <SampleHandler/Sample.h>
#include <SampleHandler/SamplePtr.h>
#include <SampleHandler/ToolsOther.h>
#include <TFile.h>
#include <TH1.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TTree.h>
#include <fstream>
#include <memory>
#include <xAODRootAccess/LoadDictionaries.h>

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
    std::unique_ptr<TObject> output (output_swallow);

    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE_SOFT (output_swallow != 0);

    RCU::SetDirectory (output_swallow, 0);
    ModuleData::addOutput (std::move (output));
  }



  void Worker ::
  addOutputList (const std::string& name, TObject *output_swallow)
  {
    std::unique_ptr<TObject> output (output_swallow);

    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE_SOFT (output_swallow != 0);

    RCU::SetDirectory (output_swallow, 0);
    std::unique_ptr<TList> list (new TList);
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

    ANA_MSG_INFO ("calling firstInitialize on all modules");
    for (auto& module : m_modules)
      ANA_CHECK (module->firstInitialize (*this));
    ANA_MSG_INFO ("calling preFileInitialize on all modules");
    for (auto& module : m_modules)
      ANA_CHECK (module->preFileInitialize (*this));
    
    return ::StatusCode::SUCCESS;
  }



  ::StatusCode Worker ::
  finalize ()
  {
    using namespace msgEventLoop;

    RCU_CHANGE_INVARIANT (this);

    if (m_algorithmsInitialized == false)
    {
      ANA_MSG_ERROR ("algorithms never got initialized");
      return StatusCode::FAILURE;
    }

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

    if (m_algorithmsInitialized == false)
    {
      for (auto& module : m_modules)
        ANA_CHECK (module->onInitialize (*this));
      m_algorithmsInitialized = true;
    }

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
      if (algsExecute().isFailure())
      {
        ANA_MSG_ERROR ("processing event " << treeEntry() << " on file " << inputFileName());
        return ::StatusCode::FAILURE;
      }
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
    if (data.file() == nullptr)
    {
      ANA_MSG_ERROR ("output stream does not have a file attached");
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



  ::StatusCode Worker ::
  directExecute (const SH::SamplePtr& sample, const Job& job,
                 const std::string& location, const SH::MetaObject& options)
  {
    using namespace msgEventLoop;
    RCU_CHANGE_INVARIANT (this);

    SH::MetaObject meta (*sample->meta());
    meta.fetchDefaults (options);

    setMetaData (&meta);
    setOutputHist (location);
    setSegmentName (sample->name());

    ANA_MSG_INFO ("Running sample: " << sample->name());

    setJobConfig (JobConfig (job.jobConfig()));

    for (Job::outputIter out = job.outputBegin(),
           end = job.outputEnd(); out != end; ++ out)
    {
      Detail::OutputStreamData data {
        out->output()->makeWriter (sample->name(), "", ".root")};
      ANA_CHECK (addOutputStream (out->label(), std::move (data)));
    }

    ANA_CHECK (initialize ());

    Long64_t maxEvents
      = metaData()->castDouble (Job::optMaxEvents, -1);
    Long64_t skipEvents
      = metaData()->castDouble (Job::optSkipEvents, 0);

    std::vector<std::string> files = sample->makeFileList();
    for (const std::string& fileName : files)
    {
      EventRange eventRange;
      eventRange.m_url = fileName;
      if (skipEvents == 0 && maxEvents == -1)
      {
        ANA_CHECK (processEvents (eventRange));
      } else
      {
        // just open the input file to inspect it
        ANA_CHECK (openInputFile (fileName));
        eventRange.m_endEvent = inputFileNumEntries();

        if (skipEvents != 0 && skipEvents >= eventRange.m_endEvent)
        {
          skipEvents -= eventRange.m_endEvent;
          continue;
        }
        eventRange.m_beginEvent = skipEvents;
        skipEvents = 0;

        if (maxEvents != -1)
        {
          if (eventRange.m_endEvent > eventRange.m_beginEvent + maxEvents)
            eventRange.m_endEvent = eventRange.m_beginEvent + maxEvents;
          maxEvents -= eventRange.m_endEvent - eventRange.m_beginEvent;
          assert (maxEvents >= 0);
        }
        ANA_CHECK (processEvents (eventRange));
        if (maxEvents == 0)
          break;
      }
    }
    ANA_CHECK (finalize ());
    return ::StatusCode::SUCCESS;
  }



  ::StatusCode Worker ::
  batchExecute (unsigned job_id, const char *confFile)
  {
    using namespace msgEventLoop;
    RCU_CHANGE_INVARIANT (this);

    try
    {
      std::unique_ptr<TFile> file (TFile::Open (confFile, "READ"));
      if (file.get() == nullptr || file->IsZombie())
      {
        ANA_MSG_ERROR ("failed to open file: " << confFile);
        return ::StatusCode::FAILURE;
      }

      // This is a temporary (30 Sep 20) hot-fix to ensure that
      // `LoadDictionaries()` gets called before any algorithm gets
      // instantiated.  This is not ideal.  To do it proper, the
      // algorithms should be loaded at a later point, instead of
      // together with `BatchJob`.  Or `BatchJob` should be loaded
      // later and the modules should be set up without relying on
      // `BatchJob`.  Either way, `BatchJob` should be split up, which
      // was anyways planned for unrelated reasons.
      ANA_CHECK (xAOD::LoadDictionaries());

      std::unique_ptr<BatchJob> job (dynamic_cast<BatchJob*>(file->Get ("job")));
      if (job.get() == nullptr)
      {
        ANA_MSG_ERROR ("failed to retrieve BatchJob object");
        return ::StatusCode::FAILURE;
      }

      if (job_id >= job->segments.size())
      {
        ANA_MSG_ERROR ("invalid job-id " << job_id << ", max is " << job->segments.size());
        return ::StatusCode::FAILURE;
      }
      BatchSegment *segment = &job->segments[job_id];
      RCU_ASSERT (segment->job_id == job_id);
      RCU_ASSERT (segment->sample < job->samples.size());
      BatchSample *sample = &job->samples[segment->sample];

      gSystem->Exec ("pwd");
      gSystem->MakeDirectory ("output");

      setMetaData (&sample->meta);
      setOutputHist (job->location + "/fetch");
      setSegmentName (segment->fullName);

      setJobConfig (JobConfig (job->job.jobConfig()));

      for (Job::outputIter out = job->job.outputBegin(),
             end = job->job.outputEnd(); out != end; ++ out)
      {
        Detail::OutputStreamData data {
          out->output()->makeWriter (segment->sampleName, segment->segmentName, ".root")};
        ANA_CHECK (addOutputStream (out->label(), std::move (data)));
      }

      Long64_t beginFile = segment->begin_file;
      Long64_t endFile   = segment->end_file;
      Long64_t lastFile  = segment->end_file;
      RCU_ASSERT (beginFile <= endFile);
      Long64_t beginEvent = segment->begin_event;
      Long64_t endEvent   = segment->end_event;
      if (endEvent > 0) endFile += 1;

      ANA_CHECK (initialize ());

      for (Long64_t file = beginFile; file != endFile; ++ file)
      {
        RCU_ASSERT (std::size_t(file) < sample->files.size());
        EventRange eventRange;
        eventRange.m_url = sample->files[file];
        eventRange.m_beginEvent = (file == beginFile ? beginEvent : 0);
        eventRange.m_endEvent = (file == lastFile ? endEvent : EventRange::eof);
        ANA_CHECK (processEvents (eventRange));
      }
      ANA_CHECK (finalize ());

      std::ostringstream job_name;
      job_name << job_id;
      std::ofstream completed ((job->location + "/status/completed-" + job_name.str()).c_str());
      return ::StatusCode::SUCCESS;
    } catch (...)
    {
      Detail::report_exception ();
      return ::StatusCode::FAILURE;
    }
  }



  ::StatusCode Worker ::
  gridExecute (const std::string& sampleName)
  {
    using namespace msgEventLoop;
    RCU_CHANGE_INVARIANT (this);

    ANA_MSG_INFO ("Running with ROOT version " << gROOT->GetVersion()
                  << " (" << gROOT->GetVersionDate() << ")");

    ANA_MSG_INFO ("Loading EventLoop grid job");


    TList bigOutputs;
    std::unique_ptr<JobConfig> jobConfig;
    SH::MetaObject *mo = 0;

    std::unique_ptr<TFile> f (TFile::Open("jobdef.root"));
    if (f == nullptr || f->IsZombie()) {
      ANA_MSG_ERROR ("Could not read jobdef");
      return ::StatusCode::FAILURE;
    }

    // This is a temporary (30 Sep 20) hot-fix to ensure that
    // `LoadDictionaries()` gets called before any algorithm gets
    // instantiated.  This is not ideal.  To do it proper, the
    // algorithms should be loaded at a later point, instead of
    // together with `JobConfig`.  Or `JobConfig` should be loaded
    // later and the modules should be set up without relying on
    // `JobConfig`.  Either way, `JobConfig` should be split up, which
    // was anyways planned for unrelated reasons.
    ANA_CHECK (xAOD::LoadDictionaries());

    mo = dynamic_cast<SH::MetaObject*>(f->Get(sampleName.c_str()));
    if (!mo) {
      ANA_MSG_ERROR ("Could not read in sample meta object");
      return ::StatusCode::FAILURE;
    }

    jobConfig.reset (dynamic_cast<JobConfig*>(f->Get("jobConfig")));
    if (jobConfig == nullptr)
    {
      ANA_MSG_ERROR ("failed to read jobConfig object");
      return ::StatusCode::FAILURE;
    }

    {
      std::unique_ptr<TList> outs ((TList*)f->Get("outputs"));
      if (outs == nullptr)
      {
        ANA_MSG_ERROR ("Could not read list of outputs");
        return ::StatusCode::FAILURE;
      }

      TIter itr(outs.get());
      TObject *obj = 0;
      while ((obj = itr())) {
        EL::OutputStream * out = dynamic_cast<EL::OutputStream*>(obj);
        if (out) {
          bigOutputs.Add(out);
        }
        else {
          ANA_MSG_ERROR ("Encountered unexpected entry in list of outputs");
          return ::StatusCode::FAILURE;
        }
      }
    }

    f->Close();
    f.reset ();

    const std::string location = ".";

    setMetaData (mo);
    setOutputHist (location);
    setSegmentName ("output");

    ANA_MSG_INFO ("Starting EventLoop Grid worker");

    {//Create and register the "big" output files with base class
      TIter itr(&bigOutputs);
      TObject *obj = 0;
      while ((obj = itr())) {
        EL::OutputStream *os = dynamic_cast<EL::OutputStream*>(obj);
        if (os == nullptr)
        {
          ANA_MSG_ERROR ("Bad input");
          return ::StatusCode::FAILURE;
        }
        {
          Detail::OutputStreamData data {
            location + "/" + os->label() + ".root", "RECREATE"};
          ANA_CHECK (addOutputStream (os->label(), std::move (data)));
        }
      }
    }

    setJobConfig (std::move (*jobConfig));

    addModule (std::make_unique<Detail::GridReportingModule> ());
    ANA_CHECK (initialize());

    std::vector<std::string> fileList; 
    {
      std::ifstream infile("input.txt");
      while (infile) {
        std::string sLine;
        if (!getline(infile, sLine)) break;
        std::istringstream ssLine(sLine);
        while (ssLine) {
          std::string sFile;
          if (!getline(ssLine, sFile, ',')) break;
          fileList.push_back(sFile);
        }
      }
    } 
    if (fileList.size() == 0) {
      ANA_MSG_ERROR ("no input files provided");
      //User was expecting input after all.
      gSystem->Exit(EC_BADINPUT);
    }

    for (const std::string& file : fileList)
    {
      EventRange eventRange;
      eventRange.m_url = file;

      ANA_CHECK (processEvents (eventRange));
    }

    ANA_CHECK (finalize ());

    int nEvents = eventsProcessed();
    int nFiles = fileList.size();
    ANA_MSG_INFO ("Loop finished.");
    ANA_MSG_INFO ("Read " << nEvents << " events in " << nFiles << " files.");

    gridNotifyJobFinished(eventsProcessed(), fileList);

    ANA_MSG_INFO ("EventLoop Grid worker finished");
    ANA_MSG_INFO ("Saving output");
    return ::StatusCode::SUCCESS;
  }

  void Worker::gridNotifyJobFinished(uint64_t eventsProcessed,
                                     const std::vector<std::string>& fileList) {
    // createJobSummary
    std::ofstream summaryfile("../AthSummary.txt");
    if (summaryfile.is_open()) {
      unsigned int nFiles = fileList.size();
      summaryfile << "Files read: " << nFiles << std::endl;
      for (unsigned int i = 0; i < nFiles; i++) {
        summaryfile << "  " << fileList.at(i) << std::endl;
      }      
      summaryfile << "Events Read:    " << eventsProcessed << std::endl;
      summaryfile.close();
    }
    else {
      //cout << "Failed to write summary file.\n";
    } 
  }
}
