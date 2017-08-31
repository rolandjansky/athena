/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * Name :	EFexEratioAlgo.h
 * PACKAGE :	Trigger/TrigL1Upgrade/TrigL1CaloUpgradeAnalysis/EFexEratioAlgo
 *
 * AUTHOR :	Denis Oliveira Damazio
 *
 * PURPOSE :	Some initial EFex output analysis
 *
 * **/

#ifndef TRIGT1CALOEFEXANALYSIS_EFEXERATIOALGO
#define TRIGT1CALOEFEXANALYSIS_EFEXERATIOALGO

#include "AthenaBaseComps/AthAlgorithm.h"
class TFile;
class TH1F;
class TH2F;
#include "xAODTrigCalo/TrigEMClusterContainer.h"

class EFexEratioAlgo : public AthAlgorithm
{
public :

	// Usual athena stuff
	EFexEratioAlgo( const std::string& name, ISvcLocator* pSvcLocator );
	virtual ~EFexEratioAlgo();
	StatusCode initialize();
	StatusCode finalize();
	StatusCode execute();

private :
	/** fast delta phi */
        inline float deltaPhi(const float& phi1, const float& phi2) { float dphi=fabsf(phi1-phi2);
		dphi=fabsf(M_PI-dphi); dphi=fabsf(M_PI-dphi); return dphi; }
	/** Monitoring Enabled */
	bool m_enableMon;
	/** TFile for monitoring */
	TFile * m_histFile;
	/** Histograms with monitoring activity */
	TH1F* m_EtSElectron;
	TH1F* m_numpt1;
	TH1F* m_numpt2;
	TH1F* m_numpt3;
	TH1F* m_numpt4;
	TH1F* m_denpt;
	TH1F* m_numeta1;
	TH1F* m_numeta2;
	TH1F* m_numeta3;
	TH1F* m_numeta4;
	TH1F* m_deneta;
	TH1F* m_numphi1;
	TH1F* m_numphi2;
	TH1F* m_numphi3;
	TH1F* m_numphi4;
	TH1F* m_denphi;
	TH1F* m_trackPt;
	TH1F* m_trackEta;
        bool m_doTruth;
        bool m_doOffline;
	std::string m_inputClusterName;
	std::string m_inputEratioName;
	std::string m_inputLvl1Name;
	float m_EEtThr;
	float m_rEtaThr;
	float m_EratioThr;

};

#endif
