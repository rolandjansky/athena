/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_InDetPerfPlot_HITEFFICIENCY
#define INDETPHYSVALMONITORING_InDetPerfPlot_HITEFFICIENCY
/**
 * @file InDetPerfPlot_HitEfficiency.h
 * @author nora pettersson
 **/





// local includes

#include "InDetPlotBase.h"
// could be fwd declared?
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"


// std includes
#include <atomic>
#include <string>

class TProfile;
class TEfficiency;

///class holding res plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_HitEfficiency: public InDetPlotBase {
public:
  InDetPerfPlot_HitEfficiency(InDetPlotBase* pParent, const std::string& dirName);

  void fill(const xAOD::TrackParticle& trkprt);
  ~InDetPerfPlot_HitEfficiency() {/** nop **/
  }

private:
  // enum copied from the hitDecorator tool in InDetPhysValMonitoring
  enum Subdetector {
    INVALID_DETECTOR=-1, L0PIXBARR, PIXEL, SCT, TRT, N_SUBDETECTORS
  };
  enum Region {
    INVALID_REGION=-1, BARREL, ENDCAP, N_REGIONS
  };
private:
  void initializePlots();

  //TProfile* m_eff_hit_vs_eta[N_SUBDETECTORS][N_REGIONS];
  TEfficiency* m_HitEfficiencyVsEta[N_SUBDETECTORS][N_REGIONS];
  bool m_debug;

  mutable std::atomic<int> m_warnCount{0};
};


#endif
