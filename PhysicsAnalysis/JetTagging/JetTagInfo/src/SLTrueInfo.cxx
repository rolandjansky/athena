/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagInfo/SLTrueInfo.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

namespace Analysis {

SLTrueInfo::SLTrueInfo() {
}

SLTrueInfo::SLTrueInfo(int barc, int pdg, int pdgm, bool b, bool d, bool w, Eigen::Vector3d& p, Eigen::Vector3d& r) {
  m_barcode          = barc;
  m_pdgCode          = pdg;
  m_pdgCodeMother    = pdgm;
  m_isFromBhadron    = b;
  m_isFromDhadron    = d;
  m_isFromGHboson    = w;
  m_Momentum         = p;
  m_ProductionVertex = r;  
}
	
SLTrueInfo::~SLTrueInfo() {
}

MsgStream& operator<<( MsgStream& out, const SLTrueInfo& info) {
  out << " -> SLTrueInfo " 
      << " barcode = " << info.barcode()
      << " pdg = " << info.pdgId() 
      << " pt = " << info.momentum().perp()
      << " pdgMother = " << info.pdgIdMother()
      << " From B = " << info.FromB() << " From D = " << info.FromD() << " From Gauge/Higgs = " << info.FromGH()
      << endreq; 
  return out;
}
  
std::ostream& operator<<( std::ostream& out, const SLTrueInfo& info) {
  out << " -> SLTrueInfo " 
      << " barcode = " << info.barcode()
      << " pdg = " << info.pdgId() 
      << " pt = " << info.momentum().perp()
      << " pdgMother = " << info.pdgIdMother()
      << " From B = " << info.FromB() << " From D = " << info.FromD() << " From Gauge/Higgs = " << info.FromGH()
      << std::endl; 
  return out;
}

} 
