// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxVectorBase.h 793298 2017-01-21 01:30:06Z ssnyder $
/**
 * @file AthContainers/AuxVectorBase.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2013
 * @brief Manage index tracking and synchronization of auxiliary data.
 */


#ifndef ATHCONTAINERS_AUXVECTORBASE_H
#define ATHCONTAINERS_AUXVECTORBASE_H


#include "AthContainers/AuxVectorData.h"
#include "AthContainers/OwnershipPolicy.h"
#include "AthContainers/IndexTrackingPolicy.h"
#include "AthContainers/AuxElement.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/exceptions.h"
#include "AthContainers/tools/ATHCONTAINERS_ASSERT.h"
#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainersInterfaces/AuxStore_traits.h"
#include <vector>


namespace SG {


/**
 * @brief Manage index tracking and synchronization of auxiliary data.
 *
 * An object, usually a @c DataVector, can have vectors of auxiliary
 * data associated with it.  In order to be able to find the auxiliary
 * data for a given container element, the elements must hold their
 * indices within the container (done by the @c AuxElement class).
 *
 * When the container changes, this information must be updated.
 * Indices need to be changed, and corresponding changes need to be made
 * in the vectors of associated auxiliary data.  Most of the code
 * in this class consists of helpers that effect these changes.
 * These are intended to be called only by the deriving container class.
 *
 * This class also contains a flag to control whether or not indices
 * should be tracked like this.  This is controlled by the type of the
 * payload and by the selected ownership and indexing policies.
 *
 * If the payload type does not derive from @c SG::AuxElement, then no index
 * tracking is possible, and the flag is always false.
 *
 * Otherwise, it depends on the setting of the policies.  The default
 * value for the index tracking policy is DEFAULT_TRACK_INDICES.  In that
 * case, we track indices if this container owns its elements
 * (OWN_ELEMENTS) and we don't if the container does not down its
 * elements (VIEW_ELEMENTS).  This is the usual case.  However,
 * the index tracking policy may also be set to ALWAYS_TRACK_INDICES
 * or NEVER_TRACK_INDICES to override this.  (The major foreseen use case
 * for this is to allow index tracking for a view container populated
 * with objects allocated from a @c DataPool.)
 *
 * The derived container should initialize this class by calling
 * @c initAuxVectorBase.  The derived container class is passed
 * as a template argument; the ownership and index tracking policies
 * are also passed.  This method can be called again if the policies change.
 *
 * There are only a couple user-callable methods.  The @c trackIndices
 * method returns the current state of the index tracking flag.
 * There are also wrappers for @c setStore method, which enforce the
 * condition that one can't set a store for a container that does
 * not do index tracking.
 *
 * The remaining methods are intended to be called only from the derived
 * container class (and thus they are declared as protected).
 * Besides a @c swap method, these include
 *
 *  - @c setIndices, to reset the indices within a range to sequential values.
 *  - @c clearIndex, to clear the index / container for a single element.
 *  - @c clearIndices, to clear the index / container for elements
 *    within a range.
 *  - @c resize, to change the size of the aux data.
 *  - @c reserve, to change the capacity of the aux data.
 *  - @c shift, to shift a range of aux data up or down, to track
 *    insertions or deletions from within the container.
 *  - @c moveAux, to be used when an element or range of elements is moved
 *    into the container.
 *  - @c swapElementsAux, to swap aux data for two elements, possibly in
 *    different containers.
 *  - @c resortAux, to rearrange indices and aux data correctly after
 *    an operation that permuted the elements of the container.
 *  
 */
class AuxVectorBase
  : public AuxVectorData
{
public:
  /**
   * @brief Default constructor.
   *
   * This will disable index tracking by default.
   * The constructor should be followed by a call to @c initAuxVectorBase.
   * (This is separated from the constructor because calling non-default
   * constructors of @c DataVector base classes can be awkward in the case
   * of virtual derivation.)
   */
  AuxVectorBase();


  /**
   * @brief Move constructor.
   * @param rhs The container from which to move.
   */
  AuxVectorBase (AuxVectorBase&& rhs);


  /**
   * @brief Move assignment.
   * @param rhs The container from which to move.
   */
  AuxVectorBase& operator= (AuxVectorBase&& rhs);


  /**
   * @brief Destructor.
   */
  virtual ~AuxVectorBase();


  /**
   * @brief Return true if index tracking is enabled for this container.
   */
  bool trackIndices() const;


  /**
   * @brief Set the store associated with this object.
   * @param store The new store.
   *
   * This will clear the non-const store pointer, and also
   * clear the cache.
   *
   * It is an error to set a store for a container for which index tracking
   * is disabled.  That will raise an @c ExcUntrackedSetStore exception.
   */
  void setStore (const SG::IConstAuxStore* store);


  /**
   * @brief Set the store associated with this object.
   * @param store The new store.
   *
   * This will set both the const and non-const store pointers, and also
   * clear the cache.
   *
   * It is an error to set a store for a container for which index tracking
   * is disabled.  That will raise an @c ExcUntrackedSetStore exception.
   */
  void setStore (SG::IAuxStore* store);


  /**
   * @brief Set the store associated with this object.
   * @param store The new store.
   *
   * This will clear the non-const store pointer, and also
   * clear the cache.
   *
   * It is an error to set a store for a container for which index tracking
   * is disabled.  That will raise an @c ExcUntrackedSetStore exception.
   */
  void setStore (const DataLink< SG::IConstAuxStore >& store);


  /**
   * @brief Synonym for @c setStore with @c IConstAuxStore.
   * @param store The new store.
   */
  void setConstStore (const SG::IConstAuxStore* store);


  /**
   * @brief Synonym for @c setStore with @c IAuxStore.
   * @param store The new store.
   */
  void setNonConstStore (SG::IAuxStore* store);


protected:
  /**
   * @brief Initialize index tracking mode.
   * @param ownPolicy The container ownership policy.
   * @param indexTrackingPolicy The requested index tracking policy.
   *
   * DVL should be the most-derived class for this container.
   *
   * This handles the logic for setting the state of index tracking.
   * If this container does not handle aux data, then index tracking
   * is always off.  Otherwise, it depends on the requested policies.
   * In any case, it is an error to turn off index tracking
   * for a container that has an associated aux store.
   */
  template <class DVL>
  void initAuxVectorBase (SG::OwnershipPolicy ownPolicy,
                          SG::IndexTrackingPolicy indexTrackingPolicy);


  /**
   * @brief Swap with another container.
   * @param other The container with which to swap.
   */
  void swap (AuxVectorBase& other);


  /**
   * @brief Set container/index for all elements within a range.
   * @param beg Beginning of the range.
   * @param end End of the range.
   * @param first Index to set for the first element in the range.
   *
   * For all elements in the range, the container will be set to this container,
   * and indices will be set sequentially, starting with @c first.
   *
   * @c ForwardIterator should be an iterator over the @c DataVector
   * (not a base iterator).
   */
  template <class ForwardIterator>
  void setIndices (ForwardIterator beg,
                   ForwardIterator end,
                   size_t first = 0);


  /**
   * @brief Clear the container / index for element @c elt.
   * @param elt Iterator to the element to clear.
   *
   * @c ForwardIterator should be an iterator over the @c DataVector
   * (not a base iterator).
   */
  template <class ForwardIterator>
  void clearIndex (ForwardIterator elt);


  /**
   * @brief Clear the container / index for a range of elements.
   * @param beg Beginning of the range.
   * @param end End of the range.
   *
   * @c ForwardIterator should be an iterator over the @c DataVector
   * (not a base iterator).
   */
  template <class ForwardIterator>
  void clearIndices (ForwardIterator beg, ForwardIterator end);


  /**
   * @brief Resize the aux data associated with this container.
   * @param size The new container size.
   *
   * DVL should be the most-derived class for this container.
   */
  template <class DVL>
  void resize (size_t size);


  /**
   * @brief Change the capacity of the aux data associated with this container.
   * @param size The new container size.
   *
   * DVL should be the most-derived class for this container.
   */
  template <class DVL>
  void reserve (size_t size);


  /**
   * @brief Shift the elements of the container.
   * @param cont The container that's being shifted.
   * @param pos The starting index for the shift.
   * @param offs The (signed) amount of the shift.
   *
   * The container should be the derived container.
   * The elements in the container should have already been shifted;
   * this operation will then adjust the element indices and also shift
   * the elements in the vectors for all aux data items.
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
  template <class DVL>
  void shift (DVL& cont, size_t pos, ptrdiff_t offs);


  /**
   * @brief Set index on an element and copy auxiliary data.
   * @param index Container index at which the new element is being added.
   * @param p The new element being added.
   * @param clear If true, then any auxiliary data initially associated
   *              with @c p are cleared after being copied.
   * @param skipDestClear Normally, if @c p does not have auxiliary data,
   *                      then the variables of the destination are cleared.
   *                      If this flag is true, then this clear is skipped.
   *                      This can be appropriate as part of a push_back,
   *                      where the destination is already known to be clear.
   *
   * Element @c p is being added to the container at @c index.
   * If @c p has associated auxiliary data, copy it to the container
   * at @c index.  Then set the container / index on @c p.
   */
  void moveAux (size_t index, SG::AuxElement* p,
                bool clear = false,
                bool skipDestClear = false);


  /**
   * @brief Set index on an element and copy auxiliary data.
   * @param index Container index at which the new element is being added.
   * @param p The new element being added.
   * @param clear If true, then any auxiliary data initially associated
   *              with @c p are cleared after being copied.
   * @param skipDestClear Normally, if @c p does not have auxiliary data,
   *                      then the variables of the destination are cleared.
   *                      If this flag is true, then this clear is skipped.
   *                      This can be appropriate as part of a push_back,
   *                      where the destination is already known to be clear.
   *
   * Overload for the no-auxdata case.
   */
  void moveAux (size_t index, const void* p, bool clear = false,
                bool skipDestClear = false);


  /**
   * @brief Set index on a range of elements and copy auxiliary data.
   * @param index Container index at which the first new element is being added.
   * @param beg The start of the range of new elements.
   * @param end The end of the range of new elements.
   *
   * The elements in the range are being a added to the container at @c index.
   * If the new elements have associated auxiliary data,
   * copy it to the container starting at @c index.
   * Then set the container / index on the elements in the range.
   *
   * @c ForwardIterator should be an iterator over the @c DataVector
   * (not a base iterator).
   */
  template <class ForwardIterator>
  void moveAux (size_t index, ForwardIterator beg, ForwardIterator end);


  /**
   * @brief Swap indices and auxiliary data between two elements.
   * @param aindex Index of the first element, in this container.
   * @param bindex Index of the second element, in @c bcont.
   * @param a Pointer to the first element.
   * @param b Pointer to the second element.
   * @param bcont Container holding the second element.
   *              (May be the same as this, but doesn't have to be.)
   *
   * Elements @c a at @c aindex in @c this and @c b at @c bindex in @c bcont
   * are being swapped.  Swap the index / container references between the
   * elements and also swap the auxiliary data if it exists.
   */
  void swapElementsAux (size_t aindex,
                        size_t bindex,
                        SG::AuxElement* a,
                        SG::AuxElement* b,
                        AuxVectorBase* bcont);


  /**
   * @brief Swap indices and auxiliary data between two elements.
   * @param aindex Index of the first element, in this container.
   * @param bindex Index of the second element, in @c bcont.
   * @param a Pointer to the first element.
   * @param b Pointer to the second element.
   * @param bcont Container holding the second element.
   *              (May be the same as this, but doesn't have to be.)
   *
   * This is the no-auxdata case; it is a no-op except for checking
   * @c m_trackIndices.
   */
  void swapElementsAux (size_t aindex,
                        size_t bindex,
                        const void* a,
                        const void* b,
                        AuxVectorBase* bcont);


  /**
   * @brief Reset indices / reorder aux data after elements have been permuted.
   * @param index Index in the container of the start of the range.
   * @param beg Start of the range of elements to process.
   * @param end End of the range of elements to process.
   *
   * Call this after some operation that has permuted the elements in the
   * container (such as sort).  The index information in the elements
   * will be used to permute all auxiliary data in the same way.
   * Finally, all the indices will be reset in the correct order.
   *
   * @c ForwardIterator should be an iterator over the @c DataVector
   * (not a base iterator).
   */
  template <class ForwardIterator>
  void resortAux (size_t index,
                  ForwardIterator beg,
                  ForwardIterator end);


private:
  /**
   * @brief Initialize index tracking mode --- no-auxdata specialization.
   * @param ownPolicy The container ownership policy.
   * @param indexTrackingPolicy The requested index tracking policy.
   *
   * Since this is the no-auxdata case, it always sets index tracking to false.
   * An exception is raised if the container has an associated store
   * (but that should never actually happen).
   */
  void initAuxVectorBase1 (const std::false_type&,
                           SG::OwnershipPolicy /*ownPolicy*/,
                           SG::IndexTrackingPolicy /*indexTrackingPolicy*/);


  /**
   * @brief Initialize index tracking mode --- auxdata specialization.
   * @param ownPolicy The container ownership policy.
   * @param indexTrackingPolicy The requested index tracking policy.
   *
   * Sets index tracking based on the requested policies.
   * An exception is raised if index tracking is disabled and the container
   * has an associated store.
   */
  void initAuxVectorBase1 (const std::true_type&,
                           SG::OwnershipPolicy ownPolicy,
                           SG::IndexTrackingPolicy indexTrackingPolicy);


  /**
   * @brief Set container/index for all elements within a range.
   * @param beg Beginning of the range.
   * @param end End of the range.
   * @param first Index to set for the first element in the range.
   *
   * This is the no-auxdata case; doesn't do anything other than checking
   * @c m_trackIndices.
   */
  template <class ForwardIterator>
  void setIndices1 (const std::false_type&,
                    ForwardIterator beg,
                    ForwardIterator end,
                    size_t first);


  /**
   * @brief Set container/index for all elements within a range.
   * @param beg Beginning of the range.
   * @param end End of the range.
   * @param first Index to set for the first element in the range.
   *
   * This is the auxdata case.
   * For all elements in the range, the container will be set to this container,
   * and indices will be set sequentially, starting with @c first.
   */
  template <class ForwardIterator>
  void setIndices1 (const std::true_type&,
                    ForwardIterator beg,
                    ForwardIterator end,
                    size_t first);


  /**
   * @brief Clear the container / index for element @c elt.
   * @param elt Iterator to the element to clear.
   *
   * This is the no-auxdata case; doesn't do anything other than checking
   * @c m_trackIndices.
   */
  template <class ForwardIterator> 
  void clearIndex1 (const std::false_type&, ForwardIterator elt);


  /**
   * @brief Clear the container / index for element @c elt.
   * @param elt Iterator to the element to clear.
   *
   * This is the auxdata case.
   */
  template <class ForwardIterator>
  void clearIndex1 (const std::true_type&, ForwardIterator elt);


  /**
   * @brief Clear the container / index for a range of elements.
   * @param beg Beginning of the range.
   * @param end End of the range.
   *
   * No-auxdata case; a no-op except for checking @c m_trackIndices.
   */
  template <class ForwardIterator>
  void clearIndices1 (const std::false_type&,
                      ForwardIterator,
                      ForwardIterator);


  /**
   * @brief Clear the container / index for a range of elements.
   * @param beg Beginning of the range.
   * @param end End of the range.
   *
   * Auxdata case.
   */
  template <class ForwardIterator>
  void clearIndices1 (const std::true_type&,
                      ForwardIterator beg,
                      ForwardIterator end);



  /**
   * @brief Resize the aux data associated with this container.
   * @param size The new container size.
   *
   * The no-auxdata case; a no-op except for checking @c m_trackIndices.
   */
  void resize1 (const std::false_type&, size_t size);


  /**
   * @brief Resize the aux data associated with this container.
   * @param size The new container size.
   *
   * The auxdata case.
   */
  void resize1 (const std::true_type&, size_t size);


  /**
   * @brief Change the capacity of the aux data associated with this container.
   * @param size The new container size.
   *
   * The no-auxdata case; a no-op except for checking @c m_trackIndices.
   */
  void reserve1 (const std::false_type&, size_t size);


  /**
   * @brief Change the capacity of the aux data associated with this container.
   * @param size The new container size.
   *
   * The auxdata case.
   */
  void reserve1 (const std::true_type&, size_t size);


  /**
   * @brief Shift the elements of the container.
   * @param cont The container that's being shifted.
   * @param pos The starting index for the shift.
   * @param offs The (signed) amount of the shift.
   *
   * No-auxdata version; a no-op except for checking @c m_trackIndices.
   */
  template <class DVL>
  void shift1 (const std::false_type&,
               DVL& cont, size_t pos, ptrdiff_t offs);


  /**
   * @brief Shift the elements of the container.
   * @param cont The container that's being shifted.
   * @param pos The starting index for the shift.
   * @param offs The (signed) amount of the shift.
   *
   * Auxdata version.
   *
   * The container should be the derived container.
   * The elements in the container should have already been shifted;
   * this operation will then adjust the element indices and also shift
   * the elements in the vectors for all aux data items.
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
  template <class DVL>
  void shift1 (const std::true_type&,
               DVL& cont, size_t pos, ptrdiff_t offs);


  /**
   * @brief Set index on a range of elements and copy auxiliary data.
   * @param index Container index at which the first new element is being added.
   * @param beg The start of the range of new elements.
   * @param end The end of the range of new elements.
   *
   * No-auxdata version; a no-op except for checking @c m_trackIndices.
   */
  template <class ForwardIterator>
  void moveAux1 (const std::false_type&,
                 size_t index,
                 ForwardIterator beg,
                 ForwardIterator end);


  /**
   * @brief Set index on a range of elements and copy auxiliary data.
   * @param index Container index at which the first new element is being added.
   * @param beg The start of the range of new elements.
   * @param end The end of the range of new elements.
   *
   * The elements in the range are being a added to the container at @c index.
   * If the new elements have associated auxiliary data,
   * copy it to the container starting at @c index.
   * Then set the container / index on the elements in the range.
   *
   * The auxdata case.
   */
  template <class ForwardIterator>
  void moveAux1 (const std::true_type&,
                 size_t index,
                 ForwardIterator beg,
                 ForwardIterator end);


  /**
   * @brief Reset indices / reorder aux data after elements have been permuted.
   * @param index Index in the container of the start of the range.
   * @param beg Start of the range of elements to process.
   * @param end End of the range of elements to process.
   *
   * No-auxdata version; a no-op except for checking @c m_trackIndices.
   */
  template <class ForwardIterator>
  void resortAux1 (const std::false_type&,
                   size_t index,
                   ForwardIterator beg,
                   ForwardIterator end);


  /**
   * @brief Reset indices / reorder aux data after elements have been permuted.
   * @param index Index in the container of the start of the range.
   * @param beg Start of the range of elements to process.
   * @param end End of the range of elements to process.
   *
   * Call this after some operation that has permuted the elements in the
   * container (such as sort).  The index information in the elements
   * will be used to permute all auxiliary data in the same way.
   * Finally, all the indices will be reset in the correct order.
   *
   * The auxdata case.
   */
  template <class ForwardIterator>
  void resortAux1 (const std::true_type&,
                   size_t index,
                   ForwardIterator a,
                   ForwardIterator b);


private:
  /**
   * @brief Helper to factor out template-independent part of @c resortAux.
   * 
   * See the cxx file for more details.
   */
  class ResortAuxHelper
  {
  public:
    ResortAuxHelper (size_t sz, size_t index, AuxVectorBase& vec);
    void resortElement (size_t idx, SG::AuxElement* elt);

  private:
    AuxVectorBase& m_vec;
    size_t m_index;
    std::vector<size_t> m_imap;
    std::vector<size_t> m_rmap;
    std::vector<void*> m_auxdata;
    std::vector<SG::auxid_t> m_auxids;
    SG::AuxTypeRegistry::lock_t m_lock;

    ResortAuxHelper();
  };
  void* getDataArrayForResort (SG::auxid_t auxid);
  void setIndexForResort (SG::AuxElement* elt, size_t idx);
  friend class ResortAuxHelper;


  /// Flag if index tracking is enabled.
  bool m_trackIndices;
};


} // namespace SG


#include "AthContainers/AuxVectorBase.icc"


#endif // not DATAMODEL_AUXVECTORBASE_H
