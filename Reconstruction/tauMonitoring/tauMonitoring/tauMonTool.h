#ifndef TAUMONTOOL_H
#define TAUMONTOOL_H

//*********************************************************************
// tauMonTool.h : Implementation of offline tau Data Quality 
//                histogramming, inheriting from ManagedMonitorToolBase
//
// authors:  C Cuenca Almenar, S Demers, E Ideal, A Leister, YALE
// 			Felix Friedrich (basic xAOD migration)
//*********************************************************************

#include "TH1.h"
#include "TH2.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "xAODTau/TauDefs.h"
#include "xAODTau/TauJet.h"

class tauMonTool : public ManagedMonitorToolBase {

public:
	// constructor
	tauMonTool( const std::string & type, const std::string & name, const IInterface* parent );
	// destructor
	virtual ~tauMonTool();

	StatusCode bookHists();
	StatusCode bookHistograms();
	StatusCode fillHistograms();
	StatusCode procHistograms();
	StatusCode bookHistogramsRecurrent( );
	StatusCode Book1DHist(TH1 ** hist, MonGroup * monName, std::string histName, std::string histTitle, int NBins, double lowBin, double highBin);
	StatusCode Book2DHist(TH2 ** hist, MonGroup * monName, std::string histName, std::string histTitle, int NXBins, double lowXBin, double highXBin, int NYBins, double lowYBin, double highYBin);

protected:
	std::string m_tauJetKey;
	bool        m_doTrigger;

	double  m_maxNLB;
	int  m_currentLB;
        int  m_NevtLB ; 
        float privtx_z0 ;

	//global plots
	struct s_basicPlots {
		TH1* h_ntaus_vs_LB;
		int  h_ntausLB;
		TH1* h_ntaus;

		TH1* h_eta;
		TH1* h_phi;
		TH1* h_et;
		TH1* h_charge;
		TH1* h_numTracks;
                TH1* h_nclst;
		TH1* h_nHighPTtaus;

                TH1* h_pT_TES_MVA ;
                TH1* h_pT_combined ;

		TH2* h_EtVsEta;
		TH2* h_EtVsPhi;
		TH2* h_PhiVsEta;
		TH2* h_PhiVsEta_et15;
		TH2* h_PhiVsEta_et15_BDTLoose ;

                TH2* h_numCoreTracks_vs_LB ;
		TH2* h_Eta_vs_LB;
		TH2* h_Phi_vs_LB;
		TH2* h_Et_vs_LB;
		TH2* h_Phi_vs_LB_et15_BDTLoose ;
		TH2* h_Eta_vs_LB_et15_BDTLoose ;

	} m_basicPlots, m_tauBasePlots, m_emPlots, m_jetPlots, m_mbtsPlots, m_basic_LS;

	//trigger
	TH1* m_triggers;
	TH2* m_triggers_vs_LB;

	struct s_BDTFolder{
		TH1* h_et;
		TH1* h_eta;
		TH1* h_phi;
		TH1* h_nTracks;
                TH1* h_nclst;
                TH1* h_panmode ;

	};

        struct s_tauJetBDT {
                TH1* h_CentFrac;
                TH1* h_trkAvgDist;
                TH1* h_dRmax;
                TH1* h_SumPtTrkFrac ;
                TH1* h_etOverPtLeadTrk ;
                TH1* h_absipSigLeadTrk ;
                TH1* h_trFlightPathSig ;
                TH1* h_massTrkSys ;
                TH1* h_EMPOverTrkSysP ;
                TH1* h_mEflowApprox ;
                TH1* h_ptRatioEflowApprox ;
                TH1* h_ptIntermediateAxis ;
                
        } ;

        struct s_EleVeto {
                TH1* h_etHotShotWinOverPtLeadTrk;
                TH1* h_EMFracFixed;
                TH1* h_hadLeakFracFixed;
                TH1* h_PSSFrac ;
        } ;

	struct s_idFolder {
		TH1* h_tauBDTLoose;
		TH1* h_tauBDTMedium;
		TH1* h_tauBDTTight;
		TH1* h_muonVeto;
		TH1* h_JetBDTBkgMedium;    // ?
		TH1* h_eleBDTMedium;   // ?
		TH1* h_eleBDTTight;   // ?
		TH1* h_BDTJetScore;
		TH1* h_BDTJetScoreSigTrans;
                TH1* h_BDTEleScoreSigTrans;

		s_BDTFolder BDTLooseFolder;
                s_tauJetBDT tauJetinputs ;
                s_EleVeto EleVetoinputs ;
	};

	struct s_trkFolder {
                TH1* h_dRJetSeedAxis;   // ?
		TH1* h_leadTrkPt;
		TH1* h_trkWidth2;
		TH1* h_ipZ0SinThetaSigLeadTrk;  
		TH1* h_d0;
                TH1* h_z0sinThetaTJVA;
                TH1* h_z0sinThetaPriVtx;
		TH1* h_phi;
		TH1* h_eta;
		TH1* h_pT;
		TH1* h_nHighPTtaus;

		TH1* h_numberOfTRTHighThresholdHits;
                TH1* h_eProbabilityHT;
		TH1* h_numberOfTRTHits;
		TH1* h_numberOfTRTHighThresholdOutliers;
		TH1* h_numberOfTRTOutliers;

		TH1* h_numberOfSCTHits;
                TH1* h_numberOfSCTSharedHits;

		TH1* h_numberOfPixelHits;
                TH1* h_numberOfPixelSharedHits;
                TH1* h_numberOfInnermostPixelLayerHits;

                TH1* h_leadTrackDeltaEta;  // ?
                TH1* h_leadTrackDeltaPhi;   // ?

                TH1* h_rConv;
                TH1* h_rConvII;
		TH2* h_z0_vs_LB;

	};

	struct s_caloFolder {
		TH1* h_eta;
		TH1* h_phi;
                TH1* h_jetSeedPt;
		TH1* h_etEMAtEMScale;
		TH1* h_etHadAtEMScale;
		TH1* h_EMRadius;
		TH1* h_hadRadius;
		TH1* h_isolFrac;
		TH1* h_stripWidth2;
		TH1* h_nStrip;
      
		TH2* h_CentFrac_vs_LB;
		TH2* h_isolFrac_vs_LB;
	};

	struct s_kinFolder {
		TH1* h_ntaus;
		TH1* h_eta;
		TH1* h_phi;
		TH1* h_et;
		TH1* h_charge;

		TH2* h_PhiVsEta;
		TH2* h_Eta_vs_LB;
		TH2* h_Phi_vs_LB;

	};

        struct s_sbstrctFolder {
               TH1* h_nShot ;
               TH1* h_InvMass ;
               TH1* h_L2EOverAllClusterE ;
               TH1* h_IsoCorr ;
               TH1* h_EMFracTrk ;
               TH1* h_nNeutPFO ;
               TH1* h_stpt3 ;
               TH1* h_pi0bdt ;
               TH1* h_panmode ;
               TH1* h_panpt ;
               TH1* h_paneta ;
               TH1* h_panphi ;
        } ;

	struct s_mainFolder {
		s_idFolder   idFolder;
		s_trkFolder  trkFolder;
		s_caloFolder caloFolder;
		s_kinFolder  kinFolder;
                s_sbstrctFolder sbstrctFolder ;

	} m_tauB, m_tauCR, m_tauE, m_tauPlots, m_tauLS ;

	// Z details
	TH1* m_eta_Tau_Z;
	TH2* m_pTVsEta_Tau_Z;
	TH2* m_pTVsEta_Lepton_Z;

	// W details
	TH2* m_pTVsEta_Tau_W;

private:
	StoreGateSvc* m_storeGate;

        std::vector< MgmtParams<TH1> > Histos_vsLB ;

        bool m_Trigged, m_doLS, m_extendLB, m_reCurrent ;
        std::vector< std::vector< std::string > > m_trigItems ;

	StatusCode bookPlots_LB_dependent(s_basicPlots& trigPlots, std::string prefix, int start = -5 );
	StatusCode bookBasicPlots(s_basicPlots& trigPlots, MonGroup &aGroup, std::string preffix );
	void fillBasicPlots(s_basicPlots& someBasicPlots, const xAOD::TauJet* tau);

	StatusCode bookHistos(s_mainFolder& mainFolder, std::string folderName, Interval_t interval);
	StatusCode bookKinHistos(s_kinFolder& folder,  MonGroup &aGroup);
	StatusCode bookIDHistos(s_idFolder& folder,std::string folderName, Interval_t interval) ;
	StatusCode bookTrackHistos(s_trkFolder& folder,std::string folderName, Interval_t interval);
	StatusCode bookCaloHistos(s_caloFolder& folder,std::string folderName, Interval_t interval);
	StatusCode bookSubStructureHistos(s_sbstrctFolder& folder,std::string folderName, Interval_t interval);
	StatusCode bookBDTLooseHistos(s_BDTFolder& folder,std::string folderName, Interval_t interval);
        StatusCode bookJetsigBDTHistos( s_tauJetBDT& folder, std::string folderName, Interval_t interval) ;
        StatusCode bookEleVetoHistos( s_EleVeto & folder, std::string folderName, Interval_t interval) ;
	StatusCode bookPhysicsHistograms();
	//StatusCode bookOldHistograms();

	StatusCode fillHistograms(s_mainFolder& mainFolder, const xAOD::TauJet* tau);
	StatusCode fillHistogramsTauTrig(s_mainFolder& mainFolder, const xAOD::TauJet* tau);
	StatusCode fillHistogramsLowStat(s_mainFolder& mainFolder, const xAOD::TauJet* tau);

	StatusCode fillKinHistos(s_kinFolder& folder, const xAOD::TauJet* tau) ;
	StatusCode fillIDHistos(s_idFolder& folder, const xAOD::TauJet* tau);
        StatusCode fillJetsigBDTHistos( s_tauJetBDT& folder, const xAOD::TauJet* tau);
        StatusCode fillEleVetoHistos( s_EleVeto & folder, const xAOD::TauJet* tau);
	StatusCode fillTrackHistos(s_trkFolder& folder, const xAOD::TauJet* tau);

	StatusCode fillCaloHistos(s_caloFolder& folder, const xAOD::TauJet* tau);
	StatusCode fillSubStructureHistos(s_sbstrctFolder&, const xAOD::TauJet* tau);
	StatusCode fillBDTHistos(s_BDTFolder&, const xAOD::TauJet* tau);
	StatusCode fillPhysicsHistograms(const xAOD::TauJet* tau);

	StatusCode Book1DHistVsLB(TH1 ** hist, MonGroup * monName, std::string histName, std::string histTitle, int NBins, double lowBin, double highBin);
	StatusCode Book2DHistVsLB(TH2 ** hist, MonGroup * monName, std::string histName, std::string histTitle, int NXBins, double lowXBin, double highXBin, int NYBins, double lowYBin, double highYBin);
	std::string fixName(std::string name);
	void addBinLabelIDHistos(TH1* h);
};

#endif
