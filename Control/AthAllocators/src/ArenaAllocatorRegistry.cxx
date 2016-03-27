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

// xxx FIXME: not thread-safe.

#include "AthAllocators/ArenaAllocatorRegistry.h"
#include "AthAllocators/ArenaAllocatorCreator.h"
#include <vector>
#include <map>
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
   * @param name The name of the allocator type.  Must not already exist.
   * @param creator The factory object to create instances of this type.
   *                The registry takes ownership of this pointer.
   * @return The new integer index for this allocator type.
   */
  size_t registerCreator (const std::string& name,
                          ArenaAllocatorCreator* creator);


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
  ArenaAllocatorBase* create (size_t i);


private:
  /// Map from names to indices.
  typedef std::map<std::string, size_t> map_t;
  map_t m_map;

  /// Vector of factory instances.
  std::vector<ArenaAllocatorCreator*> m_creators;
};


/**
 * @brief Destructor.
 */
ArenaAllocatorRegistryImpl::~ArenaAllocatorRegistryImpl()
{
  // Free the saved factory instances.
  for (size_t i = 0; i < m_creators.size(); i++)
    delete m_creators[i];
}


/**
 * @brief Register a new allocator type.
 * @param name The name of the allocator type.  Must not already exist.
 * @param creator The factory object to create instances of this type.
 *                The registry takes ownership of this pointer.
 * @return The new integer index for this allocator type.
 */
size_t
ArenaAllocatorRegistryImpl::registerCreator (const std::string& name,
                                             ArenaAllocatorCreator* creator)
{
  // The name must not already exist.
  assert (m_map.count (name) == 0);

  // The new index.
  size_t i = m_creators.size();

  // Remember the index and store the creator.
  m_map[name] = i;
  m_creators.push_back (creator);
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
  map_t::iterator it = m_map.find (name);
  if (it == m_map.end())
    return std::string::npos;
  return it->second;
}


/**
 * @brief Create a new instance of an allocator.
 * @param i The index of the allocator to create.
 * @return A newly-allocated allocator instance.
 */
ArenaAllocatorBase* ArenaAllocatorRegistryImpl::create (size_t i)
{
  assert (i < m_creators.size());
  return m_creators[i]->create();
}


//==========================================================================

/**
 * @brief Register a new allocator type.
 * @param name The name of the allocator type.  Must not already exist.
 * @param creator The factory object to create instances of this type.
 *                The registry takes ownership of this pointer.
 * @return The new integer index for this allocator type.
 */
size_t
ArenaAllocatorRegistry::registerCreator (const std::string& name,
                                         ArenaAllocatorCreator* creator)
{
  return m_impl->registerCreator (name, creator);
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
ArenaAllocatorBase* ArenaAllocatorRegistry::create (size_t i)
{
  return m_impl->create (i);
}


/**
 * @brief Return a pointer to the global @c ArenaAllocatorRegistry instance.
 */
ArenaAllocatorRegistry* ArenaAllocatorRegistry::instance()
{
  static ArenaAllocatorRegistry tmp;
  return &tmp;
}


/**
 * @brief Constructor.
 */
ArenaAllocatorRegistry::ArenaAllocatorRegistry()
  : m_impl (new ArenaAllocatorRegistryImpl)
{
}


/**
 * @brief Destructor.
 */
ArenaAllocatorRegistry::~ArenaAllocatorRegistry()
{
  delete m_impl;
}


} // namespace SG
