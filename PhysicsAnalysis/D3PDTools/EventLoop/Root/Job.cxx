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

#include <EventLoop/Job.h>

#include <memory>
#include <EventLoop/MessageCheck.h>
#include <EventLoop/AnaAlgorithmWrapper.h>
#include <EventLoop/Algorithm.h>
#include <EventLoop/OutputStream.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/CheckRootVersion.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/MetaNames.h>
#include <sstream>

//
// method implementations
//

namespace EL
{
  const std::string Job::optRemoveSubmitDir = "nc_EventLoop_RemoveSubmitDir";
  const std::string Job::optMaxEvents = "nc_EventLoop_MaxEvents";
  const std::string Job::optSkipEvents = "nc_EventLoop_SkipEvents";
  const std::string Job::optFilesPerWorker = "nc_EventLoop_FilesPerWorker";
  const std::string Job::optEventsPerWorker = "nc_EventLoop_EventsPerWorker";
  const std::string Job::optSubmitFlags = "nc_EventLoop_SubmitFlags";
  const std::string Job::optCondorConf = "nc_EventLoop_CondorConf";
  const std::string Job::optCacheLearnEntries = "nc_EventLoop_CacheLearnEntries";
  const std::string Job::optD3PDPerfStats = "nc_EventLoop_D3PDPerfStats";
  const std::string Job::optD3PDReadStats = "nc_EventLoop_D3PDReadStats";
  const std::string Job::optXAODPerfStats = "nc_EventLoop_XAODPerfStats";
  const std::string Job::optXAODReadStats = "nc_EventLoop_XAODReadStats";
  const std::string Job::optD3PDCacheMinEvent = "nc_EventLoop_D3PDCacheMinEvent";
  const std::string Job::optD3PDCacheMinEventFraction = "nc_EventLoop_D3PDCacheMinEventFraction";
  const std::string Job::optD3PDCacheMinByte = "nc_EventLoop_D3PDCacheMinByte";
  const std::string Job::optD3PDCacheMinByteFraction = "nc_EventLoop_D3PDCacheMinByteFraction";
  const std::string Job::optPerfTree = "nc_EventLoop_PerfTree";
  const std::string Job::optXAODInput = "nc_EventLoop_XAODInput";
  const std::string Job::optXaodAccessMode = "nc_EventLoop_XaodAccessMode";
  const std::string Job::optXaodAccessMode_branch = "branch";
  const std::string Job::optXaodAccessMode_class = "class";
  const std::string Job::optXaodAccessMode_athena = "athena";
  const std::string Job::optXAODSummaryReport = "nc_xaod_summary_report";
  const std::string Job::optPrintPerFileStats = "nc_print_per_file_stats";
  const std::string Job::optDisableMetrics = "nc_disable_metrics";
  const std::string Job::optResetShell = "nc_reset_shell";
  const std::string Job::optLocalNoUnsetup = "nc_local_no_unsetup";
  const std::string Job::optBackgroundProcess = "nc_background_process";
  const std::string Job::optGridDestSE = "nc_destSE";
  const std::string Job::optGridSite = "nc_site";
  const std::string Job::optGridCloud = "nc_cloud";
  const std::string Job::optGridExcludedSite = "nc_excludedSite";
  const std::string Job::optGridNGBPerJob = "nc_nGBPerJob";
  const std::string Job::optGridMemory = "nc_memory";
  const std::string Job::optGridMaxCpuCount = "nc_maxCpuCount";
  const std::string Job::optGridNFiles = "nc_nFiles";
  const std::string Job::optGridNFilesPerJob = "nc_nFilesPerJob";
  const std::string Job::optGridNJobs = "nc_nJobs";
  const std::string Job::optGridMaxFileSize = "nc_maxFileSize";
  const std::string Job::optGridMaxNFilesPerJob = "nc_maxNFilesPerJob";
  const std::string Job::optGridUseChirpServer = "nc_useChirpServer";
  const std::string Job::optGridExpress = "nc_express";
  const std::string Job::optGridNoSubmit = "nc_noSubmit";
  const std::string Job::optGridMergeOutput = "nc_mergeOutput";
  const std::string Job::optBatchSharedFileSystem = "nc_sharedFileSystem";
  const std::string Job::optBatchSlurmExtraConfigLines = "nc_SlurmExtraConfig";
  const std::string Job::optBatchSlurmWrapperExec = "nc_SlurmWrapperExec";
  const std::string Job::optBatchSetupCommand = "nc_BatchSetupCommand";
  const std::string Job::optTmpDir = "nc_tmpDir";
  const std::string Job::optRootVer = "nc_rootVer";
  const std::string Job::optCmtConfig = "nc_cmtConfig";
  const std::string Job::optGridDisableAutoRetry = "nc_disableAutoRetry";
  const std::string Job::optOfficial = "nc_official";
  const std::string Job::optVoms = "nc_voms";

  /// warning: this has to be synchronized with
  ///   SampleHandler::MetaFields.  I can't just copy it here, because
  ///   the order of initialization is undefined
  const std::string Job::optCacheSize = "nc_cache_size";

  const std::string Job::optRetries = SH::MetaNames::openRetries();
  const std::string Job::optRetriesWait = SH::MetaNames::openRetriesWait();

  const std::string Job::optMemResidentPerEventIncreaseLimit =
     "nc_resMemPerEventIncrease";
  const std::string Job::optMemVirtualPerEventIncreaseLimit =
     "nc_virtMemPerEventIncrease";
  const std::string Job::optMemResidentIncreaseLimit = "nc_resMemAbsIncrease";
  const std::string Job::optMemVirtualIncreaseLimit = "nc_virtMemAbsIncrease";
  const std::string Job::optMemFailOnLeak = "nc_failOnMemLeak";


  const std::string Job::histogramStreamName = "HISTOGRAM";


  void swap (Job& a, Job& b)
  {
    swap (a.m_sampleHandler, b.m_sampleHandler);
    a.m_jobConfig.swap (b.m_jobConfig);
    swap (a.m_output, b.m_output);
  }



  void Job ::
  testInvariant () const
  {
    RCU_INVARIANT (this);
  }



  Job ::
  Job ()
  {
    RCU::check_root_version ();

    RCU_NEW_INVARIANT (this);
  }



  Job ::
  Job (const Job& that)
    : m_sampleHandler ((RCU_READ_INVARIANT (&that),
			that.m_sampleHandler)),
      m_output (that.m_output),
      m_options (that.m_options),
      m_jobConfig (that.m_jobConfig)
  {
    RCU_NEW_INVARIANT (this);
  }



  Job ::
  ~Job ()
  {
    RCU_DESTROY_INVARIANT (this);
  }



  Job& Job ::
  operator = (const Job& that)
  {
    // no invariant used
    Job tmp (that);
    swap (*this, tmp);
    return *this;
  }



  const SH::SampleHandler& Job ::
  sampleHandler () const
  {
    RCU_READ_INVARIANT (this);
    return m_sampleHandler;
  }



  void Job ::
  sampleHandler (const SH::SampleHandler& val_sampleHandler)
  {
    RCU_CHANGE_INVARIANT (this);
    m_sampleHandler = val_sampleHandler;
  }



  void Job ::
  algsAdd (std::unique_ptr<Algorithm> val_algorithm)
  {
    using namespace msgEventLoop;

    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE_SOFT (val_algorithm != nullptr);


    std::string myname = val_algorithm->GetName();
    if (myname.empty() || algsHas (myname))
    {
      if (myname.empty())
        myname = "UnnamedAlgorithm";
      bool unique = false;
      for (unsigned iter = 1; !unique; ++ iter)
      {
        std::ostringstream str;
        str << myname << iter;
        if (!algsHas (str.str()))
        {
          myname = str.str();
          unique = true;
        }
      }
      if (strlen (val_algorithm->GetName()) > 0)
        ANA_MSG_WARNING ("renaming algorithm " << val_algorithm->GetName() << " to " << myname << " to make the name unique");
      val_algorithm->SetName (myname.c_str());
      if (val_algorithm->GetName() != myname)
      {
        std::ostringstream message;
        message << "failed to rename algorithm " << val_algorithm->GetName() << " to " << myname;
        RCU_THROW_MSG (message.str());
      }
    }

    ANA_CHECK_THROW (m_jobConfig.addAlgorithm (std::move (val_algorithm)));
  }



  void Job ::
  algsAdd (Algorithm *alg_swallow)
  {
    using namespace msgEventLoop;

    std::unique_ptr<Algorithm> alg (alg_swallow);

    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE_SOFT (alg_swallow != 0);

    std::string myname = alg_swallow->GetName();
    if (myname.empty() || algsHas (myname))
    {
      if (myname.empty())
        myname = "UnnamedAlgorithm";
      bool unique = false;
      for (unsigned iter = 1; !unique; ++ iter)
      {
        std::ostringstream str;
        str << myname << iter;
        if (!algsHas (str.str()))
        {
          myname = str.str();
          unique = true;
        }
      }
      if (strlen (alg_swallow->GetName()) > 0)
        ANA_MSG_WARNING ("renaming algorithm " << alg_swallow->GetName() << " to " << myname << " to make the name unique");
      alg_swallow->SetName (myname.c_str());
      if (alg_swallow->GetName() != myname)
      {
        std::ostringstream message;
        message << "failed to rename algorithm " << alg_swallow->GetName() << " to " << myname;
        RCU_THROW_MSG (message.str());
      }
    }

    alg->sysSetupJob (*this);
    ANA_CHECK_THROW (m_jobConfig.addAlgorithm (std::move (alg)));
  }



  void Job ::
  algsAdd (const AnaAlgorithmConfig& config)
  {
    // no invariant used
    algsAdd (new AnaAlgorithmWrapper (config));
  }



  void Job ::
  algsAddClone (const Algorithm& alg)
  {
    // no invariant used
    algsAdd (dynamic_cast<Algorithm*>(alg.Clone()));
  }



  bool Job ::
  algsHas (const std::string& name) const
  {
    RCU_READ_INVARIANT (this);
    return m_jobConfig.getAlgorithm (name) != nullptr;
  }



  Job::outputMIter Job ::
  outputBegin ()
  {
    RCU_READ_INVARIANT (this);
    return ( m_output.size() ? &m_output[0] : nullptr );
  }



  Job::outputIter Job ::
  outputBegin () const
  {
    RCU_READ_INVARIANT (this);
    return ( m_output.size() ? &m_output[0] : nullptr );
  }



  Job::outputMIter Job ::
  outputEnd ()
  {
    RCU_READ_INVARIANT (this);
    return ( m_output.size() ? &m_output[m_output.size()] : nullptr );
  }



  Job::outputIter Job ::
  outputEnd () const
  {
    RCU_READ_INVARIANT (this);
    return ( m_output.size() ? &m_output[m_output.size()] : nullptr );
  }



  void Job ::
  outputAdd (const OutputStream& val_output)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE2_SOFT (!outputHas (val_output.label()), ("trying to create two output streams with the label " + val_output.label() + "\nplease make sure that every output stream has a unique label").c_str());
    m_output.push_back (val_output);
  }



  bool Job ::
  outputHas (const std::string& name) const
  {
    RCU_CHANGE_INVARIANT (this);
    for (outputIter iter = outputBegin(),
	   end = outputEnd(); iter != end; ++ iter)
    {
      if (iter->label() == name)
	return true;
    }
    return false;
  }



  void Job ::
  useXAOD ()
  {
    RCU_CHANGE_INVARIANT (this);

    options()->setBool (Job::optXAODInput, true);
  }



  SH::MetaObject *Job ::
  options ()
  {
    RCU_READ_INVARIANT (this);
    return &m_options;
  }



  const SH::MetaObject *Job ::
  options () const
  {
    RCU_READ_INVARIANT (this);
    return &m_options;
  }



  const JobConfig& Job ::
  jobConfig () const noexcept
  {
    RCU_READ_INVARIANT (this);
    return m_jobConfig;
  }
}
