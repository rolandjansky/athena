/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHALLOCATORS_DATAPOOL_H
#define ATHALLOCATORS_DATAPOOL_H
/** @class DataPool
 * @brief  a typed memory pool that saves time spent
 *         allocation small object. This is typically used
 *         by container such as DataVector and DataList
 *
 * Traditionally, these objects were declared as static, even after that
 * was no longer needed due to the rewrite which based them on the Allocator
 * classes.  However, declaring @c DataPool instances as static will
 * cause thread-safety problems, and thus should no longer be done.
 *
 * Be aware that a DataPool holds a lock on the underlying allocator.
 * Therefore, if you try to create two pool instances referencing
 * the same allocator (i.e, same type and same thread), you'll get a deadlock.
 *
 * @author Srini Rajagopalan - ATLAS Collaboration
 *$Id: DataPool.h 470529 2011-11-24 23:54:22Z ssnyder $	
 */

#include "AthAllocators/ArenaCachingHandle.h"
#include "AthAllocators/ArenaPoolAllocator.h"
#include <string>
#include "boost/iterator/iterator_adaptor.hpp"

template <typename VALUE>
class DataPool
{
private:
  typedef SG::ArenaPoolAllocator alloc_t;
  typedef SG::ArenaCachingHandle<VALUE, alloc_t> handle_t;
  
public:
  typedef typename handle_t::pointer pointer;
  typedef size_t size_type;

  class const_iterator;

  class iterator
    : public boost::iterator_adaptor<
    iterator,
    typename handle_t::iterator,
    VALUE *,
    boost::forward_traversal_tag,
    VALUE *>
  {
  public:
    iterator (const typename handle_t::iterator& it)
      : iterator::iterator_adaptor_ (it)
    {
    }

    friend class const_iterator;

  private:
    friend class boost::iterator_core_access;

    typename iterator::reference dereference() const
    { return &*this->base_reference(); }
  };

  class const_iterator
    : public boost::iterator_adaptor<
    const_iterator,
    typename handle_t::const_iterator,
    VALUE const *,
    boost::forward_traversal_tag,
    VALUE const *>
  {
  public:
    const_iterator (const typename handle_t::const_iterator& it)
      : const_iterator::iterator_adaptor_ (it)
    {
    }

    const_iterator (const iterator& it)
      : const_iterator::iterator_adaptor_ (it.base_reference())
    {
    }

  private:
    friend class boost::iterator_core_access;

    typename const_iterator::reference dereference() const
    { return &*this->base_reference(); }
  };


  //////////////////////////////////////////////////////////////////////
  /// Constructors:
  //////////////////////////////////////////////////////////////////////

  DataPool(size_type n = 0);

  DataPool(const EventContext& ctx,
           size_type n = 0);
 
  DataPool(SG::Arena* arena,
           size_type n = 0);
 
  ///////////////////////////////////////////////////////

  /// release all elements in the pool.
  void reset();

  /// free all memory in the pool.
  void erase();
  
  void reserve(unsigned int size);

  /// return capacity of pool OK
  unsigned int capacity();

  /// return size already allocated OK
  unsigned int allocated();

  /// begin iterators over pool
  iterator begin();
  const_iterator begin() const;

  /// the end() method will allow looping over only valid elements
  /// and not over ALL elements of the pool 
  iterator end();
  const_iterator end() const;
 
  /// obtain the next available element in pool by pointer
  /// pool is resized if its limit has been reached
  /// This may be faster than calling the constructors with 
  /// the following two methods. But one must be sure to 
  /// completely reset each object since it has values set in the 
  /// previous event.
  pointer nextElementPtr(); 

  /// obtain the next available element in pool by pointer
  /// return as void* to minimize misuse, client usage is:
  /// MyElement* m = new(pool->mem) MyElement(...); // pool is ptr
  /// DANGEROUS --- do we need this???
  void* mem();

  /// can also say: 
  /// MyElement* m = new ((*pool)()) MyElement(...); // pool = pointer
  /// MyElement* m = new (pool()) MyElement(...); // pool = value
  /// DANGEROUS --- do we need this???
  void *operator ()();

   /// typename of pool
   static const std::string& typeName();

//-----------------------------------------------------------//

 private:

   handle_t m_handle;

   const static typename alloc_t::Params s_params;

  /// minimum number of elements in pool
  static constexpr size_t s_minRefCount = 1024;
};


#include "AthAllocators/DataPool.icc"

#endif


