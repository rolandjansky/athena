/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/Root/AuxElement.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2013
 * @brief Base class for elements of a container that can have aux data.
 */


#include "AthContainers/AuxElement.h"
#include "AthContainers/AuxStoreStandalone.h"
#include "AthContainers/exceptions.h"



namespace SG {


/**
 * @brief Internal data container.
 *
 * If an object has a private or standalone data store,
 * its container is set to point to an instance of this.
 */
class AuxElementData
  : public AuxVectorData
{
public:
  virtual size_t size_v() const { return 1; }
  virtual size_t capacity_v() const { return 1; }
};


/**
 * @brief Internal data container for private store.
 *
 * This is used when we have a private aux store.
 * It contains an embedded store object.
 */
class AuxElementPrivateData
  : public AuxElementData
{
public:
  AuxElementPrivateData ()
  {
    this->setStore (&m_store);
  }


private:
  AuxStoreStandalone m_store;
};


/**
 * @brief Internal data container for standalone store.
 *
 * This is used when we have an external (`standalone') aux store.
 */
class AuxElementStandaloneData
  : public AuxElementData
{
public:
  using AuxElementData::setStore;
};


//************************************************************************


/// Special value used to mark that an object had a private store,
/// but it was released because it was added to a container.
/// (And therefore we should recreate the private store if the
/// object is later removed.)
SG::AuxElementData* const AuxElement::s_privatePlaceholder =
  reinterpret_cast<SG::AuxElementData*>(1);


/**
 * @brief Create a new (empty) private store for this object.
 *
 * @c ExcBadPrivateStore will be thrown if this object is already
 * associated with a store.
 */
void AuxElement::makePrivateStore()
{
  if (m_privateData || m_container) {
    throw SG::ExcBadPrivateStore ("store already exists");
  }

  m_privateData = new SG::AuxElementPrivateData;
  m_index = 0;
  m_container = m_privateData;
}


/**
 * @brief Release and free any private store associated with this object.
 *
 * @c ExcBadPrivateStore will be thrown if this object does not
 * have a private store.
 */
void AuxElement::releasePrivateStore()
{
  if (m_privateData == s_privatePlaceholder) {
    // We had a private store, but it was released because this object
    // was added to a container.  Just forget about it.
    m_privateData = 0;
    return;
  }

  if (!m_privateData ||
      (m_container && m_container != m_privateData) ||
      typeid(*m_privateData) != typeid(AuxElementPrivateData))
  {
    throw SG::ExcBadPrivateStore ("no private store exists");
  }

  m_index = 0;
  m_container = 0;
  delete m_privateData;
  m_privateData = 0;
}


/**
 * @brief Set the store associated with this object.
 * @param store The new store.
 *
 * If store is nonzero, this adds a standalone store to the object.
 * The object must not be in a container and must not have a private store.
 * If store is zero, this removes a standalone store.
 */
void AuxElement::setStore (const SG::IConstAuxStore* store)
{
  AuxElementStandaloneData* data = setStore1 (store);
  if (store)
    data->setStore (store);
}


/**
 * @brief Set the store associated with this object.
 * @param store The new store.
 *
 * If store is nonzero, this adds a standalone store to the object.
 * The object must not be in a container and must not have a private store.
 * If store is zero, this removes a standalone store.
 */
void AuxElement::setStore (SG::IAuxStore* store)
{
  AuxElementStandaloneData* data = setStore1 (store);
  if (store)
    data->setStore (store);
}


/**
 * @brief Set the store associated with this object.
 * @param store Link to the new store.
 */
void AuxElement::setStore (const DataLink< SG::IConstAuxStore >& store)
{
  AuxElementStandaloneData* data = setStore1 (store);
  if (store)
    data->setStore (store);
}


/**
 * @brief Test to see if this object is currently using a private store.
 */
bool AuxElement::usingPrivateStore() const
{
  return privateDataValid() && 
    typeid(*m_privateData) == typeid(AuxElementPrivateData);
}


/**
 * @brief Test to see if this object is currently using a private store.
 */
bool AuxElement::usingStandaloneStore() const
{
  return privateDataValid() &&
    typeid(*m_privateData) == typeid(AuxElementStandaloneData);
}


/**
 * @brief Return the current store, as a const interface.
 *
 * This will be non-zero if either a const or non-const store
 * is associated with this object.
 * This will fetch either a private or standalone store.
 */
const SG::IConstAuxStore* AuxElement::getConstStore() const
{
  if (privateDataValid())
    return m_privateData->getConstStore();
  return 0;
}


/**
 * @brief Return the current store, as a non-const interface.
 *
 * This will be non-zero if a non-const store is associated with this object.
 * This will fetch either a private or standalone store.
 */
SG::IAuxStore* AuxElement::getStore() const
{
  if (privateDataValid())
    return m_privateData->getStore();
  return 0;
}


/**
 * @brief Clear the cached aux data pointers.
 *
 * You should call this any time something changes in the aux store
 * that could invalidate the vector pointers.
 */
void AuxElement::clearCache()
{
  if (m_container)
    m_container->clearCache();
}


/**
 * @brief Return a set of identifiers for existing data items
 *        for this object.
 *
 *        If this object has a private or standalone store, then information
 *        from that will be returned.  Otherwise, if this element
 *        is part of a container, then information for the container
 *        will be returned.  Otherwise, return an empty set.
 */
const SG::auxid_set_t& AuxElement::getAuxIDs() const
{
  if (privateDataValid())
    return m_privateData->getConstStore()->getAuxIDs();
  if (container())
    return container()->getAuxIDs();
  static const SG::auxid_set_t null_set;
  return null_set;
}


/**
 * @brief Return true if this object has an associated store.
 *
 * This will be true for either a private or standalone store.
 */
bool AuxElement::hasStore() const
{
  if (privateDataValid())
    return m_privateData->hasStore();
  return false;
}


/**
 * @brief Return true if this object has an associated non-const store.
 *
 * This will be true for either a private or standalone store.
 */
bool AuxElement::hasNonConstStore() const
{
  if (privateDataValid())
    return m_privateData->hasNonConstStore();
  return false;
}


/**
 * @brief Clear all decorations.
 *
 * Erase all decorations from an associated store, restoring the state to when
 * @c lock was called.
 *
 * Returns true if there were any decorations that were cleared,
 * false if the store did not contain any decorations.
 */
bool AuxElement::clearDecorations() const
{
  if (privateDataValid())
    return m_privateData->clearDecorations();
  return false;
}


/**
 * @brief Out-of-line portion of destructor.
 *
 * Delete a private store if we have one.
 */
void AuxElement::releasePrivateStoreForDtor()
{
  if (m_privateData != s_privatePlaceholder)
  {
    delete m_privateData;
  }
}


/**
 * @brief Set the store associated with this object.
 * @param store The new store.
 *
 * Helper for @c setStore.  Creates the @x AuxElementStandaloneData
 * object if needed and returns it.
 */
AuxElementStandaloneData*
AuxElement::setStore1 (const SG::IConstAuxStore* store)
{
  if (store) {
    // Want this object be standalone.
    if (!m_container) {
      // Not in a container (and no private store).  Make a new object.
      AuxElementStandaloneData* data = new AuxElementStandaloneData;
      m_privateData = data;
      m_container = data;
      return data;
    }
    if (usingStandaloneStore()) {
      // Standalone --- return existing object.
      return static_cast<AuxElementStandaloneData*> (m_privateData);
    }
    // Otherwise, it's an error.
    throw ExcBadPrivateStore ("Attempt to attach a standalone store to an "
                              "object in a container or with a private store.");
  }

  else {
    // Getting rid of a standalone store.
    if (usingStandaloneStore()) {
      delete m_privateData;
      m_privateData = 0;
      m_container = 0;
    }
    else if (m_container != 0)
      throw ExcBadPrivateStore ("Attempt to remove a standalone store from an "
                                "object in a container or with a private store.");
    return 0;
  }
}


/**
 * @brief Set the index/container for this element.
 * @param index The index of this object within the container.
 * @param container The container holding this object.
 *                  May be null if this object is being removed
 *                  from a container.
 *
 * This is called from @c setIndex when we have a private store to deal with.
 */
bool AuxElement::setIndexPrivate (size_t index, SG::AuxVectorData* container)
{
  // Precondition: m_privateData != 0.

  if (m_privateData == s_privatePlaceholder) {
    // We had a private store, but it was released because we were added
    // to a container.

    if (container == 0) {
      // We're being moved out of the container.  Make a new private
      // store, copy the data, and switch to it.
      m_privateData = new SG::AuxElementPrivateData;
      AuxElement to (m_privateData, 0);
      to.copyAux (*this);
      m_index = 0;
      m_container = m_privateData;
      return true;
    }
  }
  else if (typeid(*m_privateData) == typeid(AuxElementPrivateData)) {
    // We currently have a private store.

    if (container != 0 && container != m_privateData) {
      // We're being added to a container.
      // Aux data has already been copied.
      // Release private store.
      delete m_privateData;
      m_privateData = s_privatePlaceholder;
    }
  }
  else {
    // We have a standalone store.
    throw SG::ExcBadPrivateStore ("Attempt to add/remove a standalone object "
                                  "from a container.");
  }

  m_index = index;
  m_container = container;
  return false;
}


/**
 * @brief Create a new private store for this object and copy aux data.
 * @param other The object from which aux data should be copied.
 *
 * @c ExcBadPrivateStore will be thrown if this object is already
 * associated with a store.
 *
 * This overload handles the case where @c other does have aux data.
 */
void AuxElement::makePrivateStore1 (const AuxElement* other)
{
  makePrivateStore();
  if (other)
    this->copyAux (*other);
}


/**
 * @brief Clear all aux data associated with this element.
 *
 * If this object has no associated store, this does nothing.
 * If the associated aux data is const, this throws @c ExcConstAuxData.
 */
void AuxElement::clearAux()
{
  if (!m_container) return;
  if (!m_container->hasStore()) return;
  if (!m_container->hasNonConstStore())
    throw SG::ExcConstAuxData ("clearAux", SG::null_auxid);

  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  SG::AuxTypeRegistry::lock_t lock (r);
  for (SG::auxid_t auxid : m_container->getWritableAuxIDs()) {
    void* dst = m_container->getDataArray (auxid);
    r.clear (lock, auxid, dst, m_index);
  }
}


/**
 * @brief Copy aux data from another object.
 * @param other The object from which to copy.
 *
 * If this object has no associated store, this does nothing.
 * If the associated aux data is const, this throws @c ExcConstAuxData.
 *
 * All aux data items from @c other are copied to this object.
 * Any aux data items associated with this object that are not present
 * in @c other are cleared.  (If @c other has no aux data, then all
 * aux data items for this object are cleared.)
 */
void AuxElement::copyAux (const AuxElement& other)
{
  if (!m_container) return;
  if (!m_container->hasStore()) return;
  if (!m_container->hasNonConstStore())
    throw SG::ExcConstAuxData ("copyAux");

  const SG::AuxVectorData* ocont = other.container();

  if (!ocont || !ocont->hasStore()) {
    this->clearAux();
    return;
  }

  size_t oindex = other.index();
  const SG::auxid_set_t& other_ids = ocont->getAuxIDs();

  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();

  for (SG::auxid_t auxid : other_ids) {
    const void* src = ocont->getDataArrayAllowMissing (auxid);
    if (src) {
      void* dst = m_container->getDataArray (auxid);
      r.copy (auxid, dst, m_index, src, oindex);
    }
    else {
      void* dst = m_container->getDataArray (auxid);
      r.clear (auxid, dst, m_index);
    }
  }

  for (SG::auxid_t auxid : m_container->getWritableAuxIDs()) {
    if (other_ids.find (auxid) == other_ids.end()) {
      void* dst = m_container->getDataArray (auxid);
      r.clear (auxid, dst, m_index);
    }
  }
}


} // namespace SG


