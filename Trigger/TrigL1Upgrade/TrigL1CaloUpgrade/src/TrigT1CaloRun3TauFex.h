/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * Name :	TrigT1CaloRun3TauFex.h
 * PACKAGE :	Trigger/L1CaloUpgrade/TrigT1CaloRun3TauFex
 *
 * AUTHOR :	Denis Oliveira Damazio
 *
 * PURPOSE :	Emulate the eFex system, for phase 1, L1Calo upgrade
 *
 * **/

//#ifndef TRIGT1CALOEFEX_TRIGT1CALOEFEX
//#define TRIGT1CALOEFEX_TRIGT1CALOEFEX
#ifndef TRIGT1CALOEFEX_TRIGT1CALORUN3TAUFEX
#define TRIGT1CALOEFEX_TRIGT1CALORUN3TAUFEX

#include "../src/TrigT1CaloBaseFex.h"

class TrigT1CaloRun3TauFex : public TrigT1CaloBaseFex
{
public :

	// Usual athena stuff
	TrigT1CaloRun3TauFex( const std::string& name, ISvcLocator* pSvcLocator );
	virtual ~TrigT1CaloRun3TauFex();
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
	TH2F* m_SupercellMapEM0;
	TH2F* m_SupercellMapEM1;
	TH2F* m_SupercellMapEM2;
	TH2F* m_SupercellMapEM3;
	TH2F* m_SupercellMapHAD;

	std::string m_scoreName;
	std::string m_newScoreName;
	SG::AuxElement::ConstAccessor<float>* acc_score;
	SG::AuxElement::Accessor<float>* acc_newScore;

	SG::AuxElement::Accessor<float>* acc_clusterET;
	SG::AuxElement::Accessor<float>* acc_clusterIso;

        /** output name */
        std::string m_outputClusterName;
	/** cut for time measurement */
	float m_timeThr;

};

#endif
