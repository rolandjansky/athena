/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * Name :	EFexAnalysis.h
 * PACKAGE :	Trigger/TrigL1Upgrade/TrigL1CaloUpgradeAnalysis/EFexAnalysis
 *
 * AUTHOR :	Denis Oliveira Damazio
 *
 * PURPOSE :	Some initial EFex output analysis
 *
 * **/

#ifndef TRIGT1CALOEFEXANALYSIS_EFEXANALYSIS
#define TRIGT1CALOEFEXANALYSIS_EFEXANALYSIS

#include "AthenaBaseComps/AthAlgorithm.h"
class TFile;
class TH1F;
class TH2F;
#include "xAODTrigCalo/TrigEMClusterContainer.h"

class EFexAnalysis : public AthAlgorithm
{
public :

	// Usual athena stuff
	EFexAnalysis( const std::string& name, ISvcLocator* pSvcLocator );
	virtual ~EFexAnalysis();
	StatusCode initialize();
	StatusCode finalize();
	StatusCode execute();

private :
	/** Monitoring Enabled */
	bool m_enableMon;
	/** TFile for monitoring */
	TFile * m_histFile;
	/** Histograms with monitoring activity */
	TH1F* m_EtSElectron;
	TH1F* m_HadEtSElectron;
	TH1F* m_EtaSElectron;
	TH1F* m_PhiSElectron;
	TH2F* m_EtSElectronEta;
	TH2F* m_HadEtSElectronEta;
        TH1F* m_rEta;
	TH1F* m_f1;
	TH1F* m_f3;
        TH1F* m_EtSElectronA;
        TH1F* m_HadEtSElectronA;
        TH1F* m_EtaSElectronA;
        TH1F* m_PhiSElectronA;
        TH2F* m_EtSElectronEtaA;
        TH2F* m_HadEtSElectronEtaA;
        TH1F* m_rEtaA;
        TH1F* m_f1A;
        TH1F* m_f3A;
	TH1F* m_res_truth;
	TH2F* m_res_truth_eta;
	TH2F* m_res_truth_pt;
	TH2F* m_res_truth_ptA;
	TH1F* m_eff_truth_pt_n;
	TH1F* m_eff_truth_pt_d;
	TH1F* m_eff_truth_ptA_n;
	TH1F* m_eff_truth_ptA_d;
	TH1F* m_eff_truth_eta_n;
	TH1F* m_eff_truth_eta_d;
	TH1F* m_res_off;
	TH2F* m_res_off_eta;
	TH2F* m_res_off_pt;
	TH2F* m_res_off_nvtx;
	TH2F* m_res_off_bcid;
	TH2F* m_res_off_bcidN;
	TH1F* m_res_rEta_off;
	TH2F* m_res_rEta_off_eta;
	TH2F* m_res_rEta_off_pt;
	TH2F* m_res_rEta_off_nvtx;
	TH2F* m_res_rEta_off_bcid;
	TH2F* m_res_rEta_off_bcidN;
	TH1F* m_res_eta_off;
	TH2F* m_res_eta_off_eta;
	TH2F* m_res_eta_off_pt;
	TH1F* m_res_phi_off;
	TH2F* m_res_phi_off_eta;
	TH2F* m_res_phi_off_pt;
	TH1F* m_res_f1_off;
	TH2F* m_res_f1_off_eta;
	TH2F* m_res_f1_off_pt;
	TH2F* m_res_f1_off_nvtx;
	TH1F* m_res_f3_off;
	TH2F* m_res_f3_off_eta;
	TH2F* m_res_f3_off_pt;
	TH2F* m_res_f3_off_nvtx;
	TH2F* m_res_f3_off_bcid;
	TH2F* m_res_f3_off_bcidN;
	TH1F* m_eff_off_pt_n;
	TH1F* m_eff_off_pt_d;
	TH1F* m_eff_off_eta_n;
	TH1F* m_eff_off_eta_d;
        bool m_doTruth;
        bool m_doOffline;
	std::string m_inputClusterName;
	std::string m_inputLvl1Name;

};

#endif
