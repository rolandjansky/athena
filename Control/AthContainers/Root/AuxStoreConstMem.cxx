// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthContainers/AuxStoreConstMem.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2021
 * @brief 
 */


#include "AthContainers/AuxStoreConstMem.h"


namespace SG {


/**
 * @brief Constructor.
 * @param size The number of elements in the container.
 */
AuxStoreConstMem::AuxStoreConstMem (size_t size)
  : m_size (size)
{
}


/**
 * @brief Add a new, external buffer.
 * @param auxid The identifier of the aux data item.
 * @param buf Pointer to the start of the buffer for the variable.
 *
 * The actual type of the data pointed to must match the registered
 * type of @c auxid; no checking is done.
 *
 * This method is not compatible with any other accesses to this object
 * in other threads.
 */
void AuxStoreConstMem::addBuffer (SG::auxid_t auxid, const void* buf)
{
  if (m_buffers.size() <= auxid)
    m_buffers.resize (auxid+1);
  m_buffers[auxid] = buf;
  m_auxids.insert (auxid);
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
const void* AuxStoreConstMem::getData (SG::auxid_t auxid) const
{
  if (auxid < m_buffers.size()) {
    return m_buffers[auxid];
  }
  return nullptr;
}


/**
 * @brief Return the data vector for one aux data decoration item.
 * @param auxid The identifier of the desired aux data item.
 * @param size The current size of the container (in case the data item
 *             does not already exist).
 * @param capacity The current capacity of the container (in case
 *                 the data item does not already exist).
 *
 * This always returns null for this (const) class.
 */
void* AuxStoreConstMem::getDecoration (auxid_t /*auxid*/, size_t /*size*/, size_t /*capacity*/)
{
  return nullptr;
}


/**
 * @brief Return a set of identifiers for existing data items
 *        in this store.
 *
 *        This should include identifiers for all items,
 *        const and non-const.
 */
const SG::auxid_set_t& AuxStoreConstMem::getAuxIDs() const
{
  return m_auxids;
}


/**
 * @brief Test if a particular variable is tagged as a decoration.
 * @param auxid The identifier of the desired aux data item.
 *
 * This always returns false for this (const) class.
 */
bool AuxStoreConstMem::isDecoration (auxid_t /*auxid*/) const
{
  return false;
}


/**
 * @brief Lock the container.
 *
 * After this, only decorations can be changed/modified.
 *
 * This is a no-op for this (const) class.
 */
void AuxStoreConstMem::lock()
{
}


/**
 * @brief Clear all decorations.
 *
 * This is a no-op for this (const) class.  Always returns false.
 */
bool AuxStoreConstMem::clearDecorations()
{
  return false;
}


/**
 * @brief Return the number of elements in the store.
 *
 * May return 0 for a store with no aux data.
 */
size_t AuxStoreConstMem::size() const
{
  return m_size;
}


/**
 * @brief Lock a decoration.
 * @param auxid Identifier of the decoration to lock.
 *
 * This is a no-op for this (const) class.
 */
void AuxStoreConstMem::lockDecoration (SG::auxid_t /*auxid*/)
{
}


} // namespace SG
