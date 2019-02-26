/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     JetEfficienciesMonTool.cxx
// PACKAGE:  TrigT1CaloMonitoring
//
// AUTHOR:   Hardeep Bansil
//           Adapted for monitoring: Peter Faulkner
//
//
// ********************************************************************

#include <cmath>
#include <cstdio>

#include "TH1.h"
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
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxTrackAtVertex.h"
// #include "JetSelectorTools/JetCleaningTool.h"
#include "JetInterface/IJetSelector.h"

#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "Identifier/Identifier.h"

#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloEvent/CaloCellContainer.h"
#include "TileEvent/TileCell.h"
#include "TileEvent/TileTTL1Cell.h"
#include "TileConditions/TileCablingService.h"


#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramToolV1.h"

#include "JetEfficienciesMonTool.h"
// ============================================================================
namespace LVL1 {
// ============================================================================
using xAOD::Jet;
using xAOD::JetContainer;
// ============================================================================
JetEfficienciesMonTool::JetEfficienciesMonTool(const std::string & type,
        const std::string & name, const IInterface* parent)
	: ManagedMonitorToolBase(type, name, parent),
	  m_errorTool("LVL1::TrigT1CaloMonErrorToolV1/TrigT1CaloMonErrorToolV1"),
	  m_histTool("LVL1::TrigT1CaloLWHistogramToolV1/TrigT1CaloLWHistogramToolV1"),
	  m_ttTool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"),
	  m_larEnergy("LVL1::L1CaloLArTowerEnergy/L1CaloLArTowerEnergy"),
	  // m_looseJetSelector("JetCleaningTool/JetCleaningLooseTool"),
	  // m_mediumJetSelector("JetCleaningTool/JetCleaningMediumTool"),
	  // m_tightJetSelector("JetCleaningTool/JetCleaningTightTool"),
	  m_tileID(0),
	  m_TT_ID(0),
	  m_tileCablingService(0),
	  m_dbPpmDeadChannelsFolder("/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels"),
	  m_triggerTowersLocation("TriggerTowers"),
	  m_caloCellContainerLocation("AllCalo"),
	  m_lvl1RoIsLocation("LVL1_ROI"),
	  m_offlineJetsLocation("AntiKt4TopoEMJets"),
	  m_primaryVertexLocation("VxPrimaryCandidate"),
	  m_eventInfo(0),
	  m_dbPpmDeadChannels(0),
	  m_triggerTowers(0),
	  m_lvl1RoIs(0),
	  m_offlineJets(0),
	  m_primaryVertex(0),
	  m_numEvents(0),
	  m_numOffJets(0),
	  m_numOffJetsInContainer(0),
	  m_numOffJetsPassCuts(0),
	  m_numOffJetsTriggered(0),
	  m_numJetObjPassTrigger(0),
	  m_numJetObjTotal(0),
	  m_passed_L1_EM_Trigger(false),
	  m_passed_EF_Trigger(false),
	  m_passed_EF_SingleJet_Trigger(false),
	  m_passed_EF_SingleEgamma_Trigger(false),
	  m_passed_EF_SingleEgamma_Trigger_HighestVH(false),
	  m_passed_EF_MultiJet_Trigger(false),
	  m_passed_EF_Tau_Trigger(false),
	  m_passed_EF_MissingEnergy_Trigger(false),
	  m_firstEvent(true),
	  m_h_JetEmScale_Et(0),
	  m_h_JetEmScale_Et_central(0),
	  m_h_JetEmScale_Et_forward(0),
	  m_h_JetEmScale_Et_triggered(0),
	  m_h_JetEmScale_Et_triggered_central(0),
	  m_h_JetEmScale_Et_triggered_forward(0),
	  m_h_JetEmScale_Et_triggered_Eff(0),
	  m_h_JetEmScale_Et_triggered_central_Eff(0),
	  m_h_JetEmScale_Et_triggered_forward_Eff(0),
	  m_h_JetEmScale_Eta_vs_Phi(0),
	  m_h_JetEmScale_Eta_vs_Phi_triggered(0),
	  m_h_JetEmScale_Eta_vs_Phi_triggered_Eff(0),
	  m_h_JetEmScale_50GeV_Eta_vs_Phi(0),
	  m_h_JetEmScale_100GeV_Eta_vs_Phi(0),
	  m_h_JetEmScale_200GeV_Eta_vs_Phi(0),
	  m_h_TrigTower_jetBadCalo(0),
	  m_h_TrigTower_jetDeadChannel(0),
	  m_h_LAr_jetNoisy(0),
	  m_h_nPriVtx(0)

	  /*---------------------------------------------------------*/
{

	declareProperty("HistogramTool", m_histTool);
	declareProperty("TriggerTowerTool", m_ttTool);
	declareProperty("LArTowerEnergyTool", m_larEnergy);

	declareProperty("JetCleaningLooseTool", m_looseJetSelector);
	declareProperty("JetCleaningMediumTool", m_mediumJetSelector);
	declareProperty("JetCleaningTightTool", m_tightJetSelector);

	declareProperty("DeadChannelsFolder", m_dbPpmDeadChannelsFolder);
	declareProperty("TriggerTowersLocation", m_triggerTowersLocation);
	declareProperty("RoIsLocation", m_lvl1RoIsLocation);
	declareProperty("OfflineJetsLocation", m_offlineJetsLocation);
	declareProperty("PrimaryVertexLocation", m_primaryVertexLocation);
	declareProperty("CaloCellContainerLocation", m_caloCellContainerLocation);

	declareProperty("RootDirectory", m_rootDir = "L1Calo");
	declareProperty("UseTrigger", m_useTrigger = true);
	declareProperty("TriggerStrings", m_triggerStrings);

	// HSB - python cuts
	declareProperty("goodEMDeltaRMatch_Cut", m_goodEMDeltaRMatch_Cut = 0.5);
	declareProperty("goodHadDeltaRMatch_Cut", m_goodHadDeltaRMatch_Cut = 0.2);
	declareProperty("goodHadDeltaPhiMatch_Cut", m_goodHadDeltaPhiMatch_Cut = 0.3);
	declareProperty("UseEmThresholdsOnly", m_useEmThresholdsOnly = true);
	declareProperty("JetQualityLevel", m_jetQualityLevel = 30);
	declareProperty("NtracksAtPrimaryVertex", m_nTracksAtPrimaryVertex = 4);
	declareProperty("HadCoreVHCut", m_hadCoreVHCut = 1000);
	declareProperty("RemoveNoiseBursts", m_removeNoiseBursts = true);

	for (int i = 0; i < JET_ROI_BITS; ++i) {
		m_h_JetEmScale_Et_J_item[i] = 0;
		m_h_JetEmScale_Et_J_Eff_item[i] = 0;
		m_h_JetEmScale_50GeV_Eta_vs_Phi_J_item[i] = 0;
		m_h_JetEmScale_50GeV_Eta_vs_Phi_J_Eff_item[i] = 0;
		m_h_JetEmScale_100GeV_Eta_vs_Phi_J_item[i] = 0;
		m_h_JetEmScale_100GeV_Eta_vs_Phi_J_Eff_item[i] = 0;
		m_h_JetEmScale_200GeV_Eta_vs_Phi_J_item[i] = 0;
		m_h_JetEmScale_200GeV_Eta_vs_Phi_J_Eff_item[i] = 0;
	}
	for (int i = 0; i < FJET_ROI_BITS; ++i) {
		m_h_JetEmScale_Et_FJ_J_item[i] = 0;
		m_h_JetEmScale_Et_FJ_J_Eff_item[i] = 0;
		m_h_JetEmScale_50GeV_Eta_vs_Phi_FJ_J_item[i] = 0;
		m_h_JetEmScale_50GeV_Eta_vs_Phi_FJ_J_Eff_item[i] = 0;
		m_h_JetEmScale_100GeV_Eta_vs_Phi_FJ_J_item[i] = 0;
		m_h_JetEmScale_100GeV_Eta_vs_Phi_FJ_J_Eff_item[i] = 0;
		m_h_JetEmScale_200GeV_Eta_vs_Phi_FJ_J_item[i] = 0;
		m_h_JetEmScale_200GeV_Eta_vs_Phi_FJ_J_Eff_item[i] = 0;
		m_linkedHistos[i] = 0;
	}
}

/*---------------------------------------------------------*/
JetEfficienciesMonTool::~JetEfficienciesMonTool()
/*---------------------------------------------------------*/
{
}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode JetEfficienciesMonTool::initialize()
/*---------------------------------------------------------*/
{
	msg(MSG::INFO) << "Initializing " << name() << " - package version " << PACKAGE_VERSION << endmsg;

	StatusCode sc;

	sc = ManagedMonitorToolBase::initialize();
	if (sc.isFailure())
		return sc;

	sc = m_errorTool.retrieve();
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloMonErrorTool" << endmsg;
		return sc;
	}

	sc = m_histTool.retrieve();
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloHistogramTool" << endmsg;
		return sc;
	}

	sc = m_ttTool.retrieve();
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Cannot retrieve L1TriggerTowerTool" << endmsg;
		return sc;
	}

	sc = m_larEnergy.retrieve();
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Cannot retrieve L1CaloLArTowerEnergy" << endmsg;
		return sc;
	}

	sc = m_looseJetSelector.retrieve();
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Can't get handle on JetCleaningTool/JetCleaningToolLoose" << endmsg;
		return sc;
	}

	sc = m_mediumJetSelector.retrieve();
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Can't get handle on JetCleaningTool/JetCleaningToolMedium" << endmsg;
		return sc;
	}

	sc = m_tightJetSelector.retrieve();
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Can't get handle on JetCleaningTool/JetCleaningTooTight" << endmsg;
		return sc;
	}


	// retrieve CaloLVL1_ID, TileID, from det store
	sc = detStore()->retrieve(m_TT_ID);
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Unable to retrieve CaloLVL1_ID helper from DetectorStore" << endmsg;
		return sc;
	}

	sc = detStore()->retrieve(m_tileID);
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Unable to retrieve TileID helper from DetectorStore" << endmsg;
		return sc;
	}

	m_tileCablingService = TileCablingService::getInstance();

	std::vector<std::string>::const_iterator iter = m_triggerStrings.begin();
	std::vector<std::string>::const_iterator iterE = m_triggerStrings.end();
	msg(MSG::INFO) << "TriggerStrings:";
	for (; iter != iterE; ++iter)
		msg(MSG::INFO) << " " << *iter;
	msg(MSG::INFO) << endmsg;

	return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode JetEfficienciesMonTool::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
	msg(MSG::DEBUG) << "bookHistograms entered" << endmsg;

	if (m_environment == AthenaMonManager::online) {
		// book histograms that are only made in the online environment...
	}

	if (m_dataType == AthenaMonManager::cosmics) {
		// book histograms that are only relevant for cosmics data...
	}

	//if (newLumiBlock) {
	//}

	if (newRunFlag()) {

		MgmtAttr_t attr = ATTRIB_UNMANAGED;
		std::string dir(m_rootDir + "/Reco/JetEfficiencies");

		MonGroup monJetDead(this, dir + "/DeadOrBadChannels", run, attr, "", "lowerLB");
		MonGroup monJetNoisy(this, dir + "/DeadOrBadChannels", run, attr);
		MonGroup monJetEmScaleNum(this, dir + "/JetEmScale_Et/numerator", run, attr);
		MonGroup monJetEmScaleDen(this, dir + "/JetEmScale_Et/denominator", run, attr);
		MonGroup monJetEmScaleEff(this, dir + "/JetEmScale_Et", run, attr, "", "perBinEffPerCent");
		MonGroup monJetEmScaleVtx(this, dir + "/JetEmScale_Et", run, attr);
		MonGroup monJetEmScale50GeVNum(this, dir + "/JetEmScale_50GeV_EtaVsPhi/numerator", run, attr);
		MonGroup monJetEmScale50GeVDen(this, dir + "/JetEmScale_50GeV_EtaVsPhi/denominator", run, attr);
		MonGroup monJetEmScale50GeVEff(this, dir + "/JetEmScale_50GeV_EtaVsPhi", run, attr, "", "perBinEffPerCent");
		MonGroup monJetEmScale100GeVNum(this, dir + "/JetEmScale_100GeV_EtaVsPhi/numerator", run, attr);
		MonGroup monJetEmScale100GeVDen(this, dir + "/JetEmScale_100GeV_EtaVsPhi/denominator", run, attr);
		MonGroup monJetEmScale100GeVEff(this, dir + "/JetEmScale_100GeV_EtaVsPhi", run, attr, "", "perBinEffPerCent");
		MonGroup monJetEmScale200GeVNum(this, dir + "/JetEmScale_200GeV_EtaVsPhi/numerator", run, attr);
		MonGroup monJetEmScale200GeVDen(this, dir + "/JetEmScale_200GeV_EtaVsPhi/denominator", run, attr);
		MonGroup monJetEmScale200GeVEff(this, dir + "/JetEmScale_200GeV_EtaVsPhi", run, attr, "", "perBinEffPerCent");

		//Set up JET thresholds arrays with threshold names
		std::string thrNum[JET_ROI_BITS] = { "0", "1", "2", "3", "4", "5", "6", "7" };
		TrigConf::L1DataDef def;
		std::vector < std::string > jetL1t;
		m_histTool->thresholdNames(def.jetType(), jetL1t);
		int size = jetL1t.size();
		for (int i = 0; i < JET_ROI_BITS; ++i) {
			if (i < size)
				jetL1t[i] = "L1_" + jetL1t[i];
			else
				jetL1t.push_back("L1_??");
		}
		std::vector < std::string > fjetL1t;
		m_histTool->thresholdNames(def.jfType(), fjetL1t);
		size = fjetL1t.size();
		for (int i = 0; i < FJET_ROI_BITS; ++i) {
			if (i < size)
				fjetL1t[i] = "L1_" + fjetL1t[i];
			else
				fjetL1t.push_back("L1_??");
		}

		//work out which L1_Jet histogram connects to which L1_ForwardJet histogram
		int count = 0;
		std::vector < std::string > fjetL1n(FJET_ROI_BITS);
		for (int i = 0; i < FJET_ROI_BITS; i++) {
			m_linkedHistos[i] = 0;
			std::string strTemp = fjetL1t[i].substr(5);
			for (int j = 0; j < JET_ROI_BITS; j++) {
				std::string strTemp2 = jetL1t[j].substr(4);
				if (strTemp == strTemp2) {
					m_linkedHistos[i] = j;
					++count;
					break;
				}
			}
			fjetL1n[i] = fjetL1t[i] + " || " + jetL1t[m_linkedHistos[i]];
		}
		if (count != FJET_ROI_BITS) {
			msg(MSG::WARNING) << "Jet ForwardJet mismatch" << endmsg;
		}

		m_histTool->setMonGroup(&monJetDead);

		m_h_TrigTower_jetDeadChannel = m_histTool->bookPPMHadEtaVsPhi("TrigTower_jetDeadChannel", "Jet Trigger Towers with dead channels - #eta against #phi (E_{T} > 5 GeV)");

		m_h_TrigTower_jetBadCalo = m_histTool->bookPPMHadEtaVsPhi("TrigTower_jetBadCalo", "Jet Trigger Towers - Missing FEBs/Tile Quality - #eta against #phi (E_{T} > 5 GeV)");

		if (m_removeNoiseBursts) {

			m_histTool->setMonGroup(&monJetNoisy);

			m_h_LAr_jetNoisy = m_histTool->book1F("LAr_jetNoisy", "LAr Error Bits in Rejected Events", 6, 0, 6);
			LWHist::LWHistAxis* axis = m_h_LAr_jetNoisy->GetXaxis();
			axis->SetBinLabel(1, "BadFEBs");
			axis->SetBinLabel(2, "MediumSat");
			axis->SetBinLabel(3, "TightSat");
			axis->SetBinLabel(4, "NoiseWindow");
			axis->SetBinLabel(5, "Corrupt");
			axis->SetBinLabel(6, "CorruptWindow");
		}

		//Raw Jet Histograms

		m_histTool->setMonGroup(&monJetEmScaleDen);

		const int etbins = 200;
		const double etmax = 1000.;
		m_h_JetEmScale_Et = m_histTool->book1F("JetEmScale_Et", "Raw Jet E_{T};E_{T} Jet [GeV];Jets", etbins, 0., etmax);
		m_h_JetEmScale_Et_forward = m_histTool->book1F("JetEmScale_Et_forward", "Raw Jet E_{T} Forward;E_{T} Jet [GeV];Jets", etbins, 0., etmax);
		m_h_JetEmScale_Et_central = m_histTool->book1F("JetEmScale_Et_central", "Raw Jet E_{T} Central;E_{T} Jet [GeV];Jets", etbins, 0., etmax);
		m_h_JetEmScale_Eta_vs_Phi = m_histTool->bookPPMHadEtaVsPhi("JetEmScale_Eta_vs_Phi", "Raw Jet #eta v #phi");

		m_histTool->setMonGroup(&monJetEmScaleNum);

		m_h_JetEmScale_Et_triggered = m_histTool->book1F("JetEmScale_Et_triggered", "Raw Jet E_{T} (Triggered);E_{T} Jet [GeV];Jets", etbins, 0., etmax);
		m_h_JetEmScale_Et_triggered_forward = m_histTool->book1F("JetEmScale_Et_triggered_forward", "Raw Jet E_{T} Forward (Triggered);E_{T} Jet [GeV];Jets", etbins, 0., etmax);
		m_h_JetEmScale_Et_triggered_central = m_histTool->book1F("JetEmScale_Et_triggered_central", "Raw Jet E_{T} Central (Triggered);E_{T} Jet [GeV];Jets", etbins, 0., etmax);
		m_h_JetEmScale_Eta_vs_Phi_triggered = m_histTool->bookPPMHadEtaVsPhi("JetEmScale_Eta_vs_Phi_triggered", "Raw Jet #eta v #phi (Triggered)");

		std::string name;
		std::string title;
		for (int i = 0; i < JET_ROI_BITS; ++i) {
			name = "JetEmScale_Et_J_item_" + thrNum[i];
			title = "Raw E_{T} for Triggered Jets passing " + jetL1t[i] + ";E_{T} Jet [GeV];Jets";
			m_h_JetEmScale_Et_J_item[i] =  m_histTool->book1F(name, title,  etbins, 0., etmax);
			if (i < FJET_ROI_BITS) {
				name = "JetEmScale_Et_FJ_J_item_" + thrNum[i];
				title = "Raw E_{T} for Triggered Jets passing " + fjetL1n[i] + ";E_{T} Jet [GeV];Jets";
				m_h_JetEmScale_Et_FJ_J_item[i] = m_histTool->book1F(name, title,  etbins, 0., etmax);
			}
		}

		m_histTool->setMonGroup(&monJetEmScaleEff);

		m_h_JetEmScale_Et_triggered_Eff = m_histTool->book1F("JetEmScale_Et_triggered_Eff", "Raw Jet E_{T} (Triggered) Efficiency;E_{T} Jet [GeV];Efficiency %", etbins, 0., etmax);
		m_h_JetEmScale_Et_triggered_forward_Eff = m_histTool->book1F("JetEmScale_Et_triggered_forward_Eff", "Raw Jet E_{T} Forward (Triggered) Efficiency;E_{T} Jet [GeV];Efficiency %", etbins, 0., etmax);
		m_h_JetEmScale_Et_triggered_central_Eff = m_histTool->book1F("JetEmScale_Et_triggered_central_Eff", "Raw Jet E_{T} Central (Triggered) Efficiency;E_{T} Jet [GeV];Efficiency %", etbins, 0., etmax);
		m_h_JetEmScale_Eta_vs_Phi_triggered_Eff = m_histTool->bookPPMHadEtaVsPhi("JetEmScale_Eta_vs_Phi_triggered_Eff", "Raw Jet #eta v #phi Efficiency (%)");

		for (int i = 0; i < JET_ROI_BITS; ++i) {
			name = "JetEmScale_Et_J_Eff_item_" + thrNum[i];
			title = "Raw E_{T} for Triggered Jets passing " + jetL1t[i] + " Efficiency;E_{T} Jet [GeV];Efficiency %";
			m_h_JetEmScale_Et_J_Eff_item[i] =  m_histTool->book1F(name, title,  etbins, 0., etmax);
			if (i < FJET_ROI_BITS) {
				name = "JetEmScale_Et_FJ_J_Eff_item_" + thrNum[i];
				title = "Raw E_{T} for Triggered Jets passing " + fjetL1n[i] + " Efficiency;E_{T} Jet [GeV]; Efficiency %";
				m_h_JetEmScale_Et_FJ_J_Eff_item[i] = m_histTool->book1F(name, title,  etbins, 0., etmax);
			}
		}

		m_histTool->setMonGroup(&monJetEmScaleVtx);

		m_h_nPriVtx = m_histTool->book1F("nPriVtx", "Primary Vertex Multiplicity", 30, 0, 30);

		m_histTool->setMonGroup(&monJetEmScale50GeVNum);

		//Et Raw Jet greater > 50 GeV
		for (int i = 0; i < JET_ROI_BITS; ++i) {
			name = "JetEmScale_50GeV_Eta_vs_Phi_J_item_" + thrNum[i];
			title = "Raw Jet #eta v #phi (triggered on " + jetL1t[i] + " with E_{T} > 50 GeV)";
			m_h_JetEmScale_50GeV_Eta_vs_Phi_J_item[i] = m_histTool->bookPPMHadEtaVsPhi(name, title);
			if (i < FJET_ROI_BITS) {
				name = "JetEmScale_50GeV_Eta_vs_Phi_FJ_J_item_" + thrNum[i];
				title = "Raw Jet #eta v #phi (triggered on " + fjetL1n[i] + " with E_{T} > 50 GeV)";
				m_h_JetEmScale_50GeV_Eta_vs_Phi_FJ_J_item[i] = m_histTool->bookPPMHadEtaVsPhi(name, title);
			}
		}

		m_histTool->setMonGroup(&monJetEmScale50GeVDen);

		m_h_JetEmScale_50GeV_Eta_vs_Phi = m_histTool->bookPPMHadEtaVsPhi("JetEmScale_50GeV_Eta_vs_Phi", "Raw Jet #eta v #phi (E_{T} > 50 GeV)");

		m_histTool->setMonGroup(&monJetEmScale50GeVEff);

		for (int i = 0; i < JET_ROI_BITS; ++i) {
			name = "JetEmScale_50GeV_Eta_vs_Phi_J_Eff_item_" + thrNum[i];
			title = "Raw Jet #eta v #phi (triggered on " + jetL1t[i] + " with E_{T} > 50 GeV) Efficiency (%)";
			m_h_JetEmScale_50GeV_Eta_vs_Phi_J_Eff_item[i] = m_histTool->bookPPMHadEtaVsPhi(name, title);
			if (i < FJET_ROI_BITS) {
				name = "JetEmScale_50GeV_Eta_vs_Phi_FJ_J_Eff_item_" + thrNum[i];
				title = "Raw Jet #eta v #phi (triggered on " + fjetL1n[i] + " with E_{T} > 50 GeV) Efficiency (%)";
				m_h_JetEmScale_50GeV_Eta_vs_Phi_FJ_J_Eff_item[i] = m_histTool->bookPPMHadEtaVsPhi(name, title);
			}
		}

		m_histTool->setMonGroup(&monJetEmScale100GeVNum);

		//Et Raw Jet greater > 100 GeV
		for (int i = 0; i < JET_ROI_BITS; ++i) {
			name = "JetEmScale_100GeV_Eta_vs_Phi_J_item_" + thrNum[i];
			title = "Raw Jet #eta v #phi (triggered on " + jetL1t[i] + " with E_{T} > 100 GeV)";
			m_h_JetEmScale_100GeV_Eta_vs_Phi_J_item[i] = m_histTool->bookPPMHadEtaVsPhi(name, title);
			if (i < FJET_ROI_BITS) {
				name = "JetEmScale_100GeV_Eta_vs_Phi_FJ_J_item_" + thrNum[i];
				title = "Raw Jet #eta v #phi (triggered on " + fjetL1n[i] + " with E_{T} > 100 GeV)";
				m_h_JetEmScale_100GeV_Eta_vs_Phi_FJ_J_item[i] = m_histTool->bookPPMHadEtaVsPhi(name, title);
			}
		}

		m_histTool->setMonGroup(&monJetEmScale100GeVDen);

		m_h_JetEmScale_100GeV_Eta_vs_Phi = m_histTool->bookPPMHadEtaVsPhi("JetEmScale_100GeV_Eta_vs_Phi", "Raw Jet #eta v #phi (E_{T} > 100 GeV)");

		m_histTool->setMonGroup(&monJetEmScale100GeVEff);

		for (int i = 0; i < JET_ROI_BITS; ++i) {
			name = "JetEmScale_100GeV_Eta_vs_Phi_J_Eff_item_" + thrNum[i];
			title = "Raw Jet #eta v #phi (triggered on " + jetL1t[i] + " with E_{T} > 100 GeV) Efficiency (%)";
			m_h_JetEmScale_100GeV_Eta_vs_Phi_J_Eff_item[i] = m_histTool->bookPPMHadEtaVsPhi(name, title);
			if (i < FJET_ROI_BITS) {
				name = "JetEmScale_100GeV_Eta_vs_Phi_FJ_J_Eff_item_" + thrNum[i];
				title = "Raw Jet #eta v #phi (triggered on " + fjetL1n[i] + " with E_{T} > 100 GeV) Efficiency (%)";
				m_h_JetEmScale_100GeV_Eta_vs_Phi_FJ_J_Eff_item[i] = m_histTool->bookPPMHadEtaVsPhi(name, title);
			}
		}

		m_histTool->setMonGroup(&monJetEmScale200GeVNum);

		//Et Raw Jet greater > 200 GeV
		for (int i = 0; i < JET_ROI_BITS; ++i) {
			name = "JetEmScale_200GeV_Eta_vs_Phi_J_item_" + thrNum[i];
			title = "Raw Jet #eta v #phi (triggered on " + jetL1t[i] + " with E_{T} > 200 GeV)";
			m_h_JetEmScale_200GeV_Eta_vs_Phi_J_item[i] = m_histTool->bookPPMHadEtaVsPhi(name, title);
			if (i < FJET_ROI_BITS) {
				name = "JetEmScale_200GeV_Eta_vs_Phi_FJ_J_item_" + thrNum[i];
				title = "Raw Jet #eta v #phi (triggered on " + fjetL1n[i] + " with E_{T} > 200 GeV)";
				m_h_JetEmScale_200GeV_Eta_vs_Phi_FJ_J_item[i] = m_histTool->bookPPMHadEtaVsPhi(name, title);
			}
		}

		m_histTool->setMonGroup(&monJetEmScale200GeVDen);

		m_h_JetEmScale_200GeV_Eta_vs_Phi = m_histTool->bookPPMHadEtaVsPhi("JetEmScale_200GeV_Eta_vs_Phi", "Raw Jet #eta v #phi (E_{T} > 200 GeV)");

		m_histTool->setMonGroup(&monJetEmScale200GeVEff);

		for (int i = 0; i < JET_ROI_BITS; ++i) {
			name = "JetEmScale_200GeV_Eta_vs_Phi_J_Eff_item_" + thrNum[i];
			title = "Raw Jet #eta v #phi (triggered on " + jetL1t[i] + " with E_{T} > 200 GeV) Efficiency (%)";
			m_h_JetEmScale_200GeV_Eta_vs_Phi_J_Eff_item[i] = m_histTool->bookPPMHadEtaVsPhi(name, title);
			if (i < FJET_ROI_BITS) {
				name = "JetEmScale_200GeV_Eta_vs_Phi_FJ_J_Eff_item_" + thrNum[i];
				title = "Raw Jet #eta v #phi (triggered on " + fjetL1n[i] + " with E_{T} > 200 GeV) Efficiency (%)";
				m_h_JetEmScale_200GeV_Eta_vs_Phi_FJ_J_Eff_item[i] = m_histTool->bookPPMHadEtaVsPhi(name, title);
			}
		}

		m_histTool->unsetMonGroup();

		// HSB - counters
		m_numEvents = 0;
		m_numOffJets = 0;
		m_numOffJetsInContainer = 0;
		m_numOffJetsPassCuts = 0;
		m_numOffJetsTriggered = 0;
		m_numJetObjPassTrigger = 0;
		m_numJetObjTotal = 0;
	}

	msg(MSG::DEBUG) << "Leaving bookHistograms" << endmsg;

	return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode JetEfficienciesMonTool::fillHistograms()
/*---------------------------------------------------------*/
{
	const bool debug = msgLvl(MSG::DEBUG);
	if (debug) msg(MSG::DEBUG) << "fillHistograms entered" << endmsg;

	// Skip events believed to be corrupt

	if (m_errorTool->corrupt()) {
		if (debug) msg(MSG::DEBUG) << "Skipping corrupt event" << endmsg;
		return StatusCode::SUCCESS;
	}

	StatusCode sc;

	// On first event plot disabled channels and bad calo
	if (m_firstEvent) {
		m_firstEvent = false;
		sc = this->triggerTowerAnalysis();
		if (sc.isFailure()) {
			msg(MSG::WARNING) << "Problem analysing Trigger Towers" << endmsg;
			return sc;
		}
	}

	// Here we can use the trigger menu to decide if we want an event.
	bool useEvent = false;
	if (m_useTrigger) {
		typedef std::vector<std::string>::iterator Itr_s;
		for (Itr_s i = m_triggerStrings.begin(); i != m_triggerStrings.end(); ++i) {
			if (m_trigDecTool->isPassed(*i)) {
				useEvent = true;
                                Trig::TrigDecisionTool* trigdec = dynamic_cast<Trig::TrigDecisionTool*> (&*m_trigDecTool);
				if (debug) msg(MSG::DEBUG) << "First requested trigger that fired is : " << (*i) << " with prescale " << trigdec->getPrescale(*i);
				break;
			}
		}

		sc = this->triggerChainAnalysis();
		if (sc.isFailure()) {
			if (debug) msg(MSG::DEBUG) << "Problem checking Trigger Chains" << endmsg;
			return sc;
		}
	}

	else {
		useEvent = true;
	}

	if ( useEvent ) {
		m_eventInfo = 0;
		sc = evtStore()->retrieve(m_eventInfo);
		if (sc.isFailure()) {
			msg(MSG::WARNING) << "Failed to load EventInfo" << endmsg;
			return sc;
		}
		if ( m_removeNoiseBursts ) {
			EventInfo::EventFlagErrorState efState = m_eventInfo->errorState(EventInfo::LAr);
			if (efState == EventInfo::Warning || efState == EventInfo::Error) {
				useEvent = false;
				for (unsigned char bit = 0; bit < 6; ++bit) {
					if (m_eventInfo->isEventFlagBitSet(EventInfo::LAr, bit)) m_h_LAr_jetNoisy->Fill(bit);
				}
			}
		}
	}

	if (useEvent) {
		++m_numEvents;

		sc = this->loadContainers();
		if (sc.isFailure()) {
			msg(MSG::WARNING) << "Problem loading Athena Containers" << endmsg;
			return sc;
		}

		if (debug) msg(MSG::DEBUG) << "Run number " << m_eventInfo->event_ID()->run_number() << " : Lumi Block " << m_eventInfo->event_ID()->lumi_block() << " : Event " << m_eventInfo->event_ID()->event_number() << endmsg;

		// Look at vertex requirements
		unsigned int nPriVtx = this->nPrimaryVertex();
		if (nPriVtx < 1) {
			if (debug) msg(MSG::DEBUG) << "Event " << m_eventInfo->event_ID()->event_number() << " fails vertex requirements " << endmsg;
			return StatusCode::SUCCESS;
		}

		m_h_nPriVtx->Fill(nPriVtx);

		m_numOffJetsInContainer = 0;
		m_numOffJetsPassCuts = 0;
		m_numOffJetsTriggered = 0;
		sc = this->analyseOfflineJets();
		if (sc.isFailure()) {
			msg(MSG::WARNING) << "analyseJets Failed " << endmsg;
			return sc;
		}
		if (debug) msg(MSG::DEBUG) << "Number of Offline Jets = " << m_numOffJetsInContainer << " Passing Cuts = " << m_numOffJetsPassCuts << " Triggered = " << m_numOffJetsTriggered << endmsg;

	}

	if (debug)
		msg(MSG::DEBUG) << "Leaving fillHistograms" << endmsg;

	return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/
StatusCode JetEfficienciesMonTool::procHistograms()
/*---------------------------------------------------------*/
{
	msg(MSG::DEBUG) << "procHistograms entered" << endmsg;

	//if (endOfLumiBlock) {
	//}

	if (endOfRunFlag()) {
		msg(MSG::DEBUG) << "Number of offline jets = " << m_numOffJets << endmsg;
		msg(MSG::DEBUG) << "Number of events = " << m_numEvents << endmsg;

		m_histTool->efficienciesForMerge(m_h_JetEmScale_Et,
		                                 m_h_JetEmScale_Et_triggered,
		                                 m_h_JetEmScale_Et_triggered_Eff);
		m_histTool->efficienciesForMerge(m_h_JetEmScale_Et_forward,
		                                 m_h_JetEmScale_Et_triggered_forward,
		                                 m_h_JetEmScale_Et_triggered_forward_Eff);
		m_histTool->efficienciesForMerge(m_h_JetEmScale_Et_central,
		                                 m_h_JetEmScale_Et_triggered_central,
		                                 m_h_JetEmScale_Et_triggered_central_Eff);
		m_histTool->efficienciesForMerge(m_h_JetEmScale_Eta_vs_Phi,
		                                 m_h_JetEmScale_Eta_vs_Phi_triggered,
		                                 m_h_JetEmScale_Eta_vs_Phi_triggered_Eff);

		for (int i = 0; i < JET_ROI_BITS; ++i) {
			m_histTool->efficienciesForMerge(m_h_JetEmScale_Et,
			                                 m_h_JetEmScale_Et_J_item[i],
			                                 m_h_JetEmScale_Et_J_Eff_item[i]);
			m_histTool->efficienciesForMerge(m_h_JetEmScale_50GeV_Eta_vs_Phi,
			                                 m_h_JetEmScale_50GeV_Eta_vs_Phi_J_item[i],
			                                 m_h_JetEmScale_50GeV_Eta_vs_Phi_J_Eff_item[i]);
			m_histTool->efficienciesForMerge(m_h_JetEmScale_100GeV_Eta_vs_Phi,
			                                 m_h_JetEmScale_100GeV_Eta_vs_Phi_J_item[i],
			                                 m_h_JetEmScale_100GeV_Eta_vs_Phi_J_Eff_item[i]);
			m_histTool->efficienciesForMerge(m_h_JetEmScale_200GeV_Eta_vs_Phi,
			                                 m_h_JetEmScale_200GeV_Eta_vs_Phi_J_item[i],
			                                 m_h_JetEmScale_200GeV_Eta_vs_Phi_J_Eff_item[i]);
			if (i >= FJET_ROI_BITS) continue;
			// Copy the Jet Trigger information into the Forward Jet histograms (as it should be the logical OR of these two sets)
			m_h_JetEmScale_Et_FJ_J_item[i]->getROOTHistBase()->Add(m_h_JetEmScale_Et_J_item[m_linkedHistos[i]]->getROOTHistBase());
			m_h_JetEmScale_50GeV_Eta_vs_Phi_FJ_J_item[i]->getROOTHistBase()->Add(m_h_JetEmScale_50GeV_Eta_vs_Phi_J_item[m_linkedHistos[i]]->getROOTHistBase());
			m_h_JetEmScale_100GeV_Eta_vs_Phi_FJ_J_item[i]->getROOTHistBase()->Add(m_h_JetEmScale_100GeV_Eta_vs_Phi_J_item[m_linkedHistos[i]]->getROOTHistBase());
			m_h_JetEmScale_200GeV_Eta_vs_Phi_FJ_J_item[i]->getROOTHistBase()->Add(m_h_JetEmScale_200GeV_Eta_vs_Phi_J_item[m_linkedHistos[i]]->getROOTHistBase());
			m_histTool->efficienciesForMerge(m_h_JetEmScale_Et,
			                                 m_h_JetEmScale_Et_FJ_J_item[i],
			                                 m_h_JetEmScale_Et_FJ_J_Eff_item[i]);
			m_histTool->efficienciesForMerge(m_h_JetEmScale_50GeV_Eta_vs_Phi,
			                                 m_h_JetEmScale_50GeV_Eta_vs_Phi_FJ_J_item[i],
			                                 m_h_JetEmScale_50GeV_Eta_vs_Phi_FJ_J_Eff_item[i]);
			m_histTool->efficienciesForMerge(m_h_JetEmScale_100GeV_Eta_vs_Phi,
			                                 m_h_JetEmScale_100GeV_Eta_vs_Phi_FJ_J_item[i],
			                                 m_h_JetEmScale_100GeV_Eta_vs_Phi_FJ_J_Eff_item[i]);
			m_histTool->efficienciesForMerge(m_h_JetEmScale_200GeV_Eta_vs_Phi,
			                                 m_h_JetEmScale_200GeV_Eta_vs_Phi_FJ_J_item[i],
			                                 m_h_JetEmScale_200GeV_Eta_vs_Phi_FJ_J_Eff_item[i]);
		}

	}

	return StatusCode::SUCCESS;
}

//------------------------------------------------------------------
// The check to see if an object is triggered w.r.t. a RoI
// It can be done in two ways so allow it to handle either one
//------------------------------------------------------------------
bool JetEfficienciesMonTool::deltaMatch(double etaJet, double etaRoi, double dR, double dPhi, bool isForward) {

	// If within Tile & HEC then use delta R matching
	// if within FCAL then use delta Phi matching

	double fabsEtaJet = fabs(etaJet);

	bool etaJetPos = (etaJet >= 0.0);
	bool etaRoiPos = (etaRoi >= 0.0);

	bool etaRoiSameSideMatch = (etaJetPos == etaRoiPos);

	if (fabsEtaJet < 2.899) { //within Tile&Hec
		if (dR < m_goodHadDeltaRMatch_Cut) {
			return true;
		} else {
			return false;
		}
	} else if (fabsEtaJet >= 2.899 && fabsEtaJet < 3.2) { //end of HEC so use combined matching between central & forward regions
		if ((dR < m_goodHadDeltaRMatch_Cut && !isForward) || (dPhi < m_goodHadDeltaPhiMatch_Cut && isForward && etaRoiSameSideMatch )) {
			return true;
		} else {
			return false;
		}
	} else { //within FCAL (3.2 to 4.9)
		if (dPhi < m_goodHadDeltaPhiMatch_Cut && etaRoiSameSideMatch ) {
			return true;
		} else {
			return false;
		}
	}
}

//------------------------------------------------------------------
//Correct the value of deltaPhi so that it falls in +-PI range
//------------------------------------------------------------------
double JetEfficienciesMonTool::correctDeltaPhi(double dPhi) {

	if (fabs(dPhi) > M_PI) {
		dPhi = (dPhi > 0) ? dPhi - 2 * M_PI : dPhi + 2 * M_PI;
	}

	return dPhi;
}

//------------------------------------------------------------------
//Calculate delta R quickly between two objects
//------------------------------------------------------------------
double JetEfficienciesMonTool::calcDeltaR(double eta1, double phi1, double eta2, double phi2) {

	double dEta = eta1 - eta2;
	double dPhi = correctDeltaPhi(phi1 - phi2);

	double dR = sqrt((dEta * dEta) + (dPhi * dPhi));
	return dR;
}

//------------------------------------------------------------------
//Check for electron that it is of the right jet quality type as required from jobOptions
//------------------------------------------------------------------
bool JetEfficienciesMonTool::correctJetQuality(const Jet* jet) {

	bool correctType = false;

	// TODO: Move this tools to class members? But they are used
	// only in this method.
	//JetCleaningTool looseBadJet( JetCleaningTool::LooseBad );
	//JetCleaningTool mediumBadJet( JetCleaningTool::MediumBad );
	//JetCleaningTool tightBadJet( JetCleaningTool::TightBad );

	switch (m_jetQualityLevel) {
	case 0: //"None"
		correctType = true; break;
	case 10: //"Jet Loose"
		correctType = !m_looseJetSelector->keep(*jet);
		break;
	case 20: //"Jet Medium"
		correctType = !m_mediumJetSelector->keep(*jet);
		break;
	case 30: //"Jet Tight"
		correctType = !m_tightJetSelector->keep(*jet);
		break;
	default:
		correctType = false;
		break;
	}
	return correctType;
}

/**********************************************/
//Analysis code for offline reconstructed jets
//Compares jets with Jet RoIs
/**********************************************/
StatusCode JetEfficienciesMonTool::analyseOfflineJets() {

	//Access all of the offline reconstructed jet candidates in the event
	//typedef JetCollection::const_iterator Itr_jets;
	typedef JetContainer::const_iterator Itr_jets;
	m_numOffJetsInContainer = m_offlineJets->size();
	m_numJetObjTotal += m_numOffJetsInContainer;

	// Create variables for jet properties
	double EtOJ = 0.0, etaOJ = 0.0, fabsEtaOJ = 0.0, phiOJ = 0.0, phiOJ_L1C = 0.0;
	// Create variable to determine if selecting the right type of jets based on criteria in jO
	bool correctType = false;

	//Cycle through all of the offline reconstructed jets
	for (Itr_jets jetItr = m_offlineJets->begin(); jetItr != m_offlineJets->end(); ++jetItr) {

		//Keep track of eta, phi and Et as these will be used often
		//EtOJ = (*jetItr)->et() / CLHEP::GeV;
		EtOJ = (*jetItr)->p4().Et() / CLHEP::GeV;
		etaOJ = (*jetItr)->eta();
		fabsEtaOJ = fabs(etaOJ);
		phiOJ = (*jetItr)->phi();
		phiOJ_L1C = l1caloPhi(phiOJ);

		//Alternate triggers - the ones that will bias the result
		bool altTrigger = m_passed_EF_Tau_Trigger || m_passed_EF_MissingEnergy_Trigger || m_passed_EF_SingleJet_Trigger || m_passed_EF_MultiJet_Trigger;

		//Check that the trigger selection is not biased
		bool unbiasedTrigger = false;
		if (m_passed_EF_Trigger) {
			if (m_passed_EF_SingleEgamma_Trigger) {
				unbiasedTrigger = isolatedJetObjectEF(phiOJ, etaOJ);
			} else {
				if (altTrigger) {
					return StatusCode::SUCCESS;
				} else {
					unbiasedTrigger = true;
				}
			}
		} else if (m_passed_L1_EM_Trigger) {
			unbiasedTrigger = isolatedJetObjectL1(phiOJ, etaOJ);
		} else {
			return StatusCode::SUCCESS;
		}

		//If passed the trigger conditions then proceed to start analysis
		if (unbiasedTrigger) {

			correctType = false;
			m_numJetObjPassTrigger++;

			//Check that the good jet (not bad, ugly) matches the type selected (if any was chosen in jobOptions file)
			correctType = correctJetQuality((*jetItr));

			//Have the correct type of jet so do some analysis with it
			if (correctType) {
				//Update counters
				m_numOffJets++;
				m_numOffJetsPassCuts++;

				if (EtOJ > 0.0) {
					m_h_JetEmScale_Et->Fill(EtOJ);
					if (fabsEtaOJ >= 2.899) { //treat 2.899 as new forward range due to matching in this region, not 3.2
						m_h_JetEmScale_Et_forward->Fill(EtOJ);
					} else {
						m_h_JetEmScale_Et_central->Fill(EtOJ);
					}

					m_histTool->fillPPMHadEtaVsPhi(m_h_JetEmScale_Eta_vs_Phi, etaOJ, phiOJ_L1C);

					if (EtOJ > 50) {
						m_histTool->fillPPMHadEtaVsPhi(m_h_JetEmScale_50GeV_Eta_vs_Phi, etaOJ, phiOJ_L1C);
					}
					if (EtOJ > 100) {
						m_histTool->fillPPMHadEtaVsPhi(m_h_JetEmScale_100GeV_Eta_vs_Phi, etaOJ, phiOJ_L1C);
					}
					if (EtOJ > 200) {
						m_histTool->fillPPMHadEtaVsPhi(m_h_JetEmScale_200GeV_Eta_vs_Phi, etaOJ, phiOJ_L1C);
					}
				}

				//Set up useful numbers to keep track of RoI information
				double etaROI = 0.0, phiROI = 0.0, phiROI_L1C = 0.0;
				double dEta = 0.0, dPhi = 0.0, dR = 1000, temp_dR = 0.0;
				double bestEtaROI = 0.0;
				double bestDeltaPhi = 0.0;
				uint32_t ROIWord = 0;
				bool isForward = false, bestIsForward = false;

				//Access the Jet RoIs
				const std::vector<Jet_ROI>& jetROIs(m_lvl1RoIs->getJetROIs());
				typedef std::vector<Jet_ROI>::const_iterator Itr_jetroi;

				//Iterate over all of the Jet RoIs
				Itr_jetroi roiItrE = jetROIs.end();
				for (Itr_jetroi roiItr = jetROIs.begin(); roiItr != roiItrE; ++roiItr) {

					//Get useful values for the Jet RoI
					etaROI = (*roiItr).getEta();
					phiROI = (*roiItr).getPhi();
					phiROI_L1C = l1caloPhi(phiROI);
					isForward = (fabs(etaROI) >= 3.2);

					//Calculate the difference in eta and phi between the jet and RoI
					dEta = etaOJ - etaROI;
					dPhi = correctDeltaPhi(phiOJ_L1C - phiROI_L1C);

					//Calculate delta R
					temp_dR = sqrt(dEta * dEta + dPhi * dPhi);


					//Check if the new delta R is smaller than any previous delta R value.
					//In that case, keep track of the new RoI values
					if (temp_dR < dR) {

						//RoI information
						bestEtaROI = etaROI;
						ROIWord = (*roiItr).getROIWord();

						bestDeltaPhi = dPhi;
						dR = temp_dR;

						bestIsForward = isForward;
					}
				}

				//Check to see if there was an RoI to match with an jet
				if (dR != 1000) {

					//Check if jet and RoI matched to a very good level (less than cut) - default now 0.2
					if (deltaMatch(etaOJ, bestEtaROI, dR, bestDeltaPhi, bestIsForward)) {

						m_numOffJetsTriggered++;

						bool matchToJet[JET_ROI_BITS];
						for (int k = 0; k < JET_ROI_BITS; ++k) { matchToJet[k] = false; }

						if (EtOJ > 0.0) {

							m_h_JetEmScale_Et_triggered->Fill(EtOJ);
							if (fabsEtaOJ >= 2.899) {
								m_h_JetEmScale_Et_triggered_forward->Fill(EtOJ);
							} else {
								m_h_JetEmScale_Et_triggered_central->Fill(EtOJ);
							}

							m_histTool->fillPPMHadEtaVsPhi(m_h_JetEmScale_Eta_vs_Phi_triggered, etaOJ, phiOJ_L1C);

							//Look at each bit in the RoI word (using bitshift) to see which thresholds were
							//passed and which ones were not

							// First look at jets only up to 2.899 in mod eta
							if (fabsEtaOJ < 2.899) {
								for (int k = 0; k < JET_ROI_BITS; ++k) {
									matchToJet[k] = false;
									if ((ROIWord >> k) & 1) {

										m_h_JetEmScale_Et_J_item[k]->Fill(EtOJ);
										matchToJet[k] = true;

										if (EtOJ > 50) {
											m_histTool->fillPPMHadEtaVsPhi(m_h_JetEmScale_50GeV_Eta_vs_Phi_J_item[k], etaOJ, phiOJ_L1C);
										}
										if (EtOJ > 100) {
											m_histTool->fillPPMHadEtaVsPhi(m_h_JetEmScale_100GeV_Eta_vs_Phi_J_item[k], etaOJ, phiOJ_L1C);
										}
										if (EtOJ > 200) {
											m_histTool->fillPPMHadEtaVsPhi(m_h_JetEmScale_200GeV_Eta_vs_Phi_J_item[k], etaOJ, phiOJ_L1C);
										}

									}
								}
							} else {

								if (bestIsForward) { //implies in very forward region (greater than 3.2)

									for (int k = 0; k < FJET_ROI_BITS; ++k) {
										if ((ROIWord >> (k + 8)) & 1) {
											if (!matchToJet[m_linkedHistos[k]]) {
												m_h_JetEmScale_Et_FJ_J_item[k]->Fill(EtOJ);

												if (EtOJ > 50) {
													m_histTool->fillPPMHadEtaVsPhi(m_h_JetEmScale_50GeV_Eta_vs_Phi_FJ_J_item[k], etaOJ, phiOJ_L1C);
												}
												if (EtOJ > 100) {
													m_histTool->fillPPMHadEtaVsPhi(m_h_JetEmScale_100GeV_Eta_vs_Phi_FJ_J_item[k], etaOJ, phiOJ_L1C);
												}
												if (EtOJ > 200) {
													m_histTool->fillPPMHadEtaVsPhi(m_h_JetEmScale_200GeV_Eta_vs_Phi_FJ_J_item[k], etaOJ, phiOJ_L1C);
												}

											}
										}
									}

								} else { //between 2.899 and 3.2

									for (int k = 0; k < JET_ROI_BITS; ++k) {
										if ((ROIWord >> k) & 1) {
											if (!matchToJet[k]) {

												int fj_j_link = -1;
												for (Int_t l = 0; l < FJET_ROI_BITS; ++l) {
													if (m_linkedHistos[l] == k) { fj_j_link = l;	}
												}

												if (fj_j_link != -1) {
													m_h_JetEmScale_Et_FJ_J_item[fj_j_link]->Fill(EtOJ);

													if (EtOJ > 50) {
														m_histTool->fillPPMHadEtaVsPhi(m_h_JetEmScale_50GeV_Eta_vs_Phi_FJ_J_item[fj_j_link], etaOJ, phiOJ_L1C);
													}
													if (EtOJ > 100) {
														m_histTool->fillPPMHadEtaVsPhi(m_h_JetEmScale_100GeV_Eta_vs_Phi_FJ_J_item[fj_j_link], etaOJ, phiOJ_L1C);
													}
													if (EtOJ > 200) {
														m_histTool->fillPPMHadEtaVsPhi(m_h_JetEmScale_200GeV_Eta_vs_Phi_FJ_J_item[fj_j_link], etaOJ, phiOJ_L1C);
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
			}
		}
	}

	return StatusCode::SUCCESS;
}


//------------------------------------------------------------------
//Ask if object is has no EmTau RoIs nearby at L1
//Do this by assuming that the highest ET EmTau RoI is the one causing
//the event to trigger at HLT level
//------------------------------------------------------------------
bool JetEfficienciesMonTool::isolatedJetObjectL1(double phi, double eta) {

	bool isolated = false, tagFound = false;
	double dREm = 10.0, dR_Max = 10.0;
	double ET_Max = -10.0;
	double etaROI = 0.0, phiROI = 0.0, ET_ROI = 0.0, hadCore_ROI = 0.0;

	const std::vector<EmTau_ROI>& emTauROIs(m_lvl1RoIs->getEmTauROIs());
	typedef std::vector<EmTau_ROI>::const_iterator Itr_emTauRoi;

	//Cycle over the rois, get their properties and determine the distance from the object
	Itr_emTauRoi roiItrE = emTauROIs.end();
	for (Itr_emTauRoi roiItr = emTauROIs.begin(); roiItr != roiItrE; ++roiItr) {
		bool emThresholdPassed = false;
		if (m_useEmThresholdsOnly) {
			const std::vector<std::string>& thrPassed((*roiItr).getThresholdNames());
			typedef std::vector<std::string>::const_iterator Itr_s;
			Itr_s iE = thrPassed.end();
			for (Itr_s i = thrPassed.begin(); i != iE; ++i) {
				if ((*i).find("EM") != std::string::npos) {
					emThresholdPassed = true;
				}
			}
		}

		if (emThresholdPassed) {
			etaROI = (*roiItr).getEta();
			phiROI = (*roiItr).getPhi();
			ET_ROI = (*roiItr).getEMClus();
			hadCore_ROI = (*roiItr).getHadCore();
			dREm = calcDeltaR(eta, phi, etaROI, phiROI);

			//If this energy exceeds the current record then store the details of the 'tag' EM RoI
			if (ET_ROI > ET_Max && (!m_passed_EF_SingleEgamma_Trigger_HighestVH || (m_passed_EF_SingleEgamma_Trigger_HighestVH && hadCore_ROI <= m_hadCoreVHCut))) {
				ET_Max = ET_ROI;
				dR_Max = dREm;
				tagFound = true;
			}
		}
	}

	// Check that the object is far away enough from highest ET EM RoI
	if (dR_Max > m_goodEMDeltaRMatch_Cut && tagFound) {
		isolated = true;
	} else {
		isolated = false;
	}

	return isolated;
}


//------------------------------------------------------------------
//Ask if object is has no jets or jet RoIs nearby at EF
//------------------------------------------------------------------
bool JetEfficienciesMonTool::isolatedJetObjectEF(double phi, double eta) {

	//At present just call L1 function, change if there are technicalities
	return isolatedJetObjectL1(phi, eta);
}

//---------------------------------------------------------------
// Trigger Tower Analysis
//---------------------------------------------------------------
StatusCode JetEfficienciesMonTool::triggerTowerAnalysis() {

	m_dbPpmDeadChannels = 0;
	StatusCode sc = detStore()->retrieve(m_dbPpmDeadChannels, m_dbPpmDeadChannelsFolder);
	if (sc.isFailure()) {
		msg(MSG::WARNING) << "Failed to load DB PPM Dead Channels" << endmsg;
		return sc;
	}

	m_triggerTowers = 0;
	sc = evtStore()->retrieve(m_triggerTowers, m_triggerTowersLocation);
	if (sc.isFailure()) {
		msg(MSG::WARNING) << "Failed to load Trigger Towers" << endmsg;
		return sc;
	}

	sc = this->mapTileQuality();
	if (sc.isFailure()) {
		return sc;
	}

	typedef TriggerTowerCollection::const_iterator Itr_tt;
	for (Itr_tt ttItr = m_triggerTowers->begin(); ttItr != m_triggerTowers->end(); ++ttItr) {

		// Get the values of eta and phi for the trigger towers
		double ttEta = (*ttItr)->eta();
		double ttPhi = (*ttItr)->phi();
		const L1CaloCoolChannelId emCoolId(m_ttTool->channelID(ttEta, ttPhi, 0));
		const L1CaloCoolChannelId hadCoolId(m_ttTool->channelID(ttEta, ttPhi, 1));
		const Identifier emIdent(m_ttTool->identifier(ttEta, ttPhi, 0));
		const Identifier hadIdent(m_ttTool->identifier(ttEta, ttPhi, 1));

		// The Disabled Towers DB folder is used for 2011 data (& 2012 hopefully)
		unsigned int emDisabled(0), hadDisabled(0);
		// The dead channels folder (only has entries for dead channels - no entry = good channel)
		CondAttrListCollection::const_iterator itr = m_dbPpmDeadChannels->chanAttrListPair(emCoolId.id());
		if (itr != m_dbPpmDeadChannels->end()) {
                        const coral::AttributeList& attrList(itr->second);
			emDisabled = attrList["ErrorCode"].data<unsigned int>();
		}
		itr = m_dbPpmDeadChannels->chanAttrListPair(hadCoolId.id());
		if (itr != m_dbPpmDeadChannels->end()) {
                        const coral::AttributeList& attrList(itr->second);
			hadDisabled = attrList["ErrorCode"].data<unsigned int>();
		}

		int emBadCalo(0), hadBadCalo(0);
		emBadCalo = m_larEnergy->hasMissingFEB(emIdent);
		if (fabs(ttEta) > 1.5) {
			hadBadCalo = m_larEnergy->hasMissingFEB(hadIdent);
		} else {
			IdTileQualityMapType::const_iterator qiter(m_idTileQualityMap.find(hadIdent));
			if (qiter != m_idTileQualityMap.end()) {
				hadBadCalo = qiter->second;
			}
		}

		if (emBadCalo || hadBadCalo) m_histTool->fillPPMHadEtaVsPhi(m_h_TrigTower_jetBadCalo, ttEta, ttPhi, (double)(emBadCalo + hadBadCalo));
		if (emDisabled || hadDisabled) m_histTool->fillPPMHadEtaVsPhi(m_h_TrigTower_jetDeadChannel, ttEta, ttPhi, (double)(emDisabled + hadDisabled));
	}
	m_idTileQualityMap.clear();

	return StatusCode::SUCCESS;
}

//---------------------------------------------------------------
// Trigger Chain Analysis - find out which trigger chains have
// passed so that we do not bias our efficiencies
//---------------------------------------------------------------
StatusCode JetEfficienciesMonTool::triggerChainAnalysis() {

	std::string vhCheck;
	int maxTV = 0;

	// Get the list of all triggers but do this only once in the event loop
	if (m_configuredChains.size() == 0) {
                Trig::TrigDecisionTool* trigdec = dynamic_cast<Trig::TrigDecisionTool*> (&*m_trigDecTool);
		m_configuredChains = trigdec->getListOfTriggers();
		m_wantedTriggers.resize(m_configuredChains.size());
		int i = 0;
		std::vector<std::string>::const_iterator itE = m_configuredChains.end();
		for (std::vector<std::string>::const_iterator it =
		            m_configuredChains.begin(); it != itE; ++it, ++i) {

			// L1 em trigger items
			if ((*it).find("L1_EM") != std::string::npos && (*it).find("XS") == std::string::npos && (*it).find("XE") == std::string::npos) {
				m_wantedTriggers[i] |= s_L1_EM_Trigger_mask;
			}

			// EF trigger chains
			if ((*it).find("EF") != std::string::npos) {
				m_wantedTriggers[i] |= s_EF_Trigger_mask;

				//Event Filter chains corresponding to single jet triggers (keeping it simple)
				if (((*it).find("EF_j") != std::string::npos && (*it).find("EF_je") == std::string::npos) ||
				        (*it).find("EF_fj") != std::string::npos ||
				        ((*it).find("EF_L1J") != std::string::npos && (*it).find("EMPTY") == std::string::npos) ||
				        (*it).find("EF_l2j") != std::string::npos ||
				        (*it).find("EF_b")	!= std::string::npos) {
					m_wantedTriggers[i] |= s_EF_SingleJet_Trigger_mask;
				}
				//Event Filter chains corresponding to multiple jet triggers (worry about it later)
				if ((*it).find("EF_2j") != std::string::npos ||
				        (*it).find("EF_4j") != std::string::npos) {
					m_wantedTriggers[i] |= s_EF_MultiJet_Trigger_mask;
				}
				//Event Filter chains corresponding to single electrons or photons
				if (((*it).find("EF_e") != std::string::npos && (*it).find("EF_eb") == std::string::npos && (*it).find("EF_j") == std::string::npos
				        && (*it).find("_EF_xe") == std::string::npos && (*it).find("_EF_xs") == std::string::npos) ||
				        (*it).find("EF_g") != std::string::npos) {
					m_wantedTriggers[i] |= s_EF_SingleEgamma_Trigger_mask;
					vhCheck = (*it).substr(4, 6);
					int threshVal = atoi(vhCheck.c_str());
					if (threshVal > maxTV) {
						maxTV = threshVal;
						if (vhCheck.find("vh") != std::string::npos) {
							m_wantedTriggers[i] |= s_EF_SingleEgamma_Trigger_HighestVH_mask;
						}
					}
				}
				//Event Filter chains corresponding to taus
				if ((*it).find("EF_tau") != std::string::npos ||
				        (*it).find("EF_2tau") != std::string::npos ||
				        (*it).find("EF_4tau") != std::string::npos) {
					m_wantedTriggers[i] |= s_EF_Tau_Trigger_mask;
				}
				//Event Filter chains corresponding to missing energy
				//Missing energy could come from electron so it may bias results
				if ((*it).find("EF_xe") != std::string::npos ||
				        (*it).find("EF_xs") != std::string::npos) {
					m_wantedTriggers[i] |= s_EF_MissingEnergy_Trigger_mask;
				}

				//Any event filter chains which do not pass any of these checks
				//would be treated as unbiased triggers
			}
		}
	}

	//msg(MSG::DEBUG) << "Trigger Analysis: New Event" << endmsg;

	int trigSet = 0;
	int i = 0;
	std::vector<std::string>::const_iterator itE = m_configuredChains.end();
	for (std::vector<std::string>::const_iterator it =
	            m_configuredChains.begin(); it != itE; ++it, ++i) {

		//Check that the trigger was passed
		const int trigMap = m_wantedTriggers[i];
		if (((trigMap ^ trigSet)&trigMap) && m_trigDecTool->isPassed(*it)) {
			trigSet |= trigMap;
		}
	}
	// L1 Triggers
	m_passed_L1_EM_Trigger                     = (trigSet & s_L1_EM_Trigger_mask);
	// EF Triggers
	m_passed_EF_Trigger                        = (trigSet & s_EF_Trigger_mask);
	m_passed_EF_SingleJet_Trigger              = (trigSet & s_EF_SingleJet_Trigger_mask);
	m_passed_EF_MultiJet_Trigger               = (trigSet & s_EF_MultiJet_Trigger_mask);
	m_passed_EF_SingleEgamma_Trigger           = (trigSet & s_EF_SingleEgamma_Trigger_mask);
	m_passed_EF_SingleEgamma_Trigger_HighestVH = (trigSet & s_EF_SingleEgamma_Trigger_HighestVH_mask);
	m_passed_EF_Tau_Trigger                    = (trigSet & s_EF_Tau_Trigger_mask);
	m_passed_EF_MissingEnergy_Trigger          = (trigSet & s_EF_MissingEnergy_Trigger_mask);

	return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------------
// Convert offline analysis phi into value more suitable for L1Calo
//------------------------------------------------------------------------------------
double JetEfficienciesMonTool::l1caloPhi(const double phi) const {
	if (phi >= 0.0 && phi < M_PI ) {
		return phi;
	}
	if (phi < 0.0) {
		return phi + (2.0 * M_PI);
	}
	return phi;
}

//------------------------------------------------------------------------------------
// Return number of primary vertices that have at least a number of tracks (python configurable)
//------------------------------------------------------------------------------------
unsigned int JetEfficienciesMonTool::nPrimaryVertex() {
	unsigned int nPriVtx(0);
	typedef VxContainer::const_iterator Itr;
	for (Itr i = m_primaryVertex->begin(); i != m_primaryVertex->end(); ++i) {

		if ( (*i)->vertexType() == 1 || (*i)->vertexType() == 3 ) {
			if ( (*i)->vxTrackAtVertex()->size() >= m_nTracksAtPrimaryVertex ) {
				nPriVtx++;
			}
		}
	}
	return nPriVtx;
}

//------------------------------------------------------------------------------------
// Load important containers
//------------------------------------------------------------------------------------
StatusCode JetEfficienciesMonTool::loadContainers() {
	StatusCode sc;

	m_primaryVertex = 0;
	sc = evtStore()->retrieve(m_primaryVertex, m_primaryVertexLocation);
	if (sc.isFailure()) {
		msg(MSG::WARNING) << "Failed to load Primary Vertices" << endmsg;
		return sc;
	}

	m_lvl1RoIs = 0;
	sc = evtStore()->retrieve(m_lvl1RoIs, m_lvl1RoIsLocation);
	if (sc.isFailure()) {
		msg(MSG::WARNING) << "Failed to load LVL1 RoIs" << endmsg;
		return sc;
	}

	m_offlineJets = 0;
	sc = evtStore()->retrieve(m_offlineJets, m_offlineJetsLocation);
	if (sc.isFailure()) {
		msg(MSG::WARNING) << "Failed to load Offline Jets" << endmsg;
		return sc;
	}

	return sc;
}

//------------------------------------------------------------------------------------
// Map Tile quality.  Adapted from TileRecAlgs/TileCellToTTL1 to avoid running on every event.
//------------------------------------------------------------------------------------
StatusCode JetEfficienciesMonTool::mapTileQuality() {

	// -------------------------------------------------
	// Load the TileCell container
	// -------------------------------------------------

	const CaloCellContainer* cellcoll = 0;
	StatusCode sc = evtStore()->retrieve(cellcoll, m_caloCellContainerLocation) ;
	if (sc.isFailure()) {
		msg(MSG::WARNING) << "Unable to retrieve Cell container: " << m_caloCellContainerLocation << endmsg;
		return sc;
	}

	// -------------------------------------------------
	// Create intermediate arrays
	// -------------------------------------------------

	int ttNpmt[32][64];       // array of TT occupancy
	Identifier ttId[32][64];  // array of TT identifiers
	uint16_t ttStatusCells[32][64];   // array of TT status of cells
	uint16_t ttStatusChans[32][64];   // array of TT status of channels

	// clear the arrays
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 64; j++) {
			ttNpmt[i][j] = 0;
			ttId[i][j] = 0;
			ttStatusCells[i][j] = 0;
			ttStatusChans[i][j] = 0;
		}
	}

	// -------------------------------------------------
	// Loop over all cells
	// -------------------------------------------------

	CaloCellContainer::const_iterator f_cell = cellcoll->begin();
	CaloCellContainer::const_iterator l_cell = cellcoll->end();
	for ( ; f_cell != l_cell; ++f_cell) {
		const CaloCell* cell = (*f_cell);

		// keep only cells from TileCal calorimeter barrel or extended barrel
		Identifier cell_id = cell->ID();
		if (!(m_tileID->is_tile(cell_id)) ) continue;

		const TileCell* tilecell = dynamic_cast<const TileCell*> (cell);
		if (!tilecell) continue;

		int bad_cell = tilecell->badcell();
		int bad_chan[2];
		bad_chan[0] = tilecell->badch1(); bad_chan[1] = tilecell->badch2();

		// In order to make sure that the D-cells are correctly added
		// across two towers. Loop over the two PMTs in each cell

		for (int ipmt = 0; ipmt < 2;  ipmt++) {
			Identifier pmt_id = m_tileID->pmt_id(cell_id, ipmt);
			Identifier tt_id = m_tileCablingService->pmt2tt_id(pmt_id);

			// remove the E-cells
			int sample = m_tileID->sample(pmt_id);
			if (sample == TileID::SAMP_E) continue;

			// if in the negative eta region add 16 to the ieta offset arrays
			int eta_offset = 0;
			if (m_tileID->is_negative(pmt_id)) eta_offset = 16;

			// the D0 cell is not being split correctly across cells
			int ieta = m_TT_ID->eta(tt_id);
			if (sample == TileID::SAMP_D && ieta == 0 && ipmt == 1) eta_offset = 16;

			ieta += eta_offset;
			int iphi = m_TT_ID->phi(tt_id);

			if ( ttNpmt[ieta][iphi] > 0) {
				ttNpmt[ieta][iphi]++;
				ttStatusCells[ieta][iphi] += (uint16_t) bad_cell;
				ttStatusChans[ieta][iphi] += (uint16_t) bad_chan[ipmt] ;
			} else {
				ttId[ieta][iphi] = tt_id;
				ttNpmt[ieta][iphi]++;
				ttStatusCells[ieta][iphi] = (uint16_t) bad_cell;
				ttStatusChans[ieta][iphi] = (uint16_t) bad_chan[ipmt];
			}

		} // end of loop over pmts in the cell
	} // end loop over cells

	for (int ieta = 0; ieta < 32; ieta++) {
		for (int iphi = 0; iphi < 64; iphi++) {

			// don't load towers that are empty
			if (ttNpmt[ieta][iphi] == 0) continue;

			uint16_t qual = 0;
			if (ttStatusChans[ieta][iphi] == ttNpmt[ieta][iphi]) qual += TileTTL1Cell::MASK_BADTOWER;
			if (ttStatusCells[ieta][iphi] > 0) qual += TileTTL1Cell::MASK_BADCELL;
			if (ttStatusChans[ieta][iphi] > 0) qual += TileTTL1Cell::MASK_BADCHAN;

			if (qual != 0) m_idTileQualityMap[ttId[ieta][iphi]] = qual;

		}
	}

	return StatusCode::SUCCESS;
}

// ============================================================================
}  // end namespace
// ============================================================================
