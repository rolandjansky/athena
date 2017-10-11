/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__DEFINITION_MAKER_H
#define QUICK_ANA__DEFINITION_MAKER_H

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

#include <AsgTools/StatusCode.h>
#include <functional>
#include <string>
#include <vector>

namespace ana
{
  /// \brief this object wraps the factory method that creates an
  /// object definition
  ///
  /// Usually even the advanced user has nothing to do with this class
  /// directly.  The instances get instantiated and registered via the
  /// various *DEFINITION_MAKER* macros.

  class DefinitionMaker
  {
    //
    // static interface
    //

    /// \brief add a new object definition
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    static void addDefinition (DefinitionMaker val_definition);


    /// \brief make a new definition
    /// \brief Guarantee
    ///   basic
    /// \brief Failures
    ///   out of memory III\n
    ///   no definitions matching the name\n
    ///   multiple definitions matching the name
  public:
    static StatusCode makeDefinition (DefinitionArgs& args);


    /// effects: make all definitions from the configuration
    /// guarantee: basic
    /// failures: out of memory III
    /// failures: invalid configuration
    /// rationale: this is used to separate the tool generation
    ///   completely from the tool scheduler
  public:
    static StatusCode
    makeDefinitions (const Configuration& config,
		     InternalConfiguration& internal,
		     const std::string& prefix,
		     std::vector<IAnaTool*>& tools);


    //
    // public interface
    //

    /// \brief check the invariant of this object
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief standard constructor
    /// \par Guarantee
    ///   no-fail
  public:
    DefinitionMaker (ObjectType val_type, const std::string& val_name,
		     const std::function<StatusCode (DefinitionArgs&)>& val_function);



    //
    // private interface
    //

    /// \brief the object type for which this definition is defined
  private:
    ObjectType m_type;

    /// \brief the name of the object definition
  private:
    std::vector<std::string> m_names;

    /// \brief the actual factory method
  private:
    std::function<StatusCode (DefinitionArgs&)> m_function;



    //
    // legacy interface
    //

    /// \brief make a new definition, or do nothing
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory III
    /// \par Failures
    ///   definition maker didn't do anything
    /// \pre args.listSize() == 0
  private:
    StatusCode
    doMakeDefinition (DefinitionArgs& args, bool& foundName) const;
  };
}

/// TODO: REALLY needs documentation
#define QUICK_ANA_DEFINITION_MAKER(type,name,call) \
  namespace {					       \
    int QUICK_ANA_JOIN_STR (quickAnaInit, __LINE__) () {		\
      ::ana::DefinitionMaker::addDefinition (::ana::DefinitionMaker ((type), (name), [] (::ana::DefinitionArgs& args) {return (call);})); \
      return 0; }							\
    int QUICK_ANA_JOIN_STR (quickAnaDummy, __LINE__) = QUICK_ANA_JOIN_STR (quickAnaInit, __LINE__) (); }

#define QUICK_ANA_EVENTINFO_DEFINITION_MAKER(name,call) \
  QUICK_ANA_DEFINITION_MAKER (::ana::OBJECT_EVENTINFO, name, call)
#define QUICK_ANA_EVENT_SELECT_DEFINITION_MAKER(name,call) \
  QUICK_ANA_DEFINITION_MAKER (::ana::OBJECT_EVENT_SELECT, name, call)
#define QUICK_ANA_JET_DEFINITION_MAKER(name,call) \
  QUICK_ANA_DEFINITION_MAKER (::ana::OBJECT_JET, name, call)
#define QUICK_ANA_PHOTON_DEFINITION_MAKER(name,call) \
  QUICK_ANA_DEFINITION_MAKER (::ana::OBJECT_PHOTON, name, call)
#define QUICK_ANA_ELECTRON_DEFINITION_MAKER(name,call) \
  QUICK_ANA_DEFINITION_MAKER (::ana::OBJECT_ELECTRON, name, call)
#define QUICK_ANA_MUON_DEFINITION_MAKER(name,call) \
  QUICK_ANA_DEFINITION_MAKER (::ana::OBJECT_MUON, name, call)
#define QUICK_ANA_TAU_DEFINITION_MAKER(name,call) \
  QUICK_ANA_DEFINITION_MAKER (::ana::OBJECT_TAU, name, call)
#define QUICK_ANA_MET_DEFINITION_MAKER(name,call) \
  QUICK_ANA_DEFINITION_MAKER (::ana::OBJECT_MET, name, call)
#define QUICK_ANA_OR_DEFINITION_MAKER(name,call) \
  QUICK_ANA_DEFINITION_MAKER (::ana::OBJECT_OVERLAP_REMOVAL, name, call)
#define QUICK_ANA_TRIGGER_DEFINITION_MAKER(name,call) \
  QUICK_ANA_DEFINITION_MAKER (::ana::OBJECT_TRIGGER, name, call)

#endif
