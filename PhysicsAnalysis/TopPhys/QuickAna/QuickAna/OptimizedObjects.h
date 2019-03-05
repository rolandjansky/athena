/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__OPTIMIZER_OBJECTS_H
#define QUICK_ANA__OPTIMIZER_OBJECTS_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

#include <QuickAna/Global.h>

#include <PATInterfaces/SystematicSet.h>
#include <QuickAna/IEventObjects.h>

namespace ana
{

  /// An EventObjects container for specific systematic and object
  /// types at a specific point in the processing chain.
  ///
  /// Used in the optimized tool scheduler.
  ///
  class OptimizedObjects : public IEventObjects
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
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    OptimizedObjects (OptimizedStore *val_master,
		      const CP::SystematicSet& val_systematics);


    /// \brief do everything we need to do to prepare for a new event,
    ///   i.e. make all the shallow copies
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   xAOD copy failures
  public:
    StatusCode prepare () const;


    /// \brief check whether we currently pass the selection
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   xAOD access error
  public:
    bool passSelection () const;



    //
    // inherited interface
    //

    /// \brief get the object or object container for the given object
    ///   type or NULL if it hasn't been filled (yet).
    /// \par Guarantee
    ///   no-fail
    /// \warn this has to be cast exactly to the type defined by \ref
    ///   ObjecTypeInfo::asXAOD.  it is much easier to go with the
    ///   \ref get function instead.
  public:
    virtual void *getVoid (ObjectType type) const override;


    /// \brief add a shallow copy of the input container with the
    /// given name
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   object already initialized\n
    ///   input object not found\n
    ///   shallow copy failures
  public:
    virtual StatusCode addCopy (ObjectType type, const std::string& name) override;


    /// \brief add a new container of the given type
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   object already initialized\n
    ///   object creation failures
  public:
    virtual StatusCode addNew (ObjectType type) override;


    /// \brief the event weight
    ///
    /// \warn this is a dummy for now and the mechanism might change
    ///   at some point
  public:
    virtual float eventWeight () const override;


    /// \brief the systematics for these objects
    /// \par Guarantee
    ///   no-fail
  public:
    const CP::SystematicSet& systematics () const;


    /// \brief fill the given \ref EventData structure for the given
    /// systematic
    /// \par Guarantee
    ///    basic
    /// \par Failures
    ///    out of memory II
  public:
    void fillEventData (const CP::SystematicSet& sys,
			EventData& eventData) const;



    //
    // private interface
    //

    /// \brief the master store object
  private:
    OptimizedStore *m_master;

    /// \brief the index for the given object type
  private:
    std::size_t m_index [ObjectTypeInfo::numTypes];

    /// \brief the input index for the given object type
  private:
    std::size_t m_inputIndex [ObjectTypeInfo::numTypes];

    /// \brief the value of \ref systematics
  private:
    CP::SystematicSet m_systematics;

    friend class OptimizedStore;
  };
}

#endif
