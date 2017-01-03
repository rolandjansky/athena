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

	// Usual athena stuff
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

};

#endif
