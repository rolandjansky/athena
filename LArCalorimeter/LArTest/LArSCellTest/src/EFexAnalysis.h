/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * Name :	EFexAnalysis.h
 * PACKAGE :	Trigger/L1CaloUpgrade/EFexAnalysis
 *
 * AUTHOR :	Denis Oliveira Damazio
 *
 * PURPOSE :	Some initial EFex output analysis
 *
 * **/

#ifndef TRIGT1CALOEFEX_EFEXANALYSIS
#define TRIGT1CALOEFEX_EFEXANALYSIS

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
	TFile * m_histFile = nullptr;
	/** Histograms with monitoring activity */
	TH1F* m_EtSElectron = nullptr;
	TH1F* m_HadEtSElectron = nullptr;
	TH1F* m_EtaSElectron = nullptr;
	TH1F* m_PhiSElectron = nullptr;
	TH2F* m_EtSElectronEta = nullptr;
	TH2F* m_HadEtSElectronEta = nullptr;
        TH1F* m_rEta = nullptr;
	TH1F* m_f1 = nullptr;
	TH1F* m_f3 = nullptr;
        TH1F* m_EtSElectronA = nullptr;
        TH1F* m_HadEtSElectronA = nullptr;
        TH1F* m_EtaSElectronA = nullptr;
        TH1F* m_PhiSElectronA = nullptr;
        TH2F* m_EtSElectronEtaA = nullptr;
        TH2F* m_HadEtSElectronEtaA = nullptr;
        TH1F* m_rEtaA = nullptr;
        TH1F* m_f1A = nullptr;
        TH1F* m_f3A = nullptr;
	TH1F* m_res_truth = nullptr;
	TH2F* m_res_truth_eta = nullptr;
	TH2F* m_res_truth_pt = nullptr;
	TH2F* m_res_truth_ptA = nullptr;
	TH1F* m_eff_truth_pt_n = nullptr;
	TH1F* m_eff_truth_pt_d = nullptr;
	TH1F* m_eff_truth_ptA_n = nullptr;
	TH1F* m_eff_truth_ptA_d = nullptr;
	TH1F* m_eff_truth_eta_n = nullptr;
	TH1F* m_eff_truth_eta_d = nullptr;
	TH1F* m_res_off = nullptr;
	TH2F* m_res_off_eta = nullptr;
	TH2F* m_res_off_pt = nullptr;
	TH2F* m_res_off_nvtx = nullptr;
	TH1F* m_res_rEta_off = nullptr;
	TH2F* m_res_rEta_off_eta = nullptr;
	TH2F* m_res_rEta_off_pt = nullptr;
	TH2F* m_res_rEta_off_nvtx = nullptr;
	TH1F* m_res_eta_off = nullptr;
	TH2F* m_res_eta_off_eta = nullptr;
	TH2F* m_res_eta_off_pt = nullptr;
	TH1F* m_res_phi_off = nullptr;
	TH2F* m_res_phi_off_eta = nullptr;
	TH2F* m_res_phi_off_pt = nullptr;
	TH1F* m_res_f1_off = nullptr;
	TH2F* m_res_f1_off_eta = nullptr;
	TH2F* m_res_f1_off_pt = nullptr;
	TH2F* m_res_f1_off_nvtx = nullptr;
	TH1F* m_res_f3_off = nullptr;
	TH2F* m_res_f3_off_eta = nullptr;
	TH2F* m_res_f3_off_pt = nullptr;
	TH1F* m_eff_off_pt_n = nullptr;
	TH1F* m_eff_off_pt_d = nullptr;
	TH1F* m_eff_off_eta_n = nullptr;
	TH1F* m_eff_off_eta_d = nullptr;
        bool m_doTruth;
        bool m_doOffline;
	std::string m_inputClusterName;
	std::string m_inputLvl1Name;

};

#endif
