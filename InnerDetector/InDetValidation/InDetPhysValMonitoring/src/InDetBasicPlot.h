/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_InDetBasicPlot
#define INDETPHYSVALMONITORING_InDetBasicPlot
/**
 * @file InDetBasicPlot.h
 * @author shaun roe
 **/


// std includes
#include <string>
#include <array>

// local includes
#include "InDetPlotBase.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

///class holding Pt plots for Inner Detector RTT Validation and implementing fill methods
class InDetBasicPlot: public InDetPlotBase {
public:
  enum BasicParam {
    D0, Z0, PHI, THETA, ETA, QOVERP, PT, NPARAMS
  };
  enum AdditionalTruthParameters {
    Z0ST, PRODR, PRODZ, NTRUTHPARAMS
  };
  InDetBasicPlot(InDetPlotBase* pParent, const std::string& dirName);
  void fill(const xAOD::TrackParticle& particle);
  void fill(const xAOD::TruthParticle& particle);
private:
  std::array<std::string, NPARAMS> m_paramNames;
  std::array<std::string, NTRUTHPARAMS> m_truthParamNames;
  std::array<TH1*, NPARAMS> m_basicTruthPlots;
  std::array<TH1*, NTRUTHPARAMS> m_extraTruthPlots;
  std::array<TH1*, NPARAMS> m_basicTrackPlots;
  TH2* m_basic_pt_vs_eta, * m_basic_phi_vs_eta;
  TH2* m_truth_pt_vs_eta, * m_truth_phi_vs_eta;
  unsigned int m_d0IsExactlyZeroInTrackCounter;
  unsigned int m_d0IsExactlyZeroInTruthCounter;
  unsigned int m_numCallsToFillTruth;
  unsigned int m_numCallsToFillTrack;
  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
  // print out final stats on exact zeroes
  void finalizePlots();
};




#endif
