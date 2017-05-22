/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef DECISIONHANDLING_COMBINATORS_H
#define DECISIONHANDLING_COMBINATORS_H

#include <vector>
namespace HLT {
  /**
   * @class CombinationsGenerator helper to generate all possible combinations of objects
   * Provided size of all collections from which the objects shoudl be combined it allws to get all 
   **/
  class CombinationGenerator {
  public:
    /**
     * @brief construct combnations maker with the sizes of collection to which it shoudl be applied
     **/
    CombinationGenerator( const std::initializer_list<size_t>& collectionSizes );
    void add( size_t nextColl) { m_maxes.push_back( nextColl ); reset(); }
    void reset();
    size_t size() const { return m_maxes.size(); }
    /**
     * @brief returns current combination
     **/
    const std::vector<size_t>& operator()() const { return m_current; }
    const std::vector<size_t>& current() const { return m_current; }
    
    /**
     * @brief moves to the next combination
     **/    
    void operator++();

    /**
     * @brief are combinations exhausted
     **/        
    operator bool() const;
  private:
    std::vector<size_t> m_maxes;
    std::vector<size_t> m_current;
  };

  /**
   * @class Utility class to generate combinations dierctly with collections and their respective iterators
   * The usage is expected to be:
   * auto ci = CombinationIterator( collA, collB, collC );
   * while ( ci ) {
   *   //iterators aIter, bIter, cIter are iterators for collA, collB and collC respectively
   *   ci(aIter,bIter,cIter);  // they form now valid combination
   *   ++ci; // and they move ahead
   * }
   * @warning the implementation is missing for now 
   **/
  template<typename ... Collections>
  class CombinationIterator {
  public:

    CombinationIterator( Collections ... coll);
    ~CombinationIterator();

    template<typename ... Iterators>
    void operator()( Iterators ... iter ) const;

    
    void reset() {  }
    /**
     * @brief are combinations exhausted
     **/        
    operator bool() const;
        
    /**
     * @brief moves to the next combination
     **/    
    void operator++();    
  private:
  };
  
  
}

#endif // DECISIONHANDLING_COMBINATORS_H
