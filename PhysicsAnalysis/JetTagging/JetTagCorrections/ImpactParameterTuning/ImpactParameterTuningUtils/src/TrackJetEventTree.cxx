/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * TrackJetEventTree.cxx 
 *
 * Stephan.Sandvoss@cern.ch
 */

/**
 * intra-package includes
 */
#include "ImpactParameterTuningUtils/TrackJetEventTree.h"

/**
 * extra-package includes
 */

/**
 * system includes
 */
#include "TFile.h"
#include "TTree.h"
#include "math.h"

/**
 * Constructor
 */
TrackJetEventTree::TrackJetEventTree(TFile *file) :
		m_treeName("Tracks"),
		m_treeNameLong("Track Parameters With Event+Jet Information")
		, m_separationChar("_"),
		m_truth("Truth"), m_label("Label"), m_reco("Reco"),
		m_prodX("ProdX"), m_prodY("ProdY"), m_prodZ("ProdZ"), 
		m_event("event"), m_jet("jet"), m_track("track"),
		m_isData("isData"), m_eventWeight("eventWeight"),
		m_runNumber("runNumber"),
		m_eventNumber("eventNumber"), m_missingETx("missingETx"),
		m_missingETy("missingETy"), m_missingETphi("missingETphi"),
		m_NElectrons("NElectrons"), m_NMuons("NMuons"),
		m_NPhotons("NPhotons"), m_NTaus("NTaus"),
		m_isElectron("isElectron"), m_isMuon("isMuon"),
		m_isPhoton("isPhoton"), m_isTau("isTau"),
		m_jetAlgorithm("JetAlgorithm"),
		m_NJets("NJets"), m_NTracks("NTracks"),
		m_e("e"), m_px("px"), m_py("py"), m_pz("pz"),
		m_eta("eta"), m_phi("phi"), m_sinTheta("sinTheta"),
		m_charge("charge"),
		m_particleOriginType("particleOriginType"),
		m_PVx("PVx"), m_PVy("PVy"), m_PVz("PVz"),
		m_isFromV0("isFromV0"),
		m_perigeex("perigeex"), m_perigeey("perigeey"),
		m_perigeez("perigeez"), m_ipTrans("IpTrans"),
		m_ipLong("IpLong"), m_ipTransError("ipTransError"),
		m_ipLongError("ipLongError"), m_isHit("isHit"), m_zero("0"),
		m_one("1"), m_two("2"), m_three("3"), m_four("4"),
		m_five("5"), m_six("6"), m_seven("7"), m_eight("8"),
		m_pixel("pixel"), m_sct("sct"), m_trt("trt"),
		m_barrel("barrel"), m_endcap("endcap"),
		m_numberOf("NumberOf"), m_BLayerHits("BLayerHits"),
		m_PixelHoles("PixelHoles"), m_PixelHits("PixelHits"),
		m_GangedPixels("GangedPixels"), m_SCTHits("SCTHits"),
		m_SCTHoles("SCTHoles"), m_TRTHits("TRTHits"),
		m_TRTHighThresholdHits("TRTHighThresholdHits"),
		m_TRTOutliers("TRTOutliers"),
		m_TRTHighThresholdOutliers("TRTHighThresholdOutliers"),
		m_MdtHits("MdtHits"), m_TgcPhiHits("TgcPhiHits"),
		m_TgcEtaHits("TgcEtaHits"), m_CscPhiHits("CscPhiHits"),
		m_CscEtaHits("CscEtaHits"), m_RpcPhiHits("RpcPhiHits"),
		m_RpcEtaHits("RpcEtaHits"), m_CscEtaHoles("CscEtaHoles"),
		m_CscPhiHoles("CscPhiHoles"), m_RpcEtaHoles("RpcEtaHoles"),
		m_RpcPhiHoles("RpcPhiHoles"), m_MdtHoles("MdtHoles"),
		m_TgcEtaHoles("TgcEtaHoles"), m_TgcPhiHoles("TgcPhiHoles"),
		m_OutliersOnTrack("OutliersOnTrack"),
		m_BLayerSharedHits("BLayerSharedHits"),
		m_PixelSharedHits("PixelSharedHits"),
		m_SCTSharedHits("SCTSharedHits"),
		m_SCTDoubleHoles("SCTDoubleHoles"),
		m_ContribPixelLayers("ContribPixelLayers"),
		m_standardDeviationOfChi2OS("standardDeviationOfChi2OS"),
		m_BLayerOutliers("BLayerOutliers")
{
	// if root without athena/histservice is used
	if(file) {
		m_tree = (TTree*)file->Get(m_treeName.c_str() );
		m_loadTreeBranches();
		m_created = false;
	} else {
		m_tree = 0;
	}
	m_initPrivateVars();
}

/**
 * Destructor
 */
TrackJetEventTree::~TrackJetEventTree()
{
	// bug: ITHistSvc saves the tree AFTER destruction of 
	// the algorithm  -> do not delete m_tree here ! otherwise 
	// nothing is saved to athena root output file and 
	// athena crashes at THistSvc::finalize
	//if(m_tree && m_created) {
	//	delete m_tree;
	//	m_tree = 0;
	//}
}

/**
 * provide the pointer to the tree, because ITHistService/Root must have it
 */
TTree* TrackJetEventTree::getTree()
{
	return m_tree;
}

/**
 * set the pointer to the TTree, when root file was loaded
 */
void TrackJetEventTree::setTree(TTree *tree)
{
	if(m_tree) {
		delete m_tree;
		m_tree = 0;
		m_tree = tree;
	} else {
		m_tree = tree;
	}
	m_created = false;
	return;
}

/**
 * create a new TTree and its branches
 */
void TrackJetEventTree::create()
{
	if(m_tree && m_created) {
		delete m_tree;
		m_tree = 0;
	}
	m_tree = new TTree(m_treeName.c_str(), m_treeNameLong.c_str() );
	m_created = true;
	m_createTreeBranches();
	return;
}

/**
 * load the branches, when TTree was loaded
 */
void TrackJetEventTree::load(TTree *tree)
{
	if(m_tree && m_created) {
		delete m_tree;
		m_tree = 0;
		m_tree = tree;
	} else {
		m_tree = tree;
	}
	m_created = false;
	m_loadTreeBranches();
	return;
}

/**
 * provide the numbers of entries of the TTree
 */
Int_t TrackJetEventTree::getEntries()
{
	// Fast does not work properly with TChains
	//return m_tree->GetEntriesFast();
	return m_tree->GetEntries();
}

/**
 * get the provided entry of the TTree
 */
void TrackJetEventTree::getEntry(Int_t i)
{
	m_tree->GetEntry(i);
	return;
}

/**
 * get the TTree name, needed for ITHistService/Root
 */
std::string TrackJetEventTree::getTreeName()
{
	return m_treeName;
}

/**
 * get the description name of the TTree
 */
std::string TrackJetEventTree::getTreeLongName()
{
	return m_treeNameLong;
}

/**
 * get the file name for ITHistService
 */
std::string TrackJetEventTree::getHistServiceFile()
{
	return m_histServiceFile;
}

/**
 * get the directory name for ITHistService
 */
std::string TrackJetEventTree::getHistServiceDirectory()
{
	return m_histServiceDirectory;
}

/**
 * get file and directory name for ITHistService
 */
std::string TrackJetEventTree::getHistServiceFullString()
{
	return (std::string)
			("/"+m_histServiceFile+"/"+m_histServiceDirectory);
}

/**
 * set the file name for ITHistService
 */
void TrackJetEventTree::setHistServiceFile(std::string s)
{
	m_histServiceFile = s;
	return;
}

/**
 * set the directory name for ITHistService
 */
void TrackJetEventTree::setHistServiceDirectory(std::string s)
{
	m_histServiceDirectory = s;
	return;
}

/**
 * give the event weight into this class
 */
void TrackJetEventTree::setEventWeight(double d)
{
	m_EventWeight = d;
	return;
}

/**
 * give the run number into this class
 */
void TrackJetEventTree::setEventRunNumber(Int_t i)
{
	m_EventRunNumber = i;
	return;
}

/**
 * give the event number into this class
 */
void TrackJetEventTree::setEventEventNumber(Int_t i)
{
	m_EventEventNumber = i;
	return;
}

/**
 * give the data flag into this class
 */
void TrackJetEventTree::setEventIsData(Char_t b)
{
	m_EventIsData = b;
	return;
}

/**
 * give the missing ETx into this class
 */
void TrackJetEventTree::setEventETx(double d)
{
	m_EventETx = d;
	return;
}

/**
 * give the missing ETy into this class
 */
void TrackJetEventTree::setEventETy(double d)
{
	m_EventETy = d;
	return;
}

/**
 * give the missing ETphi into this class
 */
void TrackJetEventTree::setEventETphi(double d)
{
	m_EventETphi = d;
	return;
}

void TrackJetEventTree::setEventNElectrons(Char_t i)
{
	m_EventNElectrons = i;
	return;
}

void TrackJetEventTree::setEventNMuons(Char_t i)
{
	m_EventNMuons = i;
	return;
}

void TrackJetEventTree::setEventNPhotons(Char_t i)
{
	m_EventNPhotons = i;
	return;
}

void TrackJetEventTree::setEventNTaus(Char_t i)
{
	m_EventNTaus = i;
	return;
}

void TrackJetEventTree::setEventJetAlgorithm(std::string s)
{
	// bug: Root TTree does not work with std::string
	//m_EventJetAlgorithm = s;
	strcpy(m_EventJetAlgorithm, s.c_str() );
	return;
}

void TrackJetEventTree::setEventNJets(Char_t i)
{
	m_EventNJets = i;
	return;
}


void TrackJetEventTree::setJetE(double d)
{
	m_JetE = d;
	return;
}

void TrackJetEventTree::setJetPx(double d)
{
	m_JetPx = d;
	return;
}

void TrackJetEventTree::setJetPy(double d)
{
	m_JetPy = d;
	return;
}

void TrackJetEventTree::setJetPz(double d)
{
	m_JetPz = d;
	return;
}

void TrackJetEventTree::setJetEta(double d)
{
	m_JetEta = d;
	return;
}

void TrackJetEventTree::setJetPhi(double d)
{
	m_JetPhi = d;
	return;
}

void TrackJetEventTree::setJetSinTheta(double d)
{
	m_JetSinTheta = d;
	return;
}

void TrackJetEventTree::setJetIsElectron(Char_t b)
{
	m_JetIsElectron = b;
	return;
}

void TrackJetEventTree::setJetIsMuon(Char_t b)
{
	m_JetIsMuon = b;
	return;
}

void TrackJetEventTree::setJetIsPhoton(Char_t b)
{
	m_JetIsPhoton = b;
	return;
}

void TrackJetEventTree::setJetIsTau(Char_t b)
{
	m_JetIsTau = b;
	return;
}

void TrackJetEventTree::setJetNTracks(Char_t i)
{
	m_JetNTracks = i;
	return;
}

void TrackJetEventTree::setJetTruthLabel(Char_t i)
{
	m_JetTruthLabel = i;
	return;
}

void TrackJetEventTree::setTrackPx(double d)
{
	m_TrackPx = d;
	return;
}

void TrackJetEventTree::setTrackPy(double d)
{
	m_TrackPy = d;
	return;
}

void TrackJetEventTree::setTrackPz(double d)
{
	m_TrackPz = d;
	return;
}

void TrackJetEventTree::setTrackEta(double d)
{
	m_TrackEta = d;
	return;
}

void TrackJetEventTree::setTrackPhi(double d)
{
	m_TrackPhi = d;
	return;
}

void TrackJetEventTree::setTrackSinTheta(double d)
{
	m_TrackSinTheta = d;
	return;
}

void TrackJetEventTree::setTrackCharge(Float_t i)
{
	m_TrackCharge = i;
	return;
}

void TrackJetEventTree::setTrackParticleOriginType(Char_t i)
{
	m_TrackParticleOriginType = i;
	return;
}

void TrackJetEventTree::setTrackPVx(double d)
{
	m_TrackPVx = d;
	return;
}

void TrackJetEventTree::setTrackPVy(double d)
{
	m_TrackPVy = d;
	return;
}

void TrackJetEventTree::setTrackPVz(double d)
{
	m_TrackPVz = d;
	return;
}

void TrackJetEventTree::setTrackTruthPVx(double d)
{
	m_TrackTruthPVx = d;
	return;
}

void TrackJetEventTree::setTrackTruthPVy(double d)
{
	m_TrackTruthPVy = d;
	return;
}

void TrackJetEventTree::setTrackTruthPVz(double d)
{
	m_TrackTruthPVz = d;
	return;
}

void TrackJetEventTree::setTrackTruthProdX(double d)
{
	m_TrackTruthProdX = d;
	return;
}

void TrackJetEventTree::setTrackTruthProdY(double d)
{
	m_TrackTruthProdY = d;
	return;
}

void TrackJetEventTree::setTrackTruthProdZ(double d)
{
	m_TrackTruthProdZ = d;
	return;
}

void TrackJetEventTree::setTrackIsFromV0(Char_t b)
{
	m_TrackIsFromV0 = b;
	return;
}

void TrackJetEventTree::setTrackTruthIPTrans(double d)
{
	m_TrackTruthIPTrans = d;
	return;
}

void TrackJetEventTree::setTrackTruthIPLong(double d)
{
	m_TrackTruthIPLong = d;
	return;
}

void TrackJetEventTree::setTrackRecoIPTrans0(double d)
{
	m_TrackRecoIPTrans0 = d;
	return;
}

void TrackJetEventTree::setTrackRecoIPLong0(double d)
{
	m_TrackRecoIPLong0 = d;
	return;
}

void TrackJetEventTree::setTrackRecoIPTransPv(double d)
{
	m_TrackRecoIPTransPv = d;
	return;
}

void TrackJetEventTree::setTrackRecoIPLongPv(double d)
{
	m_TrackRecoIPLongPv = d;
	return;
}

void TrackJetEventTree::setTrackRecoIPTransInfo(double d)
{
	m_TrackRecoIPTransInfo = d;
	return;
}

void TrackJetEventTree::setTrackRecoIPLongInfo(double d)
{
	m_TrackRecoIPLongInfo = d;
	return;
}

void TrackJetEventTree::setTrackRecoIPTransInfoNew(double d)
{
	m_TrackRecoIPTransInfoNew = d;
	return;
}

void TrackJetEventTree::setTrackRecoIPLongInfoNew(double d)
{
	m_TrackRecoIPLongInfoNew = d;
	return;
}

void TrackJetEventTree::setTrackRecoIPTrans0New(double d)
{
	m_TrackRecoIPTrans0New = d;
	return;
}

void TrackJetEventTree::setTrackRecoIPLong0New(double d)
{
	m_TrackRecoIPLong0New = d;
	return;
}

void TrackJetEventTree::setTrackPerigeeX(double d)
{
	m_TrackPerigeeX = d;
	return;
}

void TrackJetEventTree::setTrackPerigeeY(double d)
{
	m_TrackPerigeeY = d;
	return;
}

void TrackJetEventTree::setTrackPerigeeZ(double d)
{
	m_TrackPerigeeZ = d;
	return;
}

void TrackJetEventTree::setTrackIPTransError(double d)
{
	m_TrackIPTransError = d;
	return;
}

void TrackJetEventTree::setTrackIPLongError(double d)
{
	m_TrackIPLongError = d;
	return;
}

void TrackJetEventTree::setTrackIsHitPixelBarrel0(Char_t b)
{
	m_TrackIsHitPixelBarrel0 = b;
	return;
}

void TrackJetEventTree::setTrackIsHitPixelBarrel1(Char_t b)
{
	m_TrackIsHitPixelBarrel1 = b;
	return;
}

void TrackJetEventTree::setTrackIsHitPixelBarrel2(Char_t b)
{
	m_TrackIsHitPixelBarrel2 = b;
	return;
}

void TrackJetEventTree::setTrackIsHitPixelEndcap0(Char_t b)
{
	m_TrackIsHitPixelEndcap0 = b;
	return;
}

void TrackJetEventTree::setTrackIsHitPixelEndcap1(Char_t b)
{
	m_TrackIsHitPixelEndcap1 = b;
	return;
}

void TrackJetEventTree::setTrackIsHitPixelEndcap2(Char_t b)
{
	m_TrackIsHitPixelEndcap2 = b;
	return;
}

void TrackJetEventTree::setTrackIsHitSctBarrel0(Char_t b)
{
	m_TrackIsHitSctBarrel0 = b;
	return;
}

void TrackJetEventTree::setTrackIsHitSctBarrel1(Char_t b)
{
	m_TrackIsHitSctBarrel1 = b;
	return;
}

void TrackJetEventTree::setTrackIsHitSctBarrel2(Char_t b)
{
	m_TrackIsHitSctBarrel2 = b;
	return;
}

void TrackJetEventTree::setTrackIsHitSctBarrel3(Char_t b)
{
	m_TrackIsHitSctBarrel3 = b;
	return;
}

void TrackJetEventTree::setTrackIsHitSctEndcap0(Char_t b)
{
	m_TrackIsHitSctEndcap0 = b;
	return;
}

void TrackJetEventTree::setTrackIsHitSctEndcap1(Char_t b)
{
	m_TrackIsHitSctEndcap1 = b;
	return;
}

void TrackJetEventTree::setTrackIsHitSctEndcap2(Char_t b)
{
	m_TrackIsHitSctEndcap2 = b;
	return;
}

void TrackJetEventTree::setTrackIsHitSctEndcap3(Char_t b)
{
	m_TrackIsHitSctEndcap3 = b;
	return;
}

void TrackJetEventTree::setTrackIsHitSctEndcap4(Char_t b)
{
	m_TrackIsHitSctEndcap4 = b;
	return;
}

void TrackJetEventTree::setTrackIsHitSctEndcap5(Char_t b)
{
	m_TrackIsHitSctEndcap5 = b;
	return;
}

void TrackJetEventTree::setTrackIsHitSctEndcap6(Char_t b)
{
	m_TrackIsHitSctEndcap6 = b;
	return;
}

void TrackJetEventTree::setTrackIsHitSctEndcap7(Char_t b)
{
	m_TrackIsHitSctEndcap7 = b;
	return;
}

void TrackJetEventTree::setTrackIsHitSctEndcap8(Char_t b)
{
	m_TrackIsHitSctEndcap8 = b;
	return;
}

void TrackJetEventTree::setTrackIsHitTrtBarrel(Char_t b)
{
	m_TrackIsHitTrtBarrel = b;
	return;
}

void TrackJetEventTree::setTrackIsHitTrtEndcap(Char_t b)
{
	m_TrackIsHitTrtEndcap = b;
	return;
}

void TrackJetEventTree::setTrackNumberOfBLayerHits(Char_t i)
{
	m_TrackNumberOfBLayerHits = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfPixelHoles(Char_t i)
{
	m_TrackNumberOfPixelHoles = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfPixelHits(Char_t i)
{
	m_TrackNumberOfPixelHits = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfGangedPixels(Char_t i)
{
	m_TrackNumberOfGangedPixels = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfSCTHits(Char_t i)
{
	m_TrackNumberOfSCTHits = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfSCTHoles(Char_t i)
{
	m_TrackNumberOfSCTHoles = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfTRTHits(Char_t i)
{
	m_TrackNumberOfTRTHits = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfTRTHighThresholdHits(Char_t i)
{
	m_TrackNumberOfTRTHighThresholdHits = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfTRTOutliers(Char_t i)
{
	m_TrackNumberOfTRTOutliers = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfTRTHighThresholdOutliers(Char_t i)
{
	m_TrackNumberOfTRTHighThresholdOutliers = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfMdtHits(Char_t i)
{
	m_TrackNumberOfMdtHits = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfTgcPhiHits(Char_t i)
{
	m_TrackNumberOfTgcPhiHits = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfTgcEtaHits(Char_t i)
{
	m_TrackNumberOfTgcEtaHits = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfCscPhiHits(Char_t i)
{
	m_TrackNumberOfCscPhiHits = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfCscEtaHits(Char_t i)
{
	m_TrackNumberOfCscEtaHits = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfRpcPhiHits(Char_t i)
{
	m_TrackNumberOfRpcPhiHits = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfRpcEtaHits(Char_t i)
{
	m_TrackNumberOfRpcEtaHits = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfCscEtaHoles(Char_t i)
{
	m_TrackNumberOfCscEtaHoles = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfCscPhiHoles(Char_t i)
{
	m_TrackNumberOfCscPhiHoles = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfRpcEtaHoles(Char_t i)
{
	m_TrackNumberOfRpcEtaHoles = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfRpcPhiHoles(Char_t i)
{
	m_TrackNumberOfRpcPhiHoles = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfMdtHoles(Char_t i)
{
	m_TrackNumberOfMdtHoles = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfTgcEtaHoles(Char_t i)
{
	m_TrackNumberOfTgcEtaHoles = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfTgcPhiHoles(Char_t i)
{
	m_TrackNumberOfTgcPhiHoles = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfOutliersOnTrack(Char_t i)
{
	m_TrackNumberOfOutliersOnTrack = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfBLayerSharedHits(Char_t i)
{
	m_TrackNumberOfBLayerSharedHits = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfPixelSharedHits(Char_t i)
{
	m_TrackNumberOfPixelSharedHits = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfSCTSharedHits(Char_t i)
{
	m_TrackNumberOfSCTSharedHits = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfSCTDoubleHoles(Char_t i)
{
	m_TrackNumberOfSCTDoubleHoles = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfContribPixelLayers(Char_t i)
{
	m_TrackNumberOfContribPixelLayers = i;
	return;
}

void TrackJetEventTree::setTrackStandardDeviationOfChi2OS(Char_t i)
{
	m_TrackStandardDeviationOfChi2OS = i;
	return;
}

void TrackJetEventTree::setTrackNumberOfBLayerOutliers(Char_t i)
{
	m_TrackNumberOfBLayerOutliers = i;
	return;
}


/**
 * return the event weight
 */
double TrackJetEventTree::getEventWeight()
{
	return m_EventWeight;
}

/**
 * return the run number
 */
Int_t TrackJetEventTree::getEventRunNumber()
{
	return m_EventRunNumber;
}

/**
 * return the event number
 */
Int_t TrackJetEventTree::getEventEventNumber()
{
	return m_EventEventNumber;
}

Char_t TrackJetEventTree::getEventIsData()
{
	return m_EventIsData;
}

double TrackJetEventTree::getEventETx()
{
	return m_EventETx;
}

double TrackJetEventTree::getEventETy()
{
	return m_EventETy;
}

double TrackJetEventTree::getEventETphi()
{
	return m_EventETphi;
}

Char_t TrackJetEventTree::getEventNElectrons()
{
	return m_EventNElectrons;
}

Char_t TrackJetEventTree::getEventNMuons()
{
	return m_EventNMuons;
}


Char_t TrackJetEventTree::getEventNPhotons()
{
	return m_EventNPhotons;
}

Char_t TrackJetEventTree::getEventNTaus()
{
	return m_EventNTaus;
}

std::string TrackJetEventTree::getEventJetAlgorithm()
{
	return m_EventJetAlgorithm;
}

Char_t TrackJetEventTree::getEventNJets()
{
	return m_EventNJets;
}


double TrackJetEventTree::getJetE()
{
	return m_JetE;
}

double TrackJetEventTree::getJetPx()
{
	return m_JetPx;
}

double TrackJetEventTree::getJetPy()
{
	return m_JetPy;
}

double TrackJetEventTree::getJetPz()
{
	return m_JetPz;
}

double TrackJetEventTree::getJetPt()
{
	return sqrt(m_JetPx * m_JetPx + m_JetPy * m_JetPy);
}

double TrackJetEventTree::getJetP()
{
	return sqrt(m_JetPx * m_JetPx + m_JetPy * m_JetPy + m_JetPz * m_JetPz);
}

double TrackJetEventTree::getJetEta()
{
	return m_JetEta;
}

double TrackJetEventTree::getJetPhi()
{
	return m_JetPhi;
}

double TrackJetEventTree::getJetSinTheta()
{
	return m_JetSinTheta;
}

Char_t TrackJetEventTree::getJetIsElectron()
{
	return m_JetIsElectron;
}

Char_t TrackJetEventTree::getJetIsMuon()
{
	return m_JetIsMuon;
}

Char_t TrackJetEventTree::getJetIsPhoton()
{
	return m_JetIsPhoton;
}

Char_t TrackJetEventTree::getJetIsTau()
{
	return m_JetIsTau;
}

Char_t TrackJetEventTree::getJetNTracks()
{
	return m_JetNTracks;
}

Char_t TrackJetEventTree::getJetTruthLabel()
{
	return m_JetTruthLabel;
}

double TrackJetEventTree::getTrackPx()
{
	return m_TrackPx;
}

double TrackJetEventTree::getTrackPy()
{
	return m_TrackPy;
}

double TrackJetEventTree::getTrackPz()
{
	return m_TrackPz;
}

double TrackJetEventTree::getTrackPt()
{
	return sqrt(m_TrackPx * m_TrackPx + m_TrackPy * m_TrackPy);
}

double TrackJetEventTree::getTrackP()
{
	return sqrt(m_TrackPx * m_TrackPx + m_TrackPy * m_TrackPy +
			m_TrackPz * m_TrackPz);
}

double TrackJetEventTree::getTrackEta()
{
	return m_TrackEta;
}

double TrackJetEventTree::getTrackPhi()
{
	return m_TrackPhi;
}

double TrackJetEventTree::getTrackSinTheta()
{
	return m_TrackSinTheta;
}

Float_t TrackJetEventTree::getTrackCharge()
{
	return m_TrackCharge;
}

Char_t TrackJetEventTree::getTrackParticleOriginType()
{
	return m_TrackParticleOriginType;
}

double TrackJetEventTree::getTrackPVx()
{
	return m_TrackPVx;
}

double TrackJetEventTree::getTrackPVy()
{
	return m_TrackPVy;
}

double TrackJetEventTree::getTrackPVz()
{
	return m_TrackPVz;
}

double TrackJetEventTree::getTrackTruthPVx()
{
	return m_TrackTruthPVx;
}

double TrackJetEventTree::getTrackTruthPVy()
{
	return m_TrackTruthPVy;
}

double TrackJetEventTree::getTrackTruthPVz()
{
	return m_TrackTruthPVz;
}

double TrackJetEventTree::getTrackTruthProdX()
{
	return m_TrackTruthProdX;
}

double TrackJetEventTree::getTrackTruthProdY()
{
	return m_TrackTruthProdY;
}

double TrackJetEventTree::getTrackTruthProdZ()
{
	return m_TrackTruthProdZ;
}

Char_t TrackJetEventTree::getTrackIsFromV0()
{
	return m_TrackIsFromV0;
}

double TrackJetEventTree::getTrackTruthIPTrans()
{
	return m_TrackTruthIPTrans;
}

double TrackJetEventTree::getTrackTruthIPLong()
{
	return m_TrackTruthIPLong;
}

double TrackJetEventTree::getTrackRecoIPTrans0()
{
	return m_TrackRecoIPTrans0;
}

double TrackJetEventTree::getTrackRecoIPLong0()
{
	return m_TrackRecoIPLong0;
}

double TrackJetEventTree::getTrackRecoIPTransPv()
{
	return m_TrackRecoIPTransPv;
}

double TrackJetEventTree::getTrackRecoIPLongPv()
{
	return m_TrackRecoIPLongPv;
}

double TrackJetEventTree::getTrackRecoIPTransInfo()
{
	return m_TrackRecoIPTransInfo;
}

double TrackJetEventTree::getTrackRecoIPLongInfo()
{
	return m_TrackRecoIPLongInfo;
}

double TrackJetEventTree::getTrackRecoIPTransInfoNew()
{
	return m_TrackRecoIPTransInfoNew;
}

double TrackJetEventTree::getTrackRecoIPLongInfoNew()
{
	return m_TrackRecoIPLongInfoNew;
}

double TrackJetEventTree::getTrackRecoIPTrans0New()
{
	return m_TrackRecoIPTrans0New;
}

double TrackJetEventTree::getTrackRecoIPLong0New()
{
	return m_TrackRecoIPLong0New;
}

double TrackJetEventTree::getTrackPerigeeX()
{
	return m_TrackPerigeeX;
}

double TrackJetEventTree::getTrackPerigeeY()
{
	return m_TrackPerigeeY;
}

double TrackJetEventTree::getTrackPerigeeZ()
{
	return m_TrackPerigeeZ;
}

double TrackJetEventTree::getTrackIPTrans()
{
	double x = m_TrackPerigeeX - m_TrackPVx;
	double y = m_TrackPerigeeY - m_TrackPVy;
	double ipTrans = sqrt( x * x + y * y );
	// sign of IPtrans is positive if dot product of jet
	// axis and IPvector >=0, else negative
	double sign = 0.0;
	if( (x * m_JetPx + y * m_JetPy) >= 0 ) {
		sign = +1.0;
	} else {
		sign = -1.0;
	}
	return sign * ipTrans;
}

double TrackJetEventTree::getTrackIPLong()
{
	// according to alan.poppleton@cern.ch
	double z = (m_TrackPerigeeZ - m_TrackPVz) * m_TrackSinTheta;
	double ipLong = fabs(z);
	// same procudure as for transversal
	double sign = 0.0;
	if( (z * m_JetPz ) >= 0 ) {
		sign = +1.0;
	} else {
		sign = -1.0;
	}
	return sign * ipLong;
}

double TrackJetEventTree::getTrackIPTransError()
{
	return m_TrackIPTransError;
}

double TrackJetEventTree::getTrackIPLongError()
{
	return m_TrackIPLongError;
}

Char_t TrackJetEventTree::getTrackIsHitPixelBarrel0()
{
	return m_TrackIsHitPixelBarrel0;
}

Char_t TrackJetEventTree::getTrackIsHitPixelBarrel1()
{
	return m_TrackIsHitPixelBarrel1;
}

Char_t TrackJetEventTree::getTrackIsHitPixelBarrel2()
{
	return m_TrackIsHitPixelBarrel2;
}

Char_t TrackJetEventTree::getTrackIsHitPixelEndcap0()
{
	return m_TrackIsHitPixelEndcap0;
}

Char_t TrackJetEventTree::getTrackIsHitPixelEndcap1()
{
	return m_TrackIsHitPixelEndcap1;
}

Char_t TrackJetEventTree::getTrackIsHitPixelEndcap2()
{
	return m_TrackIsHitPixelEndcap2;
}

Char_t TrackJetEventTree::getTrackIsHitSctBarrel0()
{
	return m_TrackIsHitSctBarrel0;
}

Char_t TrackJetEventTree::getTrackIsHitSctBarrel1()
{
	return m_TrackIsHitSctBarrel1;
}

Char_t TrackJetEventTree::getTrackIsHitSctBarrel2()
{
	return m_TrackIsHitSctBarrel2;
}

Char_t TrackJetEventTree::getTrackIsHitSctBarrel3()
{
	return m_TrackIsHitSctBarrel3;
}

Char_t TrackJetEventTree::getTrackIsHitSctEndcap0()
{
	return m_TrackIsHitSctEndcap0;
}

Char_t TrackJetEventTree::getTrackIsHitSctEndcap1()
{
	return m_TrackIsHitSctEndcap1;
}

Char_t TrackJetEventTree::getTrackIsHitSctEndcap2()
{
	return m_TrackIsHitSctEndcap2;
}

Char_t TrackJetEventTree::getTrackIsHitSctEndcap3()
{
	return m_TrackIsHitSctEndcap3;
}

Char_t TrackJetEventTree::getTrackIsHitSctEndcap4()
{
	return m_TrackIsHitSctEndcap4;
}

Char_t TrackJetEventTree::getTrackIsHitSctEndcap5()
{
	return m_TrackIsHitSctEndcap5;
}

Char_t TrackJetEventTree::getTrackIsHitSctEndcap6()
{
	return m_TrackIsHitSctEndcap6;
}

Char_t TrackJetEventTree::getTrackIsHitSctEndcap7()
{
	return m_TrackIsHitSctEndcap7;
}

Char_t TrackJetEventTree::getTrackIsHitSctEndcap8()
{
	return m_TrackIsHitSctEndcap8;
}

Char_t TrackJetEventTree::getTrackIsHitTrtBarrel()
{
	return m_TrackIsHitTrtBarrel;
}

Char_t TrackJetEventTree::getTrackIsHitTrtEndcap()
{
	return m_TrackIsHitTrtEndcap;
}

Char_t TrackJetEventTree::getTrackNumberOfBLayerHits()
{
	return m_TrackNumberOfBLayerHits;
}

Char_t TrackJetEventTree::getTrackNumberOfPixelHoles()
{
	return m_TrackNumberOfPixelHoles;
}

Char_t TrackJetEventTree::getTrackNumberOfPixelHits()
{
	return m_TrackNumberOfPixelHits;
}

Char_t TrackJetEventTree::getTrackNumberOfGangedPixels()
{
	return m_TrackNumberOfGangedPixels;
}

Char_t TrackJetEventTree::getTrackNumberOfSCTHits()
{
	return m_TrackNumberOfSCTHits;
}

Char_t TrackJetEventTree::getTrackNumberOfSCTHoles()
{
	return m_TrackNumberOfSCTHoles;
}

Char_t TrackJetEventTree::getTrackNumberOfTRTHits()
{
	return m_TrackNumberOfTRTHits;
}

Char_t TrackJetEventTree::getTrackNumberOfTRTHighThresholdHits()
{
	return m_TrackNumberOfTRTHighThresholdHits;
}

Char_t TrackJetEventTree::getTrackNumberOfTRTOutliers()
{
	return m_TrackNumberOfTRTOutliers;
}

Char_t TrackJetEventTree::getTrackNumberOfTRTHighThresholdOutliers()
{
	return m_TrackNumberOfTRTHighThresholdOutliers;
}

Char_t TrackJetEventTree::getTrackNumberOfMdtHits()
{
	return m_TrackNumberOfMdtHits;
}

Char_t TrackJetEventTree::getTrackNumberOfTgcPhiHits()
{
	return m_TrackNumberOfTgcPhiHits;
}

Char_t TrackJetEventTree::getTrackNumberOfTgcEtaHits()
{
	return m_TrackNumberOfTgcEtaHits;
}

Char_t TrackJetEventTree::getTrackNumberOfCscPhiHits()
{
	return m_TrackNumberOfCscPhiHits;
}

Char_t TrackJetEventTree::getTrackNumberOfCscEtaHits()
{
	return m_TrackNumberOfCscEtaHits;
}

Char_t TrackJetEventTree::getTrackNumberOfRpcPhiHits()
{
	return m_TrackNumberOfRpcPhiHits;
}

Char_t TrackJetEventTree::getTrackNumberOfRpcEtaHits()
{
	return m_TrackNumberOfRpcEtaHits;
}

Char_t TrackJetEventTree::getTrackNumberOfCscEtaHoles()
{
	return m_TrackNumberOfCscEtaHoles;
}

Char_t TrackJetEventTree::getTrackNumberOfCscPhiHoles()
{
	return m_TrackNumberOfCscPhiHoles;
}

Char_t TrackJetEventTree::getTrackNumberOfRpcEtaHoles()
{
	return m_TrackNumberOfRpcEtaHoles;
}

Char_t TrackJetEventTree::getTrackNumberOfRpcPhiHoles()
{
	return m_TrackNumberOfRpcPhiHoles;
}

Char_t TrackJetEventTree::getTrackNumberOfMdtHoles()
{
	return m_TrackNumberOfMdtHoles;
}

Char_t TrackJetEventTree::getTrackNumberOfTgcEtaHoles()
{
	return m_TrackNumberOfTgcEtaHoles;
}

Char_t TrackJetEventTree::getTrackNumberOfTgcPhiHoles()
{
	return m_TrackNumberOfTgcPhiHoles;
}

Char_t TrackJetEventTree::getTrackNumberOfOutliersOnTrack()
{
	return m_TrackNumberOfOutliersOnTrack;
}

Char_t TrackJetEventTree::getTrackNumberOfBLayerSharedHits()
{
	return m_TrackNumberOfBLayerSharedHits;
}

Char_t TrackJetEventTree::getTrackNumberOfPixelSharedHits()
{
	return m_TrackNumberOfPixelSharedHits;
}

Char_t TrackJetEventTree::getTrackNumberOfSCTSharedHits()
{
	return m_TrackNumberOfSCTSharedHits;
}

Char_t TrackJetEventTree::getTrackNumberOfSCTDoubleHoles()
{
	return m_TrackNumberOfSCTDoubleHoles;
}

Char_t TrackJetEventTree::getTrackNumberOfContribPixelLayers()
{
	return m_TrackNumberOfContribPixelLayers;
}

Char_t TrackJetEventTree::getTrackStandardDeviationOfChi2OS()
{
	return m_TrackStandardDeviationOfChi2OS;
}

Char_t TrackJetEventTree::getTrackNumberOfBLayerOutliers()
{
	return m_TrackNumberOfBLayerOutliers;
}

/**
 * init for each event
 */
void TrackJetEventTree::initFill()
{
	m_EventWeight = 0;
	m_EventRunNumber = 0;
	m_EventEventNumber = 0;
	m_EventIsData = 0;
	m_EventETx = 0;
	m_EventETy = 0;
	m_EventETphi = 0;
	m_EventNElectrons = 0;
	m_EventNMuons = 0;
	m_EventNPhotons = 0;
	m_EventNTaus = 0;
	// bug: Root TTree does not work with std::string
	//m_EventJetAlgorithm = "";
	//m_EventJetAlgorithm[30] = "";
	m_EventNJets = 0;

	m_JetE = 0;
	m_JetPx = 0;
	m_JetPy = 0;
	m_JetPz = 0;
	m_JetEta = 0;
	m_JetPhi = 0;
	m_JetSinTheta = 0;
	m_JetIsElectron = 0;
	m_JetIsMuon = 0;
	m_JetIsPhoton = 0;
	m_JetIsTau = 0;
	m_JetNTracks = 0;
	m_JetTruthLabel = 0;

	m_TrackPx = 0;
	m_TrackPy = 0;
	m_TrackPz = 0;
	m_TrackEta = 0;
	m_TrackPhi = 0;
	m_TrackSinTheta = 0;
	m_TrackCharge = 0;
	m_TrackParticleOriginType = 0;
	m_TrackPVx = 0;
	m_TrackPVy = 0;
	m_TrackPVz = 0;
	m_TrackTruthPVx = 0;
	m_TrackTruthPVy = 0;
	m_TrackTruthPVz = 0;
	m_TrackTruthProdX = 0;
	m_TrackTruthProdY = 0;
	m_TrackTruthProdZ = 0;
	m_TrackIsFromV0 = 0;
	m_TrackTruthIPTrans = 0;
	m_TrackTruthIPLong = 0;
	m_TrackRecoIPTrans0 = 0;
	m_TrackRecoIPLong0 = 0;
	m_TrackRecoIPTransPv = 0;
	m_TrackRecoIPLongPv = 0;
	m_TrackRecoIPTransInfo = 0;
	m_TrackRecoIPLongInfo = 0;
	m_TrackRecoIPTransInfoNew = 0;
	m_TrackRecoIPLongInfoNew = 0;
	m_TrackRecoIPTrans0New = 0;
	m_TrackRecoIPLong0New = 0;
	m_TrackPerigeeX = 0;
	m_TrackPerigeeY = 0;
	m_TrackPerigeeZ = 0;
	m_TrackIPTransError = 0;
	m_TrackIPLongError = 0;
	m_TrackIsHitPixelBarrel0 = 0;
	m_TrackIsHitPixelBarrel1 = 0;
	m_TrackIsHitPixelBarrel2 = 0;
	m_TrackIsHitPixelEndcap0 = 0;
	m_TrackIsHitPixelEndcap1 = 0;
	m_TrackIsHitPixelEndcap2 = 0;
	m_TrackIsHitSctBarrel0 = 0;
	m_TrackIsHitSctBarrel1 = 0;
	m_TrackIsHitSctBarrel2 = 0;
	m_TrackIsHitSctBarrel3 = 0;
	m_TrackIsHitSctEndcap0 = 0;
	m_TrackIsHitSctEndcap1 = 0;
	m_TrackIsHitSctEndcap2 = 0;
	m_TrackIsHitSctEndcap3 = 0;
	m_TrackIsHitSctEndcap4 = 0;
	m_TrackIsHitSctEndcap5 = 0;
	m_TrackIsHitSctEndcap6 = 0;
	m_TrackIsHitSctEndcap7 = 0;
	m_TrackIsHitSctEndcap8 = 0;
	m_TrackIsHitTrtBarrel = 0;
	m_TrackIsHitTrtEndcap = 0;
	m_TrackNumberOfBLayerHits = 0;
	m_TrackNumberOfPixelHoles = 0;
	m_TrackNumberOfPixelHits = 0;
	m_TrackNumberOfGangedPixels = 0;
	m_TrackNumberOfSCTHits = 0;
	m_TrackNumberOfSCTHoles = 0;
	m_TrackNumberOfTRTHits = 0;
	m_TrackNumberOfTRTHighThresholdHits = 0;
	m_TrackNumberOfTRTOutliers = 0;
	m_TrackNumberOfTRTHighThresholdOutliers = 0;
	m_TrackNumberOfMdtHits = 0;
	m_TrackNumberOfTgcPhiHits = 0;
	m_TrackNumberOfTgcEtaHits = 0;
	m_TrackNumberOfCscPhiHits = 0;
	m_TrackNumberOfCscEtaHits = 0;
	m_TrackNumberOfRpcPhiHits = 0;
	m_TrackNumberOfRpcEtaHits = 0;
	m_TrackNumberOfCscEtaHoles = 0;
	m_TrackNumberOfCscPhiHoles = 0;
	m_TrackNumberOfRpcEtaHoles = 0;
	m_TrackNumberOfRpcPhiHoles = 0;
	m_TrackNumberOfMdtHoles = 0;
	m_TrackNumberOfTgcEtaHoles = 0;
	m_TrackNumberOfTgcPhiHoles = 0;
	m_TrackNumberOfOutliersOnTrack = 0;
	m_TrackNumberOfBLayerSharedHits = 0;
	m_TrackNumberOfPixelSharedHits = 0;
	m_TrackNumberOfSCTSharedHits = 0;
	m_TrackNumberOfSCTDoubleHoles = 0;
	m_TrackNumberOfContribPixelLayers = 0;
	m_TrackStandardDeviationOfChi2OS = 0;
	m_TrackNumberOfBLayerOutliers = 0;
	return;
}

/**
 * write current track with event/jet information
 */
void TrackJetEventTree::fill()
{
	m_tree->Fill();
	return;
}

/**
 * initialize private variables
 */
void TrackJetEventTree::m_initPrivateVars()
{
	m_created = false;
	m_histServiceFile = "";
	m_histServiceDirectory = "";

	m_EventWeight = 0;
	m_EventRunNumber = 0;
	m_EventEventNumber = 0;
	m_EventIsData = 0;
	m_EventETx = 0;
	m_EventETy = 0;
	m_EventETphi = 0;
	m_EventNElectrons = 0;
	m_EventNMuons = 0;
	m_EventNPhotons = 0;
	m_EventNTaus = 0;
	// bug: Root TTree does not work with std::string
	//m_EventJetAlgorithm = "";
	//m_EventJetAlgorithm[30] = "";
	m_EventNJets = 0;

	m_JetE = 0;
	m_JetPx = 0;
	m_JetPy = 0;
	m_JetPz = 0;
	m_JetEta = 0;
	m_JetPhi = 0;
	m_JetSinTheta = 0;
	m_JetIsElectron = 0;
	m_JetIsMuon = 0;
	m_JetIsPhoton = 0;
	m_JetIsTau = 0;
	m_JetNTracks = 0;
	m_JetTruthLabel = 0;

	m_TrackPx = 0;
	m_TrackPy = 0;
	m_TrackPz = 0;
	m_TrackEta = 0;
	m_TrackPhi = 0;
	m_TrackSinTheta = 0;
	m_TrackCharge = 0;
	m_TrackParticleOriginType = 0;
	m_TrackPVx = 0;
	m_TrackPVy = 0;
	m_TrackPVz = 0;
	m_TrackTruthPVx = 0;
	m_TrackTruthPVy = 0;
	m_TrackTruthPVz = 0;
	m_TrackTruthProdX = 0;
	m_TrackTruthProdY = 0;
	m_TrackTruthProdZ = 0;
	m_TrackIsFromV0 = 0;
	m_TrackTruthIPTrans = 0;
	m_TrackTruthIPLong = 0;
	m_TrackRecoIPTrans0 = 0;
	m_TrackRecoIPLong0 = 0;
	m_TrackRecoIPTransPv = 0;
	m_TrackRecoIPLongPv = 0;
	m_TrackRecoIPTransInfo = 0;
	m_TrackRecoIPLongInfo = 0;
	m_TrackRecoIPTransInfoNew = 0;
	m_TrackRecoIPLongInfoNew = 0;
	m_TrackRecoIPTrans0New = 0;
	m_TrackRecoIPLong0New = 0;
	m_TrackPerigeeX = 0;
	m_TrackPerigeeY = 0;
	m_TrackPerigeeZ = 0;
	m_TrackIPTransError = 0;
	m_TrackIPLongError = 0;
	m_TrackIsHitPixelBarrel0 = 0;
	m_TrackIsHitPixelBarrel1 = 0;
	m_TrackIsHitPixelBarrel2 = 0;
	m_TrackIsHitPixelEndcap0 = 0;
	m_TrackIsHitPixelEndcap1 = 0;
	m_TrackIsHitPixelEndcap2 = 0;
	m_TrackIsHitSctBarrel0 = 0;
	m_TrackIsHitSctBarrel1 = 0;
	m_TrackIsHitSctBarrel2 = 0;
	m_TrackIsHitSctBarrel3 = 0;
	m_TrackIsHitSctEndcap0 = 0;
	m_TrackIsHitSctEndcap1 = 0;
	m_TrackIsHitSctEndcap2 = 0;
	m_TrackIsHitSctEndcap3 = 0;
	m_TrackIsHitSctEndcap4 = 0;
	m_TrackIsHitSctEndcap5 = 0;
	m_TrackIsHitSctEndcap6 = 0;
	m_TrackIsHitSctEndcap7 = 0;
	m_TrackIsHitSctEndcap8 = 0;
	m_TrackIsHitTrtBarrel = 0;
	m_TrackIsHitTrtEndcap = 0;
	m_TrackNumberOfBLayerHits = 0;
	m_TrackNumberOfPixelHoles = 0;
	m_TrackNumberOfPixelHits = 0;
	m_TrackNumberOfGangedPixels = 0;
	m_TrackNumberOfSCTHits = 0;
	m_TrackNumberOfSCTHoles = 0;
	m_TrackNumberOfTRTHits = 0;
	m_TrackNumberOfTRTHighThresholdHits = 0;
	m_TrackNumberOfTRTOutliers = 0;
	m_TrackNumberOfTRTHighThresholdOutliers = 0;
	m_TrackNumberOfMdtHits = 0;
	m_TrackNumberOfTgcPhiHits = 0;
	m_TrackNumberOfTgcEtaHits = 0;
	m_TrackNumberOfCscPhiHits = 0;
	m_TrackNumberOfCscEtaHits = 0;
	m_TrackNumberOfRpcPhiHits = 0;
	m_TrackNumberOfRpcEtaHits = 0;
	m_TrackNumberOfCscEtaHoles = 0;
	m_TrackNumberOfCscPhiHoles = 0;
	m_TrackNumberOfRpcEtaHoles = 0;
	m_TrackNumberOfRpcPhiHoles = 0;
	m_TrackNumberOfMdtHoles = 0;
	m_TrackNumberOfTgcEtaHoles = 0;
	m_TrackNumberOfTgcPhiHoles = 0;
	m_TrackNumberOfOutliersOnTrack = 0;
	m_TrackNumberOfBLayerSharedHits = 0;
	m_TrackNumberOfPixelSharedHits = 0;
	m_TrackNumberOfSCTSharedHits = 0;
	m_TrackNumberOfSCTDoubleHoles = 0;
	m_TrackNumberOfContribPixelLayers = 0;
	m_TrackStandardDeviationOfChi2OS = 0;
	m_TrackNumberOfBLayerOutliers = 0;
}

/**
 * creates tree branches
 */
void TrackJetEventTree::m_createTreeBranches()
{
	std::string m_separationChar = "_";
	std::string longInt = "/I";
	std::string shortInt = "/B";
	std::string longDouble = "/D";
	std::string shortDouble = "/F";
	std::string stringName = "/C";

	m_tree->Branch( (m_event + m_separationChar + m_eventWeight).c_str(),
			&m_EventWeight,
			(m_eventWeight + shortDouble ).c_str() );
	m_tree->Branch( (m_event + m_separationChar + m_eventNumber).c_str(),
			&m_EventRunNumber,
			(m_eventNumber + longInt ).c_str() );
	m_tree->Branch( (m_event + m_separationChar + m_runNumber).c_str(),
			&m_EventEventNumber,
			(m_runNumber + longInt ).c_str() );
	m_tree->Branch( (m_event + m_separationChar + m_isData).c_str(),
			&m_EventIsData,
			(m_isData + shortInt ).c_str() );
	m_tree->Branch( (m_event + m_separationChar + m_missingETx).c_str(),
			&m_EventETx,
			(m_missingETx + longDouble ).c_str() );
	m_tree->Branch( (m_event + m_separationChar + m_missingETy).c_str(),
			&m_EventETy,
			(m_missingETy + longDouble ).c_str() );
	m_tree->Branch( (m_event + m_separationChar + m_missingETphi)
			.c_str(),
			&m_EventETphi,
			(m_missingETphi + longDouble ).c_str() );
	m_tree->Branch( (m_event + m_separationChar + m_NElectrons).c_str(),
			&m_EventNElectrons,
			(m_NElectrons + shortInt ).c_str() );
	m_tree->Branch( (m_event + m_separationChar + m_NMuons).c_str(),
			&m_EventNMuons,
			(m_NMuons + shortInt ).c_str() );
	m_tree->Branch( (m_event + m_separationChar + m_NPhotons).c_str(),
			&m_EventNPhotons,
			(m_NPhotons + shortInt ).c_str() );
	m_tree->Branch( (m_event + m_separationChar + m_NTaus).c_str(),
			&m_EventNTaus,
			(m_NTaus + shortInt ).c_str() );
	m_tree->Branch( (m_event + m_separationChar + m_jetAlgorithm)
			.c_str(),
			&m_EventJetAlgorithm,
			(m_jetAlgorithm + stringName).c_str() );
	m_tree->Branch( (m_event + m_separationChar + m_NJets).c_str(),
			&m_EventNJets,
			(m_NJets + shortInt ).c_str() );
	m_tree->Branch( (m_jet + m_separationChar + m_e).c_str(),
			&m_JetE,
			(m_e + longDouble ).c_str() );
	m_tree->Branch( (m_jet + m_separationChar + m_px).c_str(),
			&m_JetPx,
			(m_px + longDouble ).c_str() );
	m_tree->Branch( (m_jet + m_separationChar + m_py).c_str(),
			&m_JetPy,
			(m_py + longDouble ).c_str() );
	m_tree->Branch( (m_jet + m_separationChar + m_pz).c_str(),
			&m_JetPz,
			(m_pz + longDouble ).c_str() );
	m_tree->Branch( (m_jet + m_separationChar + m_eta).c_str(),
			&m_JetEta,
			(m_eta + longDouble ).c_str() );
	m_tree->Branch( (m_jet + m_separationChar + m_phi).c_str(),
			&m_JetPhi,
			(m_phi + longDouble ).c_str() );
	m_tree->Branch( (m_jet + m_separationChar + m_sinTheta).c_str(),
			&m_JetSinTheta,
			(m_sinTheta + longDouble ).c_str() );
	m_tree->Branch( (m_jet + m_separationChar + m_isElectron).c_str(),
			&m_JetIsElectron,
			(m_isElectron + shortInt ).c_str() );
	m_tree->Branch( (m_jet + m_separationChar + m_isMuon).c_str(),
			&m_JetIsMuon,
			(m_isMuon + shortInt ).c_str() );
	m_tree->Branch( (m_jet + m_separationChar + m_isPhoton).c_str(),
			&m_JetIsPhoton,
			(m_isPhoton + shortInt ).c_str() );
	m_tree->Branch( (m_jet + m_separationChar + m_isTau).c_str(),
			&m_JetIsTau,
			(m_isTau + shortInt ).c_str() );
	m_tree->Branch( (m_jet + m_separationChar + m_NTracks).c_str(),
			&m_JetNTracks,
			(m_NTracks + shortInt ).c_str() );
	m_tree->Branch( (m_jet + m_separationChar + m_truth + m_label)
			.c_str(),
			&m_JetTruthLabel,
			(m_truth + m_label + shortInt ).c_str() );

	m_tree->Branch( (m_track + m_separationChar + m_px).c_str(),
			&m_TrackPx,
			(m_px + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_py).c_str(),
			&m_TrackPy,
			(m_py + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_pz).c_str(),
			&m_TrackPz,
			(m_pz + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_eta).c_str(),
			&m_TrackEta,
			(m_eta + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_phi).c_str(),
			&m_TrackPhi,
			(m_phi + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_sinTheta).c_str(),
			&m_TrackSinTheta,
			(m_sinTheta + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_charge).c_str(),
			&m_TrackCharge,
			(m_charge + shortDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_particleOriginType)
			.c_str(),
			&m_TrackParticleOriginType,
			(m_particleOriginType + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_PVx).c_str(),
			&m_TrackPVx,
			(m_PVx + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_PVy).c_str(),
			&m_TrackPVy,
			(m_PVy + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_PVz).c_str(),
			&m_TrackPVz,
			(m_PVz + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_truth + m_PVx)
			.c_str(),
			&m_TrackTruthPVx,
			(m_truth + m_PVx + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_truth + m_PVy)
			.c_str(),
			&m_TrackTruthPVy,
			(m_truth + m_PVy + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_truth + m_PVz)
			.c_str(),
			&m_TrackTruthPVz,
			(m_truth + m_PVz + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_truth + m_prodX)
			.c_str(),
			&m_TrackTruthProdX,
			(m_truth + m_prodX + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_truth + m_prodY)
			.c_str(),
			&m_TrackTruthProdY,
			(m_truth + m_prodY + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_truth + m_prodZ)
			.c_str(),
			&m_TrackTruthProdZ,
			(m_truth + m_prodZ + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isFromV0).c_str(),
			&m_TrackIsFromV0,
			(m_isFromV0 + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_truth + m_ipTrans)
			.c_str(),
			&m_TrackTruthIPTrans,
			(m_truth + m_ipTrans + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_truth + m_ipLong)
			.c_str(),
			&m_TrackTruthIPLong,
			(m_truth + m_ipLong + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_reco + m_ipTrans +
			"0").c_str(),
			&m_TrackRecoIPTrans0,
			(m_reco + m_ipTrans + "0" + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_reco + m_ipLong +
			"0").c_str(),
			&m_TrackRecoIPLong0,
			(m_reco + m_ipLong + "0" + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_reco + m_ipTrans +
			"Pv").c_str(),
			&m_TrackRecoIPTransPv,
			(m_reco + m_ipTrans + "Pv" + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_reco + m_ipLong +
			"Pv").c_str(),
			&m_TrackRecoIPLongPv,
			(m_reco + m_ipLong + "Pv" + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_reco + m_ipTrans +
			"Info").c_str(),
			&m_TrackRecoIPTransInfo,
			(m_reco + m_ipTrans + "Info" + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_reco + m_ipLong +
			"Info").c_str(),
			&m_TrackRecoIPLongInfo,
			(m_reco + m_ipLong + "Info" + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_reco + m_ipTrans +
			"InfoNew").c_str(),
			&m_TrackRecoIPTransInfoNew,
			(m_reco + m_ipTrans + "InfoNew" + longDouble )
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_reco + m_ipLong +
			"InfoNew").c_str(),
			&m_TrackRecoIPLongInfoNew,
			(m_reco + m_ipLong + "InfoNew" + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_reco + m_ipTrans +
			"0New").c_str(),
			&m_TrackRecoIPTrans0New,
			(m_reco + m_ipTrans + "0New" + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_reco + m_ipLong +
			"0New").c_str(),
			&m_TrackRecoIPLong0New,
			(m_reco + m_ipLong + "0New" + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_perigeex).c_str(),
			&m_TrackPerigeeX,
			(m_perigeex + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_perigeey).c_str(),
			&m_TrackPerigeeY,
			(m_perigeey + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_perigeez).c_str(),
			&m_TrackPerigeeZ,
			(m_perigeez + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_ipTransError)
			.c_str(),
			&m_TrackIPTransError,
			(m_ipTransError + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_ipLongError)
			.c_str(),
			&m_TrackIPLongError,
			(m_ipLongError + longDouble ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_pixel +
			m_barrel + m_zero).c_str(),
			&m_TrackIsHitPixelBarrel0,
			(m_isHit + m_pixel + m_barrel + m_zero + shortInt )
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_pixel +
			m_barrel + m_one).c_str(),
			&m_TrackIsHitPixelBarrel1,
			(m_isHit + m_pixel + m_barrel + m_one + shortInt )
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_pixel +
			m_barrel + m_two).c_str(),
			&m_TrackIsHitPixelBarrel2,
			(m_isHit + m_pixel + m_barrel + m_two + shortInt)
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_pixel +
			m_endcap + m_zero).c_str(),
			&m_TrackIsHitPixelEndcap0,
			(m_isHit + m_pixel + m_endcap + m_zero + shortInt)
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_pixel +
			m_endcap + m_one).c_str(),
			&m_TrackIsHitPixelEndcap1,
			(m_isHit + m_pixel + m_endcap + m_one + shortInt)
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_pixel +
			m_endcap + m_two).c_str(),
			&m_TrackIsHitPixelEndcap2,
			(m_isHit + m_pixel + m_endcap + m_two + shortInt)
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_sct +
			m_barrel + m_zero).c_str(),
			&m_TrackIsHitSctBarrel0,
			(m_isHit + m_sct + m_barrel + m_zero + shortInt)
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_sct +
			m_barrel + m_one).c_str(),
			&m_TrackIsHitSctBarrel1,
			(m_isHit + m_sct + m_barrel + m_one + shortInt)
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_sct +
			m_barrel + m_two).c_str(),
			&m_TrackIsHitSctBarrel2,
			(m_isHit + m_sct + m_barrel + m_two + shortInt)
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_sct +
			m_barrel + m_three).c_str(),
			&m_TrackIsHitSctBarrel3,
			(m_isHit + m_sct + m_barrel + m_three + shortInt)
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_sct +
			m_endcap + m_zero).c_str(),
			&m_TrackIsHitSctEndcap0,
			(m_isHit + m_sct + m_endcap + m_zero + shortInt)
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_sct +
			m_endcap + m_one).c_str(),
			&m_TrackIsHitSctEndcap1,
			(m_isHit + m_sct + m_endcap + m_one + shortInt)
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_sct +
			m_endcap + m_two).c_str(),
			&m_TrackIsHitSctEndcap2,
			(m_isHit + m_sct + m_endcap + m_two + shortInt)
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_sct +
			m_endcap + m_three).c_str(),
			&m_TrackIsHitSctEndcap3,
			(m_isHit + m_sct + m_endcap + m_three + shortInt)
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_sct +
			m_endcap + m_four).c_str(),
			&m_TrackIsHitSctEndcap4,
			(m_isHit + m_sct + m_endcap + m_four + shortInt)
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_sct +
			m_endcap + m_five).c_str(),
			&m_TrackIsHitSctEndcap5,
			(m_isHit + m_sct + m_endcap + m_five + shortInt)
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_sct +
			m_endcap + m_six).c_str(),
			&m_TrackIsHitSctEndcap6,
			(m_isHit + m_sct + m_endcap + m_six + shortInt)
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_sct +
			m_endcap + m_seven).c_str(),
			&m_TrackIsHitSctEndcap7,
			(m_isHit + m_sct + m_endcap + m_seven + shortInt)
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_sct +
			m_endcap + m_eight).c_str(),
			&m_TrackIsHitSctEndcap8,
			(m_isHit + m_sct + m_endcap + m_eight + shortInt)
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_trt +
			m_barrel).c_str(),
			&m_TrackIsHitTrtBarrel,
			(m_isHit+m_trt+m_barrel + shortInt).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_isHit + m_trt +
			m_endcap).c_str(),
			&m_TrackIsHitTrtEndcap,
			(m_isHit+m_trt+m_endcap + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_BLayerHits).c_str(),
			&m_TrackNumberOfBLayerHits,
			(m_numberOf+m_BLayerHits + shortInt).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_PixelHoles).c_str(),
			&m_TrackNumberOfPixelHoles,
			(m_numberOf+m_PixelHoles + shortInt).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_PixelHits).c_str(),
			&m_TrackNumberOfPixelHits,
			(m_numberOf+m_PixelHits + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_GangedPixels).c_str(),
			&m_TrackNumberOfGangedPixels,
			(m_numberOf+m_GangedPixels + shortInt).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_SCTHits).c_str(),
			&m_TrackNumberOfSCTHits,
			(m_numberOf+m_SCTHits + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_SCTHoles).c_str(),
			&m_TrackNumberOfSCTHoles,
			(m_numberOf+m_SCTHoles + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_TRTHits).c_str(),
			&m_TrackNumberOfTRTHits,
			(m_numberOf+m_TRTHits + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf+
			m_TRTHighThresholdHits).c_str(),
			&m_TrackNumberOfTRTHighThresholdHits,
			(m_numberOf+m_TRTHighThresholdHits + shortInt)
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_TRTOutliers).c_str(),
			&m_TrackNumberOfTRTOutliers,
			(m_numberOf+m_TRTOutliers + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf+
			m_TRTHighThresholdOutliers).c_str(),
			&m_TrackNumberOfTRTHighThresholdOutliers,
			(m_numberOf+m_TRTHighThresholdOutliers + shortInt )
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_MdtHits).c_str(),
			&m_TrackNumberOfMdtHits,
			(m_numberOf+m_MdtHits + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_TgcPhiHits).c_str(),
			&m_TrackNumberOfTgcPhiHits,
			(m_numberOf+m_TgcPhiHits + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_TgcEtaHits).c_str(),
			&m_TrackNumberOfTgcEtaHits,
			(m_numberOf+m_TgcEtaHits + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_CscPhiHits).c_str(),
			&m_TrackNumberOfCscPhiHits,
			(m_numberOf+m_CscPhiHits + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_CscEtaHits).c_str(),
			&m_TrackNumberOfCscEtaHits,
			(m_numberOf+m_CscEtaHits + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_RpcPhiHits).c_str(),
			&m_TrackNumberOfRpcPhiHits,
			(m_numberOf+m_RpcPhiHits + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_RpcEtaHits).c_str(),
			&m_TrackNumberOfRpcEtaHits,
			(m_numberOf+m_RpcEtaHits + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_CscEtaHoles).c_str(),
			&m_TrackNumberOfCscEtaHoles,
			(m_numberOf+m_CscEtaHoles + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_CscPhiHoles).c_str(),
			&m_TrackNumberOfCscPhiHoles,
			(m_numberOf+m_CscPhiHoles + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_RpcEtaHoles).c_str(),
			&m_TrackNumberOfRpcEtaHoles,
			(m_numberOf+m_RpcEtaHoles + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_RpcPhiHoles).c_str(),
			&m_TrackNumberOfRpcPhiHoles,
			(m_numberOf+m_RpcPhiHoles + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_MdtHoles).c_str(),
			&m_TrackNumberOfMdtHoles,
			(m_numberOf+m_MdtHoles + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_TgcEtaHoles).c_str(),
			&m_TrackNumberOfTgcEtaHoles,
			(m_numberOf+m_TgcEtaHoles + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_TgcPhiHoles).c_str(),
			&m_TrackNumberOfTgcPhiHoles,
			(m_numberOf+m_TgcPhiHoles + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_OutliersOnTrack).c_str(),
			&m_TrackNumberOfOutliersOnTrack,
			(m_numberOf+m_OutliersOnTrack + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_BLayerSharedHits).c_str(),
			&m_TrackNumberOfBLayerSharedHits,
			(m_numberOf+m_BLayerSharedHits + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_PixelSharedHits).c_str(),
			&m_TrackNumberOfPixelSharedHits,
			(m_numberOf+m_PixelSharedHits + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_SCTSharedHits).c_str(),
			&m_TrackNumberOfSCTSharedHits,
			(m_numberOf+m_SCTSharedHits + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_SCTDoubleHoles).c_str(),
			&m_TrackNumberOfSCTDoubleHoles,
			(m_numberOf+m_SCTDoubleHoles + shortInt ).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_ContribPixelLayers).c_str(),
			&m_TrackNumberOfContribPixelLayers,
			(m_numberOf+m_ContribPixelLayers + shortInt).c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf+
			m_standardDeviationOfChi2OS).c_str(),
			&m_TrackStandardDeviationOfChi2OS,
			(m_numberOf+m_standardDeviationOfChi2OS + shortInt )
			.c_str() );
	m_tree->Branch( (m_track + m_separationChar + m_numberOf +
			m_BLayerOutliers).c_str(),
			&m_TrackNumberOfBLayerOutliers,
			(m_numberOf+m_BLayerOutliers + shortInt ).c_str() );
	return;
}

/**
 * load tree branches
 */
void TrackJetEventTree::m_loadTreeBranches()
{
	m_tree->SetBranchAddress( (m_event + m_separationChar +
			m_eventWeight).c_str(),
			&m_EventWeight);
	m_tree->SetBranchAddress( (m_event + m_separationChar +
			m_eventNumber).c_str(),
			&m_EventRunNumber);
	m_tree->SetBranchAddress( (m_event + m_separationChar +
			m_runNumber).c_str(),
			&m_EventEventNumber);
	m_tree->SetBranchAddress( (m_event + m_separationChar +
			m_isData).c_str(),
			&m_EventIsData);
	m_tree->SetBranchAddress( (m_event + m_separationChar +
			m_missingETx).c_str(),
			&m_EventETx);
	m_tree->SetBranchAddress( (m_event + m_separationChar +
			m_missingETy).c_str(),
			&m_EventETy);
	m_tree->SetBranchAddress( (m_event + m_separationChar +
			m_missingETphi).c_str(),
			&m_EventETphi);
	m_tree->SetBranchAddress( (m_event + m_separationChar +
			m_NElectrons).c_str(),
			&m_EventNElectrons);
	m_tree->SetBranchAddress( (m_event + m_separationChar +
			m_NMuons).c_str(),
			&m_EventNMuons);
	m_tree->SetBranchAddress( (m_event + m_separationChar +
			m_NPhotons).c_str(),
			&m_EventNPhotons);
	m_tree->SetBranchAddress( (m_event + m_separationChar +
			m_NTaus).c_str(),
			&m_EventNTaus);
	m_tree->SetBranchAddress( (m_event + m_separationChar +
			m_jetAlgorithm).c_str(),
			&m_EventJetAlgorithm);
	m_tree->SetBranchAddress( (m_event + m_separationChar +
			m_NJets).c_str(),
			&m_EventNJets);
	m_tree->SetBranchAddress( (m_jet + m_separationChar + m_e).c_str(),
			&m_JetE);
	m_tree->SetBranchAddress( (m_jet + m_separationChar + m_px).c_str(),
			&m_JetPx);
	m_tree->SetBranchAddress( (m_jet + m_separationChar + m_py).c_str(),
			&m_JetPy);
	m_tree->SetBranchAddress( (m_jet + m_separationChar + m_pz).c_str(),
			&m_JetPz);
	m_tree->SetBranchAddress( (m_jet + m_separationChar + m_eta).c_str(),
			&m_JetEta);
	m_tree->SetBranchAddress( (m_jet + m_separationChar + m_phi).c_str(),
			&m_JetPhi);
	m_tree->SetBranchAddress( (m_jet + m_separationChar +
			m_sinTheta).c_str(),
			&m_JetSinTheta);
	m_tree->SetBranchAddress( (m_jet + m_separationChar +
			m_isElectron).c_str(),
			&m_JetIsElectron);
	m_tree->SetBranchAddress( (m_jet + m_separationChar +
			m_isMuon).c_str(),
			&m_JetIsMuon);
	m_tree->SetBranchAddress( (m_jet + m_separationChar +
			m_isPhoton).c_str(),
			&m_JetIsPhoton);
	m_tree->SetBranchAddress( (m_jet + m_separationChar +
			m_isTau).c_str(),
			&m_JetIsTau);
	m_tree->SetBranchAddress( (m_jet + m_separationChar +
			m_NTracks).c_str(),
			&m_JetNTracks);
	m_tree->SetBranchAddress( (m_jet + m_separationChar +
			m_truth + m_label).c_str(),
			&m_JetTruthLabel);

	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_px).c_str(),
			&m_TrackPx);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_py).c_str(),
			&m_TrackPy);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_pz).c_str(),
			&m_TrackPz);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_eta).c_str(),
			&m_TrackEta);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_phi).c_str(),
			&m_TrackPhi);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_sinTheta).c_str(),
			&m_TrackSinTheta);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_charge).c_str(),
			&m_TrackCharge);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_particleOriginType).c_str(),
			&m_TrackParticleOriginType);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_PVx).c_str(),
			&m_TrackPVx);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_PVy).c_str(),
			&m_TrackPVy);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_PVz).c_str(),
			&m_TrackPVz);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_truth + m_PVx).c_str(),
			&m_TrackTruthPVx);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_truth + m_PVy).c_str(),
			&m_TrackTruthPVy);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_truth + m_PVz).c_str(),
			&m_TrackTruthPVz);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_truth + m_prodX).c_str(),
			&m_TrackTruthProdX);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_truth + m_prodY).c_str(),
			&m_TrackTruthProdY);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_truth + m_prodZ).c_str(),
			&m_TrackTruthProdZ);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_isFromV0).c_str(),
			&m_TrackIsFromV0);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_truth + m_ipTrans).c_str(),
			&m_TrackTruthIPTrans);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_truth + m_ipLong).c_str(),
			&m_TrackTruthIPLong);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_reco + m_ipTrans + "0").c_str(),
			&m_TrackRecoIPTrans0);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_reco + m_ipLong + "0").c_str(),
			&m_TrackRecoIPLong0);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_reco + m_ipTrans + "Pv").c_str(),
			&m_TrackRecoIPTransPv);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_reco + m_ipLong + "Pv").c_str(),
			&m_TrackRecoIPLongPv);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_reco + m_ipTrans + "Info").c_str(),
			&m_TrackRecoIPTransInfo);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_reco + m_ipLong + "Info").c_str(),
			&m_TrackRecoIPLongInfo);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_reco + m_ipTrans + "InfoNew").c_str(),
			&m_TrackRecoIPTransInfoNew);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_reco + m_ipLong + "InfoNew").c_str(),
			&m_TrackRecoIPLongInfoNew);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_reco + m_ipTrans + "0New").c_str(),
			&m_TrackRecoIPTrans0New);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_reco + m_ipLong + "0New").c_str(),
			&m_TrackRecoIPLong0New);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_perigeex).c_str(),
			&m_TrackPerigeeX);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_perigeey).c_str(),
			&m_TrackPerigeeY);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_perigeez).c_str(),
			&m_TrackPerigeeZ);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_ipTransError).c_str(),
			&m_TrackIPTransError);
	m_tree->SetBranchAddress( (m_track + m_separationChar +
			m_ipLongError).c_str(),
			&m_TrackIPLongError);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_pixel +
			m_barrel + m_zero).c_str(),
			&m_TrackIsHitPixelBarrel0);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_pixel +
			m_barrel + m_one).c_str(),
			&m_TrackIsHitPixelBarrel1);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_pixel +
			m_barrel + m_two).c_str(),
			&m_TrackIsHitPixelBarrel2);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_pixel +
			m_endcap + m_zero).c_str(),
			&m_TrackIsHitPixelEndcap0);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_pixel +
			m_endcap + m_one).c_str(),
			&m_TrackIsHitPixelEndcap1);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_pixel +
			m_endcap + m_two).c_str(),
			&m_TrackIsHitPixelEndcap2);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_sct +
			m_barrel + m_zero).c_str(),
			&m_TrackIsHitSctBarrel0);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_sct +
			m_barrel + m_one).c_str(),
			&m_TrackIsHitSctBarrel1);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_sct +
			m_barrel + m_two).c_str(),
			&m_TrackIsHitSctBarrel2);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_sct +
			m_barrel + m_three).c_str(),
			&m_TrackIsHitSctBarrel3);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_sct +
			m_endcap + m_zero).c_str(),
			&m_TrackIsHitSctEndcap0);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_sct +
			m_endcap + m_one).c_str(),
			&m_TrackIsHitSctEndcap1);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_sct +
			m_endcap + m_two).c_str(),
			&m_TrackIsHitSctEndcap2);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_sct +
			m_endcap + m_three).c_str(),
			&m_TrackIsHitSctEndcap3);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_sct +
			m_endcap + m_four).c_str(),
			&m_TrackIsHitSctEndcap4);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_sct +
			m_endcap + m_five).c_str(),
			&m_TrackIsHitSctEndcap5);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_sct +
			m_endcap + m_six).c_str(),
			&m_TrackIsHitSctEndcap6);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_sct +
			m_endcap + m_seven).c_str(),
			&m_TrackIsHitSctEndcap7);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_sct +
			m_endcap + m_eight).c_str(),
			&m_TrackIsHitSctEndcap8);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_trt +
			m_barrel).c_str(),
			&m_TrackIsHitTrtBarrel);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_isHit + m_trt +
			m_endcap).c_str(),
			&m_TrackIsHitTrtEndcap);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_BLayerHits).c_str(),
			&m_TrackNumberOfBLayerHits);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_PixelHoles).c_str(),
			&m_TrackNumberOfPixelHoles);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_PixelHits).c_str(),
			&m_TrackNumberOfPixelHits);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_GangedPixels).c_str(),
			&m_TrackNumberOfGangedPixels);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_SCTHits).c_str(),
			&m_TrackNumberOfSCTHits);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_SCTHoles).c_str(),
			&m_TrackNumberOfSCTHoles);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_TRTHits).c_str(),
			&m_TrackNumberOfTRTHits);
	m_tree->SetBranchAddress( (m_track + m_separationChar + m_numberOf+
			m_TRTHighThresholdHits).c_str(),
			&m_TrackNumberOfTRTHighThresholdHits);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_TRTOutliers).c_str(),
			&m_TrackNumberOfTRTOutliers);
	m_tree->SetBranchAddress( (m_track + m_separationChar + m_numberOf+
			m_TRTHighThresholdOutliers).c_str(),
			&m_TrackNumberOfTRTHighThresholdOutliers);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_MdtHits).c_str(),
			&m_TrackNumberOfMdtHits);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_TgcPhiHits).c_str(),
			&m_TrackNumberOfTgcPhiHits);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_TgcEtaHits).c_str(),
			&m_TrackNumberOfTgcEtaHits);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_CscPhiHits).c_str(),
			&m_TrackNumberOfCscPhiHits);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_CscEtaHits).c_str(),
			&m_TrackNumberOfCscEtaHits);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_RpcPhiHits).c_str(),
			&m_TrackNumberOfRpcPhiHits);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_RpcEtaHits).c_str(),
			&m_TrackNumberOfRpcEtaHits);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_CscEtaHoles).c_str(),
			&m_TrackNumberOfCscEtaHoles);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_CscPhiHoles).c_str(),
			&m_TrackNumberOfCscPhiHoles);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_RpcEtaHoles).c_str(),
			&m_TrackNumberOfRpcEtaHoles);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_RpcPhiHoles).c_str(),
			&m_TrackNumberOfRpcPhiHoles);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_MdtHoles).c_str(),
			&m_TrackNumberOfMdtHoles);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_TgcEtaHoles).c_str(),
			&m_TrackNumberOfTgcEtaHoles);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_TgcPhiHoles).c_str(),
			&m_TrackNumberOfTgcPhiHoles);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_OutliersOnTrack).c_str(),
			&m_TrackNumberOfOutliersOnTrack);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_BLayerSharedHits).c_str(),
			&m_TrackNumberOfBLayerSharedHits);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_PixelSharedHits).c_str(),
			&m_TrackNumberOfPixelSharedHits);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_SCTSharedHits).c_str(),
			&m_TrackNumberOfSCTSharedHits);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_SCTDoubleHoles).c_str(),
			&m_TrackNumberOfSCTDoubleHoles);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_ContribPixelLayers).c_str(),
			&m_TrackNumberOfContribPixelLayers);
	m_tree->SetBranchAddress( (m_track + m_separationChar + m_numberOf+
			m_standardDeviationOfChi2OS).c_str(),
			&m_TrackStandardDeviationOfChi2OS);
	m_tree->SetBranchAddress(
			(m_track + m_separationChar + m_numberOf +
			m_BLayerOutliers).c_str(),
			&m_TrackNumberOfBLayerOutliers);
	return;
}
