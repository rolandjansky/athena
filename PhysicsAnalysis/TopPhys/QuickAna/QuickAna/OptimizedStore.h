/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__OPTIMIZER_STORE_H
#define QUICK_ANA__OPTIMIZER_STORE_H

//        Copyright Iowa State University 2015.
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

#include <string>
#include <vector>

namespace ana
{

  /// A wrapper to the event store for the optimized scheduler.
  ///
  /// Manages systematics information and holds pointers to all collections
  /// at all processing stages and systematics.
  ///
  class OptimizedStore
  {
    //
    // public interface
    //

    /// \brief the index to use when we do not use an object type
  public:
    static const std::size_t noIndex = std::size_t (-1);


    // no slicing
    OptimizedStore (const OptimizedStore&) = delete;
    OptimizedStore& operator = (const OptimizedStore&) = delete;


    /// \brief test the invariant of this object
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
    OptimizedStore (const std::vector<CP::SystematicSet>& val_requestedSystematics,
		    StoreType *val_store);


    /// \brief standard destructor
    /// \par Guarantee
    ///   no-fail
    /// \par Rationale
    ///   an explicit destructor to reduce includes in the header file
  public:
    ~OptimizedStore ();


    /// \brief the list of all requested systematics
    /// \par Guarantee
    ///   no-fail
  public:
    const std::vector<CP::SystematicSet>& requestedSystematics () const;


    /// \brief the list of all systematics for the given object type
    ///   and the currently registered tools
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   invalid object type or index
  public:
    const CP::SystematicSet& objectSystematics (ObjectType type) const;


    /// \brief make a preparation copy from the original index to the
    ///   new index
    ///
    /// This is used whenever we create new systematics for an object
    /// type.
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   xAOD copy failures\n
    ///   invalid indices
  public:
    StatusCode
    prepareCopy (ObjectType type, std::size_t index, std::size_t origIndex);


    /// \brief initialize the indices on the given list of systematics
    ///   objects
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II\n
    ///   requesting object type not provided yet\n
    ///   providing new object type already present
  public:
    StatusCode
    initializeIndices (const std::vector<OptimizedObjects*>& objects,
		       unsigned inputTypes, unsigned outputTypes,
		       AnalysisStep step);


    /// \brief clear out all the data in anticipation of the next event
    /// \par Guarantee
    ///   no-fail
  public:
    void clearData ();


    /// \brief do all the final initialization
    /// \par Guarantee
    ///   no-fail
  public:
    StatusCode finalInit ();


    /// \brief the defined object types
    /// \par Guarantee
    ///   no-fail
  public:
    unsigned objectTypes () const;


    /// \brief fill the given \ref EventData structure for the given
    /// systematic
    /// \par Guarantee
    ///    basic
    /// \par Failures
    ///    out of memory II
  public:
    void fillEventData (ObjectType type, unsigned index,
			const CP::SystematicSet& sys,
			EventData& eventData) const;



    //
    // the IEventObjects interface
    //

    /// \copydoc IEventObjects::getVoid
  public:
    void *getVoid (ObjectType type, std::size_t index) const;


    /// \copydoc IEventObjects::addCopy
  public:
    StatusCode
    addCopy (ObjectType type, std::size_t index,
	     const std::string& name);


    /// \copydoc IEventObjects::addNew
  public:
    StatusCode
    addNew (ObjectType type, std::size_t index);



    //
    // private interface
    //

    /// \brief the truly private implementation
  private:
    struct Pimpl;

    /// \brief the \ref Pimpl structure
  private:
    Pimpl *pimpl;

    /// \brief the information per object type
  private:
    struct InfoType;

    /// \brief the information per object
  private:
    struct InfoObject;
  };
}

#endif
