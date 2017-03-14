/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CutFlow.h
 * @author shaun roe
 **/


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
  }), m_name {}, m_desc {}, m_nPassed(0) {
    // nop
  }
  /** @brief Normal constructor
   * @param predicate any function taking the templated value and returning a bool
   * @param name optional name for this cut
   * @param description optional description for this cut
   */
  Accept(const std::function<bool(const A&)>& predicate, const std::string& name = "",
         const std::string& description = "") : m_predicate(predicate), m_name(name), m_desc(description),
    m_nPassed(0) {
    // nop
  }

  ///Overloading the () operator allows the class to be used as a functional
  bool
  operator () (const A& i) {
    return pass(i);
  }

  ///Apply the predicate function and return the value, also updating an internal counter
  bool
  pass(const A& i) {
    const bool passed = m_predicate(i);

    m_nPassed += (int) passed;
    return passed;
  }

  ///Clear internal counter
  void
  clearCount() {
    m_nPassed = 0;
  }

  ///Return internal counter
  unsigned int
  getCount() const {
    return m_nPassed;
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
  unsigned int m_nPassed;
};

/** Templated CutFlow class to contain a group of cuts.
 * The CutFlow is typically instantiated with a vector of Accept objects, although
 * additional cuts may be added with the 'add' method.
 * Cuts may be applied in one of two ways, determined by the 'mode' parameter:
 * ALL: Always apply every cut
 * UNTIL_FAIL: Apply cuts until the first one fails
 * The 'accept' method applies the cuts and keeps internal count of how many times it is called.
 */
template<class A>
class CutFlow {
public:
  ///Cut mode: Apply ALL cuts, or until the first failure
  enum CutFlowMode {
    ALL, UNTIL_FAIL
  };
  ///Normal constructor takes a vector<Accept>. Note default mode is 'ALL'.
  CutFlow(const std::vector<Accept<A> >& cuts) : m_cuts(cuts), m_mode(ALL),
    m_passed(0), m_processed(0) {
    // nop
  }

  ///Default constructor with no cuts implemented
  CutFlow() : m_cuts{}, m_mode(ALL), m_passed(0), m_processed(0) {
    // nop
  }

  ///Add one cut
  void
  add(const Accept<A>& newCut) {
    m_cuts.push_back(newCut);
  }

  ///Clear internal counters
  void
  clear() {
    m_processed = 0;
    m_passed = 0;
    for (auto& i:m_cuts) {
      i.clearCount();
    }
  }

  ///set the accept mode according to the CutFlowMode
  void
  setMode(const CutFlowMode m) {
    m_mode = m;
  }

  ///get the accept mode
  CutFlowMode
  getMode() const {
    return m_mode;
  }

  ///Apply cuts and return the boolean result; keep count of number of calls and passes
  bool
  accept(const A& value) {
    bool result(true);

    if (m_mode == ALL) {
      for (auto& thisCut:m_cuts) {
        const bool passed = thisCut.pass(value);
        result &= passed;
      }
    } else {
      for (auto& thisCut:m_cuts) {
        const bool passed = thisCut.pass(value);
        if (not passed) {
          result = false;
          break;
        }
      }
    }
    m_processed++;
    m_passed += (int) result;
    return result;
  }

  ///Return the number of cuts
  unsigned int
  size() const {
    return m_cuts.size();
  }

  ///Return the number of times the 'accept' method was called
  unsigned int
  nProcessed() const {
    return m_processed;
  }

  ///Returnn the number of values which passed all cuts
  unsigned int
  nPassed() const {
    return m_passed;
  }

  ///Return a vector of individual counters for each cut
  std::vector<unsigned int>
  counters() const {
    std::vector<unsigned int> result(m_cuts.size(), 0);
    unsigned int idx(0);
    for (const auto& i:m_cuts) {
      result[idx++] = i.getCount();
    }
    return result; // return-value-optimisation is invoked
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

  ///Produce a formatted string report of the results
  std::string
  report() const {
    std::string op = "\nCutFlow Report; Total processed: " + std::to_string(m_processed);
    op += "\nTotal passed: " + std::to_string(m_passed);
    std::string modeString = (m_mode == ALL) ? "\nAll cuts were applied\n" : "\nCuts were applied until one fails\n";
    op += modeString;
    for (const auto& thisCut:m_cuts) {
      op += thisCut.name() + ": " + std::to_string(thisCut.getCount()) + " passed\n";
    }
    return op;
  }

private:
  std::vector<Accept<A> > m_cuts;
  CutFlowMode m_mode;
  unsigned int m_passed;
  unsigned int m_processed;
};
