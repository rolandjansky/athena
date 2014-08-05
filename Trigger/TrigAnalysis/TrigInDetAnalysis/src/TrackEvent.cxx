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

