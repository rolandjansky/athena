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
	InDetPerfPlot_Eff(InDetPlotBase * pParent, const std::string & dirName, std::string particleName = "Tracks");
	void fillNumerator(const xAOD::IParticle& particle);
	void fillDenominator(const xAOD::IParticle& particle);
	void fill(const xAOD::TruthParticle & particle, const bool isReconstructed, const bool isLargeD0Track) ;

private:

	TH1* m_eff_pt_Numerator;
	TH1* m_eff_pt_Denominator;
	TH1* m_eff_eta_Numerator;
	TH1* m_eff_eta_Denominator;
	TH1* m_eff_phi_Numerator;
	TH1* m_eff_phi_Denominator;
	TH1* m_eff_d0_Numerator;
	TH1* m_eff_d0_Denominator;
	TH1* m_eff_z0_Numerator;
	TH1* m_eff_z0_Denominator;
	TH1* m_eff_R_Numerator;
	TH1* m_eff_R_Denominator;
	TH1* m_eff_Z_Numerator;
	TH1* m_eff_Z_Denominator;

	TProfile * m_eff_vs_pt;
	TProfile * m_eff_vs_eta;
	TProfile * m_eff_vs_phi;
	TProfile * m_eff_vs_d0;
	TProfile * m_eff_vs_z0;
	TProfile * m_eff_vs_R;
	TProfile * m_eff_vs_Z;


	TH1* m_eff_pt_Numerator_st;
	TH1* m_eff_eta_Numerator_st;
	TH1* m_eff_phi_Numerator_st;
	TH1* m_eff_d0_Numerator_st;
	TH1* m_eff_z0_Numerator_st;
	TH1* m_eff_R_Numerator_st;
	TH1* m_eff_Z_Numerator_st;

	TProfile * m_eff_vs_pt_st;
	TProfile * m_eff_vs_eta_st;
	TProfile * m_eff_vs_phi_st;
	TProfile * m_eff_vs_d0_st;
	TProfile * m_eff_vs_z0_st;
	TProfile * m_eff_vs_R_st;
	TProfile * m_eff_vs_Z_st;

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
