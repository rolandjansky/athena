/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     EmEfficienciesMonTool.cxx
// PACKAGE:  TrigT1CaloMonitoring  
//
// AUTHOR:   Hardeep Bansil
//           Adapted for monitoring: Peter Faulkner
//           
//
// ********************************************************************

#include <cmath>
#include <cstdio>

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "TrigConfL1Data/L1DataDef.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloLArTowerEnergy.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/PhotonContainer.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/Photon.h"
#include "egammaEvent/egammaPIDdefs.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCluster.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "Identifier/Identifier.h"

#include "TrigT1CaloMonitoring/EmEfficienciesMonTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"

/*---------------------------------------------------------*/
EmEfficienciesMonTool::EmEfficienciesMonTool(const std::string & type,
		const std::string & name, const IInterface* parent) 
		  : ManagedMonitorToolBase(type, name, parent),
                        m_errorTool("TrigT1CaloMonErrorTool"),
			m_histTool("TrigT1CaloLWHistogramTool"),
			m_ttTool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"),
			m_larEnergy("LVL1::L1CaloLArTowerEnergy/L1CaloLArTowerEnergy"),
			m_trigger("Trig::TrigDecisionTool/TrigDecisionTool"),
			m_dbPpmDeadChannelsFolder("/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels"),
			m_triggerTowersLocation("TriggerTowers"),
			m_lvl1RoIsLocation("LVL1_ROI"),
			m_offlineElectronsLocation("ElectronAODCollection"),
			m_offlinePhotonsLocation("PhotonAODCollection"),
			m_primaryVertexLocation("VxPrimaryCandidate"), 
			m_eventInfo(0),
			m_dbPpmDeadChannels(0),
			m_triggerTowers(0), 
			m_lvl1RoIs(0),
			m_offlineElectrons(0), 
			m_offlinePhotons(0), 
			m_primaryVtx(0),
			m_numEvents(0), 
			m_numOffElec(0), 
			m_numOffPhot(0),
			m_numOffElecInContainer(0), 
			m_numOffPhotInContainer(0),
			m_numOffElecPassCuts(0), 
			m_numOffPhotPassCuts(0),
			m_numOffElecTriggered(0),
			m_numOffPhotTriggered(0),
			m_numEmObjPassTrigger(0), 
			m_numEmObjTotal(0),
			m_passed_L1_Jet_Trigger(false),
			m_passed_EF_SingleJet_Trigger(false),
			m_passed_EF_MultiJet_Trigger(false),
			m_passed_EF_egTau_Trigger(false), 
			m_passed_EF_Trigger(false),
			m_emBitMask(0),
			m_firstEvent(true),
			m_h_ClusterRaw_Et_gdEta(0), 
			m_h_ClusterRaw_Et_triggered_gdEta(0),
			m_h_ClusterRaw_Et_triggered_Eff(0),
			m_h_ClusterRaw_Et_transR(0),
			m_h_ClusterRaw_Et_triggered_transR(0),
			m_h_ClusterRaw_Et_triggered_transR_Eff(0),
			m_h_ClusterRaw_10GeV_Eta_vs_Phi(0),
			m_h_ClusterRaw_20GeV_Eta_vs_Phi(0),
			m_h_ClusterRaw_30GeV_Eta_vs_Phi(0), 
			m_h_TrigTower_emBadCalo(0),
			m_h_TrigTower_emDeadChannel(0),
			m_h_LAr_emNoisy(0)

/*---------------------------------------------------------*/
{

	declareProperty("HistogramTool", m_histTool);
	declareProperty("TriggerTowerTool", m_ttTool);
	declareProperty("LArTowerEnergyTool", m_larEnergy);
	declareProperty("TrigDecisionTool", m_trigger);
	declareProperty("DeadChannelsFolder", m_dbPpmDeadChannelsFolder);
	declareProperty("TriggerTowersLocation", m_triggerTowersLocation);
	declareProperty("RoIsLocation", m_lvl1RoIsLocation);
	declareProperty("OfflineElectronsLocation", m_offlineElectronsLocation);
	declareProperty("OfflinePhotonsLocation", m_offlinePhotonsLocation);
	declareProperty("PrimaryVertexLocation", m_primaryVertexLocation);

	declareProperty("RootDirectory", m_rootDir = "L1Calo");
	declareProperty("UseTrigger", m_useTrigger = true);
	declareProperty("TriggerStrings", m_triggerStrings);

	// HSB - python cuts
	declareProperty("useDeltaRMatch", m_useDeltaRMatch = true);
	declareProperty("goodEMDeltaRMatch_Cut", m_goodEMDeltaRMatch_Cut = 0.15);
	declareProperty("goodHadDeltaRMatch_Cut", m_goodHadDeltaRMatch_Cut = 0.2);
	declareProperty("useDeltaEtaPhiMatch", m_useDeltaEtaPhiMatch = false);
	declareProperty("goodEMDeltaPhiMatch_Cut", m_goodEMDeltaPhiMatch_Cut = 0.2);
	declareProperty("goodEMDeltaEtaMatch_Cut", m_goodEMDeltaEtaMatch_Cut = 0.2);
	declareProperty("UseEmThresholdsOnly", m_useEmThresholdsOnly = true);
	declareProperty("RemoveNoiseBursts", m_removeNoiseBursts = true);
	declareProperty("IsEmType", m_isEmType = 31);

	for (int i = 0; i < ROI_BITS; ++i) {
		m_h_ClusterRaw_Et_bitcheck[i] = 0;
		m_h_ClusterRaw_Et_bitcheck_Eff[i] = 0;
		m_h_ClusterRaw_10GeV_Eta_vs_Phi_trig[i] = 0;
		m_h_ClusterRaw_10GeV_Eta_vs_Phi_trig_Eff[i] = 0;
		m_h_ClusterRaw_20GeV_Eta_vs_Phi_trig[i] = 0;
		m_h_ClusterRaw_20GeV_Eta_vs_Phi_trig_Eff[i] = 0;
		m_h_ClusterRaw_30GeV_Eta_vs_Phi_trig[i] = 0;
		m_h_ClusterRaw_30GeV_Eta_vs_Phi_trig_Eff[i] = 0;
	}
}

/*---------------------------------------------------------*/
EmEfficienciesMonTool::~EmEfficienciesMonTool()
/*---------------------------------------------------------*/
{
}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode EmEfficienciesMonTool::initialize()
/*---------------------------------------------------------*/
{
	msg(MSG::INFO) << "Initializing " << name() << " - package version " << PACKAGE_VERSION << endreq;

	StatusCode sc;

	sc = ManagedMonitorToolBase::initialize();
	if (sc.isFailure())
		return sc;

        sc = m_errorTool.retrieve();
        if (sc.isFailure()) {
                msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloMonErrorTool" << endreq;
                return sc;
        }

	sc = m_histTool.retrieve();
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloHistogramTool" << endreq;
		return sc;
	}

	sc = m_ttTool.retrieve();
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Cannot retrieve L1TriggerTowerTool" << endreq;
		return sc;
	}

	sc = m_larEnergy.retrieve();
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Cannot retrieve L1CaloLArTowerEnergy" << endreq;
		return sc;
	}

	// Load in trigger tools
	// This tool gives you access to the physics trigger menu decisions for each event.
	sc = m_trigger.retrieve();
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Can't get handle on TrigDecisionTool" << endreq;
		return sc;
	}

	std::vector<std::string>::const_iterator iter = m_triggerStrings.begin();
	std::vector<std::string>::const_iterator iterE = m_triggerStrings.end();
	msg(MSG::INFO) << "TriggerStrings:";
	for (; iter != iterE; ++iter)
		msg(MSG::INFO) << " " << *iter;
	msg(MSG::INFO) << endreq;

	return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode EmEfficienciesMonTool::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
	msg(MSG::DEBUG) << "bookHistograms entered" << endreq;

	if (m_environment == AthenaMonManager::online) {
		// book histograms that are only made in the online environment...
	}

	if (m_dataType == AthenaMonManager::cosmics) {
		// book histograms that are only relevant for cosmics data...
	}

	if (newLumiBlock) {
	}

	if (newRun) {

                MgmtAttr_t attr = ATTRIB_UNMANAGED;
		std::string dir(m_rootDir + "/Reco/EmEfficiencies");

		MonGroup monEmDead(this, dir + "/DeadOrBadChannels", run, attr, "", "lowerLB");
		MonGroup monEmNoisy(this, dir + "/DeadOrBadChannels", run, attr);
		MonGroup monClusterRawNum(this, dir + "/ClusterRaw_Et/numerator", run, attr);
		MonGroup monClusterRawDen(this, dir + "/ClusterRaw_Et/denominator", run, attr);
		MonGroup monClusterRawEff(this, dir + "/ClusterRaw_Et", run, attr, "", "perBinEffPerCent");
		MonGroup monClusterRaw10GeVNum(this, dir + "/ClusterRaw_10GeV_EtaVsPhi/numerator", run, attr);
		MonGroup monClusterRaw10GeVDen(this, dir + "/ClusterRaw_10GeV_EtaVsPhi/denominator", run, attr);
		MonGroup monClusterRaw10GeVEff(this, dir + "/ClusterRaw_10GeV_EtaVsPhi", run, attr, "", "perBinEffPerCent");
		MonGroup monClusterRaw20GeVNum(this, dir + "/ClusterRaw_20GeV_EtaVsPhi/numerator", run, attr);
		MonGroup monClusterRaw20GeVDen(this, dir + "/ClusterRaw_20GeV_EtaVsPhi/denominator", run, attr);
		MonGroup monClusterRaw20GeVEff(this, dir + "/ClusterRaw_20GeV_EtaVsPhi", run, attr, "",  "perBinEffPerCent");
		MonGroup monClusterRaw30GeVNum(this, dir + "/ClusterRaw_30GeV_EtaVsPhi/numerator", run, attr);
		MonGroup monClusterRaw30GeVDen(this, dir + "/ClusterRaw_30GeV_EtaVsPhi/denominator", run, attr);
		MonGroup monClusterRaw30GeVEff(this, dir + "/ClusterRaw_30GeV_EtaVsPhi", run, attr, "", "perBinEffPerCent");

		//Set up EMTAU thresholds array with threshold names
		std::string thrNum[ROI_BITS] = { "0", "1", "2", "3", "4", "5", "6", "7",
		                                 "8", "9", "10", "11", "12", "13", "14", "15" };
		m_emBitMask = m_histTool->thresholdMaskEm();
		TrigConf::L1DataDef def;
		std::vector < std::string > emL1t;
		m_histTool->thresholdNames(def.emType(), emL1t);
		int size = emL1t.size();
		for (int i = 0; i < ROI_BITS; ++i) {
			if (i < size)
				emL1t[i] = "L1_" + emL1t[i];
			else
				emL1t.push_back("L1_??");
		}

		m_histTool->setMonGroup(&monEmDead);

		m_h_TrigTower_emDeadChannel = m_histTool->bookCPMEtaVsPhi("TrigTower_emDeadChannel", "EM Trigger Towers with dead channels - #eta V #phi");

		m_h_TrigTower_emBadCalo = m_histTool->bookCPMEtaVsPhi("TrigTower_emBadCalo", "EM Trigger Towers affected by Missing FEBs - #eta V #phi");

                if (m_removeNoiseBursts) {

		        m_histTool->setMonGroup(&monEmNoisy);

		        m_h_LAr_emNoisy = m_histTool->book1F("LAr_emNoisy", "LAr Error Bits in Rejected Events", 6, 0, 6);
			LWHist::LWHistAxis* axis = m_h_LAr_emNoisy->GetXaxis();
			axis->SetBinLabel(1, "BadFEBs");
			axis->SetBinLabel(2, "MediumSat");
			axis->SetBinLabel(3, "TightSat");
			axis->SetBinLabel(4, "NoiseWindow");
			axis->SetBinLabel(5, "Corrupt");
			axis->SetBinLabel(6, "CorruptWindow");
                }

		//Raw Cluster Histograms

		m_histTool->setMonGroup(&monClusterRawDen);

		m_h_ClusterRaw_Et_gdEta = m_histTool->book1F("ClusterRaw_Et_gdEta", "Raw Cluster E_{T};E_{T}^{Raw} Cluster [GeV];Clusters", 100, 0, 100);
		
		m_h_ClusterRaw_Et_transR = m_histTool->book1F("ClusterRaw_Et_transR", "Raw Cluster E_{T} (in transition region);E_{T}^{Raw} Cluster [GeV];Clusters", 100, 0, 100);

		m_histTool->setMonGroup(&monClusterRawNum);

		m_h_ClusterRaw_Et_triggered_gdEta = m_histTool->book1F("ClusterRaw_Et_triggered_gdEta", "Raw Cluster E_{T} (Triggered);E_{T}^{Raw} Cluster [GeV];Clusters", 100, 0, 100);

		m_h_ClusterRaw_Et_triggered_transR = m_histTool->book1F("ClusterRaw_Et_triggered_transR", "Raw Cluster E_{T} (Triggered in transition region);E_{T}^{Raw} Cluster [GeV];Clusters", 100, 0, 100);
		
		std::string name;
		std::string title;
		for (int i = 0; i < ROI_BITS; ++i) {
		    if (emType(i)) {
			name = "ClusterRaw_Et_bitcheck_" + thrNum[i];
			title = "Raw E_{T} for Triggered Clusters passing " + emL1t[i] + ";E_{T}^{raw} Cluster [GeV];Clusters";
			m_h_ClusterRaw_Et_bitcheck[i] = m_histTool->book1F(name, title, 100, 0, 100);
		    } else m_h_ClusterRaw_Et_bitcheck[i] = 0;
		}

		m_histTool->setMonGroup(&monClusterRawEff);

		m_h_ClusterRaw_Et_triggered_Eff	= m_histTool->book1F("ClusterRaw_Et_triggered_Eff", "Raw Cluster E_{T} (Triggered) Efficiency;E_{T}^{Raw} Cluster [GeV];Efficiency %", 100, 0, 100);
		
		m_h_ClusterRaw_Et_triggered_transR_Eff = m_histTool->book1F("ClusterRaw_Et_triggered_transR_Eff", "Raw Cluster E_{T} (Triggered in transition region) Efficiency;E_{T}^{Raw} Cluster [GeV];Efficiency %", 100, 0, 100);
		
		for (int i = 0; i < ROI_BITS; ++i) {
		    if (emType(i)) {
			name = "ClusterRaw_Et_bitcheck_Eff_" + thrNum[i];
			title = "Raw E_{T} for Triggered Clusters passing " + emL1t[i] + " Efficiency;E_{T}^{raw} Cluster [GeV];Efficiency %";
			m_h_ClusterRaw_Et_bitcheck_Eff[i] = m_histTool->book1F(name, title, 100, 0, 100);
		    } else m_h_ClusterRaw_Et_bitcheck_Eff[i] = 0;
		}

		m_histTool->setMonGroup(&monClusterRaw10GeVNum);

		//Et Raw Cluster greater > 10 GeV
		for (int i = 0; i < ROI_BITS; ++i) {
		    if (emType(i)) {
			name = "ClusterRaw_10GeV_Eta_vs_Phi_trig_" + thrNum[i];
			title = "Raw Cluster #eta V #phi (Triggered on " + emL1t[i] + " with E_{T}^{raw} > 10 GeV)";
			m_h_ClusterRaw_10GeV_Eta_vs_Phi_trig[i] = m_histTool->bookCPMEtaVsPhi(name, title);
		    } else m_h_ClusterRaw_10GeV_Eta_vs_Phi_trig[i] = 0;
		}

		m_histTool->setMonGroup(&monClusterRaw10GeVDen);

		m_h_ClusterRaw_10GeV_Eta_vs_Phi = m_histTool->bookCPMEtaVsPhi("ClusterRaw_10GeV_Eta_vs_Phi",
   				                             "Raw Cluster #eta against #phi (E_{T}^{raw} > 10 GeV)");

		m_histTool->setMonGroup(&monClusterRaw10GeVEff);

		for (int i = 0; i < ROI_BITS; ++i) {
		    if (emType(i)) {
			name = "ClusterRaw_10GeV_Eta_vs_Phi_trig_Eff_" + thrNum[i];
			title = "Raw Cluster #eta V #phi (Triggered on " + emL1t[i] + " with E_{T}^{raw} > 10 GeV) Efficiency (%)";
			m_h_ClusterRaw_10GeV_Eta_vs_Phi_trig_Eff[i] = m_histTool->bookCPMEtaVsPhi(name, title);
		    } else m_h_ClusterRaw_10GeV_Eta_vs_Phi_trig_Eff[i] = 0;
		}

		m_histTool->setMonGroup(&monClusterRaw20GeVNum);

		//Et Raw Cluster greater > 20 GeV
		for (int i = 0; i < ROI_BITS; ++i) {
		    if (emType(i)) {
			name = "ClusterRaw_20GeV_Eta_vs_Phi_trig_" + thrNum[i];
			title = "Raw Cluster #eta V #phi (Triggered on " + emL1t[i] + " with E_{T}^{raw} > 20 GeV)";
			m_h_ClusterRaw_20GeV_Eta_vs_Phi_trig[i] = m_histTool->bookCPMEtaVsPhi(name, title);
		    } else m_h_ClusterRaw_20GeV_Eta_vs_Phi_trig[i] = 0;
		}

		m_histTool->setMonGroup(&monClusterRaw20GeVDen);

		m_h_ClusterRaw_20GeV_Eta_vs_Phi	= m_histTool->bookCPMEtaVsPhi("ClusterRaw_20GeV_Eta_vs_Phi", 
				                             "Raw Cluster #eta against #phi (E_{T}^{raw} > 20 GeV)");

		m_histTool->setMonGroup(&monClusterRaw20GeVEff);

		for (int i = 0; i < ROI_BITS; ++i) {
		    if (emType(i)) {
			name = "ClusterRaw_20GeV_Eta_vs_Phi_trig_Eff_" + thrNum[i];
			title = "Raw Cluster #eta V #phi (Triggered on " + emL1t[i] + " with E_{T}^{raw} > 20 GeV) Efficiency (%)";
			m_h_ClusterRaw_20GeV_Eta_vs_Phi_trig_Eff[i] = m_histTool->bookCPMEtaVsPhi(name, title);
		    } else m_h_ClusterRaw_20GeV_Eta_vs_Phi_trig_Eff[i] = 0;
		}

		m_histTool->setMonGroup(&monClusterRaw30GeVNum);

		//Et Raw Cluster greater > 30 GeV
		for (int i = 0; i < ROI_BITS; ++i) {
		    if (emType(i)) {
			name = "ClusterRaw_30GeV_Eta_vs_Phi_trig_" + thrNum[i];
			title = "Raw Cluster #eta V #phi (Triggered on " + emL1t[i]	+ " with E_{T}^{raw} > 30 GeV)";
			m_h_ClusterRaw_30GeV_Eta_vs_Phi_trig[i] = m_histTool->bookCPMEtaVsPhi(name, title);
		    } else m_h_ClusterRaw_30GeV_Eta_vs_Phi_trig[i] = 0;
		}

		m_histTool->setMonGroup(&monClusterRaw30GeVDen);

		m_h_ClusterRaw_30GeV_Eta_vs_Phi = m_histTool->bookCPMEtaVsPhi("ClusterRaw_30GeV_Eta_vs_Phi",
				                                     "Raw Cluster #eta V #phi (E_{T}^{raw} > 30 GeV)");

		m_histTool->setMonGroup(&monClusterRaw30GeVEff);

		for (int i = 0; i < ROI_BITS; ++i) {
		    if (emType(i)) {
			name = "ClusterRaw_30GeV_Eta_vs_Phi_trig_Eff_" + thrNum[i];
			title = "Raw Cluster #eta V #phi (Triggered on " + emL1t[i] + " with E_{T}^{raw} > 30 GeV) Efficiency (%)";
			m_h_ClusterRaw_30GeV_Eta_vs_Phi_trig_Eff[i] = m_histTool->bookCPMEtaVsPhi(name, title);
		    } else m_h_ClusterRaw_30GeV_Eta_vs_Phi_trig_Eff[i] = 0;
		}

		m_histTool->unsetMonGroup();

		// HSB - counters 
		m_numEvents = 0;
		m_numOffElec = 0;
		m_numOffPhot = 0;
		m_numOffElecInContainer = 0;
		m_numOffPhotInContainer = 0;
		m_numOffElecPassCuts = 0;
		m_numOffPhotPassCuts = 0;
		m_numOffElecTriggered = 0;
		m_numOffPhotTriggered = 0;
		m_numEmObjPassTrigger = 0;
		m_numEmObjTotal = 0;
	}

	msg(MSG::DEBUG) << "Leaving bookHistograms" << endreq;

	return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode EmEfficienciesMonTool::fillHistograms()
/*---------------------------------------------------------*/
{
	const bool debug = msgLvl(MSG::DEBUG);
	if (debug) msg(MSG::DEBUG) << "fillHistograms entered" << endreq;

        // Skip events believed to be corrupt

        if (m_errorTool->corrupt()) {
                if (debug) msg(MSG::DEBUG) << "Skipping corrupt event" << endreq;
                return StatusCode::SUCCESS;
        }

	StatusCode sc;

	// On first event plot disabled channels/bad calo
	if (m_firstEvent) {
		m_firstEvent = false;
	        sc = this->triggerTowerAnalysis();
		if (sc.isFailure()) {
			if (debug) msg(MSG::DEBUG) << "Problem running triggerTowerAnalysis" << endreq;
			return sc;
		}
	}

	// Here we can use the trigger menu to decide if we want an event.
	bool useEvent = false;
	if (m_useTrigger) {
		typedef std::vector<std::string>::iterator Itr_s;
		for (Itr_s i = m_triggerStrings.begin(); i != m_triggerStrings.end(); ++i) {
			if (m_trigger->isPassed(*i)) {
				useEvent = true;
				if (debug) msg(MSG::DEBUG)<< "First requested trigger that fired is : "<< (*i) << " with prescale "<< m_trigger->getPrescale(*i);
				break;
			}
		}

		sc = this->triggerChainAnalysis();
		if (sc.isFailure()) {
			if (debug) msg(MSG::DEBUG) << "Problem checking Trigger Chains" << endreq;
			return sc;
		}
	}

	else {
		useEvent = true;
	}
	
	if( useEvent ) {
	        m_eventInfo = 0;
	        sc = evtStore()->retrieve(m_eventInfo);
	        if (sc.isFailure()) {
		        msg(MSG::WARNING) << "Failed to load EventInfo" << endreq;
		        return sc;
	        }
	        if( m_removeNoiseBursts ) {
			EventInfo::EventFlagErrorState efState = m_eventInfo->errorState(EventInfo::LAr);
		        if(efState == EventInfo::Warning || efState == EventInfo::Error) { 
			        useEvent = false;
			        for (unsigned char bit = 0; bit < 6; ++bit) {
			                if (m_eventInfo->isEventFlagBitSet(EventInfo::LAr, bit)) m_h_LAr_emNoisy->Fill(bit);
                                }
		        }
	        }
	}

	if (useEvent) {
		++m_numEvents;

		sc = this->loadContainers();
		if (sc.isFailure()) {
			msg(MSG::WARNING) << "Problem loading Athena Containers" << endreq;
			return sc;
		}
		
		if (debug) msg(MSG::DEBUG) << "Run number "<< m_eventInfo->event_ID()->run_number()<< " : Lumi Block "<< m_eventInfo->event_ID()->lumi_block() << " : Event "<< m_eventInfo->event_ID()->event_number() << endreq;

		// Look at vertex requirements
		int numVtx = 0, numTrk = 0;
		if (!vertexRequirementsPassed(numVtx, numTrk)) {
			if (debug) msg(MSG::DEBUG) << "Event " << m_eventInfo->event_ID()->event_number() << " fails vertex requirements " << endreq;
			return StatusCode::SUCCESS;
		}

		m_numOffElecInContainer = 0;
		m_numOffElecPassCuts = 0;
		m_numOffElecTriggered = 0;
		sc = this->analyseOfflineElectrons();
		if (sc.isFailure()) {
			msg(MSG::WARNING) << "analyseElectrons Failed " << endreq;
			//return sc;
		}
		if (debug) msg(MSG::DEBUG) << "Number of Offline Electrons = "<< m_numOffElecInContainer << " Passing Cuts = "<< m_numOffElecPassCuts << " Triggered = "<< m_numOffElecTriggered << endreq;

		m_numOffPhotInContainer = 0;
		m_numOffPhotPassCuts = 0;
		m_numOffPhotTriggered = 0;
		sc = this->analyseOfflinePhotons();
		if (sc.isFailure()) {
			msg(MSG::WARNING) << "analysePhotons Failed " << endreq;
			//return sc;
		}
		if (debug) msg(MSG::DEBUG) << "Number of Offline Photons = "<< m_numOffPhotInContainer << " Passing Cuts = "<< m_numOffPhotPassCuts << " Triggered = "<< m_numOffPhotTriggered << endreq;

	}

	if (debug)
		msg(MSG::DEBUG) << "Leaving fillHistograms" << endreq;

	return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/
StatusCode EmEfficienciesMonTool::procHistograms()
/*---------------------------------------------------------*/
{
	msg(MSG::DEBUG) << "procHistograms entered" << endreq;

	if (endOfLumiBlock) {
	}

	if (endOfRun) {
		msg(MSG::DEBUG) << "Number of offline electrons = " << m_numOffElec << endreq;
		msg(MSG::DEBUG) << "Number of offline photons = " << m_numOffPhot << endreq;
		msg(MSG::DEBUG) << "Number of events = " << m_numEvents << endreq;

		m_histTool->efficienciesForMerge(m_h_ClusterRaw_Et_gdEta,
				m_h_ClusterRaw_Et_triggered_gdEta,
				m_h_ClusterRaw_Et_triggered_Eff);

		m_histTool->efficienciesForMerge(m_h_ClusterRaw_Et_transR,
				m_h_ClusterRaw_Et_triggered_transR,
				m_h_ClusterRaw_Et_triggered_transR_Eff);		
		
		for (int i = 0; i < ROI_BITS; ++i) {
			if (!emType(i)) continue;
			m_histTool->efficienciesForMerge(m_h_ClusterRaw_Et_gdEta,
					m_h_ClusterRaw_Et_bitcheck[i],
					m_h_ClusterRaw_Et_bitcheck_Eff[i]);
			m_histTool->efficienciesForMerge(m_h_ClusterRaw_10GeV_Eta_vs_Phi,
					m_h_ClusterRaw_10GeV_Eta_vs_Phi_trig[i],
					m_h_ClusterRaw_10GeV_Eta_vs_Phi_trig_Eff[i]);
			m_histTool->efficienciesForMerge(m_h_ClusterRaw_20GeV_Eta_vs_Phi,
					m_h_ClusterRaw_20GeV_Eta_vs_Phi_trig[i],
					m_h_ClusterRaw_20GeV_Eta_vs_Phi_trig_Eff[i]);
			m_histTool->efficienciesForMerge(m_h_ClusterRaw_30GeV_Eta_vs_Phi,
					m_h_ClusterRaw_30GeV_Eta_vs_Phi_trig[i],
					m_h_ClusterRaw_30GeV_Eta_vs_Phi_trig_Eff[i]);
		}

	}

	return StatusCode::SUCCESS;
}

bool EmEfficienciesMonTool::emType(int bitNumber) {
        return (m_emBitMask>>bitNumber)&0x1;
}

/**********************************************/
//Analysis code for offline reconstructed electrons
//Compares electrons with EmTau RoIs
/**********************************************/
StatusCode EmEfficienciesMonTool::analyseOfflineElectrons() {
	//Access all of the offline reconstructed electron candidates in the event
	typedef ElectronContainer::const_iterator Itr_electrons;
	m_numOffElecInContainer = m_offlineElectrons->size();
	m_numEmObjTotal += m_numOffElecInContainer;

	// Create variables for electron properties
	double EtCEraw = 0.0, phiCEraw = 0.0, etaCEraw = 0.0;
	double phiCErawL1 = 0.0;
	// Create variable to determine if selecting the right type of electrons based on criteria in jO
	bool correctType, inEmRegion, inEmTrans;

	//Cycle through all of the offline reconstructed electrons
	Itr_electrons elItrE = m_offlineElectrons->end();
	for (Itr_electrons elItr = m_offlineElectrons->begin(); elItr != elItrE; ++elItr) {
		//Keep track of eta, phi and Et as these will be used often
		//----------------------------------------------------------------------
		getRawClusterValuesFromCells( const_cast<CaloCluster*> ((*elItr)->cluster()), EtCEraw, etaCEraw, phiCEraw);
		phiCErawL1 = (phiCEraw < 0.) ? phiCEraw + 2.*M_PI : phiCEraw;
		inEmRegion = inEgammaGoodEtaRange(etaCEraw);
		inEmTrans = inEMTransR(etaCEraw, 0);
		//----------------------------------------------------------------------

		bool unbiasedTrigger = false;
		if (m_passed_EF_Trigger) {
			if (m_passed_EF_SingleJet_Trigger) {
				unbiasedTrigger = isolatedEmObjectEF(phiCEraw, etaCEraw);
			} else if (m_passed_EF_egTau_Trigger) {
				if (m_passed_EF_SingleJet_Trigger) {
					unbiasedTrigger = isolatedEmObjectEF(phiCEraw, etaCEraw);
				} else {
					return StatusCode::SUCCESS;
				}
			} else {
				//unbiasedTrigger = isolatedEmObjectL1(phiCEraw, etaCEraw);
				unbiasedTrigger = true;
			}
		} else {
			unbiasedTrigger = true;
		}

		//If passed the trigger conditions then proceed to start analysis
		if (unbiasedTrigger) {

			correctType = false;
			m_numEmObjPassTrigger++;
			m_numOffElec++;

			//Check that the electron matches the IsEm type selected (if any was chosen in jobOptions file)
			correctType = correctIsEmElectron((*elItr));

			// Ask electron which is the highest isEm definition it has passed 
			int isEmCode = 0;
			std::string isEmLevel = isEmLevelElectron((*elItr), isEmCode);

			//Check if the reconstructed electron is reconstructed as a standard egamma object (not forward or softe)
			bool goodAuthor = (*elItr)->author(egammaParameters::AuthorElectron);
			if (!goodAuthor) {
				correctType = false;
			}

			//Have the correct type of electron so do some analysis with it   
			if (correctType) {
				//Update counters		
				m_numOffElecPassCuts++;

				if (EtCEraw > 0.0) {

					if (inEmRegion) {
						if(inEmTrans) {
							m_h_ClusterRaw_Et_transR->Fill(EtCEraw);
						} else {
							m_h_ClusterRaw_Et_gdEta->Fill(EtCEraw);
						}

						if (EtCEraw > 10) {
							m_histTool->fillCPMEtaVsPhi(m_h_ClusterRaw_10GeV_Eta_vs_Phi, etaCEraw, phiCErawL1);
						}
						if (EtCEraw > 20) {
							m_histTool->fillCPMEtaVsPhi(m_h_ClusterRaw_20GeV_Eta_vs_Phi, etaCEraw, phiCErawL1);
						}
						if (EtCEraw > 30) {
							m_histTool->fillCPMEtaVsPhi(m_h_ClusterRaw_30GeV_Eta_vs_Phi, etaCEraw, phiCErawL1);
						}
					}
				}

				//Set up useful numbers to keep track of RoI information
				double etaROI = 0.0, phiROI = 0.0;
				double dEtaClRaw = 0.0, dPhiClRaw = 0.0, dRClRaw = 1000, tempRminClRaw = 0.0;
				uint32_t ROIWord = 0;

				//Access the EmTauRoIs
				const std::vector<EmTau_ROI>& emrois(m_lvl1RoIs->getEmTauROIs());
				typedef std::vector<EmTau_ROI>::const_iterator Itr_emroi;

				//Iterate over all of the EmTauRoIs
				Itr_emroi roiItrE = emrois.end();
				for (Itr_emroi roiItr = emrois.begin(); roiItr != roiItrE; ++roiItr) {
					bool emThresholdPassed = false;
					if (m_useEmThresholdsOnly) {

						const std::vector<std::string>& thrPassed((*roiItr).getThresholdNames());
						typedef std::vector<std::string>::const_iterator Itr_s;
						Itr_s iE = thrPassed.end();
						for (Itr_s i = thrPassed.begin(); i != iE; ++i) {
							if ((*i).find("EM") != std::string::npos) {
								emThresholdPassed = true;
								break;
							}
						}
					} else {
						emThresholdPassed = true;
					}

					if (emThresholdPassed) {

						//Get useful values for the EmTauRoI
						etaROI = (*roiItr).getEta();
						phiROI = (*roiItr).getPhi();

						//Calculate the difference in eta and phi between the electron and RoI
						dEtaClRaw = etaCEraw - etaROI;
						dPhiClRaw = correctDeltaPhi(phiCEraw - phiROI);

						//Calculate delta R
						tempRminClRaw = sqrt(dEtaClRaw * dEtaClRaw + dPhiClRaw * dPhiClRaw);

						//Check if the new delta R is smaller than any previous delta R value.
						//In that case, keep track of the new RoI values
						if (tempRminClRaw < dRClRaw) {
							ROIWord = (*roiItr).getROIWord();
							dRClRaw = tempRminClRaw;
						}
					}
				}

				//Check to see if there was an RoI to match with an electron cluster
				if (dRClRaw != 1000) {
					m_numOffElecTriggered++;

					//Check if electron and RoI matched to a very good level (less than cut)
					if (dRClRaw < m_goodEMDeltaRMatch_Cut) {
						if (EtCEraw > 0.0) {
							if (inEmRegion) {
								if(inEmTrans) {
									m_h_ClusterRaw_Et_triggered_transR->Fill(EtCEraw);
								} else {
									m_h_ClusterRaw_Et_triggered_gdEta->Fill(EtCEraw);
								}

								//Look at each bit in the RoI word (using bitshift) to see which thresholds were
								//passed and which ones were not
								for (int k = 0; k < ROI_BITS; ++k) {
									if (emType(k) && ((ROIWord >> k) & 1)) {
										if(!inEmTrans) { m_h_ClusterRaw_Et_bitcheck[k]->Fill(EtCEraw); }

										if (EtCEraw > 10) {
											m_histTool->fillCPMEtaVsPhi(m_h_ClusterRaw_10GeV_Eta_vs_Phi_trig[k], etaCEraw, phiCErawL1);
										}
										if (EtCEraw > 20) {
											m_histTool->fillCPMEtaVsPhi(m_h_ClusterRaw_20GeV_Eta_vs_Phi_trig[k], etaCEraw, phiCErawL1);
										}
										if (EtCEraw > 30) {
											m_histTool->fillCPMEtaVsPhi(m_h_ClusterRaw_30GeV_Eta_vs_Phi_trig[k], etaCEraw, phiCErawL1);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
//Analysis code for offline reconstructed photons
//Compares photons with EmTau RoIs
//----------------------------------------------------------------------
StatusCode EmEfficienciesMonTool::analyseOfflinePhotons() {
	// Access all of the offline reconstructed photon candidates in the event
	typedef PhotonContainer::const_iterator Itr_photons;
	m_numOffPhotInContainer = m_offlinePhotons->size();
	m_numEmObjTotal += m_numOffPhotInContainer;

	// Variables for accessing properties of recosntructed photons
	double EtCPraw = 0.0, etaCPraw = 0.0, phiCPraw = 0.0;
	double phiCPrawL1 = 0.0;
	// Variable to check if photon is of the right type as defined in the jobOptions
	bool correctType, inEmRegion, inEmTrans;

	//Cycle through all of the offline reconstructed photons      
	Itr_photons phItrE = m_offlinePhotons->end();
	for (Itr_photons phItr = m_offlinePhotons->begin(); phItr != phItrE; ++phItr) {
		//Keep track of eta, phi and Et as these will be used often
		//----------------------------------------------------------------------
		getRawClusterValuesFromCells(const_cast<CaloCluster*> ((*phItr)->cluster()), EtCPraw, etaCPraw, phiCPraw);
		phiCPrawL1 = (phiCPraw < 0.) ? phiCPraw + 2.*M_PI : phiCPraw;
		inEmRegion = inEgammaGoodEtaRange(etaCPraw);
		inEmTrans = inEMTransR(etaCPraw, 0);
		//----------------------------------------------------------------------	

		//Check, based on trigger chain analysis, that we are using an unbiased trigger
		bool unbiasedTrigger = false;
		//Check that event has passed EF trigger
		if (m_passed_EF_Trigger) {
			if (m_passed_EF_SingleJet_Trigger && !m_passed_EF_MultiJet_Trigger) {
				unbiasedTrigger = isolatedEmObjectEF(phiCPraw, etaCPraw);
			} else {
				if (m_passed_EF_egTau_Trigger || m_passed_EF_MultiJet_Trigger) {
					return StatusCode::SUCCESS;
				} else {
					unbiasedTrigger = true;
				}
			}			
		} else if(m_passed_L1_Jet_Trigger) {
			unbiasedTrigger = isolatedEmObjectL1(phiCPraw, etaCPraw);			
		} else {
			return StatusCode::SUCCESS;
		}
		

		//If only after the highest energy photon, make sure only select the one that has the right index number
		//Otherwise select all of them	
		if (unbiasedTrigger) {
			//Check that the photon matches the IsEm type selected (if any was chosen in jobOptions file)
			correctType = false;
			m_numEmObjPassTrigger++;
			m_numOffPhot++;

			correctType = correctIsEmPhoton((*phItr));

			// Ask photon what is the highest isEm definition that was passed 
			int isEmCode = 0;
			std::string isEmLevel = isEmLevelPhoton((*phItr), isEmCode);

			//Check if the reconstructed photon is reconstructed as a standard egamma object 
			bool goodAuthor = (*phItr)->author(egammaParameters::AuthorPhoton);
			if (!goodAuthor) {
				correctType = false;
			}

			// Ask is there a conversion - no requirement for photons yet

			//Have the correct type of photon so do some analysis with it    
			if (correctType) {
				//Update counters		
				m_numOffPhotPassCuts++;

				if (EtCPraw > 0.0) {
					if (inEmRegion) {
						if(inEmTrans) {
							m_h_ClusterRaw_Et_transR->Fill(EtCPraw);
						} else {
							m_h_ClusterRaw_Et_gdEta->Fill(EtCPraw);
						}

						if (EtCPraw > 10) {
							m_histTool->fillCPMEtaVsPhi(m_h_ClusterRaw_10GeV_Eta_vs_Phi, etaCPraw, phiCPrawL1);
						}
						if (EtCPraw > 20) {
							m_histTool->fillCPMEtaVsPhi(m_h_ClusterRaw_20GeV_Eta_vs_Phi, etaCPraw, phiCPrawL1);
						}
						if (EtCPraw > 30) {
							m_histTool->fillCPMEtaVsPhi(m_h_ClusterRaw_30GeV_Eta_vs_Phi, etaCPraw, phiCPrawL1);
						}
					}
				}

				//Set up useful numbers to keep track of RoI information
				double etaROI = 0.0, phiROI = 0.0;
				double dEtaClRaw = 0.0, dPhiClRaw = 0.0, dRClRaw = 1000, tempRminClRaw = 0.0;
				uint32_t ROIWord = 0;

				//Access the EmTau RoIs
				const std::vector<EmTau_ROI>& emrois(m_lvl1RoIs->getEmTauROIs());
				typedef std::vector<EmTau_ROI>::const_iterator Itr_emroi;

				//Iterate over the EmTau RoIs 
				Itr_emroi roiItrE = emrois.end();
				for (Itr_emroi roiItr = emrois.begin(); roiItr != roiItrE; ++roiItr) {
					bool emThresholdPassed = false;
					if (m_useEmThresholdsOnly) {
						const std::vector<std::string>& thrPassed((*roiItr).getThresholdNames());
						typedef std::vector<std::string>::const_iterator Itr_s;
						Itr_s iE = thrPassed.end();
						for (Itr_s i = thrPassed.begin(); i != iE; ++i) {
							if ((*i).find("EM") != std::string::npos) {
								emThresholdPassed = true;
								break;
							}
						}
					} else {
						emThresholdPassed = true;
					}

					if (emThresholdPassed) {
						//Get useful values for the EmTauRoI
						etaROI = (*roiItr).getEta();
						phiROI = (*roiItr).getPhi();

						//Calculate the difference in eta and phi between the electron and RoI
						dEtaClRaw = etaCPraw - etaROI;
						dPhiClRaw = correctDeltaPhi(phiCPraw - phiROI);

						//Calculate deltaR
						tempRminClRaw = sqrt(dEtaClRaw * dEtaClRaw + dPhiClRaw * dPhiClRaw);

						//Check if the new deltaR is smaller than any previous deltaR value.
						//In that case, keep track of the new RoI values 
						if (tempRminClRaw < dRClRaw) {
							ROIWord = (*roiItr).getROIWord();
							dRClRaw = tempRminClRaw;
						}
					}
				}

				//Check to see if there was an RoI to match with a photon
				if (dRClRaw != 1000) {
					m_numOffPhotTriggered++;

					//Check if photon and RoI matched to a very good level (less than cut)
					if (dRClRaw < m_goodEMDeltaRMatch_Cut) {
						if (EtCPraw > 0.0) {
							if (inEmRegion) {
								if(inEmTrans) {
									m_h_ClusterRaw_Et_triggered_transR->Fill(EtCPraw); 
								} else {
									m_h_ClusterRaw_Et_triggered_gdEta->Fill(EtCPraw);
								}

								//Look at each bit in the RoI word (using bitshift) to see which thresholds were
								//passed and which ones were not
								for (int k = 0; k < ROI_BITS; ++k) {
									if (emType(k) && ((ROIWord >> k) & 1)) {
										if(!inEmTrans) { 
											m_h_ClusterRaw_Et_bitcheck[k]->Fill(EtCPraw);
										}

										if (EtCPraw > 10) {
											m_histTool->fillCPMEtaVsPhi(m_h_ClusterRaw_10GeV_Eta_vs_Phi_trig[k], etaCPraw, phiCPrawL1);
										}
										if (EtCPraw > 20) {
											m_histTool->fillCPMEtaVsPhi(m_h_ClusterRaw_20GeV_Eta_vs_Phi_trig[k], etaCPraw, phiCPrawL1);
										}
										if (EtCPraw > 30) {
											m_histTool->fillCPMEtaVsPhi(m_h_ClusterRaw_30GeV_Eta_vs_Phi_trig[k], etaCPraw, phiCPrawL1);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return StatusCode::SUCCESS;
}

//------------------------------------------------------------------
// The check to see if an object is triggered w.r.t. an RoI
// It can be done in two ways so allow it to handle either one
//------------------------------------------------------------------
bool EmEfficienciesMonTool::deltaMatch(double dEta, double dPhi, double dR, 
		double goodDEta, double goodDPhi, double goodDR) {
	
	// Calculate if object passes cuts
	bool Rmatch = (dR < goodDR);
	bool EPmatch = ((fabs(dEta) < goodDEta) && (fabs(dPhi) < goodDPhi));

	// First check that both checks are either on or off
	if (m_useDeltaRMatch && m_useDeltaEtaPhiMatch) {
		if (Rmatch && EPmatch) {
			return true;
		} else {
			return false;
		}
	} else if (!m_useDeltaRMatch && !m_useDeltaEtaPhiMatch) {
		return false;
	}

	// Only one check is being used so match against it
	if (m_useDeltaRMatch) {
		if (Rmatch) {
			return true;
		} else {
			return false;
		}
	} else if (m_useDeltaEtaPhiMatch) {
		if (EPmatch) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

//------------------------------------------------------------------
//Asks if there is at least one primary vertex with 3 tracks coming off it
//Useful for selecting collision events
//------------------------------------------------------------------
bool EmEfficienciesMonTool::vertexRequirementsPassed(int &numVtx, int &bestNumTracks) {
	
	bool goodVtx = false;

	//See if we can find the requested primary vertex collection    
	numVtx = m_primaryVtx->size();
	bestNumTracks = 0;

	//See if there are any vertices in the collection
	if (numVtx > 0) {
		//Set up iterators
		VxContainer::const_iterator vertexItr = m_primaryVtx->begin();
		VxContainer::const_iterator vertexItrE = m_primaryVtx->end();

		//Loop over vertices
		for (; vertexItr != vertexItrE; ++vertexItr) {
			//Find out if the vertex has at least 3 tracks coming from it
			std::vector<Trk::VxTrackAtVertex*>* trklist = (*vertexItr)->vxTrackAtVertex();
			int numTracks = trklist->size();

			if (numTracks > bestNumTracks) {
				bestNumTracks = numTracks;
			}

			if (numTracks >= 3) {
				//Met our requirement
				goodVtx = true;
				break;
			}
		}
	}

	return goodVtx;
}

//------------------------------------------------------------------
//Asks if there is at least one primary vertex with 3 tracks coming off it
//Useful for selecting collision events
//------------------------------------------------------------------
bool EmEfficienciesMonTool::emObjInDeadBadTower(double eta, double phi) {
	
	bool deadBadTower = false;
	int etaBin = 0, phiBin = 0;

	phiBin = 1 + (int) ((phi + M_PI) * 64.0 / (2 * M_PI));
	etaBin = 1 + (int) ((eta + 2.5) * 50.0 / (2 * 2.5)); //this will need fixing

	bool badCalo = (m_h_TrigTower_emBadCalo->GetBinContent(etaBin, phiBin) > 0);
	bool deadChannel = (m_h_TrigTower_emDeadChannel->GetBinContent(etaBin, phiBin) > 0);

	if (badCalo || deadChannel) {
		deadBadTower = true;
	}
	return deadBadTower;
}

//------------------------------------------------------------------
// Get the raw values of the CaloCluster if they are not provided
// directly by the cluster by accessing the cells that make it up
// Uses CaloCells so needs to use ESD files
//------------------------------------------------------------------
void EmEfficienciesMonTool::getRawClusterValuesFromCells(CaloCluster* cc,
                               double& et, double& eta, double& phi) {
	
	// Add the raw information of the cluster 
	double rawE = 0., rawEta = 0., rawPhi = 0., rawEt = 0.;
	double cellE = 0., cellEta = 0., cellPhi = 0.;

	// Variables for asking if cells cross +/-M_PI boundary
	double phihi = -M_PI, philo = +M_PI, phiDiff = -1;

	CaloCluster::cell_iterator ccIt = cc->cell_begin();
	CaloCluster::cell_iterator ccItE = cc->cell_end();

	// Check if the cells in the cluster cross +/-M_PI boundary
	// by asking what is the biggest phi difference between any of
	// the cells belonging to the cluster
	for (; ccIt != ccItE; ++ccIt) {
		const CaloCell* cell = (*ccIt);
		if (cell) {
			cellPhi = (*ccIt)->phi();
			if (cellPhi > phihi) {
				phihi = cellPhi;
			}
			if (cellPhi < philo) {
				philo = cellPhi;
			}
		}
	}
	// Calculate biggest phi difference
	phiDiff = phihi - philo;

	// Loop over the cells corresponding to the cluster
	for (ccIt = cc->cell_begin(); ccIt != ccItE; ++ccIt) {
		const CaloCell* cell = (*ccIt);
		if (cell) {
			// Get the cell values
			cellE = (*ccIt)->energy();
			cellEta = (*ccIt)->eta();
			cellPhi = (*ccIt)->phi();

			// If the cells cross boundary, shift some so it is continuous
			if (phiDiff > 4 && cellPhi < 0) {
				cellPhi += 2 * M_PI;
			}

			//Add to the total energy sum
			rawE += cellE;
			//Add to the energy weighted eta and phi sum
			rawPhi += cellE * cellPhi;
			rawEta += cellE * cellEta;

		} else {
			msg(MSG::WARNING) << "Problem with Cell within Cluster, check cell pointer: " << cell << endreq;
		}
	}

	if (rawE > 0) {
		//Unweight the raw eta and phi values
		rawEta /= rawE;
		rawPhi /= rawE;

		//If shifted phi value falls out of range, correct it
		if (phiDiff > 4 && rawPhi > M_PI) {
			rawPhi -= 2 * M_PI;
		}

		//Calculate the raw et from the energy and eta
		rawEt = rawE / (CLHEP::GeV * std::cosh(rawEta));

	} else {
		rawEt  = -10.0;
		rawEta = -10.0;
		rawPhi = -10.0;
	}

	et  = rawEt;
	eta = rawEta;
	phi = rawPhi;

	return;

}

//------------------------------------------------------------------
//Correct the value of deltaPhi so that it falls in +-M_PI range
//------------------------------------------------------------------
double EmEfficienciesMonTool::correctDeltaPhi(double dPhi) {
	if (fabs(dPhi) > M_PI) {
		dPhi = (dPhi > 0) ? dPhi - 2 * M_PI : dPhi + 2 * M_PI;
	}

	return dPhi;
}

//------------------------------------------------------------------
//Calculate delta R quickly between two objects
//------------------------------------------------------------------
double EmEfficienciesMonTool::calcDeltaR(double eta1, double phi1, double eta2,	double phi2) {
	double dEta = eta1 - eta2;
	double dPhi = correctDeltaPhi(phi1 - phi2);

	double dR = sqrt((dEta * dEta) + (dPhi * dPhi));
	return dR;
}

//------------------------------------------------------------------
//Check for electron that it is of the right isEm type as required from jobOptions
//------------------------------------------------------------------
bool EmEfficienciesMonTool::correctIsEmElectron(const Analysis::Electron* el) {
	bool correctType = false;
	switch (m_isEmType) {
	case 0: //"None"
		correctType = true;
		break;
	case 10: //"ElectronLoose" 
		correctType = el->passID(egammaPID::ElectronIDLoosePP);
		break;
	case 11: //"ElectronMedium"
		correctType = el->passID(egammaPID::ElectronIDMediumPP);
		break;
	case 12: //"ElectronTight" 
		correctType = el->passID(egammaPID::ElectronIDTightPP);
		break;
	case 30: //"ElectronLoose&PhotonLooseCombination" so just ask if electron loose
		correctType = el->passID(egammaPID::ElectronIDLoosePP);
		break;
	case 31: //"ElectronMedium&PhotonLooseCombination" so just ask if electron medium
		correctType = el->passID(egammaPID::ElectronIDMediumPP);
		break;
	case 32: //"ElectronTight&PhotonTightCombination" so just ask if electron tight
		correctType = el->passID(egammaPID::ElectronIDTightPP);
		break;
	default:
		correctType = false;
		break;
	}

	return correctType;
}

//------------------------------------------------------------------
//Check for photon that it is of the right isEm type as required from jobOptions
//------------------------------------------------------------------
bool EmEfficienciesMonTool::correctIsEmPhoton(const Analysis::Photon* ph) {
	bool correctType = false;
	switch (m_isEmType) {
	case 0: //"None"
		correctType = true;
		break;
	case 20: //"PhotonLoose" 
		correctType = ph->passID(egammaPID::PhotonIDLoose);
		break;
	case 21: //"PhotonMedium" 
		correctType = ph->passID(egammaPID::PhotonIDMedium);
		break;	
	case 22: //"PhotonTight" 
		correctType = ph->passID(egammaPID::PhotonIDTight);
		break;
	case 30: //"PhotonLoose&ElectronLooseCombination" so just ask if photon loose
		correctType = ph->passID(egammaPID::PhotonIDLoose);
		break;
	case 31: //"PhotonMedium&ElectronMediumCombination" so just ask if photon medium
		correctType = ph->passID(egammaPID::PhotonIDMedium);
		break;
	case 32: //"PhotonTight&ElectronTightCombination" so just ask if photon tight
		correctType = ph->passID(egammaPID::PhotonIDTight);
		break;
	default:
		correctType = false;
		break;
	}

	return correctType;
}

//------------------------------------------------------------------
//Check for electron that it is of the right isEm type as required from jobOptions
//------------------------------------------------------------------
std::string EmEfficienciesMonTool::isEmLevelElectron(const Analysis::Electron* el, int &code) {
	std::string isEmLevel = "None";
	code = 0;
	if (el->passID(egammaPID::ElectronIDLoosePP)) {
		isEmLevel = "Loose";
		code = 1;
	}
	if (el->passID(egammaPID::ElectronIDMediumPP)) {
		isEmLevel = "Medium";
		code = 2;
	}
	if (el->passID(egammaPID::ElectronIDTightPP)) {
		isEmLevel = "Tight";
		code = 3;
	}

	return isEmLevel;
}

//------------------------------------------------------------------
//Check for photon that it is of the right isEm type as required from jobOptions
//------------------------------------------------------------------
std::string EmEfficienciesMonTool::isEmLevelPhoton(const Analysis::Photon* ph, int &code) {
	std::string isEmLevel = "None";
	code = 0;
	if (ph->passID(egammaPID::PhotonIDLoose)) {
		isEmLevel = "Loose";
		code = 1;
	}
	if (ph->passID(egammaPID::PhotonIDMedium)) {
		isEmLevel = "Medium";
		code = 2;
	}	
	if (ph->passID(egammaPID::PhotonIDTight)) {
		isEmLevel = "Tight";
		code = 3;
	}

	return isEmLevel;
}

//------------------------------------------------------------------
// Ask if object is within well defined eta range for EM performance
//------------------------------------------------------------------
bool EmEfficienciesMonTool::inEgammaGoodEtaRange(double eta) {
	//For performance studies, take all egamma within LAr region
	if (fabs(eta) > 2.50) {
		return false;
	}
	return true;
}

//------------------------------------------------------------------
//Ask if object is within EM barrel
//------------------------------------------------------------------
bool EmEfficienciesMonTool::inEMBarrel(double eta, int sign) {
	bool inEB = (fabs(eta) <= 1.37);

	if (inEB) {
		if (sign < 0) {
			inEB = (eta < 0);
		} else if (sign > 0) {
			inEB = (eta > 0);
		}
	}

	return inEB;
}

//------------------------------------------------------------------
//Ask if object is within EM Transition region
//------------------------------------------------------------------
bool EmEfficienciesMonTool::inEMTransR(double eta, int sign) {
	bool inTR = (fabs(eta) > 1.37 && fabs(eta) < 1.52);

	if (inTR) {
		if (sign < 0) {
			inTR = (eta < 0);
		} else if (sign > 0) {
			inTR = (eta > 0);
		}
	}

	return inTR;
}

//------------------------------------------------------------------
//Ask if object is within EM endcap
//------------------------------------------------------------------
bool EmEfficienciesMonTool::inEMEndcap(double eta, int sign) {
	bool inEC = (fabs(eta) >= 1.52 && fabs(eta) < 2.50);

	if (inEC) {
		if (sign < 0) {
			inEC = (eta < 0);
		} else if (sign > 0) {
			inEC = (eta > 0);
		}
	}

	return inEC;
}

//------------------------------------------------------------------
//Ask if object is has no jets or jet RoIs nearby at L1
//Do this by assuming that the highest ET Jet RoI is the one causing
//the event to trigger at HLT level
//------------------------------------------------------------------
bool EmEfficienciesMonTool::isolatedEmObjectL1(double phi, double eta) {
	bool isolated = false, tagFound = false;
	double dREm = 10.0, dR_Max = 10.0;
	double ET_Max = -10.0;
	double etaROI = 0.0, phiROI = 0.0, ET_ROI = 0.0;

	//Cycle over the rois, get their properties and determine the distance from the object
	const std::vector<Jet_ROI>& jetROIs(m_lvl1RoIs->getJetROIs());
	typedef std::vector<Jet_ROI>::const_iterator Itr_jetroi;
	Itr_jetroi roiItrE = jetROIs.end();
	for (Itr_jetroi roiItr = jetROIs.begin(); roiItr != roiItrE; ++roiItr) {
		etaROI = (*roiItr).getEta();
		phiROI = (*roiItr).getPhi();
		ET_ROI = (*roiItr).getET8x8();
		dREm = calcDeltaR(eta, phi, etaROI, phiROI);

		//If this energy exceeds the current record then store the details
		if (ET_ROI > ET_Max) {
			ET_Max = ET_ROI;
			dR_Max = dREm;
			tagFound = true;
		}
	}

	// Check that the object is far away enough from highest ET jet RoI (and that a tag was found)
	if (dR_Max > m_goodHadDeltaRMatch_Cut && tagFound) {
		isolated = true;
	} else {
		isolated = false;
	}

	return isolated;
}

//------------------------------------------------------------------
//Ask if object is has no jets or jet RoIs nearby at EF
//------------------------------------------------------------------
bool EmEfficienciesMonTool::isolatedEmObjectEF(double phi, double eta) {
	//At present just call L1 function, improve this later
	return isolatedEmObjectL1(phi, eta);
}

//---------------------------------------------------------------
// Trigger Tower Analysis
//---------------------------------------------------------------
StatusCode EmEfficienciesMonTool::triggerTowerAnalysis() {
	// We will loop over all TriggerTowers, so we will use an iterator.
	// We use the ++i convention rather than i++. With i++ a copy has to be made and is
	// therefore a lot slower. Don't really matter for ints, certainly does for iterators

	m_dbPpmDeadChannels = 0;
	StatusCode sc = detStore()->retrieve(m_dbPpmDeadChannels, m_dbPpmDeadChannelsFolder);
	if (sc.isFailure()) {
		msg(MSG::WARNING) << "Failed to load DB PPM Dead Channels" << endreq;
		return sc;
	}

	m_triggerTowers = 0;
	sc = evtStore()->retrieve(m_triggerTowers, m_triggerTowersLocation);
	if (sc.isFailure()) {
		msg(MSG::WARNING) << "Failed to load Trigger Towers" << endreq;
		return sc;
	}

	// Look at trigger towers around physics objects

	typedef TriggerTowerCollection::const_iterator Itr_tt;
	typedef std::vector<int>::const_iterator Itr_i;
	Itr_tt ttItrE = m_triggerTowers->end();
	for (Itr_tt ttItr = m_triggerTowers->begin(); ttItr != ttItrE; ++ttItr) {
		
		// Get the values of eta and phi for the trigger towers
		double ttEta = (*ttItr)->eta();
		double ttPhi = (*ttItr)->phi();
		const L1CaloCoolChannelId emCoolId(m_ttTool->channelID(ttEta, ttPhi, 0));
		const Identifier emIdent(m_ttTool->identifier(ttEta, ttPhi, 0));

		// The dead channels folder (only has entries for dead channels - no entry = good channel)
		unsigned int emDead(0);
		CondAttrListCollection::const_iterator itr = m_dbPpmDeadChannels->chanAttrListPair(emCoolId.id());
		if (itr != m_dbPpmDeadChannels->end()) {
		        const AthenaAttributeList& attrList(itr->second);
		        emDead = attrList["ErrorCode"].data<unsigned int>();
                }

		if (m_larEnergy->hasMissingFEB(emIdent)) {
			m_histTool->fillCPMEtaVsPhi(m_h_TrigTower_emBadCalo, ttEta, ttPhi);
		}
		if (emDead) {
			m_histTool->fillCPMEtaVsPhi(m_h_TrigTower_emDeadChannel, ttEta, ttPhi);
		}
	}
	return StatusCode::SUCCESS;
}

//---------------------------------------------------------------
// Trigger Chain Analysis - find out which trigger chains have 
// passed so that we do not bias our efficiencies
//---------------------------------------------------------------
StatusCode EmEfficienciesMonTool::triggerChainAnalysis() {

	// Get the list of all triggers but do this only once in the event loop
	if (m_configuredChains.size() == 0) {
		m_configuredChains = m_trigger->getListOfTriggers();
		m_wantedTriggers.resize(m_configuredChains.size());
		int i = 0;
	        std::vector<std::string>::const_iterator itE = m_configuredChains.end();
		for (std::vector<std::string>::const_iterator it =
		      m_configuredChains.begin(); it != itE; ++it, ++i) {
			// L1 jet trigger items
		      	if ((*it).find("L1_J") != std::string::npos && (*it).find("L1_JE") == std::string::npos) {
				m_wantedTriggers[i] |= s_L1_Jet_Trigger_mask;
			}
			// EF trigger chains
			if ((*it).find("EF") != std::string::npos) {
				m_wantedTriggers[i] |= s_EF_Trigger_mask;
				
				//Event Filter chains corresponding to single jet triggers (keeping it simple)
				if (((*it).find("EF_j") != std::string::npos && (*it).find(
						"EF_je") == std::string::npos) || (*it).find("EF_fj")
						!= std::string::npos || ((*it).find("EF_L1J")
						!= std::string::npos && (*it).find("EMPTY")
						== std::string::npos) || (*it).find("EF_b")
						!= std::string::npos) {
					m_wantedTriggers[i] |= s_EF_SingleJet_Trigger_mask;
				}
				//Event Filter chains corresponding to multiple jet triggers (worry about it later)
				if ((*it).find("EF_2j") != std::string::npos || (*it).find(
						"EF_4j") != std::string::npos) {
					m_wantedTriggers[i] |= s_EF_MultiJet_Trigger_mask;
				}
				//Event Filter chains corresponding to photons
				if ((*it).find("EF_g") != std::string::npos || (*it).find(
						"EF_2g") != std::string::npos || (*it).find("EF_4g")
						!= std::string::npos) {
					m_wantedTriggers[i] |= s_EF_egTau_Trigger_mask;
				}
				//Event Filter chains corresponding to electrons
				if ((*it).find("EF_e") != std::string::npos || (*it).find(
						"EF_2e") != std::string::npos || (*it).find("EF_4e")
						!= std::string::npos) {
					m_wantedTriggers[i] |= s_EF_egTau_Trigger_mask;
				}
				//Event Filter chains corresponding to taus
				if ((*it).find("EF_tau") != std::string::npos || (*it).find(
						"EF_2tau") != std::string::npos
						|| (*it).find("EF_4tau") != std::string::npos) {
					m_wantedTriggers[i] |= s_EF_egTau_Trigger_mask;
				}
				//Event Filter chains corresponding to missing energy
				//Missing energy could come from electron so it may bias results
				if ((*it).find("EF_xe") != std::string::npos || (*it).find(
						"EF_xs") != std::string::npos) {
					m_wantedTriggers[i] |= s_EF_egTau_Trigger_mask;
				}

				//Any event filter chains which do not pass any of these checks
				//would be treated as unbiased triggers
			}
		}
	}

	//msg(MSG::DEBUG) << "Trigger Analysis: New Event" << endreq;

	int trigSet = 0;
	int i = 0;
	std::vector<std::string>::const_iterator itE = m_configuredChains.end();
	for (std::vector<std::string>::const_iterator it =
			m_configuredChains.begin(); it != itE; ++it, ++i) {

		//Check that the trigger was passed
		const int trigMap = m_wantedTriggers[i];
		if (((trigMap^trigSet)&trigMap) && m_trigger->isPassed(*it)) {
			trigSet |= trigMap;
		}
	}
	m_passed_L1_Jet_Trigger       = (trigSet & s_L1_Jet_Trigger_mask);
	m_passed_EF_Trigger           = (trigSet & s_EF_Trigger_mask);
	m_passed_EF_SingleJet_Trigger = (trigSet & s_EF_SingleJet_Trigger_mask);
	m_passed_EF_MultiJet_Trigger  = (trigSet & s_EF_MultiJet_Trigger_mask);
	m_passed_EF_egTau_Trigger     = (trigSet & s_EF_egTau_Trigger_mask);

	return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------------
// Load important containers
//------------------------------------------------------------------------------------
StatusCode EmEfficienciesMonTool::loadContainers() {
	StatusCode sc;

	m_primaryVtx = 0;
	sc = evtStore()->retrieve(m_primaryVtx, m_primaryVertexLocation);
	if (sc.isFailure()) {
		msg(MSG::WARNING) << "Failed to load Primary Vertices" << endreq;
		return sc;
	}

	m_lvl1RoIs = 0;
	sc = evtStore()->retrieve(m_lvl1RoIs, m_lvl1RoIsLocation);
	if (sc.isFailure()) {
		msg(MSG::WARNING) << "Failed to load LVL1 RoIs" << endreq;
		return sc;
	}

	m_offlineElectrons = 0;
	sc = evtStore()->retrieve(m_offlineElectrons, m_offlineElectronsLocation);
	if (sc.isFailure()) {
		msg(MSG::WARNING) << "Failed to load Offline Electrons" << endreq;
		return sc;
	}

	m_offlinePhotons = 0;
	sc = evtStore()->retrieve(m_offlinePhotons, m_offlinePhotonsLocation);
	if (sc.isFailure()) {
		msg(MSG::WARNING) << "Failed to load Offline Photons" << endreq;
		return sc;
	}

	return sc;
}
