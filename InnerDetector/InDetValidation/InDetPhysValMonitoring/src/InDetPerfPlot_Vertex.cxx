/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "InDetPhysValMonitoringUtilities.h"
#include "xAODEventInfo/EventInfo.h"


using namespace IDPVM;


InDetPerfPlot_Vertex::InDetPerfPlot_Vertex(InDetPlotBase* pParent, const std::string& sDir) :
  InDetPlotBase(pParent, sDir),
  m_vx_x(nullptr), m_vx_y(nullptr), m_vx_z(nullptr),
  m_vx_err_x(nullptr), m_vx_err_y(nullptr), m_vx_err_z(nullptr),
  m_vx_chi2_over_ndf(nullptr), m_vx_type(nullptr), 
  m_vx_nTracks(nullptr),
  m_vx_track_weights(nullptr), m_vx_track_pt(nullptr),
  m_vx_track_eta(nullptr), m_vx_track_nSiHits(nullptr), m_vx_track_nSiHoles(nullptr),
  m_vx_track_d0(nullptr), m_vx_track_err_d0(nullptr), m_vx_track_z0(nullptr),
  m_vx_track_err_z0(nullptr) {
  // nop
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
InDetPerfPlot_Vertex::fill(const xAOD::Vertex& vertex) {
  // fill position plots
  fillHisto(m_vx_x, vertex.x());
  fillHisto(m_vx_y, vertex.y());
  fillHisto(m_vx_z, vertex.z());

  // fill error plots
  const AmgSymMatrix(3)& covariance = vertex.covariancePosition();
  fillHisto(m_vx_err_x, Amg::error(covariance, 0));
  fillHisto(m_vx_err_y, Amg::error(covariance, 1));
  fillHisto(m_vx_err_z, Amg::error(covariance, 2));

  // fill vertex quality and type
  fillHisto(m_vx_type, vertex.vertexType());

  float ndf = vertex.numberDoF();
  if (ndf != 0) {
    fillHisto(m_vx_chi2_over_ndf, vertex.chiSquared() / ndf);
  } else {
    fillHisto(m_vx_chi2_over_ndf, -1);
  }

  // fill vertex tracks properties
  int nTracks = vertex.nTrackParticles();
  fillHisto(m_vx_nTracks, nTracks);
  for (const float& weight : vertex.trackWeights()) {
    fillHisto(m_vx_track_weights, weight);
  }

  // fill expert plots: tracks properties at vertex
  if (m_iDetailLevel >= 100) {
    // loop over tracks at vertex
    for (const auto& elTrk : vertex.trackParticleLinks()) {
      const xAOD::TrackParticle* trk = *elTrk;
      fillHisto(m_vx_track_pt, trk->pt() / Gaudi::Units::GeV); // MeV -> GeV
      fillHisto(m_vx_track_eta, trk->eta());
      const xAOD::ParametersCovMatrix_t covTrk = trk->definingParametersCovMatrix();
      fillHisto(m_vx_track_d0, trk->d0());
      fillHisto(m_vx_track_err_d0, Amg::error(covTrk, 0));
      fillHisto(m_vx_track_z0, trk->z0() - vertex.z());
      fillHisto(m_vx_track_err_z0, Amg::error(covTrk, 1));
      bool successfulRetrieval(false);
      uint8_t iPixHits, iSctHits, iPixHoles, iSctHoles;
      successfulRetrieval = trk->summaryValue(iPixHits, xAOD::numberOfPixelHits);
      successfulRetrieval &= trk->summaryValue(iSctHits, xAOD::numberOfSCTHits);
      if (successfulRetrieval) {
        fillHisto(m_vx_track_nSiHits, iPixHits + iSctHits);
      }
      successfulRetrieval = trk->summaryValue(iPixHoles, xAOD::numberOfPixelHoles);
      successfulRetrieval &= trk->summaryValue(iSctHoles, xAOD::numberOfSCTHoles);
      if (successfulRetrieval) {
        fillHisto(m_vx_track_nSiHoles, iPixHoles + iSctHoles);
      }
    }
  }
}
