/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_InDetPerfPlot_TrtTest
#define INDETPHYSVALMONITORING_InDetPerfPlot_TrtTest

/**
 * @file InDetPerfPlot_TrtTest.h
 * @author margaret lutz
 **/

// std includes
#include <string>
#include <array>

// local includes
#include "xAODTracking/TrackParticle.h"
#include "InDetPlotBase.h"

class InDetPerfPlot_TrtTest: public InDetPlotBase {
public:
  InDetPerfPlot_TrtTest(InDetPlotBase *pParent, const std::string &dirName);
  void fill(const xAOD::TrackParticle &particle);
private:
  TH1 *m_matchProb;
  TH1 *m_trackCategories, *m_ld0TrackCategories;
  TH1 *m_pt_lrt, *m_pt_lrtExt, *m_pt_lrtTrtHit, *m_pt_lrtTrtOutlier, *m_pt_lrtTrtHitOut;
  TH1 *m_eta_lrt, *m_eta_lrtExt, *m_eta_lrtTrtHit, *m_eta_lrtTrtOutlier, *m_eta_lrtTrtHitOut;
  TH1 *m_phi_lrt, *m_phi_lrtExt, *m_phi_lrtTrtHit, *m_phi_lrtTrtOutlier, *m_phi_lrtTrtHitOut;
  TH1 *m_d0_lrt, *m_d0_lrtExt, *m_d0_lrtTrtHit, *m_d0_lrtTrtOutlier, *m_d0_lrtTrtHitOut;
  TH1 *m_z0_lrt, *m_z0_lrtExt, *m_z0_lrtTrtHit, *m_z0_lrtTrtOutlier, *m_z0_lrtTrtHitOut;

  // Plot base has no default implementation of this; we use it to book the histos
  void initializePlots();

  void finalizePlots(); // Use this to create efficiency plots?
};

#endif
