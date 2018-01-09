/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * NAME : 	TrigT1CaloEFex.cxx
 * PACKAGE : 	Trigger/L1CaloUpgrade/TrigT1CaloEFex
 *
 * AUTHOR : 	Denis Oliveira Damazio
 *
 *
 * **/

#include "TrigT1CaloBaseFex.h"
#include "TrigT1CaloEFex.h"
#include "CaloEvent/CaloCellContainer.h"
#include <math.h>
#include <string>
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"

TrigT1CaloEFex::TrigT1CaloEFex( const std::string& name, ISvcLocator* pSvcLocator ) : TrigT1CaloBaseFex (name, pSvcLocator) {
       declareProperty("EnableMonitoring", m_enableMon=false);
       declareProperty("OutputClusterName", m_outputClusterName  = "SCluster" );
       declareProperty("TimeThreshold", m_timeThr = 200 );
}

TrigT1CaloEFex::~TrigT1CaloEFex(){
	// finish base class
}

StatusCode TrigT1CaloEFex::initialize(){
	
	if ( TrigT1CaloBaseFex::initialize().isFailure() ) return StatusCode::FAILURE;
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "initializing TrigT1CaloEFex" << endmsg;
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
	}
	return StatusCode::SUCCESS;
}

StatusCode TrigT1CaloEFex::finalize(){
	if ( TrigT1CaloBaseFex::finalize().isFailure() ) return StatusCode::FAILURE;
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "finalizing TrigT1CaloEFex" << endmsg;
	if ( m_enableMon ) {
		m_histFile->Write();
		m_histFile->Close();
	}
	return StatusCode::SUCCESS;
}

StatusCode TrigT1CaloEFex::execute(){
	
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "execute TrigT1CaloEFex" << endmsg;

        std::vector<const CaloCell*> scells;
	const xAOD::TriggerTowerContainer* TTs(0);
	if ( getContainers(scells, TTs).isFailure() || (TTs==0) ) {
		msg << MSG::WARNING << " Could not get containers" << endmsg;
		return StatusCode::SUCCESS;
	}

	// Simply check cells which are above a given threshold (3GeV)
	findCellsAbove(scells,3e3,m_cellsAboveThr);
	// Prepare output containers (with all xAOD annoying features)
	xAOD::TrigEMClusterContainer* clusters = new xAOD::TrigEMClusterContainer();
	xAOD::TrigEMClusterAuxContainer* auxclusters = new xAOD::TrigEMClusterAuxContainer();
	clusters->setStore(auxclusters);
	std::string clusterName(m_outputClusterName);
	if ( evtStore()->record(clusters,clusterName).isFailure() ){
		msg << MSG::ERROR  << "recording was not possible" << endmsg;
		return StatusCode::FAILURE;
	}
	if ( evtStore()->record(auxclusters,clusterName+"Aux.").isFailure() ){
		msg << MSG::ERROR << "recording Aux was not possible" << endmsg;
		return StatusCode::FAILURE;
	}
	// Loop over seed cells, this should give us
	for( auto cellAbove : m_cellsAboveThr ) {
		// builds a vector with all the cells around the seed cell
		// with the size (deta,dphi)=(0.08,0.21)
		findCellsAround(scells, cellAbove, m_cellsAround,0.08,0.21); // large window
		float etaCluster, phiCluster; // Cluster baricenter
		// Find cluster center (eta/phiCluster) based on the
		// energy weighted scell position
		findCluster(m_cellsAround, etaCluster, phiCluster);
		// smaller cluster (closer to egamma standard)
		findCellsAround(scells, etaCluster, phiCluster, m_cellsAround,m_deta,m_dphi);
		float clusterTime=0;
		float clusterTimeWeight=0;
		for( auto cellAround : m_cellsAround ) {
			if ( cellAround->et() < m_timeThr ) continue;
			clusterTime+=cellAround->time()*cellAround->et();
			clusterTimeWeight+=cellAround->et();
		}
		if ( fabsf(clusterTimeWeight) > 0.1 )
		clusterTime /=clusterTimeWeight;
		else clusterTime = -999.99;
		msg << MSG::DEBUG << "CELL versus CLUSTER : " << cellAbove->eta() << " " << cellAbove->phi() << " " << etaCluster << " " << phiCluster << 
 " " << cellAbove->eta()-etaCluster << " " << cellAbove->phi()-phiCluster << endmsg;
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
		if ( clusterEmEnergy32 < 10 ) continue;
		// some histograms (we could only check the ones
		// in the EFex part)
		if ( m_enableMon ){
			m_EtSElectron->Fill(et/1e3);
			m_EtaSElectron->Fill( etaCluster );
			m_PhiSElectron->Fill( phiCluster );
			m_HadEtSElectron->Fill( clusterHadEnergy/1e3 );
			m_EtSElectronEta->Fill( cellAbove->eta(), et/1e3);
			m_HadEtSElectronEta->Fill( cellAbove->eta(), clusterHadEnergy/1e3);
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
		cl->setE237 ( clusterEmEnergy32 );
		cl->setE277 ( clusterEmEnergy72 );
		cl->setEhad1 ( clusterHadEnergy );
		cl->setEnergy( CaloSampling::FCAL2, clusterTime );
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
		
	}
	return StatusCode::SUCCESS;
}

