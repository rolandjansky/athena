/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file HLTMinBiasMonTool.cxx
 *   
 *    
 *    DON'T DELETE THE COMMENTS                   
 *                          
 *    @author Regina Kwee
 *    BCM modifications and updates : eo - at - hep.ucl.ac.uk
 *    updated by Alexander Paramonov
 *    updated by Edward Sarkisyan-Grinbaum (High Mult,Energy-Timing,Eff)
 *    (8 TeV Low-mu, Lucid)
 *    (pA 2012 pilot run changes:
 *    mbSpTrk->mbSp,rd0_filled_NoAlg->mbRd1_eff, mbZdc, mbLucid, histos)
 *    (pA run Jan 2013:
 *    mbSpTrk || mbSp,rd0_filled_NoAlg in, and a lot of pA hip_trk trigs,
 *    Lucid_eff, mbMbts_1_eff->mbMbts_2_eff, EF_L1LHCF_NoAlg, EF_L1ZDC_A/C_NoAlg)
 *    sedward - at - cern.ch
 *    Edward Sarkisyan-Grinbaum (moved to unmanaged histo booking:
 *    https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/
 *       LS1UpgradeAthenaMonFramework)
 *    sedward - at - cern.ch
 *
 *    Based on HLTMonTool example by Christiane Risler and Martin zur Nedden
 */
#include "GaudiKernel/IJobOptionsSvc.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "xAODTrigMinBias/TrigSpacePointCountsContainer.h"
#include "xAODTrigMinBias/TrigSpacePointCounts.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsContainer.h"
#include "xAODTrigMinBias/TrigT2MbtsBits.h"
#include "xAODTrigMinBias/TrigVertexCountsContainer.h"
#include "xAODTrigMinBias/TrigVertexCounts.h"
#include "xAODTrigMinBias/TrigTrackCountsContainer.h"
#include "xAODTrigMinBias/TrigTrackCounts.h"

#include "TileEvent/TileContainer.h"
#include "TileEvent/TileCell.h"
#include "TileEvent/TileDigitsContainer.h"

#include "InDetBCM_RawData/BCM_RDO_Container.h"

#include "LUCID_RawEvent/LUCID_RawDataContainer.h"

#include "ZdcEvent/ZdcDigitsCollection.h"
#include "ZdcEvent/ZdcRawChannelCollection.h"
#include "ZdcEvent/ZdcRawChannel.h"
#include "ZdcEvent/ZdcRawData.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloSamplingHelper.h"
#include "CaloGeoHelpers/CaloSampling.h"

// Energy-Time 
#include "TrigCaloEvent/TrigT2MbtsBitsContainer.h"

#include "TrigMinBiasMonitoring/HLTMinBiasMonTool.h" 

#include "TROOT.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TH2F.h"
#include "TH1D.h"

#include <bitset>


using namespace std;

/** Constructor, calls base class constructor with parameters
 *
 *  several properties are "declared" here, allowing selection
 *  of the filepath for histograms, the first and second plane
 *  numbers to be used, and the timebin.
 */
HLTMinBiasMonTool::HLTMinBiasMonTool(const std::string & type, const std::string & myname, const IInterface* parent) :
	IHLTMonTool(type, myname, parent), m_log(0), m_tdthandle("Trig::TrigDecisionTool/monTrigDecTool")
{
	m_log = new MsgStream(msgSvc(), name());

	//Lvl-2 containers
	declareProperty("SPContainerName", m_spContainerName);

	// Cut made on time over-threshold before any multiplicities are calculated.
	declareProperty("PixelCLToTCut", m_timeOverThresholdCut = 20.);

	// If set to false the trigger combination of Pixel and SCT is an OR.
	declareProperty("TriggerTypeAND", m_triggerTypeAND = true);

	// Threshold settings for multiplicity requirements.
	// A trigger threshold can be disabled by setting it to be < 0
	declareProperty("TotalPixelClus", m_totalPixelClus_cut = 3.);
	declareProperty("PixelClusRatioA", m_pixelClusRatioA_cut = -1.);
	declareProperty("PixelClusRatioB", m_pixelClusRatioB_cut = -1.);
	declareProperty("PixelClusEndcapC", m_pixelClusEndcapC_cut = -1.);
	declareProperty("PixelClusBarrel", m_pixelClusBarrel_cut = -1.);
	declareProperty("PixelClusEndcapA", m_pixelClusEndcapA_cut = -1.);
	declareProperty("PixelClusEndcapC_max", m_pixelClusEndcapC_max = -1.);
	declareProperty("PixelClusBarrel_max", m_pixelClusBarrel_max = -1.);
	declareProperty("PixelClusEndcapA_max", m_pixelClusEndcapA_max = -1.);
	declareProperty("PixelClusEndcaps_cut", m_pixelClusEndcaps_cut = -1.);

	declareProperty("TotalSctSp", m_totalSctSp_cut = 3.);
	declareProperty("SctSpEndcapC", m_sctSpEndcapC_cut = -1.);
	declareProperty("SctSpBarrel", m_sctSpBarrel_cut = -1.);
	declareProperty("SctSpEndcapA", m_sctSpEndcapA_cut = -1.);
	declareProperty("SctSpEndcapC_max", m_sctSpEndcapC_max = -1.);
	declareProperty("SctSpBarrel_max", m_sctSpBarrel_max = -1.);
	declareProperty("SctSpEndcapA_max", m_sctSpEndcapA_max = -1.);
	declareProperty("SctSpEndcaps_cut", m_sctSpEndcaps_cut = -1.);

	//EF container
	declareProperty("TCContainerName", m_tcContainerName);

	declareProperty("max_z0", m_max_z0 = 401.0); // (mm)
	declareProperty("min_pt", m_min_pt = 0.2); // (GeV)
	declareProperty("Required_ntrks", m_required_ntrks = 1); // (#)

	// hip_trk Sp threshold - pA 2013
	declareProperty("SctSpThresh", m_sctSp_thresh = 20.);
	
	// Parameter to be tuned to correspond to trigger threshold
        declareProperty("TimeCut", m_timeCut = 20.0);
        declareProperty("EnergyCut", m_energyCut = 40. / 222.); // It should be 60./222.
	
	//ERASE THIS...
	declareProperty("EF_mbSpTrk",m_EF_mbSpTrkName);
	declareProperty("EF_mbSp",m_EF_mbSpName);
	declareProperty("EF_mbMbts_2_eff",m_EF_mbMbts_2_effName);
	declareProperty("EF_mbRd1_eff",m_EF_mbRd1_effName);
	
	declareProperty("EF_mbLucid_eff",m_EF_mbLucid_effName);
	
	declareProperty("EF_rd0_filled_NoAlg",m_EF_rd0_filled_NoAlgName);
	
	declareProperty("EF_L1ItemStreamer_L1_MBTS_2",m_EF_L1ItemStreamer_L1_MBTS_2Name);
	declareProperty("EF_mbZdc_eff",m_EF_mbZdc_effName);
	//declareProperty("EF_mbLucid_eff",m_EF_mbLucid_effName);
	
	declareProperty("EF_mbMbts_1_1_eff",m_EF_mbMbts_1_1_effName);
	declareProperty("EF_mbMbts_2_2_eff",m_EF_mbMbts_2_2_effName);
	declareProperty("EF_mbMbts_4_4_eff",m_EF_mbMbts_4_4_effName);
	
	declareProperty("EF_mbSpTrkVtxMh",m_EF_mbSpTrkVtxMhName);
	declareProperty("EF_mbSpTrkVtxMh_eff",m_EF_mbSpTrkVtxMh_effName);
	
	declareProperty("EF_mbSpTrkVtxMh_hip_trk150_L1TE50",m_EF_mbSpTrkVtxMh_hip_trk150_L1TE50Name);
	declareProperty("EF_mbSpTrkVtxMh_hip_trk190_L1TE50",m_EF_mbSpTrkVtxMh_hip_trk190_L1TE50Name);
	declareProperty("EF_mbSpTrkVtxMh_hip_trk120_L1TE35",m_EF_mbSpTrkVtxMh_hip_trk120_L1TE35Name);
	declareProperty("EF_mbSpTrkVtxMh_hip_trk190_L1TE35",m_EF_mbSpTrkVtxMh_hip_trk190_L1TE35Name);
	declareProperty("EF_mbSpTrkVtxMh_hip_trk240_L1TE35",m_EF_mbSpTrkVtxMh_hip_trk240_L1TE35Name);
	declareProperty("EF_mbSpTrkVtxMh_hip_trk180_L1TE65",m_EF_mbSpTrkVtxMh_hip_trk180_L1TE65Name);
	declareProperty("EF_mbSpTrkVtxMh_hip_trk200_L1TE65",m_EF_mbSpTrkVtxMh_hip_trk200_L1TE65Name);
	declareProperty("EF_mbSpTrkVtxMh_hip_trk240_L1TE65",m_EF_mbSpTrkVtxMh_hip_trk240_L1TE65Name);
	declareProperty("EF_mbSpTrkVtxMh_hip_trk175_L1MBTS",m_EF_mbSpTrkVtxMh_hip_trk175_L1MBTSName);
	
	declareProperty("EF_L1ZDC_A_NoAlg",m_EF_L1ZDC_A_NoAlgName);
	declareProperty("EF_L1ZDC_C_NoAlg",m_EF_L1ZDC_C_NoAlgName);
	declareProperty("EF_L1LHCF_NoAlg",m_EF_L1LHCF_NoAlgName);
	
	// -------------------------------------------------------- NEW AUTOMATIC WAY TO COPE WITH THINGS----------------------
        declareProperty("MinBiasTrigItem", m_trigItems);
        declareProperty("MinBiasAvailableAlgorithms", m_availableAlgs);
        declareProperty("MinBiasHistoTargets", m_histoTargets);	

        m_numberOfEvents=0;
        //Energy-Time
        declareProperty("T2MBTSContainerName", m_t2mbtsContainerName);
        declareProperty("MBTSContainerName", m_mbtsContainerName);
        declareProperty("MBTS_countsSideA", m_mbtsCountsSideA);
        declareProperty("MBTS_countsSideC", m_mbtsCountsSideC);
        
        //BCM
        declareProperty("BCMContainerName", m_bcmContainerName = "BCM_RDOs");
        //Timing Cut-offs
	declareProperty("OutOfTimePulsePosition", m_OOTPulse = 11); //6 old value
	declareProperty("InTimePulsePosition", m_ITPulse = 43); //38 old value
	declareProperty("WideInTimePulsePosition", m_WITPulse = 38); //38 old value
	declareProperty("NarrowTimeWindow", m_TimeWindow = 7); //2 old value
	declareProperty("WideTimeWindow", m_WideTimeWindow = 10); //10 old value
	declareProperty("RdoIndex", m_rdoIndex = 18); // Rdo index; 0 for MC
	
	//LUCID
	declareProperty("LUCID_RawDataContainerName", m_LUCID_RawDataContainerName);
	
	//ZDC
	declareProperty("ZDC_ContainerName", m_ZdcContainerName);
	declareProperty("Calo_ContainerName", m_CaloCellContainerName);

        std::ostringstream ss;
        std::string partName;

        //side A
        for (int c = 0; c < 16; c++) {
                partName = "A";
                ss.str("");
                ss << partName << std::setfill('0') << std::setw(2) << c << std::setfill(' '); //MBTS_A00, MBTS_C03
                m_moduleLabel.push_back(ss.str());
        }

        //side C
        for (int c = 0; c < 16; c++) {
                partName = "C";
                ss.str("");
                ss << partName << std::setfill('0') << std::setw(2) << c << std::setfill(' '); //MBTS_A00, MBTS_C03
                m_moduleLabel.push_back(ss.str());
        }
}

/*---------------------------------------------------------*/
StatusCode HLTMinBiasMonTool::initialize()
/*---------------------------------------------------------*/
{
	StatusCode sc;

	sc = service("StoreGateSvc", m_storeGate);
	if (sc.isFailure()) {
		ATH_MSG_FATAL("Unable to locate Service StoreGateSvc");
		return sc;
	}

	sc = service("DetectorStore", m_detStore);
	if (sc.isFailure()) {
		(*m_log) << MSG::ERROR << "Unable to get pointer to DetectorStore Service" << endreq;
		return sc;
	}
	
	sc = m_detStore->retrieve(m_tileID);
	if (sc.isFailure()) {
		(*m_log) << MSG::ERROR << "Unable to retrieve TileID helper from DetectorStore" << endreq;
		return sc;
	}
	
	sc = m_detStore->retrieve(m_ZdcID);
	if (sc.isFailure()) {
		(*m_log) << MSG::ERROR << "Unable to retrieve ZdcID helper from DetectorStore" << endreq;
		return sc;
	}

	// retrieve the trigger decision tool
	sc = m_tdthandle.retrieve();
	if (sc.isFailure()) {
		*m_log << MSG::ERROR << "Could not retrieve TrigDecisionTool!" << endreq;
		return sc;
	}

	//The Chain Groups are initialized here.
	m_EF_mbSpTrk = m_tdthandle->getChainGroup(m_EF_mbSpTrkName);
	m_EF_mbSp = m_tdthandle->getChainGroup(m_EF_mbSpName);
	//m_EF_mbMbts_1_eff = m_tdthandle->getChainGroup("EF_mbMbts_1_eff");
	m_EF_mbMbts_2_eff = m_tdthandle->getChainGroup(m_EF_mbMbts_2_effName);
	//m_EF_mbZdc_eff = m_tdthandle->getChainGroup("EF_mbZdc_eff");
	m_EF_mbRd1_eff = m_tdthandle->getChainGroup(m_EF_mbRd1_effName);

// 8 TeV Low-mu
	m_EF_mbLucid_eff = m_tdthandle->getChainGroup(m_EF_mbLucid_effName);

	m_EF_rd0_filled_NoAlg = m_tdthandle->getChainGroup(m_EF_rd0_filled_NoAlgName);        
	//m_EF_mbMbts_2_NoAlg = m_tdthandle->getChainGroup("EF_mbMbts_2_NoAlg");      

// pA 
// Rd1 used instead of rd0 above (8 TeV pp low mu)
// defined above 	m_EF_mbRd1_eff        
// used instead of mbMbts_2_NoAlg (8 TeV pp low mu)
	m_EF_L1ItemStreamer_L1_MBTS_2 = m_tdthandle->getChainGroup(m_EF_L1ItemStreamer_L1_MBTS_2Name);      
	m_EF_mbZdc_eff = m_tdthandle->getChainGroup(m_EF_mbZdc_effName);
	//m_EF_mbLucid_eff = m_tdthandle->getChainGroup(m_EF_mbLucid_effName);

	//Heavy-Ion triggers
	m_EF_mbMbts_1_1_eff = m_tdthandle->getChainGroup(m_EF_mbMbts_1_1_effName);
	m_EF_mbMbts_2_2_eff = m_tdthandle->getChainGroup(m_EF_mbMbts_2_2_effName);
	m_EF_mbMbts_4_4_eff = m_tdthandle->getChainGroup(m_EF_mbMbts_4_4_effName);

	//High Multiplicity
	m_EF_mbSpTrkVtxMh = m_tdthandle->getChainGroup(m_EF_mbSpTrkVtxMhName);
	m_EF_mbSpTrkVtxMh_eff = m_tdthandle->getChainGroup(m_EF_mbSpTrkVtxMh_effName);

// Heavy-Ion proton-lead hip_trk (pA 2013)
	m_EF_mbSpTrkVtxMh_hip_trk150_L1TE50 = m_tdthandle->getChainGroup(m_EF_mbSpTrkVtxMh_hip_trk150_L1TE50Name);
	m_EF_mbSpTrkVtxMh_hip_trk190_L1TE50 = m_tdthandle->getChainGroup(m_EF_mbSpTrkVtxMh_hip_trk190_L1TE50Name);
	m_EF_mbSpTrkVtxMh_hip_trk120_L1TE35 = m_tdthandle->getChainGroup(m_EF_mbSpTrkVtxMh_hip_trk120_L1TE35Name);
	m_EF_mbSpTrkVtxMh_hip_trk190_L1TE35 = m_tdthandle->getChainGroup(m_EF_mbSpTrkVtxMh_hip_trk190_L1TE35Name);
	m_EF_mbSpTrkVtxMh_hip_trk240_L1TE35 = m_tdthandle->getChainGroup(m_EF_mbSpTrkVtxMh_hip_trk240_L1TE35Name);
	m_EF_mbSpTrkVtxMh_hip_trk180_L1TE65 = m_tdthandle->getChainGroup(m_EF_mbSpTrkVtxMh_hip_trk180_L1TE65Name);
	m_EF_mbSpTrkVtxMh_hip_trk200_L1TE65 = m_tdthandle->getChainGroup(m_EF_mbSpTrkVtxMh_hip_trk200_L1TE65Name);
	m_EF_mbSpTrkVtxMh_hip_trk240_L1TE65 = m_tdthandle->getChainGroup(m_EF_mbSpTrkVtxMh_hip_trk240_L1TE65Name);
	m_EF_mbSpTrkVtxMh_hip_trk175_L1MBTS = m_tdthandle->getChainGroup(m_EF_mbSpTrkVtxMh_hip_trk175_L1MBTSName);

	m_EF_L1ZDC_A_NoAlg = m_tdthandle->getChainGroup(m_EF_L1ZDC_A_NoAlgName);
	m_EF_L1ZDC_C_NoAlg = m_tdthandle->getChainGroup(m_EF_L1ZDC_C_NoAlgName);
	m_EF_L1LHCF_NoAlg = m_tdthandle->getChainGroup(m_EF_L1LHCF_NoAlgName);
	
        // -------------------------------------------------------- NEW AUTOMATIC WAY TO COPE WITH THINGS----------------------
        
        //filling map so that only desired algorithms will be executed in the following steps...
        for (unsigned i = 0; i < m_availableAlgs.size(); ++i)
                m_algorithmsForChain[m_availableAlgs[i]] = m_histoTargets[i]; 
        
        m_pathForGroup[MBTS]  =         "MBTS";
        m_pathForGroup[LUCID] =         "LUCID";
        m_pathForGroup[IDMINBIAS] =     "IDMinbias";
        m_pathForGroup[ZDC] =           "ZDC";
        m_pathForGroup[BCM] =           "BCM";
        
        m_mbtsCountsBothSides = m_mbtsCountsSideA + m_mbtsCountsSideC;
        
	/*
	 sc = service("THistSvc", m_histsvc);
	 if( sc.isFailure() ) {
	 *m_log << MSG::FATAL << "Unable to locate Service THistSvc" << endreq;
	 return sc;
	 }
	 */

	sc = ManagedMonitorToolBase::initialize();
	if (sc.isFailure()) {
		*m_log << MSG::ERROR << "Unable to call ManagedMonitoringToolBase::initialize" << endreq;
		return sc;
	}

	return sc;
}

StatusCode HLTMinBiasMonTool::init()
{
        return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasMonTool::book()
{
        // -------------------------------------------------------- NEW AUTOMATIC WAY TO COPE WITH THINGS----------------------
        addMonGroup(new MonGroup(this,"HLT/MinBiasMon", run, ATTRIB_UNMANAGED));
        //Trigger entries
        TH1I *th1i = new TH1I("TriggerEntries", "Trigger Entries;trigger chain id;entry rate", m_trigItems.size(), 0, m_trigItems.size());		
        TProfile *tProf = new TProfile("TriggerEntriesProfile", "Trigger Entries;trigger chain id;entry rate", m_trigItems.size(), 0, m_trigItems.size(), 0, 1);
        
        //Error rates for IDMinBias
        addMonGroup(new MonGroup(this,"HLT/MinBiasMon/IDMinbias", run, ATTRIB_UNMANAGED));
	TProfile *tTrigErrors = new TProfile("IDMinBiasTriggerErrors", "Trigger Errors;trigger chain id;error rate", m_trigItems.size(), 0, m_trigItems.size(), 0, 1);	
        	
        for (const auto &i: m_trigItems)	//for each chain
        {
                tProf->GetXaxis()->FindBin(i.c_str());
                tTrigErrors->GetXaxis()->FindBin(i.c_str());

                for (const auto &j: m_availableAlgs)	//iterate through algorithms
 		{
 		        if (std::string::npos != i.find(j)) //and find their signatures in the chain name
 		 	{
 		 	        unsigned k = m_algorithmsForChain[j];
 		 	        m_histoGroupForChain[i] = k;//assign histoGroup(s) for a given chain

 		 	        if (k)
 		 	        {
 		 	                for (const auto &l: m_pathForGroup)//book needed histograms
 		 	                {
 		 	                        if ( (k & l.first) == l.first)
 		 	                        {
 		 	                                addMonGroup(new MonGroup(this,"HLT/MinBiasMon/" + l.second + "/" + i, run, ATTRIB_UNMANAGED));
 		 	                                bookHistogramsForItem(i, l.first);
 		 	                        }
 		 	                }
 		 	        }
                                break; //no need to look further
 		        }
 	        }
 	}
        tProf->ResetBit(TH1::kCanRebin);
	tProf->SetMinimum(0.0);
	
	th1i->SetMinimum(0.0);
	
	tTrigErrors->ResetBit(TH1::kCanRebin);
	tTrigErrors->SetMinimum(0.0);

        addProfile(tProf, "HLT/MinBiasMon");
        addHistogram(th1i, "HLT/MinBiasMon");
        
        addProfile(tTrigErrors, "HLT/MinBiasMon/IDMinbias");
        
        return StatusCode::SUCCESS;
}

void HLTMinBiasMonTool::bookHistogramsForItem(const std::string &item, unsigned histGroup)
{
        TH1I *th1i;
        TH1F *th1f;
        TH2I *th2i;
        TH2F *th2f;
        TProfile *tProf;
        
        (*m_log) << MSG::DEBUG << "Booking histograms for " << item << " chain in hisGroups: " << histGroup << endreq; 

        if ( (histGroup & MBTS) == MBTS)
 	{
 	        th1i = new TH1I("Occupancy", "Offline MBTS Occupancy (no time cut);channel id;entries", 32, 0, 32);
		fixXaxis(th1i);
		th1i->SetMinimum(0.0);
		addHistogram(th1i);
 	        
 	        th1i = new TH1I("Occupancy_Online", "Online MBTS Occupancy (no time cut);channel id;entries", 32, 0, 32);
 	        fixXaxis(th1i);
 	        addHistogram(th1i);
		
		th2f = new TH2F("m_h_time", "MBTS time;channel id;MBTS Time [ns]", 32, 0, 32, 100, -100, 100);
		fixXaxis(th2f);
		addHistogram(th2f);
 	        
 	        th2f = new TH2F("m_h_time_Online", "Online MBTS Time;channel id;MBTS Time [ns]", 32, 0, 32, 100, -100, 100);
		fixXaxis(th2f);
		addHistogram(th2f);
		
		th2f = new TH2F("m_h_energy", "MBTS energy;channel id;MBTS Energy [pC]", 32, 0, 32, 100, -2, 2);
		fixXaxis(th2f);
                addHistogram(th2f);
		
		th2f = new TH2F("m_h_energy_Online", "Online MBTS energy;channel id;MBTS Energy [pC]", 32, 0, 32, 100,-2, 2);
		fixXaxis(th2f);
		addHistogram(th2f);
		
		addHistogram(new TH1I("mbMbts_N_N_hits", "mbMbts_N_N Passed;Min #hits on side A or C;entries", TMath::Max(m_mbtsCountsSideA, m_mbtsCountsSideC) + 1, -0.5, TMath::Max(m_mbtsCountsSideA, m_mbtsCountsSideC) + 0.5));
		addHistogram(new TH1I("TriggerRate", "# of Events per Trigger;trigger chain id;# events", 1, 0, 1));
		addHistogram(new TH1I("TriggerRatePS","# of Events per Trigger * Prescale;trigger chain id;# events * prescale", 10, 0, 10));	        
 	        addHistogram(new TH1I("mbMbts_hits", "mbMbts total hits;#hits;entries", m_mbtsCountsBothSides + 1, -0.5, m_mbtsCountsBothSides + 0.5));

                th2f = new TH2F("m_h_time_TF_SP", "L2 bit failed & simulation passed;channel id;MBTS Time [ns]", 32, 0,	32, 100, -100, 100);
		fixXaxis(th2f);
                addHistogram(th2f);

                th2f = new TH2F("m_h_time_TP_SF", "L2 bit passed & simulation failed;channel id;MBTS Time [ns]", 32, 0,	32, 100, -100, 100);
		fixXaxis(th2f);
		addHistogram(th2f);

                tProf = new TProfile("MbtsEnergyErrors", "Offline MBTS Energy Discrepancies;channel id;error rate", 32, 0, 32, 0, 1);
		fixXaxis(tProf);
		tProf->SetMinimum(0.0);
                addProfile(tProf);
                
                tProf = new TProfile("MbtsTimeErrors", "Offline MBTS Time Discrepancies;channel id;error rate", 32, 0, 32, 0, 1);
		fixXaxis(tProf);
		tProf->SetMinimum(0.0);
		addProfile(tProf);
		
		th2i = new TH2I("m_h_mbtsCorr_N_N", "MBTS hits correlation;MBTS Side A;MBTS Side C", m_mbtsCountsSideA + 1, -0.5, m_mbtsCountsSideA + 0.5, m_mbtsCountsSideC + 1, -0.5, m_mbtsCountsSideC + 0.5);
		th2i->SetOption("COLZ");
		addHistogram(th2i);
 	}
 	if ( (histGroup & LUCID) == LUCID)
 	{
 	        // Lucid
		addHistogram(new TH1I("lucidChannels", "occupancy;channel;hits per channel", 40, -0.5, 39.5));
		addHistogram(new TH1I("lucidChannels_pBX", "occupancy for the previous BX;channel;hits per channel", 40,	-0.5, 39.5));
		addHistogram(new TH1I("lucidChannels_nBX", "occupancy for the next BX;channel;hits per channel", 40, -0.5, 39.5));
		
                //addHistogram(new TH1F("LUCID_All", "L1_LUCID All passed;max. # of hits on side A or C;entries", 21, -0.5, 20.5));
                addHistogram(new TH1F("LUCID_A_C_All", "L1_LUCID_A_C All passed; min. # of hits on side A or C;entries", 21, -0.5, 20.5));
                addHistogram(new TH1F("LUCID_All", "L1_LUCID All passed;max. # of hits on side A or C;entries", 21, -0.5, 20.5));
                addHistogram(new TH1F("LUCID_nBX", "L1_LUCID passed;max. # of hits on side A or C for the next BX;entries", 21,-0.5, 20.5));
                addHistogram(new TH1F("LUCID_pBX", "L1_LUCID passed;max. # of hits on side A or C for the previous BX;entries",21, -0.5, 20.5));
                addHistogram(new TH1F("LUCID_A_C", "L1_LUCID_A_C passed; min. # of hits on side A or C;entries", 21, -0.5, 20.5));
                addHistogram(new TH1F("LUCID_A_C_nBX","L1_LUCID_A_C passed; min. # of hits on side A or C for the next BX;entries", 21, -0.5, 20.5));
                addHistogram(new TH1F("LUCID_A_C_pBX","L1_LUCID_A_C passed; min. # of hits on side A or C for the previous BX;entries", 21, -0.5, 20.5));
                
                //Error rates
		tProf = new TProfile("TriggerErrors", "Trigger Errors;trigger chain id;error rate", 5, 0, 5, 0, 1);
		tProf->GetXaxis()->FindBin("L1_LUCID");
		tProf->GetXaxis()->FindBin("L1_LUCID_A");
		tProf->GetXaxis()->FindBin("L1_LUCID_C");
		tProf->GetXaxis()->FindBin("L1_LUCID_A_C");
		tProf->GetXaxis()->FindBin("L1_LUCID_COMM");
		tProf->ResetBit(TH1::kCanRebin);
		tProf->SetMinimum(0.0);
		addProfile(tProf);
 	}
 		 	                
 	if ( (histGroup & IDMINBIAS) == IDMINBIAS)
 	{
 	        //default:
                // L2, ID
	 	addHistogram(new TH1I("pixBarr_sp", "number of L2 pixel spacepoints in the barrel;;entries", 250, -0.5, 2499.5));       
	 	addHistogram(new TH1I("pixECA_sp", "number of L2 pixel spacepoints in ECA;;entries", 250, -0.5, 2499.5));               
	 	addHistogram(new TH1I("pixECC_sp", "number of L2 pixel spacepoints in ECC;;entries", 250, -0.5, 2499.5));               
	 	addHistogram(new TH1I("sctBarr_sp", "number of L2 sct spacepoints in the barrel;;entries", 300, -0.5, 2999.5));         
	 	addHistogram(new TH1I("sctECA_sp", "number of L2 sct spacepoints in ECA;;entries", 300, -0.5, 2999.5));                 
	 	addHistogram(new TH1I("sctECC_sp", "number of L2 sct spacepoints in ECC;;entries", 300, -0.5, 2999.5));                 
	 	
	 	addHistogram(new TH1I("h_pixTot", "L2_mbMbts_eff passed;total number of L2 pixel space-points;entries", 250, -0.5, 2499.5));
	 	addHistogram(new TH1I("h_sctTot", "L2_mbMbts_eff passed;total number of L2 SCT space-points;entries", 300, -0.5, 2999.5));
	 	
	 	// EF, ID
	 	addHistogram(new TH1I("minbiasTracks", "number of minbias tracks;# of tracks; entries", 125, -0.5, 249.5));             
	 	
		// MBTS energy
  	        // The A side mean energy passed the trigger
		addHistogram(new TH1F("m_h_energyMean_A", "Mean MBTS Energy A side passed;MBTS Energy [pC];entries", 100, -5, 95));           
		
		//The A side  maximum energy passed the trigger
		addHistogram(new TH1F("m_h_energyMax_A", "Maximum MBTS Energy A side passed;MBTS Energy [ns];[pC];entries", 100, -5, 95));    

		//The C side  mean energy passed the trigger
		addHistogram(new TH1F("m_h_energyMean_C", "Mean MBTS Energy C side passed;MBTS Energy [pC];entries", 100, -5, 95));           

		//The C side  maximum energy passed the trigger
		addHistogram(new TH1F("m_h_energyMax_C", "Maximum MBTS Energy C side passed;MBTS Energy [pC];entries", 100, -5, 95));         

		//energy online + L2 , EF passed/failed
  	        //The  time online
		addHistogram(new TH1D("m_h_energy_onl", "MBTS Energy  online;MBTS Energy [pC];entries", 100, -5, 95));                  

		// Time
  	        //=== The time is monitored only if the energy is sufficient to suppress noise
   	        //The A side  mean time passed the trigger
		addHistogram(new TH1F("m_h_timeMean_A", "Mean MBTS Time A side passed;MBTS Time [ns];entries", 100, -25, 25));        

		//The A side  minimum time passed the trigger
		addHistogram(new TH1F("m_h_timeMin_A", "Minimum MBTS Time A side passed;MBTS Time [ns];entries", 100, -25, 25));      

		//The C side  mean time passed the trigger
		addHistogram(new TH1F("m_h_timeMean_C", "Mean MBTS Time C side passed;MBTS Time [ns];entries", 100, -25, 25));        

		//The C side  minimum time passed the trigger
		addHistogram(new TH1F("m_h_timeMin_C", "Minimum MBTS Time C side passed;MBTS Time [ns];entries", 100, -25, 25));      

  	        // time online + L2 , EF passed/failed
   	        //The  time online
		addHistogram(new TH1D("m_h_time_onl", "MBTS Time  online;MBTS Time [ns];entries", 100, -25, 25));                       

		// energy-time cell occupancy
  	        // side A
  	        th1i = new TH1I("Occupancy_Online - side A", "Online MBTS Occupancy (no time cut);channel id;entries", 32, 0, 32);        	
		th1i->SetMinimum(0.0);
		addHistogram(th1i);
		
		// side C
		th1i = new TH1I("Occupancy_Online - side C", "Online MBTS Occupancy (no time cut);channel id;entries", 32, 0, 32);              
		th1i->SetMinimum(0.0);
		addHistogram(th1i);
	}
 	if ( (histGroup & ZDC) == ZDC)
 	{
 	        addHistogram(new TH2I("zdcChannels", "occupancy;channel ID;hits per channel", 8, -0.5, 7.5, 100, 0,1200));
                addHistogram(new TH1I("ZDC_EMPTY", "L1_ZDC_EMPTY passed;min. Low Gain ADC counts on side A or C;entries", 100,0., 2000.));

                th1f = new TH1F("m_h_ZDC_LHCF_corr", "L1_ZDC_C vs L1_LHCF with MbSp(Trk);;events accepted", 4,0,4);
		th1f->GetXaxis()->FindBin("LHCF and ZDC_C");
		th1f->GetXaxis()->FindBin("no LHCF and no ZDC_C");
		th1f->GetXaxis()->FindBin("LHCF, no ZDC_C");
		th1f->GetXaxis()->FindBin("no LHCF but ZDC_C");
                th1f->ResetBit(TH1::kCanRebin);
                th1f->Sumw2();
                addHistogram(th1f);
                
                th1f = new TH1F("m_h_ZDC_TE", "L1_TE and L1_ZDC_x_VTE with MbSp(Trk);;events accepted", 5,0,5);
		th1f->GetXaxis()->FindBin("L1_TE0");
		th1f->GetXaxis()->FindBin("L1_TE20");
		th1f->GetXaxis()->FindBin("L1_TE50");
		th1f->GetXaxis()->FindBin("L1_ZDC_A_VTE20");
		th1f->GetXaxis()->FindBin("L1_ZDC_C_VTE20");
                th1f->ResetBit(TH1::kCanRebin);
                th1f->Sumw2();
                addHistogram(th1f);
                
                addHistogram(new TH1I("ZDC_All", "L1_ZDC passed any beam;min. Low Gain ADC counts on side A or C;entries", 100, 0., 2000.));
                addHistogram(new TH1I("ZDC_A_C_All", "L1_ZDC_A_C passed any beam;max. Low Gain ADC counts on side A or C;entries", 100, 0.,2000.));
                addHistogram(new TH2F("ZDC_vs_FCAL_A_All","L1_ZDC_A passed any beam;Low Gain ADC counts on side A;Calo energy", 100, 0., 2000., 100, 0., 20000));
                addHistogram(new TH1I("ZDC_A_All", "L1_ZDC_A passed any beam;Low Gain ADC counts on side A;entries", 100, 0., 2000.));
                addHistogram(new TH2F("ZDC_vs_FCAL_C_All","L1_ZDC_C passed any beam;Low Gain ADC counts on side C;Calo energy", 100, 0., 2000., 100, 0., 20000));
                addHistogram(new TH1I("ZDC_C_All", "L1_ZDC_C passed any beam;Low Gain ADC counts on side C;entries", 100, 0., 2000.));
 	}
 	if ( (histGroup & BCM) == BCM)
 	{
 	        addHistogram(new TH1I("bcmChannels", "occupancy;channel;hits per channel", 16, -0.5, 15.5));
 	        addHistogram(new TH2I("pulse1position", "1st pulse timing;channel;position of pulse1", 16, -0.5, 15.5, 64, -0.5, 63.5));
 	        addHistogram(new TH2I("pulse1width", "1st pulse width;channel;width of pulse1", 16, -0.5, 15.5, 32, -0.5, 31.5));
 	        addHistogram(new TH2I("pulse2position", "2nd pulse timing;channel;position of pulse2", 16, -0.5, 15.5, 64, -0.5, 63.5));
 	        addHistogram(new TH2I("pulse2width", "2nd pulse width;channel; width of pulse2", 16, -0.5, 15.5, 32, -0.5, 31.5));
 	        
 	        //TProfiles are to be drawn on top of the corresponsing 1D plots

		addProfile(new TProfile("pulse1position_prof", "1st pulse average timing;channel;position of pulse1",16, -0.5, 15.5, -0.5, 63.5));
		addProfile(new TProfile("pulse1width_prof", "1st pulse average width;channel;width of pulse1", 16, -0.5, 15.5, -0.5, 31.5));
		addProfile(new TProfile("pulse2position_prof", "2nd pulse average timing;channel;position of pulse2", 16, -0.5, 15.5, -0.5, 63.5));
		addProfile(new TProfile("pulse2width_prof", "2nd pulse average width;channel; width of pulse2", 16, -0.5, 15.5, -0.5, 31.5));
		
		addHistogram(new TH1F("BCM_Wide_All", "L1_BCM_Wide passed all; min. # of hits on side A or C;entries", 9, -0.5, 8.5));
		addHistogram(new TH1F("BCM_AC_CA_All", "L1_BCM_AC_CA passed all; min. # of hits on side A or C;entries", 9, -0.5, 8.5));
		addHistogram(new TH1F("BCM_HT_All", "L1_BCM_HT passed all;# of low-gain hits;entries", 17, -0.5, 16.5));
		addHistogram(new TH1D("BCM_Wide", "L1_BCM_Wide passed; min. # of hits on side A or C;entries", 9, -0.5, 8.5));
		addHistogram(new TH1D("BCM_AC_CA", "L1_BCM_AC_CA passed; min. # of hits on side A or C;entries", 9, -0.5, 8.5));
		addHistogram(new TH1D("BCM_HT", "L1_BCM_HT passed;# of low-gain hits;entries", 17, -0.5, 16.5));
		
		//Error rates
		tProf = new TProfile("TriggerErrors", "Trigger Errors;trigger chain id;error rate", 3, 0, 3, 0, 1);
		tProf->GetXaxis()->FindBin("L1_BCM_AC_CA");
		tProf->GetXaxis()->FindBin("L1_BCM_Wide");
		tProf->GetXaxis()->FindBin("L1_BCM_HT");
		tProf->ResetBit(TH1::kCanRebin);
		tProf->SetMinimum(0.0);
		addProfile(tProf);
 	}
 	(*m_log) << MSG::DEBUG << "All histograms booked successfully" << endreq; 
        
        //return sc;
}

StatusCode HLTMinBiasMonTool::fill()
{
        StatusCode sc = StatusCode::SUCCESS;
        
        for (const auto &i: m_trigItems)
        {
                if (m_tdthandle->isPassed(/*"EF_"*/ "HLT_" + i))        //temporally
                {
                       profile("TriggerEntriesProfile", "HLT/MinBiasMon")->Fill(i.c_str(), 1);
                       hist("TriggerEntries", "HLT/MinBiasMon")->Fill(i.c_str(), 1);
                       fillHistogramsForItem(i, m_histoGroupForChain[i]);
                }
                else{
                        profile("TriggerEntriesProfile", "HLT/MinBiasMon")->Fill(i.c_str(), 0);
                        hist("TriggerEntries", "HLT/MinBiasMon")->Fill(i.c_str(), 0);
                        (*m_log) << MSG::DEBUG << i << " chain is empty" << endreq; 
                }
        }
        
        return sc;
}

void HLTMinBiasMonTool::fillHistogramsForItem(const std::string &item, unsigned histGroup)
{
        StatusCode sc;
        
        (*m_log) << MSG::DEBUG << "Currently processing: " << item << " in " << histGroup << endreq;

        if ( (histGroup & MBTS) == MBTS)
 	{
 	        setCurrentMonGroup("HLT/MinBiasMon/MBTS/" + item);
 	        sc = fillMbtsInfo(item);
                if (sc.isFailure())
                {
                        (*m_log) << MSG::WARNING << "Couldn't fill MBTS info properly for: " << item << endreq;
                }
 	}
 	if ( (histGroup & LUCID) == LUCID)
 	{
 	        setCurrentMonGroup("HLT/MinBiasMon/LUCID/" + item);
 	        sc = fillLUCIDInfo();
                if (sc.isFailure())
                {
                        (*m_log) << MSG::WARNING << "Couldn't fill LUCID info properly for: " << item << endreq;
                }
 	}
        if ( (histGroup & IDMINBIAS) == IDMINBIAS)
 	{
 	        setCurrentMonGroup("HLT/MinBiasMon/IDMinbias/" + item);
 	        sc = fillSpacePointInfo(item);
                if (sc.isFailure())
                {
                        (*m_log) << MSG::WARNING << "Couldn't fill space point info properly for: " << item << endreq;
                }
                sc = fillTrackingInfo();
                if (sc.isFailure())
                {
                        (*m_log) << MSG::WARNING << "Couldn't fill tracking info properly for: " << item << endreq;
                }
                sc = fillHLTMbtsInfo();
                if (sc.isFailure())
                {
                        (*m_log) << MSG::WARNING << "Couldn't fill HLT MBTS info properly for: " << item << endreq;
                }
 	        
 	}
 	if ( (histGroup & ZDC) == ZDC)
 	{
 	        setCurrentMonGroup("HLT/MinBiasMon/ZDC/" + item);
 	        sc = fillZDCInfo();
                if (sc.isFailure())
                {
                        (*m_log) << MSG::WARNING << "Couldn't fill ZDC info properly for: " << item << endreq;
                }
 	}
 	if ( (histGroup & BCM) == BCM)
 	{
 	        setCurrentMonGroup("HLT/MinBiasMon/BCM/" + item);
 	        sc = fillBCMInfo();
                if (sc.isFailure())
                {
                        (*m_log) << MSG::WARNING << "Couldn't fill BCM info properly for: " << item << endreq;
                }
 	}
        
        ++m_numberOfEvents;
        (*m_log) << MSG::DEBUG << "PROCESSED SUCCESSFULLY" << endreq;        
              
        //return  StatusCode::SUCCESS;
}

StatusCode HLTMinBiasMonTool::fillZDCInfo()
{
        StatusCode sc = StatusCode::SUCCESS;
        
        // Unapcking Calorimeter energy:

	const CaloCellContainer * cellColl = 0;
	sc = m_storeGate->retrieve(cellColl,m_CaloCellContainerName);
	if (sc.isFailure()) {
	  return(StatusCode::SUCCESS); // to fix no-Calo datasets (bug #90180)
	}
	double FCal_A = 0;
	double FCal_C = 0;

	CaloCellContainer::const_iterator itrCell = cellColl->begin();
	CaloCellContainer::const_iterator itrLastCell = cellColl->end();      
	
	for ( ; itrCell!=itrLastCell; ++itrCell){
	  const CaloCell* cell = (*itrCell) ; 
	  const double theCellE=cell->energy();
	  

//	  int inum = CaloSampling::getSampling( *cell );
	  int inum = CaloSamplingHelper::getSampling( *cell );
	  if ((CaloSampling::FCAL0 == inum ) || 
	      (CaloSampling::FCAL1 == inum ) || 
	      (CaloSampling::FCAL2 == inum ) ){
	    if(cell->eta()>0){
	      FCal_A+= theCellE;
	    } else {
	      FCal_C+= theCellE;
	    }
	  }
	}
	FCal_A/= 1000.; //Calorimeter energy on side A
	FCal_C/= 1000.; //Calorimeter energy on side c


	//Unpacking Zdc Data:
	const ZdcRawChannelCollection *zdcChannelCollection = 0;

	double E_A = 0; //ZDC energy on Side A
	double E_C = 0; //ZDC energy on Side C

	sc = m_storeGate->retrieve(zdcChannelCollection, m_ZdcContainerName);

	if (sc.isFailure() || zdcChannelCollection->empty()) {
		if (sc.isFailure())
			(*m_log) << MSG::WARNING << "Failed to retrieve ZdcRawChannelCollection" << endreq;
		else
			(*m_log) << MSG::DEBUG << "ZdcRawChannelCollection is empty." << endreq;
	} else {

		ZdcRawChannelCollection::const_iterator itr = zdcChannelCollection->begin();
		ZdcRawChannelCollection::const_iterator end = zdcChannelCollection->end();

		double e;
		//double t;

		for (; itr != end; ++itr) {

			Identifier id = (*itr)->identify();
			e = ((*itr)->getEnergy(0));// / 1000.; // MeV-->GeV
			//t = (*itr)->getTime(0);

			if (m_ZdcID->module(id) < 4 && m_ZdcID->type(id) == 0) {
				if (m_ZdcID->side(id) > 0) { //Side A
					hist2("zdcChannels")->Fill(m_ZdcID->module(id), e);
					E_A += e;
				} else { //Side C
					E_C += e;
					hist2("zdcChannels")->Fill(m_ZdcID->module(id) + 4, e);
				}
			}

		} //End of loop over ZdcRawChannelCollection
	}

	//Test of the trigger decisions


	//Empty
	//----------------------------------------------------------------------
	hist("ZDC_EMPTY")->Fill(TMath::Max(E_A, E_C));

        // LHCf vs ZDC_C
        hist("m_h_ZDC_LHCF_corr")->Fill("LHCF and ZDC_C",1.0);
        hist("m_h_ZDC_LHCF_corr")->Fill("no LHCF and no ZDC_C",1.0);
        hist("m_h_ZDC_LHCF_corr")->Fill("LHCF, no ZDC_C",1.0);
        hist("m_h_ZDC_LHCF_corr")->Fill("no LHCF but ZDC_C",1.0);
        hist("m_h_ZDC_TE")->Fill("L1_TE0",1.0);
        hist("m_h_ZDC_TE")->Fill("L1_TE20",1.0);
        hist("m_h_ZDC_TE")->Fill("L1_TE50",1.0);
        hist("m_h_ZDC_TE")->Fill("L1_ZDC_C_VTE20",1.0);
        hist("m_h_ZDC_TE")->Fill("L1_ZDC_A_VTE20",1.0);

        // Any beam
	//----------------------------------------------------------------------
        hist("ZDC_All")->Fill(TMath::Max(E_A, E_C));
        hist("ZDC_A_C_All")->Fill(TMath::Min(E_A, E_C));
        hist2("ZDC_vs_FCAL_A_All")->Fill(E_A,FCal_A);
        hist("ZDC_A_All")->Fill(E_A);
        hist2("ZDC_vs_FCAL_C_All")->Fill(E_C,FCal_C);
	hist("ZDC_C_All")->Fill(E_C);

        // MBTS condition (stable beam)
	//----------------------------------------------------------------------


	/*if (orthogonal_trigger)
		return sc;*/

        return sc;
}

StatusCode HLTMinBiasMonTool::fillLUCIDInfo()
{
        StatusCode sc = StatusCode::SUCCESS;
        
        const int m_nLucidTubes = 40;
        
        int hitsA       = 0, hitsC      = 0;
	int hitsAp      = 0, hitsCp     = 0;
	int hitsAn      = 0, hitsCn     = 0;
        
        const LUCID_RawDataContainer* m_LUCID_RawDataContainer;
	sc = m_storeGate->retrieve(m_LUCID_RawDataContainer, m_LUCID_RawDataContainerName);

	if (sc.isFailure() || m_LUCID_RawDataContainer->empty()) {
		if (sc.isFailure())
			(*m_log) << MSG::WARNING << "Failed to retrieve LucidMinBiasMonTool for  LUCID_RawDataContainer" << endreq;
		else
			(*m_log) << MSG::DEBUG << "LUCID_RawDataContainer is empty." << endreq;

	} else {
		(*m_log) << (MSG::DEBUG) << " LUCID_RawDataContainer is retrived from StoreGate " << endreq;

		LUCID_RawDataContainer::const_iterator LUCID_RawData_itr = m_LUCID_RawDataContainer->begin();
		LUCID_RawDataContainer::const_iterator LUCID_RawData_end = m_LUCID_RawDataContainer->end();

		for (; LUCID_RawData_itr != LUCID_RawData_end; LUCID_RawData_itr++) {

			(*m_log) << (MSG::DEBUG) << " word1  : " << (*LUCID_RawData_itr)->getWord0() << endreq << " word2  : "
					<< (*LUCID_RawData_itr)->getWord1() << endreq << " word3  : " << (*LUCID_RawData_itr)->getWord2() << endreq
					<< " word4  : " << (*LUCID_RawData_itr)->getWord3() << endreq << " word1p : "
					<< (*LUCID_RawData_itr)->getWord0p() << endreq << " word2p : " << (*LUCID_RawData_itr)->getWord1p() << endreq
					<< " word3p : " << (*LUCID_RawData_itr)->getWord2p() << endreq << " word4p : "
					<< (*LUCID_RawData_itr)->getWord3p() << endreq << " word1n : " << (*LUCID_RawData_itr)->getWord0n() << endreq
					<< " word2n : " << (*LUCID_RawData_itr)->getWord1n() << endreq << " word3n : "
					<< (*LUCID_RawData_itr)->getWord2n() << endreq << " word4n : " << (*LUCID_RawData_itr)->getWord3n() << endreq
					<< " status: " << (*LUCID_RawData_itr)->getStatus() << endreq;

			for (int tub = 0; tub < m_nLucidTubes; tub++)
				if ((*LUCID_RawData_itr)->isTubeFired(tub, 0))
					hist("lucidChannels")->Fill(tub);

			//previous BX
			for (int tub = 0; tub < m_nLucidTubes; tub++)
				if ((*LUCID_RawData_itr)->isTubeFired(tub, 1))
					hist("lucidChannels_pBX")->Fill(tub);

			//next BX
			for (int tub = 0; tub < m_nLucidTubes; tub++)
				if ((*LUCID_RawData_itr)->isTubeFired(tub, 2))
					hist("lucidChannels_nBX")->Fill(tub);

			hitsA = (*LUCID_RawData_itr)->getNhitsPMTsideA();
			hitsC = (*LUCID_RawData_itr)->getNhitsPMTsideC();

			hitsAp = (*LUCID_RawData_itr)->getNhitsPMTsideAprevBX();
			hitsCp = (*LUCID_RawData_itr)->getNhitsPMTsideCprevBX();

			hitsAn = (*LUCID_RawData_itr)->getNhitsPMTsideAnextBX();
			hitsCn = (*LUCID_RawData_itr)->getNhitsPMTsideCnextBX();

		}
	}

	// Now we make plots to monitor the trigger decisions.
	// All the plots are done using # of hits
	// ---- fill histos with all events
	
	hist("LUCID_All")->Fill(TMath::Max(hitsA, hitsC));
	hist("LUCID_A_C_All")->Fill(TMath::Min(hitsA, hitsC));

	// --- fill only histos only for chains passing orthogonal trigger 
	/*if (orthogonal_trigger)               //how to resolve this???
		return sc;*/
        
        hist("LUCID_All")->Fill(TMath::Max(hitsA, hitsC));
	hist("LUCID_nBX")->Fill(TMath::Max(hitsAn, hitsCn));
	hist("LUCID_pBX")->Fill(TMath::Max(hitsAp, hitsCp));
	profile("TriggerErrors")->Fill("L1_LUCID", error_bit(TMath::Max(hitsA, hitsC) >= 1, 1));
        profile("TriggerErrors")->Fill("L1_LUCID_COMM", error_bit(TMath::Max(hitsA, hitsC) >= 1, 1));

        hist("LUCID_A_C")->Fill(TMath::Min(hitsA, hitsC));
	hist("LUCID_A_C_nBX")->Fill(TMath::Min(hitsAn, hitsCn));
	hist("LUCID_A_C_pBX")->Fill(TMath::Min(hitsAp, hitsCp));
	profile("TriggerErrors")->Fill("L1_LUCID_A_C", error_bit(TMath::Min(hitsA, hitsC) >= 1, 1));
        profile("TriggerErrors")->Fill("L1_LUCID_A", error_bit(TMath::Min(hitsA, hitsC) >= 1, 1));
        profile("TriggerErrors")->Fill("L1_LUCID_C", error_bit(TMath::Min(hitsA, hitsC) >= 1, 1));

	return sc;
}

StatusCode HLTMinBiasMonTool::fillBCMInfo()
{
        //hits on high-gain channels
	int n_OOT_A     = 0;
	int n_OOT_C     = 0;
	int n_W_OOT_A   = 0;
	int n_W_OOT_C   = 0;
	int n_IT_A      = 0;
	int n_IT_C      = 0;
	int n_W_IT_A    = 0;
	int n_W_IT_C    = 0;

	//hits on low-gain chnnels
	int n_LG_A = 0;
	int n_LG_C = 0;

	//we should check that the both triggers are in the same state!

	const BCM_RDO_Container* bcmRDOcont = 0;
	StatusCode sc_bcm = m_storeGate->retrieve(bcmRDOcont, m_bcmContainerName);
	if (sc_bcm.isFailure() || (bcmRDOcont->empty())) {
		if (sc_bcm.isFailure())
			(*m_log) << MSG::WARNING << "Failed to retrieve BcmMinBiasMonTool for BCM_RDO_Container" << endreq;
		else
			(*m_log) << MSG::DEBUG << " BCM_RDO_Container is empty." << endreq;
	} else {
		(*m_log) << MSG::DEBUG << " ====== START HLTMinBias MonTool for BCM_RDO_Container ====== " << endreq;

		BCM_RDO_Container::const_iterator bcm_coll_itr = bcmRDOcont->begin();
		for (; bcm_coll_itr != bcmRDOcont->end(); ++bcm_coll_itr) {

			unsigned int channelID = (*bcm_coll_itr)->getChannel();

			if ((*bcm_coll_itr)->empty()) {
				// If we look at InnerDetector/InDetDigitization/BCM_Digitization/src/BCM_Digitization.cxx, we see
				//  that a new collection is created if you are to insert an RDO in to it.  So we should not see
				//  empty collections.
				(*m_log) << MSG::DEBUG << "Encountered an empty BCM_RDO_Collection, skipping it." << endreq;
			} else {

				//Now we process a non-empty BCM container
				BCM_RDO_Collection::const_iterator bcm_rdo = (*bcm_coll_itr)->begin();

				for (; bcm_rdo != (*bcm_coll_itr)->end(); ++bcm_rdo) {

					if (channelID != static_cast<unsigned int> ((*bcm_rdo)->getChannel())) {
						// This should not happen. (Again see the digitization code mentioned above.)
						(*m_log) << MSG::WARNING << "Encountered a BCM_RDO in a wrong BCM_RDO_Collection, skipping it." << endreq;
					} else {

						//if (index != 18) { // we check only 1 BC for data
						if ((*bcm_rdo)->getLVL1A() != m_rdoIndex)
							continue;

						int pulse1pstn = (*bcm_rdo)->getPulse1Position();
						int pulse1wdth = (*bcm_rdo)->getPulse1Width();
						int pulse2pstn = (*bcm_rdo)->getPulse2Position();
						int pulse2wdth = (*bcm_rdo)->getPulse2Width();

						if (pulse1wdth > 0) {
							hist("bcmChannels")->Fill(channelID); //Hit multiplicity

							hist2("pulse1position")->Fill(channelID, pulse1pstn);
							profile("pulse1position_prof")->Fill(channelID, pulse1pstn);
							hist("pulse1width")->Fill(channelID, pulse1wdth);
							profile("pulse1width_prof")->Fill(channelID, pulse1wdth);
						}

						if (pulse2wdth > 0) {
							hist("bcmChannels")->Fill(channelID); //Hit multiplicity

							hist2("pulse2position")->Fill(channelID, pulse2pstn);
							profile("pulse2position_prof")->Fill(channelID, pulse2wdth);
							hist("pulse2width")->Fill(channelID, pulse2wdth);
							profile("pulse2width_prof")->Fill(channelID, pulse2pstn);
						}

						//Emulation of the Trigger logic
						if (channelID < 8) { //A. Low-gain channels
							if (channelID < 4) { //Side A
								if (pulse1wdth > 0)
									n_LG_A++;
								if (pulse2wdth > 0)
									n_LG_A++;
							} else { //side C
								if (pulse1wdth > 0)
									n_LG_C++;
								if (pulse2wdth > 0)
									n_LG_C++;
							}
						} else { // B.High-Gain channels
							if (channelID < 12) { //Side A

								if (abs(pulse1pstn - m_OOTPulse) < m_TimeWindow)
									n_OOT_A++;
								if (abs(pulse1pstn - m_ITPulse) < m_TimeWindow)
									n_IT_A++;
								if (abs(pulse1pstn - m_OOTPulse) < m_WideTimeWindow)
									n_W_OOT_A++;
								if (abs(pulse1pstn - m_WITPulse) < m_WideTimeWindow)
									n_W_IT_A++;

								if (abs(pulse2pstn - m_OOTPulse) < m_TimeWindow)
									n_OOT_A++;
								if (abs(pulse2pstn - m_ITPulse) < m_TimeWindow)
									n_IT_A++;
								if (abs(pulse2pstn - m_OOTPulse) < m_WideTimeWindow)
									n_W_OOT_A++;
								if (abs(pulse2pstn - m_WITPulse) < m_WideTimeWindow)
									n_W_IT_A++;
							} else { //Side C

								if (abs(pulse1pstn - m_OOTPulse) < m_TimeWindow)
									n_OOT_C++;
								if (abs(pulse1pstn - m_ITPulse) < m_TimeWindow)
									n_IT_C++;
								if (abs(pulse1pstn - m_OOTPulse) < m_WideTimeWindow)
									n_W_OOT_C++;
								if (abs(pulse1pstn - m_WITPulse) < m_WideTimeWindow)
									n_W_IT_C++;

								if (abs(pulse2pstn - m_OOTPulse) < m_TimeWindow)
									n_OOT_C++;
								if (abs(pulse2pstn - m_ITPulse) < m_TimeWindow)
									n_IT_C++;
								if (abs(pulse2pstn - m_OOTPulse) < m_WideTimeWindow)
									n_W_OOT_C++;
								if (abs(pulse2pstn - m_WITPulse) < m_WideTimeWindow)
									n_W_IT_C++;
							} // End of the if statement for the channel ID

						}// end of loop

					}
				}

			}
		}
	}

	// Now we make plots to monitor detector work, no MBTS trigger decisions.
	// All the plots are done using # of hits
	
	hist("BCM_Wide_All")->Fill(TMath::Min(n_W_IT_A, n_W_IT_C));
	int min_all = TMath::Max(TMath::Min(n_OOT_A, n_IT_C), TMath::Min(n_OOT_C, n_IT_A));
	hist("BCM_AC_CA_All")->Fill(min_all);
	hist("BCM_HT_All")->Fill(n_LG_A + n_LG_C);

	// Now we make plots to monitor the trigger decisions.
	// All the plots are done using # of hits
	// consequently, improve reliability of the monitoring code

	//if (orthogonal_trigger) return StatusCode::SUCCESS;   //WHAT TO DO WITH THIS???
	
	//up to 8 hits per side [0,8]
	hist("BCM_Wide")->Fill(TMath::Min(n_W_IT_A, n_W_IT_C));
	profile("TriggerErrors")->Fill("L1_BCM_Wide", error_bit(TMath::Min(n_W_IT_A, n_W_IT_C) >= 1, 1));
	
	//int min_all = TMath::Max(TMath::Min(n_OOT_A, n_IT_C), TMath::Min(n_OOT_C, n_IT_A));
        hist("BCM_AC_CA")->Fill(min_all);     
        profile("TriggerErrors")->Fill("L1_BCM_AC_CA", error_bit(TMath::Min(n_OOT_A, n_IT_C) >= 1 || TMath::Min(n_OOT_C, n_IT_A) >= 1, 1));
        
	//up to 16 hits in total [0,16]
	hist("BCM_HT")->Fill(n_LG_A + n_LG_C);
	profile("TriggerErrors")->Fill("L1_BCM_HT", error_bit((n_LG_A + n_LG_C) >= 1, 1));
	

	//TODO: The summary timing plot to distinguish pp collisions against beam gas & splash
	/*
	 //we find the minimum time difference between hits on side A and side C
	 for (int i = 0; i < nhits_A; i++)
	 for (int j = 0; j < nhits_C; j++)
	 if (abs(time_A[i] - time_C[j]) < t_diff)
	 t_diff = abs(time_A[i] - time_C[j]);

	 bcmTime_AC->Fill(t_diff);
	 */
	return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasMonTool::fillHLTMbtsInfo()
{
        StatusCode sc = StatusCode::SUCCESS;
        
        // Energy Time
        
        unsigned int triggerWord = 0;
        unsigned int timeWord = 0;

        std::vector<float> mbtsHitEnergies; // energy (in pC) of signal in a counter (relative to IP), vector for all counters
        std::vector<float> mbtsHitTimes; // time of signal in counter (relative to IP), vector for all counters        

        int time_ebaCounters=0, time_ebcCounters=0;
        int energy_ebaCounters=0, energy_ebcCounters=0;
        float timeMean_A = 0.;
        float timeMean_C = 0.;
        float timeMin_A = 10000.;
        float timeMin_C = 10000.;
        float energyMean_A = 0.;
        float energyMean_C = 0.;
        float energyMax_A = -10000.;
        float energyMax_C = -10000.;

        //==============================================================================
        // MBTS BITS INFORMATION
        //==============================================================================
        
        const xAOD::TrigT2MbtsBitsContainer* mbTScont = 0;
        sc = m_storeGate->retrieve(mbTScont, m_t2mbtsContainerName);

        if (sc.isFailure() || mbTScont->empty()) {
                if (sc.isFailure())
                        (*m_log) << MSG::WARNING << "Failed to retrieve MbtsMinBiasMonTool for xAOD::TrigT2MbtsBitsContainer" << endreq;
                else
                        (*m_log) << MSG::DEBUG << "xAOD::TrigT2MbtsBitsContainer is empty." << endreq;
        } else {
                (*m_log) << MSG::DEBUG << " ====== START MbtsMinBiasMonTool for xAOD::TrigT2MbtsBitsContainer ====== " << endreq;

                // Loop over EF TrigMinBias objects
                xAOD::TrigT2MbtsBitsContainer::const_iterator mbTS_coll_itr = mbTScont->begin();
                for (; mbTS_coll_itr != mbTScont->end(); ++mbTS_coll_itr) {

                        xAOD::TrigT2MbtsBits *mbtsFeature = (*mbTS_coll_itr);

                        mbtsHitEnergies = mbtsFeature->triggerEnergies();
                        mbtsHitTimes = mbtsFeature->triggerTimes();

                        if (mbtsHitTimes.size() != 32)
                                (*m_log) << MSG::ERROR << "MBTS Cell Times are stored incorrectly. The array should have 32 elements."
                                                << endreq;

                        if (mbtsHitEnergies.size() != 32)
                                (*m_log) << MSG::ERROR << "MBTS Cell Energies are stored incorrectly. The array should have 32 elements."
                                                << endreq;

                        //The energy-dependent bitmask is produced below
                        for (std::size_t k = 0; k < mbtsHitEnergies.size() && k < 32; k++)
                                if (mbtsHitEnergies.at(k) > m_energyCut) {
                                        triggerWord += (1 << k);
                                        // Online distributions
                                        const char* cell_name = (m_moduleLabel[k]).data();
                                        if (k < 16) { // A side
                                               hist("Occupancy_Online - side A")->Fill(cell_name, 1.0);
                                               timeMean_A += mbtsHitTimes.at(k); time_ebaCounters++;
                                               if (mbtsHitTimes.at(k) < timeMin_A) timeMin_A = mbtsHitTimes.at(k);
                                               if (k == 15) {  
                                                     hist("m_h_timeMin_A")->Fill(timeMin_A);
                                                     if (time_ebaCounters> 0) {
                                                          timeMean_A /= time_ebaCounters;
                                                          hist("m_h_timeMean_A")->Fill(timeMean_A);
                                                     } 
                                                     else {
                                         	           timeMean_A = -999.0;
                                                     }
                                               }
                                        } else { // C side 
                                               hist("Occupancy_Online - side C")->Fill(cell_name, 1.0);
                                               timeMean_C += mbtsHitTimes.at(k); time_ebcCounters++;
                                               if (mbtsHitTimes.at(k) < timeMin_C) timeMin_C = mbtsHitTimes.at(k);
                                               if (k == 31) {
                                                    hist("m_h_timeMin_C")->Fill(timeMin_C);
                                                    if (time_ebcCounters> 0) {
                                                         timeMean_C /= time_ebcCounters;
                                                         hist("m_h_timeMean_C")->Fill(timeMean_C);
                                                    } 
                                                    else {
                                         	         timeMean_C = -999.0;
                                                    }
                                               }
                                        }
                                        //Time online
                                        hist("m_h_time_onl")->Fill(mbtsHitTimes.at(k));                                        
                                }
                        //The time-dependent bitmask is produced for the case
                        //of time-dependent L2 trigger
                        for (std::size_t k = 0; k < mbtsHitTimes.size() && k < 32; k++) {
                                if (fabs(mbtsHitTimes.at(k)) < m_timeCut || m_timeCut <= 0.0)
                                        timeWord += (1 << k);

                                // Online distributions
                                //const char* cell_name = (m_moduleLabel[k]).data();

                                if (k < 16) { // A side
                                       energyMean_A += mbtsHitEnergies.at(k); energy_ebaCounters++;
                                       if (mbtsHitEnergies.at(k) > energyMax_A) energyMax_A = mbtsHitEnergies.at(k);
                                       if (k == 15) {
                                              hist("m_h_energyMax_A")->Fill(energyMax_A);                                             
                                              if (energy_ebaCounters> 0) {
                                                      energyMean_A /= energy_ebaCounters;
                                                      hist("m_h_energyMean_A")->Fill(energyMean_A);
                                              } 
                                              else {
                                         	      energyMean_A = -999.0;
                                              }
                                       }
                                } else { // C side 
                                       energyMean_C += mbtsHitEnergies.at(k); energy_ebcCounters++;
                                       if (mbtsHitEnergies.at(k) > energyMax_C) energyMax_C = mbtsHitEnergies.at(k);
                                       if (k == 31)  {
                                             hist("m_h_energyMax_C")->Fill(energyMax_C);
                                             if ( energy_ebcCounters> 0) {
                                                     energyMean_C /= energy_ebcCounters;
                                                     hist("m_h_energyMean_C")->Fill(energyMean_C);
                                             } 
                                             else {
                                                     energyMean_C = -999.0;
                                             }
                                       }
                                }
                                //Energy online
                                hist("m_h_energy_onl")->Fill(mbtsHitEnergies.at(k));                                 
                        }
                }
        } // end TrigT2MbtsBitsContainer.h    
        
        return sc;
}

StatusCode HLTMinBiasMonTool::fillMbtsInfo(const std::string &item)
{
        unsigned int triggerWord = 0;
	unsigned int timeWord = 0;
	const unsigned int mask = 65535; // 2^16 - 1
	int counter = 0;

	unsigned int innerMBTS_EBA = 0;
	unsigned int innerMBTS_EBC = 0;
	unsigned int outerMBTS_EBA = 0;
	unsigned int outerMBTS_EBC = 0;

	unsigned int innerMBTS_EBA_NoCut_Time = 10; // it can not be larger than 8
	unsigned int innerMBTS_EBC_NoCut_Time = 0;
	unsigned int outerMBTS_EBA_NoCut_Time = 0;
	unsigned int outerMBTS_EBC_NoCut_Time = 0;

	unsigned int total_hits_NoCut_Time = 0;
	//unsigned int sideA_hits_NoCut_Time = 0;
	//unsigned int sideC_hits_NoCut_Time = 0;

	//unsigned int total_hits = 0;
	unsigned int sideA_hits = 0;
	unsigned int sideC_hits = 0;

	std::vector<float> mbtsHitEnergies; // energy (in pC) of signal in a counter (relative to IP), vector for all counters
	std::vector<float> mbtsHitTimes; // time of signal in counter (relative to IP), vector for all counters
	
	//==============================================================================
	// MBTS BITS INFORMATION
	//==============================================================================
	const xAOD::TrigT2MbtsBitsContainer* mbTScont = 0;
	StatusCode sc = m_storeGate->retrieve(mbTScont, m_t2mbtsContainerName);

	if (sc.isFailure() || mbTScont->empty()) {
		if (sc.isFailure())
			(*m_log) << MSG::WARNING << "Failed to retrieve MbtsMinBiasMonTool for xAOD::TrigT2MbtsBitsContainer" << endreq;
		else
			(*m_log) << MSG::DEBUG << "xAOD::TrigT2MbtsBitsContainer is empty." << endreq;
	} else {
		(*m_log) << MSG::DEBUG << " ====== START MbtsMinBiasMonTool for xAOD::TrigT2MbtsBitsContainer ====== " << endreq;

		// Loop over EF TrigMinBias objects
		xAOD::TrigT2MbtsBitsContainer::const_iterator mbTS_coll_itr = mbTScont->begin();
		for (; mbTS_coll_itr != mbTScont->end(); ++mbTS_coll_itr) {

			xAOD::TrigT2MbtsBits *mbtsFeature = (*mbTS_coll_itr);

			mbtsHitEnergies = mbtsFeature->triggerEnergies();
			mbtsHitTimes = mbtsFeature->triggerTimes();

			if (mbtsHitTimes.size() != 32)
				(*m_log) << MSG::ERROR << "MBTS Cell Times are stored incorrectly. The array should have 32 elements."
						<< endreq;

			if (mbtsHitEnergies.size() != 32)
				(*m_log) << MSG::ERROR << "MBTS Cell Energies are stored incorrectly. The array should have 32 elements."
						<< endreq;

			//The energy-dependent bitmask is produced below 
			for (std::size_t k = 0; k < mbtsHitEnergies.size() && k < 32; k++)
				if (mbtsHitEnergies.at(k) > m_energyCut) {
					triggerWord += (1 << k);
					// Online distributions
					const char* cell_name = (m_moduleLabel[k]).data();
					hist("Occupancy_Online")->Fill(cell_name, 1.0);                        
					hist2("m_h_time_Online")->Fill(cell_name, mbtsHitTimes.at(k), 1.0);         
				}

			//The time-dependent bitmask is produced for the case 
			//of time-dependent L2 trigger
			for (std::size_t k = 0; k < mbtsHitTimes.size() && k < 32; k++) {
				if (fabs(mbtsHitTimes.at(k)) < m_timeCut || m_timeCut <= 0.0)
					timeWord += (1 << k);

				// Online distributions
				const char* cell_name = (m_moduleLabel[k]).data();
				hist2("m_h_energy_Online")->Fill(cell_name, mbtsHitEnergies.at(k), 1.0);            
			}

			timeWord &= triggerWord;

			//This are the multiplicities without the time cut-offs
			//TODO: The calculations can be simplified significantly

			std::bitset<8> InnerEbaBitset_NoCut_Time(triggerWord & mask);
			std::bitset<8> OuterEbaBitset_NoCut_Time((triggerWord >> 8) & mask);
			std::bitset<8> InnerEbcBitset_NoCut_Time((triggerWord >> 16) & mask);
			std::bitset<8> OuterEbcBitset_NoCut_Time((triggerWord >> 24) & mask);

			innerMBTS_EBA_NoCut_Time = InnerEbaBitset_NoCut_Time.count();
			outerMBTS_EBA_NoCut_Time = OuterEbaBitset_NoCut_Time.count();
			innerMBTS_EBC_NoCut_Time = InnerEbcBitset_NoCut_Time.count();
			outerMBTS_EBC_NoCut_Time = OuterEbcBitset_NoCut_Time.count();

			total_hits_NoCut_Time = innerMBTS_EBA_NoCut_Time + outerMBTS_EBA_NoCut_Time + innerMBTS_EBC_NoCut_Time
					+ outerMBTS_EBC_NoCut_Time;
			//sideA_hits_NoCut_Time = innerMBTS_EBA_NoCut_Time + outerMBTS_EBA_NoCut_Time;
			//sideC_hits_NoCut_Time = innerMBTS_EBC_NoCut_Time + outerMBTS_EBC_NoCut_Time;

			std::bitset<8> InnerEbaBitset(triggerWord & timeWord & mask);
			std::bitset<8> OuterEbaBitset(((triggerWord & timeWord) >> 8) & mask);
			std::bitset<8> InnerEbcBitset(((triggerWord & timeWord) >> 16) & mask);
			std::bitset<8> OuterEbcBitset(((triggerWord & timeWord) >> 24) & mask);

			innerMBTS_EBA = InnerEbaBitset.count();
			outerMBTS_EBA = OuterEbaBitset.count();
			innerMBTS_EBC = InnerEbcBitset.count();
			outerMBTS_EBC = OuterEbcBitset.count();

			//total_hits = innerMBTS_EBA + outerMBTS_EBA + innerMBTS_EBC + outerMBTS_EBC;
			sideA_hits = innerMBTS_EBA + outerMBTS_EBA;
			sideC_hits = innerMBTS_EBC + outerMBTS_EBC;

			//Filling Histograms

                        hist("mbMbts_N_N_hits")->Fill(TMath::Min(sideA_hits, sideC_hits));               
			hist("TriggerRate")->Fill(/*item.c_str(),*/ 1);                                      
			hist("TriggerRatePS")->Fill(/*item.c_str(),*/ m_tdthandle->getChainGroup(item.c_str())->getPrescale());       
			hist("mbMbts_hits")->Fill(total_hits_NoCut_Time);  
			
			hist2("m_h_mbtsCorr_N_N")->Fill(sideA_hits, sideC_hits, 1);                        
		}
	} // end TrigT2MbtsBitsContainer.h


	//==============================================================================
	// CELL LEVEL INFORMATION
	//==============================================================================
	//Retrieve MBTS container collection from SG

	const TileCellContainer* theMBTScontainer;
	sc = m_storeGate->retrieve(theMBTScontainer, m_mbtsContainerName);
	if (sc.isFailure() || theMBTScontainer->empty()) {
		if (sc.isFailure())
			(*m_log) << MSG::WARNING << "Cannot find TileCellContainer object with name " << m_mbtsContainerName << " in TDS"
					<< endreq;
		else
			(*m_log) << MSG::DEBUG << "TileCellContainer is empty." << endreq;
	} else {
		(*m_log) << MSG::VERBOSE << "Retrieval of MBTS container " << m_mbtsContainerName << " succeeded" << endreq;

		double quality;
		double energy[32], time[32];
		memset(energy, 0, sizeof(energy));
		memset(time, 0, sizeof(time));

		bool sim_bit = false;
		bool trigger_bit = false;

		bool sim_time_bit = false;
		bool time_bit = false;

		TileCellContainer::const_iterator iCell = theMBTScontainer->begin();
		TileCellContainer::const_iterator lastCell = theMBTScontainer->end();

		//We process the cell information only if the T2MbtsFex was ran
		for (; iCell != lastCell; ++iCell) {

			const TileCell* mbtsCell = *iCell; // pointer to cell object
			if (mbtsCell != 0) {
				Identifier id = mbtsCell->ID();

				// Calculate MBTS counter from "side", "tower" and "module"
				// Counter goes 0-31.
				// EBA-inner:0-7,EBA-outer:8-15,EBC-inner:16-23,EBC-outer:24-31
				// tower is 1 for outer counter (lower eta) and 0 for inner counter (higher eta)
				// module counts from 0-7 in increasing phi
				// side is -1 for EBC 1 for EBA
				//int counter = (m_tileID->module(id)*2+1) - m_tileID->tower(id);

				counter = (m_tileID->module(id)) + 8 * (m_tileID->tower(id));
				if (m_tileID->side(id) < 0) // EBC side
					counter += 16;

				//the counter should not go outside of the range: [0,31]
				if (counter < 0 || counter > 31) {
					(*m_log) << MSG::WARNING << "MBTS Cell ID is incorrect. The cell is skipped. " << endreq;
					continue;
				}

				energy[counter] = mbtsCell->energy();
				time[counter] = mbtsCell->time();
				quality = mbtsCell->quality();

				const char* cell_name = (m_moduleLabel[counter]).data();

				if (msgSvc()->outputLevel(name()) < MSG::DEBUG) {
					(*m_log) << MSG::VERBOSE << "Counter: " << counter << endreq;
					(*m_log) << MSG::VERBOSE << "Energy= " << energy[counter] << " pCb" << endreq;
					(*m_log) << MSG::VERBOSE << "Time= " << time[counter] << endreq;
					(*m_log) << MSG::VERBOSE << "Quality= " << quality << endreq;
				}

				hist2("m_h_energy")->Fill(cell_name, energy[counter], 1.0);      

				//The simulated trigger dicision
				sim_bit = (energy[counter] > m_energyCut);

				//The simulated trigger time dicision
				sim_time_bit = ((fabs(time[counter]) < m_timeCut) || (m_timeCut <= 0.0));

				if (sim_bit) {
					hist("Occupancy")->Fill(cell_name, 1.0);            
					hist2("m_h_time")->Fill(cell_name, time[counter], 1.0);  
				}

				//The histograms are filled only when the L2 MbtsFex was ran
				if (innerMBTS_EBA_NoCut_Time < 9) {
					//L2 trigger bit
					trigger_bit = (((triggerWord >> counter) & 1) == 1);

					//L2 time bit
					time_bit = (((timeWord >> counter) & 1) == 1);

					if (sim_bit && !trigger_bit)
						hist2("m_h_time_TF_SP")->Fill(cell_name, energy[counter], 1.0);        

					if (!sim_bit && trigger_bit)
						hist2("m_h_time_TP_SF")->Fill(cell_name, energy[counter], 1.0);        

					profile("MbtsEnergyErrors")->Fill(cell_name, error_bit(sim_bit, trigger_bit)); 

					//Timing information is stored only for cells with E > threshold @ L2:
					if (trigger_bit && sim_bit) {
						if (!time_bit && sim_time_bit)
							hist2("m_h_time_TF_SP")->Fill(cell_name, time[counter], 1.0);    

						if (time_bit && !sim_time_bit)
							hist2("m_h_time_TP_SF")->Fill(cell_name, time[counter], 1.0);    

						profile("MbtsTimeErrors")->Fill(cell_name, error_bit(time_bit, sim_time_bit)); 
					}
				} //require L2 Trigger

			} // end if cell not empty
		} // end Cell loop


	} // End of processing of TileCellContainer


	return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasMonTool::fillSpacePointInfo(const std::string &item)
{
        StatusCode sc = StatusCode::SUCCESS;

        pixSpBarr = 0;
	pixSpECA = 0;
	pixSpECC = 0;
	sctSpBarr = 0;
	sctSpECA = 0;
	sctSpECC = 0;

	const xAOD::TrigSpacePointCountsContainer* mbSPcont = 0;
	StatusCode sc_mbsp = m_storeGate->retrieve(mbSPcont, m_spContainerName);

	if (sc_mbsp.isFailure() || mbSPcont->empty()) {
		if (sc_mbsp.isFailure())
			(*m_log) << MSG::WARNING << "Failed to retrieve HLT MinBias for xAOD::TrigSpacePointCountsContainer" << endreq;
		else
			(*m_log) << MSG::DEBUG << "xAOD::TrigSpacePointCountsContainer is empty." << endreq;
	} else {
		(*m_log) << MSG::DEBUG << " ====== START HLTMinBias MonTool for xAOD::TrigSpacePointCountsContainer ====== " << endreq;

		(*m_log) << MSG::DEBUG << "space point container has " << mbSPcont->size() << " entries." << endreq;

		// Loop over TrigMinBias feature objects
		xAOD::TrigSpacePointCountsContainer::const_iterator mbSP_coll_itr = mbSPcont->begin();
		xAOD::TrigSpacePointCountsContainer::const_iterator mbSP_coll_itrE = mbSPcont->end();

		for (; mbSP_coll_itr != mbSP_coll_itrE; ++mbSP_coll_itr) {
			xAOD::TrigSpacePointCounts *id_mbFeature = (*mbSP_coll_itr);
			/*if (id_mbFeature->pixelClusBarrel().nbins_x() > 0 && id_mbFeature->pixelClusBarrel().nbins_y() > 0) {
				pixSpBarr = (int) id_mbFeature->pixelClusBarrel().sumEntries(m_timeOverThresholdCut, 0.,
						TrigHistoCutType::ABOVE_X_ABOVE_Y);
				hist("pixBarr_sp")->Fill(pixSpBarr);
			} else {
				(*m_log) << MSG::WARNING << "Histogram pixelClusBarrel() is not initialized properly; it has 0 bins in X or Y."
						<< endreq;

			}

			if (id_mbFeature->pixelClusEndcapA().nbins_x() > 0 && id_mbFeature->pixelClusEndcapA().nbins_y() > 0) {
				pixSpECA = (int) id_mbFeature->pixelClusEndcapA().sumEntries(m_timeOverThresholdCut, 0.,
						TrigHistoCutType::ABOVE_X_ABOVE_Y);
				hist("pixECA_sp")->Fill(pixSpECA);
			} else {
				(*m_log) << MSG::WARNING
						<< "Histogram pixelClusEndcapA() is not initialized properly; it has 0 bins in X or Y." << endreq;

			}

			if (id_mbFeature->pixelClusEndcapC().nbins_x() > 0 && id_mbFeature->pixelClusEndcapC().nbins_y() > 0) {
				pixSpECC = (int) id_mbFeature->pixelClusEndcapC().sumEntries(m_timeOverThresholdCut, 0.,
						TrigHistoCutType::ABOVE_X_ABOVE_Y);
				hist("pixECC_sp")->Fill(pixSpECC);

			} else {
				(*m_log) << MSG::WARNING
						<< "Histogram pixelClusEndcapC()  is not initialized properly; it has 0 bins in X or Y." << endreq;
			}*/
                        
			sctSpBarr = (int) id_mbFeature->sctSpBarrel();
			sctSpECA = (int) id_mbFeature->sctSpEndcapA();
			sctSpECC = (int) id_mbFeature->sctSpEndcapC();
			
			hist("sctBarr_sp")->Fill(sctSpBarr);
			hist("sctECA_sp")->Fill(sctSpECA);
			hist("sctECC_sp")->Fill(sctSpECC);
                        
			hist("h_pixTot")->Fill(pixSpBarr + pixSpECA + pixSpECC);
			hist("h_sctTot")->Fill(sctSpBarr + sctSpECA + sctSpECC);
		}

	} // end TrigSpacePointCounts

	//Now we can form a proper L2 simulated decision
        
        bool sim = false;
	if (m_triggerTypeAND) {
                sim = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) && ((sctSpBarr + sctSpECA + sctSpECC) > m_totalSctSp_cut);
	} else {
		sim  = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) || ((sctSpBarr + sctSpECA + sctSpECC) > m_totalSctSp_cut);
	}
         
	if (!sim) { //Trigger mismatch; error
		profile("IDMinBiasTriggerErrors", "HLT/MinBiasMon/IDMinbias")->Fill(item.c_str(), 1.0);
	} else {
		profile("IDMinBiasTriggerErrors", "HLT/MinBiasMon/IDMinbias")->Fill(item.c_str(), 0.0);
	}
       
        return sc;
}

StatusCode HLTMinBiasMonTool::fillTrackingInfo()
{
        StatusCode sc = StatusCode::SUCCESS;

	// EF ID (it is the same for trt and pixel+sct)
	const xAOD::TrigTrackCountsContainer* mbTTcont = 0;
	StatusCode sc_mbtt = m_storeGate->retrieve(mbTTcont, m_tcContainerName);

	if (sc_mbtt.isFailure() || mbTTcont->empty()) {
		if (sc_mbtt.isFailure())
			(*m_log) << MSG::WARNING << "Failed to retrieve HLT MinBias MonTool for xAOD::TrigTrackCountsContainer" << endreq;
		else
			(*m_log) << MSG::DEBUG << "xAOD::TrigTrackCountsContainer is empty." << endreq;

	} else {
		(*m_log) << MSG::DEBUG << " ====== START HLTMinBias MonTool for xAOD::TrigTrackCountsContainer ====== " << endreq;

		mbTracks = 0;
		totpix_spEF = 0;
		totsct_spEF = 0;

		// Loop over EF TrigMinBias objects
		xAOD::TrigTrackCountsContainer::const_iterator mbTT_coll_itr = mbTTcont->begin();
		for (; mbTT_coll_itr != mbTTcont->end(); ++mbTT_coll_itr) {
			xAOD::TrigTrackCounts *mbTT = (*mbTT_coll_itr);

			/*if (mbTT->z0_pt().nbins_x() > 0 && mbTT->z0_pt().nbins_y() > 0) {
				mbTracks = (int) (mbTT->z0_pt().sumEntries(m_max_z0, m_min_pt, TrigHistoCutType::BELOW_X_ABOVE_Y));
				hist("minbiasTracks")->Fill(mbTracks);
			} else {
				(*m_log) << MSG::WARNING << "The trigger histogram z0_pt is not initialized properly; it has 0 bins in X or Y."
						<< endreq;
			}*/

		}
	} // end TrigTrackCounts

        return sc;
}

//USUNIĘCIE ZBEDNEGO KODU (STARE ROZWIAZANIE) POWINNO ZMNIEJSZYC PLIK DO PONIZEJ 800 LINII KODU

StatusCode HLTMinBiasMonTool::proc()
{
        StatusCode sc = StatusCode::SUCCESS;
        
        return sc;
}

/*---------------------------------------------------------*/
HLTMinBiasMonTool::~HLTMinBiasMonTool()
/*---------------------------------------------------------*/
{
	if (m_log)
		delete m_log;
}

/*---------------------------------------------------------*/
StatusCode HLTMinBiasMonTool::bookHistogramsRecurrent()//suppress 'unused' compiler warning
/*---------------------------------------------------------*/
{
	StatusCode sc = StatusCode::SUCCESS;

	(*m_log) << MSG::DEBUG << "book being called" << endreq;

//	if (isNewRun) {
	if (newRun) {

                m_numberOfEvents=0;

//		MonGroup m_MonGroup(this, "HLT/MinBiasMon/IDMinbias", shift, run);
		MonGroup m_MonGroup(this, "HLT/MinBiasMon/IDMinbias",run,ATTRIB_UNMANAGED);

		// L2, ID
		m_pixBarr_sp = new TH1I("pixBarr_sp", "number of L2 pixel spacepoints in the barrel;;entries", 250, -0.5, 2499.5);
		sc = m_MonGroup.regHist(m_pixBarr_sp);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		m_pixECA_sp = new TH1I("pixECA_sp", "number of L2 pixel spacepoints in ECA;;entries", 250, -0.5, 2499.5);
		sc = m_MonGroup.regHist(m_pixECA_sp);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		m_pixECC_sp = new TH1I("pixECC_sp", "number of L2 pixel spacepoints in ECC;;entries", 250, -0.5, 2499.5);
		sc = m_MonGroup.regHist(m_pixECC_sp);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		m_sctBarr_sp = new TH1I("sctBarr_sp", "number of L2 sct spacepoints in the barrel;;entries", 300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(m_sctBarr_sp);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		m_sctECA_sp = new TH1I("sctECA_sp", "number of L2 sct spacepoints in ECA;;entries", 300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(m_sctECA_sp);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		m_sctECC_sp = new TH1I("sctECC_sp", "number of L2 sct spacepoints in ECC;;entries", 300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(m_sctECC_sp);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		// EF, ID
		m_minbiasTracks = new TH1I("minbiasTracks", "number of minbias tracks;# of tracks; entries", 125, -0.5, 249.5);
		sc = m_MonGroup.regHist(m_minbiasTracks);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		//L2_mbMbts_1_eff trigger distributions
		// no mbMbts_1_eff
/* 
		h_mbMbts_1_eff_pixTot_P = new TH1I("h_mbMbts_1_eff_pixTot_P",
				"L2_mbMbts_1_eff passed;total number of L2 pixel space-points;entries", 250, -0.5, 2499.5);
		sc = m_MonGroup.regHist(h_mbMbts_1_eff_pixTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_mbMbts_1_eff_sctTot_P = new TH1I("h_mbMbts_1_eff_sctTot_P",
				"L2_mbMbts_1_eff passed;total number of L2 SCT space-points;entries", 300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_mbMbts_1_eff_sctTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
*/

// mbMbtS_2_eff instead of mbMbtS_1_eff
		//L2_mbMbts_2_eff trigger distributions
		h_mbMbts_2_eff_pixTot_P = new TH1I("h_mbMbts_2_eff_pixTot_P",
				"L2_mbMbts_2_eff passed;total number of L2 pixel space-points;entries", 250, -0.5, 2499.5);
		sc = m_MonGroup.regHist(h_mbMbts_2_eff_pixTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_mbMbts_2_eff_sctTot_P = new TH1I("h_mbMbts_2_eff_sctTot_P",
				"L2_mbMbts_2_eff passed;total number of L2 SCT space-points;entries", 300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_mbMbts_2_eff_sctTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}


// 8 teV low-mu Lucid
// changed for pA: mbLucid -> mbLucid_a_c, pA 2013 changed back to mbLucid 

		//L2_mbLucid_eff trigger distributions
		h_mbLucid_eff_pixTot_P = new TH1I("h_mbLucid_eff_pixTot_P",
				"L2_mbLucid_eff passed;total number of L2 pixel space-points;entries", 250, -0.5, 2499.5);
		//		"L2_mbLucid_a_c_eff passed;total number of L2 pixel space-points;entries", 250, -0.5, 2499.5);
		sc = m_MonGroup.regHist(h_mbLucid_eff_pixTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_mbLucid_eff_sctTot_P = new TH1I("h_mbLucid_eff_sctTot_P",
				"L2_mbLucid_eff passed;total number of L2 SCT space-points;entries", 300, -0.5, 2999.5);
		//		"L2_mbLucid_a_c_eff passed;total number of L2 SCT space-points;entries", 300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_mbLucid_eff_sctTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

//

		//L2_mbZdc_eff trigger distributions
		h_mbZdc_eff_pixTot_P = new TH1I("h_mbZdc_eff_pixTot_P",
				"L2_mbZdc_a_c_eff passed;total number of L2 pixel space-points;entries", 250, -0.5, 2499.5);
		//		"L2_mbZdc_eff passed;total number of L2 pixel space-points;entries", 250, -0.5, 2499.5);
		sc = m_MonGroup.regHist(h_mbZdc_eff_pixTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_mbZdc_eff_sctTot_P = new TH1I("h_mbZdc_eff_sctTot_P",
				"L2_mbZdc_a_c_eff passed;total number of L2 SCT space-points;entries", 300, -0.5, 2999.5);
		//		"L2_mbZdc_eff passed;total number of L2 SCT space-points;entries", 300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_mbZdc_eff_sctTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		//L2_mbRd1_eff trigger distributions
		h_mbRd1_eff_pixTot_P = new TH1I("h_mbRd1_eff_pixTot_P",
				"L2_mbRd1_eff passed;total number of L2 pixel space-points;entries", 250, -0.5, 2499.5);
		sc = m_MonGroup.regHist(h_mbRd1_eff_pixTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_mbRd1_eff_sctTot_P = new TH1I("h_mbRd1_eff_sctTot_P",
				"L2_mbRd1_eff passed;total number of L2 SCT space-points;entries", 300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_mbRd1_eff_sctTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
/* 
// *** no mbMbts_1_eff ; mbMbts_2_eff instead

		// EF_mbMbts_1_eff
		h_mbMbts_1_eff_trkTot_P = new TH1I("h_mbMbts_1_eff_trkTot_P", "EF_mbMbts_1_eff passed;# of tracks;entries", 50,
				-0.5, 249.5);
		sc = m_MonGroup.regHist(h_mbMbts_1_eff_trkTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
*/
		// EF_mbMbts_2_eff
		h_mbMbts_2_eff_trkTot_P = new TH1I("h_mbMbts_2_eff_trkTot_P", "EF_mbMbts_2_eff passed;# of tracks;entries", 50,
				-0.5, 249.5);
		sc = m_MonGroup.regHist(h_mbMbts_2_eff_trkTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_mbMbts_2_eff_trkTot_F = new TH1I("h_mbMbts_2_eff_trkTot_F", "EF_mbMbts_2_eff failed;# of tracks;entries", 50,
				-0.5, 249.5);
		sc = m_MonGroup.regHist(h_mbMbts_2_eff_trkTot_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

// 8 TeV Low-mu Lucid
//pA: changed mbLucid-> mbLucid_a_c 
//pA 2013: back to mbLucid 

		// EF_mbLucid_eff
		h_mbLucid_eff_trkTot_P = new TH1I("h_mbLucid_eff_trkTot_P", "EF_mbLucid_eff passed;# of tracks;entries", 50, -0.5, 249.5);
		//h_mbLucid_eff_trkTot_P = new TH1I("h_mbLucid_eff_trkTot_P", "EF_mbLucid_a_c_eff passed;# of tracks;entries", 50, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_mbLucid_eff_trkTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
//
		// EF_mbZdc_eff
		h_mbZdc_eff_trkTot_P = new TH1I("h_mbZdc_eff_trkTot_P", "EF_mbZdc_a_c_eff passed;# of tracks;entries", 50, -0.5, 249.5);
		//h_mbZdc_eff_trkTot_P = new TH1I("h_mbZdc_eff_trkTot_P", "EF_mbZdc_eff passed;# of tracks;entries", 50, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_mbZdc_eff_trkTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		// EF_mbZdc_eff
		h_mbZdc_eff_trkTot_F = new TH1I("h_mbZdc_eff_trkTot_F", "EF_mbZdc_a_c_eff failed;# of tracks;entries", 50, -0.5, 249.5);
		//h_mbZdc_eff_trkTot_F = new TH1I("h_mbZdc_eff_trkTot_F", "EF_mbZdc_eff failed;# of tracks;entries", 50, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_mbZdc_eff_trkTot_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		// EF_mbRd1_eff
		h_mbRd1_eff_trkTot_P = new TH1I("h_mbRd1_eff_trkTot_P", "EF_mbRd1_eff passed;# of tracks;entries", 50, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_mbRd1_eff_trkTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		// EF_mbSpTrk -> mbSp
		h_mbSp_trkTot_P = new TH1D("h_mbSp_trkTot_P", "EF_mbSp OR EF_mbSpTrk passed;# of tracks;entries", 125, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_mbSp_trkTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_mbSp_trkTot_F = new TH1D("h_mbSp_trkTot_F", "EF_mbSp, EF_mbSpTrk failed ;# of tracks;entries", 125, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_mbSp_trkTot_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

///*
// moved 
//  with mbSpTrk -> mbSp, mbSp || mbSpTrk , rd0 -> Rd1, 
// mbMbts_2_NoAlg -> L1ItemStreamer_L1_MBTS_2 to pA below 
//
// 8 TeV Low-mu
// rd0 back in pA jan 2013
		// EF_mbSpTrk_trkTot_rd0_filled_NoAlg
		h_mbSpTrk_trkTot_rd0_P = new TH1D("h_mbSpTrk_trkTot_rd0_P", "EF_mbSpTrk & EF_rd0_filled_NoAlg passed;# of tracks;entries", 125, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_mbSpTrk_trkTot_rd0_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_mbSpTrk_trkTot_rd0_F = new TH1D("h_mbSpTrk_trkTot_rd0_F", "EF_mbSpTrk passed, EF_rd0_failed_NoAlg failed ;# of tracks;entries", 125, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_mbSpTrk_trkTot_rd0_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

/*
		// EF_mbSpTrk_trkTot_rd0_filled_NoAlg
		h_mbSpTrk_trkTot_mb2Alg_P = new TH1D("h_mbSpTrk_trkTot_mb2Alg_P", "EF_mbSpTrk & EF_mbMbts_2_NoAlg passed;# of tracks;entries", 125, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_mbSpTrk_trkTot_mb2Alg_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_mbSpTrk_trkTot_mb2Alg_F = new TH1D("h_mbSpTrk_trkTot_mb2Alg_F", "EF_mbSpTrk passed, EF_mbMbts_2_NoAlg failed ;# of tracks;entries", 125, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_mbSpTrk_trkTot_mb2Alg_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
//
*/

// in pp 8 TeV low mu above  was mbSpTrk (-> mbSp) , rd0_filled_NoAlg (Rd1_eff) 
//
// pA (Sept 2012)
		// EF_mbSp_trkTot_mbRd1_eff
		h_mbSp_trkTot_Rd1_P = new TH1D("h_mbSp_trkTot_Rd1_P", "EF_mbSp & EF_mbRd1_eff passed;# of tracks;entries", 125, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_mbSp_trkTot_Rd1_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_mbSp_trkTot_Rd1_F = new TH1D("h_mbSp_trkTot_Rd1_F", "EF_mbSp passed, EF_mbRd1_eff failed ;# of tracks;entries", 125, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_mbSp_trkTot_Rd1_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}


// in pp 8 TeV low mu above  was mbSpTrk (-> mbSp) , rd0_filled_NoAlg (Rd1_eff),  mbMbts_2_NoAlg (-> L1ItemStreamer_L1_MBTS_2)  
		// EF_mbSp_trkTot_mb2
		h_mbSp_trkTot_mb2_P = new TH1D("h_mbSp_trkTot_mb2_P", "EF_mbSp & EF_L1ItemStreamer_L1_MBTS_2 passed;# of tracks;entries", 125, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_mbSp_trkTot_mb2_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_mbSp_trkTot_mb2_F = new TH1D("h_mbSp_trkTot_mb2_F", "EF_mbSp passed, EF_L1ItemStreamer_L1_MBTS_2 failed ;# of tracks;entries", 125, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_mbSp_trkTot_mb2_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
//


/*
// Track efficiency
		h_mbSpTrk_trkTot_Eff = new TH1D("h_mbSpTrk_trkTot_Eff", "EF_mbSpTrk effic ;# of tracks;efficiency", 125, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_mbSpTrk_trkTot_Eff);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
*/              


		//EF High Multiplicity (tracks)
		h_EF_mbSpTrkVtxMh_P = new TH1I("h_EF_mbSpTrkVtxMh_P", "EF_mbSpTrkVtxMh passed;# of tracks;entries", 125, -0.5,
				249.5);
		sc = m_MonGroup.regHist(h_EF_mbSpTrkVtxMh_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_EF_mbSpTrkVtxMh_F = new TH1I("h_EF_mbSpTrkVtxMh_F", "EF_mbSpTrkVtxMh failed;# of tracks;entries", 125, -0.5,
				249.5);
		sc = m_MonGroup.regHist(h_EF_mbSpTrkVtxMh_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_EF_mbSpTrkVtxMh_eff_P = new TH1I("h_EF_mbSpTrkVtxMh_eff_P", "EF_mbSpTrkVtxMh_eff passed;# of tracks;entries",
				200, -0.5, 199.5);
		sc = m_MonGroup.regHist(h_EF_mbSpTrkVtxMh_eff_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		/*
		 h_EF_mbSpTrkVtxMh_eff_F = new TH1I("h_EF_mbSpTrkVtxMh_eff_F", "EF_mbSpTrkVtxMh_eff failed;# of tracks;entries",
		 200, -0.5, 199.5);
		 sc = m_MonGroup.regHist(h_EF_mbSpTrkVtxMh_eff_F);
		 if (sc.isFailure()) {
		 *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
		 return sc;
		 }
		 */


// moved 
//  with mbSpTrk -> mbSp , rd0 -> rd1, mbMbts_2_NoAlg -> L1ItemStreamer_L1_MBTS_2 to pA below 
// rd0 back in pA Jan 2013
/*

		//L2_mbSpTrk trigger distributions

		h_mbSpTrk_pixTot_P = new TH1D("h_mbSpTrk_pixTot_P",
				"L2_mbSpTrk passed;total number of L2 pixel spacepoints;entries", 250, -0.5, 2499.5);
		sc = m_MonGroup.regHist(h_mbSpTrk_pixTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_mbSpTrk_pixTot_F = new TH1D("h_mbSpTrk_pixTot_F",
				"L2_mbSpTrk failed;total number of L2 pixel spacepoints;entries", 250, -0.5, 2499.5);
		sc = m_MonGroup.regHist(h_mbSpTrk_pixTot_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
*/
// 8 TeV Low-mu
		// EF_mbSpTrk_pixTot_rd0_filled_NoAlg
		h_mbSpTrk_pixTot_rd0_P = new TH1D("h_mbSpTrk_pixTot_rd0_P", "EF_mbSpTrk & EF_rd0_filled_NoAlg passed;total number of L2 pixel spacepoints;entries", 250, -0.5, 2499.5);
                sc = m_MonGroup.regHist(h_mbSpTrk_pixTot_rd0_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
		h_mbSpTrk_pixTot_rd0_F = new TH1D("h_mbSpTrk_pixTot_rd0_F", "EF_mbSpTrk passed, EF_rd0_failed_NoAlg failed;total number of L2 pixel spacepoints;entries", 
                           250, -0.5, 2499.5);
                sc = m_MonGroup.regHist(h_mbSpTrk_pixTot_rd0_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

/*
		// EF_mbSpTrk_sctTot_mbMbts_2_NoAlg
		h_mbSpTrk_pixTot_mb2Alg_P = new TH1D("h_mbSpTrk_pixTot_mb2Alg_P", "EF_mbSpTrk & EF_mbMbts_2_NoAlg passed;total number of L2 pixel spacepoints;entries",
                           250, -0.5, 2499.5);
		sc = m_MonGroup.regHist(h_mbSpTrk_pixTot_mb2Alg_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
		h_mbSpTrk_pixTot_mb2Alg_F = new TH1D("h_mbSpTrk_pixTot_mb2Alg_F", "EF_mbSpTrk passed, EF_mbMbts_2_NoAlg failed;total number of L2 pixel spacepoints;entries",
                           250, -0.5, 2499.5);
		sc = m_MonGroup.regHist(h_mbSpTrk_pixTot_mb2Alg_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
*/


// pA  
//  in 8 TeV pp low mu was  mbSpTrk (-> mbSp), rd0 (-> Rd1), mbMbts_2_NoAlg (-> L1ItemStreamer_L1_MBTS_2)  just above

		//L2_mbSp trigger distributions

		h_mbSp_pixTot_P = new TH1D("h_mbSp_pixTot_P",
				"L2_mbSp passed;total number of L2 pixel spacepoints;entries", 250, -0.5, 2499.5);
		sc = m_MonGroup.regHist(h_mbSp_pixTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_mbSp_pixTot_F = new TH1D("h_mbSp_pixTot_F",
				"L2_mbSp failed;total number of L2 pixel spacepoints;entries", 250, -0.5, 2499.5);
		sc = m_MonGroup.regHist(h_mbSp_pixTot_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

//
		// EF_mbSp_pixTot_Rd1_eff
		h_mbSp_pixTot_Rd1_P = new TH1D("h_mbSp_pixTot_Rd1_P", "EF_mbSp & EF_mbRd1_eff passed;total number of L2 pixel spacepoints;entries", 250, -0.5, 2499.5);
                sc = m_MonGroup.regHist(h_mbSp_pixTot_Rd1_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
		h_mbSp_pixTot_Rd1_F = new TH1D("h_mbSp_pixTot_Rd1_F", "EF_mbSp passed, EF_mbRd1_eff failed;total number of L2 pixel spacepoints;entries", 
                           250, -0.5, 2499.5);
                sc = m_MonGroup.regHist(h_mbSp_pixTot_Rd1_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}


		// EF_L1ItemStreamer_L1_MBTS_2
		h_mbSp_pixTot_mb2_P = new TH1D("h_mbSp_pixTot_mb2_P", "EF_mbSp & EF_L1ItemStreamer_L1_MBTS_2 passed;total number of L2 pixel spacepoints;entries",
                           250, -0.5, 2499.5);
		sc = m_MonGroup.regHist(h_mbSp_pixTot_mb2_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
		h_mbSp_pixTot_mb2_F = new TH1D("h_mbSp_pixTot_mb2_F", "EF_mbSp passed, EF_L1ItemStreamer_L1_MBTS_2 failed;total number of L2 pixel spacepoints;entries",
                           250, -0.5, 2499.5);
		sc = m_MonGroup.regHist(h_mbSp_pixTot_mb2_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}



/*
// Pixel Sp. points efficiency
		h_mbSpTrk_pixTot_Eff = new TH1D("h_mbSpTrk_pixTot_Eff", 
				"L2_mbSpTrk effic ;total number of L2 pixel spacepoints;efficiency", 250, -0.5, 2499.5);
		sc = m_MonGroup.regHist(h_mbSpTrk_pixTot_Eff);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
*/
//
/*
// moved 
//  with mbSpTrk -> mbSp , rd0 -> rd1, mbMbts_2_NoAlg -> L1ItemStreamer_L1_MBTS_2 to pA below 

		h_mbSpTrk_sctTot_P = new TH1D("h_mbSpTrk_sctTot_P", "L2_mbSpTrk passed;total number of L2 SCT spacepoints;entries",
				300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_mbSpTrk_sctTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_mbSpTrk_sctTot_F = new TH1D("h_mbSpTrk_sctTot_F", "L2_mbSpTrk failed;total number of L2 SCT spacepoints;entries",
				300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_mbSpTrk_sctTot_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
*/
// rd0 back in use in pA Jan 2013
// 8 TeV Low-mu
		// EF_mbSpTrk_sctTot_rd0_filled_NoAlg
		h_mbSpTrk_sctTot_rd0_P = new TH1D("h_mbSpTrk_sctTot_rd0_P", "EF_mbSpTrk & EF_rd0_filled_NoAlg passed;total number of L2 SCT spacepoints;entries",
                                300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_mbSpTrk_sctTot_rd0_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
		h_mbSpTrk_sctTot_rd0_F = new TH1D("h_mbSpTrk_sctTot_rd0_F", "EF_mbSpTrk passed, EF_rd0_filled_NoAlg failed;total number of L2 SCT spacepoints;entries",
                                300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_mbSpTrk_sctTot_rd0_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

/*
		// EF_mbSpTrk_sctTot_mbMbts_2_NoAlg
		h_mbSpTrk_sctTot_mb2Alg_P = new TH1D("h_mbSpTrk_sctTot_mb2Alg_P", "EF_mbSpTrk & EF_mbMbts_2_NoAlg passed;total number of L2 SCT spacepoints;entries",
                                300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_mbSpTrk_sctTot_mb2Alg_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
		h_mbSpTrk_sctTot_mb2Alg_F = new TH1D("h_mbSpTrk_sctTot_mb2Alg_F", "EF_mbSpTrk passed, EF_mbMbts_2_NoAlg failed;total number of L2 SCT spacepoints;entries",
                                300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_mbSpTrk_sctTot_mb2Alg_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

*/

// pA  
//  in 8 TeV pp low mu was  mbSpTrk (-> mbSp), rd0 (-> rd1), mbMbts_2_NoAlg (-> L1ItemStreamer_L1_MBTS_2)  just above

		h_mbSp_sctTot_P = new TH1D("h_mbSp_sctTot_P", "L2_mbSp passed;total number of L2 SCT spacepoints;entries",
				300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_mbSp_sctTot_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_mbSp_sctTot_F = new TH1D("h_mbSp_sctTot_F", "L2_mbSp failed;total number of L2 SCT spacepoints;entries",
				300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_mbSp_sctTot_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
//
		// EF_mbSp_sctTot_Rd1_eff
		h_mbSp_sctTot_Rd1_P = new TH1D("h_mbSp_sctTot_Rd1_P", "EF_mbSp & EF_mbRd1_eff passed;total number of L2 SCT spacepoints;entries",
                                300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_mbSp_sctTot_Rd1_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
		h_mbSp_sctTot_Rd1_F = new TH1D("h_mbSp_sctTot_Rd1_F", "EF_mbSp passed, EF_mbRd1_eff failed;total number of L2 SCT spacepoints;entries",
                                300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_mbSp_sctTot_Rd1_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}


		// EF_mbSp_sctTot_L1ItemStreamer_L1_MBTS_2
		h_mbSp_sctTot_mb2_P = new TH1D("h_mbSp_sctTot_mb2_P", "EF_mbSp & EF_L1ItemStreamer_L1_MBTS_2 passed;total number of L2 SCT spacepoints;entries",
                                300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_mbSp_sctTot_mb2_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
		h_mbSp_sctTot_mb2_F = new TH1D("h_mbSp_sctTot_mb2_F", "EF_mbSp passed, EF_L1ItemStreamer_L1_MBTS_2 failed;total number of L2 SCT spacepoints;entries",
                                300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_mbSp_sctTot_mb2_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}



//

/*
// SCT Sp. points efficiency
		h_mbSpTrk_sctTot_Eff = new TH1D("h_mbSpTrk_sctTot_Eff", "L2_mbSpTrk effic;total number of L2 SCT spacepoints;efficiency",
				300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_mbSpTrk_sctTot_Eff);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
*/
//L2 (SCT) High Multiplicity
		h_sct_mbSpTrkVtxMh_P = new TH1I("h_sct_mbSpTrkVtxMh_P", "L2_mbSpTrkVtxMh passed;# of L2 SCT spacepoints;entries",
				300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_sct_mbSpTrkVtxMh_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_sct_mbSpTrkVtxMh_F = new TH1I("h_sct_mbSpTrkVtxMh_F", "L2_mbSpTrkVtxMh failed;# of L2 SCT spacepoints;entries",
				300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_sct_mbSpTrkVtxMh_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_sct_mbSpTrkVtxMh_eff_P = new TH1I("h_sct_mbSpTrkVtxMh_eff_P",
				"L2_mbSpTrkVtxMh_eff passed;# of L2 SCT spacepoints;entries", 300, -0.5, 2999.5);
		sc = m_MonGroup.regHist(h_sct_mbSpTrkVtxMh_eff_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		/*
		 //The trigger can not fail
		 h_sct_mbSpTrkVtxMh_eff_F = new TH1I("h_sct_mbSpTrkVtxMh_eff_F",
		 "L2_mbSpTrkVtxMh failed;# of L2 SCT spacepoints;entries", 2000, -0.5, 1999.5);
		 sc = m_MonGroup.regHist(h_sct_mbSpTrkVtxMh_eff_F);
		 if (sc.isFailure()) {
		 *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
		 return sc;
		 }
		 */

		//*****
		// L2 (PIXEL) High Multiplicity
		h_pix_mbSpTrkVtxMh_P = new TH1I("h_pix_mbSpTrkVtxMh_P", "L2_mbSpTrkVtxMh passed;# of L2 pixel spacepoints;entries",
				400, -0.5, 1119.5);
		sc = m_MonGroup.regHist(h_pix_mbSpTrkVtxMh_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_pix_mbSpTrkVtxMh_F = new TH1I("h_pix_mbSpTrkVtxMh_F", "L2_mbSpTrkVtxMh failed;# of L2 pixel spacepoints;entries",
				400, -0.5, 1119.5);
		sc = m_MonGroup.regHist(h_pix_mbSpTrkVtxMh_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_pix_mbSpTrkVtxMh_eff_P = new TH1I("h_pix_mbSpTrkVtxMh_eff_P",
				"L2_mbSpTrkVtxMh_eff passed;# of L2 pixel spacepoints;entries", 400, -0.5, 1119.5);
		sc = m_MonGroup.regHist(h_pix_mbSpTrkVtxMh_eff_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_pix_mbSpTrkVtxMh_eff_F = new TH1I("h_pix_mbSpTrkVtxMh_eff_F",
				"L2_mbSpTrkVtxMh_eff failed;# of L2 pixel spacepoints;entries", 400, -0.5, 1119.5);
		sc = m_MonGroup.regHist(h_pix_mbSpTrkVtxMh_eff_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		//Error rates 
		// the number of bins to be the number of input items
		//m_h_TriggerErrors = new TProfile("TriggerErrors", "Trigger Errors;trigger chain id;error rate", 2, 0, 2, 0, 1);
		//m_h_TriggerErrors = new TProfile("TriggerErrors", "Trigger Errors;trigger chain id;error rate", 6, 0, 6, 0, 1);
		m_h_TriggerErrors = new TProfile("TriggerErrors", "Trigger Errors;trigger chain id;error rate", 23, 0, 23, 0, 1);
      		m_h_TriggerErrors->GetXaxis()->FindBin("EF_mbSpTrk");
		m_h_TriggerErrors->GetXaxis()->FindBin("L2_mbSpTrk");
		m_h_TriggerErrors->GetXaxis()->FindBin("EF_mbSp");
		m_h_TriggerErrors->GetXaxis()->FindBin("L2_mbSp");
		//m_h_TriggerErrors->GetXaxis()->FindBin("EF_mbMbts_1_eff");
		//m_h_TriggerErrors->GetXaxis()->FindBin("L2_mbMbts_1_eff");
		m_h_TriggerErrors->GetXaxis()->FindBin("EF_mbMbts_2_eff");
		m_h_TriggerErrors->GetXaxis()->FindBin("L2_mbMbts_2_eff");
		m_h_TriggerErrors->GetXaxis()->FindBin("EF_mbZdc_a_c_eff");
		m_h_TriggerErrors->GetXaxis()->FindBin("L2_mbZdc_a_c_eff");
		//  High Mult
		m_h_TriggerErrors->GetXaxis()->FindBin("EF_mbSpTrkVtxMh");
		m_h_TriggerErrors->GetXaxis()->FindBin("EF_mbSpTrkVtxMh_eff");
		m_h_TriggerErrors->GetXaxis()->FindBin("L2_mbSpTrkVtxMh_eff");
		m_h_TriggerErrors->GetXaxis()->FindBin("L2_mbSpTrkVtxMh");
// 8 TeV Low-mu
		m_h_TriggerErrors->GetXaxis()->FindBin("EF_mbLucid_eff");
		m_h_TriggerErrors->GetXaxis()->FindBin("L2_mbLucid_eff");
// moved to rd1 and mb2 stream in pA below, rd0 back in use in pA jan 2013
		m_h_TriggerErrors->GetXaxis()->FindBin("EF_rd0_filled_NoAlg");
		m_h_TriggerErrors->GetXaxis()->FindBin("L2_rd0_filled_NoAlg");
		//m_h_TriggerErrors->GetXaxis()->FindBin("EF_mbMbts_2_NoAlg");
		//m_h_TriggerErrors->GetXaxis()->FindBin("L2_mbMbts_2_NoAlg");

// pA
		m_h_TriggerErrors->GetXaxis()->FindBin("EF_mbRd1_eff");
		m_h_TriggerErrors->GetXaxis()->FindBin("L2_mbRd1_eff");
		m_h_TriggerErrors->GetXaxis()->FindBin("EF_L1ItemStreamer_L1_MBTS_2");
		m_h_TriggerErrors->GetXaxis()->FindBin("L2_L1ItemStreamer_L1_MBTS_2");

// pA Jan 2013
		m_h_TriggerErrors->GetXaxis()->FindBin("EF_L1LHCF_NoAlg");
		m_h_TriggerErrors->GetXaxis()->FindBin("EF_L1ZDC_A_NoAlg");
		m_h_TriggerErrors->GetXaxis()->FindBin("EF_L1ZDC_C_NoAlg");

		m_h_TriggerErrors->ResetBit(TH1::kCanRebin);
		m_h_TriggerErrors->SetMinimum(0.0);
		sc = m_MonGroup.regHist(m_h_TriggerErrors);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;

                }



// Energy-Time
// ENERGY

               // MBTS energy
               // The A side mean energy passed the trigger
                m_h_energyMean_A_P = new TH1F("m_h_energyMean_A_P", "Mean MBTS Energy A side passed;MBTS Energy [pC];entries", 100, -5, 95);
                sc = m_MonGroup.regHist(m_h_energyMean_A_P);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}


                //The A side mean energy failed
                m_h_energyMean_A_F = new TH1F("m_h_energyMean_A_F", "Mean MBTS Energy A side failed;MBTS Energy [pC];entries", 100, -5, 95);
                sc = m_MonGroup.regHist(m_h_energyMean_A_F);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}


                //The A side  maximum energy passed the trigger
                m_h_energyMax_A_P = new TH1F("m_h_energyMax_A_P", "Maximum MBTS Energy A side passed;MBTS Energy [ns];[pC];entries", 100, -5, 95);
                sc = m_MonGroup.regHist(m_h_energyMax_A_P);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}

                //The A side maximum energy failed
                m_h_energyMax_A_F = new TH1F("m_h_energyMax_A_F", "Maximum MBTS Energy A side failed;MBTS Energy [pC];entries", 100, -5, 95);
                sc = m_MonGroup.regHist(m_h_energyMax_A_F);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}


                //The C side  mean energy passed the trigger
                m_h_energyMean_C_P = new TH1F("m_h_energyMean_C_P", "Mean MBTS Energy C side passed;MBTS Energy [pC];entries", 100, -5, 95);
                sc = m_MonGroup.regHist(m_h_energyMean_C_P);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}

                //The C side mean energy failed
                m_h_energyMean_C_F = new TH1F("m_h_energyMean_C_F", "Mean MBTS Energy C side failed;MBTS Energy [pC];entries", 100, -5, 95);
                sc = m_MonGroup.regHist(m_h_energyMean_C_F);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}


                //The C side  maximum energy passed the trigger
                m_h_energyMax_C_P = new TH1F("m_h_energyMax_C_P", "Maximum MBTS Energy C side passed;MBTS Energy [pC];entries", 100, -5, 95);
                sc = m_MonGroup.regHist(m_h_energyMax_C_P);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}

                //The C side maximum energy failed
                m_h_energyMax_C_F = new TH1F("m_h_energyMax_C_F", "Maxmum MBTS Energy C side failed;MBTS Energy [pC];entries", 100, -5, 95);
                sc = m_MonGroup.regHist(m_h_energyMax_C_F);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}


    // energy online + L2 , EF passed/failed

                //The  time online
                m_h_energy_1d = new TH1D("m_h_energy_onl", "MBTS Energy  online;MBTS Energy [pC];entries", 100, -5, 95);
                sc = m_MonGroup.regHist(m_h_energy_1d);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}

                //The  energy online L2 passed
                m_h_energy_L2_P = new TH1D("m_h_energy_L2_P", "MBTS Energy online L2 passed;MBTS Energy [pC];entries", 100, -5, 95);
                sc = m_MonGroup.regHist(m_h_energy_L2_P);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}


                //The  energy online L2 failed
                m_h_energy_L2_F = new TH1F("m_h_energy_L2_F", "MBTS Energy online L2 failed;MBTS Energy [pC];entries", 100, -5, 95);
                sc = m_MonGroup.regHist(m_h_energy_L2_F);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}

/*
// energy effic. L2
                //The  energy online L2 passed
                m_h_energy_L2_Eff = new TH1D("m_h_energy_L2_Eff", "MBTS Energy online L2 effic;MBTS Energy [pC];efficiency", 100, -5, 95);
                sc = m_MonGroup.regHist(m_h_energy_L2_Eff);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}
*/

                //The  energy online EF passed
                m_h_energy_EF_P = new TH1F("m_h_energy_EF_P", "MBTS Energy online EF passed;MBTS Energy [pC];entries", 100, -5, 95);
                sc = m_MonGroup.regHist(m_h_energy_EF_P);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}

                //The  energy online EF failed
                m_h_energy_EF_F = new TH1F("m_h_energy_EF_F", "MBTS Energy online EF failed;MBTS Energy [pC];entries", 100, -5, 95);
                sc = m_MonGroup.regHist(m_h_energy_EF_F);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}


// Time
                //=== The time is monitored only if the energy is sufficient to suppress noise
                //The A side  mean time passed the trigger
                m_h_timeMean_A_P = new TH1F("m_h_timeMean_A_P", "Mean MBTS Time A side passed;MBTS Time [ns];entries", 100, -25, 25);
                sc = m_MonGroup.regHist(m_h_timeMean_A_P);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}

                //The A side mean time failed
                m_h_timeMean_A_F = new TH1F("m_h_timeMean_A_F", "Mean MBTS Time A side failed;MBTS Time [ns];entries", 100, -25, 25);
                sc = m_MonGroup.regHist(m_h_timeMean_A_F);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}


                //The A side  minimum time passed the trigger
                m_h_timeMin_A_P = new TH1F("m_h_timeMin_A_P", "Minimum MBTS Time A side passed;MBTS Time [ns];entries", 100, -25, 25);
                sc = m_MonGroup.regHist(m_h_timeMin_A_P);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}

                //The A side minimum time failed
                m_h_timeMin_A_F = new TH1F("m_h_timeMin_A_F", "Minimum MBTS Time A side failed;MBTS Time [ns];entries", 100, -25, 25);
                sc = m_MonGroup.regHist(m_h_timeMin_A_F);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}


                //The C side  mean time passed the trigger
                m_h_timeMean_C_P = new TH1F("m_h_timeMean_C_P", "Mean MBTS Time C side passed;MBTS Time [ns];entries", 100, -25, 25);
                sc = m_MonGroup.regHist(m_h_timeMean_C_P);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}

                //The C side mean time failed
                m_h_timeMean_C_F = new TH1F("m_h_timeMean_C_F", "Mean MBTS Time C side failed;MBTS Time [ns];entries", 100, -25, 25);
                sc = m_MonGroup.regHist(m_h_timeMean_C_F);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}


                //The C side  minimum time passed the trigger
                m_h_timeMin_C_P = new TH1F("m_h_timeMin_C_P", "Minimum MBTS Time C side passed;MBTS Time [ns];entries", 100, -25, 25);
                sc = m_MonGroup.regHist(m_h_timeMin_C_P);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}

                //The C side minimum time failed
                m_h_timeMin_C_F = new TH1F("m_h_timeMin_C_F", "Minimum MBTS Time C side failed;MBTS Time [ns];entries", 100, -25, 25);
                sc = m_MonGroup.regHist(m_h_timeMin_C_F);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}


    // time online + L2 , EF passed/failed

                //The  time online
                m_h_time_1d = new TH1D("m_h_time_onl", "MBTS Time  online;MBTS Time [ns];entries", 100, -25, 25);
                sc = m_MonGroup.regHist(m_h_time_1d);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}

                //The  time online L2 passed
                m_h_time_L2_P = new TH1D("m_h_time_L2_P", "MBTS Time online L2 passed;MBTS Time [ns];entries", 100, -25, 25);
                sc = m_MonGroup.regHist(m_h_time_L2_P);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}

                //The  time online L2 failed
                m_h_time_L2_F = new TH1F("m_h_time_L2_F", "MBTS Time online L2 failed;MBTS Time [ns];entries", 100, -25, 25);
                sc = m_MonGroup.regHist(m_h_time_L2_F);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}


/*
// Timing efficiency (L2) 
                m_h_time_L2_Eff = new TH1D("m_h_time_L2_Eff", "MBTS Time online L2 effic;MBTS Time [ns];efiiciency", 100, -25, 25);
                sc = m_MonGroup.regHist(m_h_time_L2_Eff);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}
*/

                //The  time online EF passed
                m_h_time_EF_P = new TH1F("m_h_time_EF_P", "MBTS Time online EF passed;MBTS Time [ns];entries", 100, -25, 25);
                sc = m_MonGroup.regHist(m_h_time_EF_P);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}

                //The  time online EF failed
                m_h_time_EF_F = new TH1F("m_h_time_EF_F", "MBTS Time online EF failed;MBTS Time [ns];entries", 100, -25, 25);
                sc = m_MonGroup.regHist(m_h_time_EF_F);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
		}


// energy-time cell occupancy
                // side A
                m_h_occupancy_onl_A = new TH1I("Occupancy_Online - side A", "Online MBTS Occupancy (no time cut);channel id;entries", 32, 0,
                                32);
                //fixXaxis( m_h_occupancy_onl_A);
                m_h_occupancy_onl_A->SetMinimum(0.0);
                sc = m_MonGroup.regHist(m_h_occupancy_onl_A);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
                } 


                // side A
                m_h_occupancy_onl_C = new TH1I("Occupancy_Online - side C", "Online MBTS Occupancy (no time cut);channel id;entries", 32, 0,
                                32);
                //fixXaxis( m_h_occupancy_onl_C);
                m_h_occupancy_onl_C->SetMinimum(0.0);
                sc = m_MonGroup.regHist(m_h_occupancy_onl_C);
                if (sc.isFailure()) {
                        *m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
                        return sc;
                } 

//pA 2013 hip_trk due to Sp threshold 
                //PASSED
		m_h_Trig_hip_trk_P = new TH1F("EF_hip_trk_P", "EF_mbSpTrkVtxMh_hip_trk with Sp thresh passed;hip_trk chain id;entries", 9, 0, 9);
      		m_h_Trig_hip_trk_P->GetXaxis()->FindBin("trk150_L1TE50");
      		m_h_Trig_hip_trk_P->GetXaxis()->FindBin("trk190_L1TE50");

      		m_h_Trig_hip_trk_P->GetXaxis()->FindBin("trk120_L1TE35");
      		m_h_Trig_hip_trk_P->GetXaxis()->FindBin("trk190_L1TE35");
      		m_h_Trig_hip_trk_P->GetXaxis()->FindBin("trk240_L1TE35");

      		m_h_Trig_hip_trk_P->GetXaxis()->FindBin("trk180_L1TE65");
      		m_h_Trig_hip_trk_P->GetXaxis()->FindBin("trk200_L1TE65");
      		m_h_Trig_hip_trk_P->GetXaxis()->FindBin("trk240_L1TE65");

		m_h_Trig_hip_trk_P->GetXaxis()->FindBin("trk175_L1MBTS");
		m_h_Trig_hip_trk_P->ResetBit(TH1::kCanRebin);
		//m_h_Trig_hip_trk_P->SetMinimum(0.0);
		m_h_Trig_hip_trk_P->Sumw2();
		sc = m_MonGroup.regHist(m_h_Trig_hip_trk_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
                }
                //FAILED
		m_h_Trig_hip_trk_F = new TH1F("EF_hip_trk_F", "EF_mbSpTrkVtxMh_hip_trk with Sp thresh failed;hip_trk chain id;entries", 9, 0, 9);

      		m_h_Trig_hip_trk_F->GetXaxis()->FindBin("trk150_L1TE50");
      		m_h_Trig_hip_trk_F->GetXaxis()->FindBin("trk190_L1TE50");

      		m_h_Trig_hip_trk_F->GetXaxis()->FindBin("trk120_L1TE35");
      		m_h_Trig_hip_trk_F->GetXaxis()->FindBin("trk190_L1TE35");
      		m_h_Trig_hip_trk_F->GetXaxis()->FindBin("trk240_L1TE35");

      		m_h_Trig_hip_trk_F->GetXaxis()->FindBin("trk180_L1TE65");
      		m_h_Trig_hip_trk_F->GetXaxis()->FindBin("trk200_L1TE65");
      		m_h_Trig_hip_trk_F->GetXaxis()->FindBin("trk240_L1TE65");

		m_h_Trig_hip_trk_F->GetXaxis()->FindBin("trk175_L1MBTS");

		m_h_Trig_hip_trk_F->ResetBit(TH1::kCanRebin);
		//m_h_Trig_hip_trk_F->SetMinimum(0.0);
		m_h_Trig_hip_trk_F->Sumw2();
		sc = m_MonGroup.regHist(m_h_Trig_hip_trk_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
                }

		// EF_L1ZDC_A_NoAlg vs. number of tracks  
		h_L1ZDC_A_NoAlg_trkToT_P  = new TH1I("h_L1ZDC_A_NoAlg_trk_ToT_P", "EF_L1ZDC_A_NoAlg passed;# of tracks;entries", 125, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_L1ZDC_A_NoAlg_trkToT_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_L1ZDC_A_NoAlg_trkToT_F = new TH1I("h_L1ZDC_A_NoAlg_trkToT_F", "EF_L1ZDC_A_NoAlg failed ;# of tracks;entries", 125, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_L1ZDC_A_NoAlg_trkToT_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}


		// EF_L1ZDC_C_NoAlg vs. number of tracks  
		h_L1ZDC_C_NoAlg_trkToT_P  = new TH1I("h_L1ZDC_C_NoAlg_trk_ToT_P", "EF_L1ZDC_C_NoAlg passed;# of tracks;entries", 125, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_L1ZDC_C_NoAlg_trkToT_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_L1ZDC_C_NoAlg_trkToT_F = new TH1I("h_L1ZDC_C_NoAlg_trkToT_F", "EF_L1ZDC_C_NoAlg failed ;# of tracks;entries", 125, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_L1ZDC_C_NoAlg_trkToT_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}
		// EF_L1LHCF_NoAlg vs. number of tracks  
		h_L1LHCF_NoAlg_trkToT_P  = new TH1I("h_L1LHCF_NoAlg_trk_ToT_P", "EF_L1LHCF_NoAlg passed;# of tracks;entries", 125, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_L1LHCF_NoAlg_trkToT_P);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}

		h_L1LHCF_NoAlg_trkToT_F = new TH1I("h_L1LHCF_NoAlg_trkToT_F", "EF_L1LHCF_NoAlg failed ;# of tracks;entries", 125, -0.5, 249.5);
		sc = m_MonGroup.regHist(h_L1LHCF_NoAlg_trkToT_F);
		if (sc.isFailure()) {
			*m_log << MSG::ERROR << "Unable to call regHist() for the current MonGroup" << endreq;
			return sc;
		}


//	} else if (isNewEventsBlock || isNewLumiBlock) {
	} else if (newLumiBlock) {
		return sc;
	}

	return sc;
}

/*---------------------------------------------------------*/
StatusCode HLTMinBiasMonTool::fillHistograms()
/*---------------------------------------------------------*/
{

	*m_log << MSG::DEBUG << " ====== Begin fillHists() ====== " << endreq;
	
	StatusCode fSC = fill();
	if (fSC.isFailure())
	{
	        (*m_log) << MSG::ERROR << "An error in new code occured!" << endreq;
	}

	//we obtain the unprescaled decision @ L1 since the triggers run in PT
	unsigned int L1_bits = TrigDefs::L1_isPassedAfterVeto | TrigDefs::L1_isPassedAfterPrescale
			| TrigDefs::L1_isPassedBeforePrescale;

	bool L1_mbMbts_2_eff = (m_EF_mbMbts_2_eff->isPassedBits() & L1_bits) == L1_bits;
	//bool L1_mbMbts_1_eff = (m_EF_mbMbts_1_eff->isPassedBits() & L1_bits) == L1_bits;
	bool L1_mbMbts_1_1_eff = (m_EF_mbMbts_1_1_eff->isPassedBits() & L1_bits) == L1_bits;
	bool L1_mbMbts_2_2_eff = (m_EF_mbMbts_2_2_eff->isPassedBits() & L1_bits) == L1_bits;
	bool L1_mbMbts_4_4_eff = (m_EF_mbMbts_4_4_eff->isPassedBits() & L1_bits) == L1_bits;
	bool L1_mbZdc_eff = (m_EF_mbZdc_eff->isPassedBits() & L1_bits) == L1_bits;
	bool L1_mbRd1_eff = (m_EF_mbRd1_eff->isPassedBits() & L1_bits) == L1_bits;
	bool L1_mbSpTrk = (m_EF_mbSpTrk->isPassedBits() & L1_bits) == L1_bits;
// 8 TeV Low-mu
	bool L1_mbLucid_eff = (m_EF_mbLucid_eff->isPassedBits() & L1_bits) == L1_bits;

	bool L1_rd0_filled_NoAlg = (m_EF_rd0_filled_NoAlg->isPassedBits() & L1_bits) == L1_bits;
//	bool L1_mbMbts_2_NoAlg = (m_EF_mbMbts_2_NoAlg->isPassedBits() & L1_bits) == L1_bits;
  
// pA       
//	bool L1_mbRd1_eff = is already defined just above 
	bool L1_L1ItemStreamer_L1_MBTS_2 = (m_EF_L1ItemStreamer_L1_MBTS_2->isPassedBits() & L1_bits) == L1_bits;
	bool L1_mbSp = (m_EF_mbSp->isPassedBits() & L1_bits) == L1_bits;

	// High Mult
	bool L1_mbSpTrkVtxMh = (m_EF_mbSpTrkVtxMh->isPassedBits() & L1_bits) == L1_bits;
	bool L1_mbSpTrkVtxMh_eff = (m_EF_mbSpTrkVtxMh_eff->isPassedBits() & L1_bits) == L1_bits;

	//L2 should not be prescaled
	//bool L2_mbMbts_1_eff_ps = (m_EF_mbMbts_1_eff->isPassedBits() & TrigDefs::L2_prescaled) == TrigDefs::L2_prescaled;
	bool L2_mbMbts_2_eff_ps = (m_EF_mbMbts_2_eff->isPassedBits() & TrigDefs::L2_prescaled) == TrigDefs::L2_prescaled;
	bool L2_mbMbts_1_1_eff_ps = (m_EF_mbMbts_1_1_eff->isPassedBits() & TrigDefs::L2_prescaled) == TrigDefs::L2_prescaled;
	bool L2_mbMbts_2_2_eff_ps = (m_EF_mbMbts_2_2_eff->isPassedBits() & TrigDefs::L2_prescaled) == TrigDefs::L2_prescaled;
	bool L2_mbMbts_4_4_eff_ps = (m_EF_mbMbts_4_4_eff->isPassedBits() & TrigDefs::L2_prescaled) == TrigDefs::L2_prescaled;
	bool L2_mbZdc_eff_ps = (m_EF_mbZdc_eff->isPassedBits() & TrigDefs::L2_prescaled) == TrigDefs::L2_prescaled;
	bool L2_mbRd1_eff_ps = (m_EF_mbRd1_eff->isPassedBits() & TrigDefs::L2_prescaled) == TrigDefs::L2_prescaled;
	bool L2_mbSpTrk_ps = (m_EF_mbSpTrk->isPassedBits() & TrigDefs::L2_prescaled) == TrigDefs::L2_prescaled;
// 8 TeV Low mu
	bool L2_mbLucid_eff_ps = (m_EF_mbLucid_eff->isPassedBits() & TrigDefs::L2_prescaled) == TrigDefs::L2_prescaled;

	bool L2_rd0_filled_NoAlg_ps = (m_EF_rd0_filled_NoAlg->isPassedBits() & TrigDefs::L2_prescaled) == TrigDefs::L2_prescaled;
	//bool L2_mbMbts_2_NoAlg_ps = (m_EF_mbMbts_2_NoAlg->isPassedBits() & TrigDefs::L2_prescaled) == TrigDefs::L2_prescaled;
       
// pA 
	//bool L2_mbRd1_eff_ps defined just above
	bool L2_L1ItemStreamer_L1_MBTS_2_ps = (m_EF_L1ItemStreamer_L1_MBTS_2->isPassedBits() & TrigDefs::L2_prescaled) == TrigDefs::L2_prescaled;
	bool L2_mbSp_ps = (m_EF_mbSp->isPassedBits() & TrigDefs::L2_prescaled) == TrigDefs::L2_prescaled;

	//  High Mult
	bool L2_mbSpTrkVtxMh_ps = (m_EF_mbSpTrkVtxMh->isPassedBits() & TrigDefs::L2_prescaled) == TrigDefs::L2_prescaled;
	bool L2_mbSpTrkVtxMh_eff_ps = (m_EF_mbSpTrkVtxMh_eff->isPassedBits() & TrigDefs::L2_prescaled)
			== TrigDefs::L2_prescaled;

	//L2 Should not be in Pass Through 
	//bool L2_mbMbts_1_eff_pt = (m_EF_mbMbts_1_eff->isPassedBits() & TrigDefs::L2_passThrough) == TrigDefs::L2_passThrough;
	bool L2_mbMbts_2_eff_pt = (m_EF_mbMbts_2_eff->isPassedBits() & TrigDefs::L2_passThrough) == TrigDefs::L2_passThrough;
	bool L2_mbMbts_1_1_eff_pt = (m_EF_mbMbts_1_1_eff->isPassedBits() & TrigDefs::L2_passThrough)
			== TrigDefs::L2_passThrough;
	bool L2_mbMbts_2_2_eff_pt = (m_EF_mbMbts_2_2_eff->isPassedBits() & TrigDefs::L2_passThrough)
			== TrigDefs::L2_passThrough;
	bool L2_mbMbts_4_4_eff_pt = (m_EF_mbMbts_4_4_eff->isPassedBits() & TrigDefs::L2_passThrough)
			== TrigDefs::L2_passThrough;
	bool L2_mbZdc_eff_pt = (m_EF_mbZdc_eff->isPassedBits() & TrigDefs::L2_passThrough) == TrigDefs::L2_passThrough;
	bool L2_mbRd1_eff_pt = (m_EF_mbRd1_eff->isPassedBits() & TrigDefs::L2_passThrough) == TrigDefs::L2_passThrough;
	bool L2_mbSpTrk_pt = (m_EF_mbSpTrk->isPassedBits() & TrigDefs::L2_passThrough) == TrigDefs::L2_passThrough;
// 8 TeV Low-mu
	bool L2_mbLucid_eff_pt = (m_EF_mbLucid_eff->isPassedBits() & TrigDefs::L2_passThrough) == TrigDefs::L2_passThrough;

	bool L2_rd0_filled_NoAlg_pt = (m_EF_rd0_filled_NoAlg->isPassedBits() & TrigDefs::L2_passThrough) == TrigDefs::L2_passThrough;
	//bool L2_mbMbts_2_NoAlg_pt = (m_EF_mbMbts_2_NoAlg->isPassedBits() & TrigDefs::L2_passThrough) == TrigDefs::L2_passThrough;
// pA 
	// bool L2_mbRd1_eff_pt  defined just above
	bool L2_L1ItemStreamer_L1_MBTS_2_pt = (m_EF_L1ItemStreamer_L1_MBTS_2->isPassedBits() & TrigDefs::L2_passThrough) == TrigDefs::L2_passThrough;
	bool L2_mbSp_pt = (m_EF_mbSp->isPassedBits() & TrigDefs::L2_passThrough) == TrigDefs::L2_passThrough;

	//  High Mult
	bool L2_mbSpTrkVtxMh_pt = (m_EF_mbSpTrkVtxMh->isPassedBits() & TrigDefs::L2_passThrough) == TrigDefs::L2_passThrough;
	bool L2_mbSpTrkVtxMh_eff_pt = (m_EF_mbSpTrkVtxMh_eff->isPassedBits() & TrigDefs::L2_passThrough)
			== TrigDefs::L2_passThrough;

	//L2 raw decision
	//bool L2_mbMbts_1_eff_pr = (m_EF_mbMbts_1_eff->isPassedBits() & TrigDefs::L2_passedRaw) == TrigDefs::L2_passedRaw;
	bool L2_mbMbts_2_eff_pr = (m_EF_mbMbts_2_eff->isPassedBits() & TrigDefs::L2_passedRaw) == TrigDefs::L2_passedRaw;
	bool L2_mbZdc_eff_pr = (m_EF_mbZdc_eff->isPassedBits() & TrigDefs::L2_passedRaw) == TrigDefs::L2_passedRaw;
	//bool L2_mbRd1_eff_pr = (m_EF_mbRd1_eff->isPassedBits() & TrigDefs::L2_passedRaw) == TrigDefs::L2_passedRaw;
	bool L2_mbSpTrk_pr = (m_EF_mbSpTrk->isPassedBits() & TrigDefs::L2_passedRaw) == TrigDefs::L2_passedRaw;
// 8 TeV Low-mu
// lucid_eff, rd0 back in pA Jan 2013
	//bool L2_mbLucid_eff_pr = (m_EF_mbLucid_eff->isPassedBits() & TrigDefs::L2_passedRaw) == TrigDefs::L2_passedRaw;

	bool L2_rd0_filled_NoAlg_pr = (m_EF_rd0_filled_NoAlg->isPassedBits() & TrigDefs::L2_passedRaw) == TrigDefs::L2_passedRaw;
	//bool L2_mbMbts_2_NoAlg_pr = (m_EF_mbMbts_2_NoAlg->isPassedBits() & TrigDefs::L2_passedRaw) == TrigDefs::L2_passedRaw;

// pA
        bool L2_mbRd1_eff_pr = (m_EF_mbRd1_eff->isPassedBits() & TrigDefs::L2_passedRaw) == TrigDefs::L2_passedRaw;
        bool L2_L1ItemStreamer_L1_MBTS_2_pr = (m_EF_L1ItemStreamer_L1_MBTS_2->isPassedBits() & TrigDefs::L2_passedRaw) == TrigDefs::L2_passedRaw;
	bool L2_mbSp_pr = (m_EF_mbSp->isPassedBits() & TrigDefs::L2_passedRaw) == TrigDefs::L2_passedRaw;
// pA Jan 2013
	bool L2_L1ZDC_A_NoAlg_pr = (m_EF_L1ZDC_A_NoAlg->isPassedBits() & TrigDefs::L2_passedRaw) == TrigDefs::L2_passedRaw;
	bool L2_L1ZDC_C_NoAlg_pr = (m_EF_L1ZDC_C_NoAlg->isPassedBits() & TrigDefs::L2_passedRaw) == TrigDefs::L2_passedRaw;
	bool L2_L1LHCF_NoAlg_pr = (m_EF_L1LHCF_NoAlg->isPassedBits() & TrigDefs::L2_passedRaw) == TrigDefs::L2_passedRaw;


	//  High Mult Raw decision
	bool L2_mbSpTrkVtxMh_pr = (m_EF_mbSpTrkVtxMh->isPassedBits() & TrigDefs::L2_passedRaw) == TrigDefs::L2_passedRaw;
	bool L2_mbSpTrkVtxMh_eff_pr = (m_EF_mbSpTrkVtxMh_eff->isPassedBits() & TrigDefs::L2_passedRaw)
			== TrigDefs::L2_passedRaw;

	//EF should not be prescaled
	//bool EF_mbMbts_1_eff_ps = (m_EF_mbMbts_1_eff->isPassedBits() & TrigDefs::EF_prescaled) == TrigDefs::EF_prescaled;
	bool EF_mbMbts_2_eff_ps = (m_EF_mbMbts_2_eff->isPassedBits() & TrigDefs::EF_prescaled) == TrigDefs::EF_prescaled;
	bool EF_mbMbts_1_1_eff_ps = (m_EF_mbMbts_1_1_eff->isPassedBits() & TrigDefs::EF_prescaled) == TrigDefs::EF_prescaled;
	bool EF_mbMbts_2_2_eff_ps = (m_EF_mbMbts_2_2_eff->isPassedBits() & TrigDefs::EF_prescaled) == TrigDefs::EF_prescaled;
	bool EF_mbMbts_4_4_eff_ps = (m_EF_mbMbts_4_4_eff->isPassedBits() & TrigDefs::EF_prescaled) == TrigDefs::EF_prescaled;
	bool EF_mbZdc_eff_ps = (m_EF_mbZdc_eff->isPassedBits() & TrigDefs::EF_prescaled) == TrigDefs::EF_prescaled;
	bool EF_mbRd1_eff_ps = (m_EF_mbRd1_eff->isPassedBits() & TrigDefs::EF_prescaled) == TrigDefs::EF_prescaled;
	bool EF_mbSpTrk_ps = (m_EF_mbSpTrk->isPassedBits() & TrigDefs::EF_prescaled) == TrigDefs::EF_prescaled;
// 8 TeV Low-mu
	bool EF_mbLucid_eff_ps = (m_EF_mbLucid_eff->isPassedBits() & TrigDefs::EF_prescaled) == TrigDefs::EF_prescaled;
	bool EF_rd0_filled_NoAlg_ps = (m_EF_rd0_filled_NoAlg->isPassedBits() & TrigDefs::EF_prescaled) == TrigDefs::EF_prescaled;
	//bool EF_mbMbts_2_NoAlg_ps = (m_EF_mbMbts_2_NoAlg->isPassedBits() & TrigDefs::EF_prescaled) == TrigDefs::EF_prescaled;

// pA
	//bool EF_mbRd1_eff_ps  is defined just above
	bool EF_L1ItemStreamer_L1_MBTS_2_ps = (m_EF_L1ItemStreamer_L1_MBTS_2->isPassedBits() & TrigDefs::EF_prescaled) == TrigDefs::EF_prescaled;
	bool EF_mbSp_ps = (m_EF_mbSp->isPassedBits() & TrigDefs::EF_prescaled) == TrigDefs::EF_prescaled;

	//  High Mult
	bool EF_mbSpTrkVtxMh_ps = (m_EF_mbSpTrkVtxMh->isPassedBits() & TrigDefs::EF_prescaled) == TrigDefs::EF_prescaled;
	bool EF_mbSpTrkVtxMh_eff_ps = (m_EF_mbSpTrkVtxMh_eff->isPassedBits() & TrigDefs::EF_prescaled)
			== TrigDefs::EF_prescaled;

// pA Jan 2013
	bool EF_L1ZDC_A_NoAlg_ps = (m_EF_L1ZDC_A_NoAlg->isPassedBits() & TrigDefs::EF_prescaled) == TrigDefs::EF_prescaled;
	bool EF_L1ZDC_C_NoAlg_ps = (m_EF_L1ZDC_C_NoAlg->isPassedBits() & TrigDefs::EF_prescaled) == TrigDefs::EF_prescaled;
	bool EF_L1LHCF_NoAlg_ps = (m_EF_L1LHCF_NoAlg->isPassedBits() & TrigDefs::EF_prescaled) == TrigDefs::EF_prescaled;


	//EF Should not be in Pass Through
	//bool EF_mbMbts_1_eff_pt = (m_EF_mbMbts_1_eff->isPassedBits() & TrigDefs::EF_passThrough) == TrigDefs::EF_passThrough;
	bool EF_mbMbts_2_eff_pt = (m_EF_mbMbts_2_eff->isPassedBits() & TrigDefs::EF_passThrough) == TrigDefs::EF_passThrough;
	bool EF_mbMbts_1_1_eff_pt = (m_EF_mbMbts_1_1_eff->isPassedBits() & TrigDefs::EF_passThrough)
			== TrigDefs::EF_passThrough;
	bool EF_mbMbts_2_2_eff_pt = (m_EF_mbMbts_2_2_eff->isPassedBits() & TrigDefs::EF_passThrough)
			== TrigDefs::EF_passThrough;
	bool EF_mbMbts_4_4_eff_pt = (m_EF_mbMbts_4_4_eff->isPassedBits() & TrigDefs::EF_passThrough)
			== TrigDefs::EF_passThrough;
	bool EF_mbZdc_eff_pt = (m_EF_mbZdc_eff->isPassedBits() & TrigDefs::EF_passThrough) == TrigDefs::EF_passThrough;
	bool EF_mbRd1_eff_pt = (m_EF_mbRd1_eff->isPassedBits() & TrigDefs::EF_passThrough) == TrigDefs::EF_passThrough;
	bool EF_mbSpTrk_pt = (m_EF_mbSpTrk->isPassedBits() & TrigDefs::EF_passThrough) == TrigDefs::EF_passThrough;
// 8 TeV Low-mu
	bool EF_mbLucid_eff_pt = (m_EF_mbLucid_eff->isPassedBits() & TrigDefs::EF_passThrough) == TrigDefs::EF_passThrough;

	bool EF_rd0_filled_NoAlg_pt = (m_EF_rd0_filled_NoAlg->isPassedBits() & TrigDefs::EF_passThrough) == TrigDefs::EF_passThrough;
	//bool EF_mbMbts_2_NoAlg_pt = (m_EF_mbMbts_2_NoAlg->isPassedBits() & TrigDefs::EF_passThrough) == TrigDefs::EF_passThrough;

// pA
	//bool EF_mbRd1_eff_pt is just defined
	bool EF_L1ItemStreamer_L1_MBTS_2_pt = (m_EF_L1ItemStreamer_L1_MBTS_2->isPassedBits() & TrigDefs::EF_passThrough) == TrigDefs::EF_passThrough;
	bool EF_mbSp_pt = (m_EF_mbSp->isPassedBits() & TrigDefs::EF_passThrough) == TrigDefs::EF_passThrough;


	//  High Mult
	bool EF_mbSpTrkVtxMh_pt = (m_EF_mbSpTrkVtxMh->isPassedBits() & TrigDefs::EF_passThrough) == TrigDefs::EF_passThrough;
	bool EF_mbSpTrkVtxMh_eff_pt = (m_EF_mbSpTrkVtxMh_eff->isPassedBits() & TrigDefs::EF_passThrough)
			== TrigDefs::EF_passThrough;
// pA Jan 2013
	bool EF_L1ZDC_A_NoAlg_pt = (m_EF_L1ZDC_A_NoAlg->isPassedBits() & TrigDefs::EF_passThrough) == TrigDefs::EF_passThrough;
	bool EF_L1ZDC_C_NoAlg_pt = (m_EF_L1ZDC_C_NoAlg->isPassedBits() & TrigDefs::EF_passThrough) == TrigDefs::EF_passThrough;
	bool EF_L1LHCF_NoAlg_pt = (m_EF_L1LHCF_NoAlg->isPassedBits() & TrigDefs::EF_passThrough) == TrigDefs::EF_passThrough;



	//EF decision
	//bool EF_mbMbts_1_eff_pr = (m_EF_mbMbts_1_eff->isPassedBits() & TrigDefs::EF_passedRaw) == TrigDefs::EF_passedRaw;
        bool EF_mbMbts_2_eff_pr = (m_EF_mbMbts_2_eff->isPassedBits() & TrigDefs::EF_passedRaw) == TrigDefs::EF_passedRaw;
	bool EF_mbZdc_eff_pr = (m_EF_mbZdc_eff->isPassedBits() & TrigDefs::EF_passedRaw) == TrigDefs::EF_passedRaw;
	bool EF_mbSpTrk_pr = (m_EF_mbSpTrk->isPassedBits() & TrigDefs::EF_passedRaw) == TrigDefs::EF_passedRaw;
// 8 TeV Low-mu
	//bool EF_mbLucid_eff_pr = (m_EF_mbLucid_eff->isPassedBits() & TrigDefs::EF_passedRaw) == TrigDefs::EF_passedRaw;
	bool EF_rd0_filled_NoAlg_pr = (m_EF_rd0_filled_NoAlg->isPassedBits() & TrigDefs::EF_passedRaw) == TrigDefs::EF_passedRaw;
	//bool EF_mbMbts_2_NoAlg_pr = (m_EF_mbMbts_2_NoAlg->isPassedBits() & TrigDefs::EF_passedRaw) == TrigDefs::EF_passedRaw;
// pA
	bool EF_mbRd1_eff_pr = (m_EF_mbRd1_eff->isPassedBits() & TrigDefs::EF_passedRaw) == TrigDefs::EF_passedRaw;
	bool EF_L1ItemStreamer_L1_MBTS_2_pr = (m_EF_L1ItemStreamer_L1_MBTS_2->isPassedBits() & TrigDefs::EF_passedRaw) == TrigDefs::EF_passedRaw;
	bool EF_mbSp_pr = (m_EF_mbSp->isPassedBits() & TrigDefs::EF_passedRaw) == TrigDefs::EF_passedRaw;


	// High Mult
	bool EF_mbSpTrkVtxMh_pr = (m_EF_mbSpTrkVtxMh->isPassedBits() & TrigDefs::EF_passedRaw) == TrigDefs::EF_passedRaw;
	bool EF_mbSpTrkVtxMh_eff_pr = (m_EF_mbSpTrkVtxMh_eff->isPassedBits() & TrigDefs::EF_passedRaw)
			== TrigDefs::EF_passedRaw;

// pA Jan 2013
	bool EF_L1ZDC_A_NoAlg_pr = (m_EF_L1ZDC_A_NoAlg->isPassedBits() & TrigDefs::EF_passedRaw) == TrigDefs::EF_passedRaw;
	bool EF_L1ZDC_C_NoAlg_pr = (m_EF_L1ZDC_C_NoAlg->isPassedBits() & TrigDefs::EF_passedRaw) == TrigDefs::EF_passedRaw;
	bool EF_L1LHCF_NoAlg_pr = (m_EF_L1LHCF_NoAlg->isPassedBits() & TrigDefs::EF_passedRaw) == TrigDefs::EF_passedRaw;


	//check if L2 Hypo algorithms was ran (to improve efficiency of the code)
	//bool L2_mbMbts_1_eff_ran = (L1_mbMbts_1_eff && (!L2_mbMbts_1_eff_ps || L2_mbMbts_1_eff_pt)
	//		&& !m_EF_mbMbts_1_eff->getListOfTriggers().empty());

	bool L2_mbMbts_2_eff_ran = (L1_mbMbts_2_eff && (!L2_mbMbts_2_eff_ps || L2_mbMbts_2_eff_pt)
			&& !m_EF_mbMbts_2_eff->getListOfTriggers().empty());

	//Additional Heavy-Ion triggers
/* no mbMbts_1_eff ; mbMbts_2_eff is intead
	L2_mbMbts_1_eff_ran |= (L1_mbMbts_1_1_eff && (!L2_mbMbts_1_1_eff_ps || L2_mbMbts_1_1_eff_pt)
			&& !m_EF_mbMbts_1_1_eff->getListOfTriggers().empty());
	L2_mbMbts_1_eff_ran |= (L1_mbMbts_2_2_eff && (!L2_mbMbts_2_2_eff_ps || L2_mbMbts_2_2_eff_pt)
			&& !m_EF_mbMbts_2_2_eff->getListOfTriggers().empty());
	L2_mbMbts_1_eff_ran |= (L1_mbMbts_4_4_eff && (!L2_mbMbts_4_4_eff_ps || L2_mbMbts_4_4_eff_pt)
			&& !m_EF_mbMbts_4_4_eff->getListOfTriggers().empty());
*/
	L2_mbMbts_2_eff_ran |= (L1_mbMbts_1_1_eff && (!L2_mbMbts_1_1_eff_ps || L2_mbMbts_1_1_eff_pt)
			&& !m_EF_mbMbts_1_1_eff->getListOfTriggers().empty());
	L2_mbMbts_2_eff_ran |= (L1_mbMbts_2_2_eff && (!L2_mbMbts_2_2_eff_ps || L2_mbMbts_2_2_eff_pt)
			&& !m_EF_mbMbts_2_2_eff->getListOfTriggers().empty());
	L2_mbMbts_2_eff_ran |= (L1_mbMbts_4_4_eff && (!L2_mbMbts_4_4_eff_ps || L2_mbMbts_4_4_eff_pt)
			&& !m_EF_mbMbts_4_4_eff->getListOfTriggers().empty());

	bool L2_mbZdc_eff_ran = (L1_mbZdc_eff && (!L2_mbZdc_eff_ps || L2_mbZdc_eff_pt)
			&& !m_EF_mbZdc_eff->getListOfTriggers().empty());

	bool L2_mbRd1_eff_ran = (L1_mbRd1_eff && (!L2_mbRd1_eff_ps || L2_mbRd1_eff_pt)
			&& !m_EF_mbRd1_eff->getListOfTriggers().empty());

	bool L2_mbSpTrk_ran = (L1_mbSpTrk && (!L2_mbSpTrk_ps || L2_mbSpTrk_pt) && !m_EF_mbSpTrk->getListOfTriggers().empty());
// 8 TeV Low-mu
	bool L2_mbLucid_eff_ran = (L1_mbLucid_eff && (!L2_mbLucid_eff_ps || L2_mbLucid_eff_pt)
			&& !m_EF_mbLucid_eff->getListOfTriggers().empty());

	bool L2_rd0_filled_NoAlg_ran = (L1_rd0_filled_NoAlg && (!L2_rd0_filled_NoAlg_ps || L2_rd0_filled_NoAlg_pt) 
				  && !m_EF_rd0_filled_NoAlg->getListOfTriggers().empty());
//	bool L2_mbMbts_2_NoAlg_ran = (L1_mbMbts_2_NoAlg && (!L2_mbMbts_2_NoAlg_ps || L2_mbMbts_2_NoAlg_pt) 
//                                  && !m_EF_mbMbts_2_NoAlg->getListOfTriggers().empty());
// pA
	//bool L2_mbRd1_eff_ran  is defined just above
	bool L2_L1ItemStreamer_L1_MBTS_2_ran = (L1_L1ItemStreamer_L1_MBTS_2 && (!L2_L1ItemStreamer_L1_MBTS_2_ps || L2_L1ItemStreamer_L1_MBTS_2_pt) 
                                  && !m_EF_L1ItemStreamer_L1_MBTS_2->getListOfTriggers().empty());
	bool L2_mbSp_ran = (L1_mbSp && (!L2_mbSp_ps || L2_mbSp_pt) && !m_EF_mbSp->getListOfTriggers().empty());

	// High Mult ran
	bool L2_mbSpTrkVtxMh_ran = (L1_mbSpTrkVtxMh && (!L2_mbSpTrkVtxMh_ps || L2_mbSpTrkVtxMh_pt)
			&& !m_EF_mbSpTrkVtxMh->getListOfTriggers().empty());
	bool L2_mbSpTrkVtxMh_eff_ran = (L1_mbSpTrkVtxMh_eff && (!L2_mbSpTrkVtxMh_eff_ps || L2_mbSpTrkVtxMh_eff_pt)
			&& !m_EF_mbSpTrkVtxMh_eff->getListOfTriggers().empty());

	/*
	 bool sct_pix_hypo_on = L2_mbMbts_1_eff_ran || L2_mbZdc_eff_ran || L2_mbRd1_eff_ran;

	 if (!sct_pix_hypo_on)
	 return StatusCode::SUCCESS;
	 */
	// retrieve MinBias features from transient classes
	// L2, ID

	pixSpBarr = 0;
	pixSpECA = 0;
	pixSpECC = 0;
	sctSpBarr = 0;
	sctSpECA = 0;
	sctSpECC = 0;

	const xAOD::TrigSpacePointCountsContainer* mbSPcont = 0;
	StatusCode sc_mbsp = m_storeGate->retrieve(mbSPcont, m_spContainerName);

	if (sc_mbsp.isFailure() || mbSPcont->empty()) {
		if (sc_mbsp.isFailure())
			(*m_log) << MSG::WARNING << "Failed to retrieve HLT MinBias for xAOD::TrigSpacePointCountsContainer" << endreq;
		else
			(*m_log) << MSG::DEBUG << "xAOD::TrigSpacePointCountsContainer is empty." << endreq;
	} else {
		(*m_log) << MSG::DEBUG << " ====== START HLTMinBias MonTool for xAOD::TrigSpacePointCountsContainer ====== " << endreq;

		(*m_log) << MSG::DEBUG << "space point container has " << mbSPcont->size() << " entries." << endreq;

		// Loop over TrigMinBias feature objects
		xAOD::TrigSpacePointCountsContainer::const_iterator mbSP_coll_itr = mbSPcont->begin();
		xAOD::TrigSpacePointCountsContainer::const_iterator mbSP_coll_itrE = mbSPcont->end();

		for (; mbSP_coll_itr != mbSP_coll_itrE; ++mbSP_coll_itr) {
			xAOD::TrigSpacePointCounts *id_mbFeature = (*mbSP_coll_itr);

			/*if (id_mbFeature->pixelClusBarrel().nbins_x() > 0 && id_mbFeature->pixelClusBarrel().nbins_y() > 0) {
				pixSpBarr = (int) id_mbFeature->pixelClusBarrel().sumEntries(m_timeOverThresholdCut, 0.,
						TrigHistoCutType::ABOVE_X_ABOVE_Y);
				m_pixBarr_sp->Fill(pixSpBarr);
			} else {
				(*m_log) << MSG::WARNING << "Histogram pixelClusBarrel() is not initialized properly; it has 0 bins in X or Y."
						<< endreq;

			}

			if (id_mbFeature->pixelClusEndcapA().nbins_x() > 0 && id_mbFeature->pixelClusEndcapA().nbins_y() > 0) {
				pixSpECA = (int) id_mbFeature->pixelClusEndcapA().sumEntries(m_timeOverThresholdCut, 0.,
						TrigHistoCutType::ABOVE_X_ABOVE_Y);
				m_pixECA_sp->Fill(pixSpECA);
			} else {
				(*m_log) << MSG::WARNING
						<< "Histogram pixelClusEndcapA() is not initialized properly; it has 0 bins in X or Y." << endreq;

			}

			if (id_mbFeature->pixelClusEndcapC().nbins_x() > 0 && id_mbFeature->pixelClusEndcapC().nbins_y() > 0) {
				pixSpECC = (int) id_mbFeature->pixelClusEndcapC().sumEntries(m_timeOverThresholdCut, 0.,
						TrigHistoCutType::ABOVE_X_ABOVE_Y);
				m_pixECC_sp->Fill(pixSpECC);

			} else {
				(*m_log) << MSG::WARNING
						<< "Histogram pixelClusEndcapC()  is not initialized properly; it has 0 bins in X or Y." << endreq;
			}*/

			sctSpBarr = (int) id_mbFeature->sctSpBarrel();
			sctSpECA = (int) id_mbFeature->sctSpEndcapA();
			sctSpECC = (int) id_mbFeature->sctSpEndcapC();

			m_sctBarr_sp->Fill(sctSpBarr);
			m_sctECA_sp->Fill(sctSpECA);
			m_sctECC_sp->Fill(sctSpECC);

			//Check if a L2 trigger is ran
/* 
// ***** no mbMbts_1_eff ; mbMbts_2_eff instead 
			if (L2_mbMbts_1_eff_ran) {
				h_mbMbts_1_eff_pixTot_P->Fill(pixSpBarr + pixSpECA + pixSpECC);
				h_mbMbts_1_eff_sctTot_P->Fill(sctSpBarr + sctSpECA + sctSpECC);
			}
*/
			if (L2_mbMbts_2_eff_ran) {
				h_mbMbts_2_eff_pixTot_P->Fill(pixSpBarr + pixSpECA + pixSpECC);
				h_mbMbts_2_eff_sctTot_P->Fill(sctSpBarr + sctSpECA + sctSpECC);
			}

			if (L2_mbZdc_eff_ran) {
				h_mbZdc_eff_pixTot_P->Fill(pixSpBarr + pixSpECA + pixSpECC);
				h_mbZdc_eff_sctTot_P->Fill(sctSpBarr + sctSpECA + sctSpECC);
			}

// 8 TeV Lucid
			if (L2_mbLucid_eff_ran) {
				h_mbLucid_eff_pixTot_P->Fill(pixSpBarr + pixSpECA + pixSpECC);
				h_mbLucid_eff_sctTot_P->Fill(sctSpBarr + sctSpECA + sctSpECC);
			}


			if (L2_mbRd1_eff_ran) {
				h_mbRd1_eff_pixTot_P->Fill(pixSpBarr + pixSpECA + pixSpECC);
				h_mbRd1_eff_sctTot_P->Fill(sctSpBarr + sctSpECA + sctSpECC);
			}

/* 
// moved to pA just below with the changes

			if (L2_mbSpTrk_ran) {
				if (L2_mbSpTrk_pr) { //Trigger mismatch; error
					h_mbSpTrk_pixTot_P->Fill(pixSpBarr + pixSpECA + pixSpECC);
					h_mbSpTrk_sctTot_P->Fill(sctSpBarr + sctSpECA + sctSpECC);
// 8 TeV Low-mu
//
                                        if (L2_mbMbts_2_NoAlg_ran) {
                                        	if (L2_mbMbts_2_NoAlg_pr) { //Trigger mismatch; error
                                                        h_mbSpTrk_pixTot_mb2Alg_P->Fill(pixSpBarr + pixSpECA + pixSpECC);
                                                	h_mbSpTrk_sctTot_mb2Alg_P->Fill(sctSpBarr + sctSpECA + sctSpECC);
				              	} else {
                                                        h_mbSpTrk_pixTot_mb2Alg_F->Fill(pixSpBarr + pixSpECA + pixSpECC);
                                                	h_mbSpTrk_sctTot_mb2Alg_F->Fill(sctSpBarr + sctSpECA + sctSpECC);
                                                }
                                        }        
                               
				} else {
					h_mbSpTrk_pixTot_F->Fill(pixSpBarr + pixSpECA + pixSpECC);
					h_mbSpTrk_sctTot_F->Fill(sctSpBarr + sctSpECA + sctSpECC);
				}
			}
*/

// ========> pA ; pA 2013: mbSp || mbSpTrk, rd0 in

			if (L2_mbSp_ran || L2_mbSpTrk_ran) {
				if (L2_mbSp_pr || L2_mbSpTrk_pr) { //Trigger mismatch; error
					h_mbSp_pixTot_P->Fill(pixSpBarr + pixSpECA + pixSpECC);
					h_mbSp_sctTot_P->Fill(sctSpBarr + sctSpECA + sctSpECC);
//
                                        if (L2_rd0_filled_NoAlg_ran) {
                                        	if (L2_rd0_filled_NoAlg_pr) { //Trigger mismatch; error
                                                        h_mbSpTrk_pixTot_rd0_P->Fill(pixSpBarr + pixSpECA + pixSpECC);
                                                        h_mbSpTrk_sctTot_rd0_P->Fill(sctSpBarr + sctSpECA + sctSpECC);
                                        	} else {
                                                        h_mbSpTrk_pixTot_rd0_F->Fill(pixSpBarr + pixSpECA + pixSpECC);
                                                        h_mbSpTrk_sctTot_rd0_F->Fill(sctSpBarr + sctSpECA + sctSpECC);
                                                }
                                        }        
                                        if (L2_mbRd1_eff_ran) {
                                        	if (L2_mbRd1_eff_pr) { //Trigger mismatch; error
                                                        h_mbSp_pixTot_Rd1_P->Fill(pixSpBarr + pixSpECA + pixSpECC);
                                                        h_mbSp_sctTot_Rd1_P->Fill(sctSpBarr + sctSpECA + sctSpECC);
                                        	} else {
                                                        h_mbSp_pixTot_Rd1_F->Fill(pixSpBarr + pixSpECA + pixSpECC);
                                                        h_mbSp_sctTot_Rd1_F->Fill(sctSpBarr + sctSpECA + sctSpECC);
                                                }
                                        }        
//
                                        if (L2_L1ItemStreamer_L1_MBTS_2_ran) {
                                        	if (L2_L1ItemStreamer_L1_MBTS_2_pr) { //Trigger mismatch; error
                                                        h_mbSp_pixTot_mb2_P->Fill(pixSpBarr + pixSpECA + pixSpECC);
                                                	h_mbSp_sctTot_mb2_P->Fill(sctSpBarr + sctSpECA + sctSpECC);
				              	} else {
                                                        h_mbSp_pixTot_mb2_F->Fill(pixSpBarr + pixSpECA + pixSpECC);
                                                	h_mbSp_sctTot_mb2_F->Fill(sctSpBarr + sctSpECA + sctSpECC);
                                                }
                                        }        
                               
				} else {
					h_mbSp_pixTot_F->Fill(pixSpBarr + pixSpECA + pixSpECC);
					h_mbSp_sctTot_F->Fill(sctSpBarr + sctSpECA + sctSpECC);
				}
			}


			//High Mult
			if (L2_mbSpTrkVtxMh_ran) {
				if (L2_mbSpTrkVtxMh_pr) { //Trigger mismatch; error
					h_pix_mbSpTrkVtxMh_P->Fill(pixSpBarr + pixSpECA + pixSpECC);
					h_sct_mbSpTrkVtxMh_P->Fill(sctSpBarr + sctSpECA + sctSpECC);
				} else {
					h_pix_mbSpTrkVtxMh_F->Fill(pixSpBarr + pixSpECA + pixSpECC);
					h_sct_mbSpTrkVtxMh_F->Fill(sctSpBarr + sctSpECA + sctSpECC);
				}
			}

			//High Mult eff
			if (L2_mbSpTrkVtxMh_eff_ran) {
				//The trigger can not fail
				//if (L2_mbSpTrkVtxMh_eff_pr) { //Trigger mismatch; error
					h_pix_mbSpTrkVtxMh_eff_P->Fill(pixSpBarr + pixSpECA + pixSpECC);
					h_sct_mbSpTrkVtxMh_eff_P->Fill(sctSpBarr + sctSpECA + sctSpECC);
				//} else {
				//	h_pix_mbSpTrkVtxMh_eff_F->Fill(pixSpBarr + pixSpECA + pixSpECC);
				//	h_sct_mbSpTrkVtxMh_eff_F->Fill(sctSpBarr + sctSpECA + sctSpECC);
				//}
			}

			//End of Check

		}

	} // end TrigSpacePointCounts

	//Now we can form a proper L2 simulated decision

	bool sim_L2_mbSpTrk = false;
	bool sim_L2_mbSp = false;

	if (m_triggerTypeAND) {
		sim_L2_mbSpTrk = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) && ((sctSpBarr + sctSpECA + sctSpECC)
				> m_totalSctSp_cut);
		sim_L2_mbSp = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) && ((sctSpBarr + sctSpECA + sctSpECC)
				> m_totalSctSp_cut);
	} else {
		sim_L2_mbSpTrk = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) || ((sctSpBarr + sctSpECA + sctSpECC)
				> m_totalSctSp_cut);
		sim_L2_mbSp = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) || ((sctSpBarr + sctSpECA + sctSpECC)
				> m_totalSctSp_cut);
	}

	//Check if a L2 trigger is ran
	if (L2_mbSp_ran) {  
		if (sim_L2_mbSp != L2_mbSp_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("L2_mbSp", 1.0); 
		} else {
			m_h_TriggerErrors->Fill("L2_mbSp", 0.0); 
		}
	}

	if (L2_mbSpTrk_ran) {  
		if (sim_L2_mbSpTrk != L2_mbSpTrk_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("L2_mbSpTrk", 1.0); 
		} else {
			m_h_TriggerErrors->Fill("L2_mbSpTrk", 0.0); 
		}
	}


//*
// moved to pA with the changes 
// 8 TeV Low-mu
// rd0 back in use for pA 2013
        bool sim_L2_rd0_filled_NoAlg = false;
	if (m_triggerTypeAND) {
                sim_L2_rd0_filled_NoAlg = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) && ((sctSpBarr + sctSpECA + sctSpECC)
                                > m_totalSctSp_cut);
	} else {
		sim_L2_rd0_filled_NoAlg  = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) || ((sctSpBarr + sctSpECA + sctSpECC)
				> m_totalSctSp_cut);
	}
         
	//Check if a L2 trigger is ran
        if (L2_rd0_filled_NoAlg_ran) {
		if (sim_L2_rd0_filled_NoAlg != L2_rd0_filled_NoAlg_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("L2_rd0_filled_NoAlg", 1.0);
		} else {
			m_h_TriggerErrors->Fill("L2_rd0_filled_NoAlg", 0.0);
		}
	}
                    
	//Check if a L2 trigger is ran
        bool sim_L2_mbMbts_2_eff = false;
	if (m_triggerTypeAND) {
                sim_L2_mbMbts_2_eff = ((pixSpBarr + pixSpECC + pixSpECA) > 
m_totalPixelClus_cut) && ((sctSpBarr + sctSpECA + sctSpECC)
                                > m_totalSctSp_cut);
	} else {
		sim_L2_mbMbts_2_eff  = ((pixSpBarr + pixSpECC + pixSpECA) 
> m_totalPixelClus_cut) || ((sctSpBarr + sctSpECA + sctSpECC)
				> m_totalSctSp_cut);
	}
         
	//Check if a L2 trigger is ran
        if (L2_mbMbts_2_eff_ran) {
		if (sim_L2_mbMbts_2_eff != L2_mbMbts_2_eff_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("L2_mbMbts_2_eff", 1.0);
		} else {
			m_h_TriggerErrors->Fill("L2_mbMbts_2_eff", 0.0);
		}
	}
                    
/*
        bool sim_L2_mbMbts_2_NoAlg = false;
	if (m_triggerTypeAND) {
                sim_L2_mbMbts_2_NoAlg = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) && ((sctSpBarr + sctSpECA + sctSpECC)
                                > m_totalSctSp_cut);
	} else {
		sim_L2_mbMbts_2_NoAlg  = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) || ((sctSpBarr + sctSpECA + sctSpECC)
				> m_totalSctSp_cut);
	}
         
	//Check if a L2 trigger is ran
        if (L2_mbMbts_2_NoAlg_ran) {
		if (sim_L2_mbMbts_2_NoAlg != L2_mbMbts_2_NoAlg_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("L2_mbMbts_2_NoAlg", 1.0);
		} else {
			m_h_TriggerErrors->Fill("L2_mbMbts_2_NoAlg", 0.0);
		}
	}
                    
*/

// ============> pA

        bool sim_L2_mbRd1_eff = false;
	if (m_triggerTypeAND) {
                sim_L2_mbRd1_eff = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) && ((sctSpBarr + sctSpECA + sctSpECC)
                                > m_totalSctSp_cut);
	} else {
		sim_L2_mbRd1_eff  = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) || ((sctSpBarr + sctSpECA + sctSpECC)
				> m_totalSctSp_cut);
	}
         
	//Check if a L2 trigger is ran
        if (L2_mbRd1_eff_ran) {
		if (sim_L2_mbRd1_eff != L2_mbRd1_eff_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("L2_mbRd1_eff", 1.0);
		} else {
			m_h_TriggerErrors->Fill("L2_mbRd1_eff", 0.0);
		}
	}
                    
        bool sim_L2_L1ItemStreamer_L1_MBTS_2 = false;
	if (m_triggerTypeAND) {
                sim_L2_L1ItemStreamer_L1_MBTS_2 = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) && ((sctSpBarr + sctSpECA + sctSpECC)
                                > m_totalSctSp_cut);
	} else {
		sim_L2_L1ItemStreamer_L1_MBTS_2  = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) || ((sctSpBarr + sctSpECA + sctSpECC)
				> m_totalSctSp_cut);
	}
         
	//Check if a L2 trigger is ran
        if (L2_L1ItemStreamer_L1_MBTS_2_ran) {
		if (sim_L2_L1ItemStreamer_L1_MBTS_2 != L2_L1ItemStreamer_L1_MBTS_2_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("L2_L1ItemStreamer_L1_MBTS_2", 1.0);
		} else {
			m_h_TriggerErrors->Fill("L2_L1ItemStreamer_L1_MBTS_2", 0.0);
		}
	}


	// High Mult - VtxMh 
	bool sim_L2_mbSpTrkVtxMh = false;

	if (m_triggerTypeAND) {
		sim_L2_mbSpTrkVtxMh = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) && ((sctSpBarr + sctSpECA
				+ sctSpECC) > m_totalSctSp_cut);
	} else {
		sim_L2_mbSpTrkVtxMh = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) || ((sctSpBarr + sctSpECA
				+ sctSpECC) > m_totalSctSp_cut);
	}

	// High Mult Check if a L2 trigger is ran
	if (L2_mbSpTrkVtxMh_ran) {
		if (sim_L2_mbSpTrkVtxMh != L2_mbSpTrkVtxMh_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("L2_mbSpTrkVtxMh", 1.0);
		} else {
			m_h_TriggerErrors->Fill("L2_mbSpTrkVtxMh", 0.0);
		}
	}

	// High Mult - VtxMh_eff 
	bool sim_L2_mbSpTrkVtxMh_eff = false;

	if (m_triggerTypeAND) {
		sim_L2_mbSpTrkVtxMh_eff = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) && ((sctSpBarr + sctSpECA
				+ sctSpECC) > m_totalSctSp_cut);
	} else {
		sim_L2_mbSpTrkVtxMh_eff = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) || ((sctSpBarr + sctSpECA
				+ sctSpECC) > m_totalSctSp_cut);
	}

	// High Mult eff Check if a L2 trigger is ran
	if (L2_mbSpTrkVtxMh_eff_ran) {
		if (sim_L2_mbSpTrkVtxMh_eff != L2_mbSpTrkVtxMh_eff_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("L2_mbSpTrkVtxMh_eff", 1.0);
		} else {
			m_h_TriggerErrors->Fill("L2_mbSpTrkVtxMh_eff", 0.0);
		}
	}


        bool sim_L2_mbZdc_eff = false;
	if (m_triggerTypeAND) {
                sim_L2_mbZdc_eff = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) && ((sctSpBarr + sctSpECA + sctSpECC)
                                > m_totalSctSp_cut);
	} else {
		sim_L2_mbZdc_eff  = ((pixSpBarr + pixSpECC + pixSpECA) > m_totalPixelClus_cut) || ((sctSpBarr + sctSpECA + sctSpECC)
				> m_totalSctSp_cut);
	}
         
	//Check if a L2 trigger is ran
        if (L2_mbZdc_eff_ran) {
		if (sim_L2_mbZdc_eff != L2_mbZdc_eff_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("L2_mbZdc_a_c_eff", 1.0);
		} else {
			m_h_TriggerErrors->Fill("L2_mbZdc_a_c_eff", 0.0);
		}
	}


	/*
	 if (L2_mbZdc_eff_ran) {
	 if (sim_L2_mbSpTrk != L2_mbZdc_eff_pr) { //Trigger mismatch; error
	 m_h_TriggerErrors->Fill("L2_mbZdc_eff", 1.0);
	 } else {
	 m_h_TriggerErrors->Fill("L2_mbZdc_eff", 0.0);
	 }
	 }

	 if (L2_mbRd1_eff_ran) {
	 if (sim_L2_mbSpTrk != L2_mbRd1_eff_pr) { //Trigger mismatch; error
	 m_h_TriggerErrors->Fill("L2_mbRd1_eff", 1.0);
	 } else {
	 m_h_TriggerErrors->Fill("L2_mbRd1_eff", 0.0);
	 }
	 }
	 */
	//--------------------------------------------------------------------------------------------------------------

/*********   This part is specifically fot the hip_trk histo ************** */
// Simplified pass of EF_mbSp/Trk and hip_trk trigs.  pA Jan 2013

	//bool L1_mbSp_defined = !(L1_mbSp->getListOfTriggers().empty());
	//bool L1_mbSpTrk_defined = !(L1_mbSpTrk->getListOfTriggers().empty());
        bool mbSp_passed = m_EF_mbSp->isPassed(TrigDefs::eventAccepted);
        bool mbSpTrk_passed = m_EF_mbSpTrk->isPassed(TrigDefs::eventAccepted);

 	bool L1_hip_trk150_L1TE50 = (m_EF_mbSpTrkVtxMh_hip_trk150_L1TE50->isPassedBits() & L1_bits) == L1_bits;
 	bool L1_hip_trk190_L1TE50 = (m_EF_mbSpTrkVtxMh_hip_trk190_L1TE50->isPassedBits() & L1_bits) == L1_bits;
 	bool L1_hip_trk120_L1TE35 = (m_EF_mbSpTrkVtxMh_hip_trk120_L1TE35->isPassedBits() & L1_bits) == L1_bits;
 	bool L1_hip_trk190_L1TE35 = (m_EF_mbSpTrkVtxMh_hip_trk190_L1TE35->isPassedBits() & L1_bits) == L1_bits;
 	bool L1_hip_trk240_L1TE35 = (m_EF_mbSpTrkVtxMh_hip_trk240_L1TE35->isPassedBits() & L1_bits) == L1_bits;
 	bool L1_hip_trk240_L1TE65 = (m_EF_mbSpTrkVtxMh_hip_trk240_L1TE65->isPassedBits() & L1_bits) == L1_bits;
 	bool L1_hip_trk200_L1TE65 = (m_EF_mbSpTrkVtxMh_hip_trk200_L1TE65->isPassedBits() & L1_bits) == L1_bits;
 	bool L1_hip_trk180_L1TE65 = (m_EF_mbSpTrkVtxMh_hip_trk180_L1TE65->isPassedBits() & L1_bits) == L1_bits;
	bool L1_hip_trk175_L1MBTS = (m_EF_mbSpTrkVtxMh_hip_trk175_L1MBTS->isPassedBits() & L1_bits) == L1_bits;

	bool L1_hip_trk150_L1TE50_passed = (L1_hip_trk150_L1TE50 && !m_EF_mbSpTrkVtxMh_hip_trk150_L1TE50->getListOfTriggers().empty());
	bool L1_hip_trk190_L1TE50_passed = (L1_hip_trk190_L1TE50 && !m_EF_mbSpTrkVtxMh_hip_trk190_L1TE50->getListOfTriggers().empty());
	bool L1_hip_trk120_L1TE35_passed = (L1_hip_trk120_L1TE35 && !m_EF_mbSpTrkVtxMh_hip_trk120_L1TE35->getListOfTriggers().empty());
	bool L1_hip_trk190_L1TE35_passed = (L1_hip_trk190_L1TE35 && !m_EF_mbSpTrkVtxMh_hip_trk190_L1TE35->getListOfTriggers().empty());
	bool L1_hip_trk240_L1TE35_passed = (L1_hip_trk240_L1TE35 && !m_EF_mbSpTrkVtxMh_hip_trk240_L1TE35->getListOfTriggers().empty());
	bool L1_hip_trk240_L1TE65_passed = (L1_hip_trk240_L1TE65 && !m_EF_mbSpTrkVtxMh_hip_trk240_L1TE65->getListOfTriggers().empty());
	bool L1_hip_trk200_L1TE65_passed = (L1_hip_trk200_L1TE65 && !m_EF_mbSpTrkVtxMh_hip_trk200_L1TE65->getListOfTriggers().empty());
	bool L1_hip_trk180_L1TE65_passed = (L1_hip_trk180_L1TE65 && !m_EF_mbSpTrkVtxMh_hip_trk180_L1TE65->getListOfTriggers().empty());
	bool L1_hip_trk175_L1MBTS_passed = (L1_hip_trk175_L1MBTS && !m_EF_mbSpTrkVtxMh_hip_trk175_L1MBTS->getListOfTriggers().empty());

        bool sim_hip_trk_150_50 = false;
        if (m_triggerTypeAND) {
		sim_hip_trk_150_50 = ((sctSpBarr + sctSpECA + sctSpECC) 
				> m_sctSp_thresh);
        }
        bool sim_hip_trk_190_50 = false;
        if (m_triggerTypeAND) {
		sim_hip_trk_190_50 = ((sctSpBarr + sctSpECA + sctSpECC) 
				> m_sctSp_thresh);
        }
         bool sim_hip_trk_190_35 = false;
        if (m_triggerTypeAND) {
		sim_hip_trk_190_35 = ((sctSpBarr + sctSpECA + sctSpECC) 
				> m_sctSp_thresh);
        }
        bool sim_hip_trk_240_35 = false;
        if (m_triggerTypeAND) {
		sim_hip_trk_240_35 = ((sctSpBarr + sctSpECA + sctSpECC) 
				> m_sctSp_thresh);
        }
        bool sim_hip_trk_120_35 = false;
        if (m_triggerTypeAND) {
		sim_hip_trk_120_35 = ((sctSpBarr + sctSpECA + sctSpECC) 
				> m_sctSp_thresh);
        }
        bool sim_hip_trk_180_65 = false;
        if (m_triggerTypeAND) {
		sim_hip_trk_180_65 = ((sctSpBarr + sctSpECA + sctSpECC) 
				> m_sctSp_thresh);
        }
        bool sim_hip_trk_200_65 = false;
        if (m_triggerTypeAND) {
		sim_hip_trk_200_65 = ((sctSpBarr + sctSpECA + sctSpECC) 
				> m_sctSp_thresh);
        }
        bool sim_hip_trk_240_65 = false;
        if (m_triggerTypeAND) {
		sim_hip_trk_240_65 = ((sctSpBarr + sctSpECA + sctSpECC) 
				> m_sctSp_thresh);
        }
        bool sim_hip_trk_175_mb = false;
        if (m_triggerTypeAND) {
		sim_hip_trk_175_mb = ((sctSpBarr + sctSpECA + sctSpECC) 
				> m_sctSp_thresh);
        }

        //if (L1_mbSp_defined || L1_mbSpTrk_defined) {
        if (mbSp_passed || mbSpTrk_passed) {
		if (sim_hip_trk_150_50) {
			if (L1_hip_trk150_L1TE50_passed) {
  			    m_h_Trig_hip_trk_P->Fill("trk150_L1TE50",1.0);
                        } else {   
  			    m_h_Trig_hip_trk_F->Fill("trk150_L1TE50",1.0);
                        }
                }
		if (sim_hip_trk_190_50) {
                        if (L1_hip_trk190_L1TE50_passed) {
  			   m_h_Trig_hip_trk_P->Fill("trk190_L1TE50",1.0);
                        } else {   
  			   m_h_Trig_hip_trk_F->Fill("trk190_L1TE50",1.0);
                        }
                }
		if (sim_hip_trk_120_35) {
                        if (L1_hip_trk120_L1TE35_passed) {
  			    m_h_Trig_hip_trk_P->Fill("trk120_L1TE35",1.0);
                        } else {   
  			    m_h_Trig_hip_trk_F->Fill("trk120_L1TE35",1.0);
                        }
                }
		if (sim_hip_trk_190_35) {
                        if (L1_hip_trk190_L1TE35_passed) {  	
			    m_h_Trig_hip_trk_P->Fill("trk190_L1TE35",1.0);
                        } else {   
  			    m_h_Trig_hip_trk_F->Fill("trk190_L1TE35",1.0);
                        }
                }
		if (sim_hip_trk_240_35) {
                        if (L1_hip_trk240_L1TE35_passed) {  	
		            m_h_Trig_hip_trk_P->Fill("trk240_L1TE35",1.0);
                        } else {   
  			    m_h_Trig_hip_trk_F->Fill("trk240_L1TE35",1.0);
                        }
                }
		if (sim_hip_trk_240_65) {
                        if (L1_hip_trk240_L1TE65_passed) {  	
		            m_h_Trig_hip_trk_P->Fill("trk240_L1TE65",1.0);
                        } else {   
  			    m_h_Trig_hip_trk_F->Fill("trk240_L1TE65",1.0);
                        }
                }
		if (sim_hip_trk_200_65) {
                        if (L1_hip_trk200_L1TE65_passed) {  	
		            m_h_Trig_hip_trk_P->Fill("trk200_L1TE65",1.0);
                        } else {   
  			    m_h_Trig_hip_trk_F->Fill("trk200_L1TE65",1.0);
                        }
                }
		if (sim_hip_trk_180_65) {
                        if (L1_hip_trk180_L1TE65_passed) {  	
		            m_h_Trig_hip_trk_P->Fill("trk180_L1TE65",1.0);
                        } else {   
  			    m_h_Trig_hip_trk_F->Fill("trk180_L1TE65",1.0);
                        }
                }
		if (sim_hip_trk_175_mb) {
  			if (L1_hip_trk175_L1MBTS_passed) {
  			    m_h_Trig_hip_trk_P->Fill("trk175_L1MBTS",1.0);
                	} else {   
  			    m_h_Trig_hip_trk_F->Fill("trk175_L1MBTS",1.0);
	                }
                }
        }
// ********** end of hip_trk *********** */
	//EF active triggers
	//bool EF_mbMbts_1_eff_enabled = ((!EF_mbMbts_1_eff_ps || EF_mbMbts_1_eff_pt) && L2_mbMbts_1_eff_ran
	//		&& !m_EF_mbMbts_1_eff->getListOfTriggers().empty());
	bool EF_mbMbts_2_eff_enabled = ((!EF_mbMbts_2_eff_ps || EF_mbMbts_2_eff_pt) && L2_mbMbts_2_eff_ran
			&& !m_EF_mbMbts_2_eff->getListOfTriggers().empty());

	//Specific to heavy-ion conditions
/*
	EF_mbMbts_1_eff_enabled |= ((!EF_mbMbts_1_1_eff_ps || EF_mbMbts_1_1_eff_pt) && L2_mbMbts_1_eff_ran
			&& !m_EF_mbMbts_1_1_eff->getListOfTriggers().empty());
	EF_mbMbts_1_eff_enabled |= ((!EF_mbMbts_2_2_eff_ps || EF_mbMbts_2_2_eff_pt) && L2_mbMbts_1_eff_ran
			&& !m_EF_mbMbts_2_2_eff->getListOfTriggers().empty());
	EF_mbMbts_1_eff_enabled |= ((!EF_mbMbts_4_4_eff_ps || EF_mbMbts_4_4_eff_pt) && L2_mbMbts_1_eff_ran
			&& !m_EF_mbMbts_4_4_eff->getListOfTriggers().empty());
*/
	EF_mbMbts_2_eff_enabled |= ((!EF_mbMbts_1_1_eff_ps || EF_mbMbts_1_1_eff_pt) && L2_mbMbts_2_eff_ran
			&& !m_EF_mbMbts_1_1_eff->getListOfTriggers().empty());
	EF_mbMbts_2_eff_enabled |= ((!EF_mbMbts_2_2_eff_ps || EF_mbMbts_2_2_eff_pt) && L2_mbMbts_2_eff_ran
			&& !m_EF_mbMbts_2_2_eff->getListOfTriggers().empty());
	EF_mbMbts_2_eff_enabled |= ((!EF_mbMbts_4_4_eff_ps || EF_mbMbts_4_4_eff_pt) && L2_mbMbts_2_eff_ran
			&& !m_EF_mbMbts_4_4_eff->getListOfTriggers().empty());
	EF_mbMbts_2_eff_enabled |= ((!EF_mbMbts_4_4_eff_ps || EF_mbMbts_4_4_eff_pt) && L2_mbMbts_2_eff_ran
			&& !m_EF_mbMbts_4_4_eff->getListOfTriggers().empty());

	bool EF_mbZdc_eff_enabled = ((!EF_mbZdc_eff_ps || EF_mbZdc_eff_pt) && L2_mbZdc_eff_ran
			&& !m_EF_mbZdc_eff->getListOfTriggers().empty());

	bool EF_mbRd1_eff_enabled = ((!EF_mbRd1_eff_ps || EF_mbRd1_eff_pt) && L2_mbRd1_eff_ran
			&& !m_EF_mbRd1_eff->getListOfTriggers().empty());

	bool EF_mbSpTrk_enabled = ((!EF_mbSpTrk_ps || EF_mbSpTrk_pt) && L2_mbSpTrk_pr
			&& !m_EF_mbSpTrk->getListOfTriggers().empty());
// 8 TeV low-mu
	bool EF_mbLucid_eff_enabled = ((!EF_mbLucid_eff_ps || EF_mbLucid_eff_pt) && L2_mbLucid_eff_ran
			&& !m_EF_mbLucid_eff->getListOfTriggers().empty());

        bool EF_rd0_filled_NoAlg_enabled = ((!EF_rd0_filled_NoAlg_ps || EF_rd0_filled_NoAlg_pt) && L2_rd0_filled_NoAlg_pr
			&& !m_EF_rd0_filled_NoAlg->getListOfTriggers().empty());
        //bool EF_mbMbts_2_NoAlg_enabled = ((!EF_mbMbts_2_NoAlg_ps || EF_mbMbts_2_NoAlg_pt) && L2_mbMbts_2_NoAlg_pr 
        //                && !m_EF_mbMbts_2_NoAlg->getListOfTriggers().empty());
// pA
	//bool EF_mbRd1_eff_enabled  is already defined just above
        bool EF_L1ItemStreamer_L1_MBTS_2_enabled = ((!EF_L1ItemStreamer_L1_MBTS_2_ps || EF_L1ItemStreamer_L1_MBTS_2_pt) && L2_L1ItemStreamer_L1_MBTS_2_pr 
                        && !m_EF_L1ItemStreamer_L1_MBTS_2->getListOfTriggers().empty());
	bool EF_mbSp_enabled = ((!EF_mbSp_ps || EF_mbSp_pt) && L2_mbSp_pr && !m_EF_mbSp->getListOfTriggers().empty());
//
//pA jan 2013
	bool EF_L1ZDC_A_NoAlg_enabled = ((!EF_L1ZDC_A_NoAlg_ps || EF_L1ZDC_A_NoAlg_pt) && L2_L1ZDC_A_NoAlg_pr && !m_EF_L1ZDC_A_NoAlg->getListOfTriggers().empty());
	bool EF_L1ZDC_C_NoAlg_enabled = ((!EF_L1ZDC_C_NoAlg_ps || EF_L1ZDC_C_NoAlg_pt) && L2_L1ZDC_C_NoAlg_pr && !m_EF_L1ZDC_C_NoAlg->getListOfTriggers().empty());
	bool EF_L1LHCF_NoAlg_enabled = ((!EF_L1LHCF_NoAlg_ps || EF_L1LHCF_NoAlg_pt) && L2_L1LHCF_NoAlg_pr && !m_EF_L1LHCF_NoAlg->getListOfTriggers().empty());

	//High Mult 
	bool EF_mbSpTrkVtxMh_enabled = ((!EF_mbSpTrkVtxMh_ps || EF_mbSpTrkVtxMh_pt) && L2_mbSpTrkVtxMh_pr
			&& !m_EF_mbSpTrkVtxMh->getListOfTriggers().empty());

	bool EF_mbSpTrkVtxMh_eff_enabled = ((!EF_mbSpTrkVtxMh_eff_ps || EF_mbSpTrkVtxMh_eff_pt) && L2_mbSpTrkVtxMh_eff_pr
			&& !m_EF_mbSpTrkVtxMh_eff->getListOfTriggers().empty());

	// EF ID (it is the same for trt and pixel+sct)
	const xAOD::TrigTrackCountsContainer* mbTTcont = 0;
	StatusCode sc_mbtt = m_storeGate->retrieve(mbTTcont, m_tcContainerName);

	if (sc_mbtt.isFailure() || mbTTcont->empty()) {
		if (sc_mbtt.isFailure())
			(*m_log) << MSG::WARNING << "Failed to retrieve HLT MinBias MonTool for xAOD::TrigTrackCountsContainer" << endreq;
		else
			(*m_log) << MSG::DEBUG << "xAOD::TrigTrackCountsContainer is empty." << endreq;

	} else {
		(*m_log) << MSG::DEBUG << " ====== START HLTMinBias MonTool for xAOD::TrigTrackCountsContainer ====== " << endreq;

		mbTracks = 0;
		totpix_spEF = 0;
		totsct_spEF = 0;

		// Loop over EF TrigMinBias objects
		xAOD::TrigTrackCountsContainer::const_iterator mbTT_coll_itr = mbTTcont->begin();
		for (; mbTT_coll_itr != mbTTcont->end(); ++mbTT_coll_itr) {
			xAOD::TrigTrackCounts *mbTT = (*mbTT_coll_itr);

			/*if (mbTT->z0_pt().nbins_x() > 0 && mbTT->z0_pt().nbins_y() > 0) {
				mbTracks = (int) (mbTT->z0_pt().sumEntries(m_max_z0, m_min_pt, TrigHistoCutType::BELOW_X_ABOVE_Y));
				m_minbiasTracks->Fill(mbTracks);
			} else {
				(*m_log) << MSG::WARNING << "The trigger histogram z0_pt is not initialized properly; it has 0 bins in X or Y."
						<< endreq;
			}*/

//// --> pA ; pA Jan 2013
			//Check if the EF_mbSp/Trk trigger is ran
			if (EF_mbSp_enabled || EF_mbSpTrk_enabled) {
				if (EF_mbSp_pr || EF_mbSpTrk_pr) { //Trigger mismatch; error
					h_mbSp_trkTot_P->Fill(mbTracks);
//////// no _mbMbts_2_NoAlg
/*
//
                  			//Check if the EF_mbMbts_2_NoAlg trigger is ran
                                        if (EF_mbMbts_2_NoAlg_enabled) {
                                        	if (EF_mbMbts_2_NoAlg_pr) { //Trigger mismatch; error
                                                        h_mbSpTrk_trkTot_mb2Alg_P->Fill(mbTracks);
				              	} else {
                                                        h_mbSpTrk_trkTot_mb2Alg_F->Fill(mbTracks);
                                                }
                                        }        
*/
// rd0 back in use: pA 2013
                  			//Check if the EF_rd0_filled_NoAlg trigger is ran
                                        if (EF_rd0_filled_NoAlg_enabled) {
                                        	if (EF_rd0_filled_NoAlg_pr) { //Trigger mismatch; error
                                                        h_mbSpTrk_trkTot_rd0_P->Fill(mbTracks);
                                        	} else {
                                                        h_mbSpTrk_trkTot_rd0_F->Fill(mbTracks);
                                                }
                                        }        

                  			//Check if the EF_mbRd1_eff trigger is ran
                                        if (EF_mbRd1_eff_enabled) {
                                        	if (EF_mbRd1_eff_pr) { //Trigger mismatch; error
                                                        h_mbSp_trkTot_Rd1_P->Fill(mbTracks);
                                        	} else {
                                                        h_mbSp_trkTot_Rd1_F->Fill(mbTracks);
                                                }
                                        }        
//
                  			//Check if the L1ItemStreamer_L1_MBTS_2 trigger is ran
                                        if (EF_L1ItemStreamer_L1_MBTS_2_enabled) {
                                        	if (EF_L1ItemStreamer_L1_MBTS_2_pr) { //Trigger mismatch; error
                                                        h_mbSp_trkTot_mb2_P->Fill(mbTracks);
				              	} else {
                                                        h_mbSp_trkTot_mb2_F->Fill(mbTracks);
                                                }
                                        }        

// no mbMbts_1_eff ; mbMbts_2_eff is instead
/*
					//Check if the EF_mbMbts_1_eff trigger is ran
					if (EF_mbMbts_1_eff_enabled) {
						//if (EF_mbMbts_1_eff_pr) { //Trigger mismatch; error
							h_mbMbts_1_eff_trkTot_P->Fill(mbTracks);
						//} else {
						//	h_mbMbts_1_eff_trkTot_F->Fill(mbTracks);
						//}
					}
*/
					//Check if the EF_mbMbts_2_eff trigger is ran
					if (EF_mbMbts_2_eff_enabled) {
						if (EF_mbMbts_2_eff_pr) { //Trigger mismatch; error
							h_mbMbts_2_eff_trkTot_P->Fill(mbTracks);
						} else {
					        	h_mbMbts_2_eff_trkTot_F->Fill(mbTracks);
						}
					}

					//Check if the EF_mbZdc_eff trigger is ran
					if (EF_mbZdc_eff_enabled) {
						if (EF_mbZdc_eff_pr) { //Trigger mismatch; error
							h_mbZdc_eff_trkTot_P->Fill(mbTracks);
						} else {
							h_mbZdc_eff_trkTot_F->Fill(mbTracks);
						}
					}

					// 8 TeV low-mu Lucid
					//Check if the EF_mbLucid_eff trigger is ran
					if (EF_mbLucid_eff_enabled) {
						//if (EF_mbLucid_eff_pr) { //Trigger mismatch; error
							h_mbLucid_eff_trkTot_P->Fill(mbTracks);
						//} else {
						//	h_mbLucid_eff_trkTot_F->Fill(mbTracks);
						//}
					}

					//High Mult VtxMh
					// Check if the EF_mbSpTrkVtxMh trigger is ran
					if (EF_mbSpTrkVtxMh_enabled) {
						if (EF_mbSpTrkVtxMh_pr) { //Trigger mismatch; error
							h_EF_mbSpTrkVtxMh_P->Fill(mbTracks);
						} else {
							h_EF_mbSpTrkVtxMh_F->Fill(mbTracks);
						}
					}

					//High Mult 
					// Check if the EF_mbSpTrkVtxMh_eff trigger is ran
					if (EF_mbSpTrkVtxMh_eff_enabled) {
					//The trigger can not fail by definition
						//if (EF_mbSpTrkVtxMh_eff_pr) { //Trigger mismatch; error
							h_EF_mbSpTrkVtxMh_eff_P->Fill(mbTracks);
						//} else {
						//	h_EF_mbSpTrkVtxMh_eff_F->Fill(mbTracks);
						//}
					}

					// pA Jan 2013
					//Check if the EF_L1ZDC_A_NoAlg trigger is ran
					if (EF_L1ZDC_A_NoAlg_enabled) {
						if (EF_L1ZDC_A_NoAlg_pr) { //Trigger mismatch; error
							h_L1ZDC_A_NoAlg_trkToT_P->Fill(mbTracks);
							} else {
							h_L1ZDC_A_NoAlg_trkToT_F->Fill(mbTracks);
							}
					}

					//Check if the EF_L1ZDC_C_NoAlg trigger is ran
					if (EF_L1ZDC_C_NoAlg_enabled) {
						if (EF_L1ZDC_C_NoAlg_pr) { //Trigger mismatch; error
							h_L1ZDC_C_NoAlg_trkToT_P->Fill(mbTracks);
							} else {
							h_L1ZDC_C_NoAlg_trkToT_F->Fill(mbTracks);
							}
					}

					//Check if the EF_L1LHCF_NoAlg trigger is ran
					if (EF_L1LHCF_NoAlg_enabled) {
						if (EF_L1LHCF_NoAlg_pr) { //Trigger mismatch; error
							h_L1LHCF_NoAlg_trkToT_P->Fill(mbTracks);
							} else {
							h_L1LHCF_NoAlg_trkToT_F->Fill(mbTracks);
							}
					}

//
				} else {
					h_mbSp_trkTot_F->Fill(mbTracks);
				}
			}

		}
	} // end TrigTrackCounts


	//Now we can form a proper EF simulated decision

	bool sim_EF_mbSpTrk = mbTracks >= m_required_ntrks;
	bool sim_EF_mbSp = mbTracks >= m_required_ntrks;

	/*
	 //Check if the EF trigger is ran
	 if (EF_mbMbts_1_eff_enabled) {
	 if (sim_EF_mbSpTrk != EF_mbMbts_1_eff_pr) { //Trigger mismatch; error
	 m_h_TriggerErrors->Fill("EF_mbMbts_1_eff", 1.0);
	 } else {
	 m_h_TriggerErrors->Fill("EF_mbMbts_1_eff", 0.0);
	 }
	 }

	 //Check if the EF trigger is ran
	 if (EF_mbZdc_eff_enabled) {
	 if (sim_EF_mbSpTrk != EF_mbZdc_eff_pr) { //Trigger mismatch; error
	 m_h_TriggerErrors->Fill("EF_mbZdc_eff", 1.0);
	 } else {
	 m_h_TriggerErrors->Fill("EF_mbZdc_eff", 0.0);
	 }
	 }

	 //Check if the EF trigger is ran
	 if (EF_mbRd1_eff_enabled) {
	 if (sim_EF_mbSpTrk != EF_mbRd1_eff_pr) { //Trigger mismatch; error
	 m_h_TriggerErrors->Fill("EF_mbRd1_eff", 1.0);
	 } else {
	 m_h_TriggerErrors->Fill("EF_mbRd1_eff", 0.0);
	 }
	 }
	 */

	//Check if the EF trigger is ran
///*
// pA 2013, EfmbSpTrk + EF_mbSp
	if (EF_mbSpTrk_enabled) {
		if (sim_EF_mbSpTrk != EF_mbSpTrk_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("EF_mbSpTrk", 1.0);
		} else {
			m_h_TriggerErrors->Fill("EF_mbSpTrk", 0.0);
		}
	}
//*/
	if (EF_mbSp_enabled) {
		if (sim_EF_mbSp != EF_mbSp_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("EF_mbSp", 1.0);
		} else {
			m_h_TriggerErrors->Fill("EF_mbSp", 0.0);
		}
	}

//*
// moved to pA with the changes (SpTrk->Sp, etc.) just below
// 8 TeV Low-mu
// rd0 back in use pA 2013


	bool sim_EF_mbMbts_2_eff = mbTracks >= m_required_ntrks;
	 //Check if the EF trigger is ran
	 if (EF_mbMbts_2_eff_enabled) {
		 if (sim_EF_mbMbts_2_eff != EF_mbMbts_2_eff_pr) { //Trigger mismatch; error
	 		m_h_TriggerErrors->Fill("EF_mbMbts_2_eff", 1.0);
	 		} else {
	 		m_h_TriggerErrors->Fill("EF_mbMbts_2_eff", 0.0);
	 		}
	 }

	bool sim_EF_rd0_filled_NoAlg = mbTracks >= m_required_ntrks;
	//bool sim_EF_mbMbts_2_NoAlg = mbTracks >= m_required_ntrks;

	//Check if the EF trigger is ran
	if (EF_rd0_filled_NoAlg_enabled) {
		if (sim_EF_rd0_filled_NoAlg != EF_rd0_filled_NoAlg_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("EF_rd0_filled_NoAlg", 1.0);
		} else {
			m_h_TriggerErrors->Fill("EF_rd0_filled_NoAlg", 0.0);
		}
	}
/*
	//Check if the EF trigger is ran
	if (EF_mbMbts_2_NoAlg_enabled) {
		if (sim_EF_mbMbts_2_NoAlg != EF_mbMbts_2_NoAlg_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("EF_mbMbts_2_NoAlg", 1.0);
		} else {
			m_h_TriggerErrors->Fill("EF_mbMbts_2_NoAlg", 0.0);
		}
	}

*/

// ======> pA 

	bool sim_EF_mbRd1_eff = mbTracks >= m_required_ntrks;
	bool sim_EF_L1ItemStreamer_L1_MBTS_2 = mbTracks >= m_required_ntrks;

	//Check if the EF trigger is ran
	if (EF_mbRd1_eff_enabled) {
		if (sim_EF_mbRd1_eff != EF_mbRd1_eff_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("EF_mbRd1_eff", 1.0);
		} else {
			m_h_TriggerErrors->Fill("EF_mbRd1_eff", 0.0);
		}
	}
	//Check if the EF trigger is ran
	if (EF_L1ItemStreamer_L1_MBTS_2_enabled) {
		if (sim_EF_L1ItemStreamer_L1_MBTS_2 != EF_L1ItemStreamer_L1_MBTS_2_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("EF_L1ItemStreamer_L1_MBTS_2", 1.0);
		} else {
			m_h_TriggerErrors->Fill("EF_L1ItemStreamer_L1_MBTS_2", 0.0);
		}
	}

        bool sim_EF_Zdc_eff =  mbTracks >= m_required_ntrks;
	//Check if the EF trigger is ran
	if (EF_mbZdc_eff_enabled) {
		if (sim_EF_Zdc_eff != EF_mbZdc_eff_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("EF_mbZdc_a_c_eff", 1.0);
		} else {
			m_h_TriggerErrors->Fill("EF_mbZdc_a_b_eff", 0.0);
		}
	}

	// High Mult
	bool sim_EF_mbSpTrkVtxMh = mbTracks >= m_required_ntrks;
	//Check if the EF trigger is ran
	if (EF_mbSpTrkVtxMh_enabled) {
		if (sim_EF_mbSpTrkVtxMh != EF_mbSpTrkVtxMh_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("EF_mbSpTrkVtxMh", 1.0);
		} else {
			m_h_TriggerErrors->Fill("EF_mbSpTrkVtxMh", 0.0);
		}
	}

	bool sim_EF_mbSpTrkVtxMh_eff = mbTracks >= m_required_ntrks;
	//Check if the EF eff trigger is ran
	if (EF_mbSpTrkVtxMh_eff_enabled) {
		if (sim_EF_mbSpTrkVtxMh_eff != EF_mbSpTrkVtxMh_eff_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("EF_mbSpTrkVtxMh_eff", 1.0);
		} else {
			m_h_TriggerErrors->Fill("EF_mbSpTrkVtxMh_eff", 0.0);
		}
	}


// pA Jan 2013

	bool sim_EF_L1ZDC_A_NoAlg = mbTracks >= m_required_ntrks;
	//Check if the EF trigger is ran
	if (EF_L1ZDC_A_NoAlg_enabled) {
		if (sim_EF_L1ZDC_A_NoAlg != EF_L1ZDC_A_NoAlg_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("EF_L1ZDC_A_NoAlg", 1.0);
		} else {
			m_h_TriggerErrors->Fill("EF_L1ZDC_A_NoAlg", 0.0);
		}
	}

	bool sim_EF_L1ZDC_C_NoAlg = mbTracks >= m_required_ntrks;
	//Check if the EF trigger is ran
	if (EF_L1ZDC_C_NoAlg_enabled) {
		if (sim_EF_L1ZDC_C_NoAlg != EF_L1ZDC_C_NoAlg_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("EF_L1ZDC_C_NoAlg", 1.0);
		} else {
			m_h_TriggerErrors->Fill("EF_L1ZDC_C_NoAlg", 0.0);
		}
	}


	bool sim_EF_L1LHCF_NoAlg = mbTracks >= m_required_ntrks;
	//Check if the EF trigger is ran
	if (EF_L1LHCF_NoAlg_enabled) {
		if (sim_EF_L1LHCF_NoAlg != EF_L1LHCF_NoAlg_pr) { //Trigger mismatch; error
			m_h_TriggerErrors->Fill("EF_L1LHCF_NoAlg", 1.0);
		} else {
			m_h_TriggerErrors->Fill("EF_L1LHCF_NoAlg", 0.0);
		}
	}



// Energy Time
        
        unsigned int triggerWord = 0;
        unsigned int timeWord = 0;

        std::vector<float> mbtsHitEnergies; // energy (in pC) of signal in a counter (relative to IP), vector for all counters
        std::vector<float> mbtsHitTimes; // time of signal in counter (relative to IP), vector for all counters        

        int time_ebaCounters=0, time_ebcCounters=0;
        int energy_ebaCounters=0, energy_ebcCounters=0;
        float timeMean_A = 0.;
        float timeMean_C = 0.;
        float timeMin_A = 10000.;
        float timeMin_C = 10000.;
        float energyMean_A = 0.;
        float energyMean_C = 0.;
        float energyMax_A = -10000.;
        float energyMax_C = -10000.;

        //==============================================================================
        // MBTS BITS INFORMATION
        //==============================================================================
       const xAOD::TrigT2MbtsBitsContainer* mbTScont = 0;
        StatusCode sc = m_storeGate->retrieve(mbTScont, m_t2mbtsContainerName);

        if (sc.isFailure() || mbTScont->empty()) {
                if (sc.isFailure())
                        (*m_log) << MSG::WARNING << "Failed to retrieve MbtsMinBiasMonTool for xAOD::TrigT2MbtsBitsContainer" << endreq;
                else
                        (*m_log) << MSG::DEBUG << "xAOD::TrigT2MbtsBitsContainer is empty." << endreq;
        } else {
                (*m_log) << MSG::DEBUG << " ====== START MbtsMinBiasMonTool for xAOD::TrigT2MbtsBitsContainer ====== " << endreq;

                // Loop over EF TrigMinBias objects
                xAOD::TrigT2MbtsBitsContainer::const_iterator mbTS_coll_itr = mbTScont->begin();
                for (; mbTS_coll_itr != mbTScont->end(); ++mbTS_coll_itr) {

                        xAOD::TrigT2MbtsBits *mbtsFeature = (*mbTS_coll_itr);

                        mbtsHitEnergies = mbtsFeature->triggerEnergies();
                        mbtsHitTimes = mbtsFeature->triggerTimes();

                        if (mbtsHitTimes.size() != 32)
                                (*m_log) << MSG::ERROR << "MBTS Cell Times are stored incorrectly. The array should have 32 elements."
                                                << endreq;

                        if (mbtsHitEnergies.size() != 32)
                                (*m_log) << MSG::ERROR << "MBTS Cell Energies are stored incorrectly. The array should have 32 elements."
                                                << endreq;

        //The energy-dependent bitmask is produced below
                        for (std::size_t k = 0; k < mbtsHitEnergies.size() && k < 32; k++)
                                if (mbtsHitEnergies.at(k) > m_energyCut) {
                                        triggerWord += (1 << k);
                                        // Online distributions
                                        const char* cell_name = (m_moduleLabel[k]).data();
                                        if (k < 16) { // A side
                                               m_h_occupancy_onl_A->Fill(cell_name, 1.0);
                                               timeMean_A += mbtsHitTimes.at(k); time_ebaCounters++;
                                               if (mbtsHitTimes.at(k) < timeMin_A) timeMin_A = mbtsHitTimes.at(k);
                                               if (k == 15) {  
                                                     //if (L1_mbMbts_1_eff) { //          <--- the trigger
                                                     if (L1_mbMbts_2_eff) { 
                                                              m_h_timeMin_A_P->Fill(timeMin_A);
                                                     } else {
                                                              m_h_timeMin_A_F->Fill(timeMin_A);
                                                     }
                                                     if (time_ebaCounters> 0) {
                                                          timeMean_A /= time_ebaCounters;
                                                          //if (L1_mbMbts_1_eff) { //       <--- the trigger   
                                                          if (L1_mbMbts_2_eff) { 
                                                                   m_h_timeMean_A_P->Fill(timeMean_A);
                                                          } else {
                                                                   m_h_timeMean_A_F->Fill(timeMean_A);
                                                          }
                                                     } else {
                                         	           timeMean_A = -999.0;
                                                     }
                                               }
                                        } else { // C side 
                                               m_h_occupancy_onl_C->Fill(cell_name, 1.0);
                                               timeMean_C += mbtsHitTimes.at(k); time_ebcCounters++;
                                               if (mbtsHitTimes.at(k) < timeMin_C) timeMin_C = mbtsHitTimes.at(k);
                                               if (k == 31) { 
                                                     //if (L1_mbMbts_1_eff) { //              <--- the trigger   
                                                     if (L1_mbMbts_2_eff) { 
                                                              m_h_timeMin_C_P->Fill(timeMin_C);
                                                     } else {
                                                              m_h_timeMin_C_F->Fill(timeMin_C);
                                                     }
                                                    if (time_ebcCounters> 0) {
                                                         timeMean_C /= time_ebcCounters;
                                                         //if (L1_mbMbts_1_eff) { //          <---  the trigger   
                                                         if (L1_mbMbts_2_eff) { 
                                                                 m_h_timeMean_C_P->Fill(timeMean_C);
                                                         } else {
                                                                 m_h_timeMean_C_F->Fill(timeMean_C);
                                                         }
                                                    } else {
                                         	         timeMean_C = -999.0;
                                                    }
                                               }
                                        }
                                        //Time online
                                        m_h_time_1d->Fill(mbtsHitTimes.at(k)); 
                                        //Check if the L2_mbMbts_1_eff trigger is ran
                                        // now mbMbts_2_eff (pA 2013)
                                        //if (L2_mbMbts_1_eff_ran) {
                                        if (L2_mbMbts_2_eff_ran) {
                                                  m_h_time_L2_P->Fill(mbtsHitTimes.at(k));
                                        } else {
                                                  m_h_time_L2_F->Fill(mbtsHitTimes.at(k)); 
                                        }
                                        //Check if the EF_mbMbts_1_eff trigger is ran
                                        // now mbMbts_2_eff (pA 2013)
                                       //if (EF_mbMbts_1_eff_enabled) {
                                       if (EF_mbMbts_2_eff_enabled) {
                                                  m_h_time_EF_P->Fill(mbtsHitTimes.at(k));
                                        } else {
                                                  m_h_time_EF_F->Fill(mbtsHitTimes.at(k)); 
                                        }
                                }
                        //The time-dependent bitmask is produced for the case
                        //of time-dependent L2 trigger
                        for (std::size_t k = 0; k < mbtsHitTimes.size() && k < 32; k++) {
                                if (fabs(mbtsHitTimes.at(k)) < m_timeCut || m_timeCut <= 0.0)
                                        timeWord += (1 << k);

                                // Online distributions
                                //const char* cell_name = (m_moduleLabel[k]).data();

                                if (k < 16) { // A side
                                       energyMean_A += mbtsHitEnergies.at(k); energy_ebaCounters++;
                                       if (mbtsHitEnergies.at(k) > energyMax_A) energyMax_A = mbtsHitEnergies.at(k);
                                       if (k == 15) {
                                              //if (L1_mbMbts_1_eff) { // --->is this the trigger needed ??????  
                                              if (L1_mbMbts_2_eff) { 
                                                       m_h_energyMax_A_P->Fill(energyMax_A);
                                              } else {
                                                       m_h_energyMax_A_F->Fill(energyMax_A);
                                              }
                                              if (energy_ebaCounters> 0) {
                                                      energyMean_A /= energy_ebaCounters;
                                                      //if (L1_mbMbts_1_eff) { // --->is this the trigger needed ??????  
                                                      if (L1_mbMbts_2_eff) { 
                                                              m_h_energyMean_A_P->Fill(energyMean_A);
                                                      } else {
                                                              m_h_energyMean_A_F->Fill(energyMean_A);
                                                      }
                                              } else {
                                         	      energyMean_A = -999.0;
                                              }
                                       }
                                } else { // C side 
                                       energyMean_C += mbtsHitEnergies.at(k); energy_ebcCounters++;
                                       if (mbtsHitEnergies.at(k) > energyMax_C) energyMax_C = mbtsHitEnergies.at(k);
                                       if (k == 31)  {
                                             //if (L1_mbMbts_1_eff) { // --->is this the trigger needed ??????  
                                             if (L1_mbMbts_2_eff) { 
                                                      m_h_energyMax_C_P->Fill(energyMax_C);
                                             } else {
                                                      m_h_energyMax_C_F->Fill(energyMax_C);
                                             }
                                             if ( energy_ebcCounters> 0) {
                                                     energyMean_C /= energy_ebcCounters;
                                                     //if (L1_mbMbts_1_eff) { // --->is this the trigger needed ??????  
                                                     if (L1_mbMbts_2_eff) 
{ 
                                                               m_h_energyMean_C_P->Fill(energyMean_C);
                                                     } else {
                                                               m_h_energyMean_C_F->Fill(energyMean_C);
                                                     }
                                             } else {
                                                     energyMean_C = -999.0;
                                             }
                                       }
                                }
                                //Energy online
                                m_h_energy_1d->Fill(mbtsHitEnergies.at(k)); 
                                //Check if the L2_mbMbts_1_eff trigger is ran
				// now mbMbts_2_eff (pA Jan 2013)
                                //if (L2_mbMbts_1_eff_ran) {
                                if (L2_mbMbts_2_eff_ran) {
                                        m_h_energy_L2_P->Fill(mbtsHitEnergies.at(k));
                                } else {
                                        m_h_energy_L2_F->Fill(mbtsHitEnergies.at(k)); 
                                }
                                //Check if the EF_mbMbts_1_eff trigger is ran
				// now mbMbts_2_eff (pA Jan 2013)
                                //if (EF_mbMbts_1_eff_enabled) {
                                if (EF_mbMbts_2_eff_enabled) {
                                        m_h_energy_EF_P->Fill(mbtsHitEnergies.at(k));
                                } else {
                                        m_h_energy_EF_F->Fill(mbtsHitEnergies.at(k)); 
                                }
                        }
                }
        } // end TrigT2MbtsBitsContainer.h    

        m_numberOfEvents++;
	return StatusCode::SUCCESS;
}

void HLTMinBiasMonTool::fixXaxis(TH1* h) {

	for (unsigned int i = 0; i < m_moduleLabel.size(); i++) {
		const char* cell_name = (m_moduleLabel[i]).data();
		h->GetXaxis()->FindBin(cell_name);
	}
	h->ResetBit(TH1::kCanRebin);

	return;
}

int HLTMinBiasMonTool::error_bit(bool a, bool b) {

	if (a == b)
		return 0;
	else
		return 1;

}

/*---------------------------------------------------------*/
/*
StatusCode HLTMinBiasMonTool::procHistograms(bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun) 
*/
/*---------------------------------------------------------*/
/*
{
       if (isEndOfEventsBlock || isEndOfLumiBlock) {}
       if (isEndOfRun) {

//                cout << " m_numberOfEvents: " << m_numberOfEvents << " events " << endreq;

                msg(MSG::DEBUG) << " m_numberOfEvents: " << m_numberOfEvents << endreq;
              
               // Track efficiency (EF)
               TH1D *SumTrk=(TH1D*)h_mbSpTrk_trkTot_P->Clone(); 
               SumTrk->Add(h_mbSpTrk_trkTot_P,h_mbSpTrk_trkTot_F);
               h_mbSpTrk_trkTot_Eff->Divide(h_mbSpTrk_trkTot_P,SumTrk,1.,1.,"B");

               // Pixel Space Points efficiency (L2)
               TH1D *SumPix=(TH1D*)h_mbSpTrk_pixTot_P->Clone(); 
               SumPix->Add(h_mbSpTrk_pixTot_P,h_mbSpTrk_pixTot_F);
               h_mbSpTrk_pixTot_Eff->Divide(h_mbSpTrk_pixTot_P,SumPix,1.,1.,"B");

               // SCT Space Points efficiency (L2)
               TH1D *SumSCT=(TH1D*)h_mbSpTrk_sctTot_P->Clone(); 
               SumSCT->Add(h_mbSpTrk_sctTot_P,h_mbSpTrk_sctTot_F);
               h_mbSpTrk_sctTot_Eff->Divide(h_mbSpTrk_sctTot_P,SumSCT,1.,1.,"B");

               // Timing efficiency (L2)
               m_h_time_L2_Eff->Divide(m_h_time_L2_P,m_h_time_1d,1.,1.,"B");

               // Energy efficiency (L2)
               m_h_energy_L2_Eff->Divide(m_h_energy_L2_P,m_h_energy_1d,1.,1.,"B");

       }
   
       return StatusCode::SUCCESS;
}
*/
