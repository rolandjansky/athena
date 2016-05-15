// emacs: this is -*- c++ -*-
//
//   @file    TrackAnalysis.h        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: TrackAnalysis.h, v0.0   Sun 18 Jan 2009 19:53:18 GMT sutt $


#ifndef __TRACKANALYSIS_H
#define __TRACKANALYSIS_H

#include <iostream>
#include <vector>
#include <string>
#include <map>


#include "TrigInDetAnalysis/TrackAssociator.h"
#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TIDAVertex.h"
#include "TrigInDetAnalysis/TIDAFeatureStore.h"

#include "TH1.h"


class TrackAnalysis { 

public:

  TrackAnalysis( const std::string& name ) : 
    mname(name), 
    m_xBeamReference(0), m_yBeamReference(0), 
    m_xBeamTest(0),      m_yBeamTest(0) 
  { } 
  
  virtual ~TrackAnalysis() { } 

  /// return identifier
  std::string name() const { return mname; }  
    
  /// standard operation interface 
  virtual void initialise() = 0;
 
  virtual void execute( const std::vector<TIDA::Track*>& tracks1,
			const std::vector<TIDA::Track*>& tracks2,
			TrackAssociator* matcher ) = 0;

  virtual void execute_vtx( const std::vector<TIDA::Vertex*>& ,
			    const std::vector<TIDA::Vertex*>& ) { }  
			   
  
  virtual void finalise() = 0;

  // histogram map access - add a histogram
  void addHistogram( TH1* h ) { 
    std::string name = h->GetName();
    m_histos.insert( std::map<std::string, TH1*>::value_type( name, h) );
  }
  
  /// access the histograms
  const std::map<std::string, TH1*>& getHistograms() const { return m_histos; }
  
  std::map<std::string, TH1*>::const_iterator begin() const { return m_histos.begin(); }
  std::map<std::string, TH1*>::const_iterator end()   const { return m_histos.end(); }

  /// set the beamline positions
  void  setBeamRef( double x, double y, double z=0 ) {  m_xBeamReference = x;  m_yBeamReference = y; m_zBeamReference = z; } 
  void setBeamTest( double x, double y, double z=0 ) {  m_xBeamTest      = x;       m_yBeamTest = y;      m_zBeamTest = z; }

  TIDA::FeatureStore& store() { return m_store; }

protected:
 
  /// identifier of the of the analysis - also used for the root
  ///  directory into which the histograms are put
  std::string   mname;

  /// lookup table for the histograms by name - does this 
  /// need to be in the base class?  
  std::map<std::string, TH1*> m_histos;

  /// beamline positions
  /// reference sample 
  double m_xBeamReference;
  double m_yBeamReference;
  double m_zBeamReference;

  /// test sample
  double m_xBeamTest;
  double m_yBeamTest;
  double m_zBeamTest;

  TIDA::FeatureStore m_store;

};


inline std::ostream& operator<<(std::ostream& s, const TrackAnalysis& ta) { 
  return s << ta.name();
}


#endif  // __TRACKANALYSIS_H 










