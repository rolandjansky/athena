/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * Name :	TrigT1CaloforwEFex.h
 * PACKAGE :	Trigger/L1CaloUpgrade/TrigT1CaloforwEFex
 *
 * AUTHOR :	Denis Oliveira Damazio
 *
 * PURPOSE :	Emulate the forwEFex system, for phase 1, L1Calo upgrade
 *
 * **/

#ifndef TRIGT1CALOEFEX_TRIGT1CALOFORWEFEX
#define TRIGT1CALOEFEX_TRIGT1CALOFORWEFEX

#include "../src/TrigT1CaloBaseFex.h"

class TrigT1CaloforwEFex : public TrigT1CaloBaseFex
{
public :

	// Usual athena stuff
	TrigT1CaloforwEFex( const std::string& name, ISvcLocator* pSvcLocator );
	virtual ~TrigT1CaloforwEFex();
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
	TH2F* m_testEtavsR;
	TH1F* m_testR_IW;
	TH1F* m_testR_FCAL;
	TH2F* m_fmax_vs_eta;
	TH1F* m_fmax_IW;
	TH1F* m_fmax_FCAL;
        /** output name */
        std::string m_outputClusterName;
	/** cut for time measurement */
	float m_timeThr;

};

#endif
