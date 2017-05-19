/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef DECISIONHANDLING_COMBINATORS_H
#define DECISIONHANDLING_COMBINATORS_H

#include <vector>
namespace HLT {
  /**
   * @brief This function verifies if in the input data there exists an inclusive combination
   * The is there is enough elements to form an N elements combination of objects passing the array of inclusive cuts
   * @return is a list of objects that pass the participate in at least one calid combination
   * The object F has to implement following methods:
   * size_t size() returning number of elements required in the ocmbination
   * bool passing(T obj, size_t N) returning tru if the obj passes the cut N
   * this is a desiderata of the interface, an implementation is missing
   **/
  template<typename T, typename F>
    std::vector<T> inclusive_combination_subset( const std::vector<T>& data, F f) {
    return data;
  }

  /**
   * @class CombinationsGenerator helper to generate all possible combinations of objects
   * Provided size of all collections from which the objects shoudl be combined it allws to get all 
   * 
   **/
  class CombinationsGenerator {
  public:
    CombinationsGenerator( const std::initializer_list<size_t>& collectionSizes );
    void reset();
    const std::vector<size_t>& operator()() const;
    void operator++();
    operator bool() const;
  private:
    const std::vector<size_t> m_maxes;
    std::vector<size_t> m_current;
  };
  
}

#endif // DECISIONHANDLING_COMBINATORS_H
