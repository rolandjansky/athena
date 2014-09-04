/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COSMICGENERATOR_COSMICEVENTPARSER_H
#define COSMICGENERATOR_COSMICEVENTPARSER_H

#include <iostream>
#include "CLHEP/Vector/ThreeVector.h"


class CosmicEventParser {
 public:
  CosmicEventParser() : m_eventNumber(0), m_pdgId(0) {};
  const CLHEP::HepLorentzVector& Vertex(void){return m_vertex;}
  const CLHEP::HepLorentzVector& Momentum(void){return m_momentum;}
  int        pdgID(void){return m_pdgId;}
 private:
  CLHEP::HepLorentzVector m_vertex;
  CLHEP::HepLorentzVector m_momentum;
  int        m_eventNumber;
  int        m_pdgId;
  std::istream&   read(std::istream& is);
  std::ostream&   write(std::ostream& os) const;

  friend std::istream& operator >> (std::istream& is,CosmicEventParser& ev);
  friend std::ostream& operator << (std::ostream& os,const CosmicEventParser& ev);
};

std::istream& operator >> (std::istream& is,CosmicEventParser& ev) {
  return ev.read(is);
}

std::ostream& operator << (std::ostream& os,const CosmicEventParser& ev) {
  return ev.write(os);
}

inline
std::istream& CosmicEventParser::read(std::istream& is)
{
  int dummy;
  int id;
  CLHEP::Hep3Vector vert,mom;
  double v_x,v_y,v_z;
  is >> m_eventNumber >> dummy >> id >> v_x >> v_y >> v_z >> mom;

  //
  // rotate over pi in x-z plane
  //
  vert.setX(-v_x);
  vert.setY( v_y);
  vert.setZ(-v_z);

  //
  // convert to MeV's and mm units
  //
  mom = 1000*mom;
  vert = 10*vert;


  m_vertex.setVect(vert);
  m_vertex.setE(0.);


  m_momentum.setVect(mom);
  double energy = sqrt(pow(105.66,2)+mom.mag2());
  m_momentum.setE(energy);

  if(id == 5) m_pdgId = 13;
  else m_pdgId = -13;
		
  return is;
}

inline
std::ostream& CosmicEventParser::write(std::ostream& os) const
{
  int dummy(1);
  int id(5);
  if(m_pdgId == -13) id = 6;
  os << m_eventNumber << " " << dummy << " " << id << " " 
     << m_vertex.x() << " " << m_vertex.y() << " " << m_vertex.z() << " " 
     << m_momentum.x() << " " << m_momentum.y() << " " << m_momentum.z();

  return os;
}


#endif
