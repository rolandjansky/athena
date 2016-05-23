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

InDetPerfPlot_Vertex::InDetPerfPlot_Vertex(InDetPlotBase* pParent, const std::string & sDir):
	InDetPlotBase(pParent, sDir),
	m_vx_x(nullptr), m_vx_y(nullptr), m_vx_z(nullptr),
	m_vx_err_x(nullptr), m_vx_err_y(nullptr), m_vx_err_z(nullptr),
	m_vx_chi2_over_ndf(nullptr), m_vx_type(nullptr), m_vx_nTracks(nullptr),
	m_vx_sumpt2(nullptr),m_vx_track_weights(nullptr), m_vx_track_pt(nullptr),
	m_vx_track_eta(nullptr),m_vx_track_nSiHits(nullptr),m_vx_track_nSiHoles(nullptr),
	m_vx_track_d0(nullptr),m_vx_track_err_d0(nullptr),m_vx_track_z0(nullptr),
	m_vx_track_err_z0(nullptr){
  //nop
}


void 
InDetPerfPlot_Vertex::initializePlots() {

    IDPVM_BOOK(m_vx_x);
    IDPVM_BOOK(m_vx_y);
    IDPVM_BOOK(m_vx_z);

    IDPVM_BOOK(m_vx_err_x);
    IDPVM_BOOK(m_vx_err_y);
    IDPVM_BOOK(m_vx_err_z);

    IDPVM_BOOK(m_vx_chi2_over_ndf);
    IDPVM_BOOK(m_vx_type);

    IDPVM_BOOK(m_vx_nTracks);
    IDPVM_BOOK(m_vx_track_weights);

    // Expert plots (iDetailLevel >= 100)
    if (m_iDetailLevel >= 100) {
      IDPVM_BOOK(m_vx_track_pt);
      IDPVM_BOOK(m_vx_track_eta);
      IDPVM_BOOK(m_vx_track_nSiHits);
      IDPVM_BOOK(m_vx_track_nSiHoles);
      IDPVM_BOOK(m_vx_track_d0);
      IDPVM_BOOK(m_vx_track_err_d0);
      IDPVM_BOOK(m_vx_track_z0);
      IDPVM_BOOK(m_vx_track_err_z0);
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

