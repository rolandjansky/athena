/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/src/AuxVectorBase.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2013
 * @brief Manage index tracking and synchronization of auxiliary data.
 */


#include "AthContainers/AuxVectorBase.h"


namespace SG {


/**
 * @brief Default constructor.
 *
 * This will disable index tracking by default.
 * The constructor should be followed by a call to @c initAuxVectorBase.
 * (This is separated from the constructor because calling non-default
 * constructors of @c DataVector base classes can be awkward in the case
 * of virtual derivation.)
 */
AuxVectorBase::AuxVectorBase()
  : m_trackIndices (false)
{
}


/**
 * @brief Move constructor.
 * @param rhs The container from which to move.
 */
AuxVectorBase::AuxVectorBase (AuxVectorBase&& rhs)
  : AuxVectorData (std::move (rhs)),
    m_trackIndices (rhs.m_trackIndices)
{
}


/**
 * @brief Move assignment.
 * @param rhs The container from which to move.
 */
AuxVectorBase& AuxVectorBase::operator= (AuxVectorBase&& rhs)
{
  if (this != &rhs) {
    AuxVectorData::operator= (std::move (rhs));
    m_trackIndices = rhs.m_trackIndices;
  }
  return *this;
}


/**
 * @brief Destructor.
 */
AuxVectorBase::~AuxVectorBase()
{
}


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
void AuxVectorBase::setStore (const SG::IConstAuxStore* store)
{
  if (store && !m_trackIndices)
    throw SG::ExcUntrackedSetStore();
  SG::AuxVectorData::setStore (store);
}


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
void AuxVectorBase::setStore (SG::IAuxStore* store)
{
  if (store && !m_trackIndices)
    throw SG::ExcUntrackedSetStore();
  SG::AuxVectorData::setStore (store);
}


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
void AuxVectorBase::setStore (const DataLink<SG::IConstAuxStore>& store)
{
  if (!m_trackIndices)
    throw SG::ExcUntrackedSetStore();
  SG::AuxVectorData::setStore (store);
}


/**
 * @brief Initialize index tracking mode --- no-auxdata specialization.
 * @param ownPolicy The container ownership policy.
 * @param indexTrackingPolicy The requested index tracking policy.
 *
 * Since this is the no-auxdata case, it always sets index tracking to false.
 * An exception is raised if the container has an associated store
 * (but that should never actually happen).
 */
void
AuxVectorBase::initAuxVectorBase1 (const std::false_type&,
                                   SG::OwnershipPolicy /*ownPolicy*/,
                                   SG::IndexTrackingPolicy /*indexTrackingPolicy*/)
{
  m_trackIndices = false;
  if (this->hasStore())
    throw SG::ExcUntrackedSetStore();
}


/**
 * @brief Initialize index tracking mode --- auxdata specialization.
 * @param ownPolicy The container ownership policy.
 * @param indexTrackingPolicy The requested index tracking policy.
 *
 * Sets index tracking based on the requested policies.
 * An exception is raised if index tracking is disabled and the container
 * has an associated store.
 */
void
AuxVectorBase::initAuxVectorBase1 (const std::true_type&,
                                   SG::OwnershipPolicy ownPolicy,
                                   SG::IndexTrackingPolicy indexTrackingPolicy)
{
  if (indexTrackingPolicy == SG::ALWAYS_TRACK_INDICES)
    m_trackIndices = true;
  else if (indexTrackingPolicy == SG::NEVER_TRACK_INDICES)
    m_trackIndices = false;
  else if (ownPolicy == SG::OWN_ELEMENTS)
    m_trackIndices = true;
  else
    m_trackIndices = false;

  if (!m_trackIndices && this->hasStore())
    throw SG::ExcUntrackedSetStore();
}


/**
 * @brief Resize the aux data associated with this container.
 * @param size The new container size.
 *
 * The auxdata case.
 */
void AuxVectorBase::resize1 (const std::true_type&, size_t size)
{
  if (this->hasNonConstStore()) {
    if (!this->getStore()->resize (size)) {
      // Only clear the cache if iterators have been invalidated.
      clearCache();
    }
  }
  else if (this->hasStore())
    throw SG::ExcConstAuxData ("resize");
}


/**
 * @brief Change the capacity of the aux data associated with this container.
 * @param size The new container size.
 *
 * The auxdata case.
 */
void AuxVectorBase::reserve1 (const std::true_type&, size_t size)
{
  if (this->hasNonConstStore()) {
    this->getStore()->reserve (size);
    clearCache();
  }
  else if (this->hasStore())
    throw SG::ExcConstAuxData ("reserve");
}


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
void
AuxVectorBase::moveAux (size_t index, SG::AuxElement* p,
                        bool clear /*= false*/,
                        bool skipDestClear /*= false*/)
{
  if (!m_trackIndices)
    return;

  SG::AuxElement to (this, index);
  if (!p) {
    if (!skipDestClear) to.clearAux();
    return;
  }

  if (p->hasStore() || !skipDestClear)
    to.copyAux (*p);
  if (clear)
    p->clearAux();
  p->setIndex (index, this);
}


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
void AuxVectorBase::swapElementsAux (size_t aindex,
                                     size_t bindex,
                                     SG::AuxElement* a,
                                     SG::AuxElement* b,
                                     AuxVectorBase* bcont)
{
  AuxVectorBase* acont = this;

  // If one of the elements is null, then this is just a @c moveAux.
  if (!b) {
    if (!a) return;
    acont->moveAux (aindex, a, true);
    return;
  }
  if (!a) {
    bcont->moveAux (bindex, b, true);
    return;
  }

  // Reset indices.
  if (b && bcont->trackIndices())
    b->setIndex (bindex, bcont);
  if (a && acont->trackIndices())
    a->setIndex (aindex, acont);

  // Check stores.
  if (!this->hasNonConstStore()) {
    if (this->hasStore()) {
      throw SG::ExcConstAuxData ("swapElementsAux");
    }
    return;
  }

  // Swap aux data.

  const SG::auxid_set_t& a_ids = acont->getAuxIDs();
  const SG::auxid_set_t& b_ids = bcont->getAuxIDs();

  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  SG::AuxTypeRegistry::lock_t lock (r);

  for (SG::auxid_t auxid : a_ids) {
    void* aptr = acont->getDataArray (auxid);
    void* bptr = bcont->getDataArray (auxid);
    r.swap (lock, auxid, aptr, aindex, bptr, bindex);
  }
  for (SG::auxid_t auxid : b_ids) {
    if (a_ids.find (auxid) == a_ids.end()) {
      void* aptr = acont->getDataArray (auxid);
      void* bptr = bcont->getDataArray (auxid);
      r.swap (lock, auxid, aptr, aindex, bptr, bindex);
    }
  }
}


/**
 * @brief Set up for resorting.
 * @param sz The number of elements in the resort range.
 * @param index The index of the first element in the range in the container.
 * @param vec The containing vector.
 */
AuxVectorBase::ResortAuxHelper::ResortAuxHelper (size_t sz,
                                                 size_t index,
                                                 AuxVectorBase& vec)
  : m_vec (vec),
    m_index (index),
    m_imap (sz),
    m_rmap (sz),
    m_lock (SG::AuxTypeRegistry::instance())
{
  for (size_t i = 0; i < sz; i++)
    m_imap[i] = m_rmap[i] = i;

  const SG::auxid_set_t& auxid_set = vec.getAuxIDs();
  size_t naux = auxid_set.size();
  m_auxdata.reserve (naux);
  m_auxids.reserve (naux);
  for (SG::auxid_t auxid : auxid_set) {
    m_auxdata.push_back (vec.getDataArrayForResort (auxid));
    m_auxids.push_back (auxid);
  }
}


/**
 * @param Resort one element.
 * @param idx The index of the element within the range.
 * @param elt The element.
 */
void
AuxVectorBase::ResortAuxHelper::resortElement (size_t idx, SG::AuxElement* elt)
{
  //                          V
  // x:        9  8  7  6  5  0  1  2  3  4
  // m_index:  0  1  2  3  4  0  1  2  3  4
  // imap      9  8  7  6  5  4  3  2  1  0
  // rmap      9  8  7  6  5  4  3  2  1  0
  // aux      19 18 17 16 15 14 13 12 11 10
  //
  // imap[i] tells in which slot the auxdata originally in slot
  // i is currently located.
  // rmap[i] gives the original location of the auxdata
  // currently in slot i.
  //
  // each time through the loop, looking at index i:
  //  -- look at the index that was set.
  //     we want to move the aux data originally at
  //     auxindex=v[i]->index() here.
  //  -- that data is currently at slot ii1 = imap[auxindex]
  //  -- swap auxdata between slots i and ii1
  //  -- swap rmap between slots i and ii1
  //  -- swap imap entries rmap[i] and rmap[ii1]
  //  -- set v[i]->index() to index.
  // but need to remember to take into account any offset between
  // the beginning of the range we're given and the beginning 
  // of the container (i.e., index was nonzero on entry).
  //
  // ??? can this be made more efficient?  do we need so much
  //     working storage???  xxx

  size_t index = m_index;
  size_t auxindex = elt->index();
  ATHCONTAINERS_ASSERT (auxindex-index < m_imap.size());
  size_t ii1 = m_imap[auxindex-index];
  if (ii1 != idx) {
    ATHCONTAINERS_ASSERT (ii1 > idx);
    // swap between auxdata slots dx+index, ii1+index
    SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
    size_t naux = m_auxids.size();
    for (size_t iid = 0; iid < naux; iid++) {
      void* ptr = m_auxdata[iid];
      r.swap (m_lock, m_auxids[iid], ptr, idx+index, ptr, ii1+index);
    }
    std::swap (m_rmap[idx], m_rmap[ii1]);
    std::swap (m_imap[m_rmap[idx]], m_imap[m_rmap[ii1]]);
  }

  m_vec.setIndexForResort (elt, index+idx);
}


} // namespace SG
