/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOHistUtils/PFOPVMatchedPlots.h"

namespace PFO {

  PFOPVMatchedPlots::PFOPVMatchedPlots(PlotBase* pParent, std::string sDir, std::string sPFOContainerName, std::string sFEContainerName) : PlotBase(pParent, sDir), m_sPFOContainerName(sPFOContainerName), m_sFEContainerName(sFEContainerName){
    m_PFO_pt = nullptr;
    m_PFO_eta = nullptr;
    m_PFO_phi = nullptr;
    m_PFO_m = nullptr;
    m_PFO_charge = nullptr;

    m_FE_pt = nullptr;
    m_FE_eta = nullptr;
    m_FE_phi = nullptr;
    m_FE_m = nullptr;
    m_FE_charge = nullptr;
  }

  void PFOPVMatchedPlots::initializePlots(){
    // book PFO histograms
    if(!m_sPFOContainerName.empty()){
      m_PFO_pt = Book1D("_PVMatched_Pt",m_sPFOContainerName + "_Pt (Entries/1 GeV)",30,-10.0,20.0);
      m_PFO_eta = Book1D("_PVMatched_Eta",m_sPFOContainerName + "_Eta (Entries/0.1)",100,-5.0,5.0);
      m_PFO_phi = Book1D("_PVMatched_Phi",m_sPFOContainerName + "_Phi (Entries/0.1)",64,-3.2,3.2);
      m_PFO_m = Book1D("_PVMatched_m",m_sPFOContainerName + "_m (Entries/100 MeV)",10,0.0,0.5);
      m_PFO_charge = Book1D("_PVMatched_charge",m_sPFOContainerName + "_charge (Entries/unit charge)",5,-2,2);
    
      m_PFO_pt_etaBinA = Book1D("_Pt_PVMatched_A",m_sPFOContainerName + "_Pt (Entries/1 GeV, |eta| < 1 )",30,-10.0,20.0);
      m_PFO_pt_etaBinB = Book1D("_Pt_PVMatched_B",m_sPFOContainerName + "_Pt (Entries/1 GeV,  1 <= |eta| < 2 )",30,-10.0,20.0);
      m_PFO_pt_etaBinC = Book1D("_Pt_PVMatched_C",m_sPFOContainerName + "_Pt (Entries/1 GeV, |eta| >= 2 )",30,-10.0,20.0);
    }
    //book Flow Element histograms
    if(!m_sFEContainerName.empty()){
      m_FE_pt = Book1D("_PVMatched_Pt",m_sFEContainerName + "_Pt (Entries/1 GeV)",30,-10.0,20.0);
      m_FE_eta = Book1D("_PVMatched_Eta",m_sFEContainerName + "_Eta (Entries/0.1)",100,-5.0,5.0);
      m_FE_phi = Book1D("_PVMatched_Phi",m_sFEContainerName + "_Phi (Entries/0.1)",64,-3.2,3.2);
      m_FE_m = Book1D("_PVMatched_m",m_sFEContainerName + "_m (Entries/100 MeV)",10,0.0,0.5);
      m_FE_charge = Book1D("_PVMatched_charge",m_sFEContainerName + "_charge (Entries/unit charge)",5,-2,2);
    
      m_FE_pt_etaBinA = Book1D("_Pt_PVMatched_A",m_sFEContainerName + "_Pt (Entries/1 GeV, |eta| < 1 )",30,-10.0,20.0);
      m_FE_pt_etaBinB = Book1D("_Pt_PVMatched_B",m_sFEContainerName + "_Pt (Entries/1 GeV,  1 <= |eta| < 2 )",30,-10.0,20.0);
      m_FE_pt_etaBinC = Book1D("_Pt_PVMatched_C",m_sFEContainerName + "_Pt (Entries/1 GeV, |eta| >= 2 )",30,-10.0,20.0);      
    }
  }

  void PFOPVMatchedPlots::fill(const xAOD::PFO& PFO,  const xAOD::Vertex& theVertex, const xAOD::EventInfo& eventInfo){

    const xAOD::TrackParticle* theTrack = PFO.track(0);

    if (theTrack){    
      //vtz.z() provides z of that vertex w.r.t the center of the beamspot (z = 0). Thus we corrext the track z0 to be w.r.t z = 0
      float z0 = theTrack->z0() + theTrack->vz();
      z0 = z0 - theVertex.z();
      float theta = theTrack->theta();
      if ( fabs(z0*sin(theta)) < 2.0 ){
    	m_PFO_pt->Fill(PFO.pt()/1000.0,eventInfo.beamSpotWeight());
	    m_PFO_eta->Fill(PFO.eta(),eventInfo.beamSpotWeight());
	    m_PFO_phi->Fill(PFO.phi(),eventInfo.beamSpotWeight());
	    m_PFO_m->Fill(PFO.m()/1000.0,eventInfo.beamSpotWeight());
	    m_PFO_charge->Fill(PFO.charge(),eventInfo.beamSpotWeight());

	    if (fabs(PFO.eta()) < 1)  m_PFO_pt_etaBinA->Fill(PFO.pt()/1000.0,eventInfo.beamSpotWeight());
	    else if (fabs(PFO.eta()) < 2) m_PFO_pt_etaBinB->Fill(PFO.pt()/1000.0,eventInfo.beamSpotWeight());
	    else m_PFO_pt_etaBinC->Fill(PFO.pt()/1000.0,eventInfo.beamSpotWeight());
      }//fill histograms if track is matched to the PV
    }//if valid pointer to track
  } 



  void PFOPVMatchedPlots::fill(const xAOD::FlowElement& FE,  const xAOD::Vertex& theVertex, const xAOD::EventInfo& eventInfo){
    if(FE.nChargedObjects()==0)
      return;
    const xAOD::TrackParticle* theTrack =static_cast<const xAOD::TrackParticle*>(FE.chargedObjects().at(0));

    if (theTrack){    
      //vtz.z() provides z of that vertex w.r.t the center of the beamspot (z = 0). Thus we corrext the track z0 to be w.r.t z = 0
      float z0 = theTrack->z0() + theTrack->vz();
      z0 = z0 - theVertex.z();
      float theta = theTrack->theta();
      if ( fabs(z0*sin(theta)) < 2.0 ){
	    m_FE_pt->Fill(FE.pt()/1000.0,eventInfo.beamSpotWeight());
	    m_FE_eta->Fill(FE.eta(),eventInfo.beamSpotWeight());
	    m_FE_phi->Fill(FE.phi(),eventInfo.beamSpotWeight());
	    m_FE_m->Fill(FE.m()/1000.0,eventInfo.beamSpotWeight());
	    m_FE_charge->Fill(FE.charge(),eventInfo.beamSpotWeight());

    	if (fabs(FE.eta()) < 1)  m_FE_pt_etaBinA->Fill(FE.pt()/1000.0,eventInfo.beamSpotWeight());
	    else if (fabs(FE.eta()) < 2) m_FE_pt_etaBinB->Fill(FE.pt()/1000.0,eventInfo.beamSpotWeight());
	    else m_FE_pt_etaBinC->Fill(FE.pt()/1000.0,eventInfo.beamSpotWeight());
      }//fill histograms if track is matched to the PV
    }//if valid pointer to track
  } 


}
