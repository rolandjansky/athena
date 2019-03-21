/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__EVENT_OBJECTS_H
#define QUICK_ANA__EVENT_OBJECTS_H

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
