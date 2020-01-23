/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__I_WORKER_H
#define EVENT_LOOP__I_WORKER_H

#include <EventLoop/Global.h>

#include <AnaAlgorithm/IFilterWorker.h>
#include <AnaAlgorithm/IHistogramWorker.h>
#include <AnaAlgorithm/ITreeWorker.h>

#include <Rtypes.h>
#include <SampleHandler/Global.h>

class TFile;

namespace xAOD
{
  class TEvent;
  class TStore;
}

namespace EL
{
  /// \brief the interface for algorithms to access \ref IWorker
  ///
  /// Up to now (16 Jan 19) algorithms could directly access the
  /// worker object.  However, there is no good reason for algorithms
  /// to access the worker directly, so I now put an interface between
  /// the two.

  class IWorker : public IFilterWorker, public IHistogramWorker,
                  public ITreeWorker
  {

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
    virtual void addOutputList (const std::string& name, TObject *output_swallow) = 0;


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
    virtual TFile *getOutputFile (const std::string& label) const = 0;


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
    virtual TFile *getOutputFileNull (const std::string& label) const = 0;


    /// description: the sample meta-data we are working on
    /// guarantee: no-fail
    /// invariant: metaData != 0
    /// rationale: this can be used for accessing sample meta-data
  public:
    virtual const SH::MetaObject *metaData () const = 0;


    /// description: the tree we are running on
    /// guarantee: no-fail
  public:
    virtual TTree *tree () const = 0;


    /// description: the entry in the tree we are reading
    /// guarantee: no-fail
  public:
    virtual Long64_t treeEntry () const = 0;


    /// description: the file we are reading the current tree from
    /// guarantee: no-fail
  public:
    virtual TFile *inputFile () const = 0;


    /// \brief the name of the file we are reading the current tree
    /// from, without the path component
    /// \par Guarantee
    ///   no-fail
  public:
    virtual std::string inputFileName () const = 0;


    /// description: the trigger config tree from the input file, or
    ///   NULL if we did not find it
    /// guarantee: strong
    /// failures: i/o errors
  public:
    virtual TTree *triggerConfig () const = 0;


    /// description: the xAOD event and store
    /// guarantee: strong
    /// failures: out of memory I
    /// failures: TEventSvc not configured
    /// postcondition: result != 0
  public:
    virtual xAOD::TEvent *xaodEvent () const = 0;
    virtual xAOD::TStore *xaodStore () const = 0;


    /// effects: returns the algorithms with the given name or NULL if
    ///   there is none
    /// guarantee: strong
    /// failures: out of memory II
  public:
    virtual EL::Algorithm *getAlg (const std::string& name) const = 0;


    /// effects: skip the current event, i.e. skip the rest of the
    ///   algorithms for this event
    /// guarantee: no-fail
    /// rationale: if you have an analysis strategy in which you
    ///   divide work into multiple algorithms you may want to have
    ///   dedicated algorithms for event selection that then skip
    ///   later algorithms that fill histograms
  public:
    virtual void skipEvent () = 0;
  };
}

#endif
