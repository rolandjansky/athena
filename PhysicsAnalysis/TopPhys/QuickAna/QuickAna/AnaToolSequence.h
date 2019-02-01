/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__ANA_TOOL_SEQUENCE_H
#define QUICK_ANA__ANA_TOOL_SEQUENCE_H

#include <QuickAna/Global.h>

#include <list>
#include <memory>
#include <vector>

namespace ana
{
  /// \brief this class defines/manages a tool sequence
  ///
  /// A tool sequence is similar to an algorithm sequence, but it
  /// allows to have named slots in it, which allow different options
  /// for merging multiple sequences together, e.g. a tool can request
  /// to be merged into a specific slot of another sequence and then
  /// gets merged in there.

  class AnaToolSequence
  {
    //
    // public interface
    //

    // no copying
    AnaToolSequence (const AnaToolSequence&) = delete;
    AnaToolSequence& operator = (const AnaToolSequence&) = delete;


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
    AnaToolSequence ();


    /// \brief standard move constructor
    /// \par Guarantee
    ///   no-fail
  public:
    AnaToolSequence (AnaToolSequence&& that);


    /// \brief standard move assignment
    /// \par Guarantee
    ///   no-fail
  public:
    AnaToolSequence& operator = (AnaToolSequence&& that);


    /// \brief standard swap function
    /// \par Guarantee
    ///   no-fail
  public:
    void swap (AnaToolSequence& that);


    /// \brief add a new tool to the sequence
    /// \brief Guarantee
    ///   strong
    /// \brief Failures
    ///   out of memory I
  public:
    void addTool (std::unique_ptr<IAnaTool> val_definition);


    /// \brief add a new label to the sequence
    /// \brief Guarantee
    ///   strong
    /// \brief Failures
    ///   out of memory II
  public:
    void addLabel (const std::string& val_label);


    /// \brief STL-empty function
    /// \par Guarantee
    ///   no-fail
  public:
    bool empty () const;


    /// \brief STL-iterator
  public:
    typedef std::list<std::pair<std::unique_ptr<IAnaTool>,std::string>>::iterator iterator;


    /// \brief STL-begin function
    /// \par Guarantee
    ///   no-fail
  public:
    iterator begin ();


    /// \brief STL-end function
    /// \par Guarantee
    ///   no-fail
  public:
    iterator end ();



    //
    // private interface
    //

    /// \brief the list of tools/slots in the sequence
  private:
    std::list<std::pair<std::unique_ptr<IAnaTool>,std::string>> m_tools;
  };



  /// \brief make all definitions from the configuration
  ///
  /// this is used to separate the tool generation completely from
  /// the tool scheduler
  /// \par Guarantee
  ///   basic
  /// \par Failures
  ///   out of memory III
  /// \par Failures
  ///   invalid configuration
  StatusCode
  makeToolSequence (const std::vector<DefinitionConf>& confList,
		    const DefinitionArgs& masterArgs,
		    AnaToolSequence& tools);
}

#endif
