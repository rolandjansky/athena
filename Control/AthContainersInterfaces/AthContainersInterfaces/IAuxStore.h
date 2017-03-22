// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IAuxStore.h 797203 2017-02-14 19:35:56Z ssnyder $
/**
 * @file AthContainersInterfaces/IAuxStore.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2013
 * @brief Interface for non-const operations on an auxiliary store.
 *
 * A @c DataVector may have auxiliary data associated with it.
 * These data are kept in separate AuxStore objects; this defines
 * the interface for non-const operations on such a store.
 * Const operations are defined by the base class
 * @c IConstAuxStore.
 *
 * An AuxStore may store a number of different aux data items;
 * these are identified by an integer of type @c auxid_t.
 * Each item is stored as a vector with one entry per entry
 * in the containing @c DataVector.
 *
 * This interface provides methods for getting a non-const pointer
 * to an existing vector of aux data items and for operations that
 * change the number of aux data items.
 * of existing aux data items.
 */


#ifndef ATHCONTAINERSINTERFACES_IAUXSTORE_H
#define ATHCONTAINERSINTERFACES_IAUXSTORE_H


#include "AthContainersInterfaces/IConstAuxStore.h"
#include <cstddef>


namespace SG {


class AuxDataOption;


/**
 * @brief Interface for non-const operations on an auxiliary store.
 */
class IAuxStore
  : public IConstAuxStore
{
public:
  /// Pick up the const version from the base class.
  using IConstAuxStore::getData;


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
   *
   * If the container is locked, throw an exception.
   */
  virtual void* getData (auxid_t auxid, size_t size, size_t capacity) = 0;


  /**
   * @brief Return a set of identifiers for writable data items
   *        in this store.
   *
   *        This should include only non-const identifiers.
   */
  virtual const SG::auxid_set_t& getWritableAuxIDs() const = 0;


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
  virtual bool resize (size_t sz) = 0;


  /**
   * @brief Change the capacity of all aux data vectors.
   * @param sz The new capacity.
   *
   * This should be called when the capacity of the container changes
   * (by @c reserve).  This should change the capacity for the vectors
   * for all aux data items.
   */
  virtual void reserve (size_t sz) = 0;


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
  virtual void shift (size_t pos, ptrdiff_t offs) = 0;


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
  virtual bool insertMove (size_t pos,
                           IAuxStore& other,
                           const SG::auxid_set_t& ignore = SG::auxid_set_t())
    = 0;

  
  /**
   * @brief Set an option for a given auxiliary variable.
   * @param auxid The identifier of the desired aux data item.
   * @param option The option to set.
   *
   * The interpretation of the option depends on the particular representation
   * of the variable.
   *
   * Returns true if the option setting was successful; false otherwise.
   */
  virtual bool setOption (auxid_t /*auxid*/, const AuxDataOption& /*option*/)
  { return false; }
};


} // namespace SG



#ifndef XAOD_STANDALONE
#include "SGTools/BaseInfo.h"
#include "SGTools/CLASS_DEF.h"
SG_BASE( SG::IAuxStore, SG::IConstAuxStore );
CLASS_DEF( SG::IAuxStore , 178378299 , 1 )
#endif


#endif // not ATHCONTAINERSINTERFACES_IAUXSTORE_H
