// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthContainers/AuxStoreConstMem.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2021
 * @brief IConstAuxStore implementation referencing external buffers.
 */


#ifndef ATHCONTAINERS_AUXSTORECONSTMEM_H
#define ATHCONTAINERS_AUXSTORECONSTMEM_H


#include "AthContainersInterfaces/IConstAuxStore.h"


namespace SG {


/**
 * @brief IConstAuxStore implementation referencing external buffers.
 *
 * This is an implementation of @c IConstAuxStore that, rather than managing
 * memory itself, simply holds references to buffers managed externally.
 *
 * You need to specify the number of elements when you create the object.
 * Then call @c addBuffer for each auxiliary variable to be managed,
 * passing the auxid and the buffer.
 *
 * This class assumes the managed data is strictly const, and hence
 * makes no attempt to implement decorations.
 */
class AuxStoreConstMem
  : public IConstAuxStore
{
public:
  /**
   * @brief Constructor.
   * @param size The number of elements in the container.
   */
  AuxStoreConstMem (size_t size);


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
  void addBuffer (SG::auxid_t auxid, const void* buf);


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
  virtual const void* getData (SG::auxid_t auxid) const override;


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
  virtual void* getDecoration (auxid_t auxid, size_t size, size_t capacity) override;


  /**
   * @brief Return a set of identifiers for existing data items
   *        in this store.
   *
   *        This should include identifiers for all items,
   *        const and non-const.
   */
  virtual const SG::auxid_set_t& getAuxIDs() const override;


  /**
   * @brief Test if a particular variable is tagged as a decoration.
   * @param auxid The identifier of the desired aux data item.
   *
   * This always returns false for this (const) class.
   */
  virtual bool isDecoration (auxid_t auxid) const override;


  /**
   * @brief Lock the container.
   *
   * After this, only decorations can be changed/modified.
   *
   * This is a no-op for this (const) class.
   */
  virtual void lock() override;


  /**
   * @brief Clear all decorations.
   *
   * This is a no-op for this (const) class.  Always returns false.
   */
  virtual bool clearDecorations() override;


  /**
   * @brief Return the number of elements in the store.
   *
   * May return 0 for a store with no aux data.
   */
  virtual size_t size() const override;


  /**
   * @brief Lock a decoration.
   * @param auxid Identifier of the decoration to lock.
   *
   * This is a no-op for this (const) class.
   */
  virtual void lockDecoration (SG::auxid_t auxid) override;


private:
  size_t m_size;
  std::vector<const void*> m_buffers;

  /// Set of @c auxid's for which we've added a buffer.
  SG::auxid_set_t m_auxids;
};


} // namespace SG

#endif // not ATHCONTAINERS_AUXSTORECONSTMEM_H
