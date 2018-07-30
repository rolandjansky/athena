/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  AthAllocators/src/ArenaAllocatorRegistry.cxx
 * @author scott snyder
 * @date May 2007
 * @brief Registry of allocator factories.
 *        Out-of-line implementation.
 */


#include "AthAllocators/ArenaAllocatorRegistry.h"
#include "AthAllocators/ArenaAllocatorCreator.h"
#include "AthAllocators/ArenaAllocatorBase.h"
#include "CxxUtils/checker_macros.h"
#include <vector>
#include <map>
#include <mutex>
#include <cassert>


namespace SG {


/**
 * @brief ArenaAllocatorRegistry implementation class.
 */
class ArenaAllocatorRegistryImpl
{
public:
  /// Destructor.
  ~ArenaAllocatorRegistryImpl();


  /**
   * @brief Register a new allocator type.
   * @param name The name of the allocator type.
   * @param creator The factory object to create instances of this type.
   *                The registry takes ownership of this pointer.
   * @return The new integer index for this allocator type.
   *
   * If the allocator type already exists, then the index of the existing
   * one is returned (and the object passed as @c creator is deleted).
   */
  size_t registerCreator (const std::string& name,
                          std::unique_ptr<ArenaAllocatorCreator> creator);


  /**
   * @brief Look up the index for an allocator type name.
   * @param name The name of the allocator type to find.
   * @return The index corresponding to the type, or @c std::string::npos
   *         if it hasn't yet been registered.
   */
  size_t lookup (const std::string& name);


  /**
   * @brief Create a new instance of an allocator.
   * @param i The index of the allocator to create.
   * @return A newly-allocated allocator instance.
   */
  std::unique_ptr<ArenaAllocatorBase> create (size_t i);


private:
  /// Map from names to indices.
  typedef std::map<std::string, size_t> map_t;
  map_t m_map;

  /// Vector of factory instances.
  std::vector<std::unique_ptr<ArenaAllocatorCreator> > m_creators;

  /// Mutex to protect the contents.
  std::mutex m_mutex;

  typedef std::lock_guard<std::mutex> lock_t;
};


/**
 * @brief Destructor.
 */
ArenaAllocatorRegistryImpl::~ArenaAllocatorRegistryImpl()
{
}


/**
 * @brief Register a new allocator type.
 * @param name The name of the allocator type.
 * @param creator The factory object to create instances of this type.
 *                The registry takes ownership of this pointer.
 * @return The new integer index for this allocator type.
 *
 * If the allocator type already exists, then the index of the existing
 * one is returned (and the object passed as @c creator is deleted).
 */
size_t
ArenaAllocatorRegistryImpl::registerCreator (const std::string& name,
                                             std::unique_ptr<ArenaAllocatorCreator> creator)
{
  lock_t lock (m_mutex);

  // See if there's an existing one by this name.
  // Shouldn't usually happen, since we check that the allocator doesn't
  // exist before calling this, but could happen in a MT job.
  // So we need to check it again with the lock held.
  map_t::iterator it = m_map.find (name);
  if (it != m_map.end()) {
    // creator will be deleted.
    return it->second;
  }

  // The new index.
  size_t i = m_creators.size();

  // Remember the index and store the creator.
  m_map[name] = i;
  m_creators.push_back (std::move (creator));
  return i;
}


/**
 * @brief Look up the index for an allocator type name.
 * @param name The name of the allocator type to find.
 * @return The index corresponding to the type, or @c std::string::npos
 *         if it hasn't yet been registered.
 */
size_t ArenaAllocatorRegistryImpl::lookup (const std::string& name)
{
  lock_t lock (m_mutex);
  map_t::iterator it = m_map.find (name);
  if (it == m_map.end()) {
    return std::string::npos;
  }
  return it->second;
}


/**
 * @brief Create a new instance of an allocator.
 * @param i The index of the allocator to create.
 * @return A newly-allocated allocator instance.
 */
std::unique_ptr<ArenaAllocatorBase>
ArenaAllocatorRegistryImpl::create (size_t i)
{
  lock_t lock (m_mutex);
  assert (i < m_creators.size());
  return m_creators[i]->create();
}


//==========================================================================

/**
 * @brief Register a new allocator type.
 * @param name The name of the allocator type.
 * @param creator The factory object to create instances of this type.
 *                The registry takes ownership of this pointer.
 * @return The new integer index for this allocator type.
 *
 * If the allocator type already exists, then the index of the existing
 * one is returned (and the object passed as @c creator is deleted).
 */
size_t
ArenaAllocatorRegistry::registerCreator (const std::string& name,
                                         std::unique_ptr<ArenaAllocatorCreator> creator)
{
  return m_impl->registerCreator (name, std::move (creator));
}


/**
 * @brief Look up the index for an allocator type name.
 * @param name The name of the allocator type to find.
 * @return The index corresponding to the type, or @c std::string::npos
 *         if it hasn't yet been registered.
 */
size_t ArenaAllocatorRegistry::lookup (const std::string& name)
{
  return m_impl->lookup (name);
}


/**
 * @brief Create a new instance of an allocator.
 * @param i The index of the allocator to create.
 * @return A newly-allocated allocator instance.
 */
std::unique_ptr<ArenaAllocatorBase> ArenaAllocatorRegistry::create (size_t i)
{
  return m_impl->create (i);
}


/**
 * @brief Return a pointer to the global @c ArenaAllocatorRegistry instance.
 */
ArenaAllocatorRegistry* ArenaAllocatorRegistry::instance()
{
  static ArenaAllocatorRegistry tmp ATLAS_THREAD_SAFE;
  return &tmp;
}


/**
 * @brief Constructor.
 */
ArenaAllocatorRegistry::ArenaAllocatorRegistry()
  : m_impl (std::make_unique<ArenaAllocatorRegistryImpl>())
{
}


/**
 * @brief Destructor.
 */
ArenaAllocatorRegistry::~ArenaAllocatorRegistry()
{
}


} // namespace SG
