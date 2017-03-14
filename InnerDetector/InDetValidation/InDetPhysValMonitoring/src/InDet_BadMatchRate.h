/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDET_BADMATCHRATE
#define INDETPHYSVALMONITORING_INDET_BADMATCHRATE

/**
 *@file InDet_BadMatchRate.h
 *@author: Max Baugh
 *
 **/


#include "InDetPlotBase.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODJet/Jet.h"
#include <string>

class TProfile;

class InDet_BadMatchRate: public InDetPlotBase {
public:
  InDet_BadMatchRate(InDetPlotBase* pParent, std::string sDir);

  void fillBMR(const xAOD::TrackParticle& particle, float weight);
  void fillRF(const xAOD::TrackParticle& particle, float weight);

  void jetBMR(const xAOD::TrackParticle& track, const xAOD::Jet& jet, float weight);
private:
  TProfile* m_BadMatchRate;
  TProfile* m_BMR_vs_logpt;

  TProfile* m_ReallyFakeRate;

  TProfile* m_trackinjet_badmatchrate_vs_dr_gr_j100;

  void initializePlots();
  // void finalizePlots(); //I'm not 100% sure a finalize section is actually needed here
};


#endif
