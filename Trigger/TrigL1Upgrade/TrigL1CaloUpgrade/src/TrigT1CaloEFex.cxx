/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * NAME : 	TrigT1CaloEFex.cxx
 * PACKAGE : 	Trigger/L1CaloUpgrade/TrigT1CaloEFex
 *
 * AUTHOR : 	Denis Oliveira Damazio
 * UPDATED :	Philipp Ott
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

TrigT1CaloEFex::TrigT1CaloEFex( const std::string& name, ISvcLocator* pSvcLocator ) : 
  TrigT1CaloBaseFex (name, pSvcLocator)
{
       declareProperty("EnableMonitoring", m_enableMon = false); 
       declareProperty("OutputClusterName", m_outputClusterName = "SCluster_TrigT1CaloEFex");
       declareProperty("TimeThreshold", m_timeThr = 200);
       declareProperty("SeedEnergyThreshold", m_seedE = 100);
       declareProperty("EnergyWeightedCluster", m_energyWeightedCluster = false);
       declareProperty("EtaCellFormation", m_deta_cellFormation = 0.08);
       declareProperty("PhiCellFormation", m_dphi_cellFormation = 0.21);
       declareProperty("EtaClusterFormation2", m_deta_clusterFormation_2 = 0.21);
       declareProperty("PhiClusterFormation2", m_dphi_clusterFormation_2 = 0.21);
       declareProperty("MinimumClusterEnergy", m_clusterE_EMB2_EMEC2 = 100);
       declareProperty("LimitClusterEta", m_eta_limit = 2.47);    
} 

TrigT1CaloEFex::~TrigT1CaloEFex(){
  // finish base class
}

StatusCode TrigT1CaloEFex::initialize(){
  
  // Initialize accessors
  Run3eFEXIsolation_REta       = new SG::AuxElement::Accessor<float>("Run3REta");
  Run3eFEXIsolation_RHad       = new SG::AuxElement::Accessor<float>("Run3RHad");
  pass_Run3eFEXIsolation_REta  = new SG::AuxElement::Accessor<int>("PassRun3REta");
  pass_Run3eFEXIsolation_RHad  = new SG::AuxElement::Accessor<int>("PassRun3RHad");
  pass_Run3eFEXIsolation_wstot = new SG::AuxElement::Accessor<int>("PassRun3wstot");
  pass_Run3eFEX_clusterEnergy  = new SG::AuxElement::Accessor<int>("PassRun3ClusterEnergy");
  

	if ( TrigT1CaloBaseFex::initialize().isFailure() ) return StatusCode::FAILURE;
        MsgStream msg(msgSvc(), name());
        if ( m_enableMon ){
		std::string histoName(name());
		histoName+="Algo.root";
		m_histFile = new TFile(histoName.c_str(),"RECREATE");
		m_EtSElectron = new TH1F("EtSElectron","Et of Super Cell based Electron",60,0,60);
		m_HadEtSElectron = new TH1F("HadEtSElectron","HadEt of Super Cell based Electron",60,0,6);
		m_EtaSElectron = new TH1F("EtaSElectron","Eta of Super Cell based Electron",50,-2.5,2.5);
		m_PhiSElectron = new TH1F("PhiSElectron","Phi of Super Cell based Electron",64,-3.2,3.2);
		m_EtSElectronEta = new TH2F("EtSElectronEta","Et of Super Cell based Electron versus Eta",50,-5.0,5.0,60,0,60);
		m_HadEtSElectronEta = new TH2F("HadEtSElectronEta","HadEt of Super Cell based Electron versus Eta",50,-5.0,5.0,60,0,60);
	}
	return StatusCode::SUCCESS;
}

StatusCode TrigT1CaloEFex::finalize(){
	if ( TrigT1CaloBaseFex::finalize().isFailure() ) return StatusCode::FAILURE;
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "finalizing TrigT1CaloEFex" << endreq;
	if ( m_enableMon ) {
		m_histFile->Write();
		m_histFile->Close();
	}
	return StatusCode::SUCCESS;
}

StatusCode TrigT1CaloEFex::execute(){
        MsgStream msg(msgSvc(), name());
	CaloCellContainer* scells(0);
	const xAOD::TriggerTowerContainer* TTs(0);
	if ( getContainers(scells, TTs).isFailure() || (TTs==0) ) {
		msg << MSG::WARNING << "Could not get containers" << endreq;
		return StatusCode::FAILURE;
	}
	// Prepare output containers
	xAOD::TrigEMClusterContainer* clusters = new xAOD::TrigEMClusterContainer();
	xAOD::TrigEMClusterAuxContainer* auxclusters = new xAOD::TrigEMClusterAuxContainer();
	clusters->setStore(auxclusters);
	clusters->reserve(100);
	std::string clusterName(m_outputClusterName);
	if ( evtStore()->record(clusters, clusterName).isFailure() ) {
		msg << MSG::ERROR  << "recording was not possible" << endreq;
		return StatusCode::FAILURE;
	}
	if ( evtStore()->record(auxclusters, clusterName+"Aux.").isFailure() ) {
		msg << MSG::ERROR << "recording Aux was not possible" << endreq;
		return StatusCode::FAILURE;
	}
	// Set m_energyWeightedCluster to true if an energy weighted cluster formation should be performed (default:false)
	if ( !m_energyWeightedCluster ) {
		const CaloCell_SuperCell_ID* m_idHelper = nullptr;
		if ( getIDHelper( m_idHelper ).isFailure() ) {
			msg << MSG::WARNING << "Could not get ID manager " << endreq;
			return StatusCode::FAILURE;
		}
		std::vector<std::vector<float>> clustering = baselineAlg(scells, TTs, m_idHelper);
		for (auto ithCluster : clustering){
		  if ( m_enableMon ) {
			  m_EtaSElectron->Fill( ithCluster[0] );
			  m_PhiSElectron->Fill( ithCluster[1] );
			  m_EtSElectron->Fill( ithCluster[2] );
		          m_EtSElectronEta->Fill( ithCluster[0], ithCluster[2] );
			  if (ithCluster[6] > -999) {
				  m_HadEtSElectron->Fill( ithCluster[6] );
				  m_HadEtSElectronEta->Fill( ithCluster[0], ithCluster[6] );
			  }
		  }
		  xAOD::TrigEMCluster* cl = new xAOD::TrigEMCluster();
		  clusters->push_back( cl );

		  cl->setEta( ithCluster[0] );
		  cl->setPhi( ithCluster[1] );
		  cl->setEt( ithCluster[2] );
		  (*Run3eFEXIsolation_REta)(*cl) = ithCluster[3];
		  (*Run3eFEXIsolation_RHad)(*cl) = ithCluster[4];
		  cl->setWstot( ithCluster[5] );
		  if (ithCluster[6] > -999) cl->setEhad1( ithCluster[6] );
		  cl->setE233( ithCluster[7] );
		  cl->setE237( ithCluster[8] );
		  // check if cluster fulfills all selection criteria, trivial if m_apply_BaseLineCuts = true
		  if ( m_apply_BaseLineCuts ){
			  (*pass_Run3eFEXIsolation_REta)(*cl)  = 1;
			  (*pass_Run3eFEXIsolation_RHad)(*cl)  = 1;
			  (*pass_Run3eFEXIsolation_wstot)(*cl) = 1;
			  (*pass_Run3eFEX_clusterEnergy)(*cl)  = 1;
		  }
		  else {
			  (*pass_Run3eFEXIsolation_REta)(*cl)  = 0;
			  (*pass_Run3eFEXIsolation_RHad)(*cl)  = 0;
			  (*pass_Run3eFEXIsolation_wstot)(*cl) = 0;
			  (*pass_Run3eFEX_clusterEnergy)(*cl)  = 0;
			  if ( ithCluster[2] >= m_clustET_thresh && ithCluster[2] <= m_clustET_NoIso_thresh ) (*pass_Run3eFEX_clusterEnergy)(*cl) = 1;
			  else if ( ithCluster[2] > m_clustET_NoIso_thresh ){
				  (*pass_Run3eFEXIsolation_REta)(*cl)  = 1;
				  (*pass_Run3eFEXIsolation_RHad)(*cl)  = 1;
				  (*pass_Run3eFEXIsolation_wstot)(*cl) = 1;
				  (*pass_Run3eFEX_clusterEnergy)(*cl)  = 1;
				  continue;
			  }
			  if ( ithCluster[3] <= m_REta_thresh ) (*pass_Run3eFEXIsolation_REta)(*cl) = 1;
			  if ( ithCluster[4] <= m_RHad_thresh ) (*pass_Run3eFEXIsolation_RHad)(*cl) = 1;
			  if ( fabs(ithCluster[0]) > m_eta_dropL1Width ) (*pass_Run3eFEXIsolation_wstot)(*cl) = 1;
			  else {
				  if ( ithCluster[5] < m_L1Width_thresh ) (*pass_Run3eFEXIsolation_wstot)(*cl) = 1;
			  }
		  }
		}
	}
	else {
	        /////////////////////////////////////////////////////////////////
	        // Note that there are several additional differences 
	        // on top of the different cluster formation:
	        //   -- The energy of the cluster is not given as multiples
	        //      of the digitization scale (25 MeV)
	        //   -- The cluster sizes differ per default (but can be adjusted)
	        //   -- The definition of the lateral isolation wstot differs
	        //   -- The isolation variables REta, RHad are not defined
	        /////////////////////////////////////////////////////////////////

		// Check cells which are above a user defined threshold (default: 100MeV) in EMB2 or EMEC2
		findCellsAbove_EMB2_EMEC2(scells, m_seedE, m_cellsAboveThr);
	        // Loop over possible seed cells
	        for ( auto cellAbove : m_cellsAboveThr ) {
	          float etaCluster, phiCluster; 
		  // Builds a vector with all the cells around the seed cell with the default size (deta,dphi)=(0.08,0.21)
		  findCellsAround(scells, cellAbove, m_cellsAround, m_deta_cellFormation, m_dphi_cellFormation);
		  // Find cluster center (eta/phiCluster) based on the energy weighted scell position
		  findCluster(m_cellsAround, etaCluster, phiCluster);
	          if ( fabsf(etaCluster) > 998.0 ) continue;
		  // Smaller cluster (closer to egamma standard)
		  findCellsAround(scells, etaCluster, phiCluster, m_cellsAround, m_deta, m_dphi);
	          // SElectron eta should be within the kinematic acceptance, default: Run 2 recommendations
	          if ( fabsf(etaCluster) >= m_eta_limit ) continue;
	          if ( ! isCellEmMaximum ( m_cellsAround, cellAbove ) ) continue;
	          float clusterTime=0;
	          float clusterTimeWeight=0;
	          for( auto cellAround : m_cellsAround ) {
	            if ( cellAround->et() < m_timeThr ) continue;
		    clusterTime+=cellAround->time()*cellAround->et();
		    clusterTimeWeight+=cellAround->et();
	          }
	          if ( fabsf(clusterTimeWeight) > 0.1 ) clusterTime /= clusterTimeWeight;
	          else clusterTime = -999.99;
	          msg << MSG::DEBUG << "CELL versus CLUSTER : " << cellAbove->eta() << " " << cellAbove->phi() << " " << etaCluster << " " << phiCluster << " " << cellAbove->eta()-etaCluster << " " << cellAbove->phi()-phiCluster << endreq;
	          // Other cluster sizes for some of the shower shapes
	          findCellsAround(scells, (const float)etaCluster, (const float)phiCluster, m_cellsAround2, m_deta_clusterFormation_2, m_dphi_clusterFormation_2);
	          // Include TT (for Tile region only)
	          findTTsAround(TTs, etaCluster, phiCluster, m_TTsAround);
	          float et = sumEmCells( m_cellsAround )/TMath::CosH(cellAbove->eta());
	          float clusterEmEnergy32 = sumEmCells2nd( m_cellsAround2 );
	          float clusterEmEnergy72 = sumEmCells2nd( m_cellsAround );
	          float clusterHadEnergy = sumHadCells( m_cellsAround );
	          clusterHadEnergy += sumHadTTs( m_TTsAround );
	          if ( clusterEmEnergy32 < m_clusterE_EMB2_EMEC2 ) continue;
	          if ( m_enableMon ) {
			  m_EtSElectron->Fill( et/1e3 );
		          m_EtaSElectron->Fill( etaCluster );
		          m_PhiSElectron->Fill( phiCluster );
		          m_HadEtSElectron->Fill( clusterHadEnergy/1e3 );
		          m_EtSElectronEta->Fill( cellAbove->eta(), et/1e3 );
		          m_HadEtSElectronEta->Fill( cellAbove->eta(), clusterHadEnergy/1e3 );
	          }
	          xAOD::TrigEMCluster* cl = new xAOD::TrigEMCluster();
	          clusters->push_back( cl );
                  for ( unsigned int i=0; i<(unsigned int)CaloSampling::CaloSample::Unknown; i++ ) {
	            cl->setEnergy((CaloSampling::CaloSample)i,0.0);
		  }
	          cl->setEnergy( et * TMath::CosH(cellAbove->eta()) );
	          cl->setEt( et );
	          cl->setEta( cellAbove->eta() );
	          cl->setPhi( cellAbove->phi() );
	          cl->setE237 ( clusterEmEnergy32 );
	          cl->setE277 ( clusterEmEnergy72 );
	          cl->setEhad1 ( clusterHadEnergy );
	          cl->setE233( clusterTime );
	          float wstot=0.;
	          float wstot_nor=0.;
	          for( auto cellAround : m_cellsAround ) {
		    unsigned int layer = cellAround->caloDDE()->getSampling();
		    cl->setEnergy( (CaloSampling::CaloSample)layer, cl->energy( (CaloSampling::CaloSample)layer )+cellAround->energy() );
		    if ( (layer==1) || (layer==5) ) {
			    if ( cellAround->et() < 10 ) continue;
			    wstot+=(cellAround->et()*pow(cellAround->eta()-etaCluster, 2));
			    wstot_nor+=(cellAround->et());
		    }
	          }
	          if ( fabsf(wstot_nor) > 0.01 ) wstot = sqrt(wstot/wstot_nor);
	          cl->setWstot( wstot );
	        }
	        m_cellsAround.clear();
	        m_cellsAround2.clear();
	        m_TTsAround.clear();
	}
	if ( scells ) {scells->clear(); delete scells;}
	return StatusCode::SUCCESS;
}

