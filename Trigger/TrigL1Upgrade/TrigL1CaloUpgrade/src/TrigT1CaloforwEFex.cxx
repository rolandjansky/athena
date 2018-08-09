/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * NAME : 	TrigT1CaloforwEFex.cxx
 * PACKAGE : 	Trigger/L1CaloUpgrade/TrigT1CaloforwEFex
 *
 * AUTHOR : 	Denis Oliveira Damazio
 *
 *
 * **/

#include "TrigT1CaloBaseFex.h"
#include "TrigT1CaloforwEFex.h"
#include "CaloEvent/CaloCellContainer.h"
#include <math.h>
#include <string>
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"

TrigT1CaloforwEFex::TrigT1CaloforwEFex( const std::string& name, ISvcLocator* pSvcLocator ) : TrigT1CaloBaseFex (name, pSvcLocator) {
       declareProperty("EnableMonitoring", m_enableMon=false);
       declareProperty("OutputClusterName", m_outputClusterName  = "SClusterFor" );
       declareProperty("TimeThreshold", m_timeThr = 200 );
}

TrigT1CaloforwEFex::~TrigT1CaloforwEFex(){
	// finish base class
}

StatusCode TrigT1CaloforwEFex::initialize(){
	
	if ( TrigT1CaloBaseFex::initialize().isFailure() ) return StatusCode::FAILURE;
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "initializing TrigT1CaloforwEFex" << endreq;
        if ( m_enableMon ){
		std::string histoName(name());
		histoName+="Algo.root";
		m_histFile = new TFile(histoName.c_str(),"RECREATE");
		m_EtSElectron = new TH1F("EtSElectron","Et of Super Cell based Electron",60,0,60);
		m_HadEtSElectron = new TH1F("HadEtSElectron","HadEt of Super Cell based Electron",60,0,6);
		m_EtaSElectron = new TH1F("EtaSElectron","Eta of Super Cell based Electron",50,-5.0,5.0);
		m_PhiSElectron = new TH1F("PhiSElectron","Phi of Super Cell based Electron",64,-3.2,3.2);
		m_EtSElectronEta = new TH2F("EtSElectronEta","Et of Super Cell based Electron versus Eta",50,-5.0,5.0,60,0,60);
		m_HadEtSElectronEta = new TH2F("HadEtSElectronEta","HadEt of Super Cell based Electron versus Eta",50,-5.0,5.0,60,0,60);
		m_testEtavsR = new TH2F("testEtavsR","testEtavsR",50,2.5,5.0,250,3000,5500);
		m_testR_IW = new TH1F("testR_IW","testR_IW",250,3000,5500);
		m_testR_FCAL = new TH1F("testR_FCAL","testR_FCAL",250,3000,5500);
		m_fmax_vs_eta = new TH2F("fmax_vs_eta","fmax_vs_eta",50,2.5,5.0,120,-0.1,1.1);
		m_fmax_IW = new TH1F("fmax_IW","fmax_IW",120,-0.1,1.1);
		m_fmax_FCAL = new TH1F("fmax_FCAL","fmax_FCAL",120,-0.1,1.1);
	}
	return StatusCode::SUCCESS;
}

StatusCode TrigT1CaloforwEFex::finalize(){
	if ( TrigT1CaloBaseFex::finalize().isFailure() ) return StatusCode::FAILURE;
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "finalizing TrigT1CaloforwEFex" << endreq;
	if ( m_enableMon ) {
		m_histFile->Write();
		m_histFile->Close();
	}
	return StatusCode::SUCCESS;
}

StatusCode TrigT1CaloforwEFex::execute(){
	
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "execute TrigT1CaloforwEFex" << endreq;

	CaloCellContainer* scells(0);
	const xAOD::TriggerTowerContainer* TTs(0);
	if ( getContainers(scells, TTs).isFailure() || (TTs==0) ) {
		msg << MSG::WARNING << " Could not get containers" << endreq;
		return StatusCode::SUCCESS;
	}

	// Simply check cells which are above a given threshold (3GeV)
	findCellsAbove(scells,4e3,m_cellsAboveThr);
	// Prepare output containers (with all xAOD annoying features)
	xAOD::TrigEMClusterContainer* clusters = new xAOD::TrigEMClusterContainer();
	xAOD::TrigEMClusterAuxContainer* auxclusters = new xAOD::TrigEMClusterAuxContainer();
	clusters->setStore(auxclusters);
	std::string clusterName(m_outputClusterName);
	if ( evtStore()->record(clusters,clusterName).isFailure() ){
		msg << MSG::ERROR  << "recording was not possible" << endreq;
		return StatusCode::FAILURE;
	}
	if ( evtStore()->record(auxclusters,clusterName+"Aux.").isFailure() ){
		msg << MSG::ERROR << "recording Aux was not possible" << endreq;
		return StatusCode::FAILURE;
	}
	// Loop over seed cells, this should give us
	for( auto cellAbove : m_cellsAboveThr ) {
		if ( TMath::Abs( cellAbove->eta() ) < 2.47 ) continue;
		// builds a vector with all the cells around the seed cell
		// with the size (deta,dphi)=(0.08,0.21)
		findCellsAround(scells, cellAbove, m_cellsAround,0.21,0.21); // large window
		// some clean up 
		std::vector<CaloCell*> cellsAroundNNeg;
		for( auto cellAround : m_cellsAround ) {
			if ( cellAround->et() > -5e2 ) {cellsAroundNNeg.push_back(cellAround);
			//std::cout << "\t c : " << cellAround->et() << " " << cellAround->eta() << " " << cellAround->phi() << std::endl;
			}
		}
		m_cellsAround.clear();
		m_cellsAround.insert( m_cellsAround.end(), cellsAroundNNeg.begin(), cellsAroundNNeg.end() );
		cellsAroundNNeg.clear();
		float etaCluster, phiCluster,zCluster; // Cluster baricenter
		// Find cluster center (eta/phiCluster) based on the
		// energy weighted scell position
		findClusterFor(m_cellsAround, etaCluster, phiCluster,zCluster);
		//std::cout << " TESTING : " << cellAbove->eta() << " " << cellAbove->phi() << " " << etaCluster << " " << phiCluster << " " << zCluster << std::endl;
		// smaller cluster (closer to egamma standard)
		findCellsAround(scells, etaCluster, phiCluster, m_cellsAround,m_deta,m_dphi);
		float clusterTime=0;
		float clusterTimeWeight=0;
		double zClusterN = 0.0;
		double zClusterNE = 0.0;
		double maximumEt = -999.0;
		for( auto cellAround : m_cellsAround ) {
			if ( cellAround->et() > -5e2 ) {
			//std::cout << "\t c : " << cellAround->et() << " " << cellAround->z() << std::endl;
			zClusterN += cellAround->et() * cellAround->z();
			zClusterNE += cellAround->et() ;
			if ( cellAround->et() > 1.0001*maximumEt ) maximumEt = cellAround->et();
			}
			if ( cellAround->et() < m_timeThr ) continue;
			clusterTime+=cellAround->time()*cellAround->et();
			clusterTimeWeight+=cellAround->et();
		}
		if ( fabsf(clusterTimeWeight) > 0.1 )
		clusterTime /=clusterTimeWeight;
		else clusterTime = -999.99;
		//std::cout << zClusterN << " " << zClusterNE << " " << zClusterN/zClusterNE << std::endl;
		if ( m_enableMon && (zClusterNE > 0.1) ){
			m_testEtavsR->Fill ( fabsf( etaCluster ), fabsf( zClusterN/zClusterNE ) );
			if ( etaCluster < 3.3 )
			m_testR_IW->Fill ( fabsf( zClusterN/zClusterNE ) );
			else
			m_testR_FCAL->Fill ( fabsf( zClusterN/zClusterNE ) );
		}
		msg << MSG::DEBUG << "CELL versus CLUSTER : " << cellAbove->eta() << " " << cellAbove->phi() << " " << etaCluster << " " << phiCluster << 
 " " << cellAbove->eta()-etaCluster << " " << cellAbove->phi()-phiCluster << endreq;
		// if find the cluster position fails, etaCluster=999.0
		if ( etaCluster > 998.0 ) continue;
		// other cluster sizes for some of the shower shapes
		findCellsAround(scells, (const float)etaCluster, (const float)phiCluster, m_cellsAround2,0.03,0.11);
		// include TT (for Tile region only)
		findTTsAround(TTs, etaCluster, phiCluster, m_TTsAround);
		// check if seed cell is a local maximum (maybe could
		// do this check before)
		if ( ! isCellEmMaximum ( m_cellsAround, cellAbove ) ) continue;
		float et = sumEmCells( m_cellsAround )/TMath::CosH(cellAbove->eta());
		float clusterEmEnergy32 = sumEmCells2nd( m_cellsAround2 );
		float clusterEmEnergy72 = sumEmCells2nd( m_cellsAround );
		float clusterHadEnergy = sumHadCells( m_cellsAround );
		clusterHadEnergy += sumHadTTs( m_TTsAround );
		//if ( clusterEmEnergy32 < 10 ) continue;
		// some histograms (we could only check the ones
		// in the EFex part)
		if ( m_enableMon ){
			m_EtSElectron->Fill(et/1e3);
			m_EtaSElectron->Fill( etaCluster );
			m_PhiSElectron->Fill( phiCluster );
			m_HadEtSElectron->Fill( clusterHadEnergy/1e3 );
			m_EtSElectronEta->Fill( cellAbove->eta(), et/1e3);
			m_HadEtSElectronEta->Fill( cellAbove->eta(), clusterHadEnergy/1e3);
			if ( et > 0.1 ) {
			    m_fmax_vs_eta->Fill( fabsf( etaCluster ), maximumEt / et );
			    if ( etaCluster < 3.3 )
				m_fmax_IW->Fill ( maximumEt / et );
			    else
				m_fmax_FCAL->Fill ( maximumEt / et );
			}
		}
		// Builds cluster
		xAOD::TrigEMCluster* cl = new xAOD::TrigEMCluster();
		clusters->push_back( cl );
                for(unsigned int i=0;i<(unsigned int)CaloSampling::CaloSample::Unknown;i++)
			cl->setEnergy((CaloSampling::CaloSample)i,0.0);
		// Set cluster features
		cl->setEnergy(et * TMath::CosH(cellAbove->eta()) );
		cl->setEt(et);
		cl->setEta( cellAbove->eta() );
		cl->setPhi( cellAbove->phi() );
		cl->setE237 ( zClusterN );
		cl->setE277 ( zClusterNE );
		cl->setEhad1 ( clusterHadEnergy );
		cl->setE233( clusterTime );
		cl->setEmaxs1 (  maximumEt );
		// energy in each layer
		float wstot=0.;
		float wstot_nor=0.;
		for( auto cellAround : m_cellsAround ) {
			unsigned int layer = cellAround->caloDDE()->getSampling();
			cl->setEnergy( (CaloSampling::CaloSample)layer, cl->energy( (CaloSampling::CaloSample)layer )+cellAround->energy() );
			if ( (layer==1) || (layer==5) ) {
			  if ( cellAround->et() < 10 ) continue;
			  wstot+=(cellAround->et()*fabsf(cellAround->eta()-etaCluster));
			  wstot_nor+=(cellAround->et());
			}
		}// Loop over cells
		if ( fabsf(wstot_nor)> 0.01 ) wstot/=wstot_nor;
		cl->setWstot( wstot );
		//std::cout << cl->et() << " " << cl->eta() << " " << cl->phi() << std::endl;
                //for(int ii=0;ii< 25 ; ii++) std::cout << ii << " " << cl->energy( (CaloSampling::CaloSample) ii) << "; ";
                //std::cout << std::endl;

		
	}
	return StatusCode::SUCCESS;
}

