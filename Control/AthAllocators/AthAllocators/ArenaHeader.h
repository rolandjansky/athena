// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHeader.h 470529 2011-11-24 23:54:22Z ssnyder $

/**
 * @file  AthAllocators/ArenaHeader.h
 * @author scott snyder
 * @date May 2007
 * @brief Proxy for a group of Arenas.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */


#ifndef ATLALLOCATORS_ARENAHEADER_H
#define ATLALLOCATORS_ARENAHEADER_H


#include <vector>
#include <cstdlib>
#include <string>
#include <iosfwd>
#include <mutex>
#include "boost/thread/tss.hpp"


namespace SG {


class ArenaAllocatorBase;
class ArenaBase;


/**
 * @brief Proxy for a group of Arenas.
 *        See Arena.h for an overview of the arena-based memory allocators.
 *
 * A Header collects a group of Arenas.  One of these is the current Arena,
 * in which memory operations will take place.  We can also generate
 * a report of memory usage from all the Arenas in the group.
 *
 * This is also where we handle the mapping from indices to Allocator
 * instances.  This is done simply with a vector of Allocator pointers.
 * Each Arena has such a vector.  We keep a pointer to one of these;
 * that's what defines the notion of the current Arena.
 */
class ArenaHeader
{
public:
  /// Vector of pointers to Allocator instances.
  typedef std::vector<ArenaAllocatorBase*> ArenaAllocVec_t;


  /**
   * @brief Constructor.
   */
  ArenaHeader();


  /**
   * @brief Destructor.
   *
   * This will clean up any memory allocated in the default Arena.
   */
  ~ArenaHeader();


  /**
   * @brief Translate an integer index to an Allocator index.
   * @param i The index to look up.
   *
   * If the index isn't valid, an assertion will be tripped.
   */
  ArenaAllocatorBase* allocator (size_t i);


  /**
   * @brief Set the current Arena.
   * @param allocvec New vector of Allocator instances.
   * @return The previous vector.
   *
   * This sets the notion of the current Arena.
   */
  ArenaAllocVec_t* setAllocVec (ArenaAllocVec_t* allocvec);


  /**
   * @brief Add a new Arena to the group.
   * @param a The Arena to add.
   */
  void addArena (ArenaBase* a);


  /**
   * @brief Remove an Arena from the group.
   * @param a The Arena to remove.
   *
   * Will trip an assertion if the Arena is not in the group.
   */
  void delArena (ArenaBase* a);


  /**
   * @brief Generate a report of all Arenas in the group.
   * @param os Stream to which to send a report.
   */
  void report (std::ostream& os) const;


  /**
   * @brief Generate a report of all Arenas in the group, and return
   *        the result as a string.
   *
   * We have this in addition to @c report() in order to make it easier
   * to call from scripting languages.
   */
  std::string reportStr () const;


  /**
   * @brief Call @c reset on all Allocators in the current Arena.
   *
   * All elements allocated are returned to the free state.
   * @c clear should be called on them if it was provided.
   * The elements may continue to be cached internally, without
   * returning to the system.
   */
  void reset();


  /**
   * @brief Return the global default Header instance.
   */
  static ArenaHeader* defaultHeader();


private:
  /**
   * @brief Make a new Allocator for index i.
   * @param i The index of the Allocator.
   *
   * The Allocator vector was empty for index @c i.  Make an appropriate
   * new Allocator, store it in the vector, and return it.  Will trip
   * an assertion if the index is not valid.
   */
  ArenaAllocatorBase* makeAllocator (size_t i);


  /// Current vector of Allocators.
  boost::thread_specific_ptr<ArenaAllocVec_t> m_allocvec;

  /// Vector of Allocators which are owned by this object.
  /// This constitutes the default Arena.
  ArenaAllocVec_t* m_ownedAllocvec;

  /// List of all Arenas in our group.
  std::vector<ArenaBase*>  m_arenas;

  /// Mutex to protect access to m_ownedAllocvec and m_arenas.
  mutable std::mutex m_mutex;
};


} // namespace SG


#include "AthAllocators/ArenaHeader.icc"



#endif // not ATLALLOCATORS_ARENAHEADER_H

