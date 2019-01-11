/* emacs: this is -*- c++ -*- */
/**
 **     @file    Associator_DeltaEtaPhi.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:39 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/

/// Fixme: needs to be replaced by something inheriting 
///        from BestMatcher


#ifndef TrigInDetAnalysisUtils_Associator_DeltaEtaPhi_H
#define TrigInDetAnalysisUtils_Associator_DeltaEtaPhi_H


#include <iostream>
#include <string>
#include <cmath>
#include <map>

#include "TrigInDetAnalysis/TrackAssociator.h"
#include "TrigInDetAnalysis/Track.h"


class Associator_DeltaEtaPhi : public TrackAssociator {

public:

  Associator_DeltaEtaPhi(const std::string& name, double deltaEta, double deltaPhi) : TrackAssociator(name), m_deltaEta(deltaEta), m_deltaPhi(deltaPhi)  {} 
  
  ~Associator_DeltaEtaPhi() { } 
  
  virtual void match(const std::vector<TIDA::Track*>& referenceTracks, 
		     const std::vector<TIDA::Track*>& testTracks) {
    
    // Clear previously filled association map
    clear();
    
    
    // Loop over reference tracks
    std::vector<TIDA::Track*>::const_iterator reference, referenceEnd=referenceTracks.end();
    for(reference=referenceTracks.begin(); reference!=referenceEnd; reference++) {
      
      // std::cout << "Offline = " << (*reference)->eta() << "  " << (*reference)->phi() << "  " << (*reference)->pT() << std::endl; 
      
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
	
	if (fabs(deta)>m_deltaEta || fabs(dphi)>m_deltaPhi) continue;
	
	double deltaR =  (deta*deta)/(m_deltaEta*m_deltaEta)+(dphi*dphi)/(m_deltaPhi*m_deltaPhi);

	// Check if this is the best match so far
	if(bestMatch==NULL || deltaR<bestDeltaR) { 
	  bestDeltaR = deltaR;
	  bestMatch  = (*test);
	} 
      }
      
      // Check if the best match is within delta R specifications
      if(bestMatch) { 
	// Create reference->test and test->reference associations
	mmatched.insert(map_type::value_type(*reference, bestMatch));
	mrevmatched.insert(map_type::value_type(bestMatch, *reference));
	//std::cout << "Matched = " << bestMatch->eta() << "  "<< bestMatch->phi() << "  " << bestMatch->pT() << std::endl;
      }
    }
  }
  
private:
  
  double m_deltaEta;
  double m_deltaPhi;
  
};


#endif  // TrigInDetAnalysisUtils_Associator_DeltaEtaPhi_H

