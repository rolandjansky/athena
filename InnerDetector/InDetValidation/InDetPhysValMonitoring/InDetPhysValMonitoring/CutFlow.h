/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CutList.h
 * @author shaun roe
 **/

#ifndef _IDPVM_CutFlow_h_
#define _IDPVM_CutFlow_h_
#include <functional>
#include <algorithm>
#include <string>
#include <vector>


/** Templated class containing a cut, name of cut and description of cut(optional)
 * Typically, the class will be templated on <xAOD::TruthParticle> or <xAOD::TrackParticle>.
 * The cut is passed in as any predicate function, the type is declared as function<bool(A)>
 * although typically these will be lambda functions.
 * The cut is an 'accept' cut, i.e. the value passes if it is true.
 * These predicate functions are called each time the 'pass' method is called.
 * The class keeps an internal count of how many values passed, and may be used
 * as a functional due to the overloaded () operator.
 */
template<class A>
class Accept {
public:
  ///Default constructor with a simple predicate returning false
  Accept() : m_predicate([](A) {
    return false;
  }), m_name {}, m_desc {} {
    // nop
  }
  /** @brief Normal constructor
   * @param predicate any function taking the templated value and returning a bool
   * @param name optional name for this cut
   * @param description optional description for this cut
   */
  Accept(const std::function<bool(const A&)>& predicate, const std::string& name = "",
         const std::string& description = "") : m_predicate(predicate), m_name(name), m_desc(description)
  {
    // nop
  }

  ///Overloading the () operator allows the class to be used as a functional
  bool
  operator () (const A& i) const {
    return pass(i);
  }

  ///Apply the predicate function and return the value, also updating an internal counter
  bool
  pass(const A& i) const {
    const bool passed = m_predicate(i);
    return passed;
  }


  ///Return cut name
  std::string
  name() const {
    return m_name;
  }

  ///Return cut description
  std::string
  description() const {
    return m_desc;
  }

  ///Utility typedefs to help callers: the value type
  typedef A value_type;
  ///Utility typedefs to help callers: the function type
  typedef const std::function<bool (const A&)> func_type;
private:
  // std::function<bool(A)> m_predicate;
  func_type m_predicate;
  std::string m_name;
  std::string m_desc;
};

/** Templated CutList class to contain a group of cuts.
 * The CutList is typically instantiated with a vector of Accept objects, although
 * additional cuts may be added with the 'add' method.
 * Cuts may be applied in one of two ways, determined by the 'mode' parameter:
 * ALL: Always apply every cut
 * UNTIL_FAIL: Apply cuts until the first one fails
 * The 'accept' method applies the cuts and keeps internal count of how many times it is called.
 */
template<class A>
class CutList {
public:

  ///Normal constructor takes a vector<Accept>. Note default mode is 'ALL'.
  CutList(const std::vector<Accept<A> >& cuts) : m_cuts(cuts) {
    // nop
  }

  ///Default constructor with no cuts implemented
  CutList() : m_cuts{} {
    // nop
  }


  ///Add one cut
  void
  add(const Accept<A>& newCut) {
    m_cuts.push_back(newCut);
  }

  ///Apply cuts and return the boolean result; keep count of number of calls and passes
  unsigned int
  accept(const A& value) const {
    unsigned int missing_cuts = m_cuts.size();

    for (auto& thisCut:m_cuts) {
      if (not thisCut.pass(value)) {
        break;
      }
      --missing_cuts;
    }
    return missing_cuts;
  }

  unsigned int
  testAllCuts(const A& value, std::vector<unsigned int> &counter) const {
    unsigned int idx          = 0;
    ++(counter[idx++]);
    if (counter.size() != m_cuts.size()+2 /* CutFlow::kNReserved */) {
      throw std::logic_error("Number of cuts and counters do not match." );
    }
    unsigned int missing_cuts = 0;
    ++(counter[idx++]);
    for (auto& thisCut:m_cuts) {
      if (!thisCut.pass(value)) { ++missing_cuts;   }
      else                      { ++(counter[idx]); }
      ++idx;
    }
    return missing_cuts;
  }

  ///Return the number of cuts
  unsigned int
  size() const {
    return m_cuts.size();
  }

  ///Return a vector of the cut names
  std::vector<std::string>
  names() const {
    std::vector<std::string> result(m_cuts.size());
    unsigned int idx(0);
    for (const auto& i:m_cuts) {
      result[idx++] = i.name();
    }
    return result; // return-value-optimisation is invoked
  }

private:
  std::vector<Accept<A> > m_cuts;
};

class CutFlow
{
public:

  enum CutMode {
    ALL, UNTIL_FAIL
  };

  enum EReservedCuts {
    kAll,
    kIsValidParticle,
    kNReserved
  };

  CutFlow() {}

  CutFlow(unsigned int n_cuts, CutMode cut_mode = UNTIL_FAIL)
    : m_counter(n_cuts+kNReserved,0) ,
      m_integrated( cut_mode == ALL ),
      m_accumulateIntegrated( cut_mode == ALL )
  {}

  std::vector<unsigned int>       &counter()       {return m_counter; }
  const std::vector<unsigned int> &counter() const {return m_counter; }

 private:
  // disallow implicit conversion of CutResult
  void update(bool) {
  }

 public:
  //@TODO event weights ?
  void update(unsigned int missing_cuts) {
    assert( m_integrated == false);
    ++(m_counter.at(m_counter.size()-missing_cuts-1) );
  }

  void merge(CutFlow &&a_cutflow) {
    if (m_counter.empty()) {
      m_counter              = std::move(a_cutflow.m_counter);
      m_integrated           = a_cutflow.m_integrated;
      m_accumulateIntegrated = a_cutflow.m_accumulateIntegrated;
    }
    else {
      if (m_counter.size() != a_cutflow.m_counter.size() || m_integrated != a_cutflow.m_integrated) {
         throw std::logic_error("Tried to merge non matching cut_flows.");
      }
      std::vector<unsigned int>::iterator iter=m_counter.begin();
      for(unsigned int count : a_cutflow.m_counter)  {
        *(iter++)  += count;
      }
    }
  }

  void clear() {
    for(unsigned int &count : m_counter)  {count=0; }
  }

  ///Produce a formatted string report of the results
  std::string
  report(const std::vector<std::string> &names) {
    if (not m_integrated) {
      unsigned int sum=0;
      for(std::vector<unsigned int>::reverse_iterator iter = m_counter.rbegin();
          iter != m_counter.rend();
          ++iter)  {
        *iter += sum;
        sum    = *iter;
      };
      m_integrated=true;
    }

    std::string op = "\nCutList Report; Total processed: " + std::to_string(m_counter[kIsValidParticle]);
    op += "\nTotal passed: " + std::to_string(m_counter[m_counter.size()-1]);
    std::string modeString = (m_accumulateIntegrated) ? "\nAll cuts were applied\n" : "\nCuts were applied until one fails\n";
    op += modeString;
    for (unsigned int idx=0; idx<names.size(); ++idx) {
      op += names[idx] + ": " + std::to_string( idx+kNReserved<m_counter.size() ? m_counter[idx+kNReserved] : -1) + " passed\n";
    }
    if (names.size()+kNReserved != m_counter.size()) {
      throw std::logic_error(std::string( "Number of cuts and counters do not match. Resulting report:\n") + op );
    }
    return op;
  }

private:
  std::vector<unsigned int> m_counter;
  bool                      m_integrated           = false;
  bool                      m_accumulateIntegrated = false;
};
#endif
