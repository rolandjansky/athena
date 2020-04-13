/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <EventLoop/BatchDriver.h>

#include <EventLoop/BatchJob.h>
#include <EventLoop/BatchSample.h>
#include <EventLoop/BatchSegment.h>
#include <EventLoop/ManagerData.h>
#include <EventLoop/ManagerStep.h>
#include <EventLoop/MessageCheck.h>
#include <EventLoop/OutputStream.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ShellExec.h>
#include <RootCoreUtils/StringUtil.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <RootCoreUtils/hadd.h>
#include <SampleHandler/DiskOutputLocal.h>
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/MetaVector.h>
#include <SampleHandler/Sample.h>
#include <TFile.h>
#include <TSystem.h>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <sstream>

//
// method implementations
//

ClassImp(EL::BatchDriver)

namespace EL
{
  namespace
  {
    /// effects: fill the job object from the original job information
    ///   except for the actual samples submitted
    /// guarantee: basic
    /// failures: out of memory II
    void fillJob (BatchJob& myjob, const Job& job,
		  const std::string& submitDir)
    {
      myjob.job = job;
      myjob.location = submitDir;
    }



    /// effects: fill the given sample object with the information
    ///   from the SampleHandler object, except for the segments that
    ///   get filled separately
    /// guarantee: basic
    /// failures: out of memory II
    void fillSample (BatchSample& mysample,
		     const SH::Sample& sample, const SH::MetaObject& meta)
    {
      mysample.name = sample.name();
      mysample.meta = *sample.meta();
      mysample.meta.fetchDefaults (meta);

      mysample.files = sample.makeFileList ();
    }



    /// effects: add the given sample with the given segments to the
    ///   job
    /// guarantee: basic
    /// failures: out of memory II
    /// failures: segment misconfiguration
    /// requires: !segments.empty()
    void addSample (BatchJob& job, BatchSample sample,
		    const std::vector<BatchSegment>& segments)
    {
      RCU_REQUIRE (!segments.empty());

      sample.begin_segments = job.segments.size();

      RCU_ASSERT (segments[0].begin_file == 0);
      RCU_ASSERT (segments[0].begin_event == 0);
      for (std::size_t iter = 0, end = segments.size(); iter != end; ++ iter)
      {
	BatchSegment segment = segments[iter];

        segment.sampleName = sample.name;
        {
          std::ostringstream myname;
          myname << sample.name << "-" << iter;
          segment.fullName = myname.str();
        }
        {
          std::ostringstream myname;
          myname << iter;
          segment.segmentName = myname.str();
        }

	segment.sample = job.samples.size();
	segment.job_id = job.segments.size();
	if (iter+1 < end)
	{
	  segment.end_file  = segments[iter+1].begin_file;
	  segment.end_event = segments[iter+1].begin_event;
	} else
	{
	  segment.end_file = sample.files.size();
	  segment.end_event = 0;
	}
	RCU_ASSERT (segment.begin_file < segment.end_file || (segment.begin_file == segment.end_file && segment.begin_event <= segment.end_event));
	job.segments.push_back (segment);
      }

      sample.end_segments = job.segments.size();
      job.samples.push_back (sample);
    }





    /// effects: generate the splits for the sample by file
    /// guarantee: basic
    /// failures: out of memory II
    /// failures: i/o errors
    /// requires: !sample.files.empty()
    /// requires: numJobs == rint (numJobs)
    /// postcondition: !segments.empty()
    void splitSampleByFile (const BatchSample& sample,
			    std::vector<BatchSegment>& segments,
			    double numJobs)
    {
      // RCU_REQUIRE (!sample.files.empty());
      RCU_REQUIRE (numJobs == rint (numJobs));
      RCU_REQUIRE (numJobs <= sample.files.size());

      for (std::size_t index = 0, end = numJobs;
	   index < end; ++ index)
      {
	BatchSegment segment;
	segment.begin_file = rint (index * (sample.files.size() / numJobs));
	segments.push_back (segment);
      }
    }





    /// effects: generate the splits for the sample
    /// guarantee: basic
    /// failures: out of memory II
    /// failures: i/o errors
    /// requires: !sample.files.empty()
    /// requires: numJobs == rint (numJobs)
    /// postcondition: !segments.empty()
    void splitSampleByEvent (const BatchSample& sample,
			     std::vector<BatchSegment>& segments,
			     const std::vector<Long64_t>& eventsFile,
			     double numJobs)
    {
      RCU_REQUIRE (!sample.files.empty());
      RCU_REQUIRE (sample.files.size() == eventsFile.size());
      RCU_REQUIRE (numJobs == rint (numJobs));

      Long64_t eventsSum = 0;
      for (std::vector<Long64_t>::const_iterator nevents = eventsFile.begin(),
	     end = eventsFile.end(); nevents != end; ++ nevents)
      {
	RCU_ASSERT_SOFT (*nevents >= 0);
	eventsSum += *nevents;
      }
      if (numJobs > eventsSum)
	numJobs = eventsSum;
      Long64_t eventsMax
	= sample.meta.castDouble (Job::optEventsPerWorker);
      if (eventsMax > 0)
	numJobs = ceil (double (eventsSum) / eventsMax);
      eventsMax = Long64_t (ceil (double (eventsSum) / numJobs));

      BatchSegment segment;
      while (std::size_t (segment.begin_file) < eventsFile.size())
      {
	while (segment.begin_event < eventsFile[segment.begin_file])
	{
	  segments.push_back (segment);
	  segment.begin_event += eventsMax;
	}
	segment.begin_event -= eventsFile[segment.begin_file];
	++ segment.begin_file;
      }
      RCU_ASSERT (segments.size() == numJobs);
    }





    /// effects: generate the splits for the sample
    /// guarantee: basic
    /// failures: out of memory II
    /// failures: i/o errors
    /// requires: !sample.files.empty()
    /// postcondition: !segments.empty()
    void splitSample (const BatchSample& sample,
		      std::vector<BatchSegment>& segments)
    {
      // RCU_REQUIRE (!sample.files.empty());

      // rationale: I'm now calculating the number of jobs here, so
      //   that independent of whether we divide on a per-file or
      //   per-event basis we can get the same number of jobs.  also,
      //   it will balance out things slightly if we only have a few
      //   files and multiple files per job.
      const double filesPerWorker
	= sample.meta.castDouble (Job::optFilesPerWorker, 1);
      if (filesPerWorker < 1)
      {
	std::ostringstream msg;
	msg << "invalid number of files per worker: " << filesPerWorker;
	RCU_THROW_MSG (msg.str());
      }
      double numJobs = ceil (sample.files.size() / filesPerWorker);

      const TObject *meta
	= sample.meta.get (SH::MetaFields::numEventsPerFile);
      if (meta)
      {
	const SH::MetaVector<Long64_t> *const meta_nentries
	  = dynamic_cast<const SH::MetaVector<Long64_t> *>(meta);
	RCU_ASSERT_SOFT (meta == meta_nentries);
	RCU_ASSERT_SOFT (meta_nentries->value.size() == sample.files.size());
	splitSampleByEvent (sample, segments, meta_nentries->value, numJobs);
      } else
      {
	splitSampleByFile (sample, segments, numJobs);
      }


      if (segments.empty())
      {
	// rationale: this isn't really the proper thing to do.  if a
	//   sample is empty I should just run the job locally.
	BatchSegment empty;
	segments.push_back (empty);
      }

      RCU_PROVIDE (!segments.empty());
    }


    /// effects: fill the job object from the original job information
    ///   including the actual samples submitted
    /// guarantee: basic
    /// failures: out of memory II
    void fillFullJob (BatchJob& myjob, const Job& job,
		      const std::string& location,
		      const SH::MetaObject& meta)
    {
      fillJob (myjob, job, location);
      *myjob.job.options() = meta;

      for (std::size_t sampleIndex = 0, end = job.sampleHandler().size();
	   sampleIndex != end; ++ sampleIndex)
      {
	BatchSample mysample;
	fillSample (mysample, *job.sampleHandler()[sampleIndex], meta);

	std::vector<BatchSegment> subsegments;
	splitSample (mysample, subsegments);
	myjob.njobs_old.push_back (subsegments.size());

	addSample (myjob, mysample, subsegments);
      }
    }
  }



  void BatchDriver ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  BatchDriver ::
  BatchDriver ()
  {
    RCU_NEW_INVARIANT (this);
  }



  ::StatusCode BatchDriver ::
  doManagerStep (Detail::ManagerData& data) const
  {
    using namespace msgEventLoop;
    ANA_CHECK (Driver::doManagerStep (data));
    switch (data.step)
    {
    case Detail::ManagerStep::fillOptions:
      {
        data.sharedFileSystem
          = data.options.castBool (Job::optBatchSharedFileSystem, true);
      }
      break;

    case Detail::ManagerStep::createSubmitDir:
      {
        if (data.sharedFileSystem) // Shared file-system, write to output
          data.batchWriteLocation = data.submitDir;
        else
          data.batchWriteLocation = ".";

        if (data.sharedFileSystem) // Shared file-system, use local path
          data.batchSubmitLocation = data.submitDir+"/submit";
        else
          data.batchSubmitLocation = ".";
      }
      break;

    case Detail::ManagerStep::updateOutputLocation:
      {
        const std::string writeLocation=data.batchWriteLocation;
        for (Job::outputMIter out = data.job->outputBegin(),
               end = data.job->outputEnd(); out != end; ++ out)
        {
          if (out->output() == nullptr)
          {
            out->output (new SH::DiskOutputLocal
                         (writeLocation + "/fetch/data-" + out->label() + "/"));
          }
        }
      }
      break;

    case Detail::ManagerStep::batchCreateDirectories:
      {
        ANA_MSG_DEBUG ("submitting batch job in location " << data.submitDir);
        const std::string submitDir = data.submitDir + "/submit";
        if (gSystem->MakeDirectory (submitDir.c_str()) != 0)
        {
          ANA_MSG_ERROR ("failed to create directory " + submitDir);
          return ::StatusCode::FAILURE;
        }
        const std::string runDir = data.submitDir + "/run";
        if (gSystem->MakeDirectory (runDir.c_str()) != 0)
        {
          ANA_MSG_ERROR ("failed to create directory " + runDir);
          return ::StatusCode::FAILURE;
        }
        const std::string fetchDir = data.submitDir + "/fetch";
        if (gSystem->MakeDirectory (fetchDir.c_str()) != 0)
        {
          ANA_MSG_ERROR ("failed to create directory " + fetchDir);
          return ::StatusCode::FAILURE;
        }
        const std::string statusDir = data.submitDir + "/status";
        if (gSystem->MakeDirectory (statusDir.c_str()) != 0)
        {
          ANA_MSG_ERROR ("failed to create directory " + statusDir);
          return ::StatusCode::FAILURE;
        }
      }
      break;

    case Detail::ManagerStep::batchCreateJob:
      {
        data.batchJob = std::make_unique<BatchJob> ();
        fillFullJob (*data.batchJob, *data.job, data.submitDir, data.options);
        data.batchJob->location=data.batchWriteLocation;
        {
          std::string path = data.submitDir + "/submit/config.root";
          std::unique_ptr<TFile> file (TFile::Open (path.c_str(), "RECREATE"));
          data.batchJob->Write ("job");
        }
        {
          std::ofstream file ((data.submitDir + "/submit/segments").c_str());
          for (std::size_t iter = 0, end = data.batchJob->segments.size();
               iter != end; ++ iter)
          {
            file << iter << " " << data.batchJob->segments[iter].fullName << "\n";
          }
        }
      }
      break;

    case Detail::ManagerStep::batchScriptVar:
      {
        data.batchName = "run";
        data.batchInit = "";
        data.batchJobId = "EL_JOBID=$1\n";
      }
      break;

    case Detail::ManagerStep::batchMakeScript:
      {
        makeScript (data, data.batchJob->segments.size());
      }
      break;

    case Detail::ManagerStep::batchMakeIndices:
      {
        for (std::size_t index = 0; index != data.batchJob->segments.size(); ++ index)
          data.batchJobIndices.push_back (index);
      }
      break;

    case Detail::ManagerStep::readConfigResubmit:
    case Detail::ManagerStep::readConfigRetrieve:
      {
        ANA_MSG_INFO ("retrieving batch job in location " << data.submitDir);

        std::unique_ptr<TFile> file
          {TFile::Open ((data.submitDir + "/submit/config.root").c_str(), "READ")};
        if (file == nullptr || file->IsZombie())
        {
          ANA_MSG_ERROR ("failed to read config.root");
          return ::StatusCode::FAILURE;
        }
        data.batchJob.reset (dynamic_cast<BatchJob*>(file->Get ("job")));
        if (data.batchJob == nullptr)
        {
          ANA_MSG_ERROR ("failed to get job object from config.root");
          return ::StatusCode::FAILURE;
        }
        data.job = &data.batchJob->job;
      }
      break;

    case Detail::ManagerStep::batchJobStatusResubmit:
    case Detail::ManagerStep::batchJobStatusRetrieve:
      {
	for (std::size_t job = 0; job != data.batchJob->segments.size(); ++ job)
	{
	  std::ostringstream completedFile;
	  completedFile << data.submitDir << "/status/completed-" << job;
          const bool hasCompleted =
            (gSystem->AccessPathName (completedFile.str().c_str()) == 0);

	  std::ostringstream failFile;
	  failFile << data.submitDir << "/status/fail-" << job;
          const bool hasFail =
            (gSystem->AccessPathName (failFile.str().c_str()) == 0);

          if (hasCompleted)
          {
            if (hasFail)
            {
              ANA_MSG_ERROR ("sub-job " << job << " reported both success and failure");
              return ::StatusCode::FAILURE;
            } else
              data.batchJobSuccess.insert (job);
          } else
          {
            if (hasFail)
              data.batchJobFailure.insert (job);
            else
              data.batchJobUnknown.insert (job);
          }
        }
        ANA_MSG_INFO ("current job status: " << data.batchJobSuccess.size() << " success, " << data.batchJobFailure.size() << " failure, " << data.batchJobUnknown.size() << " running/unknown");
      }
      break;

    case Detail::ManagerStep::batchPreResubmit:
      {
        bool all_missing = false;
        if (data.resubmitOption == "ALL_MISSING")
        {
          all_missing = true;
        } else if (!data.resubmitOption.empty())
        {
          ANA_MSG_ERROR ("unknown resubmit option " + data.resubmitOption);
          return ::StatusCode::FAILURE;
        }

        for (std::size_t segment = 0; segment != data.batchJob->segments.size(); ++ segment)
        {
          if (all_missing)
          {
            if (data.batchJobSuccess.find (segment) == data.batchJobSuccess.end())
              data.batchJobIndices.push_back (segment);
          } else
          {
            if (data.batchJobFailure.find (segment) != data.batchJobFailure.end())
              data.batchJobIndices.push_back (segment);
          }
        }

        if (data.batchJobIndices.empty())
        {
          ANA_MSG_INFO ("found no jobs to resubmit");
          data.nextStep = Detail::ManagerStep::final;
          return ::StatusCode::SUCCESS;
        }

        for (std::size_t segment : data.batchJobIndices)
        {
          std::ostringstream command;
          command << "rm -rf";
          command << " " << data.submitDir << "/status/completed-" << segment;
          command << " " << data.submitDir << "/status/fail-" << segment;
          command << " " << data.submitDir << "/status/done-" << segment;
          RCU::Shell::exec (command.str());
        }
        data.options = *data.batchJob->job.options();
      }
      break;

    case Detail::ManagerStep::doRetrieve:
      {
        bool merged = mergeHists (data);
        if (merged)
        {
          diskOutputSave (data);
        }
        data.retrieved = true;
        data.completed = merged;
      }
      break;

    default:
      (void) true; // safe to do nothing
    }
    return ::StatusCode::SUCCESS;
  }



  std::string BatchDriver ::
  defaultReleaseSetup (const Detail::ManagerData& data) const
  {
    RCU_READ_INVARIANT (this);

    // name of tarball being made (this needs to match CondorDriver.cxx)
    const std::string tarballName("AnalysisPackage.tar.gz");

    std::ostringstream file;

    // <path of build dir>/x86_64-slc6-gcc62-opt (comes from CMake, we need this)
    const char *WORKDIR_DIR         = getenv ("WorkDir_DIR");
    if (WORKDIR_DIR == nullptr)
      RCU_THROW_MSG ("could not find environment variable $WorkDir_DIR");

    if(!data.sharedFileSystem)
    {
      file << "mkdir -p build && tar -C build/ -xf " << tarballName << " || abortJob\n";
      file << "\n";
    }


    file << "\n";
    // /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/AtlasSetup/.config/.asetup.site
    if(getenv("AtlasSetupSite"))  file << "export AtlasSetupSite=" << getenv("AtlasSetupSite") << "\n";
    // /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/AtlasSetup/V00-07-75/AtlasSetup
    if(getenv("AtlasSetup"))      file << "export AtlasSetup=" << getenv("AtlasSetup") << "\n";
    // for now, needed because of errors like:
    //  /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/swConfig/asetup/asetupEpilog.sh: line 38:
    //      /swConfig/python/pythonFix-Linux.sh: No such file or directory
    file << "export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase\n";
    file << "source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh --quiet\n";

    // default setup command
    std::ostringstream defaultSetupCommand;
    {
      // AnalysisBase
      if(getenv("AtlasProject"))     defaultSetupCommand << "export AtlasProject=" << getenv("AtlasProject") << "\n";
      // 21.2.3
      if(getenv("AtlasVersion"))     defaultSetupCommand << "export AtlasVersion=" << getenv("AtlasVersion") << "\n";
      // 2017-08-16T2249 (only set if using a nightly release)
      if(getenv("AtlasBuildStamp"))  defaultSetupCommand << "export AtlasBuildStamp=" << getenv("AtlasBuildStamp") << "\n";
      // 21.2
      if(getenv("AtlasBuildBranch")) defaultSetupCommand << "export AtlasBuildBranch=" << getenv("AtlasBuildBranch") << "\n";
      // stable vs nightly
      if(getenv("AtlasReleaseType")) defaultSetupCommand << "export AtlasReleaseType=" << getenv("AtlasReleaseType") << "\n";
      defaultSetupCommand << "if [ \"${AtlasReleaseType}\" == \"stable\" ]; then\n";
      defaultSetupCommand << "     source ${AtlasSetup}/scripts/asetup.sh ${AtlasProject},${AtlasVersion} || abortJob\n";
      defaultSetupCommand << "else\n";
      defaultSetupCommand << "     source ${AtlasSetup}/scripts/asetup.sh ${AtlasProject},${AtlasBuildBranch},${AtlasBuildStamp} || abortJob\n";
      defaultSetupCommand << "fi\n";
      defaultSetupCommand << "echo \"Using default setup command\"";
    }

    file << options()->castString(Job::optBatchSetupCommand, defaultSetupCommand.str()) << " || abortJob\n";
    if(data.sharedFileSystem) file << "source " << WORKDIR_DIR << "/setup.sh || abortJob\n";
    else                 file << "source build/setup.sh || abortJob\n";
    file << "\n";

    if(!data.sharedFileSystem)
    {
      std::ostringstream cmd;
      cmd << "tar --dereference -C " << WORKDIR_DIR << " -czf " << tarballName << " .";
      // suppress the output from the tarball command
      if (gSystem->Exec (cmd.str().c_str()) != 0){
        RCU_THROW_MSG (("failed to execute: " + cmd.str()).c_str());
      }
    }

    return file.str();
  }



  void BatchDriver ::
  makeScript (Detail::ManagerData& data,
              std::size_t njobs) const
  {
    RCU_READ_INVARIANT (this);

    std::string name = data.batchName;
    bool multiFile = (name.find ("{JOBID}") != std::string::npos);
    for (std::size_t index = 0, end = multiFile ? njobs : 1; index != end; ++ index)
    {
      std::ostringstream str;
      str << index;
      const std::string fileName = data.submitDir + "/submit/" + RCU::substitute (name, "{JOBID}", str.str());

      {
        std::ofstream file (fileName.c_str());
        file << "#!/bin/bash\n";
        file << "echo starting batch job initialization\n";
        file << RCU::substitute (data.batchInit, "{JOBID}", str.str()) << "\n";
        file << "echo batch job user initialization finished\n";
        if (multiFile) file << "EL_JOBID=" << index << "\n\n";
        else           file << data.batchJobId << "\n";

        file << "function abortJob {\n";
        file << "  echo \"abort EL_JOBID=${EL_JOBID}\"\n";
        file << "  touch \"" << data.batchWriteLocation << "/status/fail-$EL_JOBID\"\n";
        file << "  touch \"" << data.batchWriteLocation << "/status/done-$EL_JOBID\"\n";
        file << "  exit 1\n";
        file << "}\n\n";

        file << "test \"$TMPDIR\" == \"\" && TMPDIR=/tmp\n";

        file << "EL_JOBSEG=`grep \"^$EL_JOBID \" \"" << data.batchSubmitLocation << "/segments\" | awk ' { print $2 }'`\n";
        file << "test \"$EL_JOBSEG\" != \"\" || abortJob\n";
        file << "hostname\n";
        file << "pwd\n";
        file << "whoami\n";
        file << shellInit << "\n";

        if(!data.sharedFileSystem)
        { // Create output transfer directories
          file << "mkdir \"${TMPDIR}/fetch\" || abortJob\n";
          file << "mkdir \"${TMPDIR}/status\" || abortJob\n";
          file << "\n";
        }

        if(data.sharedFileSystem)
        {
          file << "RUNDIR=${TMPDIR}/EventLoop-Worker-$EL_JOBSEG-`date +%s`-$$\n";
          file << "mkdir \"$RUNDIR\" || abortJob\n";
        } else
        {
          file << "RUNDIR=${TMPDIR}\n";
        }
        file << "cd \"$RUNDIR\" || abortJob\n";

        if (!data.batchSkipReleaseSetup)
          file << defaultReleaseSetup (data);

        file << "eventloop_batch_worker $EL_JOBID '" << data.batchSubmitLocation << "/config.root' || abortJob\n";

        file << "test -f \"" << data.batchWriteLocation << "/status/completed-$EL_JOBID\" || "
             << "touch \"" << data.batchWriteLocation << "/status/fail-$EL_JOBID\"\n";
        file << "touch \"" << data.batchWriteLocation << "/status/done-$EL_JOBID\"\n";
        if(data.sharedFileSystem) file << "cd .. && rm -rf \"$RUNDIR\"\n";
      }

      {
        std::ostringstream cmd;
        cmd << "chmod +x " << fileName;
        if (gSystem->Exec (cmd.str().c_str()) != 0)
          RCU_THROW_MSG (("failed to execute: " + cmd.str()).c_str());
      }
    }
  }



  bool BatchDriver ::
  mergeHists (Detail::ManagerData& data)
  {
    using namespace msgEventLoop;

    // This picks up the DiskOutput object used to write out our
    // histograms, we will then use that to locate the output files.
    // this is not really the best way of doing this, but there are
    // bigger rewrites of this code excepted, so I don't want to spend
    // a lot of time on this now (06 Feb 19).
    std::unique_ptr<SH::DiskOutput> origHistOutputMemory;
    const SH::DiskOutput *origHistOutput = nullptr;
    for (auto iter = data.job->outputBegin(), end = data.job->outputEnd();
         iter != end; ++ iter)
    {
      if (iter->label() == Job::histogramStreamName)
        origHistOutput = iter->output();
    }
    if (origHistOutput == nullptr)
    {
      origHistOutputMemory = std::make_unique<SH::DiskOutputLocal>
        (data.submitDir + "/fetch/hist-");
      origHistOutput = origHistOutputMemory.get();
    }
    RCU_ASSERT (origHistOutput != nullptr);

    bool result = true;

    ANA_MSG_DEBUG ("merging histograms in location " << data.submitDir);

    RCU_ASSERT (data.batchJob->njobs_old.size() == data.batchJob->samples.size());
    for (std::size_t sample = 0, end = data.batchJob->samples.size();
	 sample != end; ++ sample)
    {
      const BatchSample& mysample (data.batchJob->samples[sample]);

      std::ostringstream output;
      output << data.submitDir << "/hist-" << data.batchJob->samples[sample].name << ".root";
      if (gSystem->AccessPathName (output.str().c_str()) != 0)
      {
        ANA_MSG_VERBOSE ("merge files for sample " << data.batchJob->samples[sample].name);

	bool complete = true;
	std::vector<std::string> input;
	for (std::size_t segment = mysample.begin_segments,
	       end = mysample.end_segments; segment != end; ++ segment)
	{
	  const BatchSegment& mysegment = data.batchJob->segments[segment];

	  const std::string hist_file = origHistOutput->targetURL
            (mysegment.sampleName, mysegment.segmentName, ".root");

          ANA_MSG_VERBOSE ("merge segment " << segment << " completed=" << (data.batchJobSuccess.find(segment)!=data.batchJobSuccess.end()) << " fail=" << (data.batchJobFailure.find(segment)!=data.batchJobFailure.end()) << " unknown=" << (data.batchJobUnknown.find(segment)!=data.batchJobUnknown.end()));

	  input.push_back (hist_file);

	  if (data.batchJobFailure.find(segment)!=data.batchJobFailure.end())
	  {
	    std::ostringstream message;
	    message << "subjob " << segment << "/" << mysegment.fullName
		    << " failed";
	    RCU_THROW_MSG (message.str());
	  }
	  else if (data.batchJobSuccess.find(segment)==data.batchJobSuccess.end())
	    complete = false, result = false;
	}
	if (complete)
	{
	  RCU::hadd (output.str(), input);

	  // Merge output data directories
	  for (Job::outputIter out = data.batchJob->job.outputBegin(),
		 end = data.batchJob->job.outputEnd(); out != end; ++ out)
	    {
	      output.str("");
	      output << data.submitDir << "/data-" << out->label();

	      if(gSystem->AccessPathName(output.str().c_str()))
		gSystem->mkdir(output.str().c_str(),true);


	      output << "/" << data.batchJob->samples[sample].name << ".root";

	      std::vector<std::string> input;
	      for (std::size_t segment = mysample.begin_segments,
		     end = mysample.end_segments; segment != end; ++ segment)
		{
		  const BatchSegment& mysegment = data.batchJob->segments[segment];

		  const std::string infile =
		    data.submitDir + "/fetch/data-" + out->label() + "/" + mysegment.fullName + ".root";

		  input.push_back (infile);
		}

	      RCU::hadd(output.str(), input);
	    }
	}
      }
    }
    return result;
  }
}
