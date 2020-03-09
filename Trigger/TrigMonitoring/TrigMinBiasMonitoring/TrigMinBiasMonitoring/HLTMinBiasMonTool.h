/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "CaloIdentifier/TileTBID.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/ChainGroup.h"

#include "TrigHLTMonitoring/IHLTMonTool.h"

#include "ZdcIdentifier/ZdcID.h"

#include "xAODEventInfo/EventInfo.h"

//selection tool
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
//#include "InDetTrackSelectionTool/InDetTrackCut.h"

#include <string>
#include <vector>
#include <map>

#include "TH1I.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TEfficiency.h"
#include "TH2S.h"
#include "TProfile.h"

#define MBTS 		1
#define LUCID 		2
#define IDMINBIAS 	4
#define ZDC 		8
#define BCM 		16
#define HMT			32

class TileTBID; 

///Concrete monitoring tool derived from IHLTMonTool
class HLTMinBiasMonTool: public IHLTMonTool
{

public:
	HLTMinBiasMonTool(const std::string & type, const std::string & name, const IInterface* parent);

	virtual ~HLTMinBiasMonTool();
	
	StatusCode init(); // called by initialize
	StatusCode fill(); // called by fillHistograms
#ifdef ManagedMonitorToolBase_Uses_API_201401
	StatusCode proc();
	StatusCode book(); // called by bookHistograms
#else
	StatusCode book(bool newEventsBlock, bool newLumiBlock, bool newRun)
	StatusCode proc(bool endOfEventsBlock, bool endOfLumiBlock, bool endOfRun);
#endif
        //StatusCode procHistograms(bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun); // called by procHistograms

        /* -----------Left for future efficiency implementation----------- */
        
// efficiency, finalizing over all runs
//        StatusCode procHistograms(bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun);

	  //StatusCode proc( bool /*isEndOfEventsBlock*/, bool /*isEndOfLumiBlock*/,
	  //               bool /*isEndOfRun*/ );{ return StatusCode::SUCCESS; }


private:
	int m_numberOfEvents;
	void fixXaxis(TH1* h); //Adjusts the axis lables to avoid rebinning.
	int error_bit(bool a, bool b);

	MsgStream* m_log; //Message Stream
	// Histogram Service
	//ITHistSvc* m_histsvc;

	//Trigger Decision
	/*ToolHandle<Trig::ITrigDecisionTool> m_tdthandle;*/
	
	void bookHistogramsForItem(const std::string &item, unsigned histGroup);
	void fillHistogramsForItem(const std::string &item, unsigned histGroup);
	void fillPurityForItem(const std::string &item, const ToolHandle< InDet::IInDetTrackSelectionTool > &selTool, unsigned greaterThan = 1, const std::string& collectiveHistogramPostfix = "");
	unsigned howManyGoodTracks(const ToolHandle< InDet::IInDetTrackSelectionTool > &selTool, std::vector<double> *acceptedTracksPt = nullptr);
	void fillEfficiencyForItem(const std::string &item, unsigned goodTracks, bool isPassed, const std::string &collectiveHistogramPostfix = "");
	
	unsigned receiveIsPassedCondition(unsigned internalIsPassedCondition);
	
	StatusCode fillHLTMbtsInfo();
	StatusCode fillMbtsInfo(const std::string &item);
	StatusCode fillSpacePointInfo(const std::string &item);
	StatusCode fillTrackingInfo();
	StatusCode fillBCMInfo();
	StatusCode fillLUCIDInfo();
	StatusCode fillZDCInfo();
	
	//HMT INFO
	StatusCode fillHMTSpacePointsInfo();
	StatusCode fillHMTVertexCountsInfo();
	StatusCode fillHMTTrigVertexCollectionInfo();
	
	unsigned m_mbtsCountsSideA, m_mbtsCountsSideC, m_mbtsCountsBothSides;
	
	//Chain group names
	std::vector<std::string> m_trigItems;
	
	//RecoAlgorithms defined in SignatureDicts.py for MinBias
	std::vector<std::string> m_availableAlgs; // eg. ['mbts', 'sptrk', 'noalg', 'perf', 'hmt', 'idperf'] set in config file
	
	//Which histograms to fill in given alg
	std::vector< unsigned > m_histoTargets;   // eg. [MBTS + ZDC, IDMinBias, '', '', '' ] numerically set in config file
	
	//Which histograms have to be filled by given algorithm key = m_availableAlgs, items = m_histoTarget
	
	std::map< std::string, unsigned > m_algorithmsForChainClass;
	std::map< std::string, unsigned > m_effCutForChainClass;
	std::map< std::string, unsigned > m_numGoodTracksPerCutType;
	std::map< std::string, unsigned > m_isPassedCondtitionForChainClass;
	
	struct chainMapping{
		unsigned histoGroup;
		unsigned effCutIdx;
		unsigned isPassedCondition;
		std::string algorithm;
	};
	std::map< std::string, chainMapping > m_chainProperties;
	
	std::map< unsigned, std::string > m_pathForGroup;
	
	//collective histograms
	std::vector < std::string > m_collectiveHistogramsNames;
	std::vector < std::string > m_collectiveHistogramForAlgorithm; 
	//<algorithmName, collectiveHistorgramPostfix>
	std::map < std::string, std::string > m_collectiveHistogramPostfixForAlgorithm; 
	
	const TileTBID* m_tileTBID;
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

	std::string m_spContainerName;
	std::string m_trtContainerName;
	std::string m_tcContainerName;

	//Energy-time
	std::string m_t2mbtsContainerName;
	std::string m_mbtsContainerName;
	std::vector<std::string> m_moduleLabel; // array of module names
        
	//HMT
	std::string m_vcContainerName;
	std::string m_vcolContainerName;
	//std::string m_trigSpacePointCountsName;
	double m_timeOverThreshold_cut;
	//

	float m_energyCut; // Controls the simulated trigger decision
	float m_timeCut; // Controls the simulated trigger decision

	// ID L2
	int m_pixSpBarr;
	int m_pixSpECA;
	int m_pixSpECC;
	int m_sctSpBarr;
	int m_sctSpECA;
	int m_sctSpECC;

	/** Cut made on time overthreshold before any multiplicities are calculated for Pixel and SCT. */
	float m_timeOverThresholdCut;

	/** If set to false the trigger combination of Pixel and SCT is an OR. */
	bool m_triggerTypeAND;

	// Threshold settings for multiplicity requirements.
	// A trigger threshold can be disabled by setting it to be < 0

	/** Total number of pixel clusters required */
	float m_totalPixelClus_cut;

	/** Total number of sct space points required */
	float m_totalSctSp_cut;

	// ID EF
	/** Hypo selection criteria */
	float m_max_z0;
	float m_min_pt;

	int m_mbTracks;
	int m_totpix_spEF;
	int m_totsct_spEF;
	
	ToolHandle< InDet::IInDetTrackSelectionTool >  m_mbtsEffSelTool;
	ToolHandle< InDet::IInDetTrackSelectionTool >  m_sptrkEffSelTool;
	ToolHandle< InDet::IInDetTrackSelectionTool >  m_noalgEffSelTool;
	ToolHandle< InDet::IInDetTrackSelectionTool >  m_hmtperfEffSelTool;
	ToolHandle< InDet::IInDetTrackSelectionTool >  m_idperfEffSelTool;
	ToolHandle< InDet::IInDetTrackSelectionTool >  m_perfEffSelTool;
	ToolHandle< InDet::IInDetTrackSelectionTool >  m_hmtEffSelTool;
	std::vector< ToolHandle< InDet::IInDetTrackSelectionTool >* > m_effSelTool;
	
	ToolHandle< InDet::IInDetTrackSelectionTool >  m_purSelTool;
	
	std::string m_inDetTrackParticleContainerName;
	std::vector<unsigned> m_isPassedCondtitions;
	
	std::string m_refTrigItem;
};

#endif
