// emacs: this is -*- c++ -*-
//
//   @file    TrackMatchDeltaR.h        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: TrackMatchDeltaR.h, v0.0   Sun 18 Jan 2009 19:40:47 GMT sutt $


#ifndef __TRACKMATCHDELTAR_H
#define __TRACKMATCHDELTAR_H

#include <iostream>
#include <string>
#include <cmath>
#include <map>


#include "TrigInDetAnalysis/TrackAssociator.h"
#include "TrigInDetAnalysis/Track.h"


class TrackMatchDeltaR : public TrackAssociator {

public:

  TrackMatchDeltaR(const std::string& name, double R) : 
    TrackAssociator(name), mR2(R*R) 
  { } 

  ~TrackMatchDeltaR() { } 

  //  virtual void match( const std::vector<Track*>& ref, const std::vector<Track*>& test ) { 
  virtual void match(const std::vector<TrigInDetAnalysis::Track*>& s1, 
		     const std::vector<TrigInDetAnalysis::Track*>& s2 ) 
  {
    clear();
    
    const std::vector<TrigInDetAnalysis::Track*>& ref  = s1; 
    const std::vector<TrigInDetAnalysis::Track*>& test = s2;

    // loop over reference tracks
    for ( int i=ref.size() ; i-- ; ) {
      
      TrigInDetAnalysis::Track* reftrack = ref[i];
      
      //  find the closest track
      TrigInDetAnalysis::Track* tmptrack = NULL;
      double R2min = 0;

      // loop over test tracks
      for ( int j=test.size() ; j-- ; ) { 

	TrigInDetAnalysis::Track* testtrack = test[j];

	double R2tmp = deltaR2( reftrack, testtrack );

	// found a close track
	if ( tmptrack==NULL ||  R2tmp<R2min ) { 
	  R2min = R2tmp;
	  tmptrack = testtrack;
	} 
      }

      // is this inside the delta R specification?
      if ( tmptrack && R2min<mR2 ) { 
	mmatched.insert(    track_map::value_type(reftrack,tmptrack) );
	mrevmatched.insert( track_map::value_type(tmptrack,reftrack) );
	
	std::cout << "\t\tSUTT matched " << *reftrack  << "\t -> \t" << *tmptrack << "\tDr=" << R2min << std::endl; 
      }
    }
  }
   

  virtual double deltaR2( const TrigInDetAnalysis::Track* reftrack, const TrigInDetAnalysis::Track* testtrack ) {
    double deta = reftrack->eta()-testtrack->eta();
    double dphi = reftrack->phi()-testtrack->phi();
    if ( dphi> M_PI ) dphi-=2*M_PI;
    if ( dphi<-M_PI ) dphi+=2*M_PI;
    return deta*deta+dphi*dphi;
  } 

private:

  double mR2;

};


inline std::ostream& operator<<(std::ostream& s, const TrackMatchDeltaR& /*tm*/) { 
  return s;
}


#endif  // __TRACKMATCHDELTAR_H 










