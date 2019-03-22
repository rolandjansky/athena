/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * NAME : 	TrigT1CaloRun3TauFex.cxx
 * PACKAGE : 	Trigger/L1CaloUpgrade/TrigT1CaloRun3TauFex
 *
 * AUTHOR : 	Denis Oliveira Damazio
 *
 *
 * **/

#include "TrigT1CaloBaseFex.h"
#include "TrigT1CaloRun3TauFex.h"
#include "CaloEvent/CaloCellContainer.h"
#include <math.h>
#include <string>
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TVector2.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/EmTauRoIAuxContainer.h"
#include "xAODTrigger/EmTauRoI.h"

TrigT1CaloRun3TauFex::TrigT1CaloRun3TauFex( const std::string& name, ISvcLocator* pSvcLocator ) : 
 TrigT1CaloBaseFex (name, pSvcLocator), 
 acc_score(0), 
 acc_newScore(0) 
{
       declareProperty("EnableMonitoring", m_enableMon=false);
       declareProperty("OutputClusterName", m_outputClusterName  = "SCluster" );
       declareProperty("TimeThreshold", m_timeThr = 200 );
}

/*
TrigT1CaloRun3TauFex::TrigT1CaloRun3TauFex( const std::string& name, ISvcLocator* pSvcLocator ) : TrigT1CaloBaseFex (name, pSvcLocator) {
       declareProperty("EnableMonitoring", m_enableMon=false);
       declareProperty("OutputClusterName", m_outputClusterName  = "SCluster" );
       declareProperty("TimeThreshold", m_timeThr = 200 );
}
*/
TrigT1CaloRun3TauFex::~TrigT1CaloRun3TauFex(){
	// finish base class
	delete acc_clusterET;
	delete acc_clusterIso;
}

StatusCode TrigT1CaloRun3TauFex::initialize(){
	
	if ( TrigT1CaloBaseFex::initialize().isFailure() ) return StatusCode::FAILURE;
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "initializing TrigT1CaloRun3TauFex" << endreq;
	std::string histoName(name());
	histoName+="Algo.root";
	m_histFile = new TFile(histoName.c_str(),"RECREATE");
        if ( m_enableMon ){
		m_EtSElectron = new TH1F("EtSElectron","Et of Super Cell based Electron",60,0,60);
		m_HadEtSElectron = new TH1F("HadEtSElectron","HadEt of Super Cell based Electron",60,0,6);
		m_EtaSElectron = new TH1F("EtaSElectron","Eta of Super Cell based Electron",50,-5.0,5.0);
		m_PhiSElectron = new TH1F("PhiSElectron","Phi of Super Cell based Electron",64,-3.2,3.2);
		m_EtSElectronEta = new TH2F("EtSElectronEta","Et of Super Cell based Electron versus Eta",50,-5.0,5.0,60,0,60);
		m_HadEtSElectronEta = new TH2F("HadEtSElectronEta","HadEt of Super Cell based Electron versus Eta",50,-5.0,5.0,60,0,60);
	}
	m_SupercellMapEM0 = new TH2F("SupercellMapEM0","Supercell map of EM0",98,-4.9,4.9,64,0,2*M_PI);
	m_SupercellMapEM1 = new TH2F("SupercellMapEM1","Supercell map of EM1",392,-4.9,4.9,64,0,2*M_PI);
	m_SupercellMapEM2 = new TH2F("SupercellMapEM2","Supercell map of EM2",392,-4.9,4.9,64,0,2*M_PI);
	m_SupercellMapEM3 = new TH2F("SupercellMapEM3","Supercell map of EM3",98,-4.9,4.9,64,0,2*M_PI);
	m_SupercellMapHAD = new TH2F("SupercellMapHAD","Supercell map of HAD",98,-4.9,4.9,64,0,2*M_PI);

	// Hard-code variable names - we don't want them to be configurable for now
	// Feel free to change the names
	// Also, note that you should not use the 'ConstAccessor' as this one can't write to the object
	acc_clusterET = new SG::AuxElement::Accessor<float>("R3ClusterET");
	acc_clusterIso = new SG::AuxElement::Accessor<float>("R3ClusterIso");
	return StatusCode::SUCCESS;
}

StatusCode TrigT1CaloRun3TauFex::finalize(){
	if ( TrigT1CaloBaseFex::finalize().isFailure() ) return StatusCode::FAILURE;
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "finalizing TrigT1CaloRun3TauFex" << endreq;
	if ( m_enableMon ) {
		m_histFile->Write();
		m_histFile->Close();
	}
	return StatusCode::SUCCESS;
}

StatusCode TrigT1CaloRun3TauFex::execute(){
	
        MsgStream msg(msgSvc(), name());
#ifndef NDEBUG
	msg << MSG::DEBUG << "execute TrigT1CaloRun3TauFex" << endreq;
#endif

	CaloCellContainer* scells(0);
	const xAOD::TriggerTowerContainer* TTs(0);
	if ( getContainers(scells, TTs).isFailure() || (TTs==0) ) {
		msg << MSG::WARNING << " Could not get containers" << endreq;
		return StatusCode::SUCCESS;
	}

	// Simply check cells which are above a given threshold (1GeV) 
	findCellsAbove_EMB2_EMEC2(scells,1e3,m_cellsAboveThr);

	// Prepare output containers (with all xAOD annoying features)
	xAOD::EmTauRoIContainer* clustersForTau = new xAOD::EmTauRoIContainer();
	xAOD::EmTauRoIAuxContainer* auxclustersForTau = new xAOD::EmTauRoIAuxContainer();
	clustersForTau->setStore(auxclustersForTau);
	clustersForTau->reserve(100);
	std::string clusterName(m_outputClusterName);
	if ( evtStore()->record(clustersForTau,clusterName).isFailure() ){
		msg << MSG::ERROR  << "recording was not possible" << endreq;
		return StatusCode::FAILURE;
	}
	if ( evtStore()->record(auxclustersForTau,clusterName+"Aux.").isFailure() ){
		msg << MSG::ERROR << "recording Aux was not possible" << endreq;
		return StatusCode::FAILURE;
	}

	// prepare all supercells vector in whole ATLAS detector
	createCellList(scells,allSuperCells);

	// clear all TH2 histograms for supercell map
	m_SupercellMapEM0->Reset();
	m_SupercellMapEM1->Reset();
	m_SupercellMapEM2->Reset();
	m_SupercellMapEM3->Reset();
	m_SupercellMapHAD->Reset();

	// fill energy in all TH2 histograms for supercell map
	for(auto scell : allSuperCells) {
		//if (scell->caloDDE()->getSampling()==0||scell->caloDDE()->getSampling()==4) {m_SupercellMapEM0->Fill(scell->eta(),scell->phi(),scell->et());}
		//if (scell->caloDDE()->getSampling()==1||scell->caloDDE()->getSampling()==5) {m_SupercellMapEM1->Fill(scell->eta(),scell->phi(),scell->et());}
		//if (scell->caloDDE()->getSampling()==2||scell->caloDDE()->getSampling()==6) {m_SupercellMapEM2->Fill(scell->eta(),scell->phi(),scell->et());}
		//if (scell->caloDDE()->getSampling()==3||scell->caloDDE()->getSampling()==7) {m_SupercellMapEM3->Fill(scell->eta(),scell->phi(),scell->et());}
		//if (scell->caloDDE()->getSampling()==0||scell->caloDDE()->getSampling()==4) {m_SupercellMapEM0->Fill(scell->eta(),scell->phi(),scell->energy());}
		//if (scell->caloDDE()->getSampling()==1||scell->caloDDE()->getSampling()==5) {m_SupercellMapEM1->Fill(scell->eta(),scell->phi(),scell->energy());}
		//if (scell->caloDDE()->getSampling()==2||scell->caloDDE()->getSampling()==6) {m_SupercellMapEM2->Fill(scell->eta(),scell->phi(),scell->energy());}
		//if (scell->caloDDE()->getSampling()==3||scell->caloDDE()->getSampling()==7) {m_SupercellMapEM3->Fill(scell->eta(),scell->phi(),scell->energy());}
		//else {m_SupercellMapHAD->Fill(scell->eta(),scell->phi(),scell->energy());}
	  if (scell->caloDDE()->getSampling()==0||scell->caloDDE()->getSampling()==4) {m_SupercellMapEM0->Fill(scell->eta(),TVector2::Phi_0_2pi(scell->phi()),scell->energy()/TMath::CosH(scell->eta()));}
	  if (scell->caloDDE()->getSampling()==1||scell->caloDDE()->getSampling()==5) {m_SupercellMapEM1->Fill(scell->eta(),TVector2::Phi_0_2pi(scell->phi()),scell->energy()/TMath::CosH(scell->eta()));}
	      if (scell->caloDDE()->getSampling()==2||scell->caloDDE()->getSampling()==6) {m_SupercellMapEM2->Fill(scell->eta(),TVector2::Phi_0_2pi(scell->phi()),scell->energy()/TMath::CosH(scell->eta()));}
		  if (scell->caloDDE()->getSampling()==3||scell->caloDDE()->getSampling()==7) {m_SupercellMapEM3->Fill(scell->eta(),TVector2::Phi_0_2pi(scell->phi()),scell->energy()/TMath::CosH(scell->eta()));}
		  else {m_SupercellMapHAD->Fill(scell->eta(),TVector2::Phi_0_2pi(scell->phi()),scell->energy()/TMath::CosH(scell->eta()));}
	}

	// Loop over seed cells, this should give us
	// 
	for( auto cellAbove : m_cellsAboveThr ) {
		// builds a vector with all the cells around the seed cell
		// with the size (deta,dphi)=(0.08,0.21)
		findCellsAround(scells, cellAbove, m_cellsAround,0.13,0.15); // large window
		// check if seed cell is a local maximum in 3*3 window
		if ( ! isCellEmMaximum ( m_cellsAround, cellAbove ) ) continue;

		float seedCenterEtaCourse = m_SupercellMapEM0->GetXaxis()->GetBinCenter( m_SupercellMapEM0->GetXaxis()->FindBin(cellAbove->eta()) );
		float seedCenterPhiCourse = m_SupercellMapEM0->GetYaxis()->GetBinCenter( m_SupercellMapEM0->GetYaxis()->FindBin(TVector2::Phi_0_2pi(cellAbove->phi())) );
		float seedCenterEtaFine = m_SupercellMapEM2->GetXaxis()->GetBinCenter( m_SupercellMapEM2->GetXaxis()->FindBin(cellAbove->eta()) );
		float seedCenterPhiFine = m_SupercellMapEM2->GetYaxis()->GetBinCenter( m_SupercellMapEM2->GetYaxis()->FindBin(TVector2::Phi_0_2pi(cellAbove->phi())) );
		float seedPlace=0;
		if (seedCenterEtaCourse-seedCenterEtaFine>0.0124&&seedCenterEtaCourse-seedCenterEtaFine<0.0126) seedPlace=0.0125; //  |*||
		if (seedCenterEtaCourse-seedCenterEtaFine>-0.0126&&seedCenterEtaCourse-seedCenterEtaFine<-0.0124) seedPlace=0.0375; // ||*|
		if (seedCenterEtaCourse-seedCenterEtaFine>0.0374&&seedCenterEtaCourse-seedCenterEtaFine<0.0376) seedPlace=0.; // *|||
		if (seedCenterEtaCourse-seedCenterEtaFine>-0.0376&&seedCenterEtaCourse-seedCenterEtaFine<-0.0374) seedPlace=0.05; // |||*

		msg << MSG::DEBUG  <<"Esumi_ seed center etadiff = "<<seedCenterEtaCourse-seedCenterEtaFine<<" , phidiff = "<<seedCenterPhiCourse-seedCenterPhiFine<<endreq;
		std::vector<float> EM0Supercells55;
		std::vector<float> EM1Supercells55;
		std::vector<float> EM2Supercells55;
		std::vector<float> EM3Supercells55;
		std::vector<float> HADSupercells55;
		std::vector<float> EM0Supercells33;
		std::vector<float> EM1Supercells33;
		std::vector<float> EM2Supercells33;
		std::vector<float> EM3Supercells33;
		std::vector<float> HADSupercells33;
		for (int iPhi=0;iPhi<5;iPhi++) {
			for (int iEta=0;iEta<5;iEta++) {
				if ( fabs(seedCenterEtaCourse-0.2+0.1*iEta)<4.9 ) {
					if ((seedCenterPhiCourse-0.2+0.1*iPhi)>0 && (seedCenterPhiCourse-0.2+0.1*iPhi)<2*M_PI) {
						EM0Supercells55.push_back(m_SupercellMapEM0->GetBinContent( m_SupercellMapEM0->GetBin(m_SupercellMapEM0->GetXaxis()->FindBin(seedCenterEtaCourse-0.2+0.1*iEta),m_SupercellMapEM0->GetYaxis()->FindBin(seedCenterPhiCourse-0.2+0.1*iPhi))));
						EM3Supercells55.push_back(m_SupercellMapEM3->GetBinContent( m_SupercellMapEM3->GetBin(m_SupercellMapEM3->GetXaxis()->FindBin(seedCenterEtaCourse-0.2+0.1*iEta),m_SupercellMapEM3->GetYaxis()->FindBin(seedCenterPhiCourse-0.2+0.1*iPhi))));
						HADSupercells55.push_back(m_SupercellMapHAD->GetBinContent( m_SupercellMapHAD->GetBin(m_SupercellMapHAD->GetXaxis()->FindBin(seedCenterEtaCourse-0.2+0.1*iEta),m_SupercellMapHAD->GetYaxis()->FindBin(seedCenterPhiCourse-0.2+0.1*iPhi))));
						if (iEta>0 && iEta<4 && iPhi>0 && iPhi<4) {
							EM0Supercells33.push_back(m_SupercellMapEM0->GetBinContent( m_SupercellMapEM0->GetBin(m_SupercellMapEM0->GetXaxis()->FindBin(seedCenterEtaCourse-0.2+0.1*iEta),m_SupercellMapEM0->GetYaxis()->FindBin(seedCenterPhiCourse-0.2+0.1*iPhi))));
							EM3Supercells33.push_back(m_SupercellMapEM3->GetBinContent( m_SupercellMapEM3->GetBin(m_SupercellMapEM3->GetXaxis()->FindBin(seedCenterEtaCourse-0.2+0.1*iEta),m_SupercellMapEM3->GetYaxis()->FindBin(seedCenterPhiCourse-0.2+0.1*iPhi))));
							HADSupercells33.push_back(m_SupercellMapHAD->GetBinContent( m_SupercellMapHAD->GetBin(m_SupercellMapHAD->GetXaxis()->FindBin(seedCenterEtaCourse-0.2+0.1*iEta),m_SupercellMapHAD->GetYaxis()->FindBin(seedCenterPhiCourse-0.2+0.1*iPhi))));
						}
					}
					if ((seedCenterPhiCourse-0.2+0.1*iPhi)<=0) {
						EM0Supercells55.push_back(m_SupercellMapEM0->GetBinContent( m_SupercellMapEM0->GetBin(m_SupercellMapEM0->GetXaxis()->FindBin(seedCenterEtaCourse-0.2+0.1*iEta),m_SupercellMapEM0->GetYaxis()->FindBin(seedCenterPhiCourse-0.2+0.1*iPhi+2.*M_PI))));
						EM3Supercells55.push_back(m_SupercellMapEM3->GetBinContent( m_SupercellMapEM3->GetBin(m_SupercellMapEM3->GetXaxis()->FindBin(seedCenterEtaCourse-0.2+0.1*iEta),m_SupercellMapEM3->GetYaxis()->FindBin(seedCenterPhiCourse-0.2+0.1*iPhi+2.*M_PI))));
						HADSupercells55.push_back(m_SupercellMapHAD->GetBinContent( m_SupercellMapHAD->GetBin(m_SupercellMapHAD->GetXaxis()->FindBin(seedCenterEtaCourse-0.2+0.1*iEta),m_SupercellMapHAD->GetYaxis()->FindBin(seedCenterPhiCourse-0.2+0.1*iPhi+2.*M_PI))));
						if (iEta>0 && iEta<4 && iPhi>0 && iPhi<4) {
							EM0Supercells33.push_back(m_SupercellMapEM0->GetBinContent( m_SupercellMapEM0->GetBin(m_SupercellMapEM0->GetXaxis()->FindBin(seedCenterEtaCourse-0.2+0.1*iEta),m_SupercellMapEM0->GetYaxis()->FindBin(seedCenterPhiCourse-0.2+0.1*iPhi+2.*M_PI))));
							EM3Supercells33.push_back(m_SupercellMapEM3->GetBinContent( m_SupercellMapEM3->GetBin(m_SupercellMapEM3->GetXaxis()->FindBin(seedCenterEtaCourse-0.2+0.1*iEta),m_SupercellMapEM3->GetYaxis()->FindBin(seedCenterPhiCourse-0.2+0.1*iPhi+2.*M_PI))));
							HADSupercells33.push_back(m_SupercellMapHAD->GetBinContent( m_SupercellMapHAD->GetBin(m_SupercellMapHAD->GetXaxis()->FindBin(seedCenterEtaCourse-0.2+0.1*iEta),m_SupercellMapHAD->GetYaxis()->FindBin(seedCenterPhiCourse-0.2+0.1*iPhi+2.*M_PI))));
						}
					}
					if ((seedCenterPhiCourse-0.2+0.1*iPhi)>=2*M_PI) {
						EM0Supercells55.push_back(m_SupercellMapEM0->GetBinContent( m_SupercellMapEM0->GetBin(m_SupercellMapEM0->GetXaxis()->FindBin(seedCenterEtaCourse-0.2+0.1*iEta),m_SupercellMapEM0->GetYaxis()->FindBin(seedCenterPhiCourse-0.2+0.1*iPhi-2.*M_PI))));
						EM3Supercells55.push_back(m_SupercellMapEM3->GetBinContent( m_SupercellMapEM3->GetBin(m_SupercellMapEM3->GetXaxis()->FindBin(seedCenterEtaCourse-0.2+0.1*iEta),m_SupercellMapEM3->GetYaxis()->FindBin(seedCenterPhiCourse-0.2+0.1*iPhi-2.*M_PI))));
						HADSupercells55.push_back(m_SupercellMapHAD->GetBinContent( m_SupercellMapHAD->GetBin(m_SupercellMapHAD->GetXaxis()->FindBin(seedCenterEtaCourse-0.2+0.1*iEta),m_SupercellMapHAD->GetYaxis()->FindBin(seedCenterPhiCourse-0.2+0.1*iPhi-2.*M_PI))));
						if (iEta>0 && iEta<4 && iPhi>0 && iPhi<4) {
							EM0Supercells33.push_back(m_SupercellMapEM0->GetBinContent( m_SupercellMapEM0->GetBin(m_SupercellMapEM0->GetXaxis()->FindBin(seedCenterEtaCourse-0.2+0.1*iEta),m_SupercellMapEM0->GetYaxis()->FindBin(seedCenterPhiCourse-0.2+0.1*iPhi-2.*M_PI))));
							EM3Supercells33.push_back(m_SupercellMapEM3->GetBinContent( m_SupercellMapEM3->GetBin(m_SupercellMapEM3->GetXaxis()->FindBin(seedCenterEtaCourse-0.2+0.1*iEta),m_SupercellMapEM3->GetYaxis()->FindBin(seedCenterPhiCourse-0.2+0.1*iPhi-2.*M_PI))));
							HADSupercells33.push_back(m_SupercellMapHAD->GetBinContent( m_SupercellMapHAD->GetBin(m_SupercellMapHAD->GetXaxis()->FindBin(seedCenterEtaCourse-0.2+0.1*iEta),m_SupercellMapHAD->GetYaxis()->FindBin(seedCenterPhiCourse-0.2+0.1*iPhi-2.*M_PI))));
						}
					}
				}
				if ( fabs(seedCenterEtaCourse-0.2+0.1*iEta)>=4.9 ) {
					EM0Supercells55.push_back(0.);
					EM3Supercells55.push_back(0.);
					HADSupercells55.push_back(0.);
					if (iEta>0 && iEta<4 && iPhi>0 && iPhi<4) {
						EM0Supercells33.push_back(0.);
						EM3Supercells33.push_back(0.);
						HADSupercells33.push_back(0.);
					}
				}
			}
		}

		for (int iPhi=0;iPhi<5;iPhi++) {
			for (int iEta=0;iEta<20;iEta++) {
				if ( fabs(seedCenterEtaFine-0.2+0.1*iEta+seedPlace)<4.9 ) {
					if ((seedCenterPhiFine-0.2+0.1*iPhi)>0 && (seedCenterPhiFine-0.2+0.1*iPhi)<2*M_PI) {
						EM1Supercells55.push_back(m_SupercellMapEM1->GetBinContent( m_SupercellMapEM1->GetBin(m_SupercellMapEM1->GetXaxis()->FindBin(seedCenterEtaFine-0.2+0.1*iEta+seedPlace),m_SupercellMapEM1->GetYaxis()->FindBin(seedCenterPhiFine-0.2+0.1*iPhi))));
						EM2Supercells55.push_back(m_SupercellMapEM2->GetBinContent( m_SupercellMapEM2->GetBin(m_SupercellMapEM2->GetXaxis()->FindBin(seedCenterEtaFine-0.2+0.1*iEta+seedPlace),m_SupercellMapEM2->GetYaxis()->FindBin(seedCenterPhiFine-0.2+0.1*iPhi))));
						if (iEta>3 && iEta<16 && iPhi>0 && iPhi<4) {
							EM1Supercells33.push_back(m_SupercellMapEM1->GetBinContent( m_SupercellMapEM1->GetBin(m_SupercellMapEM1->GetXaxis()->FindBin(seedCenterEtaFine-0.2+0.1*iEta+seedPlace),m_SupercellMapEM1->GetYaxis()->FindBin(seedCenterPhiFine-0.2+0.1*iPhi))));
							EM2Supercells33.push_back(m_SupercellMapEM2->GetBinContent( m_SupercellMapEM2->GetBin(m_SupercellMapEM2->GetXaxis()->FindBin(seedCenterEtaFine-0.2+0.1*iEta+seedPlace),m_SupercellMapEM2->GetYaxis()->FindBin(seedCenterPhiFine-0.2+0.1*iPhi))));
						}
					}
					if ((seedCenterPhiFine-0.2+0.1*iPhi)<=0) {
						EM1Supercells55.push_back(m_SupercellMapEM1->GetBinContent( m_SupercellMapEM1->GetBin(m_SupercellMapEM1->GetXaxis()->FindBin(seedCenterEtaFine-0.2+0.1*iEta+seedPlace),m_SupercellMapEM1->GetYaxis()->FindBin(seedCenterPhiFine-0.2+0.1*iPhi+2.*M_PI))));
						EM2Supercells55.push_back(m_SupercellMapEM2->GetBinContent( m_SupercellMapEM2->GetBin(m_SupercellMapEM2->GetXaxis()->FindBin(seedCenterEtaFine-0.2+0.1*iEta+seedPlace),m_SupercellMapEM2->GetYaxis()->FindBin(seedCenterPhiFine-0.2+0.1*iPhi+2.*M_PI))));
						if (iEta>3 && iEta<16 && iPhi>0 && iPhi<4) {
							EM1Supercells33.push_back(m_SupercellMapEM1->GetBinContent( m_SupercellMapEM1->GetBin(m_SupercellMapEM1->GetXaxis()->FindBin(seedCenterEtaFine-0.2+0.1*iEta+seedPlace),m_SupercellMapEM1->GetYaxis()->FindBin(seedCenterPhiFine-0.2+0.1*iPhi+2.*M_PI))));
							EM2Supercells33.push_back(m_SupercellMapEM2->GetBinContent( m_SupercellMapEM2->GetBin(m_SupercellMapEM2->GetXaxis()->FindBin(seedCenterEtaFine-0.2+0.1*iEta+seedPlace),m_SupercellMapEM2->GetYaxis()->FindBin(seedCenterPhiFine-0.2+0.1*iPhi+2.*M_PI))));
						}
					}
					if ((seedCenterPhiFine-0.2+0.1*iPhi)>=2*M_PI) {
						EM1Supercells55.push_back(m_SupercellMapEM1->GetBinContent( m_SupercellMapEM1->GetBin(m_SupercellMapEM1->GetXaxis()->FindBin(seedCenterEtaFine-0.2+0.1*iEta+seedPlace),m_SupercellMapEM1->GetYaxis()->FindBin(seedCenterPhiFine-0.2+0.1*iPhi-2.*M_PI))));
						EM2Supercells55.push_back(m_SupercellMapEM2->GetBinContent( m_SupercellMapEM2->GetBin(m_SupercellMapEM2->GetXaxis()->FindBin(seedCenterEtaFine-0.2+0.1*iEta+seedPlace),m_SupercellMapEM2->GetYaxis()->FindBin(seedCenterPhiFine-0.2+0.1*iPhi-2.*M_PI))));
						if (iEta>3 && iEta<16 && iPhi>0 && iPhi<4) {
							EM1Supercells33.push_back(m_SupercellMapEM1->GetBinContent( m_SupercellMapEM1->GetBin(m_SupercellMapEM1->GetXaxis()->FindBin(seedCenterEtaFine-0.2+0.1*iEta+seedPlace),m_SupercellMapEM1->GetYaxis()->FindBin(seedCenterPhiFine-0.2+0.1*iPhi-2.*M_PI))));
							EM2Supercells33.push_back(m_SupercellMapEM2->GetBinContent( m_SupercellMapEM2->GetBin(m_SupercellMapEM2->GetXaxis()->FindBin(seedCenterEtaFine-0.2+0.1*iEta+seedPlace),m_SupercellMapEM2->GetYaxis()->FindBin(seedCenterPhiFine-0.2+0.1*iPhi-2.*M_PI))));
						}
					}
				}
				if ( fabs(seedCenterEtaFine-0.2+0.1*iEta+seedPlace)>=4.9 ) {
					EM1Supercells55.push_back(0.);
					EM2Supercells55.push_back(0.);
					if ((seedCenterPhiFine-0.2+0.1*iPhi)>=2*M_PI) {
						EM1Supercells55.push_back(0.);
						EM2Supercells55.push_back(0.);
					}
				}
			}
		}

		float eFEXTDRcluser = eFEXTDRclus(EM0Supercells33, EM1Supercells33, EM2Supercells33, EM3Supercells33, HADSupercells33, seedPlace);
		float eFEXTDRIso = eFEXIsoTDR(EM2Supercells33,seedPlace);
		//bool iseFEXTDRIso = TrigT1CaloBaseFex::IseFEXIsoTDR (EM2Supercells33,seedPlace,0.6) ;


		// build cluster for tau
		//xAOD::EmTauRoI_v2* clForTau = new xAOD::EmTauRoI_v2();
		xAOD::EmTauRoI* clForTau = new xAOD::EmTauRoI();
		clustersForTau->push_back( clForTau );
		clForTau->setEta( cellAbove->eta() );
		clForTau->setPhi( cellAbove->phi() );

		(*acc_clusterET)(*clForTau) = eFEXTDRcluser;
		(*acc_clusterIso)(*clForTau) = eFEXTDRIso;
		
		// Retrieve tau properties
		//double score = (*acc_score)(clForTau);
		//msg << MSG::INFO << "Esumi score = "<<score<< endreq;

/*
		clForTau->setClusEnergyTDR( eFEXTDRcluser );
		clForTau->seteFEXIsoTDR( eFEXTDRIso );
		clForTau->setIseFEXTDRIso( iseFEXTDRIso );
*/		
	}
	// avoid memory leak
	if ( scells ) {scells->clear(); delete scells;}
	m_cellsAround.clear();
	m_cellsAround2.clear();
	m_TTsAround.clear();
	return StatusCode::SUCCESS;
}
