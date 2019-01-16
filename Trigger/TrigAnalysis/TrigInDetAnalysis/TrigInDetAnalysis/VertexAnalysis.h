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

  VertexAnalysis( const std::string& n ) : mname(n) { } 

  virtual ~VertexAnalysis() { } 

  std::string name() const { return mname; }
  
  virtual void initialise() = 0;
  
  virtual void execute( const std::vector<TIDA::Vertex*>& vtx0,
			const std::vector<TIDA::Vertex*>& vtx1, 
			const TIDA::Event* te=0 ) = 0;

  virtual void finalise() = 0;


  void addHistogram( TH1* h ) {
    //    std::string name = h->GetName();
    //   m_histos.insert( std::map<std::string, TObject*>::value_type( name, h) );
    m_histos.push_back( h );
  }


  void addHistogram( TProfile* h ) {
    //    std::string name = h->GetName();
    //   m_histos.insert( std::map<std::string, TObject*>::value_type( name, h) );
    m_profs.push_back( h );
  }

  //  void addHistogram( TProfile* h ) {
  //    std::string name = h->GetName();
  //    m_effhistos.insert( std::map<std::string, TProfile*>::value_type( name, h) );
  //  }

  std::vector<TH1*>       objects() { return m_histos; }
  std::vector<TProfile*>  profs()   { return m_profs; }

protected:

  //  std::map<std::string, TObject*>  m_histos;
  std::vector<TH1*>       m_histos;
  std::vector<TProfile*>  m_profs;
  //  std::map<std::string, TProfile*> m_effhistos;


private:

  std::string mname;

};

inline std::ostream& operator<<( std::ostream& s, const VertexAnalysis& _v ) { 
  return s << _v.name();
}


#endif  // VERTEXANALYSIS_H 










