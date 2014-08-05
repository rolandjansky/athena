// emacs: this is -*- c++ -*-
//
//   @file    TrackAssociator.h        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: TrackAssociator.h, v0.0   Sun 18 Jan 2009 19:08:11 GMT sutt $


#ifndef __TRACKASSOCIATOR_H
#define __TRACKASSOCIATOR_H

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <map>


#include "TrigInDetAnalysis/Track.h"


class TrackAssociator {

public:

  typedef std::map<TrigInDetAnalysis::Track*, TrigInDetAnalysis::Track*> track_map; 

public:

  TrackAssociator(const std::string& name)
    :  mname(name) 
  { } 
  
  virtual ~TrackAssociator() { } 
  
  virtual void match(const std::vector<TrigInDetAnalysis::Track*>& s1, 
		     const std::vector<TrigInDetAnalysis::Track*>& s2 ) = 0;
  
  // get matched track from map
  virtual const TrigInDetAnalysis::Track* matched( TrigInDetAnalysis::Track* t) { 
     track_map::const_iterator titr = mmatched.find(t);
     if ( titr != mmatched.end() ) return titr->second;
     else                          return 0;
  } 

  // get matched track from reverse map
  virtual const TrigInDetAnalysis::Track* revmatched( TrigInDetAnalysis::Track* t) { 
     track_map::const_iterator titr = mrevmatched.find(t);
     if ( titr != mrevmatched.end() ) return titr->second;
     else                             return 0;
  } 

  // get the lookup tables
  const track_map&    matched() const { return mmatched; }
  const track_map& revmatched() const { return mrevmatched; }

  // clear the match lookup tables
  void clear() { mmatched.clear(); mrevmatched.clear(); } 

  // how many items were matched
  unsigned size() const { return mmatched.size(); }

protected:

  std::string mname;
  
  track_map mmatched;
  track_map mrevmatched;

};


inline std::ostream& operator<<(std::ostream& s, const TrackAssociator& ) { 
  return s;
}


#endif  // __TRACKASSOCIATOR_H 










