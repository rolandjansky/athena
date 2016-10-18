#undef NDEBUG
/**
 * @file CxxUtils/test/hashtable_test.cxx
 * @author scott snyder <snyder@bnl.gov>, copied from gcc4.
 * @date Apr, 2007
 * @brief Regression tests for TR1 hashtable class.  Adapted from
 *        the tests in gcc4.
 */

// Copyright (C) 2005 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

// 6.3 Unordered associative containers

#include "CxxUtils/unordered_set.h"
#include "CxxUtils/unordered_map.h"
#include <string>
#include <cassert>

#define VERIFY assert

  template<template<typename, typename> class Relationship,
           typename Type1, typename Type2>
    bool
    test_relationship(bool value)
    {
      bool ret = true;
      ret &= Relationship<Type1, Type2>::value == value;
      ret &= Relationship<Type1, Type2>::type::value == value;
      return ret;
    }

  /// @brief  relationships between types [4.6].
  template<typename, typename>
    struct is_same
    : public CxxUtils_Internal::false_type { };

  template<typename _Tp>
    struct is_same<_Tp, _Tp>
    : public CxxUtils_Internal::true_type { };



// libstdc++/23053
void test01()
{
  SG::unordered_set<int> s;

  const SG::unordered_set<int> &sref = s;

  sref.find(27);
}  

// libstdc++/23465
void test02()
{
  bool test __attribute__((unused)) = true;

  for (float lf = 0.1f; lf < 101.0f; lf *= 10.0f)
    for (int size = 1; size <= 6561; size *= 3)
      {
	SG::unordered_set<int> us1, us2;
	typedef SG::unordered_set<int>::local_iterator local_iterator;
	typedef SG::unordered_set<int>::size_type      size_type;
	
	us1.max_load_factor(lf);

	for (int i = 0; i < size; ++i)
	  us1.insert(i);
	
	us2 = us1;
	
	VERIFY( us2.size() == us1.size() );
	VERIFY( us2.bucket_count() == us1.bucket_count() );
	
	for (size_type b = 0; b < us1.bucket_count(); ++b)
	  {
	    size_type cnt = 0;
	    for (local_iterator it1 = us1.begin(b), it2 = us2.begin(b);
		 it1 != us1.end(b) && it2 != us2.end(b); ++it1, ++it2)
	      {
		VERIFY( *it1 == *it2 );
		++cnt;
	      }
	    VERIFY( cnt == us1.bucket_size(b) );
	  }
      }
}

// libstdc++/24054
void test03()
{
  bool test __attribute__((unused)) = true;

  typedef SG::unordered_multiset<std::string> Set;

  Set s;

  s.insert("etaoin");
  s.insert("etaoin");
  s.insert("etaoin");
  s.insert("shrdlu");

  VERIFY( s.erase("") == 0 );
  VERIFY( s.size() == 4 );

  VERIFY( s.erase("etaoin") == 3 );
  VERIFY( s.size() == 1 );

  VERIFY( s.erase("shrdlu") == 1 );
  VERIFY( s.size() == 0 );
}

// libstdc++/24064
void test04()
{
  bool test __attribute__((unused)) = true;

  using namespace std;
  using namespace SG;

  unordered_map<int, char, SG::hash<int>, equal_to<int>,
    allocator<pair<const int, char> >, true> m;
 
  for (int i = 0; i < 1000; ++i)
    m[i] = static_cast<char> ('0' + i % 9);
		
  for (int i = 0; i < 1000; ++i)
    VERIFY( ++m.find(i)->second == '1' + i % 9 );
}

// libstdc++/26127
void test05()
{
  SG::unordered_set<int> s;

  s.bucket(42);
  (void)s.key_eq();
  (void)s.max_load_factor();  
}

// libstdc++/26132
void test06()
{
  bool test __attribute__((unused)) = true;

  for (float lf = 1.0f; lf < 101.0f; lf *= 10.0f)
    for (int size = 1; size <= 6561; size *= 3)
      {
	SG::unordered_set<int> us1;
	typedef SG::unordered_set<int>::size_type size_type;
	
	us1.max_load_factor(10.0);

	for (int i = 0; i < size; ++i)
	  us1.insert(i);

	us1.max_load_factor(lf);
        double imaxload __attribute__((unused)) = 1. / us1.max_load_factor();

	for (int i = 1; i <= 6561; i *= 81)
	  {
	    const size_type n = size * 81 / i;
	    us1.rehash(n);
	    VERIFY( us1.bucket_count() >
                    static_cast<float>(us1.size()) * imaxload );
	    VERIFY( us1.bucket_count() >= n );
	  }
      }
}

// libstdc++/24061
void test07()
{
  bool test __attribute__((unused)) = true;
  
  typedef SG::unordered_map<std::string, int> Map;
  typedef Map::iterator       iterator;
  typedef Map::const_iterator const_iterator;
  typedef Map::value_type     value_type;
  
  Map m1;

  m1.insert(value_type("all the love in the world", 1));
  m1.insert(value_type("you know what you are?", 2));
  m1.insert(value_type("the collector", 3));
  m1.insert(value_type("the hand that feeds", 4));
  m1.insert(value_type("love is not enough", 5));
  m1.insert(value_type("every day is exactly the same", 6));
  m1.insert(value_type("with teeth", 7));
  m1.insert(value_type("only", 8));
  m1.insert(value_type("getting smaller", 9));
  m1.insert(value_type("sunspots", 10)); 
  VERIFY( m1.size() == 10 );

  iterator it1 = m1.begin();
  ++it1;
  iterator it2 = it1;
  ++it2;
  iterator it3 = m1.erase(it1);
  VERIFY( m1.size() == 9 );
  VERIFY( it3 == it2 );
  VERIFY( *it3 == *it2 );

  iterator it4 = m1.begin();
  ++it4;
  ++it4;
  ++it4;
  iterator it5 = it4;
  ++it5;
  ++it5;
  iterator it6 = m1.erase(it4, it5);
  VERIFY( m1.size() == 7 );
  VERIFY( it6 == it5 );
  VERIFY( *it6 == *it5 );

  const_iterator it7 = m1.begin();
  ++it7;
  ++it7;
  ++it7;
  const_iterator it8 = it7;
  ++it8;
  const_iterator it9 = m1.erase(it7);
  VERIFY( m1.size() == 6 );
  VERIFY( it9 == it8 );
  VERIFY( *it9 == *it8 );

  const_iterator it10 = m1.begin();
  ++it10;
  const_iterator it11 = it10;
  ++it11;
  ++it11;
  ++it11;
  ++it11;
  const_iterator it12 = m1.erase(it10, it11);
  VERIFY( m1.size() == 2 );
  VERIFY( it12 == it11 );
  VERIFY( *it12 == *it11 );
  VERIFY( ++it12 == m1.end() );

  iterator it13 = m1.erase(m1.begin(), m1.end());
  VERIFY( m1.size() == 0 );
  VERIFY( it13 == it12 );
  VERIFY( it13 == m1.begin() );
}
  
// libstdc++/24061
void test08()
{
  bool test __attribute__((unused)) = true;
  
  typedef SG::unordered_multimap<std::string, int> Mmap;
  typedef Mmap::iterator       iterator;
  typedef Mmap::const_iterator const_iterator;
  typedef Mmap::value_type     value_type;
  
  Mmap mm1;

  mm1.insert(value_type("all the love in the world", 1));
  mm1.insert(value_type("you know what you are?", 2));
  mm1.insert(value_type("the collector", 3));
  mm1.insert(value_type("the hand that feeds", 4));
  mm1.insert(value_type("love is not enough", 5));
  mm1.insert(value_type("every day is exactly the same", 6));
  mm1.insert(value_type("with teeth", 7));
  mm1.insert(value_type("only", 8));
  mm1.insert(value_type("getting smaller", 9));
  mm1.insert(value_type("sunspots", 10));

  mm1.insert(value_type("you know what you are?", 5));
  mm1.insert(value_type("the collector", 6));
  mm1.insert(value_type("the hand that feeds", 7));
  VERIFY( mm1.size() == 13 );

  iterator it1 = mm1.begin();
  ++it1;
  iterator it2 = it1;
  ++it2;
  iterator it3 = mm1.erase(it1);
  VERIFY( mm1.size() == 12 );
  VERIFY( it3 == it2 );
  VERIFY( *it3 == *it2 );

  iterator it4 = mm1.begin();
  ++it4;
  ++it4;
  ++it4;
  iterator it5 = it4;
  ++it5;
  ++it5;
  iterator it6 = mm1.erase(it4, it5);
  VERIFY( mm1.size() == 10 );
  VERIFY( it6 == it5 );
  VERIFY( *it6 == *it5 );

  const_iterator it7 = mm1.begin();
  ++it7;
  ++it7;
  ++it7;
  const_iterator it8 = it7;
  ++it8;
  const_iterator it9 = mm1.erase(it7);
  VERIFY( mm1.size() == 9 );
  VERIFY( it9 == it8 );
  VERIFY( *it9 == *it8 );

  const_iterator it10 = mm1.begin();
  ++it10;
  const_iterator it11 = it10;
  ++it11;
  ++it11;
  ++it11;
  ++it11;
  const_iterator it12 = mm1.erase(it10, it11);
  VERIFY( mm1.size() == 5 );
  VERIFY( it12 == it11 );
  VERIFY( *it12 == *it11 );

  iterator it13 = mm1.erase(mm1.begin(), mm1.end());
  VERIFY( mm1.size() == 0 );
  VERIFY( it13 == mm1.end() );
  VERIFY( it13 == mm1.begin() );
}
  
// libstdc++/24061
void test09()
{
  bool test __attribute__((unused)) = true;
  
  typedef SG::unordered_multiset<std::string> Mset;
  typedef Mset::iterator       iterator;
  typedef Mset::const_iterator const_iterator;

  Mset ms1;
  
  ms1.insert("all the love in the world");
  ms1.insert("you know what you are?");
  ms1.insert("the collector");
  ms1.insert("the hand that feeds");
  ms1.insert("love is not enough");
  ms1.insert("every day is exactly the same");
  ms1.insert("with teeth");
  ms1.insert("only");
  ms1.insert("getting smaller");
  ms1.insert("sunspots");

  ms1.insert("the hand that feeds");
  ms1.insert("love is not enough");
  ms1.insert("every day is exactly the same");
  VERIFY( ms1.size() == 13 );

  iterator it1 = ms1.begin();
  ++it1;
  iterator it2 = it1;
  ++it2;
  iterator it3 = ms1.erase(it1);
  VERIFY( ms1.size() == 12 );
  VERIFY( it3 == it2 );
  VERIFY( *it3 == *it2 );

  iterator it4 = ms1.begin();
  ++it4;
  ++it4;
  ++it4;
  iterator it5 = it4;
  ++it5;
  ++it5;
  iterator it6 = ms1.erase(it4, it5);
  VERIFY( ms1.size() == 10 );
  VERIFY( it6 == it5 );
  VERIFY( *it6 == *it5 );

  const_iterator it7 = ms1.begin();
  ++it7;
  ++it7;
  ++it7;
  const_iterator it8 = it7;
  ++it8;
  const_iterator it9 = ms1.erase(it7);
  VERIFY( ms1.size() == 9 );
  VERIFY( it9 == it8 );
  VERIFY( *it9 == *it8 );

  const_iterator it10 = ms1.begin();
  ++it10;
  const_iterator it11 = it10;
  ++it11;
  ++it11;
  ++it11;
  ++it11;
  const_iterator it12 = ms1.erase(it10, it11);
  VERIFY( ms1.size() == 5 );
  VERIFY( it12 == it11 );
  VERIFY( *it12 == *it11 );

  iterator it13 = ms1.erase(ms1.begin(), ms1.end());
  VERIFY( ms1.size() == 0 );
  VERIFY( it13 == ms1.end() );
  VERIFY( it13 == ms1.begin() );
}
  
// libstdc++/24061
void test10()
{
  bool test __attribute__((unused)) = true;
  
  typedef SG::unordered_set<std::string> Set;
  typedef Set::iterator       iterator;
  typedef Set::const_iterator const_iterator;

  Set s1;
  
  s1.insert("all the love in the world");
  s1.insert("you know what you are?");
  s1.insert("the collector");
  s1.insert("the hand that feeds");
  s1.insert("love is not enough");
  s1.insert("every day is exactly the same");
  s1.insert("with teeth");
  s1.insert("only");
  s1.insert("getting smaller");
  s1.insert("sunspots");
  VERIFY( s1.size() == 10 );

  iterator it1 = s1.begin();
  ++it1;
  iterator it2 = it1;
  ++it2;
  iterator it3 = s1.erase(it1);
  VERIFY( s1.size() == 9 );
  VERIFY( it3 == it2 );
  VERIFY( *it3 == *it2 );

  iterator it4 = s1.begin();
  ++it4;
  ++it4;
  ++it4;
  iterator it5 = it4;
  ++it5;
  ++it5;
  iterator it6 = s1.erase(it4, it5);
  VERIFY( s1.size() == 7 );
  VERIFY( it6 == it5 );
  VERIFY( *it6 == *it5 );

  const_iterator it7 = s1.begin();
  ++it7;
  ++it7;
  ++it7;
  const_iterator it8 = it7;
  ++it8;
  const_iterator it9 = s1.erase(it7);
  VERIFY( s1.size() == 6 );
  VERIFY( it9 == it8 );
  VERIFY( *it9 == *it8 );

  const_iterator it10 = s1.begin();
  ++it10;
  const_iterator it11 = it10;
  ++it11;
  ++it11;
  ++it11;
  ++it11;
  const_iterator it12 = s1.erase(it10, it11);
  VERIFY( s1.size() == 2 );
  VERIFY( it12 == it11 );
  VERIFY( *it12 == *it11 );
  VERIFY( ++it12 == s1.end() );

  iterator it13 = s1.erase(s1.begin(), s1.end());
  VERIFY( s1.size() == 0 );
  VERIFY( it13 == s1.end() );
  VERIFY( it13 == s1.begin() );
}
  
void test11()
{
  typedef SG::unordered_map<std::string, int> Map;
  typedef std::pair<const std::string, int> Pair;

  Map m;
  VERIFY(m.empty());

  std::pair<Map::iterator, bool> tmp = m.insert(Pair("grape", 3));
  Map::iterator i = tmp.first;
  VERIFY(tmp.second);

  Map::iterator i2 = m.find("grape");
  VERIFY(i2 != m.end());
  VERIFY(i2 == i);
  VERIFY(i2->first == "grape");
  VERIFY(i2->second == 3);

  Map::iterator i3 = m.find("lime");
  VERIFY(i3 == m.end());

  std::pair<Map::iterator, Map::iterator> p = m.equal_range("grape");
  VERIFY(std::distance(p.first, p.second) == 1);
  VERIFY(p.first == i2);

  std::pair<Map::iterator, Map::iterator> p2 = m.equal_range("lime");
  VERIFY(p2.first == p2.second);

  VERIFY(m.count("grape") == 1);
  VERIFY(m.count("lime") == 0);
}

void test12()
{
  typedef SG::unordered_multimap<std::string, int> Map;
  typedef std::pair<const std::string, int> Pair;

  Map m;
  VERIFY(m.empty());

  m.insert(Pair("grape", 3));
  m.insert(Pair("durian", 8));
  m.insert(Pair("grape", 7));

  Map::iterator i1 = m.find("grape");
  Map::iterator i2 = m.find("durian");
  Map::iterator i3 = m.find("kiwi");

  VERIFY(i1 != m.end());
  VERIFY(i1->first == "grape");
  VERIFY(i1->second == 3 || i2->second == 7);
  VERIFY(i2 != m.end());
  VERIFY(i2->first == "durian");
  VERIFY(i2->second == 8);
  VERIFY(i3 == m.end());

  std::pair<Map::iterator, Map::iterator> p1 = m.equal_range("grape");
  VERIFY(std::distance(p1.first, p1.second) == 2);
  Map::iterator tmp = p1.first;
  ++tmp;
  VERIFY(p1.first->first == "grape");
  VERIFY(tmp->first == "grape");
  VERIFY((p1.first->second == 3 && tmp->second == 7) ||
	 (p1.first->second == 7 && tmp->second == 3));

  std::pair<Map::iterator, Map::iterator> p2 = m.equal_range("durian");
  VERIFY(std::distance(p2.first, p2.second) == 1);
  VERIFY(p2.first->first == "durian");
  VERIFY(p2.first->second == 8);

  std::pair<Map::iterator, Map::iterator> p3 = m.equal_range("kiwi");
  VERIFY(p3.first == p3.second);

  VERIFY(m.count("grape") == 2);
  VERIFY(m.count("durian") == 1);
  VERIFY(m.count("kiwi") == 0);
}

void test13()
{
  typedef SG::unordered_multiset<std::string> Set;
  Set s;
  VERIFY(s.empty());

  s.insert("grape");
  s.insert("banana");
  s.insert("grape");

  Set::iterator i2 = s.find("banana");
  VERIFY(i2 != s.end());
  VERIFY(*i2 == "banana");
  
  std::pair<Set::iterator, Set::iterator> p = s.equal_range("grape");
  VERIFY(std::distance(p.first, p.second) == 2);
  Set::iterator i3 = p.first;
  ++i3;
  VERIFY(*p.first == "grape");
  VERIFY(*i3 == "grape");

  Set::iterator i4 = s.find("lime");
  VERIFY(i4 == s.end());  

  VERIFY(s.count("grape") == 2);
  VERIFY(s.count("banana") == 1);
  VERIFY(s.count("lime") == 0);
}

void test14()
{
  typedef SG::unordered_set<std::string> Set;
  Set s;
  VERIFY(s.empty());

  std::pair<Set::iterator, bool> tmp = s.insert("grape");
  Set::iterator i = tmp.first;

  Set::iterator i2 = s.find("grape");
  VERIFY(i2 != s.end());
  VERIFY(i2 == i);
  VERIFY(*i2 == "grape");

  std::pair<Set::iterator, Set::iterator> p = s.equal_range("grape");
  VERIFY(p.first == i2);
  VERIFY(std::distance(p.first, p.second) == 1);

  Set::iterator i3 = s.find("lime");
  VERIFY(i3 == s.end());  

  std::pair<Set::iterator, Set::iterator> p2 = s.equal_range("lime");
  VERIFY(p2.first == p2.second);

  VERIFY(s.count("grape") == 1);
  VERIFY(s.count("lime") == 0);
}

template<typename T>
  void
  do_test15()
  {
    bool test __attribute__((unused)) = true;

    typedef typename SG::hash<T>::argument_type  argument_type;
    typedef typename SG::hash<T>::result_type    result_type;
    
    VERIFY( (test_relationship<is_same, argument_type, T>(true)) );
    VERIFY( (test_relationship<is_same, result_type, std::size_t>(true)) );
  }
  
// libstdc++/24799
void test15()
{
  do_test15<bool>();
  do_test15<char>();
  do_test15<signed char>();
  do_test15<unsigned char>();
  do_test15<short>();
  do_test15<int>();
  do_test15<long>();
  do_test15<unsigned short>();
  do_test15<unsigned int>();
  do_test15<unsigned long>();
  do_test15<int*>();
  do_test15<std::string>();
  do_test15<float>();
  do_test15<double>();
  do_test15<long double>();

#ifdef _GLIBCXX_USE_WCHAR_T
  do_test15<wchar_t>();
  do_test15<std::wstring>();
#endif
}

// libstdc++/24061
void test16()
{
  bool test __attribute__((unused)) = true;
  
  typedef SG::unordered_map<std::string, int> Map;
  typedef Map::iterator       iterator;
  typedef Map::const_iterator const_iterator;
  typedef Map::value_type     value_type;

  Map m1;
  
  iterator it1 = m1.insert(m1.begin(),
			   value_type("all the love in the world", 1));
  VERIFY( m1.size() == 1 );
  VERIFY( *it1 == value_type("all the love in the world", 1) );
  
  const_iterator cit1(it1);
  const_iterator cit2 = m1.insert(cit1,
				  value_type("you know what you are?", 2));
  VERIFY( m1.size() == 2 );
  VERIFY( cit2 != cit1 );
  VERIFY( *cit2 == value_type("you know what you are?", 2) );

  iterator it2 = m1.insert(it1, value_type("all the love in the world", 3));
  VERIFY( m1.size() == 2 );
  VERIFY( it2 == it1 );
  VERIFY( *it2 == value_type("all the love in the world", 1) );
}
  
// libstdc++/24061
void test17()
{
  bool test __attribute__((unused)) = true;
  
  typedef SG::unordered_multimap<std::string, int> Mmap;
  typedef Mmap::iterator       iterator;
  typedef Mmap::const_iterator const_iterator;
  typedef Mmap::value_type     value_type;

  Mmap mm1;
  
  iterator it1 = mm1.insert(mm1.begin(),
			    value_type("all the love in the world", 1));
  VERIFY( mm1.size() == 1 );
  VERIFY( *it1 == value_type("all the love in the world", 1) );
  
  const_iterator cit1(it1);
  const_iterator cit2 = mm1.insert(cit1,
				   value_type("you know what you are?", 2));
  VERIFY( mm1.size() == 2 );
  VERIFY( cit2 != cit1 );
  VERIFY( *cit2 == value_type("you know what you are?", 2) );

  iterator it2 = mm1.insert(it1, value_type("all the love in the world", 3));
  VERIFY( mm1.size() == 3 );
  VERIFY( it2 != it1 );
  VERIFY( *it2 == value_type("all the love in the world", 3) );
}
  
// libstdc++/24061
void test18()
{
  bool test __attribute__((unused)) = true;
  
  typedef SG::unordered_multiset<std::string> Mset;
  typedef Mset::iterator       iterator;
  typedef Mset::const_iterator const_iterator;

  Mset ms1;
  
  iterator it1 = ms1.insert(ms1.begin(), "all the love in the world");
  VERIFY( ms1.size() == 1 );
  VERIFY( *it1 == "all the love in the world" );
  
  const_iterator cit1(it1);
  const_iterator cit2 = ms1.insert(cit1, "you know what you are?");
  VERIFY( ms1.size() == 2 );
  VERIFY( cit2 != cit1 );
  VERIFY( *cit2 == "you know what you are?" );

  iterator it2 = ms1.insert(it1, "all the love in the world");
  VERIFY( ms1.size() == 3 );
  VERIFY( it2 != it1 );
  VERIFY( *it2 == "all the love in the world" );
}
  
// libstdc++/24061
void test19()
{
  bool test __attribute__((unused)) = true;
  
  typedef SG::unordered_set<std::string> Set;
  typedef Set::iterator       iterator;
  typedef Set::const_iterator const_iterator;

  Set s1;
  
  iterator it1 = s1.insert(s1.begin(), "all the love in the world");
  VERIFY( s1.size() == 1 );
  VERIFY( *it1 == "all the love in the world" );
  
  const_iterator cit1(it1);
  const_iterator cit2 = s1.insert(cit1, "you know what you are?");
  VERIFY( s1.size() == 2 );
  VERIFY( cit2 != cit1 );
  VERIFY( *cit2 == "you know what you are?" );

  iterator it2 = s1.insert(it1, "all the love in the world");
  VERIFY( s1.size() == 2 );
  VERIFY( it2 == it1 );
  VERIFY( *it2 == "all the love in the world" );
}
  
void test20()
{
  typedef SG::unordered_map<std::string, int> Map;
  //typedef std::pair<const std::string, int> Pair;

  Map m;
  VERIFY(m.empty());

  m["red"] = 17;
  VERIFY(m.size() == 1);
  VERIFY(m.begin()->first == "red");
  VERIFY(m.begin()->second == 17);
  VERIFY(m["red"] == 17);

  m["blue"] = 9;
  VERIFY(m.size() == 2);
  VERIFY(m["blue"] == 9);

  m["red"] = 5;
  VERIFY(m.size() == 2);
  VERIFY(m["red"] == 5);
  VERIFY(m["blue"] == 9);
}

void test21()
{
  typedef SG::unordered_map<std::string, int> Map;
  typedef std::pair<const std::string, int> Pair;

  Map m;
  VERIFY(m.empty());

  Pair A[5] =
    {
      Pair("red", 5),
      Pair("green", 9),
      Pair("blue", 3),
      Pair("cyan", 8),
      Pair("magenta", 7)
    };

  m.insert(A+0, A+5);
  VERIFY(m.size() == 5);
  VERIFY(std::distance(m.begin(), m.end()) == 5);

  VERIFY(m["red"] == 5);
  VERIFY(m["green"] == 9);
  VERIFY(m["blue"] == 3);
  VERIFY(m["cyan"] == 8);
  VERIFY(m["magenta"] == 7);
}

void test22()
{
  typedef SG::unordered_map<std::string, int> Map;
  typedef std::pair<const std::string, int> Pair;

  Map m;
  VERIFY(m.empty());

  Pair A[9] =
    {
      Pair("red", 5),
      Pair("green", 9),
      Pair("red", 19),
      Pair("blue", 3),
      Pair("blue", 60),
      Pair("cyan", 8),
      Pair("magenta", 7),
      Pair("blue", 99),
      Pair("green", 33)
    };

  m.insert(A+0, A+9);
  VERIFY(m.size() == 5);
  VERIFY(std::distance(m.begin(), m.end()) == 5);

  VERIFY(m["red"] == 5);
  VERIFY(m["green"] == 9);
  VERIFY(m["blue"] == 3);
  VERIFY(m["cyan"] == 8);
  VERIFY(m["magenta"] == 7);
}

void test23()
{
  typedef SG::unordered_map<std::string, int> Map;
  typedef std::pair<const std::string, int> Pair;

  Map m;
  VERIFY(m.empty());

  std::pair<Map::iterator, bool> p = m.insert(Pair("abcde", 3));
  VERIFY(p.second);
  VERIFY(m.size() == 1);
  VERIFY(std::distance(m.begin(), m.end()) == 1);
  VERIFY(p.first == m.begin());
  VERIFY(p.first->first == "abcde");
  VERIFY(p.first->second == 3);
}

void test24()
{
  typedef SG::unordered_map<std::string, int> Map;
  typedef std::pair<const std::string, int> Pair;

  Map m;
  VERIFY(m.empty());

  std::pair<Map::iterator, bool> p1 = m.insert(Pair("abcde", 3));
  std::pair<Map::iterator, bool> p2 = m.insert(Pair("abcde", 7));

  VERIFY(p1.second);
  VERIFY(!p2.second);
  VERIFY(m.size() == 1);
  VERIFY(p1.first == p2.first);
  VERIFY(p1.first->first == "abcde");
  VERIFY(p2.first->second == 3);
}

void test25()
{
  typedef SG::unordered_multimap<std::string, int> Map;
  typedef std::pair<const std::string, int> Pair;

  Map m;
  VERIFY(m.empty());

  Pair A[5] =
    {
      Pair("red", 5),
      Pair("green", 9),
      Pair("blue", 3),
      Pair("cyan", 8),
      Pair("magenta", 7)
    };

  m.insert(A+0, A+5);
  VERIFY(m.size() == 5);
  VERIFY(std::distance(m.begin(), m.end()) == 5);

  for (int i = 0; i < 5; ++i)
    VERIFY(std::find(m.begin(), m.end(), A[i]) != m.end());
}

void test26()
{
  typedef SG::unordered_multimap<std::string, int> Map;
  typedef std::pair<const std::string, int> Pair;

  Map m;
  VERIFY(m.empty());

  Pair A[9] =
    {
      Pair("red", 5),
      Pair("green", 9),
      Pair("red", 19),
      Pair("blue", 3),
      Pair("blue", 60),
      Pair("cyan", 8),
      Pair("magenta", 7),
      Pair("blue", 99),
      Pair("green", 33)
    };

  m.insert(A+0, A+9);
  VERIFY(m.size() == 9);
  VERIFY(std::distance(m.begin(), m.end()) == 9);

  for (int i = 0; i < 9; ++i)
    VERIFY(std::find(m.begin(), m.end(), A[i]) != m.end());
}

void test27()
{
  typedef SG::unordered_multimap<std::string, int> Map;
  typedef std::pair<const std::string, int> Pair;

  Map m;
  VERIFY(m.empty());

  Map::iterator i = m.insert(Pair("abcde", 3));
  VERIFY(m.size() == 1);
  VERIFY(std::distance(m.begin(), m.end()) == 1);
  VERIFY(i == m.begin());
  VERIFY(i->first == "abcde");
  VERIFY(i->second == 3);
}

void test28()
{
  typedef SG::unordered_multimap<std::string, int> Map;
  typedef std::pair<const std::string, int> Pair;

  Map m;
  VERIFY(m.empty());

  m.insert(Pair("abcde", 3));
  m.insert(Pair("abcde", 7));

  VERIFY(m.size() == 2);
  VERIFY(std::distance(m.begin(), m.end()) == 2);

  Map::iterator i1 = m.begin();
  Map::iterator i2 = i1;
  ++i2;

  VERIFY(i1->first == "abcde");
  VERIFY(i2->first == "abcde");
  VERIFY((i1->second == 3 && i2->second == 7) ||
	 (i1->second == 7 && i2->second == 3));
}

void test29()
{
  typedef SG::unordered_multiset<std::string> Set;
  Set s;
  VERIFY(s.empty());

  const int N = 10;
  const std::string A[N] = { "red", "green", "blue", "violet", "cyan",
			     "magenta", "yellow", "orange", "pink", "gray" };

  s.insert(A+0, A+N);
  VERIFY(s.size() == static_cast<unsigned int>(N));
  VERIFY(std::distance(s.begin(), s.end()) == N);

  for (int i = 0; i < N; ++i) {
    std::string str = A[i];
    Set::iterator it = std::find(s.begin(), s.end(), str);
    VERIFY(it != s.end());
  }
}

void test30()
{
  typedef SG::unordered_multiset<int> Set;
  Set s;
  VERIFY(s.empty());

  const int N = 8;
  const int A[N] = { 3, 7, 4, 8, 2, 4, 6, 7 };

  s.insert(A+0, A+N);
  VERIFY(s.size() == static_cast<unsigned int>(N));
  VERIFY(std::distance(s.begin(), s.end()) == N);

  VERIFY(std::count(s.begin(), s.end(), 2) == 1);
  VERIFY(std::count(s.begin(), s.end(), 3) == 1);
  VERIFY(std::count(s.begin(), s.end(), 4) == 2);
  VERIFY(std::count(s.begin(), s.end(), 6) == 1);
  VERIFY(std::count(s.begin(), s.end(), 7) == 2);
  VERIFY(std::count(s.begin(), s.end(), 8) == 1);
}

void test31()
{
  typedef SG::unordered_set<std::string> Set;
  Set s;
  VERIFY(s.empty());

  const int N = 10;
  const std::string A[N] = { "red", "green", "blue", "violet", "cyan",
			     "magenta", "yellow", "orange", "pink", "gray" };

  s.insert(A+0, A+N);
  VERIFY(s.size() == static_cast<unsigned int>(N));
  VERIFY(std::distance(s.begin(), s.end()) == N);

  for (int i = 0; i < N; ++i) {
    std::string str = A[i];
    Set::iterator it = std::find(s.begin(), s.end(), str);
    VERIFY(it != s.end());
  }
}

void test32()
{
  typedef SG::unordered_set<int> Set;
  Set s;
  VERIFY(s.empty());

  const int N = 8;
  const int A[N] = { 3, 7, 4, 8, 2, 4, 6, 7 };

  s.insert(A+0, A+N);
  VERIFY(s.size() == 6);
  VERIFY(std::distance(s.begin(), s.end()) == 6);

  VERIFY(std::count(s.begin(), s.end(), 2) == 1);
  VERIFY(std::count(s.begin(), s.end(), 3) == 1);
  VERIFY(std::count(s.begin(), s.end(), 4) == 1);
  VERIFY(std::count(s.begin(), s.end(), 6) == 1);
  VERIFY(std::count(s.begin(), s.end(), 7) == 1);
  VERIFY(std::count(s.begin(), s.end(), 8) == 1);
}

// Verify we can default-construct iterators - sss.
void test33()
{
  SG::unordered_set<int>::iterator i1;
  SG::unordered_set<int>::const_iterator i2;
  SG::unordered_map<int, int>::iterator i3;
  SG::unordered_map<int, int>::const_iterator i4;
}

int main()
{
  test01();
  test02();
  test03();
  test04();
  test05();
  test06();
  test07();
  test08();
  test09();
  test10();
  test11();
  test12();
  test13();
  test14();
  test15();
  test16();
  test17();
  test18();
  test19();
  test20();
  test21();
  test22();
  test23();
  test24();
  test25();
  test26();
  test27();
  test28();
  test29();
  test30();
  test31();
  test32();
  test33();
  return 0;
}


using namespace std;
using namespace SG;

// Verify that we can instantiate hash for every required type.
//template struct SG::hash<bool>;
//template struct SG::hash<char>;
//template struct SG::hash<signed char>;
//template struct SG::hash<unsigned char>;
//template struct SG::hash<short>;
//template struct SG::hash<int>;
//template struct SG::hash<long>;
//template struct SG::hash<unsigned short>;
//template struct SG::hash<unsigned int>;
//template struct SG::hash<unsigned long>;
//template struct SG::hash<float>;
//template struct SG::hash<double>;
//template struct SG::hash<long double>;
template struct SG::hash<void*>;
//template struct SG::hash<std::string>;

#ifdef _GLIBCXX_USE_WCHAR_T
//template struct SG::hash<wchar_t>;
//template struct SG::hash<std::wstring>;
#endif


template class SG::unordered_map<string, float>;
template class SG::unordered_map<string, float,
			     SG::hash<string>, equal_to<string>, 
			     allocator<pair<const string, float> >, true>;


template class SG::unordered_multimap<string, float>;
template class SG::unordered_multimap<string, float,
				  SG::hash<string>, equal_to<string>, 
				  allocator<pair<const string, float> >, true>;


template class SG::unordered_multiset<int>;
template class SG::unordered_multiset<int, SG::hash<int>, equal_to<int>,
				  allocator<int>, true>;


template class SG::unordered_set<int>;
template class SG::unordered_set<int, SG::hash<int>, equal_to<int>,
			     allocator<int>, true>;
