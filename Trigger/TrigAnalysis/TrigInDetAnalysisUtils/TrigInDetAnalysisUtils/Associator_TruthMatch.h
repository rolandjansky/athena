/* emacs: this is -*- c++ -*- */
/**
 **     @file    Associator_TruthMatch.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:39 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TIDAUTILS_ASSOCIATOR_TRUTHMATCH_H
#define TIDAUTILS_ASSOCIATOR_TRUTHMATCH_H

#include <set>
#include <map>


//BP next two
// #include "TrigInDetEvent/TrigInDetTrackCollection.h"
// #include "McParticleEvent/TruthParticleContainer.h"

///TruthMap
//#include "TrigInDetTruthEvent/TrigInDetTrackTruthMap.h"
//##include "HepMC/GenEvent.h"
//##include "HepMC/GenVertex.h"

//BP
// #include "HepMC/GenParticle.h"
// #include "GeneratorObjects/HepMcParticleLink.h"


#include "TrigInDetAnalysis/TrackAssociator.h"
#include "TrigInDetAnalysisUtils/Associator_BestMatch.h"
#include "TrigInDetAnalysis/Track.h"



#if 0
class Associator_TruthMatcher : public Associator_BestMatcher {

public:
  Associator_TruthMatcher() :
    Associator_MatcherBase( "Truth" )
  { }

  virtual double distance( TIDA::Track* refTrack, TIDA::Track* testTrack ) {
    if      (testTrack->match_barcode() == -1)                  return 1;
    else if (testTrack->match_barcode() == refTrack->barcode()) return 0;
    else                                                        return 1;
  }
  
};
#endif


// class Associator_TruthMatcher : public Associator_MatcherBase{
class Associator_TruthMatcher : public TrackAssociator {
  
public:

  Associator_TruthMatcher() : TrackAssociator("Truth")  { }
  
  virtual ~Associator_TruthMatcher() { }
  
  //Fill reference tracks in matching step
  virtual void match( const std::vector<TIDA::Track*>& refTracks, 
		      const std::vector<TIDA::Track*>& testTracks) {

    //std::cout<<"refTracks.size() "<<refTracks.size()<<" \t testTracks.size() "<<testTracks.size()<<std::endl;
    
    for (unsigned int i = 0; i < refTracks.size(); i++) {
      
      //        std::cout<<refTracks[i]->author() <<std::endl;
      
      for (unsigned int j = 0; j < testTracks.size(); j++) {

	//  if(testTracks[j]->match_barcode()!=-1){
	//    std::cout << "ref barcode" << refTracks[i]->barcode() << std::endl;
	//    std::cout << "test barcode " << testTracks[j]->match_barcode() << std::endl;
	//  }
	
	if ( distance( refTracks[i], testTracks[j] ) < 1. ) {
	  //              std::cout<<"MATCHED"<<std::endl;
	  mmatched.insert(    map_type::value_type(refTracks[i],testTracks[j]));
	  mrevmatched.insert( map_type::value_type(testTracks[j],refTracks[i]));
	}
      }
    }
    
    return;
  }
  
  
  virtual double distance( TIDA::Track* refTrack, TIDA::Track* testTrack ) {
      if      (testTrack->match_barcode() == -1)                   return 1;
      else if (testTrack->match_barcode() == refTrack->barcode())  return 0;
      else                                                         return 1;
  }
  
private:

  // double md;

};


#endif //  TIDAUTILS_ASSOCIATOR_TRUTHMATCH_H
                                                        
