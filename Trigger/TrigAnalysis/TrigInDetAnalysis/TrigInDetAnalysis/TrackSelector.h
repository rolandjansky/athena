// emacs: this is -*- c++ -*-
// 
// @file TrackSelector.h 
//
//

#ifndef __TRACKSELECTOR_H
#define __TRACKSELECTOR_H

#include <string>
#include <vector>

#include "TrigInDetAnalysis/TrackFilter.h"
#include "TrigInDetAnalysis/Track.h"



class TrackSelector {
  
public:
  
  TrackSelector( /*TrigInDetAnalysis::*/TrackFilter* selector=0 ) :  mselector(selector) {  } 
  virtual ~TrackSelector() {}

  // add a track, do the selection while adding?
  // or later with a method which loops through them all? 
  virtual bool addTrack(TrigInDetAnalysis::Track* t, bool (*f)(const TrigInDetAnalysis::Track*)=0 ) {
    //std::cout << "addtrack()  before f: t  " << *t << " " << size() << "\t f  " << f << std::endl;
    if ( f==0 ) { 
      if ( mselector && mselector->select(t) )  { mtracks.push_back(t);/*std::cout << "addtrack() after filter: t: " << *t << " f: " << f << "   " << size() << std::endl;*/ return true; }
      else                                      { cleanup(t); }
    }
    else { 
      if ( f(t) )  { mtracks.push_back(t); /*std::cout << "addtrack() no filter" << *t << " " << size() << std::endl;*/ return true; } 
      else         { cleanup(t); } 
    }
    return false;
  }

  virtual void addTracks(std::vector<TrigInDetAnalysis::Track*>& t, bool (*f)(const TrigInDetAnalysis::Track*)=0 ) {
    for ( unsigned i=0 ; i<t.size() ; i++ ){ addTrack( t[i], f ); /*std::cout << "addtrack() no filter called " << t[i] << " " << size() << std::endl;*/}
  }  
  
  // get the selected tracks   
  const std::vector<TrigInDetAnalysis::Track*>& tracks() const { return mtracks; }
  
  std::vector<TrigInDetAnalysis::Track*> tracks(  /*TrigInDetAnalysis::*/TrackFilter* selector ) const {
    if ( selector==0 ) return mtracks; 
    std::vector<TrigInDetAnalysis::Track*> t;
    for ( int i=mtracks.size() ; i-- ; ) if ( selector->select(mtracks[i]) ) t.push_back(mtracks[i]);
    return t;
  }

  // how many tracks in this selection
  unsigned size() const { return mtracks.size(); } 

  // clear tracks for this roi/event etc 
  //  void clear() { for ( int i=mtracks.size() ; i-- ; ) delete mtracks[i]; mtracks.clear(); } 
  virtual void clear() { mtracks.clear(); } 
  
protected:

  // can be used to delete tracks if they are not added to the selector to avoid memory 
  // leaks if the TrackSelector is managing the tracks itself. 
  virtual void cleanup(TrigInDetAnalysis::Track* ) { } 
  // virtual void cleanup(TrigInDetAnalysis::Track* t) { delete t; } 
  
protected:
  
  std::vector<TrigInDetAnalysis::Track*> mtracks;
 
  // selection function
  // static bool (*mselector)(const /*TrigInDetAnalysis::*/Track*);
  /*TrigInDetAnalysis::*/TrackFilter*  mselector;
 
};



#endif // __TRACKSELECTOR_H
