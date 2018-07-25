/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * Name :	LArFex.h
 * PACKAGE :	Trigger/TrigL1Upgrade/TrigL1CaloUpgrade/LArFex
 *
 * AUTHOR :	Denis Oliveira Damazio
 *
 * PURPOSE :	Process LArCells for phase II studies
 *
 * **/

#ifndef TRIGT1CALOEFEX_LARFEX
#define TRIGT1CALOEFEX_LARFEX

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TH1F.h"
#include "TFile.h"
class CaloCellContainer;
class CaloCell;
class LArSuperCellCablingTool;
class ICalorimeterNoiseTool;


class LArFex : public AthAlgorithm
{
public :

	// Usual athena stuff
	LArFex( const std::string& name, ISvcLocator* pSvcLocator );
	virtual ~LArFex();
	StatusCode initialize();
	StatusCode finalize();
	StatusCode execute();

private :

	/** For cell <-> SCell comparisons */
	ToolHandle<LArSuperCellCablingTool> m_cabling;
	/** base file */
	TFile* m_file;
	/** event counter */
	int m_counter;
	/** some keys */
	std::string m_inputClusterName;
        std::string m_inputLvl1Name;
	std::string m_outputClusterName;
        std::string m_cellsName;

        ToolHandle< ICalorimeterNoiseTool > m_noiseTool;
        float  m_etInSigma;
        float  m_et;
        float  m_etInSigmaS;
        float  m_etS;

	/** Monitoring histograms */
	bool m_enableMon;
        TH1F* m_allLa1;
	TH1F* m_allLa1AboveThr;
	TH1F* m_allLa1Above1Sig;
	TH1F* m_allLa1Above2Sig;
	TH1F* m_allLa1Above3Sig;
	TH1F* m_Eratio;
	TH1F* m_frac73;
	TH1F* m_wStot;

};


#endif
