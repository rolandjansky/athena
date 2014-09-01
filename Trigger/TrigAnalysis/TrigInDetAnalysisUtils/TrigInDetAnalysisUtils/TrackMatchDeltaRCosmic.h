// emacs: this is -*- c++ -*-
//
//   @file    TrackMatchDeltaRCosmic.h        
//            
//            checks if track is within DeltaR of cosmic, or within 
//            DeltaR back to back       
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: TrackMatchDeltaRCosmic.h, v0.0   Sun 18 Jan 2009 19:40:47 GMT sutt $


#ifndef __TRACKMATCHDELTARCOSMIC_H
#define __TRACKMATCHDELTARCOSMIC_H

#include <iostream>
#include <string>
#include <cmath>
#include <map>


#include "TrigInDetAnalysis/TrackAssociator.h"
#include "TrigInDetAnalysis/Track.h"


class TrackMatchDeltaRCosmic : public TrackAssociator {

public:

  TrackMatchDeltaRCosmic(const std::string& name, double R) : 
    TrackAssociator(name), mR2(R*R) 
  { } 

  ~TrackMatchDeltaRCosmic() { } 

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

	double R2tmp   =   deltaR2( reftrack, testtrack );
	double R2PItmp = deltaR2PI( reftrack, testtrack );
	
	if ( R2PItmp<R2tmp ) R2tmp = R2PItmp;

	// found a close track
	if ( tmptrack==NULL || R2tmp<R2min ) { 
	  R2min    = R2tmp;
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
   

  virtual double deltaR2PI( const TrigInDetAnalysis::Track* reftrack, const TrigInDetAnalysis::Track* testtrack ) {
    double deta = reftrack->eta()+testtrack->eta();
    double dphi = reftrack->phi()-testtrack->phi()-M_PI;
    if ( dphi> M_PI ) dphi-=2*M_PI;
    if ( dphi<-M_PI ) dphi+=2*M_PI;
    return deta*deta+dphi*dphi;
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


inline std::ostream& operator<<(std::ostream& s, const TrackMatchDeltaRCosmic& /*tm*/) { 
  return s;
}


#endif  // __TRACKMATCHDELTAR_H 










