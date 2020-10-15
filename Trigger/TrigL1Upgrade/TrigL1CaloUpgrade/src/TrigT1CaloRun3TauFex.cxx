/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
	ATH_MSG_DEBUG( "initializing TrigT1CaloRun3TauFex");
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
	m_SupercellMapEM1_coarse = new TH2F("SupercellMapEM1_coarse","Supercell map of EM1 coarse",196,-4.9,4.9,64,0,2*M_PI);
	m_SupercellMapEM2_coarse = new TH2F("SupercellMapEM2_coarse","Supercell map of EM2 coarse",196,-4.9,4.9,64,0,2*M_PI);
	m_SupercellMapEM3 = new TH2F("SupercellMapEM3","Supercell map of EM3",98,-4.9,4.9,64,0,2*M_PI);
	m_SupercellMapHAD = new TH2F("SupercellMapHAD","Supercell map of HAD",98,-4.9,4.9,64,0,2*M_PI);
	m_SupercellMapTWR = new TH2F("SupercellMapTWR","Supercell map of TWR",98,-4.9,4.9,64,0,2*M_PI);

	// Hard-code variable names - we don't want them to be configurable for now
	// Feel free to change the names
	// Also, note that you should not use the 'ConstAccessor' as this one can't write to the object
	acc_clusterET = new SG::AuxElement::Accessor<float>("R3ClusterET");
	acc_clusterIso = new SG::AuxElement::Accessor<float>("R3ClusterIso");
	acc_Ore_clusterET = new SG::AuxElement::Accessor<float>("R3_Ore_ClusterET");
	acc_Ore_clusterIso = new SG::AuxElement::Accessor<float>("R3_Ore_ClusterIso");
	acc_Ore_clusterIso_12pass = new SG::AuxElement::Accessor<bool>("R3_Ore_ClusterIso_12pass");
	acc_Ore_clusterIso_20pass = new SG::AuxElement::Accessor<bool>("R3_Ore_ClusterIso_20pass");
	acc_BC_clusterET = new SG::AuxElement::Accessor<float>("R3_BC_ClusterET");
	acc_BC_clusterIso = new SG::AuxElement::Accessor<float>("R3_BC_ClusterIso");
	acc_BC_clusterIso_12pass = new SG::AuxElement::Accessor<bool>("R3_BC_ClusterIso_12pass");
	acc_BC_clusterIso_20pass = new SG::AuxElement::Accessor<bool>("R3_BC_ClusterIso_20pass");
	return StatusCode::SUCCESS;
}

StatusCode TrigT1CaloRun3TauFex::finalize(){
	if ( TrigT1CaloBaseFex::finalize().isFailure() ) return StatusCode::FAILURE;
	ATH_MSG_DEBUG( "finalizing TrigT1CaloRun3TauFex");
	if ( m_enableMon ) {
		m_histFile->Write();
		m_histFile->Close();
	}
	delete acc_clusterET;
	delete acc_clusterIso;
	delete acc_Ore_clusterET;
	delete acc_Ore_clusterIso;
	delete acc_Ore_clusterIso_12pass;
	delete acc_Ore_clusterIso_20pass;
	delete acc_BC_clusterET;
	delete acc_BC_clusterIso;
	delete acc_BC_clusterIso_12pass;
	delete acc_BC_clusterIso_20pass;
	return StatusCode::SUCCESS;
}

StatusCode TrigT1CaloRun3TauFex::execute(){
#ifndef NDEBUG
	ATH_MSG_DEBUG( "execute TrigT1CaloRun3TauFex");
#endif

	CaloCellContainer* scells(0);
	const xAOD::TriggerTowerContainer* TTs(0);
	if ( getContainers(scells, TTs).isFailure() || (TTs==0) ) {
		ATH_MSG_WARNING( " Could not get containers");
		return StatusCode::SUCCESS;
	}

	// Prepare output containers (with all xAOD annoying features)
	xAOD::EmTauRoIContainer* clustersForTau = new xAOD::EmTauRoIContainer();
	xAOD::EmTauRoIAuxContainer* auxclustersForTau = new xAOD::EmTauRoIAuxContainer();
	clustersForTau->setStore(auxclustersForTau);
	clustersForTau->reserve(100);
	std::string clusterName(m_outputClusterName);
	if ( evtStore()->record(clustersForTau,clusterName).isFailure() ){
		ATH_MSG_ERROR( "recording was not possible");
		return StatusCode::FAILURE;
	}
	if ( evtStore()->record(auxclustersForTau,clusterName+"Aux.").isFailure() ){
		ATH_MSG_ERROR( "recording Aux was not possible");
		return StatusCode::FAILURE;
	}

	// prepare all supercells vector in whole ATLAS detector
	createCellList(scells,allSuperCells);

	// clear all TH2 histograms for supercell map
	m_SupercellMapEM0->Reset();
	m_SupercellMapEM1->Reset();
	m_SupercellMapEM2->Reset();
	m_SupercellMapEM1_coarse->Reset();
	m_SupercellMapEM2_coarse->Reset();
	m_SupercellMapEM3->Reset();
	m_SupercellMapHAD->Reset();
	m_SupercellMapTWR->Reset();

	int currentSampling = 0;
	float currentEta = 0;
	float currentPhi = 0;
	float currentCellEt = 0;

	// fill energy in all TH2 histograms for supercell map
	for(auto scell : allSuperCells) {
	  currentSampling = scell->caloDDE()->getSampling();
	  currentEta = scell->eta();
	  currentPhi = TVector2::Phi_0_2pi(scell->phi());
	  currentCellEt = TrigT1CaloBaseFex::CaloCellET(scell, m_nominalDigitization, m_nominalNoise_thresh);

	  // Store maps per layer
	  if (currentSampling==0 || currentSampling==4)
	    m_SupercellMapEM0->Fill(currentEta,currentPhi,currentCellEt);
	  else if (currentSampling==1 || currentSampling==5){
	    m_SupercellMapEM1->Fill(currentEta,currentPhi,currentCellEt);
	    m_SupercellMapEM1_coarse->Fill(currentEta,currentPhi,currentCellEt);
	  }
	  else if (currentSampling==2 || currentSampling==6){
	    m_SupercellMapEM2->Fill(currentEta,currentPhi,currentCellEt);
	    m_SupercellMapEM2_coarse->Fill(currentEta,currentPhi,currentCellEt);
	  }
	  else if (currentSampling==3 || currentSampling==7)
	    m_SupercellMapEM3->Fill(currentEta,currentPhi,currentCellEt);
	  else m_SupercellMapHAD->Fill(currentEta,currentPhi,currentCellEt);
	  
	  // Store a map sum of all layers
	  
	  m_SupercellMapTWR->Fill(currentEta,currentPhi,currentCellEt);
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
	m_localMaxima.reserve(200);

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
	    binsAbove.reserve(4);
	    std::vector<double> binsBelow;
	    binsBelow.reserve(4);
	    
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
	

	if (msgLvl(MSG::DEBUG)){
	  for (int i=1;i<m_SupercellMapEM1_coarse->GetNbinsX()+1;i++){
	    for (int j=1;j<m_SupercellMapEM1_coarse->GetNbinsY()+1;j++){
	      ATH_MSG_DEBUG("m_SupercellMapEM1_coarse->GetBinContent(" << i<<","<<j<<") "<< m_SupercellMapEM1_coarse->GetBinContent(i, j) );
	      ATH_MSG_DEBUG("m_SupercellMapEM2_coarse->GetBinContent(" << i<<","<<j<<") "<< m_SupercellMapEM2_coarse->GetBinContent(i, j) );
	      ATH_MSG_DEBUG("m_SupercellMapEM2_coarse->GetXaxis()->GetBinCenter("<< i<<") "<< m_SupercellMapEM2_coarse->GetXaxis()->GetBinCenter(i) );
	      ATH_MSG_DEBUG("m_SupercellMapEM2_coarse->GetYaxis()->GetBinCenter("<< j<<") "<< m_SupercellMapEM2_coarse->GetYaxis()->GetBinCenter(j) );
	      }
	    }
	  }

	// Now loop over local maxima, decide what to do
	for( auto myMaximum : m_localMaxima ) {
	  
	  // Check eta bounds
	  if(fabs(myMaximum.Eta()) > 2.5)
	    continue;
	  
	  // Cluster coordinates
	  // Get eta coordinate for coarse layers EM0, EM3, HAD, and TWR
	  int i=m_SupercellMapTWR->GetXaxis()->FindFixBin(myMaximum.Eta());
	  // Careful, ROOT conventions are -pi,pi
	  int j=m_SupercellMapTWR->GetYaxis()->FindFixBin(TVector2::Phi_0_2pi(myMaximum.Phi()));
	  // Get eta coordinate for fine layers EM1 and EM2
	  int i_fine_start=((i - 1) * 4) + 1;
	  int i_offset = 0;
	  double i_max = 0;
	  for(unsigned int i_off_cand=0; i_off_cand<4; i_off_cand++){
	    int i_et = m_SupercellMapEM2->GetBinContent(i_fine_start + i_off_cand, j);
	    if(i_et > i_max) {
	      i_max = i_et;
	      i_offset = i_off_cand;
	      }
	  }
	  int i_fine = i_fine_start + i_offset;

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
	  allET.reserve(4);

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

	  // Pick largest resulting sum
	  double eFEXOldCluster = 0;
	  for(unsigned int k=0; k<allET.size(); k++)
	    {
	      if(allET.at(k) > eFEXOldCluster)
		eFEXOldCluster = allET.at(k);
	    }
	  
	  // Calculate Oregon algorithm reconstructed ET 
	  // Determine if the Oregon shapes, which are asymmetric in phi, should be oriented up or down
	  bool sumAboveInPhi = true;
	  
	  // Sum the cells above in phi for EM1 and EM2
	  double abovePhiCellET = 0;
	  abovePhiCellET += m_SupercellMapEM2->GetBinContent(i_fine, aboveInPhi);
	  // Sum the cells below in phi for EM1 and EM2
	  double belowPhiCellET = 0;
	  belowPhiCellET += m_SupercellMapEM2->GetBinContent(i_fine, belowInPhi);
	  
	  // If more energy deposited below in phi, then orient shapes downward
	  if(belowPhiCellET > abovePhiCellET)
	    sumAboveInPhi = false;
	  
	  // Hold the coordinate of the non-central phi row
	  int offPhiCoordinate = 0;
	  if(sumAboveInPhi)
	    offPhiCoordinate = aboveInPhi;
	  else
	    offPhiCoordinate = belowInPhi;
	  
	  // Construct 3x2 EM0 Oregon layer energy
	  double em0OregonET = 0;
	  em0OregonET += m_SupercellMapEM0->GetBinContent(i, j);
	  em0OregonET += m_SupercellMapEM0->GetBinContent(i - 1, j);
	  em0OregonET += m_SupercellMapEM0->GetBinContent(i + 1, j);
	  em0OregonET += m_SupercellMapEM0->GetBinContent(i, offPhiCoordinate);
	  em0OregonET += m_SupercellMapEM0->GetBinContent(i - 1, offPhiCoordinate);
	  em0OregonET += m_SupercellMapEM0->GetBinContent(i + 1, offPhiCoordinate);
	  
	  // Construct 5x2 EM1 Oregon layer energy
	  double em1OregonET = 0;
	  em1OregonET += m_SupercellMapEM1->GetBinContent(i_fine, j);
	  em1OregonET += m_SupercellMapEM1->GetBinContent(i_fine - 1, j);
	  em1OregonET += m_SupercellMapEM1->GetBinContent(i_fine - 2, j);
	  em1OregonET += m_SupercellMapEM1->GetBinContent(i_fine + 1, j);
	  em1OregonET += m_SupercellMapEM1->GetBinContent(i_fine + 2, j);
	  em1OregonET += m_SupercellMapEM1->GetBinContent(i_fine, offPhiCoordinate);
	  em1OregonET += m_SupercellMapEM1->GetBinContent(i_fine - 1, offPhiCoordinate);
	  em1OregonET += m_SupercellMapEM1->GetBinContent(i_fine - 2, offPhiCoordinate);
	  em1OregonET += m_SupercellMapEM1->GetBinContent(i_fine + 1, offPhiCoordinate);
	  em1OregonET += m_SupercellMapEM1->GetBinContent(i_fine + 2, offPhiCoordinate);
	  
	  // Construct 5x2 EM2 Oregon layer energy
	  double em2OregonET = 0;
	  em2OregonET += m_SupercellMapEM2->GetBinContent(i_fine, j);
	  em2OregonET += m_SupercellMapEM2->GetBinContent(i_fine - 1, j);
	  em2OregonET += m_SupercellMapEM2->GetBinContent(i_fine - 2, j);
	  em2OregonET += m_SupercellMapEM2->GetBinContent(i_fine + 1, j);
	  em2OregonET += m_SupercellMapEM2->GetBinContent(i_fine + 2, j);
	  em2OregonET += m_SupercellMapEM2->GetBinContent(i_fine, offPhiCoordinate);
	  em2OregonET += m_SupercellMapEM2->GetBinContent(i_fine - 1, offPhiCoordinate);
	  em2OregonET += m_SupercellMapEM2->GetBinContent(i_fine - 2, offPhiCoordinate);
	  em2OregonET += m_SupercellMapEM2->GetBinContent(i_fine + 1, offPhiCoordinate);
	  em2OregonET += m_SupercellMapEM2->GetBinContent(i_fine + 2, offPhiCoordinate);
	  
	  // Construct 3x2 EM3 Oregon layer energy
	  double em3OregonET = 0;
	  em3OregonET += m_SupercellMapEM3->GetBinContent(i, j);
	  em3OregonET += m_SupercellMapEM3->GetBinContent(i - 1, j);
	  em3OregonET += m_SupercellMapEM3->GetBinContent(i + 1, j);
	  em3OregonET += m_SupercellMapEM3->GetBinContent(i, offPhiCoordinate);
	  em3OregonET += m_SupercellMapEM3->GetBinContent(i - 1, offPhiCoordinate);
	  em3OregonET += m_SupercellMapEM3->GetBinContent(i + 1, offPhiCoordinate);
	  
	  // Construct 3x2 HAD Oregon layer energy
	  double hadOregonET = 0;
	  hadOregonET += m_SupercellMapHAD->GetBinContent(i, j);
	  hadOregonET += m_SupercellMapHAD->GetBinContent(i - 1, j);
	  hadOregonET += m_SupercellMapHAD->GetBinContent(i + 1, j);
	  hadOregonET += m_SupercellMapHAD->GetBinContent(i, offPhiCoordinate);
	  hadOregonET += m_SupercellMapHAD->GetBinContent(i - 1, offPhiCoordinate);
	  hadOregonET += m_SupercellMapHAD->GetBinContent(i + 1, offPhiCoordinate);
	  
	  // Add individual layer energies to get the full Oregon reconstructed energy
	  double eFEX_OregonET = em0OregonET + em1OregonET + em2OregonET + em3OregonET + hadOregonET; 
	  
	  // Calculate Oregon algorithm isolation value
	  // Construct inner 3x2 energy
	  double oreIsoInnerET = 0;
	  oreIsoInnerET += m_SupercellMapEM2->GetBinContent(i_fine, j);
	  oreIsoInnerET += m_SupercellMapEM2->GetBinContent(i_fine - 1, j);
	  oreIsoInnerET += m_SupercellMapEM2->GetBinContent(i_fine + 1, j);
	  oreIsoInnerET += m_SupercellMapEM2->GetBinContent(i_fine, offPhiCoordinate);
	  oreIsoInnerET += m_SupercellMapEM2->GetBinContent(i_fine - 1, offPhiCoordinate);
	  oreIsoInnerET += m_SupercellMapEM2->GetBinContent(i_fine + 1, offPhiCoordinate);
	 
	  // Construct outer 9x2 energy using inner sum plus extra cells
	  double oreIsoOuterET = oreIsoInnerET;
	  oreIsoOuterET += m_SupercellMapEM2->GetBinContent(i_fine - 2, j);
	  oreIsoOuterET += m_SupercellMapEM2->GetBinContent(i_fine - 3, j);
	  oreIsoOuterET += m_SupercellMapEM2->GetBinContent(i_fine - 4, j);
	  oreIsoOuterET += m_SupercellMapEM2->GetBinContent(i_fine + 2, j);
	  oreIsoOuterET += m_SupercellMapEM2->GetBinContent(i_fine + 3, j);
	  oreIsoOuterET += m_SupercellMapEM2->GetBinContent(i_fine + 4, j);
	  oreIsoOuterET += m_SupercellMapEM2->GetBinContent(i_fine - 2, offPhiCoordinate);
	  oreIsoOuterET += m_SupercellMapEM2->GetBinContent(i_fine - 3, offPhiCoordinate);
	  oreIsoOuterET += m_SupercellMapEM2->GetBinContent(i_fine - 4, offPhiCoordinate);
	  oreIsoOuterET += m_SupercellMapEM2->GetBinContent(i_fine + 2, offPhiCoordinate);
	  oreIsoOuterET += m_SupercellMapEM2->GetBinContent(i_fine + 3, offPhiCoordinate);
	  oreIsoOuterET += m_SupercellMapEM2->GetBinContent(i_fine + 4, offPhiCoordinate);
	  
	  // Calculate isolation value as the ratio of inner over outer energies
	  double eFEX_OregonIso = oreIsoInnerET / oreIsoOuterET;
	  
	  // Calculation of isolation cut values discussed here https://indico.cern.ch/event/867020/contributions/3726146/attachments/2003208/3344698/L1CALOJoint03132020.pdf

	  // Set boolean for whether event passes 12 GeV isolation cut, hardcoding isolation threshold for now
	  bool eFEX_OregonIso_12pass = true;
	  if (10000. < eFEX_OregonET && 15000. > eFEX_OregonET && eFEX_OregonIso < 0.69)
	    eFEX_OregonIso_12pass = false;

	  // Set boolean for whether event passes 20 GeV isolation cut, hardcoding isolation threshold for now
	  bool eFEX_OregonIso_20pass = true;
	  if (20000. < eFEX_OregonET && 25000. > eFEX_OregonET && eFEX_OregonIso < 0.61)
	    eFEX_OregonIso_20pass = false;

	  // Code Oregon cluster later
	  // Will need to write a library of shape summation functions
	  // EM0: 1x2, 4 possibilities
	  // EM1: 5x2
	  // EM2: 5x2
	  // EM3: 3x2
	  // HAD: 3x2

	  //Code TLV bigCluster algorithm

	  //make vectors with EM0/3/HAD energies in each of 3x3 bins
	  //Make a function since this is disgusting but vim does it very fast
	  std::vector<double> E_EM0;
	  E_EM0.reserve(9);
	  E_EM0.push_back(m_SupercellMapEM0->GetBinContent(i, j)              );
	  E_EM0.push_back(m_SupercellMapEM0->GetBinContent(i-1, j)            );
	  E_EM0.push_back(m_SupercellMapEM0->GetBinContent(i-1, aboveInPhi)   );
	  E_EM0.push_back(m_SupercellMapEM0->GetBinContent(i, aboveInPhi)     );
	  E_EM0.push_back(m_SupercellMapEM0->GetBinContent(i+1, aboveInPhi)   );
	  E_EM0.push_back(m_SupercellMapEM0->GetBinContent(i+1, j)            );
	  E_EM0.push_back(m_SupercellMapEM0->GetBinContent(i+1, belowInPhi)   );
	  E_EM0.push_back(m_SupercellMapEM0->GetBinContent(i, belowInPhi)     );
	  E_EM0.push_back(m_SupercellMapEM0->GetBinContent(i-1, belowInPhi)   );
	  std::vector<double> E_EM3;
	  E_EM3.reserve(9);
	  E_EM3.push_back(m_SupercellMapEM3->GetBinContent(i, j)              );
	  E_EM3.push_back(m_SupercellMapEM3->GetBinContent(i-1, j)            );
	  E_EM3.push_back(m_SupercellMapEM3->GetBinContent(i-1, aboveInPhi)   );
	  E_EM3.push_back(m_SupercellMapEM3->GetBinContent(i, aboveInPhi)     );
	  E_EM3.push_back(m_SupercellMapEM3->GetBinContent(i+1, aboveInPhi)   );
	  E_EM3.push_back(m_SupercellMapEM3->GetBinContent(i+1, j)            );
	  E_EM3.push_back(m_SupercellMapEM3->GetBinContent(i+1, belowInPhi)   );
	  E_EM3.push_back(m_SupercellMapEM3->GetBinContent(i, belowInPhi)     );
	  E_EM3.push_back(m_SupercellMapEM3->GetBinContent(i-1, belowInPhi)   );
	  std::vector<double> E_HAD;
	  E_HAD.reserve(9);
	  E_HAD.push_back(m_SupercellMapHAD->GetBinContent(i, j)              );
	  E_HAD.push_back(m_SupercellMapHAD->GetBinContent(i-1, j)            );
	  E_HAD.push_back(m_SupercellMapHAD->GetBinContent(i-1, aboveInPhi)   );
	  E_HAD.push_back(m_SupercellMapHAD->GetBinContent(i, aboveInPhi)     );
	  E_HAD.push_back(m_SupercellMapHAD->GetBinContent(i+1, aboveInPhi)   );
	  E_HAD.push_back(m_SupercellMapHAD->GetBinContent(i+1, j)            );
	  E_HAD.push_back(m_SupercellMapHAD->GetBinContent(i+1, belowInPhi)   );
	  E_HAD.push_back(m_SupercellMapHAD->GetBinContent(i, belowInPhi)     );
	  E_HAD.push_back(m_SupercellMapHAD->GetBinContent(i-1, belowInPhi)   );
	  std::vector<double> E_EM12_central;
	  E_EM12_central.reserve(5);
	  std::vector<double> E_EM12_above;
	  E_EM12_above.reserve(6);
	  std::vector<double> E_EM12_below;
	  E_EM12_below.reserve(6);
	  float seedEta = m_SupercellMapTWR->GetXaxis()->GetBinCenter(i);
	  int EM2seedBin = m_SupercellMapEM2_coarse->GetXaxis()->FindBin(seedEta-0.025);

	  // Make a vector with the 5 possible energies in the central phi row
	  E_EM12_central.push_back(m_SupercellMapEM2_coarse->GetBinContent(EM2seedBin-2,j)+m_SupercellMapEM2_coarse->GetBinContent(EM2seedBin-1,j)+m_SupercellMapEM1_coarse->GetBinContent(EM2seedBin-2,j)+m_SupercellMapEM1_coarse->GetBinContent(EM2seedBin-1,j));
	  E_EM12_central.push_back(m_SupercellMapEM2_coarse->GetBinContent(EM2seedBin-1,j)+m_SupercellMapEM2_coarse->GetBinContent(EM2seedBin-0,j)+m_SupercellMapEM1_coarse->GetBinContent(EM2seedBin-1,j)+m_SupercellMapEM1_coarse->GetBinContent(EM2seedBin-0,j));
	  E_EM12_central.push_back(m_SupercellMapEM2_coarse->GetBinContent(EM2seedBin-0,j)+m_SupercellMapEM2_coarse->GetBinContent(EM2seedBin+1,j)+m_SupercellMapEM1_coarse->GetBinContent(EM2seedBin-0,j)+m_SupercellMapEM1_coarse->GetBinContent(EM2seedBin+1,j));
	  E_EM12_central.push_back(m_SupercellMapEM2_coarse->GetBinContent(EM2seedBin+1,j)+m_SupercellMapEM2_coarse->GetBinContent(EM2seedBin+2,j)+m_SupercellMapEM1_coarse->GetBinContent(EM2seedBin+1,j)+m_SupercellMapEM1_coarse->GetBinContent(EM2seedBin+2,j));
	  E_EM12_central.push_back(m_SupercellMapEM2_coarse->GetBinContent(EM2seedBin+2,j)+m_SupercellMapEM2_coarse->GetBinContent(EM2seedBin+3,j)+m_SupercellMapEM1_coarse->GetBinContent(EM2seedBin+2,j)+m_SupercellMapEM1_coarse->GetBinContent(EM2seedBin+3,j));
	  //For upper and lower phi rows take the bin with highest energy
	  for(int k=-2;k<=3;k++){
	    E_EM12_above.push_back(m_SupercellMapEM2_coarse->GetBinContent(EM2seedBin+k,aboveInPhi)+m_SupercellMapEM1_coarse->GetBinContent(EM2seedBin+k,aboveInPhi));
	    E_EM12_below.push_back(m_SupercellMapEM2_coarse->GetBinContent(EM2seedBin+k,belowInPhi)+m_SupercellMapEM1_coarse->GetBinContent(EM2seedBin+k,belowInPhi));
	  }

	  sort(E_EM0.begin(), E_EM0.end());
	  sort(E_EM3.begin(), E_EM3.end());
	  sort(E_HAD.begin(), E_HAD.end());
	  sort(E_EM12_central.begin(), E_EM12_central.end());
	  sort(E_EM12_above.begin(), E_EM12_above.end());
	  sort(E_EM12_below.begin(), E_EM12_below.end());

	  double eFEX_BC=0;

	  //Three hottest bins in EM0
	  eFEX_BC = E_EM0.at(E_EM0.size()-1)+E_EM0.at(E_EM0.size()-2)+E_EM0.at(E_EM0.size()-3);
	  //Two hottest bins in EM3
	  eFEX_BC += E_EM3.at(E_EM3.size()-1)+E_EM3.at(E_EM3.size()-2);
	  //Three hottest bins in HAD
	  eFEX_BC += E_HAD.at(E_HAD.size()-1)+E_HAD.at(E_HAD.size()-2)+E_HAD.at(E_HAD.size()-3);
	  eFEX_BC += E_EM12_central.at(E_EM12_central.size()-1);
	  eFEX_BC += E_EM12_above.at(E_EM12_above.size()-1);
	  eFEX_BC += E_EM12_below.at(E_EM12_below.size()-1);

	  //Bigcluster isolation
	  float nomeFEX_BCiso = 0;
	  nomeFEX_BCiso += E_EM12_central.at(E_EM12_central.size()-1);
	  nomeFEX_BCiso += E_EM12_above.at(E_EM12_above.size()-1);
	  nomeFEX_BCiso += E_EM12_below.at(E_EM12_below.size()-1);
	  nomeFEX_BCiso += E_EM0.at(E_EM0.size()-1);
	  nomeFEX_BCiso += E_EM0.at(E_EM0.size()-2);
	  nomeFEX_BCiso += E_EM0.at(E_EM0.size()-3);
	  nomeFEX_BCiso += E_EM3.at(E_EM3.size()-1);
	  nomeFEX_BCiso += E_EM3.at(E_EM3.size()-2);

	  float denBCiso = 0;
	  denBCiso += m_SupercellMapTWR->GetBinContent(i,j);
	  denBCiso += m_SupercellMapTWR->GetBinContent(i-1,j);
	  denBCiso += m_SupercellMapTWR->GetBinContent(i+1,j);
	  denBCiso += m_SupercellMapTWR->GetBinContent(i,j+1);
	  denBCiso += m_SupercellMapTWR->GetBinContent(i-1,j+1);
	  denBCiso += m_SupercellMapTWR->GetBinContent(i+1,j+1);
	  denBCiso += m_SupercellMapTWR->GetBinContent(i,j-1);
	  denBCiso += m_SupercellMapTWR->GetBinContent(i-1,j-1);
	  denBCiso += m_SupercellMapTWR->GetBinContent(i+1,j-1);

	  float eFEX_BCiso = nomeFEX_BCiso/denBCiso;

	  // Set boolean for whether event passes 12 GeV isolation cut, hardcoding isolation threshold for now
	  bool eFEX_BCiso_12pass = true;
	  if (10000. < eFEX_BC && 15000. > eFEX_BC && eFEX_BCiso < 0.38)
	    eFEX_BCiso_12pass = false;

	  // Set boolean for whether event passes 20 GeV isolation cut, hardcoding isolation threshold for now
	  bool eFEX_BCiso_20pass = true;
	  if (20000. < eFEX_BC && 25000. > eFEX_BC && eFEX_BCiso < 0.18)
	    eFEX_BCiso_20pass = false;

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
	  (*acc_Ore_clusterET)(*clForTau) = eFEX_OregonET;
	  (*acc_BC_clusterET)(*clForTau) = eFEX_BC;
	  (*acc_BC_clusterIso)(*clForTau) = 0;
	  if(denBCiso > 0)
	    (*acc_BC_clusterIso)(*clForTau) = eFEX_BCiso;
	  (*acc_BC_clusterIso_12pass)(*clForTau) = eFEX_BCiso_12pass;
	  (*acc_BC_clusterIso_20pass)(*clForTau) = eFEX_BCiso_20pass;
	  (*acc_Ore_clusterIso)(*clForTau) = 0;
	  if(oreIsoOuterET > 0)
	    (*acc_Ore_clusterIso)(*clForTau) = eFEX_OregonIso;
	  (*acc_Ore_clusterIso_12pass)(*clForTau) = eFEX_OregonIso_12pass;
	  (*acc_Ore_clusterIso_20pass)(*clForTau) = eFEX_OregonIso_20pass;
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
