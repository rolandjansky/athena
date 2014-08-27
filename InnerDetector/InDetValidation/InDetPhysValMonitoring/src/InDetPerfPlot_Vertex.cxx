/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_Vertex.cxx
 * @author Simone Pagan Griso
**/

#include "InDetPerfPlot_Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

InDetPerfPlot_Vertex::InDetPerfPlot_Vertex(PlotBase* pParent, const std::string & sDir):PlotBase(pParent, sDir){
  //nop
}


void 
InDetPerfPlot_Vertex::initializePlots() {

    m_vx_x = Book1D("vx_x", "X position of vertex;X (mm);Entries",200, -1.0, 1.0,false);
    m_vx_y = Book1D("vx_y", "Y position of vertex;Y (mm);Entries",200, -1.0, 1.0,false);
    m_vx_z = Book1D("vx_z", "Z position of vertex;Z (mm);Entries",100, -300.0, 300.0,false);

    m_vx_err_x = Book1D("vx_err_x", "X position error of vertex;#sigma(X) (mm);Entries", 100, 0.0, 0.2,false);
    m_vx_err_y = Book1D("vx_err_y", "Y position error of vertex;#sigma(Y) (mm);Entries", 100, 0.0, 0.2,false);
    m_vx_err_z = Book1D("vx_err_z", "Z position error of vertex;#sigma(Z) (mm);Entries", 100, 0.0, 1.0,false);

    m_vx_chi2_over_ndf = Book1D("vx_chi2_over_ndf", "vertex #chi^2 / ndf;#chi^{2}/ndf;Entries", 50, 0, 10.,false);
    m_vx_type = Book1D("vx_type", "Vertex type;Vertex type;Entries", 7, 0, 7,false);

    m_vx_nTracks = Book1D("vx_nTracks", "Number of tracks at vertex;Number of Tracks;Entries", 150, 0, 150,false);
    m_vx_track_weights = Book1D("vx_track_weights", "Weights of tracks at vertex;Weight;Entries", 100, 0., 10.0,false);

    // Expert plots (iDetailLevel >= 100)
    if (m_iDetailLevel >= 100) {
      m_vx_track_pt = Book1D("vx_track_pt", "Tracks at vertex p_{T};p_{T} (GeV);Entries", 100, 0, 20.,false);
      m_vx_track_eta = Book1D("vx_track_eta", "Tracks at vertex #eta;#eta;Entries", 100, -2.7, 2.7,false);
      m_vx_track_nSiHits = Book1D("vx_track_nSiHits", "Tracks at vertex number of Silicon Hits;N Si Hits;Entries", 15, 5, 20,false);
      m_vx_track_nSiHoles = Book1D("vx_track_nSiHoles", "Tracks at vertex number of Silicon Holes;N Si Holes;Entries", 5, 0, 5,false);
      m_vx_track_d0 = Book1D("vx_track_d0", "Tracks at vertex d_{0};d_{0} (mm);Entries", 100, -2.0, 2.0,false);
      m_vx_track_err_d0 = Book1D("vx_track_err_d0", "Tracks at vertex d_{0} error;#sigma(d_{0}) (mm);Entries", 50, 0.0, 1.0,false);
      m_vx_track_z0 = Book1D("vx_track_z0", "Tracks at vertex z_{0} - z_{0}^{vertex};z_{0}-z_{0}^{vertex} (mm);Entries", 100, -5.0, 5.0,false);
      m_vx_track_err_z0 = Book1D("vx_track_err_z0", "Tracks at vertex z_{0} error;#sigma(z_{0}) (mm);Entries", 50, 0.0, 5.0,false);
    }
}

void 
InDetPerfPlot_Vertex::fill(const xAOD::Vertex& vertex){
  //fill position plots
  m_vx_x->Fill(vertex.x());
  m_vx_y->Fill(vertex.y());
  m_vx_z->Fill(vertex.z());
  
  //fill error plots
  const AmgSymMatrix(3)& covariance = vertex.covariancePosition();
  m_vx_err_x->Fill(Amg::error(covariance,0));
  m_vx_err_y->Fill(Amg::error(covariance,1));
  m_vx_err_z->Fill(Amg::error(covariance,2));

  //fill vertex quality and type
  m_vx_type->Fill(vertex.vertexType());
  float ndf = vertex.numberDoF();
  if (ndf != 0)
    m_vx_chi2_over_ndf->Fill(vertex.chiSquared() / ndf);
  else
    m_vx_chi2_over_ndf->Fill(-1);

  //fill vertex tracks properties
  int nTracks = vertex.nTrackParticles();
  m_vx_nTracks->Fill(nTracks);
  for (const float& weight : vertex.trackWeights()) {
    m_vx_track_weights->Fill(weight);
  }

  //fill expert plots: tracks properties at vertex
  if (m_iDetailLevel >= 100) {
    //loop over tracks at vertex
    for (const auto& elTrk : vertex.trackParticleLinks()) {
      const xAOD::TrackParticle* trk = *elTrk;
      m_vx_track_pt->Fill(trk->pt() / 1000.); // MeV -> GeV
      m_vx_track_eta->Fill(trk->eta());
      const xAOD::ParametersCovMatrix_t covTrk = trk->definingParametersCovMatrix();
      m_vx_track_d0->Fill(trk->d0());
      m_vx_track_err_d0->Fill(Amg::error(covTrk, 0));
      m_vx_track_z0->Fill(trk->z0() - vertex.z());
      m_vx_track_err_z0->Fill(Amg::error(covTrk, 1));
      bool successfulRetrieval(false);
      uint8_t iPixHits, iSctHits, iPixHoles, iSctHoles;
      successfulRetrieval = trk->summaryValue(iPixHits, xAOD::numberOfPixelHits);
      successfulRetrieval &= trk->summaryValue(iSctHits, xAOD::numberOfSCTHits);
      if (successfulRetrieval) m_vx_track_nSiHits->Fill(iPixHits+iSctHits);
      successfulRetrieval = trk->summaryValue(iPixHoles, xAOD::numberOfPixelHoles);
      successfulRetrieval &= trk->summaryValue(iSctHoles, xAOD::numberOfSCTHoles);
      if (successfulRetrieval) m_vx_track_nSiHoles->Fill(iPixHoles+iSctHoles); 
    }
  }
}

