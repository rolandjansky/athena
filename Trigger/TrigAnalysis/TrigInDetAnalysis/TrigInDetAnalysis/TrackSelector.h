// emacs: this is -*- c++ -*-
// 
// @file TrackSelector.h 
//
//

#ifndef TRIGINDETANALYSIS_TRACKSELECTOR_H
#define TRIGINDETANALYSIS_TRACKSELECTOR_H

#include <string>
#include <vector>

#include "TrigInDetAnalysis/TrackFilter.h"
#include "TrigInDetAnalysis/Track.h"



class TrackSelector {
  
public:
  
  TrackSelector( /*TrigInDetAnalysis::*/TrackFilter* selector=0 ) :  m_selector(selector) {  } 
  virtual ~TrackSelector() {}

  // add a track, do the selection while adding?
  // or later with a method which loops through them all? 
  virtual bool addTrack(TIDA::Track* t, bool (*f)(const TIDA::Track*)=0 ) {
    //std::cout << "addtrack()  before f: t  " << *t << " " << size() << "\t f  " << f << std::endl;
    if ( f==0 ) { 
      if ( m_selector && m_selector->select(t) )  { mtracks.push_back(t);/*std::cout << "addtrack() after filter: t: " << *t << " f: " << f << "   " << size() << std::endl;*/ return true; }
      else                                      { cleanup(t); }
    }
    else { 
      if ( f(t) )  { mtracks.push_back(t); /*std::cout << "addtrack() no filter" << *t << " " << size() << std::endl;*/ return true; } 
      else         { cleanup(t); } 
    }
    return false;
  }

  virtual void addTracks(std::vector<TIDA::Track*>& t, bool (*f)(const TIDA::Track*)=0 ) {
    for ( size_t i=0 ; i<t.size() ; i++ ){ addTrack( t[i], f ); /*std::cout << "addtrack() no filter called " << t[i] << " " << size() << std::endl;*/}
  }  
  
  // get the selected tracks   
  const std::vector<TIDA::Track*>& tracks() const { return mtracks; }
  
  std::vector<TIDA::Track*> tracks(  /*TrigInDetAnalysis::*/TrackFilter* selector ) const {
    if ( selector==0 ) return mtracks; 
    std::vector<TIDA::Track*> t;
    for ( size_t i=mtracks.size() ; i-- ; ) if ( selector->select(mtracks[i]) ) t.push_back(mtracks[i]);
    return t;
  }

  // how many tracks in this selection
  size_t size() const { return mtracks.size(); } 

  // clear tracks for this roi/event etc 
  //  void clear() { for ( int i=mtracks.size() ; i-- ; ) delete mtracks[i]; mtracks.clear(); } 
  virtual void clear() { mtracks.clear(); } 

  virtual void delete_tracks() { 
    for ( size_t i=size() ; i-- ; ) delete mtracks[i];
    clear(); 
  } 
  
protected:

  // can be used to delete tracks if they are not added to the selector to avoid memory 
  // leaks if the TrackSelector is managing the tracks itself. 
  virtual void cleanup(TIDA::Track* ) { } 
  // virtual void cleanup(TIDA::Track* t) { delete t; } 
  

public:  
  /// FIXME: public for now, to avoid warnings about naming convention
  ///        violations.  Should be fixed properly after run2 finishes.
  std::vector<TIDA::Track*> mtracks;
 
protected:
  // selection function
  // static bool (*m_selector)(const /*TrigInDetAnalysis::*/Track*);
  /*TrigInDetAnalysis::*/TrackFilter*  m_selector;
 
};



#endif // TRIGINDETANALYSIS_TRACKSELECTOR_H
