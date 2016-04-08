/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigL2Bjet.cxx
// PACKAGE:  Trigger/TrigEvent/TrigParticle
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#include "TrigParticle/TrigL2Bjet.h"


TrigL2Bjet::TrigL2Bjet() :
  P4PtEtaPhiM(0,0,0,1.*CLHEP::MeV), 
  NavigableTerminalNode(),
  m_valid(false),
  m_roiID(-1),
  m_track(0), 
  m_prmvertex(0),
  m_secvertex(0),
  m_prmVtx(-1.),
  m_xcomb(-1.),
  m_xIP1d(-1.),
  m_xIP2d(-1.),
  m_xIP3d(-1.),
  m_xChi2(-1.),
  m_xSv(-1.),
  m_xmvtx(-1.),
  m_xevtx(-1.),
  m_xnvtx(-1.)
{}


TrigL2Bjet::TrigL2Bjet(int roi, float eta, float phi, const TrigInDetTrackCollection* track, 
		       const TrigVertexCollection* prmvertex, const TrigVertexCollection* secvertex, float prmvtx,
		       float ptJet, float xcomb, float xIP1d, float xIP2d, float xIP3d, float xChi2,
		       float xSv, float xmvtx, float xevtx, float xnvtx) :
  P4PtEtaPhiM((double)ptJet,(double)eta,(double)phi, 1.*CLHEP::MeV), 
  NavigableTerminalNode(),
  m_valid(false),
  m_roiID(roi),
  m_track(track), 
  m_prmvertex(prmvertex), 
  m_secvertex(secvertex),
  m_prmVtx(prmvtx),
  m_xcomb(xcomb),
  m_xIP1d(xIP1d),
  m_xIP2d(xIP2d),
  m_xIP3d(xIP3d),
  m_xChi2(xChi2),
  m_xSv(xSv),
  m_xmvtx(xmvtx),
  m_xevtx(xevtx),
  m_xnvtx(xnvtx)
{}


TrigL2Bjet::~TrigL2Bjet() {}

void  TrigL2Bjet::validate(bool v) { m_valid = v; }

bool  TrigL2Bjet::isValid()  const { return m_valid; }

int   TrigL2Bjet::roiId() const { return m_roiID; }

float TrigL2Bjet::prmVtx() const { return m_prmVtx; }

const TrigInDetTrackCollection* TrigL2Bjet::TrackCollection()     const { return m_track; }
const TrigVertexCollection*     TrigL2Bjet::PrmVertexCollection() const { return m_prmvertex; }
const TrigVertexCollection*     TrigL2Bjet::SecVertexCollection() const { return m_secvertex; }

float  TrigL2Bjet::xComb() const { return m_xcomb; }
float  TrigL2Bjet::xIP1D() const { return m_xIP1d; }
float  TrigL2Bjet::xIP2D() const { return m_xIP2d; }
float  TrigL2Bjet::xIP3D() const { return m_xIP3d; }
float  TrigL2Bjet::xCHI2() const { return m_xChi2; }
float  TrigL2Bjet::xSV()   const { return m_xSv; }
float  TrigL2Bjet::xMVtx() const { return m_xmvtx; }
float  TrigL2Bjet::xEVtx() const { return m_xevtx; }
float  TrigL2Bjet::xNVtx() const { return m_xnvtx; }


//** ----------------------------------------------------------------------------------------------------------------- **//


//helper operators

std::string str (const TrigL2Bjet& a) {

  std::stringstream ss;

  ss << "RoI index = "   << a.roiId()
     << "; prmVtx = " << a.prmVtx()
     << "; xComb = "  << a.xComb()
     << "; xIP1D = "  << a.xIP1D()
     << "; xIP2D = "  << a.xIP2D()
     << "; xIP3D = "  << a.xIP3D()
     << "; xCHI2 = "  << a.xCHI2()
     << "; xSV = "    << a.xSV()
     << "; xMVtx = "  << a.xMVtx()
     << "; xEVtx = "  << a.xEVtx()
     << "; xNVtx = "  << a.xNVtx();

  return ss.str();
}


MsgStream& operator<< (MsgStream& m, const TrigL2Bjet& a) {

  return (m << str(a));
}


bool operator== (const TrigL2Bjet& a, const TrigL2Bjet& b) {

  //* distance used to compare floats *//
  const double DELTA=1e-3; 

  if(std::abs(a.eta() - b.eta()) > DELTA) return false;
  if(std::abs(a.phi() - b.phi()) > DELTA) return false;
  
  if(std::abs(a.prmVtx() - b.prmVtx()) > DELTA) return false;
  
  if(std::abs(a.xComb() - b.xComb()) > DELTA) return false;
  if(std::abs(a.xIP1D() - b.xIP1D()) > DELTA) return false;
  if(std::abs(a.xIP2D() - b.xIP2D()) > DELTA) return false;
  if(std::abs(a.xIP3D() - b.xIP3D()) > DELTA) return false;
  if(std::abs(a.xCHI2() - b.xCHI2()) > DELTA) return false;
  if(std::abs(a.xSV()   - b.xSV())   > DELTA) return false;
  if(std::abs(a.xMVtx() - b.xMVtx()) > DELTA) return false;
  if(std::abs(a.xEVtx() - b.xEVtx()) > DELTA) return false;
  if(std::abs(a.xNVtx() - b.xNVtx()) > DELTA) return false;

  return true;
}


void diff(const TrigL2Bjet& a, const TrigL2Bjet& b, std::map<std::string, double>& variableChange) {  

  if(a.prmVtx() != b.prmVtx()) variableChange[ "prmVtx" ] = static_cast< double >( a.prmVtx() - b.prmVtx() );

  if(a.xComb() != b.xComb()) variableChange["xComb"] = static_cast<double>(a.xComb() - b.xComb());
  if(a.xIP1D() != b.xIP1D()) variableChange["xIP1D"] = static_cast<double>(a.xIP1D() - b.xIP1D());
  if(a.xIP2D() != b.xIP2D()) variableChange["xIP2D"] = static_cast<double>(a.xIP2D() - b.xIP2D());
  if(a.xIP3D() != b.xIP3D()) variableChange["xIP3D"] = static_cast<double>(a.xIP3D() - b.xIP3D());
  if(a.xCHI2() != b.xCHI2()) variableChange["xCHI2"] = static_cast<double>(a.xCHI2() - b.xCHI2());
  if(a.xSV()   != b.xSV())   variableChange["xSV"]   = static_cast<double>(a.xSV()   - b.xSV());
  if(a.xMVtx() != b.xMVtx()) variableChange["xMVtx"] = static_cast<double>(a.xMVtx() - b.xMVtx());
  if(a.xEVtx() != b.xEVtx()) variableChange["xEVtx"] = static_cast<double>(a.xEVtx() - b.xEVtx());
  if(a.xNVtx() != b.xNVtx()) variableChange["xNVtx"] = static_cast<double>(a.xNVtx() - b.xNVtx());

  return;
}
