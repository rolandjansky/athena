/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_METRICS_SVC_H
#define EVENT_LOOP_METRICS_SVC_H

/// This module defines the services for saving performance data.
/// The interface provided in this module is intended for experts only.

#include <EventLoop/Algorithm.h>

class TBenchmark;
class TTree;

namespace EL
{
  class MetricsSvc : public EL::Algorithm
  {
    //
    // public interface
    //

    /// description: the name of the service
  public:
    static const std::string name;


    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;

  
    /// effects: standard constructor.
    /// guarantee: no-fail
  public:
    MetricsSvc ();

    /// effects: standard destructor.
    /// guarantee: no-fail
  public:
    ~MetricsSvc ();



    //
    // interface inherited from Algorithm
    //

    /// effects: return the name of this algorithm
    /// guarantee: no-fail
  public:
    virtual const char *GetName () const override;

    
    /// effects: do all the processing that needs to be done once per file
    /// guarantee: no-fail
  private:
    virtual StatusCode fileExecute () override;


    /// effects: do the post-processing for each input file
    /// guarantee: basic
    /// failures: out of memory
    /// failures: in/out trees or input file not available
  private:
    virtual StatusCode endOfFile () override;


    /// effects: create the output TTrees 
    /// guarantee: basic
    /// failures: out of memory
    /// failures: output already exists
  private:
    virtual StatusCode histInitialize () override;


    /// effects: process the next event
    /// guarantee: no-fail
  private:
    virtual StatusCode execute () override;


    /// effects: do the job post-processing
    /// guarantee: basic
    /// failures: out of memory
    /// failures: in/out trees or input file not available
  private:
    virtual StatusCode histFinalize () override;



    //
    // private interface
    //

    /// description: the file cache metrics
  private:
    TTree *m_fileMetrics; //!
    
    /// description: the job metrics
  private:
    TTree *m_jobMetrics; //!
    
    /// description: the number of files processed
  private:
    Int_t m_filesRead; //!
    
    /// description: the number of events processed
  private:
    Int_t m_eventsRead; //!

    /// description: the benchmark object used
    /// rationale: this is a pointer to avoid including the header in this file
  private:
    TBenchmark *m_benchmark; //!
    
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
    ClassDef(MetricsSvc, 1);
#pragma GCC diagnostic pop
  };
}


#endif
