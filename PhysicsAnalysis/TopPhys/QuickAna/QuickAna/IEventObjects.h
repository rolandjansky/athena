/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__I_EVENT_OBJECTS_H
#define QUICK_ANA__I_EVENT_OBJECTS_H

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.



#include <QuickAna/Global.h>

#include <QuickAna/xAODInclude.h>
#include <QuickAna/xAODEventSelect.h>

namespace ana
{
  /// \brief The interface class for accessing the correct event
  /// objects.
  ///
  /// This is in essence a wrapper around StoreGate or TEvent/TStore,
  /// using ObjectType to stand in for the name (and also implying the
  /// type).

  class IEventObjects
  {
    //
    // public interface
    //

    /// \brief standard virtual destructor for base class
    /// \par Guarantee
    ///   no-fail
  public:
    virtual ~IEventObjects () = default;


    /// \brief the object or object container for the given object
    ///   type or NULL if it hasn't been filled (yet).
    /// \par Guarantee
    ///   no-fail
  public:
    template<class T> T *get (ObjectType type) const;


    /// \brief the object container as a \ref xAOD::IParticleContainer
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   object type not of IParticle type
  public:
    xAOD::IParticleContainer *getParticles (ObjectType type);


    /// \brief get the object or object container for the given object
    ///   type or NULL if it hasn't been filled (yet).
    /// \par Guarantee
    ///   no-fail
    /// \warn this has to be cast exactly to the type defined by \ref
    ///   ObjecTypeInfo::asXAOD.  it is much easier to go with the
    ///   \ref get function instead.
  public:
    virtual void *getVoid (ObjectType type) const = 0;


    /// \brief add a shallow copy of the input container with the
    /// given name
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   object already initialized\n
    ///   input object not found\n
    ///   shallow copy failures
  public:
    virtual StatusCode addCopy (ObjectType type, const std::string& name) = 0;


    /// \brief add a new container of the given type
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   object already initialized\n
    ///   object creation failures
  public:
    virtual StatusCode addNew (ObjectType type) = 0;


    /// \brief the event weight
    ///
    /// \warn this is a dummy for now and the mechanism might change
    ///   at some point
  public:
    virtual float eventWeight () const = 0;


    /// \brief the eventinfo object
    /// \par Guarantee
    ///   no-fail
  public:
    xAOD::EventInfo *eventinfo () const;


    /// \brief the eventinfo object
    /// \par Guarantee
    ///   no-fail
  public:
    EventSelectType *eventSelect () const;


    /// \brief the muon container
    /// \par Guarantee
    ///   no-fail
  public:
    xAOD::MuonContainer *muons () const;


    /// \brief the tau container
    /// \par Guarantee
    ///   no-fail
  public:
    xAOD::TauJetContainer *taus () const;


    /// \brief the electron container
    /// \par Guarantee
    ///   no-fail
  public:
    xAOD::ElectronContainer *electrons () const;


    /// \brief the photon container
    /// \par Guarantee
    ///   no-fail
  public:
    xAOD::PhotonContainer *photons () const;


    /// \brief the jet container
    /// \par Guarantee
    ///   no-fail
  public:
    xAOD::JetContainer *jets () const;


    /// \brief the fat jet container
    /// \par Guarantee
    ///   no-fail
  public:
    xAOD::JetContainer *fat_jets () const;


    /// \brief the met container
    /// \par Guarantee
    ///   no-fail
  public:
    xAOD::MissingETContainer *met () const;


    /// \brief the met container
    /// \par Guarantee
    ///   no-fail
  public:
    xAOD::MissingETContainer *met2 () const;
  };
}

#include <QuickAna/IEventObjects.icc>

#endif
