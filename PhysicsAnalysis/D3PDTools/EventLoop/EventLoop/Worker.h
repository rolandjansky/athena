/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_WORKER_HH
#define EVENT_LOOP_WORKER_HH

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.



#include <EventLoop/Global.h>

#include <map>
#include <memory>
#include <vector>
#include <Rtypes.h>
#include <TList.h>
#include <SampleHandler/Global.h>
#include <AnaAlgorithm/IFilterWorker.h>
#include <AnaAlgorithm/IHistogramWorker.h>
#include <AnaAlgorithm/ITreeWorker.h>

class TFile;
class TH1;
class TString;
class TTree;
class TStopwatch;

namespace xAOD
{
  class TEvent;
  class TStore;
}

namespace EL
{
  class Worker : public IFilterWorker, public IHistogramWorker,
                 public ITreeWorker
  {
    //
    // public interface
    //

    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;


    /// effects: standard destructor
    /// guarantee: no-fail
    /// rationale: virtual destructor for base class
  public:
    virtual ~Worker ();


    /// effects: add an object to the output.  the worker takes over
    ///   ownership of the object, but the caller may maintain a
    ///   reference to it
    /// guarantee: basic, argument is always swallowed
    /// failures: out of memory I
    /// requires: output_swallow != 0
    /// warning: so far I placed no requirements on the output
    ///   objects.  I may do that at a later stage though, possibly
    ///   breaking existing code.
  public:
    void addOutput (TObject *output_swallow) final override;


    /// effects: add a given object to the output.  instead of trying
    ///   to merge the outputs from multiple jobs the output file will
    ///   contain a TList of the given name with the objects from all
    ///   the output.
    /// guarantee: basic, argument is always swallowed
    /// failures: out of memory II
    /// requires: output_swallow != 0
    /// rationale: This was primarily build to allow the GRL output to
    ///   be collected from EventLoop jobs, but it can be used for any
    ///   kind of output that can not or should not be merged.
  public:
    void addOutputList (const std::string& name, TObject *output_swallow);


    /// \brief get the output histogram with the given name
    ///
    /// This is mostly meant, so that I can emulate the Athena
    /// histogram mechanism.
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   object not found
    /// \post result != 0
  public:
    TH1 *getOutputHist (const std::string& name) const final override;


    /// effects: get the output file that goes into the dataset with
    ///   the given label.  this dataset has to be registered before
    ///   the job is submitted.  typically that happens in the
    ///   doSetupJob method.
    /// guarantee: strong
    /// failures: no dataset with the given label
    /// postcondition: result != 0
    /// note: the default value for the argument corresponds to the
    ///   default label value in the OutputInfo class.
  public:
    TFile *getOutputFile (const std::string& label) const;


    /// effects: get the output file that goes into the dataset with
    ///   the given label.  this dataset has to be registered before
    ///   the job is submitted.  typically that happens in the
    ///   doSetupJob method.
    /// guarantee: strong
    /// failures: internal errors
    /// note: the default value for the argument corresponds to the
    ///   default label value in the OutputInfo class.
    /// rationale: this method was added to support optional output
    ///   files, i.e.: the algorithm leaves it to the user whether or
    ///   not the output file gets configured.  then the algorithm
    ///   just checks whether the output file is there. if so it fills
    ///   it, otherwise it ignores it.
  public:
    TFile *getOutputFileNull (const std::string& label) const;


    /// effects: adds a tree to an output file specified by the stream/label
    /// failures: Incorrect stream/label specified, called at the wrong time
    /// note: See getOutputFile for failure types...
  public:
    ::StatusCode addTree( const TTree& tree,
                          const std::string& stream ) final override;


    /// effects: get the tree that was added to an output file earlier
    /// failures: Tree doesn't exist
  public:
    TTree* getOutputTree( const std::string& name,
                          const std::string& stream ) const final override;


    /// description: the sample meta-data we are working on
    /// guarantee: no-fail
    /// invariant: metaData != 0
    /// rationale: this can be used for accessing sample meta-data
  public:
    const SH::MetaObject *metaData () const;


    /// description: the tree we are running on
    /// guarantee: no-fail
  public:
    TTree *tree () const;


    /// description: the entry in the tree we are reading
    /// guarantee: no-fail
  public:
    Long64_t treeEntry () const;


    /// description: the file we are reading the current tree from
    /// guarantee: no-fail
  public:
    TFile *inputFile () const;


    /// \brief the name of the file we are reading the current tree
    /// from, without the path component
    /// \par Guarantee
    ///   no-fail
  public:
    std::string inputFileName () const;


    /// description: the trigger config tree from the input file, or
    ///   NULL if we did not find it
    /// guarantee: strong
    /// failures: i/o errors
  public:
    TTree *triggerConfig () const;


    /// description: the xAOD event and store
    /// guarantee: strong
    /// failures: out of memory I
    /// failures: TEventSvc not configured
    /// postcondition: result != 0
  public:
    xAOD::TEvent *xaodEvent () const;
    xAOD::TStore *xaodStore () const;


    /// effects: returns the algorithms with the given name or NULL if
    ///   there is none
    /// guarantee: strong
    /// failures: out of memory II
  public:
    EL::Algorithm *getAlg (const std::string& name) const;


    /// effects: skip the current event, i.e. skip the rest of the
    ///   algorithms for this event
    /// guarantee: no-fail
    /// rationale: if you have an analysis strategy in which you
    ///   divide work into multiple algorithms you may want to have
    ///   dedicated algorithms for event selection that then skip
    ///   later algorithms that fill histograms
  public:
    void skipEvent ();


    /// \brief whether the current algorithm passed its filter
    /// criterion for the current event
    /// \par Guarantee
    ///   no-fail
  public:
    virtual bool filterPassed () const noexcept final override;

    /// \brief set the value of \ref filterPassed
    /// \par Guarantee
    ///   no-fail
  public:
    virtual void setFilterPassed (bool val_filterPassed) noexcept final override;



    //
    // protected interface
    //

    /// effects: standard constructor
    /// guarantee: strong
    /// failures: low level errors I
    /// requires: val_metaData != 0
    /// requires: output != 0
    /// warning: you have to keep the meta-data object around until
    ///   the worker object is destroyed.
  protected:
    Worker (const SH::MetaObject *val_metaData, TList *output);


    /// \brief set the \ref JobConfig
    ///
    /// This takes care of adding the algorithms, etc. (only
    /// algorithms for now, 03 Feb 17).
    ///
    /// Note the rvalue calling convention here: Algorithms are
    /// objects that get modified, so if you use them more than once
    /// you need to copy/clone them.  However, in practice no driver
    /// should need that (though many do for now, 03 Feb 17), as
    /// drivers generally stream the JobConfig in for one-time use.
    ///
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   job configuration/streaming errors
  protected:
    void setJobConfig (JobConfig&& jobConfig);


    /// effects: add another output file
    /// guarantee: strong
    /// failures: low level errors II
    /// failures: label already used
    /// requires: file_swallow != 0
  protected:
    void addOutputFile (const std::string& label, TFile *file_swallow);
    void addOutputWriter (const std::string& label,
			  SH::DiskWriter *writer_swallow);


    /// effects: tell all algorithms that we started a new file, so
    ///   they should either initialize themselves, or update file
    ///   specific pointers
    /// guarantee: basic
    /// failures: algorithm dependent
    /// requires: file pointers are set properly
    /// postcondition: algorithms are initialized
  protected:
    void algsChangeInput ();


    /// effects: tell all algorithms that they should process the next
    ///   event
    /// guarantee: basic
    /// failures: algorithm dependent
    /// requires: file pointers are set properly
    /// requires: algorithms are initialized
  protected:
    void algsExecute ();


    /// effects: do any actions that should happen at the end of a file
    /// guarantee: basic
    /// failures: configuration dependent
    /// requires: file pointers are set properly
    /// requires: algorithms are initialized
    /// rationale: this doesn't actually talk to the algorithms, but
    ///   is still named in the same manner to indiciate that it is to
    ///   be used in the same way as the ones notifying the
    ///   algorithms.
    /// rationale: calling this function is optional, since not all
    ///   submission modes can support (especially not PROOF)
  protected:
    void algsEndOfFile ();


    /// effects: tell all algorithms that they should finish processing
    /// guarantee: basic
    /// failures: algorithm dependent
    /// failures: worker in wrong state
    /// requires: file pointers are set properly
  protected:
    void algsFinalize ();


    /// description: s.a.
  protected:
    void treeEntry (Long64_t val_treeEntry);


    /// \brief set the value of \ref inputFile
    ///
    /// as side effects it also retrieves the input tree from the
    /// file, and sets the cache parameters if requested by the user.
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   errors initializing input tree
  protected:
    void setInputFile (TFile *val_inputFile);


    /// \brief set the input tree (for PROOF driver only)
    ///
    /// Normal workers should set the input file/tree via \ref
    /// setInputFile, which handles empty files gracefully.  However
    /// since PROOF natively works on the level of trees and filters
    /// out empty files (plus sets the cache parameters internally),
    /// this special routine is provided for it.
    /// \par Guarantee
    ///   no-fail
  protected:
    void setTreeProofOnly (TTree *val_tree);


    /// \brief the number of events in the input file
    /// \par Guarantee
    ///   no-fail
    /// \pre inputFile() != 0
  protected:
    Long64_t inputFileNumEntries () const;


    /// \brief Resident memory leak/increase during the job
    ///
    /// The idea here is that the worker captures the amount of memory used by
    /// the job during initialisation and finalisation, so that a rough estimate
    /// could be made about the memory leak behaviour of the user's code.
    ///
    /// This function returns the number of kilobytes by which the used resident
    /// memory changed between initialisation and finalisation. If the function
    /// is called at an incorrect point in time, it throws an exception.
    ///
  protected:
    Long_t memIncreaseResident () const;


    /// \brief Virtual memory leak/increase during the job
    ///
    /// The idea here is that the worker captures the amount of memory used by
    /// the job during initialisation and finalisation, so that a rough estimate
    /// could be made about the memory leak behaviour of the user's code.
    ///
    /// This function returns the number of kilobytes by which the used virtual
    /// memory changed between initialisation and finalisation. If the function
    /// is called at an incorrect point in time, it throws an exception.
    ///
  protected:
    Long_t memIncreaseVirtual () const;



    //
    // virtual interface
    //



    //
    // private interface
    //

    /// description: members directly corresponding to accessors
  private:
    const SH::MetaObject *m_metaData = nullptr;

    /// \brief the value of \ref inputFile
  private:
    TFile *m_inputFile = nullptr;

    /// \brief the value of \ref tree
  private:
    TTree *m_tree = nullptr;

  private:
    Long64_t m_treeEntry;

    /// description: the output list
  private:
    TList *m_output = nullptr;


    /// \brief the output map
  private:
    typedef std::map<std::string,TH1*>::const_iterator OutputHistMapIter;
    std::map<std::string,TH1*> m_outputHistMap;


    /// description: the list of output trees
  private:
    typedef std::map<std::pair<std::string,std::string>,TTree*>::const_iterator
       OutputTreeMapIter;
    std::map<std::pair<std::string,std::string>,TTree*> m_outputTreeMap;


    /// description: the list of output files
  private:
    typedef std::map<std::string,SH::DiskWriter*>::const_iterator outputFilesIter;
    std::map<std::string,SH::DiskWriter*> m_outputFiles;


    /// description: the list of algorithms
  private:
    typedef std::vector<EL::Algorithm*>::const_iterator algsIter;
    std::vector<EL::Algorithm*> m_algs;


    /// description: the event counter for the algorithms
  private:
    TH1 *m_eventCount = nullptr;


    /// \brief the run time summary histogram
  private:
    TH1 *m_runTime = nullptr;


    /// \brief Tree saving per-job statistics information
  private:
    TTree *m_jobStats = nullptr;


    /// \brief the tree containing the list of files for which
    /// fileExecute has been called
  private:
    TTree *m_fileExecutedTree = nullptr;

    /// \brief the name of the file being executed, to be stored
    /// inside \ref m_fileExecutedTree
  private:
    TString *m_fileExecutedName = nullptr;


    /// description: whether we are skipping the event
  private:
    bool m_skipEvent;


    /// \brief the stop watch we use for measuring total time spend
  private:
    std::unique_ptr<TStopwatch> m_stopwatch;


    /// \brief Amount of resident memory used after initialisation in kB
  private:
    Long_t m_initMemResident = -1;

    /// \brief Amount of virtual memory used after initialisation in kB
  private:
    Long_t m_initMemVirtual = -1;

    /// \brief Amount of resident memory used after finalisation in kB
  private:
    Long_t m_finMemResident = -1;

    /// \brief Amount of virtual memory used after finalisation in kB
  private:
    Long_t m_finMemVirtual = -1;


    /// \brief the initialization state of the algorithms
  private:
    enum AlgInitState
    {
      /// \brief the algorithms have been created, but not yet
      /// initialized
      AIS_NEW,

      /// \brief the algorithms have been hist-initialized, but not
      /// yet initialized
      AIS_HIST_INITIALIZED,

      /// \brief the algorithms have been initialized and
      /// hist-initialized
      AIS_INITIALIZED,

      /// \brief the algorithms have been finalized, but not
      /// hist-finalized
      AIS_FINALIZED,

      /// \brief the algorithms have been finalized as well as
      /// hist-finalized
      AIS_HIST_FINALIZED,

      /// \brief the algorithms are in an inconsistent and essentially
      /// broken state.  as an argument for changing state, it means
      /// there is no target for the initialization state
      AIS_NONE
    };

    /// \brief the value of \ref AlgInitState
  private:
    AlgInitState m_initState;


    /// \brief the execution state of the algorithms
  private:
    enum AlgExecState
    {
      /// \brief the algorithms are in a blank state, i.e. not pointed
      /// at any file
      AES_BLANK,

      /// \brief the algorithms have received fileExecute for the
      /// current file, if appropriate (i.e. on event 0)
      AES_FILE_EXECUTED,

      /// \brief the algorithms have received the changeInput signal
      /// for the current file
      AES_INPUT_CHANGED,

      /// \brief the algorithms are in an inconsistent and essentially
      /// broken state.  as an argument for changing state, it means
      /// there is no target for the execution state
      AES_NONE
    };

    /// \brief the value of \ref AlgExecState
  private:
    AlgExecState m_execState;


    /// \brief the current state of the input
  private:
    enum InputState
    {
      /// \brief the input is not valid at all
      VALID_NONE,

      /// \brief the input file is valid, but the input event may not
      /// be
      VALID_FILE,

      /// \brief the input file and event are both valid
      VALID_EVENT
    };


    /// \brief switch to the given target state for the algorithms
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   invalid target state\n
    ///   algorithm failure
  private:
    void changeAlgState (AlgInitState targetInit, AlgExecState targetExec,
			 InputState inputState);
  };
}

#endif
