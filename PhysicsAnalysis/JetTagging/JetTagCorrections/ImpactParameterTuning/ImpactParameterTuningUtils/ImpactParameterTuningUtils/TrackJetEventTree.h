/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * TrackJetEventTree.h
 *
 * Stephan.Sandvoss@cern.ch
 */
#ifndef TRACKJETEVENTTREE_H
#define TRACKJETEVENTTREE_H

/**
 * intra-package includes
 */

/**
 * extra-package includes
 */

/**
 * system includes
 */
#include "TROOT.h"

class TFile;
class TTree;

/**
 * class TrackJetEventTree for storing the event/jet/track information
 */
class TrackJetEventTree {
	public:
		// constructor
		TrackJetEventTree(TFile *file);

		// destructor
		~TrackJetEventTree();

		// call for creation of tree and branches
		void create();

		// load the provided tree
		void load(TTree *tree);
	
		// bug: ITHistService/Root wants to have poChar_ters
		void setTree(TTree *tree);

		// bug: ITHistService/Root wants to have poChar_ters
		TTree *getTree();

		// provide number of entries of the tree
		Int_t getEntries();

		// get given entry of the tree
		void getEntry(Int_t i);

		// provide the tree name
		std::string getTreeName();

		// provide the description of the tree
		std::string getTreeLongName();

		// provide the ITHistService file name
		std::string getHistServiceFile();

		// provide the ITHistService directory name
		std::string getHistServiceDirectory();

		// provide both together
		std::string getHistServiceFullString();

		// set the ITHistService file name
		void setHistServiceFile(std::string s);

		// set the ITHistService directory name
		void setHistServiceDirectory(std::string s);

		void setEventWeight(double d);
		void setEventRunNumber(Int_t i);
		void setEventEventNumber(Int_t i);
		void setEventIsData(Char_t b);
		void setEventETx(double d);
		void setEventETy(double d);
		void setEventETphi(double d);
		void setEventNElectrons(Char_t i);
		void setEventNMuons(Char_t i);
		void setEventNPhotons(Char_t i);
		void setEventNTaus(Char_t i);
		void setEventJetAlgorithm(std::string s);
		void setEventNJets(Char_t i);

		void setJetE(double d);
		void setJetPx(double d);
		void setJetPy(double d);
		void setJetPz(double d);
		void setJetEta(double d);
		void setJetPhi(double d);
		void setJetSinTheta(double d);
		void setJetIsElectron(Char_t b);
		void setJetIsMuon(Char_t b);
		void setJetIsPhoton(Char_t b);
		void setJetIsTau(Char_t b);
		void setJetNTracks(Char_t i);
		void setJetTruthLabel(Char_t i);

		void setTrackPx(double d);
		void setTrackPy(double d);
		void setTrackPz(double d);
		void setTrackEta(double d);
		void setTrackPhi(double d);
		void setTrackSinTheta(double d);
		void setTrackCharge(Float_t i);
		void setTrackParticleOriginType(Char_t i);
		void setTrackPVx(double d);
		void setTrackPVy(double d);
		void setTrackPVz(double d);
		void setTrackTruthPVx(double d);
		void setTrackTruthPVy(double d);
		void setTrackTruthPVz(double d);
		void setTrackTruthProdX(double d);
		void setTrackTruthProdY(double d);
		void setTrackTruthProdZ(double d);
		void setTrackIsFromV0(Char_t b);
		void setTrackTruthIPTrans(double d);
		void setTrackTruthIPLong(double d);
		void setTrackRecoIPTrans0(double d);
		void setTrackRecoIPLong0(double d);
		void setTrackRecoIPTransPv(double d);
		void setTrackRecoIPLongPv(double d);
		void setTrackRecoIPTransInfo(double d);
		void setTrackRecoIPLongInfo(double d);
		void setTrackRecoIPTransInfoNew(double d);
		void setTrackRecoIPLongInfoNew(double d);
		void setTrackRecoIPTrans0New(double d);
		void setTrackRecoIPLong0New(double d);
		void setTrackPerigeeX(double d);
		void setTrackPerigeeY(double d);
		void setTrackPerigeeZ(double d);
		void setTrackIPTransError(double d);
		void setTrackIPLongError(double d);
		void setTrackIsHitPixelBarrel0(Char_t b);
		void setTrackIsHitPixelBarrel1(Char_t b);
		void setTrackIsHitPixelBarrel2(Char_t b);
		void setTrackIsHitPixelEndcap0(Char_t b);
		void setTrackIsHitPixelEndcap1(Char_t b);
		void setTrackIsHitPixelEndcap2(Char_t b);
		void setTrackIsHitSctBarrel0(Char_t b);
		void setTrackIsHitSctBarrel1(Char_t b);
		void setTrackIsHitSctBarrel2(Char_t b);
		void setTrackIsHitSctBarrel3(Char_t b);
		void setTrackIsHitSctEndcap0(Char_t b);
		void setTrackIsHitSctEndcap1(Char_t b);
		void setTrackIsHitSctEndcap2(Char_t b);
		void setTrackIsHitSctEndcap3(Char_t b);
		void setTrackIsHitSctEndcap4(Char_t b);
		void setTrackIsHitSctEndcap5(Char_t b);
		void setTrackIsHitSctEndcap6(Char_t b);
		void setTrackIsHitSctEndcap7(Char_t b);
		void setTrackIsHitSctEndcap8(Char_t b);
		void setTrackIsHitTrtBarrel(Char_t b);
		void setTrackIsHitTrtEndcap(Char_t b);
		void setTrackNumberOfBLayerHits(Char_t i);
		void setTrackNumberOfPixelHoles(Char_t i);
		void setTrackNumberOfPixelHits(Char_t i);
		void setTrackNumberOfGangedPixels(Char_t i);
		void setTrackNumberOfSCTHits(Char_t i);
		void setTrackNumberOfSCTHoles(Char_t i);
		void setTrackNumberOfTRTHits(Char_t i);
		void setTrackNumberOfTRTHighThresholdHits(Char_t i);
		void setTrackNumberOfTRTOutliers(Char_t i);
		void setTrackNumberOfTRTHighThresholdOutliers(Char_t i);
		void setTrackNumberOfMdtHits(Char_t i);
		void setTrackNumberOfTgcPhiHits(Char_t i);
		void setTrackNumberOfTgcEtaHits(Char_t i);
		void setTrackNumberOfCscPhiHits(Char_t i);
		void setTrackNumberOfCscEtaHits(Char_t i);
		void setTrackNumberOfRpcPhiHits(Char_t i);
		void setTrackNumberOfRpcEtaHits(Char_t i);
		void setTrackNumberOfCscEtaHoles(Char_t i);
		void setTrackNumberOfCscPhiHoles(Char_t i);
		void setTrackNumberOfRpcEtaHoles(Char_t i);
		void setTrackNumberOfRpcPhiHoles(Char_t i);
		void setTrackNumberOfMdtHoles(Char_t i);
		void setTrackNumberOfTgcEtaHoles(Char_t i);
		void setTrackNumberOfTgcPhiHoles(Char_t i);
		void setTrackNumberOfOutliersOnTrack(Char_t i);
		void setTrackNumberOfBLayerSharedHits(Char_t i);
		void setTrackNumberOfPixelSharedHits(Char_t i);
		void setTrackNumberOfSCTSharedHits(Char_t i);
		void setTrackNumberOfSCTDoubleHoles(Char_t i);
		void setTrackNumberOfContribPixelLayers(Char_t i);
		void setTrackStandardDeviationOfChi2OS(Char_t i);
		void setTrackNumberOfBLayerOutliers(Char_t i);


		double getEventWeight();
		Int_t getEventRunNumber();
		Int_t getEventEventNumber();
		Char_t getEventIsData();
		double getEventETx();
		double getEventETy();
		double getEventETphi();
		Char_t getEventNElectrons();
		Char_t getEventNMuons();
		Char_t getEventNPhotons();
		Char_t getEventNTaus();
		std::string getEventJetAlgorithm();
		Char_t getEventNJets();

		double getJetE();
		double getJetPx();
		double getJetPy();
		double getJetPz();
		double getJetPt();
		double getJetP();
		double getJetEta();
		double getJetPhi();
		double getJetSinTheta();
		Char_t getJetIsElectron();
		Char_t getJetIsMuon();
		Char_t getJetIsPhoton();
		Char_t getJetIsTau();
		Char_t getJetNTracks();
		Char_t getJetTruthLabel();

		double getTrackPx();
		double getTrackPy();
		double getTrackPz();
		double getTrackPt();
		double getTrackP();
		double getTrackEta();
		double getTrackPhi();
		double getTrackSinTheta();
		Float_t getTrackCharge();
		Char_t getTrackParticleOriginType();
		double getTrackPVx();
		double getTrackPVy();
		double getTrackPVz();
		double getTrackTruthPVx();
		double getTrackTruthPVy();
		double getTrackTruthPVz();
		double getTrackTruthProdX();
		double getTrackTruthProdY();
		double getTrackTruthProdZ();
		Char_t getTrackIsFromV0();
		double getTrackTruthIPTrans();
		double getTrackTruthIPLong();
		double getTrackRecoIPTrans0();
		double getTrackRecoIPLong0();
		double getTrackRecoIPTransPv();
		double getTrackRecoIPLongPv();
		double getTrackRecoIPTransInfo();
		double getTrackRecoIPLongInfo();
		double getTrackRecoIPTransInfoNew();
		double getTrackRecoIPLongInfoNew();
		double getTrackRecoIPTrans0New();
		double getTrackRecoIPLong0New();
		double getTrackPerigeeX();
		double getTrackPerigeeY();
		double getTrackPerigeeZ();
		double getTrackIPTrans();
		double getTrackIPLong();
		double getTrackIPTransError();
		double getTrackIPLongError();
		Char_t getTrackIsHitPixelBarrel0();
		Char_t getTrackIsHitPixelBarrel1();
		Char_t getTrackIsHitPixelBarrel2();
		Char_t getTrackIsHitPixelEndcap0();
		Char_t getTrackIsHitPixelEndcap1();
		Char_t getTrackIsHitPixelEndcap2();
		Char_t getTrackIsHitSctBarrel0();
		Char_t getTrackIsHitSctBarrel1();
		Char_t getTrackIsHitSctBarrel2();
		Char_t getTrackIsHitSctBarrel3();
		Char_t getTrackIsHitSctEndcap0();
		Char_t getTrackIsHitSctEndcap1();
		Char_t getTrackIsHitSctEndcap2();
		Char_t getTrackIsHitSctEndcap3();
		Char_t getTrackIsHitSctEndcap4();
		Char_t getTrackIsHitSctEndcap5();
		Char_t getTrackIsHitSctEndcap6();
		Char_t getTrackIsHitSctEndcap7();
		Char_t getTrackIsHitSctEndcap8();
		Char_t getTrackIsHitTrtBarrel();
		Char_t getTrackIsHitTrtEndcap();
		Char_t getTrackNumberOfBLayerHits();
		Char_t getTrackNumberOfPixelHoles();
		Char_t getTrackNumberOfPixelHits();
		Char_t getTrackNumberOfGangedPixels();
		Char_t getTrackNumberOfSCTHits();
		Char_t getTrackNumberOfSCTHoles();
		Char_t getTrackNumberOfTRTHits();
		Char_t getTrackNumberOfTRTHighThresholdHits();
		Char_t getTrackNumberOfTRTOutliers();
		Char_t getTrackNumberOfTRTHighThresholdOutliers();
		Char_t getTrackNumberOfMdtHits();
		Char_t getTrackNumberOfTgcPhiHits();
		Char_t getTrackNumberOfTgcEtaHits();
		Char_t getTrackNumberOfCscPhiHits();
		Char_t getTrackNumberOfCscEtaHits();
		Char_t getTrackNumberOfRpcPhiHits();
		Char_t getTrackNumberOfRpcEtaHits();
		Char_t getTrackNumberOfCscEtaHoles();
		Char_t getTrackNumberOfCscPhiHoles();
		Char_t getTrackNumberOfRpcEtaHoles();
		Char_t getTrackNumberOfRpcPhiHoles();
		Char_t getTrackNumberOfMdtHoles();
		Char_t getTrackNumberOfTgcEtaHoles();
		Char_t getTrackNumberOfTgcPhiHoles();
		Char_t getTrackNumberOfOutliersOnTrack();
		Char_t getTrackNumberOfBLayerSharedHits();
		Char_t getTrackNumberOfPixelSharedHits();
		Char_t getTrackNumberOfSCTSharedHits();
		Char_t getTrackNumberOfSCTDoubleHoles();
		Char_t getTrackNumberOfContribPixelLayers();
		Char_t getTrackStandardDeviationOfChi2OS();
		Char_t getTrackNumberOfBLayerOutliers();

		// init for each event
		void initFill();

		// fill the current values Char_to the TTree
		void fill();
	private:
		// the name of the TTree
		const std::string m_treeName;

		// the long name of the TTree
		const std::string m_treeNameLong;

		// the ITHistService file name
		std::string m_histServiceFile;

		// the ITHistService directory name
		std::string m_histServiceDirectory;

		// the tree
		TTree *m_tree;

		// true, if the tree was created, false if loaded
		Char_t m_created;

		// underscore
		const std::string m_separationChar;

		double m_EventWeight;
		Int_t m_EventRunNumber;
		Int_t m_EventEventNumber;
		Char_t m_EventIsData;
		double m_EventETx;
		double m_EventETy;
		double m_EventETphi;
		Char_t m_EventNElectrons;
		Char_t m_EventNMuons;
		Char_t m_EventNPhotons;
		Char_t m_EventNTaus;
		// bug: root TTree does not work with std::string
		//std::string m_EventJetAlgorithm;
		char m_EventJetAlgorithm[30];
		Char_t m_EventNJets;

		double m_JetE;
		double m_JetPx;
		double m_JetPy;
		double m_JetPz;
		double m_JetEta;
		double m_JetPhi;
		double m_JetSinTheta;
		Char_t m_JetIsElectron;
		Char_t m_JetIsMuon;
		Char_t m_JetIsPhoton;
		Char_t m_JetIsTau;
		Char_t m_JetNTracks;
		Char_t m_JetTruthLabel;

		double m_TrackPx;
		double m_TrackPy;
		double m_TrackPz;
		double m_TrackEta;
		double m_TrackPhi;
		double m_TrackSinTheta;
		Float_t m_TrackCharge;
		Char_t m_TrackParticleOriginType;
		double m_TrackPVx;
		double m_TrackPVy;
		double m_TrackPVz;
		double m_TrackTruthPVx;
		double m_TrackTruthPVy;
		double m_TrackTruthPVz;
		double m_TrackTruthProdX;
		double m_TrackTruthProdY;
		double m_TrackTruthProdZ;
		Char_t m_TrackIsFromV0;
		double m_TrackTruthIPTrans;
		double m_TrackTruthIPLong;
		double m_TrackRecoIPTrans0;
		double m_TrackRecoIPLong0;
		double m_TrackRecoIPTransPv;
		double m_TrackRecoIPLongPv;
		double m_TrackRecoIPTransInfo;
		double m_TrackRecoIPLongInfo;
		double m_TrackRecoIPTransInfoNew;
		double m_TrackRecoIPLongInfoNew;
		double m_TrackRecoIPTrans0New;
		double m_TrackRecoIPLong0New;
		double m_TrackPerigeeX;
		double m_TrackPerigeeY;
		double m_TrackPerigeeZ;
		double m_TrackIPTransError;
		double m_TrackIPLongError;
		Char_t m_TrackIsHitPixelBarrel0;
		Char_t m_TrackIsHitPixelBarrel1;
		Char_t m_TrackIsHitPixelBarrel2;
		Char_t m_TrackIsHitPixelEndcap0;
		Char_t m_TrackIsHitPixelEndcap1;
		Char_t m_TrackIsHitPixelEndcap2;
		Char_t m_TrackIsHitSctBarrel0;
		Char_t m_TrackIsHitSctBarrel1;
		Char_t m_TrackIsHitSctBarrel2;
		Char_t m_TrackIsHitSctBarrel3;
		Char_t m_TrackIsHitSctEndcap0;
		Char_t m_TrackIsHitSctEndcap1;
		Char_t m_TrackIsHitSctEndcap2;
		Char_t m_TrackIsHitSctEndcap3;
		Char_t m_TrackIsHitSctEndcap4;
		Char_t m_TrackIsHitSctEndcap5;
		Char_t m_TrackIsHitSctEndcap6;
		Char_t m_TrackIsHitSctEndcap7;
		Char_t m_TrackIsHitSctEndcap8;
		Char_t m_TrackIsHitTrtBarrel;
		Char_t m_TrackIsHitTrtEndcap;
		Char_t m_TrackNumberOfBLayerHits;
		Char_t m_TrackNumberOfPixelHoles;
		Char_t m_TrackNumberOfPixelHits;
		Char_t m_TrackNumberOfGangedPixels;
		Char_t m_TrackNumberOfSCTHits;
		Char_t m_TrackNumberOfSCTHoles;
		Char_t m_TrackNumberOfTRTHits;
		Char_t m_TrackNumberOfTRTHighThresholdHits;
		Char_t m_TrackNumberOfTRTOutliers;
		Char_t m_TrackNumberOfTRTHighThresholdOutliers;
		Char_t m_TrackNumberOfMdtHits;
		Char_t m_TrackNumberOfTgcPhiHits;
		Char_t m_TrackNumberOfTgcEtaHits;
		Char_t m_TrackNumberOfCscPhiHits;
		Char_t m_TrackNumberOfCscEtaHits;
		Char_t m_TrackNumberOfRpcPhiHits;
		Char_t m_TrackNumberOfRpcEtaHits;
		Char_t m_TrackNumberOfCscEtaHoles;
		Char_t m_TrackNumberOfCscPhiHoles;
		Char_t m_TrackNumberOfRpcEtaHoles;
		Char_t m_TrackNumberOfRpcPhiHoles;
		Char_t m_TrackNumberOfMdtHoles;
		Char_t m_TrackNumberOfTgcEtaHoles;
		Char_t m_TrackNumberOfTgcPhiHoles;
		Char_t m_TrackNumberOfOutliersOnTrack;
		Char_t m_TrackNumberOfBLayerSharedHits;
		Char_t m_TrackNumberOfPixelSharedHits;
		Char_t m_TrackNumberOfSCTSharedHits;
		Char_t m_TrackNumberOfSCTDoubleHoles;
		Char_t m_TrackNumberOfContribPixelLayers;
		Char_t m_TrackStandardDeviationOfChi2OS;
		Char_t m_TrackNumberOfBLayerOutliers;

		// initialize all private variables
		void m_initPrivateVars();

		const std::string m_truth;
		const std::string m_label;
		const std::string m_reco;
		const std::string m_prodX;
		const std::string m_prodY;
		const std::string m_prodZ;
		const std::string m_event;
		const std::string m_jet;
		const std::string m_track;
		const std::string m_isData;
		const std::string m_eventWeight;
		const std::string m_runNumber;
		const std::string m_eventNumber;
		const std::string m_missingETx;
		const std::string m_missingETy;
		const std::string m_missingETphi;
		const std::string m_NElectrons;
		const std::string m_NMuons;
		const std::string m_NPhotons;
		const std::string m_NTaus;
		const std::string m_isElectron;
		const std::string m_isMuon;
		const std::string m_isPhoton;
		const std::string m_isTau;
		const std::string m_jetAlgorithm;
		const std::string m_NJets;
		const std::string m_NTracks;
		const std::string m_e;
		const std::string m_px;
		const std::string m_py;
		const std::string m_pz;
		const std::string m_eta;
		const std::string m_phi;
		const std::string m_sinTheta;
		const std::string m_charge;
		const std::string m_particleOriginType;
		const std::string m_PVx;
		const std::string m_PVy;
		const std::string m_PVz;
		const std::string m_isFromV0;
		const std::string m_perigeex;
		const std::string m_perigeey;
		const std::string m_perigeez;
		const std::string m_ipTrans;
		const std::string m_ipLong;
		const std::string m_ipTransError;
		const std::string m_ipLongError;
		const std::string m_isHit;
		const std::string m_zero;
		const std::string m_one;
		const std::string m_two;
		const std::string m_three;
		const std::string m_four;
		const std::string m_five;
		const std::string m_six;
		const std::string m_seven;
		const std::string m_eight;
		const std::string m_pixel;
		const std::string m_sct;
		const std::string m_trt;
		const std::string m_barrel;
		const std::string m_endcap;
		const std::string m_numberOf;
		const std::string m_BLayerHits;
		const std::string m_PixelHoles;
		const std::string m_PixelHits;
		const std::string m_GangedPixels;
		const std::string m_SCTHits;
		const std::string m_SCTHoles;
		const std::string m_TRTHits;
		const std::string m_TRTHighThresholdHits;
		const std::string m_TRTOutliers;
		const std::string m_TRTHighThresholdOutliers;
		const std::string m_MdtHits;
		const std::string m_TgcPhiHits;
		const std::string m_TgcEtaHits;
		const std::string m_CscPhiHits;
		const std::string m_CscEtaHits;
		const std::string m_RpcPhiHits;
		const std::string m_RpcEtaHits;
		const std::string m_CscEtaHoles;
		const std::string m_CscPhiHoles;
		const std::string m_RpcEtaHoles;
		const std::string m_RpcPhiHoles;
		const std::string m_MdtHoles;
		const std::string m_TgcEtaHoles;
		const std::string m_TgcPhiHoles;
		const std::string m_OutliersOnTrack;
		const std::string m_BLayerSharedHits;
		const std::string m_PixelSharedHits;
		const std::string m_SCTSharedHits;
		const std::string m_SCTDoubleHoles;
		const std::string m_ContribPixelLayers;
		const std::string m_standardDeviationOfChi2OS;
		const std::string m_BLayerOutliers;

		// create all tree branches
		void m_createTreeBranches();

		// load all tree branches
		void m_loadTreeBranches();
};
#endif
