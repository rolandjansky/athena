/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * Name :	SimpleSuperCellChecks.h
 * PACKAGE :	Trigger/TrigL1Upgrade/TrigL1CaloUpgrade/SimpleSuperCellChecks
 *
 * AUTHOR :	Denis Oliveira Damazio
 *
 * PURPOSE :	Many checks about SuperCell reconstruction
 *
 * **/

#ifndef TRIGT1CALOEFEX_SIMPLESUPERCELLCHECKS
#define TRIGT1CALOEFEX_SIMPLESUPERCELLCHECKS

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "GaudiKernel/ToolHandle.h"
class CaloCellContainer;
class CaloCell;
class TFile;
class TH1I;
class TH1F;
class TH2F;


class SimpleSuperCellChecks : public AthAlgorithm
{
public :

	// Usual athena stuff
	SimpleSuperCellChecks( const std::string& name, ISvcLocator* pSvcLocator );
	virtual ~SimpleSuperCellChecks();
	StatusCode initialize();
	StatusCode finalize();
	StatusCode execute();

private :

	/** For cell <-> SCell comparisons */
	ToolHandle<ICaloSuperCellIDTool> m_scidtool;
	/** base file */
	TFile* m_file = nullptr;
	/** base histograms about super-cells */
	TH1I* m_nSCells = nullptr;
	TH1F* m_EtSCells = nullptr;
	TH1F* m_TimeSCells = nullptr;
	TH1F* m_QualitySCells = nullptr;
	TH1F* m_TimeSCellsAT = nullptr;
	TH1F* m_QualitySCellsAT = nullptr;
	TH1F* m_TimeResol = nullptr;
	TH1F* m_TimeResolAT = nullptr;
	TH2F* m_TimeResol_vs_pt = nullptr;
	TH2F* m_TimeResol_vs_time = nullptr;
	TH2F* m_TimeComp = nullptr;
	TH2F* m_TimeCompWide = nullptr;
	TH1F* m_TimeResolW = nullptr;
	TH1F* m_TimeResolWAT = nullptr;
	TH2F* m_TimeResolW_vs_pt = nullptr;
	TH2F* m_TimeResolW_vs_time = nullptr;
	TH2F* m_TimeCompW = nullptr;
	TH2F* m_TimeCompWWide = nullptr;
	TH1F* m_QualityNCells = nullptr;
	TH1F* m_PrimaryVtx = nullptr;
	/** Now check by layer */
	std::vector<TH1I*> m_nSCells_perLayer;
	std::vector<TH1F*> m_EtSCells_perLayer;
	std::vector<TH1F*> m_EtaSCells_perLayer;
	std::vector<TH1F*> m_PhiSCells_perLayer;
	TH1F* m_resolution = nullptr;
	TH2F* m_resol_vs_et = nullptr;
	TH2F* m_resol_vs_eta = nullptr;
	std::vector<TH1F*> m_resolution_perLayer;
	std::vector<TH1F*> m_resolutionMEt_perLayer;
	std::vector<TH1F*> m_resolutionBCID_perLayer;
	std::vector<TH2F*> m_resol_vs_et_perLayer;
	std::vector<TH2F*> m_resol_vs_eta_perLayer;
	std::vector<TH2F*> m_resol_vs_nvtx_perLayer;
	std::vector<TH1F*> m_TimeResolAT_perLayer;
        int m_counter = 0;

};

#endif
