/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * Name :	CaloBandwidth.h
 * PACKAGE :	Trigger/TrigL1Upgrade/TrigL1CaloUpgradeAnalysis/CaloBandwidth
 *
 * AUTHOR :	Denis Oliveira Damazio
 *
 * PURPOSE :	Process LArCells for phase II studies
 *
 * **/

#ifndef TRIGT1CALOEFEXANALYSIS_CALOBANDWIDTH
#define TRIGT1CALOEFEXANALYSIS_CALOBANDWIDTH

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TH1F.h"
#include "TFile.h"
#include <vector>
#include <utility>
class CaloCellContainer;
class CaloCell;
class LArSuperCellCablingTool;
class ICalorimeterNoiseTool;


class CaloBandwidth : public AthAlgorithm
{
public :

	// Usual athena stuff
	CaloBandwidth( const std::string& name, ISvcLocator* pSvcLocator );
	virtual ~CaloBandwidth();
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
        std::string m_cellsName;

        ToolHandle< ICalorimeterNoiseTool > m_noiseTool;
        float  m_etInSigma;
        float  m_et;

	/** Monitoring histograms */
	std::vector<std::pair<float,float> > m_limits;
	std::vector<TH1F*> m_allLa1;
	std::vector<TH1F*> m_allLa1AboveThr;
	std::vector<TH1F*> m_allLa1Above1Sig;
	std::vector<TH1F*> m_allLa1Above2Sig;
	std::vector<TH1F*> m_allLa1Above3Sig;

};


#endif
