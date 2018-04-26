/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_SPECTRUM
#define INDETPHYSVALMONITORING_INDETPERFPLOT_SPECTRUM
/**
 * @file InDetPerfPlot_spectrum.h
 * @author Stewart Swift
 **/


// std includes
#include <string>

// local includes
#include "InDetPlotBase.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"

class InDetPerfPlot_spectrum: public InDetPlotBase {
public:
  enum Category {
    ALL, IN_JETS, N_FAKETYPES
  };
  InDetPerfPlot_spectrum(InDetPlotBase* pParent, const std::string& dirName);
  void fillSpectrum(const xAOD::TrackParticle& trkprt, Float_t prob);
  void fillSpectrum(const xAOD::TruthParticle& particle);
  void fillSpectrum(const xAOD::TrackParticle& trkprt, const xAOD::TruthVertex& truthVrt);
  void fillSpectrum(const xAOD::TrackParticle& trkprt, const xAOD::Vertex& vertex, bool fill);
  void fillSpectrumUnlinked(const xAOD::TrackParticle& trkprt, Float_t prob);
  void fillSpectrumLinked(const xAOD::TrackParticle& trkprt, Float_t prob);
  void fillSpectrumLinked(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& particle, double prob);
  void fillSpectrumUnlinked2(const xAOD::TrackParticle& trkprt, double prob);
private:
  TH1* m_nSCTHits;
  TH1* m_nPixHits;
  TH1* m_nTotHits;
  TH1* m_nSCTDeadSensors;
  TH1* m_nPixDeadSensors;
  TH1* m_nTotDeadSensors;

  TH1* m_recoMatchProb;
  TH1* m_recoPt;
  TH1* m_recoEta;
  TH1* m_recoPhi;
  TH1* m_recod0;
  TH1* m_recoz0;
  TH1* m_recoz0sin;
  //
  TH1* m_recod0_TruthVtxR;
  TH1* m_recoz0_TruthVtxZ;
  TH1* m_recoz0_TruthVtxZsin;
  TH1* m_TruthVtxR;
  TH1* m_TruthVtxZ;
  TH1* m_TruthVtxX;
  TH1* m_TruthVtxY;

  TH2* m_TVR_vs_Z;
  TH2* m_recod0_vs_z0_good;
  TH2* m_recod0_vs_z0_crazy;

  // pre-selection truth track plots
  // switch to fill, as they consume a lot of CPU time
  bool m_fillPreSelTruth;
  TH1* m_truthPt;
  TH1* m_truthEta;
  TH1* m_truthPhi;

  // PV plots
  TH1* m_recod0_PrimVtxR;
  TH1* m_recoz0_PrimVtxZ;
  TH1* m_recoz0_PrimVtxZsin;
  TH1* m_PrimVtxR;
  TH1* m_PrimVtxZ;
  TH1* m_PrimVtxX;
  TH1* m_PrimVtxY;

  TH2* m_PVR_vs_Z;

  TH2* m_nSCTHits_vs_eta;
  TH2* m_nPixHits_vs_eta;
  TH2* m_nTotHits_vs_eta;
  TH2* m_nSCTHoles_vs_eta;
  TH2* m_nPixHoles_vs_eta;
  TH2* m_nTotHoles_vs_eta;
  TH2* m_nSCTOutliers_vs_eta;
  TH2* m_nPixOutliers_vs_eta;
  TH2* m_nTotOutliers_vs_eta;
  TH2F* m_nSCTHits_phi_vs_eta;
  TH2F* m_nPixHits_phi_vs_eta;
  TH2F* m_nTotHits_phi_vs_eta;

  TProfile* m_nSCTDeadSensors_vs_eta;
  TProfile* m_nPixDeadSensors_vs_eta;
  TProfile* m_nTotDeadSensors_vs_eta;
  TH2* m_ptvsEtaUNLINKED;
  TH2* m_ptvsEtaLINKED;

  TH2* m_recoMatchvsSiHitsUNLINKED;
  TH2* m_recoMatchvsSCTHitsUNLINKED;
  TH2* m_recoMatchvsPixHitsUNLINKED;
  TH2* m_recoMatchvsSiHitsLINKED;
  TH2* m_recoMatchvsSCTHitsLINKED;
  TH2* m_recoMatchvsPixHitsLINKED;
  TProfile* m_nSCTHits_vs_etaUNLINKED;
  TProfile* m_nPixHits_vs_etaUNLINKED;
  TProfile* m_nTotHits_vs_etaUNLINKED;
  TProfile* m_nSCTHits_vs_etaLINKED;
  TProfile* m_nPixHits_vs_etaLINKED;
  TProfile* m_nTotHits_vs_etaLINKED;
  TH1* m_recoMatchProbUNLINKED;
  TH1* m_recoMatchProbLINKED;


  TH2* m_ptvsEtaUnlinked;
  TH2* m_probvsSCTUnlinked;
  TH2* m_probvsPixUnlinked;
  TH2* m_sharedHitsvsSCTUnlinked;
  TH2* m_sharedHitsvsPixUnlinked;
  TH2* m_pixholesvsPixUnlinked;
  TH2* m_holesvsPixUnlinked;
  TH2* m_sctholesvsSCTUnlinked;
  TH2* m_holesvsSCTUnlinked;
  TH2* m_outliersvsPixUnlinked;
  TH2* m_pixoutliersvsPixUnlinked;
  TH2* m_outliersvsSCTUnlinked;
  TH2* m_sctoutliersvsSCTUnlinked;
  TProfile* m_hitsvsEtaUnlinked;
  TProfile* m_pixHolesvsEtaUnlinked;
  TProfile* m_sctHolesvsEtaUnlinked;
  TH2* m_sctHitsvsPixHitsUnlinked;
  TProfile* m_sctHitsvsEtaUnlinked;
  TProfile* m_pixHitsvsEtaUnlinked;
  TProfile* m_pixOutliersvsEtaUnlinked;
  TProfile* m_sctOutliersvsEtaUnlinked;

  TH2* m_ptvsEtaLinked;
  TH2* m_probvsSCTLinked;
  TH2* m_probvsPixLinked;
  TH2* m_sharedHitsvsSCTLinked;
  TH2* m_sharedHitsvsPixLinked;
  TH2* m_pixholesvsPixLinked;
  TH2* m_holesvsPixLinked;
  TH2* m_sctholesvsSCTLinked;
  TH2* m_holesvsSCTLinked;
  TH2* m_outliersvsPixLinked;
  TH2* m_pixoutliersvsPixLinked;
  TH2* m_sctoutliersvsSCTLinked;
  TH2* m_outliersvsSCTLinked;
  TProfile* m_hitsvsEtaLinked;
  TProfile* m_pixHolesvsEtaLinked;
  TProfile* m_sctHolesvsEtaLinked;
  TH2* m_sctHitsvsPixHitsLinked;
  TProfile* m_sctHitsvsEtaLinked;
  TProfile* m_pixHitsvsEtaLinked;
  TProfile* m_pixOutliersvsEtaLinked;
  TProfile* m_sctOutliersvsEtaLinked;

  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
};




#endif
