// This file's extension implies that it's C, but it's really -*- C++ -*-.
// $Id: unordered_set.h,v 1.1.1.1 2008-09-10 04:02:52 binet Exp $
/**
 * @file CxxUtils/unordered_set.h
 * @author scott snyder <snyder@bnl.gov>, copied from gcc4.
 * @date Apr, 2007
 * @brief This is the TR1 unordered_set implementation from gcc4,
 *        adapted to build in Atlas.  Once the TR1 library is available
 *        on all our platforms, we can switch to using the system-supplied
 *        version instead.
 *
 *        Search for `sss' to find changes from the gcc version.
 */

// TR1 unordered_set -*- C++ -*-

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

/*  @file (sss --- hide from doxygen)
 *  This is a TR1 C++ Library header. 
 */

#ifndef CXXUTILS_UNORDERED_SET_H // sss GNU_LIBSTDCXX_TR1_UNORDERED_SET_
#define CXXUTILS_UNORDERED_SET_H // sss GNU_LIBSTDCXX_TR1_UNORDERED_SET_

#include "CxxUtils/hashtable.h" // <tr1/hashtable>  sss
//#include <tr1/functional>
#include <memory>
#include <functional>

//namespace std     sss
//{                 sss
namespace SG // tr1 sss
{
#define Internal CxxUtils_Internal // sss

  // XXX When we get typedef templates these class definitions
  // will be unnecessary.

  template<class Value,
	   class Hash = hash<Value>,
	   class Pred = std::equal_to<Value>,
	   class Alloc = std::allocator<Value>,
	   bool cache_hash_code = false>
    class unordered_set
    : public hashtable<Value, Value, Alloc,
		       Internal::identity<Value>, Pred,
		       Hash, Internal::mod_range_hashing,
		       Internal::default_ranged_hash,
		       Internal::prime_rehash_policy,
		       cache_hash_code, true, true>
    {
      typedef hashtable<Value, Value, Alloc,
			Internal::identity<Value>, Pred,
			Hash, Internal::mod_range_hashing,
			Internal::default_ranged_hash,
			Internal::prime_rehash_policy,
			cache_hash_code, true, true>
        Base;

    public:
      typedef typename Base::size_type size_type;
      typedef typename Base::hasher hasher;
      typedef typename Base::key_equal key_equal;
      typedef typename Base::allocator_type allocator_type;
      
      explicit
      unordered_set(size_type n = 10,
		    const hasher& hf = hasher(),
		    const key_equal& eql = key_equal(),
		    const allocator_type& a = allocator_type())
      : Base (n, hf, Internal::mod_range_hashing(),
	      Internal::default_ranged_hash(),
	      eql, Internal::identity<Value>(), a)
      { }

      template<typename InputIterator>
        unordered_set(InputIterator f, InputIterator l, 
		      size_type n = 10,
		      const hasher& hf = hasher(), 
		      const key_equal& eql = key_equal(), 
		      const allocator_type& a = allocator_type())
	: Base (f, l, n, hf, Internal::mod_range_hashing(),
		Internal::default_ranged_hash(),
		eql, Internal::identity<Value>(), a)
        { }
    };

  template<class Value,
	   class Hash = hash<Value>,
	   class Pred = std::equal_to<Value>,
	   class Alloc = std::allocator<Value>,
	   bool cache_hash_code = false>
    class unordered_multiset
    : public hashtable <Value, Value, Alloc,
			Internal::identity<Value>, Pred,
			Hash, Internal::mod_range_hashing,
			Internal::default_ranged_hash,
			Internal::prime_rehash_policy,
			cache_hash_code, true, false>
    {
      typedef hashtable<Value, Value, Alloc,
			Internal::identity<Value>, Pred,
			Hash, Internal::mod_range_hashing,
			Internal::default_ranged_hash,
			Internal::prime_rehash_policy,
			cache_hash_code, true, false>
        Base;

    public:
      typedef typename Base::size_type size_type;
      typedef typename Base::hasher hasher;
      typedef typename Base::key_equal key_equal;
      typedef typename Base::allocator_type allocator_type;
      
      explicit
      unordered_multiset(size_type n = 10,
			 const hasher& hf = hasher(),
			 const key_equal& eql = key_equal(),
			 const allocator_type& a = allocator_type())
      : Base (n, hf, Internal::mod_range_hashing(),
	      Internal::default_ranged_hash(),
	      eql, Internal::identity<Value>(), a)
      { }


      template<typename InputIterator>
        unordered_multiset(InputIterator f, InputIterator l, 
			   typename Base::size_type n = 0,
			   const hasher& hf = hasher(), 
			   const key_equal& eql = key_equal(), 
			   const allocator_type& a = allocator_type())
	: Base (f, l, n, hf, Internal::mod_range_hashing(),
		Internal::default_ranged_hash(), eql,
		Internal::identity<Value>(), a)
        { }
    };

  template<class Value, class Hash, class Pred, class Alloc,
	   bool cache_hash_code>
    inline void
    swap (unordered_set<Value, Hash, Pred, Alloc, cache_hash_code>& x,
	  unordered_set<Value, Hash, Pred, Alloc, cache_hash_code>& y)
    { x.swap(y); }

  template<class Value, class Hash, class Pred, class Alloc,
	   bool cache_hash_code>
    inline void
    swap(unordered_multiset<Value, Hash, Pred, Alloc, cache_hash_code>& x,
	 unordered_multiset<Value, Hash, Pred, Alloc, cache_hash_code>& y)
   { x.swap(y); }

#undef Internal // sss
//}  sss
}

#endif /* GNU_LIBSTDCXX_TR1_UNORDERED_SET_ */
