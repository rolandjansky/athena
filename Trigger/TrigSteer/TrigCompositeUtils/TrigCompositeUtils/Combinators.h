/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGCOMPOSITEUTILS_COMBINATORS_H
#define TRIGCOMPOSITEUTILS_COMBINATORS_H

#include <vector>
#include <set>
#include <functional>

namespace HLT {
  /**
   * @class CombinationsGenerator helper to generate all possible combinations of objects
   * @warning The class is not making any assumption as if the this are combinations with objets repeated, i.e. it works on set of indices.   
   **/
  class CombinationGenerator {
  public:
    /**
     * @brief construct combnations maker with the sizes of collection to which it should be applied
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
   * Generator of unique combinations (no indices are repeated)
   * API description @see CombinationGenerator
   **/
  class UniqueCombinationGenerator {
  public:
    UniqueCombinationGenerator( size_t nelems, size_t comblen);
    void reset();
    size_t size() const { return m_combLen; }
    const std::vector<size_t>& operator()() const { return current(); }
    const std::vector<size_t>& current() const { return m_current; }

    void operator++();
    operator bool() const;
  private:
    size_t m_nElements;
    size_t m_combLen;
    std::vector<size_t> m_current;
    std::vector<bool> m_bitmask;
  };

  /**
   * An ensemble of UniqueCombinationGenerator
   * API description @see CombinationGenerator
   **/
  class NestedUniqueCombinationGenerator {
  public:
    void add( const UniqueCombinationGenerator& gen );
    void reset();
    size_t size() const;
    const std::vector<size_t>& operator()() const { return current(); }
    const std::vector<size_t>& current() const { return m_current; }

    void operator++();
    operator bool() const;
    

  private:
    std::vector<UniqueCombinationGenerator> m_generators;
    std::vector<size_t> m_current;
    void cache();
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
    CombinationGenerator m_generator;
  };  


  /**
   * Unique combinations for case when one can not repeat the index (i.e. for N same-object kind of triggers)
   **/
  /**
   * @brief Function find all elements of the decisions which take part in succesfull unique combinations
   * The combinations are formed from the content of each of the "indices" subvector. 
   * The index never repeats in the combination.
   *
   * @arg decsions - stores indices of objects which succesfully pass selection
   * @arg participants - set of indices which contribute to any valid combination found (useful tor marking passing RoIs)
   * @arg filter - additional combinations filter (can be used to implement additional constraint on the objects)
   * Another approach would be to make a generator object like above, we shall see which approach works better.
   **/
  typedef std::vector<size_t>  Index1DVec;
  typedef std::vector< Index1DVec > Index2DVec;

  void elementsInUniqueCombinations( const Index2DVec& indices,  std::set<size_t>& participants, std::function<bool(const Index1DVec&)> filter = [](const Index1DVec&){ return true; } );
  
  /**
   * @brief Creates unique combinations of elements 
   * @arg combinations - all calid combinations
   * For desciption @see elementsInUnuqueCombinations, this method is different as it exposes all combinations formed    
   **/
  void findUniqueCombinations( const Index2DVec& indices,  std::vector<std::vector<size_t> >& combinations, std::function<bool(const Index1DVec&)> filter = [](const Index1DVec&){ return true; } );


  
}

#endif // DECISIONHANDLING_COMBINATORS_H
