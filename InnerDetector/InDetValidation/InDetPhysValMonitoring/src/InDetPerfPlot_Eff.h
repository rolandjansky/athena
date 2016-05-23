/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_EFF
#define INDETPHYSVALMONITORING_INDETPERFPLOT_EFF
/**
 * @file InDetPerfPlot_Eff.cxx
 * @author Gabrel Facini <Gabriel.Facini@cern.ch>
 * Wed Oct 29 09:58:58 CET 2014
 *
 * a lot of this is copied from EfficiencyPlots in the TrkValHistUtils which is dumb
 * the point is that many instances of this will be created so more control of the names
 * is needed.  I don't have permission for that package and time is short...as usual
**/


//std includes
#include <string>

//local includes
#include "InDetPlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"

#include "xAODBase/IParticle.h"  
#include "xAODTracking/TrackParticle.h"

#include "InDetPhysValMonitoring/IInDetPhysValDecoratorTool.h"

///class holding Pt plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_Eff:public InDetPlotBase {

 public:
  InDetPerfPlot_Eff(InDetPlotBase * pParent, const std::string & dirName);

  void pro_fill(const xAOD::TruthParticle& truth, float weight);
  
 private:

  TProfile * m_trackeff_vs_eta;
  TProfile * m_trackeff_vs_pt;
  TProfile * m_trackeff_vs_phi;
  TProfile * m_trackeff_vs_d0;
  TProfile * m_trackeff_vs_z0;
  TProfile * m_trackeff_vs_R;
  TProfile * m_trackeff_vs_Z;
  
  //plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
  void finalizePlots();
  
};

#endif
