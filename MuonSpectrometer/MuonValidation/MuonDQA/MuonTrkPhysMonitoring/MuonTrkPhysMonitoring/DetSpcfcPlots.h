/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRKPHYSMONITORING_DETSPCFCPLOTS_H
#define MUONTRKPHYSMONITORING_DETSPCFCPLOTS_H

#include "PlotBase.h"
#include "ResidualPlots.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonTrkPhysMonUtils.h"

class DetSpcfcPlots: public PlotBase {
  public:
    DetSpcfcPlots(PlotBase *pParent);
		void fill(const Trk::Perigee* measPerigee, int imdtstationName, bool isMDT, bool isCSC, bool Fill_HR_Histograms, float residual);

	  TH1F* m_A_EE_UHitRes;
	  TH1F* m_C_EE_UHitRes;
 	  TH1F* m_A_BEE_UHitRes;
	  TH1F* m_C_BEE_UHitRes;
	  TH1F* m_A_CSC_UHitRes;
	  TH1F* m_C_CSC_UHitRes;

	  TH2F* m_HitRes_Eta_Phi_Mean_I;
	  TH2F* m_HitRes_Eta_Phi_Width_I;    
	  TH2F* m_HitRes_Eta_Phi_Mean_M;
	  TH2F* m_HitRes_Eta_Phi_Width_M;
 	  TH2F* m_HitRes_Eta_Phi_Mean_O;
	  TH2F* m_HitRes_Eta_Phi_Width_O;

	  TH1F* m_MDT_TrackResiduals;
	  TH1F* m_MDT_TR_EC;
	  TH1F* m_MDT_TR_BC;
	  TH1F* m_MDT_TR_BA;
	  TH1F* m_MDT_TR_EA;
	  TH1F* m_MDT_TR_Residuals;

    ResidualPlots m_oResidualPlotsAll;
		ResidualPlots m_oResidualPlots_s1;
		ResidualPlots m_oResidualPlots_s2;
		ResidualPlots m_oResidualPlots_s3;
		ResidualPlots m_oResidualPlots_s4;
		ResidualPlots m_oResidualPlots_s5;
		ResidualPlots m_oResidualPlots_s6;
		ResidualPlots m_oResidualPlots_s7;
		ResidualPlots m_oResidualPlots_s8;
		ResidualPlots m_oResidualPlots_s9;
		ResidualPlots m_oResidualPlots_s10;
		ResidualPlots m_oResidualPlots_s11;
		ResidualPlots m_oResidualPlots_s12;
		ResidualPlots m_oResidualPlots_s13;
		ResidualPlots m_oResidualPlots_s14;
		ResidualPlots m_oResidualPlots_s15;
		ResidualPlots m_oResidualPlots_s16;
		
	
  private:
		void CalculateSectorResults(TH1F* summaryPlot, TH1F* inputPlot, int sector);
    void FinalizeTrackResidualPlots(TH1F* hist, int iBin, std::string sLabel);
    void initializePlots();
		void finalizePlots();

};

#endif
