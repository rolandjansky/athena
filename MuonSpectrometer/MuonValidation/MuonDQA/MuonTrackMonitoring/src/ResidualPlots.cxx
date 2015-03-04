/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ResidualPlots.h"

namespace Muon{
  
ResidualPlots::ResidualPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir)
								 ,m_oResidualPlots_Global(this, "Global/", "")
								 ,m_oResidualPlots_MDT(this, "MDT/", "")
								 ,m_oResidualPlots_RPC_eta(this, "RPC/", "eta")
								 ,m_oResidualPlots_RPC_phi(this, "RPC/", "phi")
								 ,m_oResidualPlots_TGC_wire(this, "TGC/", "wire")
								 ,m_oResidualPlots_TGC_strip(this, "TGC/", "strip")
								 ,m_oResidualPlots_CSC_eta(this, "CSC/", "eta")
								 ,m_oResidualPlots_CSC_phi(this, "CSC/", "phi")
{}

void ResidualPlots::fill(const Trk::ResidualPull& resPull, int stationPhi, Muon::MuonStationIndex::TechnologyIndex techid, bool measuresPhi ) {

  m_oResidualPlots_Global.fill(resPull,stationPhi);

  //MDT 
  if (techid==Muon::MuonStationIndex::MDT) {
    m_oResidualPlots_MDT.fill(resPull,stationPhi);
  }
  //RPC
  else if (techid==Muon::MuonStationIndex::RPC){
    if (measuresPhi)
      m_oResidualPlots_RPC_phi.fill(resPull,stationPhi);
    else
      m_oResidualPlots_RPC_eta.fill(resPull,stationPhi);
  }
  //TGC
  else if (techid==Muon::MuonStationIndex::TGC){
    if (measuresPhi)
      m_oResidualPlots_TGC_strip.fill(resPull,stationPhi);
    else
      m_oResidualPlots_TGC_wire.fill(resPull,stationPhi);
  }
  //CSC
  else if (techid==Muon::MuonStationIndex::CSCI){ //it's CSCI, not CSC!!!
    if (measuresPhi)
      m_oResidualPlots_CSC_phi.fill(resPull,stationPhi);
    else
      m_oResidualPlots_CSC_eta.fill(resPull,stationPhi);
  }

}

}//closing namespace
