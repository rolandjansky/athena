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
  book(m_vx_x,"vx_x");
  book(m_vx_y,"vx_y");
  book(m_vx_z,"vx_z");

  book(m_vx_err_x,"vx_err_x");
  book(m_vx_err_y,"vx_err_y");
  book(m_vx_err_z,"vx_err_z");

  book(m_vx_chi2_over_ndf,"vx_chi2_over_ndf");
  book(m_vx_type,"vx_type");
  
  book(m_vx_nTracks,"vx_nTracks");
  book(m_vx_track_weights,"vx_track_weights");



  if (m_iDetailLevel >= 100) {
    book(m_vx_track_pt,"vx_track_pt");
    book(m_vx_track_eta,"vx_track_eta");
    book(m_vx_track_nSiHits,"vx_track_nSiHits");
    book(m_vx_track_nSiHoles,"vx_track_nSiHoles");
    book(m_vx_track_d0,"vx_track_d0");
    book(m_vx_track_err_d0,"vx_track_err_d0");
    book(m_vx_track_z0,"vx_track_z0");
    book(m_vx_track_err_z0,"vx_track_err_z0");
  }
}

void
InDetPerfPlot_Vertex::fill(const xAOD::Vertex& vertex, float weight) {
  // fill position plots
  fillHisto(m_vx_x, vertex.x(), weight);
  fillHisto(m_vx_y, vertex.y(), weight);
  fillHisto(m_vx_z, vertex.z(), weight);

  // fill error plots
  const AmgSymMatrix(3)& covariance = vertex.covariancePosition();
  fillHisto(m_vx_err_x, Amg::error(covariance, 0), weight);
  fillHisto(m_vx_err_y, Amg::error(covariance, 1), weight);
  fillHisto(m_vx_err_z, Amg::error(covariance, 2), weight);

  // fill vertex quality and type
  fillHisto(m_vx_type, vertex.vertexType(), weight);

  float ndf = vertex.numberDoF();
  if (ndf != 0) {
    fillHisto(m_vx_chi2_over_ndf, vertex.chiSquared() / ndf, weight);
  } else {
    fillHisto(m_vx_chi2_over_ndf, -1, weight);
  }

  // fill vertex tracks properties
  int nTracks = vertex.nTrackParticles();
  fillHisto(m_vx_nTracks, nTracks, weight);
  for (const float& trackWeight : vertex.trackWeights()) {
    fillHisto(m_vx_track_weights, trackWeight, weight);
  }

  // fill expert plots: tracks properties at vertex
  if (m_iDetailLevel >= 100) {
    // loop over tracks at vertex
    for (const auto& elTrk : vertex.trackParticleLinks()) {
      /// This should not be run on formats with a slimmed track record.
      /// However, people may do so - so we catch the potential invalid links
      /// to prevent a (useless) athena exception message. 
      if (!elTrk.isValid()){
        /// currently, the PlotBase classes have no athena message stream.
        /// TODO: This should be improved in the future (requires changes to all validation packages)
        std::cerr << "Invalid track link on vertex. Vertex track plots will be unreliable. Please check your input format. "<<std::endl; 
        continue; 
      }
      const xAOD::TrackParticle* trk = *elTrk;
      fillHisto(m_vx_track_pt, trk->pt() / Gaudi::Units::GeV, weight); // MeV -> GeV
      fillHisto(m_vx_track_eta, trk->eta(), weight);
      const xAOD::ParametersCovMatrix_t covTrk = trk->definingParametersCovMatrix();
      fillHisto(m_vx_track_d0, trk->d0(), weight);
      fillHisto(m_vx_track_err_d0, Amg::error(covTrk, 0), weight);
      fillHisto(m_vx_track_z0, trk->z0() - vertex.z(), weight);
      fillHisto(m_vx_track_err_z0, Amg::error(covTrk, 1), weight);
      bool successfulRetrieval(false);
      uint8_t iPixHits, iSctHits, iPixHoles, iSctHoles;
      successfulRetrieval = trk->summaryValue(iPixHits, xAOD::numberOfPixelHits);
      successfulRetrieval &= trk->summaryValue(iSctHits, xAOD::numberOfSCTHits);
      if (successfulRetrieval) {
        fillHisto(m_vx_track_nSiHits, iPixHits + iSctHits, weight);
      }
      successfulRetrieval = trk->summaryValue(iPixHoles, xAOD::numberOfPixelHoles);
      successfulRetrieval &= trk->summaryValue(iSctHoles, xAOD::numberOfSCTHoles);
      if (successfulRetrieval) {
        fillHisto(m_vx_track_nSiHoles, iPixHoles + iSctHoles, weight);
      }
    }
  }
}
