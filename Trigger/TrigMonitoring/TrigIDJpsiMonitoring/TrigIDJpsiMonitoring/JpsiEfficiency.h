// emacs: this is -*- c++ -*- 
//
//   @file    JpsiEfficiency.h        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: TrackEfficiency.h, v0.0   Sun 18 Jan 2009 19:08:11 GMT sutt $

//edits P.Wijeratne (paw@hep.ucl.ac.uk)

#ifndef __JPSIEFFICIENCY_H 
#define __JPSIEFFICIENCY_H 


#include <iostream>

#include "TrigInDetAnalysis/TrackAnalysis.h"

// class Track;
// class Effplot;
// class Resplot;


class JpsiEfficiency : public TrackAnalysis { 
  
public:
  
  JpsiEfficiency( const std::string& name ) : 
    TrackAnalysis( name )
  { }  
  
  virtual void initialise();

  virtual void execute(const std::vector<TrigInDetAnalysis::Track*>& tracks1,
		       const std::vector<TrigInDetAnalysis::Track*>& tracks2,
		       TrackAssociator* matcher);

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


#endif // __JPSIEFFICIENCY_H 
