/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file HLTMinbiasMonTool.h
 *   Class declaration for HLTMinbiasMonTool
 *   
 *                          
 *                          
 *    @ Regina Kwee aattt @cern.ch
 *    modified by Alexander Paramonov 
 *    modified by Edward Sarkisyan-Grinbaum (High Mult)
 *    modified by Edward Sarkisyan-Grinbaum (8 TeV Low-mu, Lucid)
 *    modified by Edward Sarkisyan-Grinbaum (pA run changes)
 *   
 *    modified by Pawel Jurgielewicz (total makeover and menu-aware monitoring application
                                      since 00-01-01)
 */

#ifndef HLTMINBIASMONTOOL_H
#define HLTMINBIASMONTOOL_H

#include "GaudiKernel/MsgStream.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "CaloIdentifier/TileID.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/ChainGroup.h"

#include "TrigHLTMonitoring/IHLTMonTool.h"

#include "ZdcIdentifier/ZdcID.h"

#include <string>
#include <vector>
#include <map>

#include "TH1I.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2S.h"
#include "TProfile.h"

#define MBTS 		1
#define LUCID 		2
#define IDMINBIAS 	4
#define ZDC 		8
#define BCM 	        16

///Concrete monitoring tool derived from IHLTMonTool
class HLTMinBiasMonTool: public IHLTMonTool
{

public:
	HLTMinBiasMonTool(const std::string & type, const std::string & name, const IInterface* parent);

	virtual ~HLTMinBiasMonTool();

	StatusCode initialize();
	StatusCode bookHistogramsRecurrent();
	StatusCode fillHistograms();
	
        StatusCode init(); // called by initialize
        StatusCode book(); // called by bookHistograms
        StatusCode fill(); // called by fillHistograms
        StatusCode proc(); // called by procHistograms

// efficiency, finalizing over all runs
//        StatusCode procHistograms(bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun);

	//  StatusCode proc( bool /*isEndOfEventsBlock*/, bool /*isEndOfLumiBlock*/,
	//                 bool /*isEndOfRun*/ ){ return StatusCode::SUCCESS; }


private:

        int m_numberOfEvents;

        void fixXaxis(TH1* h); //Adjusts the axis lables to avoid rebinning.
        int error_bit(bool a, bool b);

	MsgStream* m_log; //Message Stream
	StoreGateSvc* m_storeGate; //Store Gate
	StoreGateSvc* m_detStore; //Detector Store
	// Histogram Service
	//ITHistSvc* m_histsvc;

	//Trigger Decision
	ToolHandle<Trig::TrigDecisionTool> m_tdthandle;
	
//------------------------ERASE THIS...
	//The Chain Groups are initialized here.
	std::string m_EF_mbSpTrkName;// = m_tdthandle->getChainGroup("HLT_mb_perf_L1MBTS_1"/*"EF_mbSpTrk"*/);
	std::string m_EF_mbSpName;// = m_tdthandle->getChainGroup("EF_mbSp");
	//m_EF_mbMbts_1_eff = m_tdthandle->getChainGroup("EF_mbMbts_1_eff");
	std::string m_EF_mbMbts_2_effName;// = m_tdthandle->getChainGroup("EF_mbMbts_2_eff");
	//m_EF_mbZdc_eff = m_tdthandle->getChainGroup("EF_mbZdc_eff");
	std::string m_EF_mbRd1_effName;// = m_tdthandle->getChainGroup("EF_mbRd1_eff");

// 8 TeV Low-mu
	std::string m_EF_mbLucid_effName;// = m_tdthandle->getChainGroup("EF_mbLucid_eff");

	std::string m_EF_rd0_filled_NoAlgName;// = m_tdthandle->getChainGroup("EF_rd0_filled_NoAlg");        
	//m_EF_mbMbts_2_NoAlg = m_tdthandle->getChainGroup("EF_mbMbts_2_NoAlg");      

// pA 
// Rd1 used instead of rd0 above (8 TeV pp low mu)
// defined above 	m_EF_mbRd1_eff        
// used instead of mbMbts_2_NoAlg (8 TeV pp low mu)
	std::string m_EF_L1ItemStreamer_L1_MBTS_2Name;// = m_tdthandle->getChainGroup("EF_L1ItemStreamer_L1_MBTS_2");      
	std::string m_EF_mbZdc_effName;// = m_tdthandle->getChainGroup("EF_mbZdc_a_c_eff");
	//std::string m_EF_mbLucid_effName;// = m_tdthandle->getChainGroup("EF_mbLucid_eff");           

	//Heavy-Ion triggers
	std::string m_EF_mbMbts_1_1_effName;// = m_tdthandle->getChainGroup("EF_mbMbts_1_1_eff");
	std::string m_EF_mbMbts_2_2_effName;// = m_tdthandle->getChainGroup("EF_mbMbts_2_2_eff");
	std::string m_EF_mbMbts_4_4_effName;// = m_tdthandle->getChainGroup("EF_mbMbts_4_4_eff");

	//High Multiplicity
	std::string m_EF_mbSpTrkVtxMhName;// = m_tdthandle->getChainGroup("EF_mbSpTrkVtxMh");
	std::string m_EF_mbSpTrkVtxMh_effName;// = m_tdthandle->getChainGroup("EF_mbSpTrkVtxMh_eff");

// Heavy-Ion proton-lead hip_trk (pA 2013)
	std::string m_EF_mbSpTrkVtxMh_hip_trk150_L1TE50Name;// = m_tdthandle->getChainGroup("EF_mbSpTrkVtxMh_hip_trk150_L1TE50");
	std::string m_EF_mbSpTrkVtxMh_hip_trk190_L1TE50Name;// = m_tdthandle->getChainGroup("EF_mbSpTrkVtxMh_hip_trk190_L1TE50");
	std::string m_EF_mbSpTrkVtxMh_hip_trk120_L1TE35Name;// = m_tdthandle->getChainGroup("EF_mbSpTrkVtxMh_hip_trk120_L1TE35");
	std::string m_EF_mbSpTrkVtxMh_hip_trk190_L1TE35Name;// = m_tdthandle->getChainGroup("EF_mbSpTrkVtxMh_hip_trk190_L1TE35");
	std::string m_EF_mbSpTrkVtxMh_hip_trk240_L1TE35Name;// = m_tdthandle->getChainGroup("EF_mbSpTrkVtxMh_hip_trk240_L1TE35");
	std::string m_EF_mbSpTrkVtxMh_hip_trk180_L1TE65Name;// = m_tdthandle->getChainGroup("EF_mbSpTrkVtxMh_hip_trk180_L1TE65");
	std::string m_EF_mbSpTrkVtxMh_hip_trk200_L1TE65Name;// = m_tdthandle->getChainGroup("EF_mbSpTrkVtxMh_hip_trk200_L1TE65");
	std::string m_EF_mbSpTrkVtxMh_hip_trk240_L1TE65Name;// = m_tdthandle->getChainGroup("EF_mbSpTrkVtxMh_hip_trk240_L1TE65");
	std::string m_EF_mbSpTrkVtxMh_hip_trk175_L1MBTSName;// = m_tdthandle->getChainGroup("EF_mbSpTrkVtxMh_hip_trk175_L1MBTS");

	std::string m_EF_L1ZDC_A_NoAlgName;// = m_tdthandle->getChainGroup("EF_L1ZDC_A_NoAlg");
	std::string m_EF_L1ZDC_C_NoAlgName;// = m_tdthandle->getChainGroup("EF_L1ZDC_C_NoAlg");
	std::string m_EF_L1LHCF_NoAlgName;// = m_tdthandle->getChainGroup("EF_L1LHCF_NoAlg");
	
	
	std::string m_name;
	
	void bookHistogramsForItem(const std::string &item, unsigned histGroup);
	void fillHistogramsForItem(const std::string &item, unsigned histGroup);
	
	StatusCode fillHLTMbtsInfo();
	StatusCode fillMbtsInfo(const std::string &item);
	StatusCode fillSpacePointInfo(const std::string &item);
	StatusCode fillTrackingInfo();
	StatusCode fillBCMInfo();
	StatusCode fillLUCIDInfo();
	StatusCode fillZDCInfo();
	
	unsigned m_mbtsCountsSideA, m_mbtsCountsSideC, m_mbtsCountsBothSides;
	
	//std::map<std::string, std::vector<void*>>;
	
	//Chain group names
	std::vector<std::string> m_trigItems;
	
	//RecoAlgorithms defined in SignatureDicts.py for MinBias
	std::vector<std::string> m_availableAlgs; // eg. ['mbts', 'sptrk', 'noalg', 'perf', 'hmt', 'idperf'] set in config file
	
	//Which histograms to fill in given alg
	std::vector< unsigned > m_histoTargets;   // eg. [MBTS + ZDC, IDMinBias, '', '', '' ] numerically set in config file
	
	//Which histograms have to be filled by given algorithm key = m_availableAlgs, items = m_histoTarget
	std::map< std::string, unsigned > m_algorithmsForChain;
	
	std::map< std::string, unsigned > m_histoGroupForChain;
	
	std::map< unsigned, std::string > m_pathForGroup;
	
	const TileID* m_tileID;
	const ZdcID *m_ZdcID;
	
	//BCM
	std::string m_bcmContainerName;
	//Trigger Setitngs
	int m_OOTPulse;
	int m_ITPulse;
	int m_WITPulse;
	int m_TimeWindow;
	int m_WideTimeWindow;
	int m_rdoIndex;
	
	//LUCID
	std::string m_LUCID_RawDataContainerName;
	
	//ZDC
	std::string m_ZdcContainerName;
	std::string m_CaloCellContainerName;

//-------------------------

	//Trigger ChainGroups
	// regular, long-term, chains
	const Trig::ChainGroup* m_EF_mbSpTrk;
	//const Trig::ChainGroup* m_EF_mbMbts_1_eff;
	const Trig::ChainGroup* m_EF_mbMbts_2_eff;
	const Trig::ChainGroup* m_EF_mbMbts_1_1_eff;
	const Trig::ChainGroup* m_EF_mbMbts_2_2_eff;
	const Trig::ChainGroup* m_EF_mbMbts_4_4_eff;
	const Trig::ChainGroup* m_EF_mbZdc_eff;
	const Trig::ChainGroup* m_EF_mbRd1_eff;

// 8 TeV Low-mu
	const Trig::ChainGroup* m_EF_mbLucid_eff;
// rd0 back in pA Jan 2013
	const Trig::ChainGroup* m_EF_rd0_filled_NoAlg;
	//const Trig::ChainGroup* m_EF_mbMbts_2_NoAlg;
// pA 
	const Trig::ChainGroup* m_EF_L1ItemStreamer_L1_MBTS_2;
	const Trig::ChainGroup* m_EF_mbSp;

	std::string m_spContainerName;
	std::string m_trtContainerName;
	std::string m_tcContainerName;

//Energy-time
        std::string m_t2mbtsContainerName;
        std::string m_mbtsContainerName;
        std::vector<std::string> m_moduleLabel; // array of module names

        float m_energyCut; // Controls the simulated trigger decision
        float m_timeCut; // Controls the simulated trigger decision

	// High Mult
	const Trig::ChainGroup* m_EF_mbSpTrkVtxMh;
	const Trig::ChainGroup* m_EF_mbSpTrkVtxMh_eff;


	// hip_trk trigs. pA Jan 2013
	const Trig::ChainGroup* m_EF_mbSpTrkVtxMh_hip_trk150_L1TE50; 	
	const Trig::ChainGroup* m_EF_mbSpTrkVtxMh_hip_trk190_L1TE50;
	const Trig::ChainGroup* m_EF_mbSpTrkVtxMh_hip_trk120_L1TE35; 	
	const Trig::ChainGroup* m_EF_mbSpTrkVtxMh_hip_trk190_L1TE35; 	
 	const Trig::ChainGroup* m_EF_mbSpTrkVtxMh_hip_trk240_L1TE35; 	
 	const Trig::ChainGroup* m_EF_mbSpTrkVtxMh_hip_trk240_L1TE65; 	
 	const Trig::ChainGroup* m_EF_mbSpTrkVtxMh_hip_trk200_L1TE65; 	
 	const Trig::ChainGroup* m_EF_mbSpTrkVtxMh_hip_trk180_L1TE65; 	
        const Trig::ChainGroup* m_EF_mbSpTrkVtxMh_hip_trk175_L1MBTS;

        const Trig::ChainGroup* m_EF_L1ZDC_A_NoAlg;
        const Trig::ChainGroup* m_EF_L1ZDC_C_NoAlg;
        const Trig::ChainGroup* m_EF_L1LHCF_NoAlg;


	// ID L2
	int pixSpBarr;
	int pixSpECA;
	int pixSpECC;
	int sctSpBarr;
	int sctSpECA;
	int sctSpECC;

	/** Cut made on time overthreshold before any multiplicities are calculated for Pixel and SCT. */
	float m_timeOverThresholdCut;

	/** If set to false the trigger combination of Pixel and SCT is an OR. */
	bool m_triggerTypeAND;

	// Threshold settings for multiplicity requirements.
	// A trigger threshold can be disabled by setting it to be < 0

	/** Total number of pixel clusters required */
	float m_totalPixelClus_cut;

	/** Pixel cluster ratio A required */
	float m_pixelClusRatioA_cut;

	/** Pixel cluster ratio B required */
	float m_pixelClusRatioB_cut;

	/** Total number of pixel clusters required in endcap C */
	float m_pixelClusEndcapC_cut;

	/** Total number of pixel clusters required in the barrel */
	float m_pixelClusBarrel_cut;

	/** Total number of pixel clusters required in endcap A */
	float m_pixelClusEndcapA_cut;

	/** Maximum number of pixel clusters allowed in endcap C */
	float m_pixelClusEndcapC_max;

	/** Maximum number of pixel clusters allowed in the barrel */
	float m_pixelClusBarrel_max;

	/** Maximum number of pixel clusters allowed in endcap A */
	float m_pixelClusEndcapA_max;

	/** Minimum number of pixel clusters required in one of the endcaps */
	float m_pixelClusEndcaps_cut;

	/** Total number of sct space points required */
	float m_totalSctSp_cut;

	/** Total number of sct space points required in endcap C */
	float m_sctSpEndcapC_cut;

	/** Total number of sct space points required in the barrel */
	float m_sctSpBarrel_cut;

	/** Total number of sct space points required in endcap A */
	float m_sctSpEndcapA_cut;

	/** Maximum number of sct space points allowed in endcap C */
	float m_sctSpEndcapC_max;

	/** Maximum number of sct space points allowed in the barrel */
	float m_sctSpBarrel_max;

	/** Maximum number of sct space points allowed in endcap A */
	float m_sctSpEndcapA_max;

	/** Minimum number of sct space points required in one of the endcaps */
	float m_sctSpEndcaps_cut;

	/** detector mask flags */
	bool m_sct_barrel_a_side;
	bool m_sct_barrel_c_side;
	bool m_sct_endcap_a_side;
	bool m_sct_endcap_c_side;
	bool m_pixel_barrel, m_pixel_b_layer, m_pixel_disk;

	float m_maxTrtHitsEC;
	float m_totalTrtHits_cut;

	// ID EF
	/** Hypo selection criteria */
	float m_max_z0;
	float m_min_pt;
	int m_required_ntrks;

	int mbTracks;
	int totpix_spEF;
	int totsct_spEF;

	/** Threshold of sct space points required for hip_trk, pA 2013*/
	float m_sctSp_thresh;


	TH1I* m_pixBarr_sp;
	TH1I* m_pixECA_sp;
	TH1I* m_pixECC_sp;
	TH1I* m_sctBarr_sp;
	TH1I* m_sctECA_sp;
	TH1I* m_sctECC_sp;
	TH1I* m_minbiasTracks;

	//L2 trigger decisions
	//TH1D* h_mbSpTrk_pixTot_P;
	//TH1D* h_mbSpTrk_pixTot_F;
	//TH1D* h_mbSpTrk_sctTot_P;
	//TH1D* h_mbSpTrk_sctTot_F;
        // pA
	TH1D* h_mbSp_pixTot_P;
	TH1D* h_mbSp_pixTot_F;
	TH1D* h_mbSp_sctTot_P;
	TH1D* h_mbSp_sctTot_F;


// Efficiencies 
//	TH1D* h_mbSpTrk_pixTot_Eff;
//	TH1D* h_mbSpTrk_sctTot_Eff;

/*
// 8 TeV Low-mu 
	TH1D* h_mbSpTrk_pixTot_mb2Alg_P;
	TH1D* h_mbSpTrk_pixTot_mb2Alg_F;
	TH1D* h_mbSpTrk_sctTot_mb2Alg_P;
	TH1D* h_mbSpTrk_sctTot_mb2Alg_F;
*/
	TH1D* h_mbSpTrk_pixTot_rd0_P;
	TH1D* h_mbSpTrk_pixTot_rd0_F;
	TH1D* h_mbSpTrk_sctTot_rd0_P;
	TH1D* h_mbSpTrk_sctTot_rd0_F;
/*
	TH1D* h_mbSpTrk_trkTot_mb2Alg_P;
	TH1D* h_mbSpTrk_trkTot_mb2Alg_F;
*/
	TH1D* h_mbSpTrk_trkTot_rd0_P;
	TH1D* h_mbSpTrk_trkTot_rd0_F;



// pA 
	TH1D* h_mbSp_pixTot_mb2_P;
	TH1D* h_mbSp_pixTot_mb2_F;
	TH1D* h_mbSp_sctTot_mb2_P;
	TH1D* h_mbSp_sctTot_mb2_F;

	TH1D* h_mbSp_pixTot_Rd1_P;
	TH1D* h_mbSp_pixTot_Rd1_F;
	TH1D* h_mbSp_sctTot_Rd1_P;
	TH1D* h_mbSp_sctTot_Rd1_F;

	TH1D* h_mbSp_trkTot_mb2_P;
	TH1D* h_mbSp_trkTot_mb2_F;
	TH1D* h_mbSp_trkTot_Rd1_P;
	TH1D* h_mbSp_trkTot_Rd1_F;
//
/*
	// L2_mbMbts_1_eff
	TH1I* h_mbMbts_1_eff_pixTot_P;
	//	TH1I* h_mbMbts_1_eff_pixTot_F;
	TH1I* h_mbMbts_1_eff_sctTot_P;
	//	TH1I* h_mbMbts_1_eff_sctTot_F;
*/
	// L2_mbMbts_2_eff
	TH1I* h_mbMbts_2_eff_pixTot_P;
	//	TH1I* h_mbMbts_2_eff_pixTot_F;
	TH1I* h_mbMbts_2_eff_sctTot_P;
	//	TH1I* h_mbMbts_2_eff_sctTot_F;

//8 TeV Low mu Lucid
	// L2_mbLucid_eff
	TH1I* h_mbLucid_eff_pixTot_P;
	//	TH1I* h_mbLucid_eff_pixTot_F;
	TH1I* h_mbLucid_eff_sctTot_P;
	//	TH1I* h_mbLucid_eff_sctTot_F;


	// L2_mbZdc_eff
	TH1I* h_mbZdc_eff_pixTot_P;
	//	TH1I* h_mbZdc_eff_pixTot_F;
	TH1I* h_mbZdc_eff_sctTot_P;
	//	TH1I* h_mbZdc_eff_sctTot_F;

	// L2_mbRd1_eff
	TH1I* h_mbRd1_eff_pixTot_P;
	//	TH1I* h_mbRd1_eff_pixTot_F;
	TH1I* h_mbRd1_eff_sctTot_P;
	//	TH1I* h_mbRd1_eff_sctTot_F;

	// L2 High Mult
	TH1I* h_pix_mbSpTrkVtxMh_P;
	TH1I* h_pix_mbSpTrkVtxMh_F;
	TH1I* h_sct_mbSpTrkVtxMh_P;
	TH1I* h_sct_mbSpTrkVtxMh_F;

	TH1I* h_pix_mbSpTrkVtxMh_eff_P;
	TH1I* h_pix_mbSpTrkVtxMh_eff_F;
	TH1I* h_sct_mbSpTrkVtxMh_eff_P;
	TH1I* h_sct_mbSpTrkVtxMh_eff_F;

	//EF decisions
	//TH1D* h_mbSpTrk_trkTot_P;
	//TH1D* h_mbSpTrk_trkTot_F;
// pA
	TH1D* h_mbSp_trkTot_P;
	TH1D* h_mbSp_trkTot_F;

//   Efficiencies
//        TH1D* h_mbSpTrk_trkTot_Eff;


/*
	//EF_mbMbts_1_eff
	TH1I* h_mbMbts_1_eff_trkTot_P;
	//TH1I* h_mbMbts_1_eff_trkTot_F;
*/
	//EF_mbMbts_2_eff
	TH1I* h_mbMbts_2_eff_trkTot_P;
	TH1I* h_mbMbts_2_eff_trkTot_F;

//8 TeV Low-mu Lucid
	//EF_mbLucid_eff
	TH1I* h_mbLucid_eff_trkTot_P;
	//TH1I* h_mbLucid_eff_trkTot_F;

	//EF_mbZdc_eff
	TH1I* h_mbZdc_eff_trkTot_P;
	TH1I* h_mbZdc_eff_trkTot_F;

	//EF_mbRd1_eff
	TH1I* h_mbRd1_eff_trkTot_P;
	//TH1I* h_mbRd1_eff_trkTot_F;

	// EF High Mult
	TH1I* h_EF_mbSpTrkVtxMh_P;
	TH1I* h_EF_mbSpTrkVtxMh_F;
	TH1I* h_EF_mbSpTrkVtxMh_eff_P;
	TH1I* h_EF_mbSpTrkVtxMh_eff_F;
 
        // pA Jan 2013
	//EF_L1ZDC_A/C_NoAlg, LHCF
	TH1I* h_L1ZDC_A_NoAlg_trkToT_P;
	TH1I* h_L1ZDC_A_NoAlg_trkToT_F;
	TH1I* h_L1ZDC_C_NoAlg_trkToT_P;
	TH1I* h_L1ZDC_C_NoAlg_trkToT_F;
	TH1I* h_L1LHCF_NoAlg_trkToT_P;
	TH1I* h_L1LHCF_NoAlg_trkToT_F;

	TProfile* m_h_TriggerErrors;

// Energy-Time
       // cell occupancy
       TH1I* m_h_occupancy_onl_A; // The simulated trigger rate on the cell-by-cell basis (does not include the time cut-off)
       TH1I* m_h_occupancy_onl_C; // The simulated trigger rate on the cell-by-cell basis (does not include the time cut-off)

//online, and L2 & EF passed / failed
       TH1D* m_h_energy_1d; //1dim Distribution of Energy in a cell (does not include the time cut-off)
       TH1D* m_h_time_1d; //1dim Distribution of Time in a cell (includes the 40/222 cut-off on energy)
       TH1D* m_h_time_L2_P;
        TH1F* m_h_time_L2_F;
        TH1F* m_h_time_EF_P;
        TH1F* m_h_time_EF_F;
       TH1D* m_h_energy_L2_P;
        TH1F* m_h_energy_L2_F;
        TH1F* m_h_energy_EF_P;
        TH1F* m_h_energy_EF_F;

// L2 timing efficiency
//        TH1D* m_h_time_L2_Eff;

// L2 energy eff
        TH1D* m_h_energy_L2_Eff;
                

       // A side
       TH1F* m_h_energyMean_A_P; //Distribution of averaged Energy in a cell  (does not include the time cut-off)
       TH1F* m_h_energyMax_A_P; // Distribution of maximum energy passed the trigger ...
       TH1F* m_h_timeMean_A_P; //Distribution of averaged Time in a cell  (includes the 40/222 cut-off on energy)
       TH1F* m_h_timeMin_A_P; // Distribution of minimum time passed the trigger ...
       TH1F* m_h_energyMean_A_F; //Distribution of averaged Energy in a cell  (does not include the time cut-off)
       TH1F* m_h_energyMax_A_F; // Distribution of maximum energy failed ...
       TH1F* m_h_timeMean_A_F; //Distribution of averaged Time in a cell  (includes the 40/222 cut-off on energy)
       TH1F* m_h_timeMin_A_F; // Distribution of minimum time failed ...
      // C side
       TH1F* m_h_energyMean_C_P; //Distribution of averaged Energy in a cell  (does not include the time cut-off)
       TH1F* m_h_energyMax_C_P; // Distribution of maximum energy passed the trigger ...
       TH1F* m_h_timeMean_C_P; //Distribution of averaged Time in a cell  (includes the 40/222 cut-off on energy)
       TH1F* m_h_timeMin_C_P; // Distribution of minimum time passed the trigger ...
       TH1F* m_h_energyMean_C_F; //Distribution of averaged Energy in a cell  (does not include the time cut-off)
       TH1F* m_h_energyMax_C_F; // Distribution of maximum energy failed ...
       TH1F* m_h_timeMean_C_F; //Distribution of averaged Time in a cell  (includes the 40/222 cut-off on energy)
       TH1F* m_h_timeMin_C_F; // Distribution of minimum time failed ...

// hip_trk with Sp thresh ; pA 2013
       TH1F* m_h_Trig_hip_trk_P;
       TH1F* m_h_Trig_hip_trk_F;
       
};

#endif
