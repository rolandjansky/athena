/* emacs: this is -*- c++ -*- */
/**
 **   @file    TrigObjectMatcher.h        
 **                    
 **   @author  sutt
 **   @date    Fri 14 Sep 2018
 **
 **   $Id: TrigObjectMatcher.h, v0.0   Fri 14 Sep 2018 15:55:41 CEST sutt $
 **
 **   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 **/

#ifndef  TRIGOBJECTMATCHER_H
#define  TRIGOBJECTMATCHER_H

#include <iostream>
#include <map>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TrackSelector.h"
#include "TrigInDetAnalysis/TrackTrigObject.h"


class TrigObjectMatcher {

public:

  typedef std::map<unsigned long, const TrackTrigObject*> map_type;

public:

  TrigObjectMatcher() : m_status(0) { } 

  TrigObjectMatcher(  const std::vector<TIDA::Track*>&      tracks, 
		      const std::vector<TrackTrigObject>& objects  ) 
    : m_status(1) {         
    for ( int i=tracks.size() ; i-- ; ) match( tracks[i]->id(), objects );
  } 


  TrigObjectMatcher(  TrackSelector* selector, 
		      const std::vector<TrackTrigObject>& objects, 
		      bool (*select_function)(const TrackTrigObject& t)=0 ) 
    : m_status(1) {         
    const std::vector<TIDA::Track*> tracks = selector->tracks();
    for ( int i=tracks.size() ; i-- ; ) if ( match( tracks[i]->id(), objects, select_function )==false ) selector->delete_track( tracks[i] );
  } 


  virtual ~TrigObjectMatcher() { } 


  const TrackTrigObject* object( unsigned long track_id) {
    map_type::const_iterator itr = m_objectmap.find( track_id );
    if ( itr==m_objectmap.end() ) return 0;
    return itr->second;
  } 

  bool status() const { return m_status; }

  map_type::const_iterator  begin() const { return m_objectmap.begin(); } 
  map_type::const_iterator  end()   const { return m_objectmap.end(); } 

  size_t size() const { return m_objectmap.size(); }   

  void print( std::ostream& s ) const { 
    s << "TrigObjectMatch size: " << m_objectmap.size();
    for ( map_type::const_iterator itr = m_objectmap.begin() ; itr!=m_objectmap.end() ; itr++ ) { 
      s << "\n[ id: 0x" << std::hex << itr->first << std::dec << "  ::  " << *(itr->second) << " ]";
    }
  }

private:

  bool  match( unsigned long track_id, const std::vector<TrackTrigObject>& objects, bool (*select_function)(const TrackTrigObject& t)=0 ) { 
    for ( int j=objects.size() ; j-- ; ) { 
      for ( int k=objects[j].children().size() ; k-- ; ) {
	if ( track_id == objects[j].children()[k] ) {
	  if ( select_function==0 || select_function(objects[j]) ) { 
	    m_objectmap.insert( map_type::value_type( track_id, &objects[j] ) );
	    return true;
	  }
	  else return false;
	}
      } 
    }
    return false;
  } 


private:
  
  int      m_status;
  map_type m_objectmap;

};

inline std::ostream& operator<<( std::ostream& s, const TrigObjectMatcher& o ) { 
  o.print( s );
  return s;
}


#endif  // TRIGOBJECTMATCHER_H 










