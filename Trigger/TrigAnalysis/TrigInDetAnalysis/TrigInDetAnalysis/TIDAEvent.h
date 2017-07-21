// emacs: this is -*- c++ -*-
//
//   @file    TIDAEvent.h        
//
//            Basic event class to contain a vector of
//            chains for trigger analysis                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: TIDAEvent.h, v0.0   Mon  1 Feb 2010 11:43:51 GMT sutt $


#ifndef TRIGINDETANALYSIS_TIDAEVENT_H
#define TRIGINDETANALYSIS_TIDAEVENT_H

#include <iostream>
#include <vector>
#include <string>

#include "TrigInDetAnalysis/TIDAChain.h"
#include "TrigInDetAnalysis/TIDAVertex.h"
//#include "TrigInDetTruthEvent/TrigInDetTrackTruthMap.h"


#include "TObject.h"



namespace TIDA { 

class Event : public TObject {

public:

  Event();

  virtual ~Event();

  /// accessors
  void run_number(unsigned  r)              { m_run_number     = r; }  
  void event_number(unsigned long long e)   { m_event_number   = e; }  
  void lumi_block(unsigned lb)    { m_lumi_block     = lb; }  
  void time_stamp(unsigned t)     { m_time_stamp     = t; }  
  void bunch_crossing_id(unsigned b) { m_bunch_crossing_id = b; }  
	void mu(double m) { m_mu = m;}

  unsigned run_number()               const { return m_run_number;   } 
  unsigned long long event_number()   const { return m_event_number; }  
  unsigned lumi_block()     const { return m_lumi_block;   }  
  unsigned time_stamp()     const { return m_time_stamp;   }  
  unsigned bunch_crossing_id() const { return m_bunch_crossing_id;  }  

  /// FIXME: what is this ? need a comment describing any not 
  ///        descriptive variable name
  double mu() const { return m_mu; } /// vertex multiplicity ?   

  /// NB all these could be avoided simply be inheriting 
  ///    from and std::vector<TIDA::Chain> rather than 
  ///    a member variable 

  /// number of chains added to this event
  unsigned size() const { return m_chains.size(); } 

  /// methods to add and access chains 
  void addChain(const std::string& chainname) { 
     m_chains.push_back(TIDA::Chain(chainname));
  }

  void addVertex(const TIDA::Vertex& v) { 
     m_vertices.push_back(v);
  }


  const std::vector<TIDA::Chain>& chains() const { return m_chains; };
  std::vector<TIDA::Chain>&       chains()       { return m_chains; };
  
  //void setTruthMap(TrigInDetTrackTruthMap truthmap) {
  //	m_truthmap = truthmap;
  //}
  
  /// clear the event
  void clear() { m_chains.clear(); m_vertices.clear(); } 
 
  /// get the last chain from the vector 
  TIDA::Chain& back() { return m_chains.back(); }
  
  // iterators
  std::vector<TIDA::Chain>::iterator begin() { return m_chains.begin(); }
  std::vector<TIDA::Chain>::iterator end()   { return m_chains.end(); }

  std::vector<TIDA::Chain>::const_iterator begin() const { return m_chains.begin(); }
  std::vector<TIDA::Chain>::const_iterator end()   const { return m_chains.end(); }
  
  /// vector operator
  TIDA::Chain& operator[](int i) { return m_chains.at(i); }

  const std::vector<TIDA::Vertex>& vertices() const { return m_vertices; }

  std::vector<std::string> chainnames() const;

  void erase( const std::string& name );


private:
 
  unsigned m_run_number; 
  unsigned long long m_event_number;  
  unsigned m_lumi_block;
  unsigned m_time_stamp;

  unsigned m_bunch_crossing_id;
  double   m_mu;  /// vertex multiplicity ?   

  /// trigger chain information
  std::vector<TIDA::Chain> m_chains;

  std::vector<TIDA::Vertex> m_vertices;
  
  ClassDef(TIDA::Event,4)

};

}

inline std::ostream& operator<<( std::ostream& s, const TIDA::Event& t ) { 
  s << "Event run: " << t.run_number() 
    << "\tevent: "   << t.event_number() 
    << "\tlb: "      << t.lumi_block() 
    << "\tbc: "      << t.bunch_crossing_id()
    << "\ttime: "    << t.time_stamp();
  for ( unsigned i=0 ; i<t.vertices().size() ; i++ ) s << "\n" << t.vertices()[i];
  for ( unsigned i=0 ; i<t.chains().size()   ; i++ ) s << "\n" << t.chains()[i];
  return s;
}


#endif  // TRIGINDETANALYSIS_TIDAEVENT_H










