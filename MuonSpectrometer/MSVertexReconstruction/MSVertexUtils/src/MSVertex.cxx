/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MSVertexUtils/MSVertex.h"

#include <sstream>
#include <math.h>

MSVertex::MSVertex() :
  m_author(0),
  m_position(),
  m_tracks(0),
  m_chi2prob(-1.),
  m_chi2(-1.),
  m_nMDT(-1), 
  m_nRPC(-1), 
  m_nTGC(-1)
{}

MSVertex::MSVertex(int author, const Amg::Vector3D& position,
		   float chi2prob, float chi2, int nMDT, int nRPC, int nTGC) : 
  m_author(author),
  m_position(position),
  m_tracks(0),
  m_chi2prob(chi2prob),
  m_chi2(chi2),
  m_nMDT(nMDT), 
  m_nRPC(nRPC), 
  m_nTGC(nTGC)
{}

MSVertex::MSVertex(int author, const Amg::Vector3D& position, const std::vector<xAOD::TrackParticle*>& tracks,
		   float chi2prob, float chi2, int nMDT, int nRPC, int nTGC) : 
  m_author(author),
  m_position(position),
  m_tracks(tracks),
  m_chi2prob(chi2prob),
  m_chi2(chi2),
  m_nMDT(nMDT), 
  m_nRPC(nRPC), 
  m_nTGC(nTGC)
{}

MSVertex::~MSVertex() {
  for (std::vector<xAOD::TrackParticle*>::iterator i = m_tracks.begin(); i != m_tracks.end(); ++i) {
    if( (*i) ) {
      delete (*i);
      (*i) = 0;
    }
  }
  m_tracks.clear();
}

MSVertex* MSVertex::clone() {
  //copy the track vector
  std::vector<xAOD::TrackParticle*> trkcopy;
  trkcopy.reserve(m_tracks.size());
  for (std::vector<xAOD::TrackParticle*>::iterator i = m_tracks.begin(); i != m_tracks.end(); ++i) {
    trkcopy.push_back( new xAOD::TrackParticle( **i ) );
  }
  MSVertex* copy = new MSVertex(m_author,m_position,trkcopy,m_chi2prob,m_chi2,m_nMDT,m_nRPC,m_nTGC);
  return copy;
}

void MSVertex::setPosition(const Amg::Vector3D& position) { m_position=position; }

const Amg::Vector3D& MSVertex::getPosition() const { return m_position; }

const std::vector<xAOD::TrackParticle*>* MSVertex::getTracks(void) const { return &m_tracks; }

void  MSVertex::setAuthor(const int author) { m_author=author; }

int   MSVertex::getAuthor()       const { return m_author; }
float MSVertex::getChi2Probability()         const { return m_chi2prob;   }
float MSVertex::getChi2()         const { return m_chi2; }

int   MSVertex::getNTracks()      const {
  if (getTracks()) return getTracks()->size();
  else             return 0;
}

void MSVertex::setNMDT(const int nMDT) { m_nMDT=nMDT; }
void MSVertex::setNRPC(const int nRPC) { m_nRPC=nRPC; }
void MSVertex::setNTGC(const int nTGC) { m_nTGC=nTGC; }

int MSVertex::getNMDT() const { return m_nMDT; }
int MSVertex::getNRPC() const { return m_nRPC; }
int MSVertex::getNTGC() const { return m_nTGC; }

std::string str (const MSVertex& a) {
  
  std::stringstream ss;
  
  ss << "author = "   << a.getAuthor()
     << "; x = " << a.getPosition().x()
     << "; y = " << a.getPosition().y()
     << "; z = " << a.getPosition().z()
     << "; phi = "  << a.getPosition().phi()
     << "; eta = "  << a.getPosition().eta()
     << "; chi2 prob. = "  << a.getChi2Probability()
     << "; # tracks = "  << a.getNTracks()
     << "; # MDT hits = "  << a.getNMDT()
     << "; # RPC hits = "  << a.getNRPC()
     << "; # TGC hits = "  << a.getNTGC();
  
  return ss.str();
}


MsgStream& operator<< (MsgStream& m, const MSVertex& a) {
  
  return (m << str(a));
}


bool operator== (const MSVertex& a, const MSVertex& b) {
  
  //* distance used to compare floats *//
  const double DELTA=1e-3;
  
  if(fabsf(a.getPosition().x()   - b.getPosition().x())   > DELTA) return false;
  if(fabsf(a.getPosition().y()   - b.getPosition().y())   > DELTA) return false;
  if(fabsf(a.getPosition().z()   - b.getPosition().z())   > DELTA) return false;
  if(fabsf(a.getPosition().eta() - b.getPosition().eta()) > DELTA) return false;
  if(fabsf(a.getPosition().phi() - b.getPosition().phi()) > DELTA) return false;
  if(fabsf(a.getChi2Probability()- b.getChi2Probability())> DELTA) return false;
  
  if(a.getAuthor()       - b.getAuthor()       != 0) return false;
  if(a.getNTracks()      - b.getNTracks()      != 0) return false;
  if(a.getNMDT()         - b.getNMDT()         != 0) return false;
  if(a.getNRPC()         - b.getNRPC()         != 0) return false;
  if(a.getNTGC()         - b.getNTGC()         != 0) return false;
  
  return true;
}

