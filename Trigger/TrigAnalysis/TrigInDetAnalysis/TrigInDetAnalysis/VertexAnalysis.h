// emacs: this is -*- c++ -*-
//
//   @file    VertexAnalysis.h        
//
//                   
//  
//   Copyright (C) 2015 M.Sutton (sutt@cern.ch)    
//
//   $Id: VertexAnalysis.h, v0.0   Fri  7 Aug 2015 10:27:38 CEST sutt $


#ifndef  VERTEXANALYSIS_H
#define  VERTEXANALYSIS_H

#include <iostream>


#include "TH1.h"

#include "TrigInDetAnalysis/TIDAVertex.h"


class VertexAnalysis {

public:

  VertexAnalysis( const std::string& n ) : mname(n) { } 

  virtual ~VertexAnalysis() { } 

  std::string name() const { return mname; }
  
  virtual void initialise() = 0;
  
  virtual void execute( const std::vector<TIDA::Vertex*>& vtx0,
			const std::vector<TIDA::Vertex*>& vtx1 ) = 0;
  
  virtual void finalise() = 0;


private:

  std::string mname;

};

inline std::ostream& operator<<( std::ostream& s, const VertexAnalysis& _v ) { 
  return s << _v.name();
}


#endif  // VERTEXANALYSIS_H 










