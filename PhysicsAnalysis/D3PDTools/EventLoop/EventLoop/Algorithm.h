/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_ALGORITHM_HH
#define EVENT_LOOP_ALGORITHM_HH


#include <EventLoop/Global.h>

#include <TNamed.h>
#include <AsgMessaging/INamedInterface.h>
#include <AsgTools/SgTEvent.h>
#include <EventLoop/StatusCode.h>

class TH1;
class MsgStream;

namespace EL
{
  class Algorithm : public TNamed, public INamedInterface
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
    Algorithm ();


    /// effects: standard destructor
    /// guarantee: no-fail
  public:
    ~Algorithm ();


    /// description: the worker that is controlling us
    /// guarantee: no-fail
  public:
    IWorker *wk () const;


    /// \brief book the given histogram
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   histogram booking error
  public:
    void book (const TH1& hist);


    /// \brief get the histogram with the given name
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   histogram not found
  public:
    TH1 *hist (const std::string& name) const;


    /// \brief get the (main) event store for this algorithm
    ///
    /// This is mostly to mirror the method of the same name in
    /// AthAlgorithm, allowing to make the tutorial instructions more
    /// dual-use.
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I\n
    ///   job not configured for xAODs
  public:
    asg::SgTEvent *evtStore() const;



    ///
    /// \brief messaging interface
    ///
    /// this is the interface to work with the standard messaging
    /// macros from AsgTools.  Instead of enums I pass ints, so that I
    /// can avoid the include dependency (forward declarations are only
    /// allowed for enum classes AFAIK).
    /// \{
    ///

    /// \brief the standard message stream for this object
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   code not compiled with AsgTools support
  public:
    MsgStream& msg () const;


    /// \brief the message stream for this object, configured for the
    /// given level
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   code not compiled with AsgTools support
  public:
    MsgStream& msg (int level) const;


    /// \brief whether we are configured to print messages at the given level
    /// \par Guarantee
    ///   no-fail
  public:
    bool msgLvl (int lvl) const;


    /// \brief set the message level for the message stream for this
    /// object
    /// \par Guarantee
    ///   no-fail
  public:
    void setMsgLevel (int level);

    /// \}



    //
    // virtual interface
    //

    /// effects: give the algorithm a chance to intialize the job with
    ///   anything this algorithm needs.  this method is automatically
    ///   called before the algorithm is actually added to the job.
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: this is currently used to give algorithms a chance
    ///   to register their output datasets, but can also be used for
    ///   other stuff.
  private:
    virtual StatusCode setupJob (Job& job);


    /// effects: do all the processing that needs to be done once per
    ///   file
    ///
    /// Warning: The user should not expect this to be called at any
    /// particular point in execution.  If a file is split between
    /// multiple jobs this will be called in only one of these jobs,
    /// and not the others.  It usually gets called before the first
    /// event in a file, but that is **not** guaranteed and relying on
    /// this is a bug.  Take a look at \ref changeInput if you want
    /// something that is guaranteed to be executed at the beginning
    /// of each input file.
    ///
    /// Warning: The execution order of changeInput and fileExecute is
    /// currently unspecified.
    ///
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: this is to read per-file accounting data, e.g. the
    ///   list of lumi-blocks processed
  private:
    virtual StatusCode fileExecute ();


    /// effects: do the post-processing for each input file
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: this is mainly used for specialized services that
    ///   need to save partial results for each input file
  private:
    virtual StatusCode endOfFile ();


    /// effects: do all changes to work with a new input file,
    ///   e.g. set new branch addresses.  if firstFile is set, this
    ///   method is called just before init() is called
    ///
    /// Warning: If a file is split across multiple jobs this will be
    /// called more than once.  This only happens for specific batch
    /// drivers and/or if it is explicitly configured by the user.
    /// With PROOF it could even happen multiple times within the same
    /// job, and while PROOF is no longer supported that behavior may
    /// come back if support for a similar framework is added in the
    /// future.  As such, this method should not be used for
    /// accounting that relies to be called exactly once per file,
    /// take a look at fileExecute() if you want something that is
    /// guaranteed to be executed exactly once per input file.
    ///
    /// Warning: The execution order of changeInput and fileExecute is
    /// currently unspecified.
    ///
    /// guarantee: basic
    /// failures: algorithm dependent
  private:
    virtual StatusCode changeInput (bool firstFile);


    /// effects: do everything that needs to be done before running
    ///   the algorithm, e.g. create output n-tuples and histograms.
    ///   this method is called only once right after
    ///   changeInput(true) is called
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: in principle all this work could be done on
    ///   changeInput(true).  However, providing this method should
    ///   make it easier for the user to set up all his outputs and to
    ///   do so only once.
  private:
    virtual StatusCode initialize ();


    /// effects: this is a pre-initialization routine that is called
    ///   before changeInput is called.
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: unlike initialize(), this method is called all the
    ///   time, even on empty input files.  so you should create any
    ///   histograms or n-tuples here that subsequent code expects
  private:
    virtual StatusCode histInitialize ();


    /// effects: process the next event
    /// guarantee: basic
    /// failures: algorithm dependent
  private:
    virtual StatusCode execute ();


    /// effects: do the post-processing for the event
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: this is mainly used for specialized services that
    ///   need to get input from subsequent algorithms before filling
    ///   their event data
  private:
    virtual StatusCode postExecute ();


    /// effects: do everything that needs to be done after completing
    ///   work on this worker
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: currently there is no use foreseen, but this
    ///   routine is provided regardless
  private:
    virtual StatusCode finalize ();


    /// effects: this is a post-initialization routine that is called
    ///   after finalize has been called.
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: unlike finalize(), this method is called all the
    ///   time, even on empty input files.
  private:
    virtual StatusCode histFinalize ();


    /// returns: whether this algorithm has the given name
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: this is to allow an algorithm to be known by
    ///   multiple names.  this is needed for NTupleSvc, so that it
    ///   can be located with and without the output tree name.
  private:
    virtual bool hasName (const std::string& name) const;



    //
    // inherited interface
    //

  public:
    virtual const std::string& name() const;


    //
    // friend interface for Job
    //

#if 1
    friend class AlgorithmWrapper;
    friend class Job;
  private:
#else
  public:
#endif

    /// effects: give the algorithm a chance to intialize the job with
    ///   anything this algorithm needs.  this method is automatically
    ///   called before the algorithm is actually added to the job.
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: this is currently used to give algorithms a chance
    ///   to register their output datasets, but can also be used for
    ///   other stuff.
    void sysSetupJob (Job& job);



    //
    // private interface
    //

    /// rationale: the worker is essentially controlling the
    ///   algorithm.  this allows it to do it without having to
    ///   duplicate most of the interface.
    friend class AlgorithmWrapper;

    // description: members directly corresponding to accessors
  private:
    IWorker *m_wk; //!

    /// \brief the value of \ref evtStore
  private:
    mutable asg::SgTEvent *m_evtStorePtr = nullptr; //!

    /// \brief when configured, the object returned by \ref evtStore
  private:
    mutable asg::SgTEvent m_evtStore; //!

    /// \brief the message stream, if it has been instantiated
  private:
    mutable MsgStream *m_msg = nullptr; //!

    /// \brief the algorithm name for which the message stream has
    /// been instantiated
  private:
    mutable std::string m_msgName; //!

    /// \brief the message level configured
  private:
    int m_msgLevel = 3;

    /// \brief the cache for \ref name
  private:
    mutable std::string m_nameCache; //!

    ClassDef(Algorithm, 1);
  };
}

#endif
