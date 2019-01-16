/* emacs: this is -*- c++ -*- */
/**
 **     @file    TrackTrigObject.h
 **
 **     @author  mark sutton
 **     @date    Mon  5 Nov 2012 00:07:15 GMT 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef  TRACKTRIGOBJECT_H
#define  TRACKTRIGOBJECT_H

#include <iostream>

#include "TrigInDetAnalysis/Track.h"

#include "TObject.h"

class TrackTrigObject : public TObject {

public:

  TrackTrigObject() { } /// just for root, don't want this really
  TrackTrigObject(double eta, double phi, double pt, double z0, 
		  int type, unsigned long id=0);

  TrackTrigObject(const TIDA::Track& t, 
		  int type, unsigned long id=0);

  virtual ~TrackTrigObject() { } 
  
  double eta() const { return m_eta; }
  double phi() const { return m_phi; }
  double pt()  const { return m_pt; }
  double z0()  const { return m_z0; }
  
  int  type()  const { return m_type; }

  unsigned long id() const { return m_id; }
  
  void addChild(unsigned long id) { m_children.push_back(id); }
  
  const std::vector<unsigned long>&  children() const { return m_children; }
  std::vector<unsigned long>         children()       { return m_children; }
  
private:
  
  /// parameters
  double m_eta;
  double m_phi;
  double m_pt;
  double m_z0;
  double m_d0;

  /// generaic type information
  int    m_type;

  /// identifier
  unsigned long m_id;

  /// identifiers of children
  std::vector<unsigned long> m_children; 

  ClassDef(TrackTrigObject,1);

};


inline std::ostream& operator<<( std::ostream& s, const TrackTrigObject& _t ) { 
  return s << "[" 
	   << "\ttype=" << _t.type()
	   << ":\teta=" << _t.eta()
	   << "\tphi="  << _t.phi()
	   << "\tpt="   << _t.pt()
	   << " ]" << std::endl;
}


#endif  // TRACKTRIGOBJECT_H 










