/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonIsolationPlots.h"

namespace Muon{

MuonIsolationPlots::MuonIsolationPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir), 
m_oPtCone20(this,"", "ptcone20"),
m_oPtCone30(this,"", "ptcone30"),
m_oPtCone40(this,"", "ptcone40"),
m_oEtCone20(this,"", "etcone20"),
m_oEtCone30(this,"", "etcone30"),
m_oEtCone40(this,"", "etcone40")
{}	

void MuonIsolationPlots::fill(const xAOD::Muon& muon){
  m_oPtCone20.fill(muon, xAOD::Iso::ptcone20);
  m_oPtCone30.fill(muon, xAOD::Iso::ptcone30);
  m_oPtCone40.fill(muon, xAOD::Iso::ptcone40);
  m_oEtCone20.fill(muon, xAOD::Iso::etcone20);
  m_oEtCone30.fill(muon, xAOD::Iso::etcone30);
  m_oEtCone40.fill(muon, xAOD::Iso::etcone40);
}

IsoPlots::IsoPlots(PlotBase* pParent, std::string sDir, std::string sConeSize):PlotBase(pParent, sDir),
m_sConeSize(sConeSize), cone(NULL), conerel(NULL)
{}

void IsoPlots::initializePlots(){
   cone    = Book1D(m_sConeSize, m_sConeSize + ";" + m_sConeSize + ";Entries", 60, -5., 25.);
   conerel = Book1D(m_sConeSize + "rel", m_sConeSize + "rel;" + m_sConeSize + "rel;Entries", 40, 0., 2.);
}

void IsoPlots::fill(const xAOD::Muon& muon, const xAOD::Iso::IsolationType &isoType){
  float fIso = 0;
  if (muon.isolation(fIso, isoType)) fill(fIso, muon.pt());
}

void IsoPlots::fill(float fIso, float fPt){
  cone->Fill(fIso*0.001);
  conerel->Fill(fIso/fPt);
}
}
