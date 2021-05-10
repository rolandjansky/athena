/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

// TrigT1CaloFexPerf includes
#include "FwdCellReader.h"
#include "FwdCell.h"

#include "FFexEMFinder.h"
#include "TSystem.h"
#include "xAODEventInfo/EventInfo.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "iostream"
#include <sstream>
#include <fstream>
#include <TMath.h>
#include "StoreGate/ReadHandle.h"
#include "xAODEventInfo/EventInfo.h"
#include "LArElecCalib/LArCalibErrorCode.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloTriggerTool/LArTTCell.h"
#include "TFile.h"


#include <math.h>

#include "PathResolver/PathResolver.h"

// Decorators for additional shower variables
namespace {
  static const SG::AuxElement::Decorator<float> dec_fracMax("R4FracMaxEnergy");
  static const SG::AuxElement::Decorator<float> dec_showD("R4ShowerDepthZ");
  static const SG::AuxElement::Decorator<float> dec_showVarR("R4ShowerVarianceR");
  static const SG::AuxElement::Decorator<float> dec_showVarZ("R4ShowerVarianceZ");
}



namespace LVL1{
FwdCellReader::FwdCellReader( const std::string& name, ISvcLocator* pSvcLocator ) :
	AthAlgorithm( name, pSvcLocator ){

		// Please do not change these unless you know how to handle the implications in the code
		declareProperty("range",m_range=0.2);

		// abs(eta) cut on fwd cells (in addition to EMEC, HEC and FCAL selection)
		declareProperty("cellEtaCut",m_cellEtaCut=2.5);
		// abs(et) cut on fwd cells. No cut by default
		declareProperty("cellEtCut",m_cellEtCut=0);
		
		// input and output handles
		declareProperty("InputCellContainer", m_inputCellContainerKey = "AllCalo");
		declareProperty("OutputClusterName", m_outputClusterContainerKey = "fEleCluster");

	}


FwdCellReader::~FwdCellReader() {

}


StatusCode FwdCellReader::initialize() {
	ATH_MSG_INFO ("Initializing " << name() << "...");

	ATH_CHECK(m_inputCellContainerKey.initialize());
	ATH_CHECK(m_outputClusterContainerKey.initialize());

	return StatusCode::SUCCESS;
}

StatusCode FwdCellReader::finalize() {

	ATH_MSG_INFO ("Finalizing " << name() << "...");

	return StatusCode::SUCCESS;
}

StatusCode FwdCellReader::execute() {

	ATH_MSG_DEBUG ("Executing " << name() << "...");

	const EventContext &ctx = Gaudi::Hive::currentContext();

	// input cells
	auto cellsHandle = SG::makeHandle(m_inputCellContainerKey, ctx);
 	if (!cellsHandle.isValid())
   	{
      		ATH_MSG_ERROR("Failed to retrieve " << m_inputCellContainerKey.key());
      		return StatusCode::FAILURE;
   	}
	CaloConstCellContainer cells(SG::VIEW_ELEMENTS);
	cells.assign(cellsHandle->begin(), cellsHandle->end());

	// output clusters
	auto clusters = std::make_unique<xAOD::EmTauRoIContainer>();
  	auto auxClusters = std::make_unique<xAOD::EmTauRoIAuxContainer>();
	clusters->setStore(auxClusters.get());



	// Initialise necessary variables
  	std::vector<std::shared_ptr<FFexEMFinder::electron>> electronsTrigger;

	setFilterPassed(false);
	const xAOD::EventInfo* eventInfo = 0;
	CHECK( evtStore()->retrieve( eventInfo, "EventInfo" ) );

  	std::vector<CaloCell*> forwardCells;

	// Only use fwd cells
  	for(unsigned i=0; i<cells.size();i++){
    		const CaloCell*cell = cells.at(i);
		if(fabs(cell->eta())<m_cellEtaCut) continue;
		if(fabs(cell->et())<m_cellEtCut) continue; 
    		if(cell->caloDDE()->getSubCalo()==(CaloCell_Base_ID::SUBCALO)2) forwardCells.push_back((CaloCell*)cell); // is FCAL cell
    		if(cell->caloDDE()->getSubCalo()==(CaloCell_Base_ID::SUBCALO)1) forwardCells.push_back((CaloCell*)cell); // is HEC cell
    		if(cell->caloDDE()->getSubCalo()==(CaloCell_Base_ID::SUBCALO)0) forwardCells.push_back((CaloCell*)cell); // is EMEC cell
    		
  	}



	// Run the actual ffex EM algo

	// Create seeds
	std::shared_ptr<FFexEMFinder::Seed> cellSeedsTrigger= std::make_shared<FFexEMFinder::Seed>();
	std::shared_ptr<FFexEMFinder::SeedXY> cellSeedsTriggerXY= std::make_shared<FFexEMFinder::SeedXY>();

	// Fill seeds
	if(cellSeedsTrigger->eta.empty()) CHECK(FFexEMFinder::SeedGrid(forwardCells,cellSeedsTrigger,cellSeedsTriggerXY));

	// Run the algorithm
	CHECK(FFexEMFinder::SeedFinding(forwardCells,cellSeedsTrigger,cellSeedsTriggerXY,m_range));  //range = 0.2

	// construct the objects
	CHECK(FFexEMFinder::BuildElectron(cellSeedsTrigger,cellSeedsTriggerXY,electronsTrigger)); 


	// Fill the objects
	// Run over all electron candidates found by algorithm
	for (unsigned c=0;c<electronsTrigger.size();c++){

		std::shared_ptr<FFexEMFinder::electron> electronTrigger = electronsTrigger.at(c);	
				
		xAOD::EmTauRoI* fEleCl = new xAOD::EmTauRoI();
		clusters->push_back(fEleCl);
		fEleCl->setEta(electronTrigger->eta);
		fEleCl->setPhi(electronTrigger->phi);
		fEleCl->setEmClus(electronTrigger->et_full);
		fEleCl->setEmIsol(electronTrigger->isolationEt);
		fEleCl->setHadIsol(electronTrigger->hadEt);

		dec_fracMax(*fEleCl) = electronTrigger->fractionMaxEnergyCell;
		dec_showD(*fEleCl) = electronTrigger->showerDepthZ;
		dec_showVarR(*fEleCl) = electronTrigger->showerVarianceR;
		dec_showVarZ(*fEleCl) = electronTrigger->showerVarianceZ;

	}

	electronsTrigger.clear();

	SG::WriteHandle<xAOD::EmTauRoIContainer> writeHandle(m_outputClusterContainerKey, ctx);
  	ATH_CHECK(writeHandle.record(std::move(clusters), std::move(auxClusters)));

	setFilterPassed(true); //if got here, assume that means algorithm passed
	return StatusCode::SUCCESS;
}
}
