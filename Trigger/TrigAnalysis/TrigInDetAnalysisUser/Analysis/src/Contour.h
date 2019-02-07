/* emacs: this is -*- c++ -*- */
/**
 **     @file    Contour.h
 **
 **     @author  mark sutton
 **     @date    Mon 17 Aug 2015 19:11:54 CEST 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef  CONTOUR_H
#define  CONTOUR_H

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdio>

#include "TrigInDetAnalysis/TIDDirectory.h"


template<typename T> 
class Contour : public std::vector< std::pair<double, T*> > {

  typedef std::pair<double, T*> value_type;

public:

  Contour(const std::string& s) : mname(s), mdir(s) { } 

  virtual ~Contour() { 
    for ( unsigned i=this->size()-1 ; i-- ;  ) if ( this->at(i).second ) delete this->at(i).second;  
  } 

  void ranges( const std::vector<double>& d, T* t ) {
    if ( d.empty() ) return;
    ranges( &d[0], d.size(), t );
  }

  void ranges( const double* d, unsigned N, T* t ) { 
    mdir.push();
    for ( unsigned i=1 ; i<N ; i++ ) { 
      this->push_back( value_type( d[i-1], new T(*t) ) );
    } 
    this->push_back( value_type( d[N-1], 0 ) );
    mdir.pop();
  }

  T* find( double d ) { 
    if ( this->empty() ) return 0;
    for ( unsigned i=this->size()-1 ; i-- ; ) {
      if ( d>=this->at(i).first && d<this->at(i+1).first ) return this->at(i).second;
    } 
    return 0;
  }
  
  std::string name() const { return mname; }

  void Write(const std::string& s="") { 
    mdir.push();
    for ( unsigned i=this->size()-1 ; i-- ;  ) { 
      if ( this->at(i).second ) { 
	char _name[64];
	if ( s=="" ) std::sprintf( _name, "%s_%d", mname.c_str(), i-1 ); 
	else         std::sprintf( _name, "%s_%d", s.c_str(), i-1 ); 
	this->at(i).second->Bayes()->Write(_name);
      }  
    }
    mdir.pop(); 
  }

  TIDDirectory* dir() { return &mdir; }

private:

  std::string   mname;

  TIDDirectory  mdir;

};

template<typename T> 
inline std::ostream& operator<<( std::ostream& s, const Contour<T>& _c ) { 
  s << "Contour : " << _c.name() << "\n";
  for ( unsigned i=0 ; i<_c.size() ; i++ ) s << "\t d " << _c[i].first << "\n"; 
  return s;
}


#endif  // CONTOUR_H 










