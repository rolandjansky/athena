// This file's extension implies that it's C, but it's really -*- C++ -*-.
// $Id: unordered_map.h,v 1.1.1.1 2008-09-10 04:02:52 binet Exp $
/**
 * @file CxxUtils/unordered_map.h
 * @author scott snyder <snyder@bnl.gov>, copied from gcc4.
 * @date Apr, 2007
 * @brief This is the TR1 unordered_set implementation from gcc4,
 *        adapted to build in Atlas.  Once the TR1 library is available
 *        on all our platforms, we can switch to using the system-supplied
 *        version instead.
 *
 *        Search for `sss' to find changes from the gcc version.
 */

// TR1 unordered_map -*- C++ -*-

// Copyright (C) 2005 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

/*  @file  (sss --- hide from doxygen)
 *  This is a TR1 C++ Library header. 
 */

#ifndef CXXUTILS_UNORDERED_MAP_H // sss GNU_LIBSTDCXX_TR1_UNORDERED_MAP_
#define CXXUTILS_UNORDERED_MAP_H // sss GNU_LIBSTDCXX_TR1_UNORDERED_MAP_

#include "CxxUtils/hashtable.h" // sss <tr1/hashtable>
//#include <tr1/functional> sss
//#include <tr1/functional> sss
#include <utility>
#include <memory>
#include <functional>

//namespace std     sss
//{                 sss
namespace SG // tr1 sss
{
#define Internal CxxUtils_Internal // sss
  // XXX When we get typedef templates these class definitions
  // will be unnecessary.

  template<class Key, class T,
	   class Hash = hash<Key>,
	   class Pred = std::equal_to<Key>,
	   class Alloc = std::allocator<std::pair<const Key, T> >,
	   bool cache_hash_code = false>
    class unordered_map
    : public hashtable <Key, std::pair<const Key, T>,
			Alloc,
			Internal::extract1st<std::pair<const Key, T> >, Pred,
			Hash, Internal::mod_range_hashing,
			Internal::default_ranged_hash,
			Internal::prime_rehash_policy,
			cache_hash_code, false, true>
    {
      typedef hashtable <Key, std::pair<const Key, T>,
			 Alloc,
			 Internal::extract1st<std::pair<const Key, T> >, Pred,
			 Hash, Internal::mod_range_hashing,
			 Internal::default_ranged_hash,
			 Internal::prime_rehash_policy,
			 cache_hash_code, false, true>
        Base;

    public:
      typedef typename Base::size_type size_type;
      typedef typename Base::hasher hasher;
      typedef typename Base::key_equal key_equal;
      typedef typename Base::allocator_type allocator_type;

      explicit
      unordered_map(size_type n = 10,
		    const hasher& hf = hasher(),
		    const key_equal& eql = key_equal(),
		    const allocator_type& a = allocator_type())
      : Base(n, hf, Internal::mod_range_hashing(),
	     Internal::default_ranged_hash(),
	     eql, Internal::extract1st<std::pair<const Key, T> >(), a)
      { }

      template<typename InputIterator>
        unordered_map(InputIterator f, InputIterator l, 
		      size_type n = 10,
		      const hasher& hf = hasher(), 
		      const key_equal& eql = key_equal(), 
		      const allocator_type& a = allocator_type())
	: Base (f, l, n, hf, Internal::mod_range_hashing(),
		Internal::default_ranged_hash(),
		eql, Internal::extract1st<std::pair<const Key, T> >(), a)
	{ }
    };
  
  template<class Key, class T,
	   class Hash = hash<Key>,
	   class Pred = std::equal_to<Key>,
	   class Alloc = std::allocator<std::pair<const Key, T> >,
	   bool cache_hash_code = false>
    class unordered_multimap
    : public hashtable <Key, std::pair<const Key, T>,
			Alloc,
			Internal::extract1st<std::pair<const Key, T> >, Pred,
			Hash, Internal::mod_range_hashing,
			Internal::default_ranged_hash,
			Internal::prime_rehash_policy,
			cache_hash_code, false, false>
    {
      typedef hashtable <Key, std::pair<const Key, T>,
			 Alloc,
			 Internal::extract1st<std::pair<const Key, T> >, Pred,
			 Hash, Internal::mod_range_hashing,
			 Internal::default_ranged_hash,
			 Internal::prime_rehash_policy,
			 cache_hash_code, false, false>
        Base;

    public:
      typedef typename Base::size_type size_type;
      typedef typename Base::hasher hasher;
      typedef typename Base::key_equal key_equal;
      typedef typename Base::allocator_type allocator_type;
      
      explicit
      unordered_multimap(size_type n = 10,
			 const hasher& hf = hasher(),
			 const key_equal& eql = key_equal(),
			 const allocator_type& a = allocator_type())
      : Base (n, hf, Internal::mod_range_hashing(),
	      Internal::default_ranged_hash(),
	      eql, Internal::extract1st<std::pair<const Key, T> >(), a)
      { }


      template<typename InputIterator>
        unordered_multimap(InputIterator f, InputIterator l, 
			   typename Base::size_type n = 0,
			   const hasher& hf = hasher(), 
			   const key_equal& eql = key_equal(), 
			   const allocator_type& a = allocator_type())
	: Base (f, l, n, hf, Internal::mod_range_hashing(),
		Internal::default_ranged_hash(),
		eql, Internal::extract1st<std::pair<const Key, T> >(), a)
        { }
    };

  template<class Key, class T, class Hash, class Pred, class Alloc,
	   bool cache_hash_code>
    inline void
    swap(unordered_map<Key, T, Hash, Pred, Alloc, cache_hash_code>& x,
	 unordered_map<Key, T, Hash, Pred, Alloc, cache_hash_code>& y)
    { x.swap(y); }

  template<class Key, class T, class Hash, class Pred, class Alloc,
	   bool cache_hash_code>
    inline void
    swap(unordered_multimap<Key, T, Hash, Pred, Alloc, cache_hash_code>& x,
	 unordered_multimap<Key, T, Hash, Pred, Alloc, cache_hash_code>& y)
    { x.swap(y); }

#undef Internal // sss
//}  sss
}

#endif /* GNU_LIBSTDCXX_TR1_UNORDERED_MAP_ */
