/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * Name :	SimpleLArDigitsChecks.h
 * PACKAGE :	Trigger/TrigL1Upgrade/TrigL1CaloUpgrade/SimpleLArDigitsChecks
 *
 * AUTHOR :	Denis Oliveira Damazio
 *
 * PURPOSE :	Many checks about SuperCell reconstruction
 *
 * **/

#ifndef TRIGT1CALOEFEX_SIMPLELARDIGITSCHECKS
#define TRIGT1CALOEFEX_SIMPLELARDIGITSCHECKS

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
class CaloCellContainer;
class CaloCell;
class LArDigit;
class TFile;
class TH1I;
class TH1F;
class TH2F;
class LArSuperCellCablingTool;


class SimpleLArDigitsChecks : public AthAlgorithm
{
public :

	// Usual athena stuff
	SimpleLArDigitsChecks( const std::string& name, ISvcLocator* pSvcLocator );
	virtual ~SimpleLArDigitsChecks();
	StatusCode initialize();
	StatusCode finalize();
	StatusCode execute();

private :

	/** For cell <-> SCell comparisons */
	ToolHandle<LArSuperCellCablingTool> m_cabling;
	/** base file */
	TFile* m_file;
	/** base histograms about super-cells */
	/** Now check by layer */
	std::vector<TH1I*> m_nSCells_perLayer;
	std::vector<TH1F*> m_EtSCells_perLayer;
	std::vector<TH2F*> m_SSamples_perLayer;
	std::vector<TH2F*> m_NorSSamples_perLayer;
        int counter;

};

#endif
