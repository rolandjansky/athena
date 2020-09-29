/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_InDetPerfPlot_hitEff
#define INDETPHYSVALMONITORING_InDetPerfPlot_hitEff
/**
 * @file InDetPerfPlot_hitEff.h
 * @author nora pettersson
 **/





// local includes

#include "InDetPlotBase.h"
// could be fwd declared?
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"


// std includes
#include <string>

class TProfile;
class TEfficiency;

///class holding res plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_hitEff: public InDetPlotBase {
public:
  InDetPerfPlot_hitEff(InDetPlotBase* pParent, const std::string& dirName);

  void fill(const xAOD::TrackParticle& trkprt);
  ~InDetPerfPlot_hitEff() {/** nop **/
  }

  void FillAdditionalITkPlots(bool fill=false);

private:
  // enum copied from the hitDecorator tool in InDetPhysValMonitoring
  enum Subdetector {
    INVALID_DETECTOR=-1, L0PIXBARR, PIXEL, SCT, TRT, DBM, N_SUBDETECTORS
  };
  enum Region {
    INVALID_REGION=-1, BARREL, ENDCAP, N_REGIONS
  };
  enum LayerType {
    INVALID_LAYER=-1, INNERMOST, NEXT_TO_INNERMOST, OTHER, N_TYPES
  };
  
  const static int N_LAYERS = 5;
  const static int N_SCTLAYERS = 4;

  void initializePlots();

  bool m_fillAdditionalITkPlots;

  //TProfile* m_eff_hit_vs_eta[N_SUBDETECTORS][N_REGIONS];
  TEfficiency* m_hitEfficiencyVsEta[N_SUBDETECTORS][N_REGIONS];
  
  // only if m_fillAdditionalITkPlots==True
  TEfficiency* m_hitEfficiencyVsEta_PerLayer[N_LAYERS][N_SUBDETECTORS][N_REGIONS];
  TEfficiency* m_hitEfficiencyVsEta_PerType[N_TYPES];
  
  bool m_debug;
};


#endif
