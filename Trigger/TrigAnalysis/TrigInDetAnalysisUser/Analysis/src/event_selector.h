/* emacs: this is -*- c++ -*- */
/**
 **     @file    event_selector.h
 **
 **     @author  mark sutton
 **     @date    $Id: event_selector.h 513250 2012-08-10 14:35:11Z 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef ANALYSIS_EVENT_SELECTOR_H
#define ANALYSIS_EVENT_SELECTOR_H

#include <iostream>
#include <set>



class event_selector : public std::set<int> {

public:

  template<class T>
  event_selector(std::vector<T> e) { 
    for (unsigned int i=0 ; i<e.size() ; i++ ) insert(int(e[i]));
  } 

  //  event_selector(std::vector<double> e) { 
  //    for (unsigned int i=0 ; i<e.size() ; i++ ) insert(int(e[i]));
  //  } 

  ~event_selector() { } 

  bool in(int event) { 
    if ( find(event)==end() ) return false;
    std::cout << "event_selector::in() found " << event << std::endl; 
    return true;
  }

};


inline std::ostream& operator<<( std::ostream& s, const event_selector& _e ) { 
  return s << _e.size();
}


#endif  // ANALYSIS_EVENT_SELECTOR_H 










