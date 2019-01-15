/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_JOB_HH
#define EVENT_LOOP_JOB_HH

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines a class that manages a complete job
/// description.  The interface provided in this module is intended
/// for the general user.  The module is considered to be in the
/// pre-alpha stage.



#include <EventLoop/Global.h>

#include <vector>
#include <AnaAlgorithm/Global.h>
#include <EventLoop/JobConfig.h>
#include <SampleHandler/SampleHandler.h>
#include <SampleHandler/MetaObject.h>

namespace EL
{
  /// effects: standard swap
  /// guarantee: no-fail
  void swap (Job& a, Job& b);


  class Job
  {
    //
    // public interface
    //

    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;


    /// effects: standard default constructor
    /// guarantee: strong
    /// failures: low level errors I
  public:
    Job ();


    /// effects: standard copy constructor
    /// guarantee: strong
    /// failures: out of memory II
  public:
    Job (const Job& that);


    /// effects: standard destructor
    /// guarantee: no-fail
  public:
    ~Job ();


    /// effects: standard assignment operator
    /// returns: *this
    /// guarantee: strong
    /// failures: out of memory II
  public:
    Job& operator = (const Job& that);


    /// description: the sample handler used
    /// guarantee: no-fail / strong
    /// failures: out of memory II
  public:
    const SH::SampleHandler& sampleHandler () const;
    void sampleHandler (const SH::SampleHandler& val_sampleHandler);


    /// description: the list of algorithms used
    /// guarantee: no-fail / strong
    /// failures: out of memory II
    /// invariant: alg != 0
  public:
    void algsAdd (std::unique_ptr<Algorithm> val_algorithm);
    void algsAdd (Algorithm *alg_swallow);
    void algsAdd (const AnaAlgorithmConfig& config);


    /// \brief add a clone of the given algorithm
    ///
    /// This is to be used when the user wants to reuse an algorithm
    /// for multiple Job objects or for some reason needs to delete it
    /// himself.
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   algorithm cloning failures\n
    ///   out of memory I
  public:
    void algsAddClone (const Algorithm& alg);


    /// returns: whether we have the algorithm with the given name
    /// guarantee: no-fail
  public:
    bool algsHas (const std::string& name) const;


    /// description: the list of output datasets used
    /// guarantee: no-fail / strong
    /// failures(3): out of memory II
    /// requires(3,soft): !outputHas (val_output.label())
    /// note: while it is not a requirement it is highly recommended
    ///   that you add the outputs from Algorithm::doSetupJob.  That
    ///   way they get automatically enabled/disabled when you
    ///   enable/disable the algorithms.  Plus your code will be
    ///   slightly less spread out.
  public:
    typedef OutputStream* outputMIter;
    typedef const OutputStream* outputIter;
    outputMIter outputBegin ();
    outputIter outputBegin () const;
    outputMIter outputEnd ();
    outputIter outputEnd () const;
    void outputAdd (const OutputStream& val_output);


    /// returns: whether we have an output with the given name
    /// guarantee: no-fail
  public:
    bool outputHas (const std::string& name) const;


    /// effects: register this job to use XAODs
    /// guarantee: strong
    /// failures: out of memory II
    /// failures: TEventSvc not available
  public:
    void useXAOD ();


    /// description: the list of options to the job
    /// guarantee: no-fail
    /// postcondition: result != 0
  public:
    SH::MetaObject *options ();
    const SH::MetaObject *options () const;


    /// \brief the \ref JobConfig object we are wrapping
    /// \par Guarantee
    ///   no-fail
  public:
    const JobConfig& jobConfig () const noexcept;


    /// description: the name of the option for overwriting the
    ///   submission directory.  if you set this to a non-zero value
    ///   it will remove any existing submit-directory before trying
    ///   to create a new one.
    /// rationale: normally you don't want to silently remove an
    ///   existing submission directory, since it may contain valuable
    ///   data, but for some cases like debugging you may be annoyed
    ///   to delete it manually.
  public:
    static const std::string optRemoveSubmitDir;


    /// description: the name of the option used for setting the
    ///   maximum number of events to process per sample
    /// rationale: this is used for test runs where you don't want to
    ///   process all events, but just want a quick look
  public:
    static const std::string optMaxEvents;


    /// description: the name of the option used for skipping a
    ///   certain number of events in the beginning
    /// rationale: this is used for test runs where you are only
    ///   interested in a particular set of events
  public:
    static const std::string optSkipEvents;


    /// description: the name of the option for selecting the number
    ///   of files per batch job.  (only BatchDriver and derived
    ///   drivers).

    /// rationale: this is typically used if you are either processing
    ///   fairly small input files or are producing a skim with a very
    ///   high reduction factor.  it will reduce the inefficiency
    ///   associated with starting a lot of jobs or reading a lot of
    ///   output files
  public:
    static const std::string optFilesPerWorker;


    /// description: the name of the option for selecting the number
    ///   of events per batch job.  (only BatchDriver and derived
    ///   drivers).
    /// warning: this option will be ignored unless you have called
    ///   SH::scanNEvents first.
    /// rationale: this allows to make your jobs all approximately
    ///   equal size and gives you much finer control of the lengths
    ///   of your jobs.  if you did run SH::scanNEvents, but didn't
    ///   specify a number of events per worker this will produce the
    ///   same number of jobs as you would have otherwise, but spread
    ///   the workload more evenly.
  public:
    static const std::string optEventsPerWorker;


    /// description: the name of the option for supplying extra submit
    ///   parameters to batch systems
    /// rationale: the primary purpose of this is to allow selecting
    ///   the right queue on your batch system, but it can be used for
    ///   all kind of submission options that EventLoop doesn't
    ///   support directly.
  public:
    static const std::string optSubmitFlags;


    /// description: the name of the option for supplying extra
    ///   parameters for condor systems
  public:
    static const std::string optCondorConf;


    /// description: this option allows to configure the TTreeCache
    ///   size for this job.  if it is smaller or equal to 0, the
    ///   cache is turned off.
    /// rationale: if you read data from across the network using the
    ///   cache mechanism will greatly increase your performance.  for
    ///   local disks you may gain or lose performance.
  public:
    static const std::string optCacheSize;


    /// description: this option allows to configure the number of
    ///   tree entries used for learning cache behavior.
    /// rationale: there is a trade-off here, if you set this too low
    ///   you will fail to cache more rarely used variables.  if you
    ///   set it too high it will take too long until the cache kicks
    ///   in.
    /// warning: right now this is performed on a per-file basis,
    ///   which at some point will be raised to a per-job basis, which
    ///   is the limit of how far caching can go in this approach.  if
    ///   you need multi-job support, either use D3PDReader or contact
    ///   me on how to do this.
  public:
    static const std::string optCacheLearnEntries;


    /// description: the name of the option for turning on
    ///   D3PDPerfStats.  To use D3PDPerfStats set this to a non-zero
    ///   value.
    /// warning: this only works if you read data through D3PDReader
    /// rationale: this can be used for fine-tuning TTreeCache or
    ///   generally optimizing i/o performance
  public:
    static const std::string optD3PDPerfStats;


    /// description: the name of the D3PDPerfStats object produced as
    ///   I gather it, as well as the name of the option passed into
    ///   the job.
    /// rationale: I use the same name in two places to allow reading
    ///   the stats object from one output and passing it to the next
  public:
    static const std::string optD3PDReadStats;


    /// description: the name of the option for turning on
    ///   XAODPerfStats.  To use XAODPerfStats set this to a non-zero
    ///   value.
    /// warning: this only works if you read data through XAODReader
    /// rationale: this can be used for fine-tuning TTreeCache or
    ///   generally optimizing i/o performance
  public:
    static const std::string optXAODPerfStats;


    /// description: the name of the XAODPerfStats object produced as
    ///   I gather it, as well as the name of the option passed into
    ///   the job.
    /// rationale: I use the same name in two places to allow reading
    ///   the stats object from one output and passing it to the next
  public:
    static const std::string optXAODReadStats;


    /// description: these options configure the D3PDReader TTreeCache
    ///   settings.  if you use more than one option, the result is
    ///   undefined.
    /// warning: this only works if you use D3PDReader
    /// rationale: the idea is that in your first job you create the
    ///   D3PDReadStats object, which you then pass to subsequent
    ///   jobs.
  public:
    static const std::string optD3PDCacheMinEvent;
    static const std::string optD3PDCacheMinEventFraction;
    static const std::string optD3PDCacheMinByte;
    static const std::string optD3PDCacheMinByteFraction;


    /// description: the option to turn on the performance tree in
    ///   PROOF.  if this is set to 1, it will write out the tree
    /// rationale: this can help in tuning your PROOF cluster or
    ///   tuning what you do in PROOF, but it may have some overhead,
    ///   which is why we don't do it by default
  public:
    static const std::string optPerfTree;


    /// description: the option to select the access mode for xAODs.
    ///   this can be "branch" for branch access, or "class" for
    ///   access.  if this option isn't specified EventLoop will pick
    ///   whatever is currently recommended.
  public:
    static const std::string optXaodAccessMode;
    static const std::string optXaodAccessMode_branch;
    static const std::string optXaodAccessMode_class;
    static const std::string optXaodAccessMode_athena;


    /// \brief the option to turn on/off the xAOD summary reporting at
    /// the end of the job
  public:
    static const std::string optXAODSummaryReport;


    /// description: the option to turn on printing of i/o statistics
    ///   at the end of each file
    /// rationale: while this is not as complete as whole sample
    ///   statistics, it can be helpful in some circumstances when
    ///   debugging the performance
    /// warning: this is not supported for all drivers
  public:
    static const std::string optPrintPerFileStats;


    /// description: the option to turn off collection of performance data
  public:
    static const std::string optDisableMetrics;


    /// description: the option to reset the shell on the worker nodes
    /// rationale: this is currently only used by the LSFDriver where
    ///   it is enabled by default to reset it on lxbatch.
  public:
    static const std::string optResetShell;

    /// \brief the option not to unsetup the environment in \ref
    /// LocalDriver
  public:
    static const std::string optLocalNoUnsetup;


    /// \brief the option to do processing in a background process in PROOF
  public:
    static const std::string optBackgroundProcess;


    /// description: grid-specific options
    /// rationale: these are named so as to correspond to prun equivalents,
    ///   bare the optGrid prefix.
  public:
    static const std::string optGridDestSE;
    static const std::string optGridSite;
    static const std::string optGridCloud;
    static const std::string optGridExcludedSite;
    static const std::string optGridNGBPerJob;
    static const std::string optGridMemory;
    static const std::string optGridMaxCpuCount;
    static const std::string optGridNFiles;
    static const std::string optGridNFilesPerJob;
    static const std::string optGridNJobs;
    static const std::string optGridMaxFileSize;
    static const std::string optGridMaxNFilesPerJob;
    static const std::string optGridUseChirpServer;
    static const std::string optGridExpress;
    static const std::string optGridNoSubmit;
    static const std::string optGridMergeOutput;
    static const std::string optTmpDir;
    static const std::string optRootVer;
    static const std::string optCmtConfig;
    static const std::string optGridDisableAutoRetry;
    static const std::string optOfficial;
    static const std::string optVoms;

    /// these options are defined in \ref SH::MetaNames
    /// \{

    static const std::string optRetries;
    static const std::string optRetriesWait;

    /// \}



    /// description: batch-specific options
    /// rationale: these options are for configuring batch drivers
  public:
    static const std::string optBatchSharedFileSystem;
    /// The content of this string will be executed in the job script on the worker node
    /// before the main executable is run.
    static const std::string optBatchSlurmExtraConfigLines;
    /// Append a command before the main executable is called
    /// This is useful is you want to execute the command e.g. within shifter.
    static const std::string optBatchSlurmWrapperExec;
    /// This overrides the asetup command if you need to use a custom one
    static const std::string optBatchSetupCommand;


  public:
    /// @name Options controlling the memory monitoring behaviour of the job
    /// @{

    /// The minimal per-event resident memory increase for triggering an error
    ///
    /// This is the main setting for triggering a failure in memory-leaking
    /// analysis jobs. It sets the limit on the per-event resident memory
    /// increase of the job for it to be still successful. It is an integer
    /// property, setting the limit in kilobytes.
    ///
    static const std::string optMemResidentPerEventIncreaseLimit;

    /// The minimal per-event virtual memory increase for triggering an error
    ///
    /// Implemented very similarly to @c optMemResidentPerEventIncreaseLimit.
    /// But since normally we don't care about the virtual memory usage of the
    /// jobs that much, it is set to zero by default. Making
    /// @c optMemResidentPerEventIncreaseLimit control the behaviour of the
    /// job.
    ///
    static const std::string optMemVirtualPerEventIncreaseLimit;

    /// The minimal resident memory increase necessary to trigger an error
    ///
    /// It is an integer property, setting the limit in kilobytes. Jobs have
    /// to increase their resident memory usage by this amount to trigger a
    /// failure.
    ///
    static const std::string optMemResidentIncreaseLimit;

    /// The minimal virtual memory increase necessary to trigger an error
    ///
    /// Implemented very similarly to @c optMemResidentIncreaseLimit. Since
    /// normally the virtual memory usage is not considered in producing a
    /// failure, it is set to zero by default.
    ///
    static const std::string optMemVirtualIncreaseLimit;

    /// Failure behaviour of the code when a "significant memory leak" is found
    ///
    /// This flag allows the user to select what should happen when the code
    /// finds a memory leak in the job that is larger than the values set by
    /// @c optMemResidentPerEventIncreaseLimit,
    /// @c optMemVirtualPerEventIncreaseLimit, @c optMemResidentIncreaseLimit
    /// and @c optMemVirtualIncreaseLimit.
    ///
    /// It's a boolean property. When set to @c true, the job fails if a
    /// significant memory leak is detected. If set to @c false, only a warning
    /// is printed.
    ///
    static const std::string optMemFailOnLeak;

    /// @}


    //
    // private interface
    //

    friend void swap (Job& a, Job& b);

    /// description: members directly corresponding to accessors
  private:
    SH::SampleHandler m_sampleHandler;
  private:
    std::vector<EL::OutputStream> m_output;
  private:
    SH::MetaObject m_options;

    /// \brief the \ref JobConfig object we use
  private:
    EL::JobConfig m_jobConfig;
  };
}

#endif
