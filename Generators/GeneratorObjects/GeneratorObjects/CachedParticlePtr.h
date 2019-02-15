// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file GeneratorObjects/CachedParticlePtr.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2019
 * @brief Hold overloaded store/particle pointer.
 */


#ifndef GENERATOROBJECTS_CACHEDPARTICLEPTR_H
#define GENERATOROBJECTS_CACHEDPARTICLEPTR_H


#include "SGTools/CurrentEventStore.h"
#include <atomic>
#include <cassert>


namespace HepMC {
  class GenParticle;
}
class IProxyDict;


namespace GeneratorObjects {


/** 
 * @brief Hold overloaded store/particle pointer.
 *
 * When we initialize a HepMcParticleLink, we need to save a reference
 * to the current event store.  We use this to look up the particle container
 * when the link is dereferenced.  After that, we no longer need
 * the store pointer.  So to save space, we overload a single pointer
 * to store both of them.  We distinguish the two by setting the low bit
 * if an IProxyDict* is stored.  We allow the value to be atomically
 * changed from an IProxyDict* to a GenParticle*.  This is safe to do
 * concurrently as long as we always get the same GenParticle*.
 */     
class CachedParticlePtr
{
public:
  /**
   * @brief Default constructor.
   * Initializes to a reference the current event store.
   */
  CachedParticlePtr();


  /**
   * @brief Constructor referencing a specific event store.
   * @param sg The store to reference, as an IProxyDict*.
   */
  CachedParticlePtr (const IProxyDict* sg);


  /**
   * @brief Constructor referencing a specific particle.
   * @param part The particle to reference.
   */
  CachedParticlePtr (const HepMC::GenParticle* part);


  /**
   * @brief Copy constructor.  (Can't be defaulted due to the atomic member.)
   */
  CachedParticlePtr (const CachedParticlePtr& other);


  /**
   * @brief Assignment.  (Can't be defaulted due to the atomic member.)
   */
  CachedParticlePtr& operator= (const CachedParticlePtr& other);


  /**
   * @brief Reset to reference a new event store.
   * @param sg The store to reference, as an IProxyDict*.
   *
   * Not compatible with any other concurrent access.
   */
  void store (const IProxyDict* sg);


  /**
   * @brief Retrieve the current value.
   * @param sg[out] Referenced event store.
   *
   * If this pointer is referencing an event store, set @c sg to that 
   * and return nullptr.
   * Otherwise, we're referencing a particular particle.
   * Return that and set @c sg to nullptr.
   */
  const HepMC::GenParticle* get (IProxyDict const* & sg) const;


  /**
   * @brief Change the pointer to refer to a particle rather than a store.
   * @param sg Store the pointer is currently referencing.
   * @param part Particle the pointer should be referencing.
   *
   * It is ok to call this concurrently, as long as each call uses
   * the same parameters.
   */
  void set (const IProxyDict* sg, const HepMC::GenParticle* part) const;


private:
  /**
   * @brief Convert a store pointer to a void* stored value.
   * @param sg IProxyDict* to reference.
   *
   * Sets the low bit and converts to void*.
   */
  static const void* storedict (const IProxyDict* sg);


  /**
   * @brief Test if a stored value references an event store.
   * @param p The value to test.
   *
   * Tests if the low bit is set.
   */
  static bool isdict (const void* p);


  /** 
   * @brief Convert a stored value void* to an IProxyDict*.
   * @param Clears the low bit and does pointer conversion.
   */
  static const IProxyDict* getdict (const void* p);


  /// The stored pointer: either an IProxyDict* or a GenParticle*.
  /// They are distinguished by setting the low bit for an IProxyDict*.
  mutable std::atomic<const void*> m_ptr;
};


} // namespace GeneratorObjects


#include "GeneratorObjects/CachedParticlePtr.icc"


#endif // not GENERATOROBJECTS_CACHEDPARTICLEPTR_H
