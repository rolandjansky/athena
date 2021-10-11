/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthContainers/Root/AuxElement.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2013
 * @brief Base class for elements of a container that can have aux data.
 */


#include "AthContainers/AuxElement.h"
#include "AthContainers/AuxStoreStandalone.h"
#include "AthContainers/exceptions.h"
#include "CxxUtils/checker_macros.h"



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


/**
 * @brief Out-of-line portion of destructor.
 *
 * Delete a private store if we have one.
 */
void ConstAuxElement::releasePrivateStoreForDtor()
{
  if (havePrivateData()) {
    delete m_container;
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
bool ConstAuxElement::setIndexPrivate (size_t index,
                                       const SG::AuxVectorData* container)
{
  if (hadPrivateData()) {
    // We had a private store, but it was released because we were added
    // to a container.

    if (container == 0) {
      // We're being moved out of the container.  Make a new private
      // store, copy the data, and switch to it.
      auto privateData = new SG::AuxElementPrivateData;
      AuxElement to (privateData, 0);
      to.copyAux (*this);
      IAuxElement::setIndex (0);
      IAuxElement::setHavePrivateData();
      m_container = privateData;
      return true;
    }
  }
  else if (havePrivateData() &&
           typeid(*m_container) == typeid(AuxElementPrivateData))
  {
    // We currently have a private store.

    if (container != 0 && container != m_container) {
      // We're being added to a container.
      // Aux data has already been copied.
      // Release private store.
      IAuxElement::setIndex (index);
      IAuxElement::setHadPrivateData();
      delete m_container;
      m_container = container;
      return false;
    }
  }
  else {
    // We have a standalone store.
    throw SG::ExcBadPrivateStore ("Attempt to add/remove a standalone object "
                                  "from a container.");
  }

  IAuxElement::setIndex (index);
  m_container = container;
  return false;
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
const SG::auxid_set_t& ConstAuxElement::getAuxIDs() const
{
  if (havePrivateData())
    return m_container->getConstStore()->getAuxIDs();
  if (container())
    return container()->getAuxIDs();
  static const SG::auxid_set_t null_set;
  return null_set;
}


//************************************************************************


/**
 * @brief Create a new (empty) private store for this object.
 *
 * @c ExcBadPrivateStore will be thrown if this object is already
 * associated with a store.
 */
void AuxElement::makePrivateStore()
{
  if (m_container) {
    throw SG::ExcBadPrivateStore ("store already exists");
  }

  m_container = new SG::AuxElementPrivateData;
  IAuxElement::setIndex (0);
  IAuxElement::setHavePrivateData();
}


/**
 * @brief Release and free any private store associated with this object.
 *
 * @c ExcBadPrivateStore will be thrown if this object does not
 * have a private store.
 */
void AuxElement::releasePrivateStore()
{
  if (hadPrivateData()) {
    // We had a private store, but it was released because this object
    // was added to a container.  Just forget about it.
    IAuxElement::setNoPrivateData();
    return;
  }

  if (!havePrivateData() ||
      !m_container ||
      typeid(*m_container) != typeid(AuxElementPrivateData))
  {
    throw SG::ExcBadPrivateStore ("no private store exists");
  }

  IAuxElement::setIndex (0);
  IAuxElement::setNoPrivateData();
  delete m_container;
  m_container = 0;
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
  return havePrivateData() && 
    typeid(*m_container) == typeid(AuxElementPrivateData);
}


/**
 * @brief Test to see if this object is currently using a private store.
 */
bool AuxElement::usingStandaloneStore() const
{
  return havePrivateData() &&
    typeid(*m_container) == typeid(AuxElementStandaloneData);
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
  if (havePrivateData()) {
    return m_container->getConstStore();
  }
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
  if (havePrivateData()) {
#ifdef ATHCONTAINERS_R21_COMPAT
    return m_container->getStore();
#else
    SG::AuxVectorData* container_nc ATLAS_THREAD_SAFE = const_cast<SG::AuxVectorData*>(container());
    return container_nc->getStore();
#endif
  }
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
  if (container())
    container()->clearCache();
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
#ifdef ATHCONTAINERS_R21_COMPAT
  if (havePrivateData())
    return m_container->getConstStore()->getAuxIDs();
  if (container())
    return container()->getAuxIDs();
  static const SG::auxid_set_t null_set;
  return null_set;
#else
  return ConstAuxElement::getAuxIDs();
#endif
}


/**
 * @brief Return true if this object has an associated store.
 *
 * This will be true for either a private or standalone store.
 */
bool AuxElement::hasStore() const
{
  if (havePrivateData())
    return m_container->hasStore();
  return false;
}


/**
 * @brief Return true if this object has an associated non-const store.
 *
 * This will be true for either a private or standalone store.
 */
bool AuxElement::hasNonConstStore() const
{
  if (havePrivateData())
    return m_container->hasNonConstStore();
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
  if (havePrivateData())
    return m_container->clearDecorations();
  return false;
}


/**
 * @brief Out-of-line portion of destructor.
 *
 * Delete a private store if we have one.
 */
void AuxElement::releasePrivateStoreForDtor()
{
#ifdef ATHCONTAINERS_R21_COMPAT
  if (havePrivateData()) {
    delete m_container;
  }
#else
  ConstAuxElement::releasePrivateStoreForDtor();
#endif
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
      IAuxElement::setHavePrivateData();
      m_container = data;
      return data;
    }
    if (usingStandaloneStore()) {
      // Standalone --- return existing object.
      return static_cast<AuxElementStandaloneData*> (container());
    }
    // Otherwise, it's an error.
    throw ExcBadPrivateStore ("Attempt to attach a standalone store to an "
                              "object in a container or with a private store.");
  }

  else {
    // Getting rid of a standalone store.
    if (usingStandaloneStore()) {
      IAuxElement::setNoPrivateData();
      delete m_container;
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
#ifdef ATHCONTAINERS_R21_COMPAT
  if (hadPrivateData()) {
    // We had a private store, but it was released because we were added
    // to a container.

    if (container == 0) {
      // We're being moved out of the container.  Make a new private
      // store, copy the data, and switch to it.
      auto privateData = new SG::AuxElementPrivateData;
      AuxElement to (privateData, 0);
      to.copyAux (*this);
      IAuxElement::setIndex (0);
      IAuxElement::setHavePrivateData();
      m_container = privateData;
      return true;
    }
  }
  else if (havePrivateData() &&
           typeid(*m_container) == typeid(AuxElementPrivateData))
  {
    // We currently have a private store.

    if (container != 0 && container != m_container) {
      // We're being added to a container.
      // Aux data has already been copied.
      // Release private store.
      IAuxElement::setIndex (index);
      IAuxElement::setHadPrivateData();
      delete m_container;
      m_container = container;
      return false;
    }
  }
  else {
    // We have a standalone store.
    throw SG::ExcBadPrivateStore ("Attempt to add/remove a standalone object "
                                  "from a container.");
  }

  IAuxElement::setIndex (index);
  m_container = container;
  return false;
#else
  return ConstAuxElement::setIndexPrivate (index, container);
#endif
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
  for (SG::auxid_t auxid : m_container->getWritableAuxIDs()) {
    void* dst = container()->getDataArray (auxid);
    r.clear (auxid, dst, index());
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
void AuxElement::copyAux (const ConstAuxElement& other)
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
  SG::auxid_set_t other_ids = ocont->getAuxIDs();

  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();

  for (SG::auxid_t auxid : other_ids) {
    const void* src = ocont->getDataArrayAllowMissing (auxid);
    if (src) {
      void* dst = container()->getDataArray (auxid);
      r.copy (auxid, dst, index(), src, oindex);
    }
    else {
      void* dst = container()->getDataArray (auxid);
      r.clear (auxid, dst, index());
    }
  }

  for (SG::auxid_t auxid : m_container->getWritableAuxIDs()) {
    if (!other_ids.test (auxid)) {
      void* dst = container()->getDataArray (auxid);
      r.clear (auxid, dst, index());
    }
  }
}


#ifdef ATHCONTAINERS_R21_COMPAT
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
  SG::auxid_set_t other_ids = ocont->getAuxIDs();

  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();

  for (SG::auxid_t auxid : other_ids) {
    const void* src = ocont->getDataArrayAllowMissing (auxid);
    if (src) {
      void* dst = container()->getDataArray (auxid);
      r.copy (auxid, dst, index(), src, oindex);
    }
    else {
      void* dst = container()->getDataArray (auxid);
      r.clear (auxid, dst, index());
    }
  }

  for (SG::auxid_t auxid : m_container->getWritableAuxIDs()) {
    if (!other_ids.test (auxid)) {
      void* dst = container()->getDataArray (auxid);
      r.clear (auxid, dst, index());
    }
  }
}
#endif


} // namespace SG


