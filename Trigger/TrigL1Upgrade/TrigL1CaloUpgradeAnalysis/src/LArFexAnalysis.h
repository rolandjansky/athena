/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * Name :	LArFexAnalysis.h
 * PACKAGE :	Trigger/TrigL1Upgrade/L1CaloUpgradeAnalysis/LArFexAnalysis
 *
 * AUTHOR :	Denis Oliveira Damazio
 *
 * PURPOSE :	Some initial LArFex output analysis
 *
 * **/

#ifndef TRIGT1CALOEFEXANALYSIS_LARFEXANALYSIS
#define TRIGT1CALOEFEXANALYSIS_LARFEXANALYSIS

#include "AthenaBaseComps/AthAlgorithm.h"
class TFile;
class TH1F;
class TH2F;
#include "xAODTrigCalo/TrigEMClusterContainer.h"

class LArFexAnalysis : public AthAlgorithm
{
public :

	// Usual athena stuff
	LArFexAnalysis( const std::string& name, ISvcLocator* pSvcLocator );
	virtual ~LArFexAnalysis();
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
	TH1F* m_eff_truth_pt_n;
	TH1F* m_eff_truth_pt_d;
	TH1F* m_eff_truth_ptA_n;
	TH1F* m_eff_truth_ptA_d;
	TH1F* m_eff_truth_eta_n;
	TH1F* m_eff_truth_eta_d;
	TH1F* m_res_Eratio_off;
	TH2F* m_res_Eratio_off_eta;
	TH2F* m_res_Eratio_off_pt;
	TH1F* m_res_fracs1_off;
	TH2F* m_res_fracs1_off_eta;
	TH2F* m_res_fracs1_off_pt;
	TH1F* m_res_wtots1_off;
	TH2F* m_res_wtots1_off_eta;
	TH2F* m_res_wtots1_off_pt;
	TH1F* m_eff_pt_n;
	TH1F* m_eff_pt_d;
	TH1F* m_eff_eta_n;
	TH1F* m_eff_eta_d;
        bool m_doTruth;
        bool m_doOffline;
	std::string m_inputClusterName;
	std::string m_inputLvl1Name;

};

#endif
