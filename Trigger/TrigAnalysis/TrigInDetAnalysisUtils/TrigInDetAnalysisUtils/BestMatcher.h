/* emacs: this is -*- c++ -*- */
/**
 **     @file    BestMatcher.h
 **
 **       truely generic template class of a best matching 
 **       algorithm based on an efficient implementation of 
 **       the "best matcher" algorithm from Ercan Ozcan 
 **
 **     @author  mark sutton
 **     @date    Mon 10 Aug 2015 03:07:24 CEST 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef  BESTMATCHER_H
#define  BESTMATCHER_H

#include <iostream>
#include <set>


#include "TrigInDetAnalysis/TIDAAssociator.h"


template<typename T, typename S=T>
class BestMatcher : public TIDA::Associator<T,S>  {

protected:

  /// internal class to store the matching paramter for a pair, 
  /// and in the indices of the pair

  class matched_ { 
    
  public:
    
    matched_(double d, int i, int j) : m_d(d), mmatch(std::pair<int,int>(i,j)) { } 
    
    double d() const { return m_d; } 
    
    std::pair<int, int> pair() const { return mmatch; }
    
    int first()  const { return mmatch.first; } 
    int second() const { return mmatch.second; } 
    
    bool  operator<(const matched_& a) const { return d()<a.d(); }
    bool  operator>(const matched_& a) const { return d()>a.d(); }
    bool operator==(const matched_& a) const { return  d()==a.d(); } 
    bool operator!=(const matched_& a) const { return  d()!=a.d(); } 
    
  private:
    double              m_d;
    std::pair<int, int> mmatch; 
  };

public:

  BestMatcher(const std::string& name, double d) : 
    TIDA::Associator<T,S>(name), m_d(d) 
  {   } 
  
  virtual ~BestMatcher() { } 
  
  
  virtual void match(const std::vector<T*>& ref, const std::vector<S*>& test ) 
  {
    this->clear();
    
    std::map<int,int> matched = matcher( ref, test);

    std::map<int,int>::iterator  mitr = matched.begin();
    while ( mitr!=matched.end() ) {
      this->mmatched.insert(    typename TIDA::Associator<T,S>::map_type::value_type( ref[mitr->first],   test[mitr->second]) );
      this->mrevmatched.insert( typename TIDA::Associator<T,S>::map_type::value_type( test[mitr->second], ref[mitr->first] ) );
      mitr++;
    }  

  }
 
  virtual double distance( const T* t0, const S* t1 ) const = 0;


protected:


  template<typename Tp, typename Tq>
  std::map<int, int>  matcher( const std::vector<Tp*>& ref, const std::vector<Tq*>& test) { 
    
    /// inserting into a multimatch sorts them into order 
    /// as they are added so you get the list of best 
    /// matches imediately 
    /// NB: probably quicker to add them to a vector and then 
    ///     sort them afterward
    
    std::multiset<matched_> m;
    
    for (unsigned int i=0 ; i<ref.size() ; i++ ) { 
      
      for (unsigned int j=0 ; j<test.size() ; j++ ) { 
	double d = distance(ref[i], test[j]);   
	if ( d<m_d ){  
	  m.insert( matched_(d, i, j) ); 
	}
      }
    }
    
    /// now go through from best to worst, adding to another
    /// sorted set only the matches that do not use tracks
    /// already used by a better matching pair already found
    
    std::vector<bool>  refused(  ref.size(), false ); 
    std::vector<bool> testused( test.size(), false ); 

    /// set of unique track pairings
    /// NB: for some reason, using the multiset is faster 
    ///     than using a vector, even though we are only
    ///     iterating through it later
    std::multiset<matched_> unique;

    typename std::multiset<matched_>::iterator mitr = m.begin();
    
    double  chi2 = 0;
    
    for ( ; mitr!=m.end() ; mitr++ ) { 

      int rind = mitr->first();
      int tind = mitr->second();

      if (  refused[rind] ) continue;
      if ( testused[tind] ) continue;

      refused[rind]  = true;
      testused[tind] = true;

      unique.insert( *mitr );

      chi2 += (mitr->d()*mitr->d());

    }
    
    
    //    std::cout << "chi2 of matches " << chi2 << std::endl;
   
    /// hooray!! now print out the answer, make a map etc
    /// this isn't technically needed, could just use the 
    /// set of "matched" objects, but hey ho 
    
    //  std::cout << "\nmatched" << std::endl; 
    
    std::map<int, int> matches;


    mitr = unique.begin();
    while ( mitr!=unique.end() ) {
      matches.insert( std::map<int, int>::value_type( mitr->first(), mitr->second() ) );
      //   std::cout << "\tbest match " << *mitr << "\t" <<  ref[mitr->first()] << "\t" << test[mitr->second()] << std::endl;
      mitr++;
    }  
 
    return matches;
    
  }


protected:

  double m_d;

};



#endif  // MATCH_H 










