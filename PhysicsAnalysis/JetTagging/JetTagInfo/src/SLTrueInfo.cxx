/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagInfo/SLTrueInfo.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

namespace Analysis {

SLTrueInfo::SLTrueInfo()
  : m_barcode(0),
    m_pdgCode(0),
    m_pdgCodeMother(0),
    m_isFromBhadron(0),
    m_isFromDhadron(0),
    m_isFromGHboson(0)
{
  m_Momentum.setZero();
  m_ProductionVertex.setZero();
}

SLTrueInfo::SLTrueInfo(int barc, int pdg, int pdgm, bool b, bool d, bool w, const Eigen::Vector3d& p, const Eigen::Vector3d& r) {
  m_barcode          = barc;
  m_pdgCode          = pdg;
  m_pdgCodeMother    = pdgm;
  m_isFromBhadron    = b;
  m_isFromDhadron    = d;
  m_isFromGHboson    = w;
  m_Momentum         = p;
  m_ProductionVertex = r;  
}
	
MsgStream& operator<<( MsgStream& out, const SLTrueInfo& info) {
  out << " -> SLTrueInfo " 
      << " barcode = " << info.barcode()
      << " pdg = " << info.pdgId() 
      << " pt = " << info.momentum().perp()
      << " pdgMother = " << info.pdgIdMother()
      << " From B = " << info.FromB() << " From D = " << info.FromD() << " From Gauge/Higgs = " << info.FromGH()
      << endmsg; 
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
