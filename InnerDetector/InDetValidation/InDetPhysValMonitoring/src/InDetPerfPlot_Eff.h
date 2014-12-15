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
#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/EfficiencyPlots.h"
#include "TrkValHistUtils/EfficiencyPurityCalculator.h"
#include "xAODBase/IParticle.h"  

///class holding Pt plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_Eff:public PlotBase {
public:
	InDetPerfPlot_Eff(PlotBase * pParent, const std::string & dirName, std::string particleName = "Tracks");
	void fillNumerator(const xAOD::IParticle& particle);
	void fillDenominator(const xAOD::IParticle& particle);
	
private:

  std::string m_sParticleType;

  Trk::ParamPlots m_pDenomPlots;
  Trk::ParamPlots m_pNumPlots;  

  TH1* eff_eta;
  TH1* eff_phi;
  TH1* eff_pt;
  TH1* eff_pti;

  TH2* eff_eta_pt;
  TH2* eff_eta_phi;
	
	//plot base has nop default implementation of this; we use it to book the histos
	void initializePlots();
  void finalizePlots();
	
};




#endif
