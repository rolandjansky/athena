/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//	egamma offline monitoring tool
//
//	authors: Kamal Benslama
//		University of Regina
//      Updated by E. Ortega University of Regina
//	date:	02 June 2007
//      First version of this tool was developed by Kamal Benslama and Gia Khoriauli (University of Regina)
//	
/////////////////////////////////////////////////////////////
#ifndef egammaMonTool_H
#define egammaMonTool_H

#include <vector>
#include <string>

#include "GaudiKernel/StatusCode.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "egammaPerformance/IEgammaMonTool.h"

#include "TrigConfL1Data/TriggerItem.h"
//#include "TrigDecision/ITrigDecisionTool.h"
//#include "TrigDecisionInterface/ITrigDecisionTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"


#include "egammaEvent/egammaPIDdefs.h"
#include "TH2.h"
#include "TProfile.h"

#include "CaloEvent/CaloCluster.h"
#include "TLorentzVector.h"
class TH1;
class TH1F;
class egamma;
class StoreGateSvc;

class egammaMonTool : public IEgammaMonTool
{
public:

	egammaMonTool( const std::string & type, const std::string & name,
	                 const IInterface* parent ); 

	virtual ~egammaMonTool();

	//-REMI append _old
	virtual StatusCode bookHistograms_old( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );
	StatusCode bookBaseElectronHistos( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun, MonGroup monGroup);
	StatusCode bookMediumElectronHistos( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun, MonGroup monGroup);
	StatusCode bookCaloClusterElectronHistos( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun, MonGroup monGroup, MonGroup mongroup2, MonGroup mongroup3);
	StatusCode bookEoverPElectronHistos( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun, MonGroup monGroup, MonGroup mongroup2, MonGroup mongroup3);
	StatusCode bookPhotonHistos( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun, MonGroup monGroup, MonGroup mongroup2, MonGroup mongroup3,MonGroup mongroup4, MonGroup mongroup5);
	StatusCode bookZmassHistos(bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun);
	virtual StatusCode fillHistograms();
	//-REMI append _old
	virtual StatusCode procHistograms_old( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun );

	//declarations due to inheritance, the implementation is void
        virtual StatusCode book( bool /*isNewEventsBlock*/, bool /*isNewLumiBlock*/, bool /*isNewRun*/){
	  return StatusCode::SUCCESS;};
        virtual StatusCode fill(){return StatusCode::SUCCESS;};
        virtual StatusCode proc( bool /*isEndOfEventsBlock*/, bool /*isEndOfLumiBlock*/, bool /*isEndOfRun*/){
	  return StatusCode::SUCCESS;};
        virtual StatusCode init(){ return StatusCode::SUCCESS;};


	
protected:


        enum IsEMCut
          {
            Loose = 0,
            Medium = 1,
            Tight = 2,
            TightNoIso = 3
          };
        static const int NofIsEMCuts = 4;
	//		helper methods
	//to fill EMShower related histograms
	void fillEMShowerHistos		(const egamma*, std::string&);	

	//to fill EMTrackMatch related histograms
	void fillEMTrackMatchHistos	(const egamma*, std::string&);
	
	//to fill EMConvert related histograms
	void fillEMConvertHistos	(const egamma*, std::string&);

	//to fill CaloCluster related histograms
	void fillCaloClusterHistos	(const egamma*, std::string&);

	// to fill ID related histograms
	void fillIDHistos (const egamma*, std::string&);

	// to fill 2D Et vs Eta and Et vs Phi histos for detector region monitoring
	void fillRegionHistos (const egamma *, std::string&);

        // to fill IsEM cuts histo
        void fillIsEMHisto (const egamma *);

        // to fill IsEM Medium cut histos
        void fillIsEMMediumCutHistos (const egamma *, std::string&);

        // to fill IsEM Tight  cut photon histos
        void fillIsEMTightCutPhotonHistos (const egamma *, std::string&);

	// to fill E/P studies' histograms
	void fillEoverPHistos(const egamma  *);

        //egammaContainer names for electrons and photons
	std::string		m_ElectronCnt;	
        std::string		m_PhotonCnt;

	//egDetail container names
	std::string		m_EMTrackMatchCnt;
	std::string		m_EMShowerCnt;
	std::string		m_EMConvertCnt;

	//number of CaloSamplings to be monitored 	
        const int           m_nOfCaloSamplings;

        //global index of a histogram in m_el/ph_histos
        int                 m_h_index; 
	
	//histograms related to electron 
        std::vector<TH1*>   m_el_histos;
	std::vector<TH1*>   m_el_ene_histos;
	std::vector<TH1*>   m_el_eta_histos;
	std::vector<TH1*>   m_el_phi_histos;	

        TH1F*	m_el_clus_meanEne;
        TH1F*   m_el_clus_meanEta;	
	TH1F* 	m_el_clus_meanPhi;


	//histograms related to photon
        std::vector<TH1*>   m_ph_histos;    
        std::vector<TH1*>   m_ph_ene_histos;
        std::vector<TH1*>   m_ph_eta_histos;
        std::vector<TH1*>   m_ph_phi_histos;

        TH1F*	m_ph_clus_meanEne; 
        TH1F*   m_ph_clus_meanEta;
        TH1F*   m_ph_clus_meanPhi;
 
	
        // name of the TrigDecision class
        std::string m_trigDecisionToolName;

        // Handle for the trig decision tool
	//        ToolHandle<TrigDec::TrigDecisionTool> m_trigDec;
        //ToolHandle<ITrigDecisionTool> m_trigDec;
        ToolHandle<Trig::TrigDecisionTool> m_trigDec;


        // Trigger items
        std::vector<std::string> m_el_triggers;
        std::vector<std::string> m_ph_triggers;
	// user flag to switch trigger on or off
	bool m_useTrigger;
        // map of string to signatures
        std::map<std::string,int> m_signatures;

        StoreGateSvc*       m_storeGate;


	// Cuts on Electrons
	double m_ElectronClusterEtCut;
	double m_ZmassLowerCut;
	double m_ZmassUpperCut;

 private:

	/////// Miscellaneous histograms ///////

        // IsEM histo
        TH1F * m_IsEMHisto;
        // el_F2 histo
        TH1F * m_el_F2Histo;
        // ph_F2 histo
        TH1F * m_ph_F2Histo;
	// Histos of Et > 100,500 and 1000 GeV
	TH1F * m_el_HighEtRangesHisto;
	// el_ClusterPhiHisto
	TH1F * m_el_ClusterPhiHisto;
	// ph_ClusterPhiHisto
	TH1F * m_ph_ClusterPhiHisto;

        /////// Medium cut histograms (Electron)///////

        // Cluster Et
        TH1F * m_el_IsEMMedium_ClusterEtHisto;
        // Cluster Eta
        TH1F * m_el_IsEMMedium_ClusterEtaHisto;
	// el_ClusterPhiHisto
	TH1F * m_el_IsEMMedium_ClusterPhiHisto;
        // Cluster Et vs Eta
        TH2F * m_el_IsEMMedium_EtvsEtaHisto;
        // Cluster Et vs Phi
        TH2F * m_el_IsEMMedium_EtvsPhiHisto;
        // EoverP
        TH1F * m_el_IsEMMedium_EoverPHisto;
        // DeltaEta
        TH1F * m_el_IsEMMedium_DeltaEta1Histo;
        // DeltaPhi
        TH1F * m_el_IsEMMedium_DeltaPhi2Histo;
        // Et in hadronic calorimeter
        TH1F * m_el_IsEMMedium_EthadHisto;
        // F1
        TH1F * m_el_IsEMMedium_F1Histo;
        // F2
        TH1F * m_el_IsEMMedium_F2Histo;
        // F3
        TH1F * m_el_IsEMMedium_F3Histo;
        // rE233E237
        TH1F * m_el_IsEMMedium_rE233E237Histo;
	// rE237E277
        TH1F * m_el_IsEMMedium_rE237E277Histo;
	// Histos of Et > 100,500 and 1000 GeV
	TH1F * m_el_IsEMMedium_HighEtRangesHisto;


        /////// Tight cut histograms (Photon)///////
	// Cluster Et
	TH1F * m_ph_IsEMTight_ClusterEtHisto;
	// Cluster Eta
	TH1F * m_ph_IsEMTight_ClusterEtaHisto;
	// Cluster Phi
	TH1F * m_ph_IsEMTight_ClusterPhiHisto;
	// F1
	TH1F * m_ph_IsEMTight_F1Histo;
	// F2
	TH1F * m_ph_IsEMTight_F2Histo;
	// F3
	TH1F * m_ph_IsEMTight_F3Histo;
	// Hadronic leakage
	TH1F * m_ph_IsEMTight_EtHadHisto;
	// e233/237
	TH1F * m_ph_IsEMTight_rE233E237Histo;
	// e237/e277
	TH1F * m_ph_IsEMTight_rE237E277Histo;
	// Core energy
	TH1F * m_ph_IsEMTight_ECoreHisto;
	// convTrkMatch
	TH1F * m_ph_IsEMTight_convTrkMatchHisto;
	// convAngleMatch
	TH1F * m_ph_IsEMTight_convAngleMatchHisto;
	// Cluster Et vs Cluster Eta
	TH2F * m_ph_IsEMTight_EtvsEtaHisto;
	// Cluster Et vs Cluster Phi
	TH2F * m_ph_IsEMTight_EtvsPhiHisto;


	// Et threshold for electrons/positrons for E/P studies
	double m_EtCut;
	// Eta binning for E/P studies
	std::vector<double> m_eta_cuts;
	// Phi binning for E/p studies
	std::vector<double> m_phi_cuts;

	// Histograns for E/P studies
	// E/P vs eta
	TH2F * m_el_Electron_EoverPvsEta;
	TH2F * m_el_Positron_EoverPvsEta;
	// E/P vs phi
	TH2F * m_el_Electron_EoverPvsPhi;
	TH2F * m_el_Positron_EoverPvsPhi;
	// E/P vs eta passing ElectronTight
	TH2F * m_el_TightElectron_EoverPvsEta;
	TH2F * m_el_TightPositron_EoverPvsEta;
	// E/P vs phi passing ElectronTight
	TH2F * m_el_TightElectron_EoverPvsPhi;
	TH2F * m_el_TightPositron_EoverPvsPhi;
	// An E/P histo for each eta bin (Electrons)
	std::vector<TH1F *> m_el_Electron_EoverP_PerEtaBin;
	// An E/P histo for each phi bin (Electrons)
	std::vector<TH1F *> m_el_Electron_EoverP_PerPhiBin;
	// An E/P histo for each eta bin (Positrons)
	std::vector<TH1F *> m_el_Positron_EoverP_PerEtaBin;
	// An E/P histo for each phi bin (Positrons)
	std::vector<TH1F *> m_el_Positron_EoverP_PerPhiBin;


	// The following plots are filled only for candidates
	// with E/p > 2 and passing kinematic cuts.
	// An eta histo for each eta bin (Electrons)
	std::vector<TH1F *> m_el_Electron_Eta_PerEtaBin;
	// An phi histo for each phi bin (Electrons)
	std::vector<TH1F *> m_el_Electron_Phi_PerPhiBin;
	// An eta histo for each eta bin (Positrons)
	std::vector<TH1F *> m_el_Positron_Eta_PerEtaBin;
	// An phi histo for each phi bin (Positrons)
	std::vector<TH1F *> m_el_Positron_Phi_PerPhiBin;
	// A E/p histo (Electrons)
	TH1F * m_el_Electron_EoverP;
	// A E/p histo (Positrons)
	TH1F * m_el_Positron_EoverP;

	// Helper methods
        bool passesIsEMCut(const unsigned int, const egamma *);
	double InvMass(const CaloCluster *, const CaloCluster *);
	bool inBarrel(const CaloCluster * );
	bool inCrack(const CaloCluster * );
	bool inEndcap(const CaloCluster * );
	void makeEffHisto(TH1F* h_num, TH1F* h_denom, TProfile* h_eff);
	/** Z mass histos */
	// Cluster Et
	TH1F * m_z_ClusterEt;
	// Cluster Eta
	TH1F * m_z_ClusterEta;
	// Cluster Phi
	TH1F * m_z_ClusterPhi;
	// Cluster Et vs Eta
	TH2F * m_z_ClusterEtvsEta;
	// Cluster EtvsPhi
	TH2F * m_z_ClusterEtvsPhi;

	// Associated track Pt
	TH1F * m_z_TrackPt;
	// Associated track Eta
	TH1F * m_z_TrackEta;
	// Associated track Phi
	TH1F * m_z_TrackPhi;
	// Associated track Pt vs Eta
	TH2F * m_z_TrackPtvsEta;
	// Associated track Pt vs Phi
	TH2F * m_z_TrackPtvsPhi;

	// Associated cluster with track Et
	TH1F * m_z_ClusterWithTrackEt;
	// Associated cluster with track Eta
	TH1F * m_z_ClusterWithTrackEta;
	// Associated cluster with track Phi
	TH1F * m_z_ClusterWithTrackPhi;
	// Associated cluster with track Et vs Eta
	TH2F * m_z_ClusterWithTrackEtvsEta;
	// Associated cluster with track Et vs Phi
	TH2F * m_z_ClusterWithTrackEtvsPhi;


	// Cluster Et after  Identification cuts
	TH1F * m_z_CutClusterEt;
	// Cluster Eta after  Identification cuts
	TH1F * m_z_CutClusterEta;
	// Cluster Phi after  Identification cuts
	TH1F * m_z_CutClusterPhi;

	// Z mass 
	TH1F * m_z_mass;
	// Z mass Barrel - Barrel
	TH1F * m_z_mass_bb;
	// Z mass Barrel - Endcap
	TH1F * m_z_mass_be;
	// Z mass Barrel - Crack
	TH1F * m_z_mass_bc;	
	// Z mass Endcap - Crack
	TH1F * m_z_mass_ec;	
	// Z mass Endcap - Endcap
	TH1F * m_z_mass_ee;	

	// Efficiency plots
	TProfile * m_z_ClusterEtEff;
	TProfile * m_z_ClusterEtaEff;
	TProfile * m_z_ClusterPhiEff;

	// Efficiency plots after Identification cuts
	TProfile * m_z_CutClusterEtEff;
	TProfile * m_z_CutClusterEtaEff;
	TProfile * m_z_CutClusterPhiEff;
};

#endif
