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
	m_SupercellMapTWR = new TH2F("SupercellMapTWR","Supercell map of TWR",98,-4.9,4.9,64,0,2*M_PI);

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
	delete acc_clusterET;
	delete acc_clusterIso;
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
	m_SupercellMapTWR->Reset();

	int currentSampling = 0;

	// fill energy in all TH2 histograms for supercell map
	for(auto scell : allSuperCells) {
	  currentSampling = scell->caloDDE()->getSampling();

	  // Store maps per layer
	  if (currentSampling==0 || currentSampling==4)
	    m_SupercellMapEM0->Fill(scell->eta(),TVector2::Phi_0_2pi(scell->phi()),scell->energy()/TMath::CosH(scell->eta()));
	  else if (currentSampling==1 || currentSampling==5)
	    m_SupercellMapEM1->Fill(scell->eta(),TVector2::Phi_0_2pi(scell->phi()),scell->energy()/TMath::CosH(scell->eta()));
	  else if (currentSampling==2 || currentSampling==6)
	    m_SupercellMapEM2->Fill(scell->eta(),TVector2::Phi_0_2pi(scell->phi()),scell->energy()/TMath::CosH(scell->eta()));
	  else if (currentSampling==3 || currentSampling==7)
	    m_SupercellMapEM3->Fill(scell->eta(),TVector2::Phi_0_2pi(scell->phi()),scell->energy()/TMath::CosH(scell->eta()));
	  else m_SupercellMapHAD->Fill(scell->eta(),TVector2::Phi_0_2pi(scell->phi()),scell->energy()/TMath::CosH(scell->eta()));
	  
	  // Store a map sum of all layers
	  
	  m_SupercellMapTWR->Fill(scell->eta(),TVector2::Phi_0_2pi(scell->phi()),scell->energy()/TMath::CosH(scell->eta()));
	}

	// Need to also loop over Run-I towers to get central region hadronic energy
	for(unsigned tt_hs=0 ; tt_hs<TTs->size(); tt_hs++){
	  const xAOD::TriggerTower * tt = TTs->at(tt_hs);
	  // Only use towers within 1.5, and in Tile

          if(tt->sampling()!=1 || fabs(tt->eta())>1.5) continue;
	  
	  // Conversion into ET
	  float cpET = tt->cpET()*500.; // EM energy scale: 1 unit corresponds to 500 MeV
	  if(cpET < 0.)
	    cpET = 0;

	  // Fill hadronic maps
	  m_SupercellMapHAD->Fill(tt->eta(), TVector2::Phi_0_2pi(tt->phi()), cpET);
	  m_SupercellMapTWR->Fill(tt->eta(), TVector2::Phi_0_2pi(tt->phi()), cpET);
	}
	
	// Find local maxima
	std::vector<TLorentzVector> m_localMaxima;

	// X is eta, Y is phi
	for(int i=0; i<m_SupercellMapTWR->GetNbinsX(); i++) {
	  for(int j=0; j<m_SupercellMapTWR->GetNbinsY(); j++) {
	    // Start by filtering out 'useless towers' (ie: anything less than a GeV)
	    double towerET = m_SupercellMapTWR->GetBinContent(i+1, j+1);
	    if(towerET < 1000.)
	      continue;
	    
	    // Check if the current tower has the largest ET in this 3x3 window
	    
	    // Need to be careful with wrap-around in phi, unfortunately.
	    std::vector<double> binsAbove;
	    std::vector<double> binsBelow;
	    
	    // Handle wrap-around in phi
	    int aboveInPhi = j+1;
	    if(j == m_SupercellMapTWR->GetNbinsY()-1)
	      aboveInPhi = 0;
	    int belowInPhi = j-1;
	    if(j == 0)
	      belowInPhi = m_SupercellMapTWR->GetNbinsY()-1;

	    // The convention here is arbitrary, but needs to be mirrored
	    // Take the cells in the next row in phi, and the one cell above in eta
	    binsAbove.push_back( m_SupercellMapTWR->GetBinContent(i, aboveInPhi+1) );
	    binsAbove.push_back( m_SupercellMapTWR->GetBinContent(i+1, aboveInPhi+1) );
	    binsAbove.push_back( m_SupercellMapTWR->GetBinContent(i+2, aboveInPhi+1) );
	    binsAbove.push_back( m_SupercellMapTWR->GetBinContent(i+2, j+1) );

	    // Inversely so for the bins below
	    binsBelow.push_back( m_SupercellMapTWR->GetBinContent(i, belowInPhi+1) );
	    binsBelow.push_back( m_SupercellMapTWR->GetBinContent(i+1, belowInPhi+1) );
	    binsBelow.push_back( m_SupercellMapTWR->GetBinContent(i+2, belowInPhi+1) );
	    binsBelow.push_back( m_SupercellMapTWR->GetBinContent(i, j+1) );
	    
	    bool isMax = true;

	    // Check if it is a local maximum
	    for(unsigned int k=0; k<binsAbove.size(); k++) {
				if(towerET < binsAbove[k])
		  		isMax = false;
			}
	    for(unsigned int k=0; k<binsBelow.size(); k++) {
				if(towerET <= binsBelow[k])
		  		isMax = false;
	  	}
	    
	    if(isMax)
	      {
		TLorentzVector myMaximum;
		myMaximum.SetPtEtaPhiM(towerET, m_SupercellMapTWR->GetXaxis()->GetBinCenter(i+1), m_SupercellMapTWR->GetYaxis()->GetBinCenter(j+1), 0);
		m_localMaxima.push_back(myMaximum);
	      }
	  }
	}	  
	

	// Now loop over local maxima, decide what to do
	for( auto myMaximum : m_localMaxima ) {
	  
	  // Check eta bounds
	  if(fabs(myMaximum.Eta()) > 2.5)
	    continue;
	  
	  // Cluster coordinates
	  int i=m_SupercellMapTWR->GetXaxis()->FindFixBin(myMaximum.Eta());
	  // Careful, ROOT conventions are -pi,pi
	  int j=m_SupercellMapTWR->GetYaxis()->FindFixBin(TVector2::Phi_0_2pi(myMaximum.Phi()));


	  // Prepare Phi Wrap-around
	  int aboveInPhi = j+1;
	  if(j == m_SupercellMapTWR->GetNbinsY())
	    aboveInPhi = 1;
	  int belowInPhi = j-1;
	  if(j == 1)
	    belowInPhi = m_SupercellMapTWR->GetNbinsY();


	  // Start calculating total energy
	  // Use Fixed 2x2 cluster, 4 possibilities
	  std::vector<double> allET;

	  double ET;	  
	  // Up and right
	  ET = m_SupercellMapTWR->GetBinContent(i, j);
	  ET += m_SupercellMapTWR->GetBinContent(i+1, j);
	  ET += m_SupercellMapTWR->GetBinContent(i, aboveInPhi);
	  ET += m_SupercellMapTWR->GetBinContent(i+1, aboveInPhi);
	  allET.push_back(ET);

	  // Up and left
	  ET = m_SupercellMapTWR->GetBinContent(i, j);
	  ET += m_SupercellMapTWR->GetBinContent(i-1, j);
	  ET += m_SupercellMapTWR->GetBinContent(i, aboveInPhi);
	  ET += m_SupercellMapTWR->GetBinContent(i-1, aboveInPhi);
	  allET.push_back(ET);
	  
	  // Down and left
	  ET = m_SupercellMapTWR->GetBinContent(i, j);
	  ET += m_SupercellMapTWR->GetBinContent(i-1, j);
	  ET += m_SupercellMapTWR->GetBinContent(i, belowInPhi);
	  ET += m_SupercellMapTWR->GetBinContent(i-1, belowInPhi);
	  allET.push_back(ET);

	  // Down and right
	  ET = m_SupercellMapTWR->GetBinContent(i, j);
	  ET += m_SupercellMapTWR->GetBinContent(i+1, j);
	  ET += m_SupercellMapTWR->GetBinContent(i, belowInPhi);
	  ET += m_SupercellMapTWR->GetBinContent(i+1, belowInPhi);
	  allET.push_back(ET);

	  // Code Oregon cluster later
	  // Will need to write a library of shape summation functions
	  // EM0: 1x2, 4 possibilities
	  // EM1: 5x2
	  // EM2: 5x2
	  // EM3: 3x2
	  // HAD: 3x2

	  // Pick largest resulting sum
	  double eFEXOldCluster = 0;
	  for(unsigned int k=0; k<allET.size(); k++)
	    {
	      if(allET.at(k) > eFEXOldCluster)
		eFEXOldCluster = allET.at(k);
	    }

	  // Calculate an EM2-based isolation
	  // Center in EM2, offset to the right in eta:
	  int em2i=m_SupercellMapEM2->GetXaxis()->FindFixBin(myMaximum.Eta()+0.05);
          // Careful, ROOT conventions are -pi,pi
          int em2j=m_SupercellMapEM2->GetYaxis()->FindFixBin(TVector2::Phi_0_2pi(myMaximum.Phi()));

	  float maximumET = m_SupercellMapEM2->GetBinContent(em2i, em2j);
	  int maximumCell = em2i;
	  
	  // Find maximum in EM2
	  for(int k=-2; k<2; k++)
	    {
	      float ETvalue = m_SupercellMapEM2->GetBinContent(em2i+k, em2j);
	      if(ETvalue > maximumET)
		{
		  maximumET = ETvalue;
		  maximumCell = em2i+k;
		}
	    } 
	  
	  // Find highest pT nearest cell, but we don't care which it is
	  // Note that we don't need to worry about phi wrap-around
	  float nextET = m_SupercellMapEM2->GetBinContent(maximumCell+1, em2j);
	  nextET = ((m_SupercellMapEM2->GetBinContent(maximumCell-1, em2j) > nextET) ? m_SupercellMapEM2->GetBinContent(maximumCell-1, em2j) : nextET);
	  nextET = ((m_SupercellMapEM2->GetBinContent(maximumCell, em2j+1) > nextET) ? m_SupercellMapEM2->GetBinContent(maximumCell, em2j+1) : nextET);
	  nextET = ((m_SupercellMapEM2->GetBinContent(maximumCell, em2j-1) > nextET) ? m_SupercellMapEM2->GetBinContent(maximumCell, em2j-1) : nextET);
	  
	  float numerator = maximumET + nextET;

	  // And now, run the full sum: avoid phi wrapping by converting back and forth
	  float denominator = 0;
	  float phicenter =  m_SupercellMapEM2->GetYaxis()->GetBinCenter(em2j);
	  for(int eta=-6; eta<6; eta++)
	    for(int phi=-1; phi<2; phi++)
	      denominator += m_SupercellMapEM2->GetBinContent(em2i+eta, m_SupercellMapEM2->GetYaxis()->FindFixBin(TVector2::Phi_0_2pi(phicenter+(phi*0.1))));
	  
	  // build cluster for tau
	  //xAOD::EmTauRoI_v2* clForTau = new xAOD::EmTauRoI_v2();
	  xAOD::EmTauRoI* clForTau = new xAOD::EmTauRoI();
	  clustersForTau->push_back( clForTau );
	  clForTau->setEta( myMaximum.Eta() );
	  clForTau->setPhi( myMaximum.Phi() );
	  
	  (*acc_clusterET)(*clForTau) = eFEXOldCluster;
	  (*acc_clusterIso)(*clForTau) = 0;
	  if(denominator > 0)
	    (*acc_clusterIso)(*clForTau) = numerator/denominator;
	  
	}
	// avoid memory leak
	if ( scells ) {scells->clear(); delete scells;}
	m_cellsAround.clear();
	m_cellsAround2.clear();
	m_TTsAround.clear();
	return StatusCode::SUCCESS;
}
