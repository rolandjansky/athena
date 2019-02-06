/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__EVENT_DATA_H
#define QUICK_ANA__EVENT_DATA_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

#include <PATInterfaces/SystematicSet.h>
#include <unordered_map>

namespace ana
{
  /// \brief the list of all container names we put into the event
  ///
  /// This is meant to be used in several places:
  ///  - The output tool can use this to determine which containers it
  ///    has to write out
  ///  - If this structure is written out as part of the xAOD
  ///    meta-data, the user can use it to determine the content of a
  ///    file when reading it back, i.e. which object types and
  ///    systematics are contained and what containers to retrieve for
  ///    each.
  ///  - In a similar manner to reading a fully corrected xAOD file,
  ///    this would also allow users to run QuickAna in one algorithm
  ///    and then retrieve the QuickAna output from another algorithm.
  ///
  /// The interface here is probably going to change: For one thing I
  /// want to phase out the ObjectType enum, so this will have to be
  /// changed from ObjectType to string at some point in the future.
  /// For another, it might be nice to provide a retrieve function as
  /// well.  Also, I probably need some functions to check which
  /// containers and systematics are actually defined.  And there may
  /// be some changes as well to make this consistent with how xAOD
  /// meta-data is used.
  ///
  /// For now this is just bolted on to the rest of the QuickAna
  /// implementation, but the hope is that I can eventually integrate
  /// this more closely into the rest of the implementation, which may
  /// require some further changes.

  class EventData
  {
    //
    // public interface
    //

    /// \brief test the invariant of this object
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief standard constructor
    /// \par Guarantee
    ///   no-fail
  public:
    EventData ();


    /// \brief get the list of object types available
    ///
    /// This is a set instead of a bit-map to make it easier to
    /// migrate from ObjectType to std::string in the future.
    /// \par Guarantee
    ///   no-fail
  public:
    const std::set<ObjectType>& getObjectTypes () const;


    /// \brief get the list of systematics configured
    /// \par Guarantee
    ///   no-fail
  public:
    const std::set<CP::SystematicSet>& getSystematics () const;


    /// \brief get the name of the given collection for the given
    /// systematic, or the empty string if that collection is not
    /// defined.
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   unknown systematic for the given collection
    /// \pre unsigned (type) < EnumSize<ObjectType>::dataSize
  public:
    const std::string&
    getName (ObjectType type, const CP::SystematicSet& sys) const;

    /// \brief set the name returned by \ref getName
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II
    /// \pre unsigned (type) < EnumSize<ObjectType>::dataSize
    /// \pre !val_name.empty()
  public:
    void
    setName (ObjectType type, const CP::SystematicSet& sys,
	     const std::string& val_name);

    /// \brief get the list of all unique names for one object type
    /// \pre unsigned (type) < EnumSize<ObjectType>::dataSize
  public:
    std::vector<std::string>
    getNames (ObjectType type) const;


    /// \brief get the name of the original collection from which the
    /// collection returned by \ref getName was copied
    ///
    /// This is for shallow copies only.  This is just an empty string
    /// if the given collection was not obtained by shallow-copying an
    /// input container.
    ///
    /// This is meant solely for providing the output tool with all
    /// the information it needs.  ordinary users should not need to
    /// access this information at all.
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   unknown systematic for the given collection
    /// \pre unsigned (type) < EnumSize<ObjectType>::dataSize
  public:
    const std::string&
    getSourceName (ObjectType type, const CP::SystematicSet& sys) const;

    /// \brief set the name returned by \ref getName
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II
    /// \pre unsigned (type) < EnumSize<ObjectType>::dataSize
    /// \pre !val_name.empty()
  public:
    void
    setSourceName (ObjectType type, const CP::SystematicSet& sys,
		   const std::string& val_sourceName);

    /// \brief get the list of all unique source names for one object type
    /// \pre unsigned (type) < EnumSize<ObjectType>::dataSize
  public:
    std::vector<std::string>
    getSourceNames (ObjectType type) const;



    //
    // private interface
    //

    /// \brief the value of \ref getObjectTypes
  private:
    std::set<ObjectType> m_objectTypes;

    /// \brief the value of \ref getSystematics
  private:
    std::set<CP::SystematicSet> m_systematics;

    /// \brief the value of \ref getName
  private:
    std::unordered_map<CP::SystematicSet,std::string> m_names [EnumSize<ObjectType>::dataSize];

    /// \brief the value of \ref getSourceName
  private:
    std::unordered_map<CP::SystematicSet,std::string> m_sourceNames [EnumSize<ObjectType>::dataSize];
  };
}

#endif
