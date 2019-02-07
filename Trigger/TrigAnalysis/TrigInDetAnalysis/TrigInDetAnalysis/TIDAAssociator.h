/* emacs: this is -*- c++ -*- */
/**
 **     @file    TIDAAssociator.h
 **
 **     @author  mark sutton
 **     @date    Sun 18 Jan 2009 19:08:11 GMT 
 **
 **     @copyright Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef   TIDA_ASSOCIATOR_H
#define   TIDA_ASSOCIATOR_H

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <map>

namespace TIDA { 

template<typename T, typename S=T> 
class Associator {

public:

  typedef std::map<T*, S*> map_type; 

public:

  Associator(const std::string& name)
    :  mname(name) 
  { } 
  
  virtual ~Associator() { } 
  
  virtual void match(const std::vector<T*>& s1, 
		     const std::vector<S*>& s2 ) = 0;
  
  // get matched track from map
  virtual const S* matched( T* t) { 
     typename map_type::const_iterator titr = mmatched.find(t);
     if ( titr != mmatched.end() ) return titr->second;
     else                          return 0;
  } 

  // get matched track from reverse map
  virtual const T* revmatched( S* t) { 
     typename map_type::const_iterator titr = mrevmatched.find(t);
     if ( titr != mrevmatched.end() ) return titr->second;
     else                             return 0;
  } 

  // get the lookup tables
  const map_type&    matched() const { return mmatched; }
  const map_type& revmatched() const { return mrevmatched; }

  // clear the match lookup tables
  void clear() { mmatched.clear(); mrevmatched.clear(); } 

  // how many items were matched
  unsigned size() const { return mmatched.size(); }

protected:

  std::string mname;
  
  map_type mmatched;
  map_type mrevmatched;

};

}




template<typename T, typename S> 
inline std::ostream& operator<<(std::ostream& s, const TIDA::Associator<T,S>& a ) { 
  typename TIDA::Associator<T,S>::map_type::const_iterator mitr = a.matched().begin();
  typename TIDA::Associator<T,S>::map_type::const_iterator mend = a.matched().end();

  std::cout << "TIDA::Associator size() " << a.size() << std::endl;

  while ( mitr!=mend ) {
    s << "\t[ ";
    if ( mitr->first ) s << *(mitr->first) << "\t";
    else               s << "-------\t\t";

    if ( mitr->second ) s << *(mitr->second);
    else                s << "-------";

    s << " ]" << std::endl;

    mitr++;
  }

  return s;
}


#endif  //  TIDA_ASSOCIATOR_H 










