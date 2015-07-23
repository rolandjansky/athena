//
//   @file    TrackEvent.cxx         
//   
//            Basic event class to contain a vector of
//            chains for trigger analysis      
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: TrackEvent.cxx, v0.0   Tue  2 Feb 2010 00:09:33 GMT sutt $

#include "TrigInDetAnalysis/TrackEvent.h"

ClassImp(TrackEvent)

TrackEvent::TrackEvent() : 
  m_run_number(0), 
  m_event_number(0), 
  m_lumi_block(0),
  m_time_stamp(0),
  m_bunch_crossing_id(0)
{ }

TrackEvent::~TrackEvent() { } 

std::vector<std::string> TrackEvent::chainnames() const { 
  std::vector<std::string> names;
  for ( std::vector<TrackChain>::const_iterator citr=begin() ; citr!=end() ; citr++ ) names.push_back( citr->name() );
  return names; 
}



void TrackEvent::erase( const std::string& name ) { 
    
  /// WHY DOES erase() NOT WORK ON THESE std::vector<TrackChain> Almost certainly 
  /// because we are using actual TObjects, ***REMOVED*** !!!
  /// so instead we need to copy all the chains, and then copy them back *if* 
  /// we don;t want to delete it ***REMOVED***!  

  std::vector<TrackChain> _chains = m_chains;
  m_chains.clear();

  std::vector<TrackChain>::const_iterator _itr = _chains.begin();

  while ( _itr!=_chains.end() ) { 
    if ( _itr->name()!=name ) m_chains.push_back( *_itr );
    _itr++;
  }

} 



