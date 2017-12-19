/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/Root/normalizedTypeinfoName.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Convert a @c type_info to a normalized string representation
 *        (matching the names used in the root dictionary).
 */


#include "AthContainers/normalizedTypeinfoName.h"
#include "AthContainers/tools/threading.h"
#include "AthContainers/tools/error.h"
#include "CxxUtils/ClassName.h"
#include "CxxUtils/checker_macros.h"
#include <unordered_map>
#include <functional>


namespace {


CxxUtils::ClassName::Rules makeRules()
{
  CxxUtils::ClassName::Rules rules;

  // STL container types that Reflex knows about.
  rules.add ("std::deque<$T, std::allocator<$T> >",
             "std::deque<$T>");
  rules.add ("std::list<$T, std::allocator<$T> >",
             "std::list<$T>");
  rules.add ("std::vector<$T, std::allocator<$T> >",
             "std::vector<$T>");
  rules.add ("std::map<$KEY, $T, std::less<$KEY>, std::allocator<std::pair<const $KEY, $T> > >",
             "std::map<$KEY, $T>");
  rules.add ("std::multimapmap<$KEY, $T, std::less<$KEY>, std::allocator<std::pair<const $KEY, $T> > >",
             "std::multimapmap<$KEY, $T>");
  rules.add ("std::set<$KEY, std::less<$KEY>, std::allocator<$KEY> >",
             "std::set<$KEY>");
  rules.add ("std::multiset<$KEY, std::less<$KEY>, std::allocator<$KEY> >",
             "std::multiset<$KEY>");
  rules.add ("std::unordered_map<$KEY, $T, std::hash<$KEY>, std::equal_to<$KEY>, std::allocator<std::pair<const $KEY, $T> > >",
             "std::unordered_map<$KEY, $T>");
  rules.add ("std::unordered_multimap<$KEY, $T, std::hash<$KEY>, std::equal_to<$KEY>, std::allocator<std::pair<const $KEY, $T> > >",
             "std::unordered_multimap<$KEY, $T>");
  rules.add ("std::unordered_set<$KEY, std::hash<$KEY>, std::equal_to<$KEY>, std::allocator<$KEY> >",
             "std::unordered_set<$KEY>");
  rules.add ("std::unordered_multiset<$KEY, std::hash<$KEY>, std::equal_to<$KEY>, std::allocator<$KEY> >",
             "std::unordered_multiset<$KEY>");
  rules.add ("std::queue<$T, std::deque<$T> >",
             "std::queue<$T>");
  rules.add ("std::stack<$T, std::deque<$T> >",
             "std::stack<$T>");

  // These container types are also defined in the C++11 standard,
  // but root5 reflex doesn't know about them.  List them here anyway.
  rules.add ("std::forward_list<$T, std::allocator<$T> >",
             "std::forward_list<$T>");
  rules.add ("std::priority_queue<$T, std::vector<$T>, std::less<$T> >",
             "std::priority_queue<$T>");

  // Reflex also handles the non-standard containers hash_map, etc.
  // Don't think atlas is using those anywhere, so skip that.

  // Reflex handles the basic_string template.
  // I actually think that demangling will produce `std::string' rather
  // than `std::basic_string' with default arguments; however, add it
  // for completeness.
  rules.add ("std::basic_string<$T, std::char_traits<$T>, std::allocator<$T> >",
             "std::string");

  // Atlas-specific mappings.
  rules.add ("DataVector<$T, $U>", "DataVector<$T>");

  // Handle gcc's C++11 ABI.
  rules.add ("std::__cxx11", "std");

  // This works around a bug in Gaudi's typeinfoName, which substitutes
  // 'std::basic_string<...> ?' with `std::string' --- but including the
  // optional trailing space in the pattern to replace means that we can
  // end up with `std::stringconst'.
  rules.add ("std::stringconst", "const std::string");

  // Needed for macos?
  rules.add ("std::__1", "std");

  return rules;
}


} // anonymous namespace


namespace SG {


/**
 * @brief Convert a @c type_info to a normalized string representation
 *        (matching the names used in the root dictionary).
 * @param info The type to convert.
 *
 * The function `AthContainer_detail::typeinfoName` may be used to convert
 * a C++ `type_info` to a string representing the name of the class; this
 * handles platform-dependent details such as performing demangling.
 *
 * However, the name you get as a result of this does not necessarily match
 * the name by which the class is known in the ROOT dictionary.
 * In particular, defaulted template arguments for STL containers and
 * @c DataVector are suppressed in the dictionary.  So, for example,
 * for a vector class @c typeinfoName may produce
 * `std::vector<int, std::allocator<T> >`, while in the dictionary
 * it is known as `std::vector<int>`.  Using @c normalizedTypeinfoName
 * instead will transform the names to match what's in the dictionary.
 * This function will also cache the typeinfo -> string conversions.
 */
std::string normalizedTypeinfoName (const std::type_info& info)
{
  // Maintain a cache of mappings, protected with an ordinary mutex.
  // We originally did this with an upgrading mutex.
  // However, these mutexes have been observed to have considerable overhead,
  // so are probably not worthwhile if the read critical section is short.
  // If this lock becomes a performance issue, consider a concurrent hashmap
  // or rcu-style solution.
  typedef std::unordered_map<const std::type_info*, std::string> typemap_t;
  static typemap_t normalizedTypemap ATLAS_THREAD_SAFE;
  typedef AthContainers_detail::mutex mutex_t;
  static mutex_t normalizedTypemapMutex;
  AthContainers_detail::lock_guard<mutex_t> lock (normalizedTypemapMutex);

  // Test to see if we already have the mapping.
  typemap_t::iterator it = normalizedTypemap.find (&info);
  if (it != normalizedTypemap.end()) {
    return it->second;
  }

  // Didn't find it.  Apply the rules.
  static const CxxUtils::ClassName::Rules normalizeRules = makeRules();
  std::string tiname = AthContainers_detail::typeinfoName (info);
  std::string normalizedName = normalizeRules.apply (tiname);

  // Remember this mapping.
  normalizedTypemap[&info] = normalizedName;
  return normalizedName;
}


} // namespace SG
