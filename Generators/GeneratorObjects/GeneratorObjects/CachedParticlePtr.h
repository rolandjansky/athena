// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file GeneratorObjects/CachedParticlePtr.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2019
 * @brief Thread-safe cached particle pointer.
 */


#ifndef GENERATOROBJECTS_CACHEDPARTICLEPTR_H
#define GENERATOROBJECTS_CACHEDPARTICLEPTR_H


#include "AtlasHepMC/GenParticle.h"
#include "CxxUtils/checker_macros.h"
#include <atomic>
#include <cassert>


// std::atomic<std::shared_ptr<...> > is only for c++20.
// Earlier, one needed to use the standalone atomic functions
// for shared_ptr --- but those were deprecated in c++20.
#if defined(HEPMC3) && __cpp_lib_atomic_shared_ptr < 201711L
# define HEPMCPARTICLELINK_USE_STANDALONE_ATOMIC 1
#else
# define HEPMCPARTICLELINK_USE_STANDALONE_ATOMIC 0
#endif


namespace GeneratorObjects {


/** 
 * @brief Thread-safe cached particle pointer.
 *
 * This holds the cached particle pointer for a HepMcParticleLink,
 * allowing for atomic updates.
 *
 * Keep in mind that for HepMC3, the pointer that we cache is actually
 * a std::shared_ptr, so we must be a bit careful.
 */     
class CachedParticlePtr
{
public:
  /**
   * @brief Default constructor.
   * Initializes to a null pointer.
   */
  CachedParticlePtr();


  /**
   * @brief Constructor referencing a specific particle.
   * @param part The particle to reference.
   */
  CachedParticlePtr (HepMC::ConstGenParticlePtr part);


  /**
   * @brief Copy constructor.  (Can't be defaulted due to the atomic member.)
   */
  CachedParticlePtr (const CachedParticlePtr& other);


  /**
   * @brief Assignment.  (Can't be defaulted due to the atomic member.)
   */
  CachedParticlePtr& operator= (const CachedParticlePtr& other);


  /**
   * @brief Reset the pointer to null.
   */
  void reset();


  /**
   * @brief Retrieve the current value.
   */
  HepMC::ConstGenParticlePtr get() const;


  /**
   * @brief Set the cached value of the pointer.
   * @param part Particle the pointer should be referencing.
   *
   * It is ok to call this concurrently, as long as each call uses
   * the same parameters.
   */
  void set (HepMC::ConstGenParticlePtr part) const;


  /**
   * @brief Check if the cached pointer is valid
   */
  bool isValid () const;


private:
  /// The stored pointer.
#if HEPMCPARTICLELINK_USE_STANDALONE_ATOMIC
  mutable HepMC::ConstGenParticlePtr m_ptr ATLAS_THREAD_SAFE;
#else
  mutable std::atomic<HepMC::ConstGenParticlePtr> m_ptr;
#endif
};


} // namespace GeneratorObjects


#include "GeneratorObjects/CachedParticlePtr.icc"


#endif // not GENERATOROBJECTS_CACHEDPARTICLEPTR_H
