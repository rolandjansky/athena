/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__EVENT_OBJECTS_H
#define QUICK_ANA__EVENT_OBJECTS_H

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

#include <QuickAna/IEventObjects.h>

namespace ana
{
  class EventObjects : public IEventObjects
  {
    //
    // public interface
    //

    /// \brief standard constructor
    /// \par Guarantee
    ///   no-fail
  public:
    EventObjects ();


    /// \brief clear the content for the next event
    /// \param store the store to use
    /// \par Guarantee
    ///   no-fail
    /// \pre store != nullptr
  public:
    void clear (StoreType *store, const std::string& postfix);



    //
    // inherited interface
    //

    /// \copydoc IEventObjects::getVoid
  public:
    virtual void *getVoid (ObjectType type) const override;

    /// \copydoc IEventObjects::addCopy
  public:
    virtual StatusCode addCopy (ObjectType type, const std::string& name) override;

    /// \copydoc IEventObjects::addNew
  public:
    virtual StatusCode addNew (ObjectType type) override;

    /// \copydoc IEventObjects::eventWeight
  public:
    virtual float eventWeight () const override;



    //
    // private interface
    //

    /// \brief the different objects returned by \ref getVoid
  private:
    void *m_objects [ObjectTypeInfo::numTypes];

    /// \brief the value of \ref eventWeight
  private:
    float m_eventWeight;

    /// \brief the postfix for the current systematic
  private:
    std::string m_postfix;

    /// description: the TStore object we are using
  private:
    StoreType *m_store;
  };
}

#endif
