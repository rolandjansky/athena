/* emacs: this is -*- c++ -*- */
/**
 **     @file    lumiList.h
 **
 **     @brief   standalone implementation of a good lumi 
 **              block list       
 **
 **     @author  mark sutton
 **     @date    $Id: lumiList.h 780225 2016-10-25 10:04:58Z 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef ANALYSIS_LUMILIST_H
#define ANALYSIS_LUMILIST_H

#include <iostream>
#include <map>
#include <algorithm>

#include "grl_run.h"




class lumiList : public std::map< int, grl_run >  {

public:

  /// full map type 
  typedef  std::map< int, grl_run >           full_type;

  /// iterators over the full map
  typedef  std::map< int, grl_run >::iterator       iterator;

  typedef  std::map< int, grl_run >::const_iterator const_iterator;

public:
  
  lumiList(bool b=true) : mterse(b) {  } 

  ~lumiList() { } 

  void setterse(bool b) { mterse=b; }

  /// add a lumi block range for a given run
  void addRange(int _run, int _start, int _end) {

    iterator it = find(_run);

    if ( it == end() ) {
      std::pair< iterator, bool > in = insert( std::pair<int, grl_run >( _run, grl_run( _run ) ) );
      it = in.first;
    }
   
    it->second.add_range( _start, _end );
   
    if ( !mterse ) std::cout << "lumiList::addRange() run " << _run << ";\t" << _start << " - " << _end << std::endl;
  } 



  /// is this run, lumiblock in any of the ranges?
  /// if the run is not in the list, it is assumed 
  /// that all events from that run are required  
  bool inRange( int _run, int _lb ) {

    iterator itr  = find(_run);
    
    if ( itr == end() ) return true;
    else { 
      grl_run& v = itr->second;
      for ( unsigned i=0 ; i<v.size() ; i++ ) { 
	if ( _lb>=v[i].first && _lb<=v[i].second ) return true; 
      }
    }
    return false; 
  }    


  /// sort the runs by the number of good lumi blocks
  void sort() { 
    
    std::cout << "lumiList::sort() sorting ..." << std::endl; 

    std::vector< const grl_run*  > vec;
    
    const_iterator itr  = begin();
    
    while ( itr!=end() ) { 
      vec.push_back( &(itr->second) ); 
      itr++;
    }

    /// sort the vector 
    std::sort(vec.begin(), vec.end(), grl_run::comparison );

    /// print out 

    double total = 0;
    for ( unsigned i=vec.size() ; i-- ;  ) total += vec[i]->lbsize();

   
    double integral = 0;
    for ( unsigned i=0 ; i<vec.size() ; i++ ) {

      integral += vec[i]->lbsize();

      std::cout << i 
		<< "\t" << int(100*(i+1.0)/vec.size()) 
		<< "\t" << int(integral/total*1000)*0.1 
		<< "\t" << *vec[i] << "\tfraction: " <<  vec[i]->lbsize()/total << std::endl;

    } 
  }

private: 

  bool mterse;

};












inline std::ostream& operator<<( std::ostream& s, const lumiList& _l ) { 

  lumiList::const_iterator itr = _l.begin();
  
  s << "lumilist: " << _l.size() << " runs" << "\n";

  while ( itr!=_l.end() ) { 
    
    s << "lumiList: run " << itr->first << " range: " << itr->second.first() << "\t- " << itr->second.last() << "\t : size " << itr->second.lbsize() << "\n"; 
    
    itr++;
  }

  return s;
}


#endif  // ANALYSIS_LUMILIST_H 










