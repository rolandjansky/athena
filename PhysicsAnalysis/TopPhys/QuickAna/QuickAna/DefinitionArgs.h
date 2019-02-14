/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__DEFINITION_ARGS_H
#define QUICK_ANA__DEFINITION_ARGS_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

#include <QuickAna/AnaToolSequence.h>
#include <list>
#include <memory>
#include <string>

namespace ana
{
  /// \brief the arguments passed into the makeXXX functions
  ///
  /// Object definitions are created via factory methods.  In order to
  /// make the interface of the factory methods stable it takes as an
  /// argument a single object of this class.  The class itself can
  /// then be altered as the need for additional arguments to the
  /// factory method arises.
  ///
  /// This is also used (partially filled) inside the definition maker
  /// to pass around arguments.  This is mostly to avoid lengthy
  /// argument lists, particularly when they are passed through
  /// multiple functions

  class DefinitionArgs
  {
    //
    // public interface
    //

    // not copyable
    DefinitionArgs (const DefinitionArgs&) = delete;
    DefinitionArgs& operator = (const DefinitionArgs&) = delete;


    /// \brief check the invariant of this object
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    DefinitionArgs (const std::string& val_prefix,
		    InternalConfiguration *val_configuration);


    /// \brief standard move constructor
    /// \par Guarantee
    ///   no-fail
  public:
    DefinitionArgs (DefinitionArgs&&) = default;


    /// \brief make a clone of this definition (without the associated
    /// tool sequence)
    ///
    /// I use this instead of the copy constructor to make it clear
    /// that the tool sequence does not get copied.
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    DefinitionArgs cloneConf () const;


    /// \brief the prefix to use for tools to be created
    ///
    /// tools need to have unique names, and this is the way to ensure
    /// them.
    /// \par Guarantee
    ///   no-fail
    /// \post !result.empty()
  public:
    const std::string& prefix () const;


    /// \brief add to the prefix
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    void addToPrefix (const std::string& extension);


    /// \brief the internal configuration to use
    /// \par Guarantee
    ///   no-fail
    /// \post result != nullptr
  public:
    InternalConfiguration *configuration () const;


    /// \brief whether this is the first working point for the object
    /// definition
    /// \par Guarantee
    ///   no-fail
  public:
    bool firstWP () const;

    /// \brief set the value of \ref firstWP
    /// \par Guarantee
    ///   no-fail
  public:
    void setFirstWP (bool val_firstWP);


    /// \brief add a new tool to the tool sequence
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    void add (std::unique_ptr<IAnaTool> val_tool);


    /// \brief whether the definition has been filled called to fill
    /// the sequence
    /// \par Guarantee
    ///   no-fail
  public:
    bool filled () const;


    /// \brief extract the sequence created
    /// \par Guarantee
    ///   no-fail
  public:
    AnaToolSequence&& extractSequence ();



    //
    // private interface
    //

    /// \brief the value of \ref prefix
  private:
    std::string m_prefix;

    /// \brief the value of \ref firstWP
  private:
    bool m_firstWP = true;

    /// \brief the value of \ref configuration
  private:
    InternalConfiguration *m_configuration;

    /// \brief the tool sequence defined
  private:
    AnaToolSequence m_sequence;



    //
    // legacy interface
    //

    /// \brief the object type we are creating this for
    /// \par Guarantee
    ///   no-fail
  public:
    ObjectType type () const;

    /// \brief set the value of \ref type
    /// \par Guarantee
    ///   no-fail
  public:
    void setType (ObjectType val_type);


    /// \brief the name of the definition
    /// \par Guarantee
    ///   no-fail
  public:
    const std::string& name () const;

    /// \brief set the value of \ref name
    /// \par Guarantee
    ///   no-fail
  public:
    void setName (const std::string& val_name);


    /// \brief members directly corresponding to accessors
  private:
    ObjectType m_type;
  private:
    std::string m_name;
  };
}

#endif
