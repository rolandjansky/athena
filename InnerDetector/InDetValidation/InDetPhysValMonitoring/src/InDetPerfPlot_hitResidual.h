/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef INDETPHYSVALMONITORING_InDetPerfPlot_hitResidual
#define INDETPHYSVALMONITORING_InDetPerfPlot_hitResidual
/**
 * @file InDetPerfPlot_hitResidual.h
 * @author shaun roe
 **/


// std includes
#include <string>

// local includes

#include "InDetPlotBase.h"

// could be fwd declared?
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

// fwd declaration
class TH1;

///class holding res plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_hitResidual: public InDetPlotBase {
public:
  InDetPerfPlot_hitResidual(InDetPlotBase* pParent, const std::string& dirName);

  void fill(const xAOD::TrackParticle& trkprt);
  ~InDetPerfPlot_hitResidual() {/** nop **/
  }

private:
  // enum copied from the hitDecorator tool in InDetPhysValMonitoring
  enum Subdetector {
    INVALID_DETECTOR=-1, L0PIXBARR, PIXEL, SCT, TRT, DBM, N_SUBDETECTORS
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
  TH1* m_residualpullx[N_SUBDETECTORS][N_REGIONS];
  TH1* m_residualpully[N_SUBDETECTORS][N_REGIONS];
  TH1* m_phiWidth[N_SUBDETECTORS][N_REGIONS];
  TH1* m_etaWidth[N_SUBDETECTORS][N_REGIONS];
};


#endif
