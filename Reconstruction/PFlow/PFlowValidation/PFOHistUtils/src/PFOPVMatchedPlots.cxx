/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOHistUtils/PFOPVMatchedPlots.h"

namespace PFO {

  PFOPVMatchedPlots::PFOPVMatchedPlots(PlotBase* pParent, std::string sDir, std::string sPFOContainerName) : PlotBase(pParent, sDir), m_sPFOContainerName(sPFOContainerName){
    m_PFO_pt = nullptr;
    m_PFO_eta = nullptr;
    m_PFO_phi = nullptr;
    m_PFO_m = nullptr;
    m_PFO_charge = nullptr;
  }

  void PFOPVMatchedPlots::initializePlots(){

    m_PFO_pt = Book1D("PFO_PVMatched_Pt",m_sPFOContainerName + "_Pt (Entries/1 GeV)",30,-10.0,20.0);
    m_PFO_eta = Book1D("PFO_PVMatched_Eta",m_sPFOContainerName + "_Eta (Entries/0.1)",100,-5.0,5.0);
    m_PFO_phi = Book1D("PFO_PVMatched_Phi",m_sPFOContainerName + "_Phi (Entries/0.1)",64,-3.2,3.2);
    m_PFO_m = Book1D("PFO_PVMatched_m",m_sPFOContainerName + "_m (Entries/100 MeV)",10,0.0,0.5);
    m_PFO_charge = Book1D("PFO_PVMatched_charge",m_sPFOContainerName + "_charge (Entries/unit charge)",5,-2,2);
    
    m_PFO_pt_etaBinA = Book1D("PFO_Pt_PVMatched_A",m_sPFOContainerName + "_Pt (Entries/1 GeV, |eta| < 1 )",30,-10.0,20.0);
    m_PFO_pt_etaBinB = Book1D("PFO_Pt_PVMatched_B",m_sPFOContainerName + "_Pt (Entries/1 GeV,  1 <= |eta| < 2 )",30,-10.0,20.0);
    m_PFO_pt_etaBinC = Book1D("PFO_Pt_PVMatched_C",m_sPFOContainerName + "_Pt (Entries/1 GeV, |eta| >= 2 )",30,-10.0,20.0);
  }

  void PFOPVMatchedPlots::fill(const xAOD::PFO& PFO,  const xAOD::Vertex& theVertex){

    const xAOD::TrackParticle* theTrack = PFO.track(0);

    if (theTrack){    
      //vtz.z() provides z of that vertex w.r.t the center of the beamspot (z = 0). Thus we corrext the track z0 to be w.r.t z = 0
      float z0 = theTrack->z0() + theTrack->vz();
      z0 = z0 - theVertex.z();
      float theta = theTrack->theta();
      if ( fabs(z0*sin(theta)) < 2.0 ){
	m_PFO_pt->Fill(PFO.pt()/1000.0);
	m_PFO_eta->Fill(PFO.eta());
	m_PFO_phi->Fill(PFO.phi());
	m_PFO_m->Fill(PFO.m()/1000.0);
	m_PFO_charge->Fill(PFO.charge());

	if (fabs(PFO.eta()) < 1)  m_PFO_pt_etaBinA->Fill(PFO.pt()/1000.0);
	else if (fabs(PFO.eta()) < 2) m_PFO_pt_etaBinB->Fill(PFO.pt()/1000.0);
	else m_PFO_pt_etaBinC->Fill(PFO.pt()/1000.0);
      }//fill histograms if track is matched to the PV
    }//if valid pointer to track
  } 

}
