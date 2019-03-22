/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * Name :	TrigT1CaloEFex.h
 * PACKAGE :	Trigger/L1CaloUpgrade/TrigT1CaloEFex
 *
 * AUTHOR :	Denis Oliveira Damazio
 *
 * PURPOSE :	Emulate the eFex system, for phase 1, L1Calo upgrade
 *
 * **/

#ifndef TRIGT1CALOEFEX_TRIGT1CALOEFEX
#define TRIGT1CALOEFEX_TRIGT1CALOEFEX

#include "../src/TrigT1CaloBaseFex.h"

class TrigT1CaloEFex : public TrigT1CaloBaseFex
{
public :
	TrigT1CaloEFex( const std::string& name, ISvcLocator* pSvcLocator );
	virtual ~TrigT1CaloEFex();
	StatusCode initialize();
	StatusCode finalize();
	StatusCode execute();

private :
	/** do I want to monitor things */
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
        /** output name */
        std::string m_outputClusterName;
	/** cut for time measurement */
	float m_timeThr;
	/** cut for seed energy in MeV */
	float m_seedE;
	/** boolean to choose cluster formation */
	bool m_energyWeightedCluster;
	/** deta for the cluster definition */
	float m_deta_cellFormation;
	/** dphi for the cluster definition */
	float m_dphi_cellFormation;
	/** deta for the cluster definition closer to egamma standard */
	float m_deta_clusterFormation;
	/** dphi for the cluster definition closer to egamma standard */
	float m_dphi_clusterFormation;
	/** different deta for some of the shower shapes */
	float m_deta_clusterFormation_2;
	/** different dphi for some of the shower shapes */
	float m_dphi_clusterFormation_2;
	/** minimum cluster energy of SCs in EMB2 or EMEC2 */
	float m_clusterE_EMB2_EMEC2;
	/** limit for eta of cluster */
	float m_eta_limit;
	
	// Acessors for dynamic decorations
	SG::AuxElement::Accessor<float>* Run3eFEXIsolation_REta;
	SG::AuxElement::Accessor<float>* Run3eFEXIsolation_RHad;
	SG::AuxElement::Accessor<int>* pass_Run3eFEXIsolation_REta;
	SG::AuxElement::Accessor<int>* pass_Run3eFEXIsolation_RHad;
	SG::AuxElement::Accessor<int>* pass_Run3eFEXIsolation_wstot;
	SG::AuxElement::Accessor<int>* pass_Run3eFEX_clusterEnergy;

};

#endif
