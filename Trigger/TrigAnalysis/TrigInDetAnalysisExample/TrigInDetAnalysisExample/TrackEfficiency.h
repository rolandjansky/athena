/* emacs: this is -*- c++ -*- */
/**
 **     @file    TrackEfficiency.h
 **
 **     @author  mark sutton
 **     @date    Sun 18 Jan 2009 19:08:11 GMT 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TIDAEXAMPLE_TRACKEFFICIENCY_H 
#define TIDAEXAMPLE_TRACKEFFICIENCY_H 


#include <iostream>

#include "TrigInDetAnalysis/TrackAnalysis.h"

// class Track;
// class Effplot;
// class Resplot;


class TrackEfficiency : public TrackAnalysis { 
  
public:

  using TrackAnalysis::execute;

public:
  
  TrackEfficiency( const std::string& name ) : 
    TrackAnalysis( name )
  { }  
  
  virtual void initialise();

  virtual void execute(const std::vector<TIDA::Track*>& tracks1,
		       const std::vector<TIDA::Track*>& tracks2,
		       TrackAssociator* matcher );

  virtual void finalise();
  
private:

#if 0
  // efficiency histograms
  Effplot*  effvsPT;
  Effplot*  effvseta;
  Effplot*  effvsphi;
  Effplot*  effvsd0;

  // resolution histograms 
  Resplot* iptvipt;
  Resplot* ptvpt;
  Resplot* phivpt;
  Resplot* etavpt;
  Resplot* z0vpt;
  Resplot* d0vpt;

  Resplot* ptveta;
  Resplot* iptveta;
  Resplot* phiveta;
  Resplot* etaveta;
  Resplot* z0veta;

  Resplot* z0vz0;
  Resplot* d0vd0;

#endif

};


#endif // TIDAEXAMPLE_TRACKEFFICIENCY_H 
