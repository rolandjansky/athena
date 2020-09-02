/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_InDetPerfPlot_HITRESIDUAL
#define INDETPHYSVALMONITORING_InDetPerfPlot_HITRESIDUAL
/**
 * @file InDetPerfPlot_HitResidual.h
 * @author shaun roe
 **/


// std includes
#include <atomic>
#include <string>
#include <vector>
#include "TProfile.h"

// local includes

#include "InDetPlotBase.h"

// could be fwd declared?
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

// fwd declaration
class IToolSvc;
class IExtrapolator;


///class holding res plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_HitResidual: public InDetPlotBase {
public:
  InDetPerfPlot_HitResidual(InDetPlotBase* pParent, const std::string& dirName);

  void fill(const xAOD::TrackParticle& trkprt);
  ~InDetPerfPlot_HitResidual() {/** nop **/
  }

private:
  // enum copied from the hitDecorator tool in InDetPhysValMonitoring
  enum Subdetector {
    INVALID_DETECTOR=-1, L0PIXBARR, PIXEL, SCT, TRT, N_SUBDETECTORS
  };
  enum Region {
    INVALID_REGION=-1, BARREL, ENDCAP, N_REGIONS
  };

  void initializePlots();

  TH1* m_residualx_1hit[TRT][N_REGIONS]; // excludes TRT, DBM
  TH1* m_residualx_2ormorehits[TRT][N_REGIONS]; // excludes TRT, DBM
  TH1* m_residualx[N_SUBDETECTORS][N_REGIONS];
  //
  TH1* m_residualy_1hit[TRT][N_REGIONS]; // excludes TRT, DBM
  TH1* m_residualy_2ormorehits[TRT][N_REGIONS]; // excludes TRT, DBM
  TH1* m_residualy[N_SUBDETECTORS][N_REGIONS];
  //
  TH1* m_pullx[N_SUBDETECTORS][N_REGIONS];
  TH1* m_pully[N_SUBDETECTORS][N_REGIONS];
  TH1* m_phiWidth[N_SUBDETECTORS][N_REGIONS];
  TH1* m_etaWidth[N_SUBDETECTORS][N_REGIONS];

  mutable std::atomic<int> m_warnCount{0};
};


#endif
