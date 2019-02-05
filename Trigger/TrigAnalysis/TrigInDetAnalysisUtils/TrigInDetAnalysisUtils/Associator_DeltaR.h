/* emacs: this is -*- c++ -*- */
/**
 **     @file    Associator_DeltaR.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:39 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/



#ifndef TrigInDetAnalysisUtils_Associator_DeltaR_H
#define TrigInDetAnalysisUtils_Associator_DeltaR_H


#include <iostream>
#include <string>
#include <cmath>
#include <map>

#include "TrigInDetAnalysis/TrackAssociator.h"
#include "TrigInDetAnalysis/Track.h"



/// We Don't need these "non-best" matcher versions now 


#if 0

class Associator_DeltaR : public TrackAssociator {

public:
  
  Associator_DeltaR(const std::string& name, double deltaR) : TrackAssociator(name), m_deltaR2(deltaR*deltaR) {} 
  
  ~Associator_DeltaR() { } 
  
  virtual void match(const std::vector<TIDA::Track*>& referenceTracks, 
		     const std::vector<TIDA::Track*>& testTracks) {

    // Clear previously filled association map
    clear();
    
    // Loop over reference tracks
    std::vector<TIDA::Track*>::const_iterator reference, referenceEnd=referenceTracks.end();
    for(reference=referenceTracks.begin(); reference!=referenceEnd; reference++) {
      
      // Loop over test tracks and find the closest
      TIDA::Track* bestMatch = NULL;
      double bestDeltaR=1000;
      
      std::vector<TIDA::Track*>::const_iterator test, testEnd=testTracks.end();
      for(test=testTracks.begin(); test!=testEnd; test++) {

	// Evaluate distance between reference and test tracks
	double deta = (*reference)->eta() - (*test)->eta();
	double dphi = (*reference)->phi() - (*test)->phi();
	if(dphi>M_PI)  dphi-=2*M_PI;
	if(dphi<-M_PI) dphi+=2*M_PI;
	double deltaR =  deta*deta+dphi*dphi;

	// Check if this is the best match so far
	if(bestMatch==NULL || deltaR<bestDeltaR) { 
	  bestDeltaR = deltaR;
	  bestMatch  = (*test);
	} 
      }

      // Check if the best match is within delta R specifications
      if(bestMatch && bestDeltaR<m_deltaR2) { 
	// Create reference->test and test->reference associations
	mmatched.insert(map_type::value_type(*reference, bestMatch));
	mrevmatched.insert(map_type::value_type(bestMatch, *reference));
      }
    }
  }
  
private:
  
  double m_deltaR2;
  
};
#endif

#endif  // TrigInDetAnalysisUtils_Associator_DeltaR_H
