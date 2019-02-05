/* emacs: this is -*- c++ -*- */
/**
 **     @file    VertexAnalysis.h
 **
 **     @author  mark sutton
 **     @date    Fri  7 Aug 2015 10:27:38 CEST 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef  VERTEXANALYSIS_H
#define  VERTEXANALYSIS_H

#include <iostream>


#include "TH1.h"
#include "TProfile.h"

#include "TrigInDetAnalysis/TIDAEvent.h"
#include "TrigInDetAnalysis/TIDAVertex.h"


class VertexAnalysis {

public:

  VertexAnalysis( const std::string& n ) : m_name(n) { } 

  virtual ~VertexAnalysis() { } 

  std::string name() const { return m_name; }
  
  virtual void initialise() = 0;
  
  virtual void execute( const std::vector<TIDA::Vertex*>& vtx0,
			const std::vector<TIDA::Vertex*>& vtx1, 
			const TIDA::Event* te=0 ) = 0;

  virtual void finalise() = 0;

  void addHistogram( TH1* h ) {  m_histos.push_back( h ); }

  void addHistogram( TProfile* h ) {  m_profs.push_back( h ); }

  std::vector<TH1*>       objects() { return m_histos; }
  std::vector<TProfile*>  profs()   { return m_profs; }

protected:

  std::vector<TH1*>       m_histos;
  std::vector<TProfile*>  m_profs;

private:

  std::string m_name;

};

inline std::ostream& operator<<( std::ostream& s, const VertexAnalysis& v ) { 
  return s << v.name();
}


#endif  // VERTEXANALYSIS_H 










