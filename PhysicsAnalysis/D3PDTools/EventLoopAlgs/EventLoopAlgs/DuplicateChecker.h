/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP_ALGS__DUPLICATE_CHECKER_H
#define EVENT_LOOP_ALGS__DUPLICATE_CHECKER_H

#include <EventLoopAlgs/Global.h>

#include <EventLoop/Algorithm.h>
#include <SampleHandler/Global.h>
#include <map>
#include <set>

namespace xAOD
{
  class TEvent;
}

class TObjString;
class TTree;

namespace EL
{
  /// \todo add documentation

  class DuplicateChecker : public Algorithm
  {
    //
    // public interface
    //

    /// \brief the integer type to use for run and event numbers
  private:
    typedef uint32_t number_type;


    /// \brief test the invariant of this object
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    DuplicateChecker ();


    /// \brief the name of the EventInfo structure to use
    ///
    /// This is mostly meant, so that in my unit test code I can point
    /// it to my own specially prepared EventInfo
    /// \par Guarantee
    ///   no-fail
  public:
    const std::string& eventInfoName () const;

    /// \brief set the value of \ref eventInfoName
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    void setEventInfoName (const std::string& val_eventInfoName);


    /// \brief the name of the output tree to create, or the empty
    /// string if none is created
    ///
    /// The output tree contains a list of run and event numbers for
    /// all events, and whether they were processed by this job.  This
    /// can be used to check whether duplicate events were processed
    /// (or whether we somehow eliminated events as duplicates that we
    /// shouldn't have).  It can also be used to create a list of
    /// duplicate events for future processing rounds.
  public:
    const std::string& outputTreeName () const;

    /// \brief set the value of \ref outputTreeName
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    void setOutputTreeName (const std::string& val_outputTreeName);


    /// \brief add a known duplicate event
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    void addKnownDuplicate (const std::string& sampleName,
			    const std::string& fileName,
			    Long64_t entry, number_type runNumber,
			    number_type eventNumber);

    /// \brief add a file with known duplicates
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   i/o errors\n
    ///   out of memory III
  public:
    void addKnownDuplicatesFile (const std::string& duplicatesFile);


    /// \brief process the summary tree from the given submission
    ///
    /// This will create a file "duplicates" inside the submission
    /// directory that contains the list of duplicates that can be fed
    /// into future submissions to filter them out.
    ///
    /// \return whether the job was successfully, i.e. each input
    /// event was read exactly once and all duplicates were skipped
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   i/o errors
  public:
    static bool
    processSummary (const std::string& submitdir, const std::string& treeName);


    /// \copydoc processSummary
    ///
    /// This version of the method provides a lower level interface,
    /// in which the list of inputs is given via a sample handler
    /// (with the tree name properly set), and the output file name
    /// freely choosable.
  public:
    static bool
    processSummary (const SH::SampleHandler& sh, const std::string& outputFile);



    //
    // inherited interface
    //

  private:
    virtual StatusCode setupJob (Job& job) override;

  private:
    virtual StatusCode changeInput (bool firstFile) override;

  private:
    virtual StatusCode initialize () override;

  private:
    virtual StatusCode execute () override;



    //
    // private interface
    //

    /// \brief the value returned by \ref eventInfoName
  private:
    std::string m_eventInfoName;

    /// \brief the value returned by \ref outputTreeName
  private:
    std::string m_outputTreeName;

    /// \brief the list of known duplicates to skip
  private:
    std::map<std::pair<std::string,std::string>,std::map<Long64_t,std::pair<number_type,number_type> > > m_duplicates;

    /// \brief the list of the duplicates in the current file to skip,
    /// or the null pointer if there are none
  private:
  std::map<Long64_t,std::pair<number_type,number_type> > *m_currentDuplicates = nullptr; //!


    /// \brief the list of run-event numbers already encountered
  private:
    std::set<std::pair<number_type,number_type> > m_processed; //!

    /// \brief the event we are reading from
  private:
    xAOD::TEvent *m_event = nullptr; //!


    /// \brief the output tree, if we are creating one
  private:
    TTree *m_outputTree = nullptr; //!

    /// \brief the name of the input file (connected to \ref
    /// m_outputTree, if present)
  private:
    std::string m_inputFileName; //!

    /// \brief the index in the input file (connected to \ref
    /// m_outputTree, if present)
  private:
    Long64_t m_inputFileIndex; //!

    /// \brief the run number of the current event (connected to \ref
    /// m_outputTree, if present)
  private:
    number_type m_runNumber; //!

    /// \brief the event number of the current event (connected to
    /// \ref m_outputTree, if present)
  private:
    number_type m_eventNumber; //!

    /// \brief whether the current event is/should be processed
    /// (connected to \ref m_outputTree, if present)
  private:
    Bool_t m_processEvent; //!

    /// \brief get the run and event number for the current event
  private:
    void read_run_event_number ();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
    ClassDef(DuplicateChecker, 1);
#pragma GCC diagnostic pop
  };
}

#endif
