// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IConstAuxStore.h 612954 2014-08-21 19:25:01Z ssnyder $
/**
 * @file AthContainersInterfaces/IConstAuxStore.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2013
 * @brief Interface for const operations on an auxiliary store.
 */


#ifndef ATHCONTAINERSINTERFACES_ICONSTAUXSTORE_H
#define ATHCONTAINERSINTERFACES_ICONSTAUXSTORE_H


#include "AthContainersInterfaces/AuxTypes.h"
#include <cstddef>


namespace SG {


/**
 * @brief Interface for const operations on an auxiliary store.
 *
 * A @c DataVector may have auxiliary data associated with it.
 * These data are kept in separate AuxStore objects; this defines
 * the interface for const operations on such a store.
 *
 * An AuxStore may store a number of different aux data items;
 * these are identified by an integer of type @c auxid_t.
 * Each item is stored as a vector with one entry per entry
 * in the containing @c DataVector.
 *
 * This interface provides methods for getting a const pointer
 * to an existing vector of aux data items and for getting the set
 * of existing aux data items.
 *
 * In addition, sometimes we want to add additional auxiliary data to
 * an existing, const container; this is called `decorating' it.
 * For example, we retrieve a const container from StoreGate,
 * run some algorithm on it, and attach additional data to the object
 * that can be accessed by downstream algorithms or written to a file.
 * To support this, we add the @c getDecoration operation, as well
 * as @c lock and @c clearDecorations.  When the object is first
 * being filled, it is unlocked.  It gets locked when @c StoreGateSvc::setConst
 * is called on it.  From then on, we can only create decorations.
 * Calling @c clearDecorations will restore the state back to where
 * it was when @c lock was called.  (Be sure that the container's
 * cache also gets cleared.)
 *
 * @c getDecoration then works as follows:
 * 
 *  - If the item doesn't exist, then it will be created.  If the container
 *    is locked, then the new item will be marked as a decoration.
 *  - If the item already exists, then we allow access only if either
 *    the container is not locked or the item is marked as a decoration.
 */
class IConstAuxStore
{
public:
  /// Destructor.
  virtual ~IConstAuxStore() = default;


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
  virtual const void* getData (SG::auxid_t auxid) const = 0;


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
  virtual void* getDecoration (auxid_t auxid, size_t size, size_t capacity) = 0;


  /**
   * @brief Return a set of identifiers for existing data items
   *        in this store.
   *
   *        This should include identifiers for all items,
   *        const and non-const.
   */
  virtual const SG::auxid_set_t& getAuxIDs() const = 0;


  /**
   * @brief Test if a particular variable is tagged as a decoration.
   * @param auxid The identifier of the desired aux data item.
   */
  virtual bool isDecoration (auxid_t auxid) const = 0;


  /**
   * @brief Lock the container.
   *
   * After this, only decorations can be changed/modified.
   * If the container is already locked, this is a no-op.
   */
  virtual void lock() = 0;


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
  virtual bool clearDecorations() = 0;


  /**
   * @brief Return the number of elements in the store.
   *
   * May return 0 for a store with no aux data.
   */
  virtual size_t size() const = 0;


  /**
   * @brief Lock a decoration.
   * @param auxid Identifier of the decoration to lock.
   *
   * A decoration is locked by changing from a decoration to an ordinary
   * variable.  If the container itself is locked, then modifications
   * to the variable are not permitted after this call.
   */
  virtual void lockDecoration (SG::auxid_t auxid) = 0;
};


} // namespace SG



#ifndef XAOD_STANDALONE
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SG::IConstAuxStore , 187169987 , 1 )
#endif // not XAOD_STANDALONE


#endif // not ATHCONTAINERSINTERFACES_ICONSTAUXSTORE_H
