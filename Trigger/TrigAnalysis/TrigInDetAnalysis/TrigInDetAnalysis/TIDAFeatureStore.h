// emacs: this is -*- c++ -*-
//
//   @file    TIDAFeatureStore.h        
//
//            Generic storage container - to contain objects wrapped 
//            as Features      
//  
//   Copyright (C) 2015 M.Sutton (sutt@cern.ch)    
//
//   $Id: FeatureStore.h, v0.0   Fri  7 Aug 2015 15:00:32 CEST sutt $


#ifndef  TIDA_FEATURESTORE_H
#define  TIDA_FEATURESTORE_H

#include <iostream>
#include <map>

#include "TIDAFeature.h"


namespace TIDA { 

class FeatureStore : public std::map<std::string, TIDA::FeatureBase*> { 

public:
  
  typedef  std::map<std::string, FeatureBase*> map_type;
  
public:

  FeatureStore() { } 

  ~FeatureStore() { 
    const_iterator itr  = begin();
    const_iterator iend = end();
    while( itr!=iend ) delete (itr++)->second;
  }


  template<typename T> 
  void insert( T* t,  const std::string& key ) { 
    TIDA::Feature<T>* tt = new TIDA::Feature<T>(t); 
    _insert( tt, key ); 
  }


  template<typename T>
  int find( T*& t, const std::string& key ) { 
    TIDA::FeatureBase* f = _find(key);
    if ( f ) {      
      TIDA::Feature<T>* fdptr = dynamic_cast<TIDA::Feature<T>*>(f);
      if ( fdptr ) { 
	t = fdptr->cptr();
	return 0;
      }
      else return -2;
    }
    return -1;
  }

private:

  void _insert( TIDA::FeatureBase* f, const std::string& key ) { 
    map_type::insert( value_type( key, f ) ); 
  } 

  TIDA::FeatureBase* _find( const std::string& key ) {
    iterator itr=map_type::find(key);
    if ( itr!=end() ) return itr->second;
    else              return 0;
  }


};

}



inline std::ostream& operator<<( std::ostream& s, const TIDA::FeatureStore& f ) { 
  TIDA::FeatureStore::const_iterator itr  = f.begin();
  TIDA::FeatureStore::const_iterator iend = f.end();
  while( itr!=iend ) { 
    if ( itr->first.size()>5 ) s << "\t[ " << itr->first << ":\t"   << itr->second << " ]\n";
    else                       s << "\t[ " << itr->first << ":\t\t" << itr->second << " ]\n";
    itr++;
  }
  return s;
}


#endif  // FEATURESTORE_H 










