/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/AuxTypeRegistry.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2013
 * @brief Handle mappings between names and auxid_t.
 */


#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/exceptions.h"
#include "AthContainers/normalizedTypeinfoName.h"
#include "AthContainers/tools/foreach.h"
#include "AthLinks/ElementLinkBase.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <sstream>
#include <cstring>


namespace SG {


/**
 * @brief Return the singleton registry instance.
 */
AuxTypeRegistry& AuxTypeRegistry::instance()
{
  static AuxTypeRegistry auxTypeRegistry ATLAS_THREAD_SAFE;
  return auxTypeRegistry;
}


/**
 * @brief Look up a name -> @c auxid_t mapping.
 * @param ti Type of the aux data item.
 * @param name The name of the aux data item.
 * @param clsname The name of its associated class.  May be blank.
 *
 * The type of the item is given by @a ti.
 * Return @c null_auxid if we don't know how to make vectors of @a ti.
 * (Use @c addFactory to register additional types.)
 * If an item with the same name was previously requested
 * with a different type, then raise @c SG::ExcAuxTypeMismatch.
 */
SG::auxid_t AuxTypeRegistry::getAuxID (const std::type_info& ti,
                                       const std::string& name,
                                       const std::string& clsname /*= ""*/)
{
  return findAuxID (name, clsname, ti, &AuxTypeRegistry::makeFactoryNull);
}


/**
 * @brief Look up a name -> @c auxid_t mapping.
 * @param name The name of the aux data item.
 * @param clsname The name of its associated class.  May be blank.
 *
 * Will only find an existing @c auxid_t; unlike @c getAuxID,
 * this won't make a new one.  If the item isn't found, this
 * returns @c null_auxid.
 */
SG::auxid_t
AuxTypeRegistry::findAuxID( const std::string& name,
                            const std::string& clsname ) const
{
   upgrading_lock_t lock (const_cast<mutex_t&>(m_mutex));
   key_t key (name, clsname);
   id_map_t::const_iterator i = m_auxids.find (key);
   if (i != m_auxids.end()) {
      return i->second;
   }
   return null_auxid;
}


/**
 * @brief Construct a new vector to hold an aux item.
 * @param auxid The desired aux data item.
 * @param size Initial size of the new vector.
 * @param capacity Initial capacity of the new vector.
 */
IAuxTypeVector* AuxTypeRegistry::makeVector (SG::auxid_t auxid,
                                             size_t size,
                                             size_t capacity) const
{
  const SG::IAuxTypeVectorFactory* factory = getFactory (auxid);
  assert (factory != 0);
  return factory->create (size, capacity);
}


/**
 * @brief Construct a new vector to hold an aux item (external locking).
 * @param lock The registry lock.
 * @param auxid The desired aux data item.
 * @param size Initial size of the new vector.
 * @param capacity Initial capacity of the new vector.
 */
IAuxTypeVector* AuxTypeRegistry::makeVector (lock_t& lock,
                                             SG::auxid_t auxid,
                                             size_t size,
                                             size_t capacity) const
{
  const SG::IAuxTypeVectorFactory* factory = getFactory (lock, auxid);
  assert (factory != 0);
  return factory->create (size, capacity);
}


/**
 * @brief Construct an @c IAuxTypeVector object from a vector.
 * @param data The vector object.
 * @param isPacked If true, @c data is a @c PackedContainer.
 * @param ownFlag If true, the newly-created IAuxTypeVector object
 *                will take ownership of @c data.
 *
 * If the element type is T, then @c data should be a pointer
 * to a std::vector<T> object, which was obtained with @c new.
 * But if @c isPacked is @c true, then @c data
 * should instead point at an object of type @c SG::PackedContainer<T>.
 *
 * Returns a newly-allocated object.
 * FIXME: Should return a unique_ptr.
 */
IAuxTypeVector* AuxTypeRegistry::makeVectorFromData (SG::auxid_t auxid,
                                                     void* data,
                                                     bool isPacked,
                                                     bool ownMode) const
{
  const SG::IAuxTypeVectorFactory* factory = getFactory (auxid);
  assert (factory != 0);
  return factory->createFromData (data, isPacked, ownMode);
}



/**
 * @brief Return the name of an aux data item.
 * @param auxid The desired aux data item.
 */
std::string AuxTypeRegistry::getName (SG::auxid_t auxid) const
{
  lock_t lock (*this);
  return getName (lock, auxid);
}


/**
 * @brief Return the name of an aux data item (external locking).
 * @param lock The registry lock.
 * @param auxid The desired aux data item.
 */
std::string AuxTypeRegistry::getName (lock_t& /*lock*/,
                                      SG::auxid_t auxid) const
{
  if (auxid >= m_types.size())
    return "";
  return m_types[auxid].m_name;
}


/**
 * @brief Return the class name associated with an aux data item
 *        (may be blank).
 * @param auxid The desired aux data item.
 */
std::string AuxTypeRegistry::getClassName (SG::auxid_t auxid) const
{
  lock_t lock(*this);
  return getClassName (lock, auxid);
}


/**
 * @brief Return the class name associated with an aux data item
 *        (may be blank).  [external locking] 
 * @param lock The registry lock.
 * @param auxid The desired aux data item.
 */
std::string AuxTypeRegistry::getClassName (lock_t& /*lock*/,
                                           SG::auxid_t auxid) const
{
  if (auxid >= m_types.size())
    return "";
  return m_types[auxid].m_clsname;
}


/**
 * @brief Return the type of an aux data item.
 * @param auxid The desired aux data item.
 */
const std::type_info* AuxTypeRegistry::getType (SG::auxid_t auxid) const
{
  lock_t lock (*this);
  return getType (lock, auxid);
}


/**
 * @brief Return the type of an aux data item (external locking).
 * @param lock The registry lock.
 * @param auxid The desired aux data item.
 */
const std::type_info* AuxTypeRegistry::getType (lock_t& /*lock*/,
                                                SG::auxid_t auxid) const
{
  if (auxid >= m_types.size())
    return 0;
  return m_types[auxid].m_ti;
}


/**
 * @brief Return the type name of an aux data item.
 * @param auxid The desired aux data item.
 *
 * Returns an empty string if the type is not known.
 */
std::string AuxTypeRegistry::getTypeName (SG::auxid_t auxid) const
{
  lock_t lock (*this);
  return getTypeName (lock, auxid);
}


/**
 * @brief Return the type name of an aux data item (external locking).
 * @param lock The registry lock.
 * @param auxid The desired aux data item.
 *
 * Returns an empty string if the type is not known.
 */
std::string AuxTypeRegistry::getTypeName (lock_t& /*lock*/,
                                          SG::auxid_t auxid) const
{
  if (auxid >= m_types.size())
    return "";
  return normalizedTypeinfoName (*m_types[auxid].m_ti);
}


/**
 * @brief Return the type of the STL vector used to hold an aux data item.
 * @param auxid The desired aux data item.
 */
const std::type_info* AuxTypeRegistry::getVecType (SG::auxid_t auxid) const
{
  const SG::IAuxTypeVectorFactory* factory = getFactory (auxid);
  if (factory)
    return factory->tiVec();
  return 0;
}


/**
 * @brief Return the type of the STL vector used to hold an aux data item.
 *        (external locking)
 * @param lock The registry lock.
 * @param auxid The desired aux data item.
 */
const std::type_info* AuxTypeRegistry::getVecType (lock_t& lock,
                                                   SG::auxid_t auxid) const
{
  const SG::IAuxTypeVectorFactory* factory = getFactory (lock, auxid);
  if (factory)
    return factory->tiVec();
  return 0;
}


/**
 * @brief Return the type name of the STL vector used to hold an aux data item.
 * @param auxid The desired aux data item.
 *
 * Returns an empty string if the type is not known.
 */
std::string AuxTypeRegistry::getVecTypeName (SG::auxid_t auxid) const
{
  const SG::IAuxTypeVectorFactory* factory = getFactory (auxid);
  if (factory)
    return normalizedTypeinfoName (*factory->tiVec());
  return "";
}


/**
 * @brief Return the type name of the STL vector used to hold an aux data item.
 *        (external locking)
 * @param lock The registry lock.
 * @param auxid The desired aux data item.
 *
 * Returns an empty string if the type is not known.
 */
std::string AuxTypeRegistry::getVecTypeName (lock_t& lock,
                                             SG::auxid_t auxid) const
{
  const SG::IAuxTypeVectorFactory* factory = getFactory (lock, auxid);
  if (factory)
    return normalizedTypeinfoName (*factory->tiVec());
  return "";
}


/**
 * @brief Return size of an element in the STL vector.
 * @param auxid The desired aux data item.
 */
size_t AuxTypeRegistry::getEltSize (SG::auxid_t auxid) const
{
  const SG::IAuxTypeVectorFactory* factory = getFactory (auxid);
  if (factory)
    return factory->getEltSize();
  return 0;
}


/**
 * @brief Return size of an element in the STL vector
 *        (external locking).
 * @param lock The registry lock.
 * @param auxid The desired aux data item.
 */
size_t AuxTypeRegistry::getEltSize (lock_t& lock,
                                    SG::auxid_t auxid) const
{
  const SG::IAuxTypeVectorFactory* factory = getFactory (lock, auxid);
  if (factory)
    return factory->getEltSize();
  return 0;
}


/**
 * @brief Copy an element between vectors.
 * @param auxid The aux data item being operated on.
 * @param dst Pointer to the start of the destination vector's data.
 * @param dst_index Index of destination element in the vector.
 * @param src Pointer to the start of the source vector's data.
 * @param src_index Index of source element in the vector.
 *
 * @c dst and @ src can be either the same or different.
 */
void AuxTypeRegistry::copy (SG::auxid_t auxid,
                            void* dst,       size_t dst_index,
                            const void* src, size_t src_index)
{
  const SG::IAuxTypeVectorFactory* factory = getFactory (auxid);
  if (factory)
    factory->copy (dst, dst_index, src, src_index);
}


/**
 * @brief Copy an element between vectors (external locking).
 * @param lock The registry lock.
 * @param auxid The aux data item being operated on.
 * @param dst Pointer to the start of the destination vector's data.
 * @param dst_index Index of destination element in the vector.
 * @param src Pointer to the start of the source vector's data.
 * @param src_index Index of source element in the vector.
 *
 * @c dst and @ src can be either the same or different.
 */
void AuxTypeRegistry::copy (lock_t& lock,
                            SG::auxid_t auxid,
                            void* dst,       size_t dst_index,
                            const void* src, size_t src_index)
{
  const SG::IAuxTypeVectorFactory* factory = getFactory (lock, auxid);
  if (factory)
    factory->copy (dst, dst_index, src, src_index);
}


/**
 * @brief Copy an element between vectors.
 *        Apply any transformations needed for output.
 * @param auxid The aux data item being operated on.
 * @param dst Pointer to the start of the destination vector's data.
 * @param dst_index Index of destination element in the vector.
 * @param src Pointer to the start of the source vector's data.
 * @param src_index Index of source element in the vector.
 *
 * @c dst and @ src can be either the same or different.
 */
void AuxTypeRegistry::copyForOutput (SG::auxid_t auxid,
                                     void* dst,       size_t dst_index,
                                     const void* src, size_t src_index)
{
  const SG::IAuxTypeVectorFactory* factory = getFactory (auxid);
  if (factory) {
    factory->copy (dst, dst_index, src, src_index);

    // It would be cleaner, safer, and more flexible to add a new factory
    // interface for this.  But that would require a full rebuild,
    // which we want to avoid at this point.
    upgrading_lock_t lock (m_mutex);
    if (m_isEL[auxid]) applyELThinning (reinterpret_cast<char*>(dst) + dst_index * factory->getEltSize());
    if (m_isELVec[auxid]) applyELVecThinning (reinterpret_cast<char*>(dst) + dst_index * factory->getEltSize());
  }
}


/**
 * @brief Copy an element between vectors (external locking).
 *        Apply any transformations needed for output.
 * @param lock The registry lock.
 * @param auxid The aux data item being operated on.
 * @param dst Pointer to the start of the destination vector's data.
 * @param dst_index Index of destination element in the vector.
 * @param src Pointer to the start of the source vector's data.
 * @param src_index Index of source element in the vector.
 *
 * @c dst and @ src can be either the same or different.
 */
void AuxTypeRegistry::copyForOutput (lock_t& lock,
                                     SG::auxid_t auxid,
                                     void* dst,       size_t dst_index,
                                     const void* src, size_t src_index)
{
  const SG::IAuxTypeVectorFactory* factory = getFactory (lock, auxid);
  if (factory) {
    factory->copy (dst, dst_index, src, src_index);

    // It would be cleaner, safer, and more flexible to add a new factory
    // interface for this.  But that would require a full rebuild,
    // which we want to avoid at this point.
    if (m_isEL[auxid]) applyELThinning (reinterpret_cast<char*>(dst) + dst_index * factory->getEltSize());
    if (m_isELVec[auxid]) applyELVecThinning (reinterpret_cast<char*>(dst) + dst_index * factory->getEltSize());
  }
}


/**
 * @brief Swap an element between vectors.
 * @param auxid The aux data item being operated on.
 * @param a Pointer to the start of the first vector's data.
 * @param aindex Index of the element in the first vector.
 * @param b Pointer to the start of the second vector's data.
 * @param bindex Index of the element in the second vector.
 *
 * @c a and @ b can be either the same or different.
 */
void AuxTypeRegistry::swap (SG::auxid_t auxid,
                            void* a, size_t aindex,
                            void* b, size_t bindex)
{
  const SG::IAuxTypeVectorFactory* factory = getFactory (auxid);
  if (factory)
    factory->swap (a, aindex, b, bindex);
}


/**
 * @brief Swap an element between vectors (external locking).
 * @param lock The registry lock.
 * @param auxid The aux data item being operated on.
 * @param a Pointer to the start of the first vector's data.
 * @param aindex Index of the element in the first vector.
 * @param b Pointer to the start of the second vector's data.
 * @param bindex Index of the element in the second vector.
 *
 * @c a and @ b can be either the same or different.
 */
void AuxTypeRegistry::swap (lock_t& lock,
                            SG::auxid_t auxid,
                            void* a, size_t aindex,
                            void* b, size_t bindex)
{
  const SG::IAuxTypeVectorFactory* factory = getFactory (lock, auxid);
  if (factory)
    factory->swap (a, aindex, b, bindex);
}


/**
 * @brief Clear an element within a vector.
 * @param auxid The aux data item being operated on.
 * @param dst Pointer to the start of the vector's data.
 * @param dst_index Index of the element in the vector.
 */
void AuxTypeRegistry::clear (SG::auxid_t auxid, void* dst, size_t dst_index)
{
  const SG::IAuxTypeVectorFactory* factory = getFactory (auxid);
  if (factory)
    factory->clear (dst, dst_index);
}


/**
 * @brief Clear an element within a vector (external locking).
 * @param lock The registry lock.
 * @param auxid The aux data item being operated on.
 * @param dst Pointer to the start of the vector's data.
 * @param dst_index Index of the element in the vector.
 */
void AuxTypeRegistry::clear (lock_t& lock,
                             SG::auxid_t auxid, void* dst, size_t dst_index)
{
  const SG::IAuxTypeVectorFactory* factory = getFactory (lock, auxid);
  if (factory)
    factory->clear (dst, dst_index);
}


/**
 * @brief Return the vector factory for a given vector element type.
 * @param ti The type of the vector element.
 *
 * Returns 0 if the type is not known.
 * (Use @c addFactory to add new mappings.)
 */
const IAuxTypeVectorFactory*
AuxTypeRegistry::getFactory (const std::type_info& ti) const
{
  lock_t lock (*this);
  return getFactory (lock, ti);
}


/**
 * @brief Return the vector factory for a given vector element type.
 *        (external locking)
 * @param lock The registry lock.
 * @param ti The type of the vector element.
 *
 * Returns 0 if the type is not known.
 * (Use @c addFactory to add new mappings.)
 */
const IAuxTypeVectorFactory*
AuxTypeRegistry::getFactory (lock_t& /*lock*/,
                             const std::type_info& ti) const
{
  return getFactoryLocked (ti);
}


/**
 * @brief Return the vector factory for a given vector element type.
 *        (external locking)
 * @param lock The registry lock.
 * @param ti The type of the vector element.
 *
 * Returns 0 if the type is not known.
 * (Use @c addFactory to add new mappings.)
 */
const IAuxTypeVectorFactory*
AuxTypeRegistry::getFactory (upgrading_lock_t& /*lock*/,
                             const std::type_info& ti) const
{
  return getFactoryLocked (ti);
}


/**
 * @brief Add a new type -> factory mapping.
 * @param ti Type of the vector element.
 * @param factory The factory instance.  The registry will take ownership.
 *
 * This records that @c factory can be used to construct vectors with
 * an element type of @c ti.  If a mapping already exists, the new
 * factory is discarded, unless the old one is a dynamic factory and
 * the new one isn't, in which case the new replaces the old one.
 */
void AuxTypeRegistry::addFactory (const std::type_info& ti,
                                  IAuxTypeVectorFactory* factory)
{
  upgrading_lock_t lock (m_mutex);
  return addFactory (lock, ti, factory);
}


/**
 * @brief Add a new type -> factory mapping.  (external locking)
 * @param lock The registry lock.
 * @param ti Type of the vector element.
 * @param factory The factory instance.  The registry will take ownership.
 *
 * This records that @c factory can be used to construct vectors with
 * an element type of @c ti.  If a mapping already exists, the new
 * factory is discarded, unless the old one is a dynamic factory and
 * the new one isn't, in which case the new replaces the old one.
 */
void AuxTypeRegistry::addFactory (upgrading_lock_t& lock,
                                  const std::type_info& ti,
                                  IAuxTypeVectorFactory* factory)
{
  lock.upgrade();

  ti_map_t::iterator it = m_factories.find (ti.name());
  if (it != m_factories.end()) {
    if (it->second->isDynamic() && !factory->isDynamic()) {
      // Replacing a dynamic factory with a non-dynamic one.
      // But don't delete the old one, since it might still be referenced.
      // Instead, push it on a vector to remember it so we can delete
      // it later.
      m_oldFactories.push_back (it->second);
      it->second = factory;
    }
    else
      delete factory;
  }
  else
    m_factories[ti.name()] = factory;
}


/**
 * @brief Constructor.
 *
 * Populates the type -> factory mappings for standard C++ types.
 */
AuxTypeRegistry::AuxTypeRegistry()
{
  // Make sure we have factories registered for common C++ types.
#define ADD_FACTORY(T) addFactory(typeid(T), new AuxTypeVectorFactory<T>)
  ADD_FACTORY (bool);
  ADD_FACTORY (char);
  ADD_FACTORY (unsigned char);
  ADD_FACTORY (short);
  ADD_FACTORY (unsigned short);
  ADD_FACTORY (int);
  ADD_FACTORY (unsigned int);
  ADD_FACTORY (long);
  ADD_FACTORY (unsigned long);
  ADD_FACTORY (long long);
  ADD_FACTORY (unsigned long long);
  ADD_FACTORY (float);
  ADD_FACTORY (double);
  ADD_FACTORY (std::string);

  ADD_FACTORY (std::vector<char>);
  ADD_FACTORY (std::vector<unsigned char>);
  ADD_FACTORY (std::vector<int>);
  ADD_FACTORY (std::vector<unsigned int>);
  ADD_FACTORY (std::vector<float>);
  ADD_FACTORY (std::vector<double>);
#undef ADD_FACTORY
}


/**
 * @brief Destructor.
 *
 * Delete factory instances.
 */
AuxTypeRegistry::~AuxTypeRegistry()
{
  ATHCONTAINERS_FOREACH (ti_map_t::value_type& p, m_factories)
    delete p.second;
  ATHCONTAINERS_FOREACH (const IAuxTypeVectorFactory* p, m_oldFactories)
    delete p;
}


/**
 * @brief Look up a name -> @c auxid_t mapping.
 * @param name The name of the aux data item.
 * @param clsname The name of its associated class.  May be blank.
 * @param ti The type of this aux data item.
 * @param makeFactory Function to create a factory for this type, if needed.
 *                    May return 0 if the type is unknown.
 *
 *
 * If the aux data item already exists, check to see if the provided
 * type matches the type that was used before.  If so, then set
 * return the auxid; otherwise, raise @c SG::ExcAuxTypeMismatch.
 *
 * If the aux data item does not already exist, then see if we
 * have a factory registered for this @c type_info.  If not, then
 * call @c makeFactory and use what it returns.  If that returns 0,
 * then fail and return null_auxid.  Otherwise, assign a new auxid
 * and return it.
 */
SG::auxid_t
AuxTypeRegistry::findAuxID (const std::string& name,
                            const std::string& clsname,
                            const std::type_info& ti,
                            IAuxTypeVectorFactory* (AuxTypeRegistry::*makeFactory) () const)
{
  upgrading_lock_t lock (m_mutex);
  key_t key (name, clsname);
  id_map_t::iterator i = m_auxids.find (key);
  if (i != m_auxids.end()) {
    typeinfo_t& m = m_types[i->second];

    // By all rights, these two tests should be redundant.
    // However, there are cases where we see distinct @c type_info objects
    // for the same type.  This is usually associated with dictionaries
    // being loaded `too early,' during python configuration processing.
    // It's a C++ standard violation for this to ever happen, but it's
    // not clear that it's feasible to actually eliminate the possibility
    // of this happening.  So if the @c type_info instances differ,
    // we still accept the match as long as the names are the same.
    if (&ti == m.m_ti || strcmp(ti.name(), m.m_ti->name()) == 0) {
      // Try to upgrade a dynamic factory.
      if (m.m_factory->isDynamic()) {
        lock.upgrade();
        IAuxTypeVectorFactory* fac2 = (*this.*makeFactory)();
        if (fac2) {
          addFactory (lock, ti, fac2);
          m.m_factory = getFactory (lock, ti);
        }
      }
      return i->second;
    }
    if( *m.m_ti != typeid(SG::AuxTypePlaceholder) ) {
       throw SG::ExcAuxTypeMismatch (i->second, ti, *m.m_ti);
    }
    // fall through, get a new auxid and real type info
    // new auxid needed so a new data vector is created in the AuxStore
  }
  lock.upgrade();
  const IAuxTypeVectorFactory* fac = getFactory (lock, ti);
  if (!fac || fac->isDynamic()) {
    IAuxTypeVectorFactory* fac2 = (*this.*makeFactory)();
    if (fac2) {
      addFactory (lock, ti, fac2);
      fac = getFactory (lock, ti);
    }
  }
  if (!fac) return null_auxid;
  SG::auxid_t auxid = m_types.size();
  m_types.resize (auxid+1);
  m_auxids[key] = auxid;
  typeinfo_t& t = m_types[auxid];
  t.m_name = name;
  t.m_clsname = clsname;
  t.m_ti = &ti;
  t.m_factory = fac;

  setELFlags (lock, auxid);
  return auxid;
}


/**
 * @brief Return the vector factory for a given vector element type.
 *        The registry lock must be held.
 * @param ti The type of the vector element.
 *
 * Returns 0 if the type is not known.
 * (Use @c addFactory to add new mappings.)
 */
const IAuxTypeVectorFactory*
AuxTypeRegistry::getFactoryLocked (const std::type_info& ti) const
{
  ti_map_t::const_iterator it = m_factories.find (ti.name());
  if (it != m_factories.end())
    return it->second;
  return 0;
}


/**
 * @brief Initialize the m_isEL* flags for a given variable.
 * @param auxid The variable for which the flags should be initialized.
 * @param lock The registry lock (must be locked).
 *
 * ??? Should go away when we extend the factory interface.
 */
void AuxTypeRegistry::setELFlags (upgrading_lock_t& /*lock*/, auxid_t auxid)
{
  m_isEL.resize (auxid+1);
  m_isELVec.resize (auxid+1);
  std::string tname =  normalizedTypeinfoName (*m_types[auxid].m_ti);
  static const std::string pat1 = "ElementLink<";
  static const std::string pat2 = "std::vector<ElementLink<";
  if (tname.substr (0, pat1.size()) == pat1)
    m_isEL[auxid] = true;
  else if (tname.substr (0, pat2.size()) == pat2)
    m_isELVec[auxid] = true;
}


/**
 * @brief Apply @c ElementLink output transformations to a single element.
 * @param dst Pointer to the element.
 *
 * ??? Should go away when we extend the factory interface.
 */
#ifdef XAOD_STANDALONE
void AuxTypeRegistry::applyELThinning (void*)
{
}
#else
void AuxTypeRegistry::applyELThinning (void* dst)
{
  reinterpret_cast<ElementLinkBase*> (dst)->thin();
}
#endif


/**
 * @brief Apply @c ElementLink output transformations to a vector.
 * @param dst Pointer to the vector.
 *
 * ??? Should go away when we extend the factory interface.
 */
#ifdef XAOD_STANDALONE
void AuxTypeRegistry::applyELVecThinning (void*)
{
}
#else
void AuxTypeRegistry::applyELVecThinning (void* dst)
{
  std::vector<ElementLinkBase>& v = 
    *reinterpret_cast<std::vector<ElementLinkBase>* > (dst);
  size_t sz = v.size();
  for (size_t i = 0; i < sz; i++)
    v[i].thin();
}
#endif


#ifndef XAOD_STANDALONE
/**
 * @brief Declare input renaming requests.
 * @param map Map of (hashed) sgkey -> sgkey for renaming requests.
 * @param pool String pool in which the hashed keys are defined.
 *
 * This is called by @c AddressRemappingSvc when there is a request
 * to rename input objects.  It saves any requests involving renaming
 * of auxiliary variables and makes that information available via
 * @c inputRename.
 */
void
AuxTypeRegistry::setInputRenameMap (const Athena::IInputRename::InputRenameMap_t* map,
                                    const IStringPool& pool)
{
  lock_t lock(*this);
  m_renameMap.clear();
  if (!map) return;
  for (const auto& p : *map) {
    sgkey_t from_sgkey = p.first;
    sgkey_t to_sgkey = p.second;

    const std::string* from_str = pool.keyToString (from_sgkey);
    if (!from_str) continue;
    std::string::size_type from_dpos = from_str->find (".");
    if (from_dpos == std::string::npos || from_dpos == from_str->size()-1) continue;

    const std::string* to_str = pool.keyToString (to_sgkey);
    if (!to_str) continue;
    std::string::size_type to_dpos = to_str->find (".");
    if (to_dpos == std::string::npos || to_dpos == to_str->size()-1) continue;

    m_renameMap[*from_str] = to_str->substr (to_dpos+1, std::string::npos);
  }
}
#endif


/**
 * @brief Check for an input renaming of an auxiliary variable.
 * @brief key The SG key of the object to which the variable is attached.
 * @brief name The name of the variable on the input file.
 *
 * @returns The variable name to use in the transient representation.
 * Will usually be @c name, but may be different if there was a renaming request.
 */
const std::string& AuxTypeRegistry::inputRename (const std::string& key,
                                                 const std::string& name) const
{
  lock_t lock(*this);
  if (m_renameMap.empty())
    return name;

  std::string fullkey = key + "." + name;
  renameMap_t::const_iterator it = m_renameMap.find (fullkey);
  if (it != m_renameMap.end())
    return it->second;
  return name;
}


} // namespace SG


