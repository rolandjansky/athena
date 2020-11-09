/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * Name :	NoiseStudy.h
 * PACKAGE :	Trigger/TrigL1Upgrade/TrigL1CaloUpgrade/NoiseStudy
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
#include "CaloInterface/ICaloLumiBCIDTool.h"
#include "GaudiKernel/ToolHandle.h"
class CaloCellContainer;
class CaloCell;
class TFile;
class TH1I;
class TH1F;
class TH2F;
class TNtuple;
class CaloCell_SuperCell_ID;
class ICaloNoiseTool;


class NoiseStudy : public AthAlgorithm
{
public :

	// Usual athena stuff
	NoiseStudy( const std::string& name, ISvcLocator* pSvcLocator );
	virtual ~NoiseStudy();
	StatusCode initialize();
	StatusCode finalize();
	StatusCode execute();

private :

	/** For cell <-> SCell comparisons */
	ToolHandle<ICaloSuperCellIDTool> m_scidtool;
	ToolHandle<ICaloNoiseTool> m_noisetool;
	ToolHandle<ICaloLumiBCIDTool> m_caloLumiBCIDTool;
        const CaloCell_SuperCell_ID*     m_schelper;
        std::vector<CaloCell*> m_resimplescells ;
        std::vector<uint32_t> m_resimplescellsID ;
	std::vector<int> m_ncells;
	std::vector<int> m_ngains;
	TH1F* m_inconePt;
	TH2F* m_inconeEtaPhi;
	TH1F* m_outconePt;
	TH2F* m_outconeEtaPhi;
	TNtuple* m_ntuple;
	TNtuple* m_noisetuple;
	TFile * m_file;
        int counter;
	bool m_first;
	struct noise {
	  uint32_t id;
	  double elec_hnoise;
	  double elec_mnoise;
	  double elec_lnoise;
	  double pileup;
	  double total_hnoise;
	  double total_mnoise;
	  double total_lnoise;
	};
	std::vector<noise> m_noise;
	bool m_compNoise;
	bool m_addBCID;
        std::string m_inputSCellName;
        std::string m_inputBaseSCellName;

};

#endif
