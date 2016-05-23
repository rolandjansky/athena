/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_InDetPerfPlot_resITk
#define INDETPHYSVALMONITORING_InDetPerfPlot_resITk
/**
 * @file InDetPerfPlot_resITk.h
 * @author shaun roe
**/


//std includes
#include <string>
#include <vector>
#include "TProfile.h"

//local includes

#include "TrkValHistUtils/PlotBase.h"

//could be fwd declared?
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTruth/TruthParticle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "InDetPlotBase.h"

#include "TFitResultPtr.h"
#include "TFitResult.h"

#include <map>

//fwd declaration
class IToolSvc;
class IExtrapolator;


///class holding res plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_resITk:public PlotBase {
public:
  enum Param{D0, Z0, QOVERP, QOVERPT, THETA, PHI, PT, Z0SIN, NPARAMS};	
	struct pCfg { std::string paraName; std::string paraLabel; std::string paraUnit; std::string resUnit; int nBinsRes; std::vector<float> limRes; int nBinsPrp; std::vector<float> limPrp; int nBinsSig; std::vector<float> limSig; };
	

  InDetPerfPlot_resITk(PlotBase * pParent, const std::string & dirName);

  void fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt);
//  virtual bool isDefined(TString t);
	virtual ~InDetPerfPlot_resITk(){/** nop **/}
	
private:
  static const int m_nEtaBins = 16;
	static const int m_nPtBins = 9;
	static const int m_nResHist = 4;


		float m_EtaBins[m_nEtaBins+1];
		float m_PtBins[m_nPtBins+1]= { 0.0, 0.5, 0.7, 1.0, 2.0, 3.0, 5.0, 10.0, 20.0, 50.0 };
		std::string m_resHisto[m_nResHist]= { "resolutionRMS", "meanRMS", "resolutionGAUS", "meanGAUS" };

		bool m_primTrk;
		bool m_secdTrk;
		bool m_allTrk;
		
		void initializePlots();
	  void finalizePlots();

		void makeResolutions(TH2* h, TH1* h2[m_nResHist], TH1* h3[], bool s);	
		void makeResolutions(TH2* h, TH1* h2[m_nResHist]);	

		void makeResolutions(TH3* h, TH1* h2[][m_nResHist], TH1* h3[][m_nResHist]);
		void getTrackParameters(const xAOD::TruthParticle& truthprt);
		void getTrackParameters(const xAOD::TrackParticle& truthprt);
		void getPlotParameters();
		void getPlots();
		void cloneHistogram(TH1D* h, TH1* hcopy);
		std::vector<float> getResolution(TH1* h, std::string s);	
		
		float trkP[NPARAMS];
		float truetrkP[NPARAMS];
		float trkErrP[NPARAMS];

		float resP[NPARAMS];
		float pullP[NPARAMS];
		float sigP[NPARAMS];

	  pCfg paramProp[NPARAMS];

		TH1* m_resITk_pull[NPARAMS];
		TH1* m_resITk_res[NPARAMS];
		TH1* m_resITk_reco[NPARAMS];
		TH1* m_resITk_true[NPARAMS];
		TH1* m_resITk_sigma[NPARAMS];

		TH1* m_resITk_chargeID;
		TH1* m_resITk_chargeID_vs_eta;
		TH1* m_resITk_chargeID_vs_pt;
		TH1* m_resITk_chargeID_vs_phi;
	
		TH1* m_resITk_momTail;
		TH1* m_resITk_momTail_vs_eta;
		TH1* m_resITk_momTail_vs_pt;
		TH1* m_resITk_momTail_vs_phi;
		TH1* m_resITk_momTail_Frac;
		TH2* m_resITk_chargeID_chgvschg;

		TH2* m_resITk_resHelpereta[NPARAMS];
		TH1* m_resITk_Resolution_vs_eta[NPARAMS][m_nResHist];
		TH1* m_resITk_ResProjections_vs_eta[NPARAMS][m_nEtaBins];

		TH2* m_resITk_resHelpereta_pos[NPARAMS];
		TH1* m_resITk_Resolution_vs_eta_pos[NPARAMS][m_nResHist];
		TH2* m_resITk_resHelpereta_neg[NPARAMS];
		TH1* m_resITk_Resolution_vs_eta_neg[NPARAMS][m_nResHist];

		TH2* m_resITk_resHelperpt[NPARAMS];
		TH1* m_resITk_Resolution_vs_pt[NPARAMS][m_nResHist];
		TH1* m_resITk_ResProjections_vs_pt[NPARAMS][m_nPtBins];

		TH2* m_resITk_resHelperpt_pos[NPARAMS];
		TH1* m_resITk_Resolution_vs_pt_pos[NPARAMS][m_nResHist];
		TH2* m_resITk_resHelperpt_neg[NPARAMS];
		TH1* m_resITk_Resolution_vs_pt_neg[NPARAMS][m_nResHist];


		TH3* m_resITk_resHelperetapt[NPARAMS];
		
		TH2* m_resITk_pullHelperpt[NPARAMS]; //pull width as a function of pT
		TH1* m_resITk_pullResolution_vs_pt[NPARAMS][m_nResHist];
		TH1* m_resITk_pullProjections_vs_pt[NPARAMS][m_nPtBins];

		TH1* m_resITk_Resolution_vs_pt_EtaBin[NPARAMS][4][m_nResHist];
		TH1* m_resITk_Resolution_vs_eta_PtBin[NPARAMS][4][m_nResHist];

		TProfile* m_resITk_meanProfeta[NPARAMS];
		TProfile* m_resITk_meanProfpt[NPARAMS];
		TH2* m_resITk_sigmaVsEta[NPARAMS];
		TProfile* m_DEBUG_D0dep[NPARAMS];
		TProfile* m_DEBUG_FirstHitR_d0;
		TH1* m_DEBUG_NOBREM_d0;
		TH1* m_DEBUG_BREM_d0;
};


#endif
