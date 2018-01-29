/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_Raw2Digit_h
#define ALFA_Raw2Digit_h


#include "AthenaBaseComps/AthAlgorithm.h"


// for ALFA
#include "ALFA_RawEv/ALFA_RawData.h"
#include "ALFA_RawEv/ALFA_RawDataContainer.h"
#include "ALFA_RawEv/ALFA_RawDataCollection.h"

#include "ALFA_RawEv/ALFA_DigitCollection.h"
#include "ALFA_RawEv/ALFA_ODDigitCollection.h"

//#include "ALFA_Geometry/ALFA_constants.h"

#include "GaudiKernel/ITHistSvc.h"


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "TH1.h"
#include "TH2.h" 
#include "TGraph.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TROOT.h"
#include "TFile.h"
#include "TString.h"
#include "TStyle.h"
#include "TColor.h"
#include "TLine.h"
#include "TLatex.h"
#include "TPave.h"
#include "TPaletteAxis.h"
#include "TPaveText.h"



class ALFA_Raw2Digit:public AthAlgorithm
{
public:
	ALFA_Raw2Digit(const std::string& name, ISvcLocator* pSvcLocator);

	StatusCode initialize();
	StatusCode execute();
	StatusCode finalize();
//	StatusCode clear();

private:
	StatusCode recordCollection();
//	StatusCode fillDigitCollection();
	StatusCode recordODCollection();
//	StatusCode fillODDigitCollection();
	StatusCode mapping();
	// changed by Petr - 19.12.2012 -----------------------------------------------------
//	StatusCode ChanPlot(uint32_t potn, uint32_t MarChan, uint32_t &i, uint32_t &j);
	StatusCode ChanPlot(uint32_t pot_n, uint32_t iLayer, uint32_t MarChan, uint32_t &chan_i, uint32_t &chan_j);

//	a handle on Store Gate
//	StoreGateSvc*               m_storeGate;
//	ServiceHandle<StoreGateSvc> m_digitsStore;
//	ActiveStoreSvc*             m_activeSvc;

	std::string m_ALFA_RawDataCollectionKey;
//	ITHistSvc * m_histSvc;
//	TTree* m_tree;

	int m_nEvents;
//	std::vector<int>* m_MBId;
//	std::vector<int>* m_PMFId;
//	std::vector<int>* m_hit;

	int m_mbID;
	int m_pmfID;
	int m_fibID;

	int m_MAPMTChan;
	int m_MapChan;

	int m_pot;
	int m_side;
	int m_plate;
	int m_fiber;

	int m_ODpot;
	int m_ODside;
	int m_ODplate;
	int m_ODfiber;

	// changed by Petr - 19.12.2012 -----------------------------------------------------
//	int maroc2fiber[8][64];
//	int maroc2mapmt[8][64];
//	int mapmt2maroc[8][64];
	int m_maroc2fiber[8][20][64];
	int m_maroc2mapmt[8][20][64];
	int m_mapmt2maroc[8][20][64];

	int m_pmf2layer[8][23];
//	int layer2pmf[8][20];
   
	int m_OD_pmf2layer[8][4];
//	int OD_layer2pmf[8][3];
	int m_OD_pmf_maroc2fiber[8][4][64];
	int m_OD_pmf_maroc2side[8][4][64];
   
	int m_mb2det[8];

/*
	TGraph *MAPMT_activity[8];
	TH2D* MAPMT_activity_h[8][23];
	TH2I* PMFvsMAPMT[8];
	TH2I* PMFvsMAROC[8];
	TH2I* MapmtHit[8][23];
*/

	TH2I *m_All_Trigger;
	TH2D *m_All_Trigger_norm;

	TH1F* m_act_lay_h[8];
	TH1F* m_hit_lay_h[8];
	uint32_t m_lumi_block, m_lumi_block_old;
	uint16_t m_charge_1[8], m_charge_2[8];
	std::vector<bool> m_trigger_pattern;
	bool m_pot_b[8], m_layer_b[8][20], m_fiber_b[8][20][64], m_trigger_pattern_b[8][16];
	Int_t m_n_hits_lay[8][20];
	Int_t m_event_no;
	TTree* m_tree;
	TFile *m_rootOutput;
	TString m_rootOutputFileName;


	Int_t m_pmf_chan_hit_counter[8][23][64];
	Int_t m_hit_counter[8];
     
//   ServiceHandle<StoreGateSvc>   m_digitsStore; 
   
	ALFA_DigitCollection* m_digitCollection;
	ALFA_Digit*           m_digitObject;

	ALFA_ODDigitCollection* m_ODdigitCollection;
	ALFA_ODDigit*           m_ODdigitObject;

	std::string m_key_DigitCollection;
	std::string m_key_ODDigitCollection;

	std::ifstream m_inChan;
	std::ifstream m_inPMF;
	std::ifstream m_inOD;
	std::ifstream m_inDet;

	std::string m_mapname;
	std::stringstream m_s;
	std::string m_h_name1, m_h_name2, m_h_name3;
	std::string m_h_loc1, m_h_loc2, m_h_loc3;
	std::string m_CanName;
      
	int m_WordId_count;
	uint32_t m_chan_i, m_chan_j;

/* 
	TTree* m_tree;
	TFile *m_rootOutput;
	TString m_rootOutputFileName;
*/   

private:
	std::string m_strMeasuredDataType;

private:
	std::ofstream m_toFile;
};


#endif
