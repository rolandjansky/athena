/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * Name :	DumpAll.h
 * PACKAGE :	Trigger/TrigL1Upgrade/TrigL1CaloUpgrade/DumpAll
 *
 * AUTHOR :	Denis Oliveira Damazio
 *
 * PURPOSE :	Dump All info in ntuples for optimization
 *
 * **/

#ifndef TRIGT1CALOEFEX_DUMPALL
#define TRIGT1CALOEFEX_DUMPALL

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
class CaloCellContainer;
class CaloCell;
class LArDigit;
class TFile;
class TH1I;
class TH1F;
class TH2F;
class TTree;


class DumpAll : public AthAlgorithm
{
public :

	// Usual athena stuff
	DumpAll( const std::string& name, ISvcLocator* pSvcLocator );
	virtual ~DumpAll();
	StatusCode initialize();
	StatusCode finalize();
	StatusCode execute();
	void ResetAllBranches();

private :

	/** base file */
	TFile* m_file;
	/** base histograms about super-cells */
        TTree* m_evt;
        TTree* m_selectron;
	TTree* m_truth;
	TTree* m_l1;
	TTree* m_offelectron;
	TTree* m_cell_infront;
	TTree* m_allcell_infront;
	/** event counter */
	int m_counter;
	/** some keys */
	std::string m_inputClusterName;
        std::string m_inputLvl1Name;

	/** branch address */
	std::vector<float> m_selectron_et;
	std::vector<float> m_selectron_eta;
	std::vector<float> m_selectron_phi;
	std::vector<float> m_selectron_energy;
	std::vector<float> m_selectron_e237;
	std::vector<float> m_selectron_e277;
	std::vector<float> m_selectron_ehad1;
	std::vector<float> m_selectron_e0;
	std::vector<float> m_selectron_e1;
	std::vector<float> m_selectron_e2;
	std::vector<float> m_selectron_e3;
	std::vector<float> m_selectron_wstot;
	std::vector<float> m_selectron_time;

	std::vector<float> m_truth_pt;
	std::vector<float> m_truth_eta;
	std::vector<float> m_truth_phi;
	std::vector<float> m_truth_pdg;
	std::vector<float> m_truth_barcode;
	std::vector<float> m_truth_charge;

	std::vector<float> m_l1_et;
	std::vector<float> m_l1_eta;
	std::vector<float> m_l1_phi;
	std::vector<float> m_l1_emclus;
	std::vector<float> m_l1_hadcore;
	std::vector<float> m_l1_hadisol;
	std::vector<float> m_l1_emisol;

	std::vector<float> m_offel_et;
	std::vector<float> m_offel_eta;
	std::vector<float> m_offel_phi;
	std::vector<float> m_offel_cleta;
	std::vector<float> m_offel_clphi;
	std::vector<float> m_offel_reta;
	std::vector<float> m_offel_f1;
	std::vector<float> m_offel_f3;
	std::vector<float> m_offel_had;
	std::vector<float> m_offel_istight;
	std::vector<float> m_offel_ismedium;
	std::vector<float> m_offel_isloose;

	std::vector< std::vector<float> > m_cell_infront_e;
	std::vector< std::vector<float> > m_cell_infront_eta;
	std::vector< std::vector<float> > m_cell_infront_phi;
	std::vector< std::vector<float> > m_cell_infront_layer;
	std::vector< std::vector<float> > m_cell_infront_time;
	std::vector< std::vector<float> > m_cell_infront_quality;

	std::vector< std::vector<float> > m_allcell_infront_idx;
	std::vector< std::vector<float> > m_allcell_infront_e;
	std::vector< std::vector<float> > m_allcell_infront_eta;
	std::vector< std::vector<float> > m_allcell_infront_phi;
	std::vector< std::vector<float> > m_allcell_infront_layer;
	std::vector< std::vector<float> > m_allcell_infront_time;
	std::vector< std::vector<float> > m_allcell_infront_quality;
};


#endif
