/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include <EventLoop/BatchDriver.h>

#include <EventLoop/BatchJob.h>
#include <EventLoop/BatchSample.h>
#include <EventLoop/BatchSegment.h>
#include <EventLoop/OutputStream.h>
#include <RootCoreUtils/Assert.h>
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

	std::ostringstream myname;
	myname << sample.name << "-" << iter;
	segment.name = myname.str();

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



  void BatchDriver ::
  doUpdateJob (Job& job, const std::string& location) const
  {
    RCU_READ_INVARIANT (this);

    const std::string writeLocation=getWriteLocation(location);

    for (Job::outputMIter out = job.outputBegin(),
	   end = job.outputEnd(); out != end; ++ out)
    {
      if (out->output() == 0)
      {
	out->output (new SH::DiskOutputLocal
		     (writeLocation + "/fetch/data-" + out->label() + "/"));
      }
    }
  }



  void BatchDriver ::
  doSubmit (const Job& job, const std::string& location) const
  {
    RCU_READ_INVARIANT (this);

    const std::string submitDir = location + "/submit";
    if (gSystem->MakeDirectory (submitDir.c_str()) != 0)
      RCU_THROW_MSG ("failed to create directory " + submitDir);
    const std::string runDir = location + "/run";
    if (gSystem->MakeDirectory (runDir.c_str()) != 0)
      RCU_THROW_MSG ("failed to create directory " + runDir);
    const std::string fetchDir = location + "/fetch";
    if (gSystem->MakeDirectory (fetchDir.c_str()) != 0)
      RCU_THROW_MSG ("failed to create directory " + fetchDir);

    SH::MetaObject meta = *job.options();
    meta.fetchDefaults (*options());

    BatchJob myjob;
    fillFullJob (myjob, job, location, meta);
    myjob.location=getWriteLocation(location);
    {
      std::auto_ptr<TFile> file
	(TFile::Open ((location + "/submit/config.root").c_str(), "RECREATE"));
      myjob.Write ("job");
    }
    {
      std::ofstream file ((location + "/submit/segments").c_str());
      for (std::size_t iter = 0, end = myjob.segments.size();
	   iter != end; ++ iter)
      {
	file << iter << " " << myjob.segments[iter].name << "\n";
      }
    }

    makeScript (location, myjob.segments.size(),
		meta.castBool(Job::optBatchSharedFileSystem,true));

    batchSubmit (location, meta, myjob.segments.size());
  }



  bool BatchDriver ::
  doRetrieve (const std::string& location) const
  {
    RCU_READ_INVARIANT (this);

    std::auto_ptr<TFile> file
      (TFile::Open ((location + "/submit/config.root").c_str(), "READ"));
    RCU_ASSERT_SOFT (file.get() != 0);
    std::auto_ptr<BatchJob> config (dynamic_cast<BatchJob*>(file->Get ("job")));
    RCU_ASSERT_SOFT (config.get() != 0);

    bool merged = mergeHists (location, *config);
    if (merged)
    {
      diskOutputSave (location, config->job);
    }
    return merged;
  }



  std::string BatchDriver ::
  batchName () const
  {
    RCU_READ_INVARIANT (this);
    return "run";
  }



  std::string BatchDriver ::
  batchInit () const
  {
    RCU_READ_INVARIANT (this);
    return "";
  }



  std::string BatchDriver ::
  batchJobId () const
  {
    RCU_READ_INVARIANT (this);
    return "EL_JOBID=$1\n";
  }



  void BatchDriver ::
  makeScript (const std::string& location, std::size_t njobs, bool sharedFileSystem) const
  {
    RCU_READ_INVARIANT (this);

    // <path of build dir>/x86_64-slc6-gcc62-opt (comes from CMake, we need this)
    const char *WORKDIR_DIR         = getenv ("WorkDir_DIR");

    // name of tarball being made (this needs to match CondorDriver.cxx)
    const std::string tarballName("AnalysisPackage.tar.gz");

    const std::string writeLocation=getWriteLocation(location);
    const std::string submitLocation=getSubmitLocation(location);

    std::string name = batchName ();
    bool multiFile = (name.find ("{JOBID}") != std::string::npos);
    for (std::size_t index = 0, end = multiFile ? njobs : 1; index != end; ++ index)
    {
      std::ostringstream str;
      str << index;
      const std::string fileName = location + "/submit/" + RCU::substitute (name, "{JOBID}", str.str());

      {
        std::ofstream file (fileName.c_str());
        file << "#!/bin/bash\n";
        file << "echo starting batch job initialization\n";
        file << RCU::substitute (batchInit(), "{JOBID}", str.str()) << "\n";
        file << "echo batch job user initialization finished\n";
        if (multiFile) file << "EL_JOBID=" << index << "\n\n";
        else           file << batchJobId() << "\n";

        file << "function abortJob {\n";
        file << "  touch \"" << writeLocation << "/fetch/fail-$EL_JOBID\"\n";
        file << "  touch \"" << writeLocation << "/fetch/done-$EL_JOBID\"\n";
        file << "  exit 1\n";
        file << "}\n\n";

        file << "test \"$TMPDIR\" == \"\" && TMPDIR=/tmp\n";

        file << "EL_JOBSEG=`grep \"^$EL_JOBID \" \"" << submitLocation << "/segments\" | awk ' { print $2 }'`\n";
        file << "test \"$EL_JOBSEG\" != \"\" || abortJob\n";
        file << "hostname\n";
        file << "pwd\n";
        file << "whoami\n";
        file << shellInit << "\n";

        if(!sharedFileSystem)
          { // Create output transfer directories
            file << "mkdir \"${TMPDIR}/fetch\" || abortJob\n";
            file << "\n";
          }

        if(sharedFileSystem)
          {
            file << "RUNDIR=${TMPDIR}/EventLoop-Worker-$EL_JOBSEG-`date +%s`-$$\n";
            file << "mkdir \"$RUNDIR\" || abortJob\n";
          }
        else
          {
            file << "RUNDIR=${TMPDIR}\n";
          }
        file << "cd \"$RUNDIR\" || abortJob\n";

        if(!sharedFileSystem)
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
          defaultSetupCommand << "if [ \"${AtlasReleaseType}\" == \"stable\" ]; then\n";
          defaultSetupCommand << "     source ${AtlasSetup}/scripts/asetup.sh ${AtlasProject},${AtlasVersion} || abortJob\n";
          defaultSetupCommand << "else\n";
          defaultSetupCommand << "     source ${AtlasSetup}/scripts/asetup.sh ${AtlasProject},${AtlasBuildBranch},${AtlasBuildStamp} || abortJob\n";
          defaultSetupCommand << "fi\n";
          defaultSetupCommand << "echo \"Using default setup command\"";
        }
        file << options()->castString(Job::optBatchSetupCommand, defaultSetupCommand.str()) << " || abortJob\n";
        if(sharedFileSystem) file << "source " << WORKDIR_DIR << "/setup.sh || abortJob\n";
        else                 file << "source build/setup.sh || abortJob\n";
        file << "\n";

        file << "eventloop_batch_worker $EL_JOBID '" << submitLocation << "/config.root' || abortJob\n";

        file << "test -f \"" << writeLocation << "/fetch/completed-$EL_JOBID\" || "
             << "touch \"" << writeLocation << "/fetch/fail-$EL_JOBID\"\n";
        file << "touch \"" << writeLocation << "/fetch/done-$EL_JOBID\"\n";
        if(sharedFileSystem) file << "cd .. && rm -rf \"$RUNDIR\"\n";
      }

      {
        std::ostringstream cmd;
        cmd << "chmod +x " << fileName;
        if (gSystem->Exec (cmd.str().c_str()) != 0)
          RCU_THROW_MSG (("failed to execute: " + cmd.str()).c_str());
      }
    }

    {
      std::ostringstream cmd;
      cmd << "tar --dereference -C " << WORKDIR_DIR << " -czvf " << tarballName << " .";
      // suppress the output from the tarball command
      gSystem->RedirectOutput("/dev/null");
      if (gSystem->Exec (cmd.str().c_str()) != 0){
        gSystem->RedirectOutput(0);
        RCU_THROW_MSG (("failed to execute: " + cmd.str()).c_str());
      }
      gSystem->RedirectOutput(0);
    }
  }



  bool BatchDriver ::
  mergeHists (const std::string& location,
	      const BatchJob& config)
  {
    bool result = true;

    RCU_ASSERT (config.njobs_old.size() == config.samples.size());
    for (std::size_t sample = 0, end = config.samples.size();
	 sample != end; ++ sample)
    {
      const BatchSample& mysample (config.samples[sample]);

      std::ostringstream output;
      output << location << "/hist-" << config.samples[sample].name << ".root";
      if (gSystem->AccessPathName (output.str().c_str()) != 0)
      {
	bool complete = true;
	std::vector<std::string> input;
	for (std::size_t segment = mysample.begin_segments,
	       end = mysample.end_segments; segment != end; ++ segment)
	{
	  const BatchSegment& mysegment = config.segments[segment];

	  const std::string hist_file =
	    location + "/fetch/hist-" + mysegment.name + ".root";
	  std::ostringstream completed_file;
	  completed_file << location << "/fetch/completed-" << segment;
	  std::ostringstream fail_file;
	  fail_file << location << "/fetch/fail-" << segment;
	  std::ostringstream done_file;
	  done_file << location << "/fetch/done-" << segment;

	  input.push_back (hist_file);

	  if (gSystem->AccessPathName (fail_file     .str().c_str()) == 0)
	  {
	    std::ostringstream message;
	    message << "subjob " << segment << "/" << mysegment.name
		    << " failed";
	    message << std::endl << "found " << fail_file.str();
	    RCU_THROW_MSG (message.str());
	  }
	  else if (gSystem->AccessPathName (completed_file.str().c_str()) != 0)
	    complete = false, result = false;
	}
	if (complete)
	{
	  RCU::hadd (output.str(), input);

	  // Merge output data directories
	  for (Job::outputIter out = config.job.outputBegin(),
		 end = config.job.outputEnd(); out != end; ++ out)
	    {
	      output.str("");
	      output << location << "/data-" << out->label();

	      if(gSystem->AccessPathName(output.str().c_str()))
		gSystem->mkdir(output.str().c_str(),true);


	      output << "/" << config.samples[sample].name << ".root";

	      std::vector<std::string> input;
	      for (std::size_t segment = mysample.begin_segments,
		     end = mysample.end_segments; segment != end; ++ segment)
		{
		  const BatchSegment& mysegment = config.segments[segment];

		  const std::string infile =
		    location + "/fetch/data-" + out->label() + "/" + mysegment.name + ".root";

		  input.push_back (infile);
		}

	      RCU::hadd(output.str(), input);
	    }
	}
      }
    }
    return result;
  }

  const std::string BatchDriver ::
  getWriteLocation (const std::string& location) const
  {
    RCU_READ_INVARIANT (this);
    if(options()->castBool(Job::optBatchSharedFileSystem,true)) // Shared file-system, write to output
      return location;
    else
      return ".";
  }

  const std::string BatchDriver ::
  getSubmitLocation (const std::string& location) const
  {
    RCU_READ_INVARIANT (this);
    if(options()->castBool(Job::optBatchSharedFileSystem,true)) // Shared file-system, use local path
      return location+"/submit";
    else
      return ".";
  }

}
