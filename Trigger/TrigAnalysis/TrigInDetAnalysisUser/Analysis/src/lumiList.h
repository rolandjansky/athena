// emacs: this is -*- c++ -*-
//
//   @file    lumiList.h        
//            standalone implementation of a good lumi 
//            block list       
//  
//   Copyright (C) 2010 M.Sutton (sutt@cern.ch)    
//
//   $Id: lumiList.h 765633 2016-07-31 08:52:35Z sutt $


#ifndef __LUMILIST_H
#define __LUMILIST_H

#include <iostream>
#include <map>


class lumiList : public std::map< int, std::vector<std::pair<int,int> > >  {

private:

  /// vector of start-end numbers for each run
  typedef  std::vector< std::pair<int,int> >    list_type;
 
  /// full map type 
  typedef  std::map< int, list_type >           full_type;

  /// iterator over the full map
  typedef  std::map< int, list_type >::iterator iterator;

public:
  
  lumiList(bool b=true) : mterse(b) {  } 

  ~lumiList() { } 

  void setterse(bool b) { mterse=b; }

  /// add a lumi block range for a given run
  void addRange(int _run, int _start, int _end) {

    static bool first = true;

    if ( first && mterse ) std::cout << "lumiList::addRange() terse output - only printing first range from each run" << std::endl;

    iterator it = find(_run);

    bool printout       = false;

    if ( it == end() ) {
      printout = true;
      std::pair< iterator, bool> in = insert( std::pair<int, list_type >( _run, list_type() ) );
      it = in.first;
      first = false;
    }
   
    it->second.push_back( std::pair<int,int>(_start,_end) );
   
    if ( !mterse || printout ) std::cout << "lumiList::addRange() run " << _run << ";\t" << _start << " - " << _end << std::endl;
  } 



  /// is this run, lumiblock in any of the ranges?
  /// if the run is not in the list, it is assumed 
  /// that all events from that run are required  
  bool inRange(int _run, int _lb) {

    //    std::map<int, list_type >::iterator itr  = mlist.find(run);
    std::map<int, list_type >::iterator itr  = find(_run);
    
    if ( itr == end() ) return true;
    else { 
      list_type& v = itr->second;
      for (unsigned int i=0 ; i<v.size() ; i++ ) { 
	if ( _lb>=v[i].first && _lb<=v[i].second ) return true; 
      }
    }
    return false; 
  }    

private: 

  bool mterse;

};


inline std::ostream& operator<<( std::ostream& s, const lumiList& _l ) { 
  return s << _l.size();
}


#endif  // __LUMILIST_H 










