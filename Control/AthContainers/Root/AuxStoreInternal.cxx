/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/src/AuxStoreInternal.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2013
 * @brief An auxiliary data store that holds data internally.
 */


#include <iostream>
#include <sstream>

#include "AthContainers/AuxStoreInternal.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/PackedParameters.h"
#include "AthContainers/exceptions.h"
#include "AthContainers/tools/error.h"


namespace SG {


/**
 * @brief Constructor.
 * @param standalone If true, then write this in standalone mode.
 */
AuxStoreInternal::AuxStoreInternal (bool standalone /*= false*/)
  : m_standalone (standalone),
    m_locked (false)
{
}


/**
 * @brief Destructor.
 *
 * All contained data will be deleted.
 */
AuxStoreInternal::~AuxStoreInternal()
{
}


/**
 * @brief Copy constructor.
 */
AuxStoreInternal::AuxStoreInternal (const AuxStoreInternal& other)
  : m_standalone (other.m_standalone),
    m_decorations (other.m_decorations),
    m_auxids (other.m_auxids),
    m_locked (other.m_locked)
{
  size_t size = other.m_vecs.size();
  m_vecs.resize (size);
  for (size_t i = 0; i < size; i++) {
    if (other.m_vecs[i])
      m_vecs[i] = other.m_vecs[i]->clone();
  }
}


/**
 * @brief Return the standalone flag.
 */
bool AuxStoreInternal::standalone() const
{
  return m_standalone;
}


/**
 * @brief Return the data vector for one aux data item
 * @param auxid The identifier of the desired aux data item.
 *
 * Each aux data item is stored as a vector, with one entry
 * per entry in the owning container.  This returns a pointer
 * to the start of the vector.
 *
 * This should return 0 if the item doesn't exist.
 */
const void* AuxStoreInternal::getData (auxid_t auxid) const
{
  guard_t guard (m_mutex);
  if (auxid >= m_vecs.size() || !m_vecs[auxid]) {
    // With the new behavior of SG::AuxElement::Accessor::isAvailable,
    // we shouldn't print an error message here. Asking the store whether
    // it has an element using this function is not necessarily an
    // error condition by now. In any case, the DataVector code will
    // complain itself in case of an error.
    return 0;
  }
  return m_vecs[auxid]->toPtr();
}


/**
 * @brief Return the data vector for one aux data item
 * @param auxid The identifier of the desired aux data item.
 * @param size The current size of the container (in case the data item
 *             does not already exist).
 * @param capacity The current capacity of the container (in case
 *                 the data item does not already exist).
 *
 * Each aux data item is stored as a vector, with one entry
 * per entry in the owning container.  This returns a pointer
 * to the start of the vector.
 *
 * If the data item does not exist, it should be created and initialized
 * to default values.  @c size and @c capacity give the size for the
 * new aux data item vector.
 */
void* AuxStoreInternal::getData (auxid_t auxid, size_t size, size_t capacity)
{
  return getDataInternal (auxid, size, capacity, false);
}


/**
 * @brief Explicitly add a vector to the store.
 * @param auxid The identifier of the aux data item being added.
 * @param vec Vector data being added.
 * @param isDecoration Should this variable be marked as a decoration?
 *
 * For internal use.  The @c auxid must not already exist in the store.
 */
void
AuxStoreInternal::addVector (auxid_t auxid,
                             std::unique_ptr<IAuxTypeVector> vec,
                             bool isDecoration)
{
  guard_t guard (m_mutex);
  if (m_locked)
    throw ExcStoreLocked (auxid);

  // Resize the vector if needed.
  if (m_vecs.size() <= auxid) {
    m_vecs.resize (auxid+1);
  }

  // Give up if the variable is already present in the store.
  if (m_vecs[auxid]) std::abort();

  // Make sure the length is consistent with the rest of the store.
  size_t sz = this->size_noLock();
  if (vec->size() < sz)
    vec->resize (sz);

  // Add it to the store.
  m_vecs[auxid] = std::move (vec);
  addAuxID (auxid);
  if (isDecoration) {
    m_decorations.insert (auxid);
  }
}


/**
 * @brief Return the data vector for one aux data decoration item.
 * @param auxid The identifier of the desired aux data item.
 * @param size The current size of the container (in case the data item
 *             does not already exist).
 * @param capacity The current capacity of the container (in case
 *                 the data item does not already exist).
 *
 * Each aux data item is stored as a vector, with one entry
 * per entry in the owning container.  This returns a pointer
 * to the start of the vector.
 *
 * If the data item does not exist, it then it will be created and initialized
 * with default values.  If the container is locked, then the new
 * item will be marked as a decoration.  @c size and @c capacity give
 * the size for the new aux data item vector.
 *
 * If the data item already exists, then we return it if either the
 * container is not locked or the item is marked as a decoration.
 * Otherwise we throw an exception.
 */
void*
AuxStoreInternal::getDecoration (auxid_t auxid, size_t size, size_t capacity)
{
  guard_t guard (m_mutex);
  if (m_vecs.size() <= auxid) {
    m_vecs.resize (auxid+1);
  }
  if (m_vecs[auxid] == 0) {
    m_vecs[auxid] = AuxTypeRegistry::instance().makeVector (auxid, size, capacity);
    addAuxID (auxid);
    if (m_locked) {
      m_decorations.insert (auxid);
    }
  }
  if (m_locked && !m_decorations.test (auxid)) {
    throw ExcStoreLocked (auxid);
  }
  return m_vecs[auxid]->toPtr();
}


/**
 * @brief Change the size of all aux data vectors.
 * @param sz The new size.
 *
 * This should be called when the size of the container changes.
 * This should resize the vectors for all aux data items.
 *
 * If the size of the container grows, the new elements should
 * be default-initialized; if it shrinks, destructors should
 * be run as appropriate.
 *
 * Should return @c true if it is known that none of the data pointers
 * changed (and thus the cache does not need to be cleared), false
 * otherwise.
 */
bool AuxStoreInternal::resize (size_t sz)
{
  guard_t guard (m_mutex);
  if (m_locked)
    throw ExcStoreLocked ("resize");
  bool nomoves = true;
  for (std::unique_ptr<IAuxTypeVector>& v : m_vecs) {
    if (v) {
      if (!v->resize (sz))
        nomoves = false;
    }
  }
  return nomoves;
}


/**
 * @brief Change the capacity of all aux data vectors.
 * @param sz The new capacity.
 *
 * This should be called when the capacity of the container changes
 * (by @c reserve).  This should change the capacity for the vectors
 * for all aux data items.
 */
void AuxStoreInternal::reserve (size_t sz)
{
  guard_t guard (m_mutex);
  if (m_locked)
    throw ExcStoreLocked ("reserve");
  for (std::unique_ptr<IAuxTypeVector>& v : m_vecs) {
    if (v)
      v->reserve (sz);
  }
}


/**
 * @brief Shift the elements of the container.
 * @param pos The starting index for the shift.
 * @param offs The (signed) amount of the shift.
 *
 * This operation shifts the elements in the vectors for all
 * aux data items, to implement an insertion or deletion.
 * @c offs may be either positive or negative.
 *
 * If @c offs is positive, then the container is growing.
 * The container size should be increased by @c offs,
 * the element at @c pos moved to @c pos + @c offs,
 * and similarly for following elements.
 * The elements between @c pos and @c pos + @c offs should
 * be default-initialized.
 *
 * If @c offs is negative, then the container is shrinking.
 * The element at @c pos should be moved to @c pos + @c offs,
 * and similarly for following elements.
 * The container should then be shrunk by @c -offs elements
 * (running destructors as appropriate).
 */
void AuxStoreInternal::shift (size_t pos, ptrdiff_t offs)
{
  guard_t guard (m_mutex);
  if (m_locked)
    throw ExcStoreLocked ("shift");
  for (std::unique_ptr<IAuxTypeVector>& v : m_vecs) {
    if (v)
      v->shift (pos, offs);
  }
}


/**
 * @brief Move all elements from @c other to this store.
 * @param pos The starting index of the insertion.
 * @param other Store from which to do the move.
 * @param ignore Set of variables that should not be added to the store.
 *
 * Let @c len be the size of @c other.  The store will be increased
 * in size by @c len elements, with the elements at @c pos being
 * copied to @c pos+len.  Then, for each auxiliary variable, the
 * entire contents of that variable for @c other will be moved to
 * this store at index @c pos.  This will be done via move semantics
 * if possible; otherwise, it will be done with a copy.  Variables
 * present in this store but not in @c other will have the corresponding
 * elements default-initialized.  Variables in @c other but not in this
 * store will be added unless they are in @c ignore.
 *
 * Returns true if it is known that none of the vectors' memory moved,
 * false otherwise.
 */
bool AuxStoreInternal::insertMove (size_t pos,
                                   IAuxStore& other,
                                   const SG::auxid_set_t& ignore)
{
  guard_t guard (m_mutex);
  const AuxTypeRegistry& r = AuxTypeRegistry::instance();

  if (m_locked)
    throw ExcStoreLocked ("insertMove");
  bool nomove = true;
  size_t other_size = other.size();
  if (other_size == 0)
    return true;
  for (SG::auxid_t id : m_auxids) {
    SG::IAuxTypeVector* v_dst = nullptr;
    if (id < m_vecs.size())
      v_dst = m_vecs[id].get();
    if (v_dst) {
      if (other.getData (id)) {
        void* src_ptr = other.getData (id, other_size, other_size);
        if (src_ptr) {
          if (!v_dst->insertMove (pos, src_ptr, reinterpret_cast<char*>(src_ptr) + other_size*r.getEltSize(id)))
            nomove = false;
        }
      }
      else {
        const void* orig = v_dst->toPtr();
        v_dst->shift (pos, other_size);
        if (orig != v_dst->toPtr())
          nomove = false;
      }
    }
  }

  // Add any new variables not present in the original container.
  for (SG::auxid_t id : other.getAuxIDs()) {
    if (!m_auxids.test(id) && !ignore.test(id))
    {
      if (other.getData (id)) {
        void* src_ptr = other.getData (id, other_size, other_size);
        if (src_ptr) {
          size_t sz = size_noLock();
          if (sz < other_size) sz = other_size + pos;
          (void)getDataInternal_noLock (id, sz, sz, false);
          m_vecs[id]->resize (sz - other_size);
          m_vecs[id]->insertMove (pos, src_ptr, reinterpret_cast<char*>(src_ptr) + other_size*r.getEltSize(id));
          nomove = false;
        }
      }
    }
  }
  
  return nomove;
}


/**
 * @brief Return a set of identifiers for existing data items
 *        in this store.
 *
 *        This should include identifiers for all items,
 *        const and non-const.
 */
const SG::auxid_set_t&
AuxStoreInternal::getAuxIDs() const
{
  return m_auxids;
}


/**
 * @brief Test if a particular variable is tagged as a decoration.
 * @param auxid The identifier of the desired aux data item.
 */
bool AuxStoreInternal::isDecoration (auxid_t auxid) const
{
  return m_decorations.test (auxid);
}


/**
 * @brief Return a set of identifiers for writable data items
 *        in this store.
 *
 *        This should include only non-const identifiers.
 */
const SG::auxid_set_t&
AuxStoreInternal::getWritableAuxIDs() const
{
  return getAuxIDs();
}


/**
 * @brief Return a pointer to the data to be stored for one aux data item.
 * @param auxid The identifier of the desired aux data item.
 * @param quiet If true, then don't print an error on failure.
 *
 * This will usually be a pointer to a @c std::vector; however, it may
 * be something different for a standalone object.
 *
 * Returns 0 and reports an error if the requested aux data item
 * does not exist.
 */
const void* AuxStoreInternal::getIODataInternal (auxid_t auxid, bool quiet) const
{
  guard_t guard (m_mutex);
  if (auxid >= m_vecs.size() || !m_vecs[auxid]) {
    if (!quiet) {
      std::ostringstream ss;
      ss  << "Requested variable "
          << SG::AuxTypeRegistry::instance().getName (auxid)
          << " (" << auxid << ") doesn't exist";
      ATHCONTAINERS_ERROR("AuxStoreInternal::getIODataInternal", ss.str());
    }
    return 0;
  }

  if (m_standalone)
    return m_vecs[auxid]->toPtr();
  return m_vecs[auxid]->toVector();
}


/**
 * @brief Return a pointer to the data to be stored for one aux data item.
 * @param auxid The identifier of the desired aux data item.
 * @param quiet If true, then don't print an error on failure.
 *
 * This will usually be a pointer to a @c std::vector; however, it may
 * be something different for a standalone object.
 *
 * Returns 0 and reports an error if the requested aux data item
 * does not exist.
 */
void* AuxStoreInternal::getIODataInternal (auxid_t auxid, bool quiet)
{
  guard_t guard (m_mutex);
  if (auxid >= m_vecs.size() || !m_vecs[auxid]) {
    if (!quiet) {
      std::ostringstream ss;
      ss  << "Requested variable "
          << SG::AuxTypeRegistry::instance().getName (auxid)
          << " (" << auxid << ") doesn't exist";
      ATHCONTAINERS_ERROR("AuxStoreInternal::getIODataInternal", ss.str());
    }
    return 0;
  }

  if (m_standalone)
    return m_vecs[auxid]->toPtr();
  return m_vecs[auxid]->toVector();
}


/**
 * @brief Return a pointer to the data to be stored for one aux data item.
 * @param auxid The identifier of the desired aux data item.
 *
 * This will usually be a pointer to a @c std::vector; however, it may
 * be something different for a standalone object.
 *
 * Returns 0 and reports an error if the requested aux data item
 * does not exist.
 */
const void* AuxStoreInternal::getIOData (auxid_t auxid) const
{
  return getIODataInternal (auxid, false);
}


/**
 * @brief Return the type of the data to be stored for one aux data item.
 * @param auxid The identifier of the desired aux data item.
 *
 * For an aux data item of type @c T, this will usually be
 * @c std::vector<T>.  For standalone objects, however, it will
 * usually be @c T; and @c std::vector<char> will be used instead
 * of @c std::vector<bool>.
 *
 * Returns 0 if the requested aux data item does not exist.
 */
const std::type_info* AuxStoreInternal::getIOType (auxid_t auxid) const
{
  if (m_standalone)
    return SG::AuxTypeRegistry::instance().getType (auxid);
  guard_t guard (m_mutex);
  if (auxid < m_vecs.size() && m_vecs[auxid]) {
    const std::type_info* ret = m_vecs[auxid]->objType();
    if (ret) return ret;
  }
  return SG::AuxTypeRegistry::instance().getVecType (auxid);
}


/**
 * @brief Get the list of all variables that need to be handled.
 */
const SG::auxid_set_t&
AuxStoreInternal::getDynamicAuxIDs() const
{
  return getAuxIDs();
}


/**
 * @brief Lock the container.
 *
 * After this, only decorations can be changed/modified.
 * If the container is already locked, this is a no-op.
 */
void AuxStoreInternal::lock()
{
  guard_t guard (m_mutex);
  m_locked = true;
}


/**
 * @brief Clear all decorations.
 *
 * Erase all decorations from the store, restoring the state to when
 * @c lock was called.  Be sure to clear the cache of the referencing
 * container!
 *
 * Returns true if there were any decorations that were cleared,
 * false if the store did not contain any decorations.
 */
bool AuxStoreInternal::clearDecorations()
{
  guard_t guard (m_mutex);
  bool anycleared = false;
  for (auxid_t id : m_decorations) {
    m_vecs[id].reset();
    m_auxids.erase (id);
    anycleared = true;
  }
  if (anycleared) {
    m_decorations.clear();
  }
  return anycleared;
}


/**
 * @brief Return the number of elements in the store.
 *
 * May return 0 for a store with no aux data.
 */
size_t AuxStoreInternal::size() const
{
  guard_t guard (m_mutex);
  return size_noLock();
}


/**
 * @brief Return the number of elements in the store.  (No locking.)
 *
 * May return 0 for a store with no aux data.
 */
size_t AuxStoreInternal::size_noLock() const
{
  for (SG::auxid_t id : m_auxids) {
    if (id < m_vecs.size() && m_vecs[id] && m_vecs[id]->size() > 0)
      return m_vecs[id]->size();
  }
  return 0;
}


/**
 * @brief Set an option for an auxiliary data variable.
 * @param id The variable for which we want to set the option.
 * @param option The option setting to make.
 *
 * The interpretation of @c option depends on the associated auxiliary store.
 * See PackedParameters.h for option settings for writing packed data.
 * Returns @c true on success, @c false otherwise.
 */
bool AuxStoreInternal::setOption (auxid_t id, const AuxDataOption& option)
{
  // Does this variable exist in this store?
  bool exists = false;
  {
    guard_t guard (m_mutex);
    if (id < m_vecs.size() && m_vecs[id] != 0)
      exists = true;
  }

  // If not, and we have a packing parameter request, then create the variable.
  if (!exists) {
    if (!PackedParameters::isValidOption (option)) return false;
    size_t sz = size();
    getDataInternal (id, sz, sz, true);
  }

  // Try the option setting.
  guard_t guard (m_mutex);
  if (m_vecs[id]->setOption (option)) return true;

  // It didn't work.  If this is a packing request, then try to convert
  // the variable to packed form and retry.
  if (!PackedParameters::isValidOption (option)) return false;
  std::unique_ptr<IAuxTypeVector> packed = m_vecs[id]->toPacked();
  if (packed) {
    // Converted to packed form.  Replace the object and retry.
    m_vecs[id] = std::move (packed);
    return m_vecs[id]->setOption (option);
  }

  // Didn't work.
  return false;
}


/**
 * @brief Add a new auxid to the set of those being managed by this store.
 * @param auxid The auxid to add.
 */
void AuxStoreInternal::addAuxID (auxid_t auxid)
{
  m_auxids.insert (auxid);
}


/// Implementation of getDataInternal; no locking.
void* AuxStoreInternal::getDataInternal_noLock (auxid_t auxid,
                                                size_t size,
                                                size_t capacity,
                                                bool no_lock_check)
{
  if (m_vecs.size() <= auxid) {
    m_vecs.resize (auxid+1);
  }
  if (m_vecs[auxid] == 0) {
    if (m_locked && !no_lock_check)
      throw ExcStoreLocked (auxid);
    m_vecs[auxid] = AuxTypeRegistry::instance().makeVector (auxid, size, capacity);
    addAuxID (auxid);
  }
  else {
    // Make sure the vector has at least the requested size.
    // One way in which it could be short: setOption was called and created
    // a variable in a store that had no other variables.
    if (m_vecs[auxid]->size() < size) {
      m_vecs[auxid]->resize (size);
      m_vecs[auxid]->reserve (capacity);
    }
  }
  return m_vecs[auxid]->toPtr();
}


/**
 * @brief Return the data vector for one aux data item
 * @param auxid The identifier of the desired aux data item.
 * @param size The current size of the container (in case the data item
 *             does not already exist).
 * @param capacity The current capacity of the container (in case
 *                 the data item does not already exist).
 * @param no_lock_check If true, then skip the test for a locked container.
 *
 * Each aux data item is stored as a vector, with one entry
 * per entry in the owning container.  This returns a pointer
 * to the start of the vector.
 *
 * If the data item does not exist, it should be created and initialized
 * to default values.  @c size and @c capacity give the size for the
 * new aux data item vector.
 */
void* AuxStoreInternal::getDataInternal (auxid_t auxid,
                                         size_t size,
                                         size_t capacity,
                                         bool no_lock_check)
{
  guard_t guard (m_mutex);
  return getDataInternal_noLock (auxid, size, capacity, no_lock_check);
}


/**
 * @brief Lock a decoration.
 * @param auxid Identifier of the decoration to lock.
 *
 * A decoration is locked by changing from a decoration to an ordinary
 * variable.  If the container itself is locked, then modifications
 * to the variable are not permitted after this call.
 */
void AuxStoreInternal::lockDecoration (SG::auxid_t auxid)
{
  guard_t guard (m_mutex);
  m_decorations.reset (auxid);
}


} // namespace SG
