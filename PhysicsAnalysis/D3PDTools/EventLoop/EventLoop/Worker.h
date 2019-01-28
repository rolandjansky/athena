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

#include <AnaAlgorithm/IFilterWorker.h>
#include <AnaAlgorithm/IHistogramWorker.h>
#include <AnaAlgorithm/ITreeWorker.h>
#include <EventLoop/ModuleData.h>
#include <Rtypes.h>
#include <map>

namespace xAOD
{
  class TEvent;
  class TStore;
}

namespace EL
{
  class Worker : public IFilterWorker, public IHistogramWorker,
                 public ITreeWorker, private Detail::ModuleData
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


    /// \brief add the given module to this worker
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  protected:
    void addModule (std::unique_ptr<Detail::Module> module);


    /// \brief initialize the worker
    ///
    /// This method ought to be called after the options on the worker
    /// are set and before any events are processed.  It is meant to
    /// make sure everything is ready and set up for event processing.
    ///
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   initialization failures
  protected:
    ::StatusCode initialize ();


    /// \brief finalize the worker
    ///
    /// This method ought to be called after all events have been
    /// processed.  It is meant to ensure that the job is ended
    /// properly and all outputs are written out and files are closed.
    ///
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   finalization failures
  protected:
    ::StatusCode finalize ();


    /// \brief process the given event range
    ///
    /// This will update `eventRange` if the end is set to eof
    ///
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   file can't be opened\n
    ///   event range exceeds length of file\n
    ///   processing failures
  protected:
    ::StatusCode processEvents (EventRange& eventRange);


    /// \brief open the given input file without processing it
    ///
    /// This is mostly to allow the driver to query the number of
    /// events in the input file without processing it, usually to
    /// determine the range of events to process.
    ///
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   file can't be opened
  protected:
    ::StatusCode openInputFile (std::string inputFileUrl);


    /// effects: add another output file
    /// guarantee: strong
    /// failures: low level errors II
    /// failures: label already used
    /// requires: file_swallow != 0
  protected:
    ::StatusCode addOutputFile (const std::string& label,
                                std::unique_ptr<TFile> file);
    ::StatusCode addOutputWriter (const std::string& label,
                                  std::unique_ptr<SH::DiskWriter> writer);


    /// effects: tell all algorithms that they should process the next
    ///   event
    /// guarantee: basic
    /// failures: algorithm dependent
    /// requires: file pointers are set properly
    /// requires: algorithms are initialized
  private:
    ::StatusCode algsExecute ();


    /// \brief the number of events in the input file
    /// \par Guarantee
    ///   no-fail
    /// \pre inputFile() != 0
  protected:
    Long64_t inputFileNumEntries () const;


    /// \brief the number of events that have been processed
    /// \par Guarantee
    ///   no-fail
  protected:
    uint64_t eventsProcessed () const noexcept;



    //
    // virtual interface
    //



    //
    // private interface
    //

  private:
    Long64_t m_inputTreeEntry {0};

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


    /// description: whether we are skipping the event
  private:
    bool m_skipEvent;


    /// \brief the list of modules we hold
  private:
    std::vector<std::unique_ptr<Detail::Module> > m_modules; //!


    /// \brief whether this is a new input file (i.e. one that has not
    /// yet been connected to the algorithms)
  private:
    bool m_newInputFile {false};
  };
}

#endif
