/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TestTruthCellsAlg.cxx
//
//  Author   : ioannis.nomidis@cern.ch
//  Created  : DEC 2016
//
//  DESCRIPTION:
//     Algorithm to test TruthCells
//
//*****************************************************************************

#include "CaloCalibHitRec/TestTruthCellsAlg.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "CaloEvent/CaloClusterCellLink.h"
#include "CaloUtils/CaloCellList.h"
#include "GaudiKernel/ITHistSvc.h"

//Gaudi Includes
#include "StoreGate/StoreGateSvc.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/xAODTruthHelpers.h"

//
#include "CaloInterface/ICaloCellMakerTool.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "xAODEgamma/EgammaEnums.h"

TestTruthCellsAlg::TestTruthCellsAlg(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator)
   , calibHitContainers(0)
   , hClusterEvsSumCellE(0), hCalibHitEvsClusterE(0), hClusterEvsTruthE(0), hClusterTrueEvsTruthShowerE(0), hClusterEResolution(0)
{
  declareProperty("TruthCaloCellsContainerName",m_truthCellsContainerName);
  declareProperty("TruthCaloClustersContainerName",m_truthClustersContainerName);
  declareProperty("CaloCalibHitsContainerName",m_caloCalibHitsContainerName);
}


TestTruthCellsAlg::~TestTruthCellsAlg() {}


////////////////   INITIALIZE   ///////////////////////
StatusCode TestTruthCellsAlg::initialize() 
{ 
  if (service("THistSvc",m_ths).isFailure())
    return StatusCode::FAILURE;
 
  for (int i=0; i<4; i++) {    
    hCalibHitEvsTruthE.push_back( new TH1F(Form("hCalibHitEvsTruthE_Cont%i",i),"",20,0.,1.) );
    ATH_CHECK(m_ths->regHist(Form("/HIST/%s",hCalibHitEvsTruthE[i]->GetName()),hCalibHitEvsTruthE[i]));
  }
				   
  hCalibHitEtotvsTruthE = new TH1F("hCalibHitEtotvsTruthE","",150,0.96,1.02);
  ATH_CHECK(m_ths->regHist(Form("/HIST/%s",hCalibHitEtotvsTruthE->GetName()),hCalibHitEtotvsTruthE));
  
  hCalibHitEnotilevsTruthE = new TH1F("hCalibHitEnotilevsTruthE","",150,0.96,1.02);
  ATH_CHECK(m_ths->regHist(Form("/HIST/%s",hCalibHitEnotilevsTruthE->GetName()),hCalibHitEnotilevsTruthE));
    
  hClusterEvsSumCellE = new TH1F("hClusterEvsSumCellE","",100,-0.5,0.5);
  ATH_CHECK(m_ths->regHist(Form("/HIST/%s",hClusterEvsSumCellE->GetName()),hClusterEvsSumCellE));

  hCalibHitEvsClusterE = new TH1F("hCalibHitEvsClusterE","",100,-0.5,0.5);
  ATH_CHECK(m_ths->regHist(Form("/HIST/%s",hCalibHitEvsClusterE->GetName()),hCalibHitEvsClusterE));

  hClusterEvsTruthE = new TH1F("hClusterEvsTruthE","",100,-0.5,0.5);
  ATH_CHECK(m_ths->regHist(Form("/HIST/%s",hClusterEvsTruthE->GetName()),hClusterEvsTruthE));

  hClusterTrueEvsTruthShowerE = new TH1F("hClusterTrueEvsTruthShowerE","",100,-0.75,1.05);
  ATH_CHECK(m_ths->regHist(Form("/HIST/%s",hClusterTrueEvsTruthShowerE->GetName()),hClusterTrueEvsTruthShowerE));

  hClusterEResolution = new TH1F("hClusterEResolution","",100,-0.1,0.5);
  ATH_CHECK(m_ths->regHist(Form("/HIST/%s",hClusterEResolution->GetName()),hClusterEResolution));
    
  const int nKeys = m_caloCalibHitsContainerName.size(); 
  calibHitContainers = new std::vector<const CaloCalibrationHitContainer*>(nKeys);
  
  ATH_MSG_INFO("initialisation completed" );
  return StatusCode::SUCCESS;
}


/////////////////   EXECUTE   //////////////////////
StatusCode TestTruthCellsAlg::execute()
{
  // Retrieve the container:
  const xAOD::ElectronContainer* electrons = evtStore()->retrieve< const xAOD::ElectronContainer >( "Electrons" );
  if( ! electrons ) {
    ATH_MSG_ERROR( "Couldn't retrieve electron container");
    return StatusCode::FAILURE;
  }

  // Check that the auxiliary store association was made successfully:
  if( ! electrons->hasStore() ) {
    ATH_MSG_ERROR( "No auxiliary store got associated to the electron container" );
    return StatusCode::FAILURE;
  }

  const int nKeys = m_caloCalibHitsContainerName.size(); 
  for (int i=0; i<nKeys; i++) {
    ATH_CHECK(evtStore()->retrieve(calibHitContainers->at(i),m_caloCalibHitsContainerName[i].c_str()));
  }

 
  double calibhitSumE[4] = {0,0,0,0};    
  for (int i=0; i<nKeys; i++) {
    for (auto calibHit: *(calibHitContainers->at(i))) {
      if (!calibHit || calibHit->particleID()!=10001) continue; //calib hit not from this truth particle
      if (i>2) calibhitSumE[3] += calibHit->energyTotal(); //sum up tile deposits
      else     calibhitSumE[i] += calibHit->energyTotal();	
    }
  }
  float calibhitSumEtot = calibhitSumE[0]+calibhitSumE[1]+calibhitSumE[2]+calibhitSumE[3];


  const CaloCellContainer* truthCells;
  ATH_CHECK(evtStore()->retrieve(truthCells, m_truthCellsContainerName.c_str()));
  ATH_MSG_INFO("Truth cells: " << truthCells->size());

  const xAOD::CaloClusterContainer* truthClusters;
  ATH_CHECK(evtStore()->retrieve(truthClusters, m_truthClustersContainerName.c_str()));
  ATH_MSG_INFO("Truth clusters: " << truthClusters->size());

  float cellSumE = 0;
  for (auto cell: *truthCells)  cellSumE += cell->energy();

  double truthClusterE = 0;
  for (const auto cluster: *truthClusters)  {
    ATH_MSG_DEBUG("cluster E " << cluster->e() );
    truthClusterE = cluster->e();
  }
  if (truthClusterE>0) hClusterEvsSumCellE->Fill( 1 - cellSumE/truthClusterE );
  if (calibhitSumEtot>0) hCalibHitEvsClusterE->Fill( 1 - truthClusterE/calibhitSumEtot ); 
       
  float truthPartE = 0;
  for (const auto el: *electrons) {
    if (!el) continue;      
    const xAOD::TruthParticle *true_electron = xAOD::TruthHelpers::getTruthParticle(*(el));     
    if (!true_electron) continue;
    if (true_electron->absPdgId()!=11 || true_electron->status()!=1 || true_electron->barcode() != 10001) continue;
    truthPartE = true_electron->e();

    const xAOD::CaloCluster* cluster = el->caloCluster();
    const CaloClusterCellLink* cellLinks = cluster->getCellLinks();
    if (!cellLinks) {
      ATH_MSG_DEBUG("cluster->getCellLinks(), no valid links");
      continue;
    }

    float Ecluster = cluster->e();
    float EclusterTrue = getTrueEnergyInCluster(cluster,truthCells);
    ATH_MSG_DEBUG("Cluster energy reco / true : " << Ecluster << " / " << EclusterTrue);
    
    // hClusterTrueEvsTruthShowerE->Fill( EclusterTrue/truthClusterE );
    // hClusterEResolution->Fill(1-EclusterTrue/Ecluster);
    
  }
  // for (int i=0; i<4; i++) {    
  //   hCalibHitEvsTruthE[i]->Fill( calibhitSumE[i]/truthPartE );
  // }
  // hCalibHitEtotvsTruthE->Fill( calibhitSumEtot/truthPartE );
  // hCalibHitEnotilevsTruthE->Fill( (calibhitSumEtot-calibhitSumE[3])/truthPartE );
  // hClusterEvsTruthE->Fill( 1-truthClusterE/truthPartE );
 	

  ATH_MSG_INFO(" E, sum truth cells " << cellSumE << " truth cluster " << truthClusterE << " calibhits " << calibhitSumE[1] << " truth part " << truthPartE );
  
  ATH_MSG_DEBUG("execute() completed successfully" );
  return StatusCode::SUCCESS;
}


/////////////////   FINALIZE   //////////////////////
StatusCode TestTruthCellsAlg::finalize()
{
  calibHitContainers->clear();
  delete calibHitContainers;
  calibHitContainers = 0;

  ATH_MSG_INFO("finalize() successfully" );  
  return StatusCode::SUCCESS;
}

////////////////
float TestTruthCellsAlg::getTrueEnergyInCluster(const xAOD::CaloCluster* cluster,const CaloCellContainer* truthCells)
{
  if (!cluster) {
    ATH_MSG_DEBUG("no valid cluster");
    return 0;
  }
  
  const CaloClusterCellLink* cellLinks = cluster->getCellLinks();
  if (!cellLinks) {
    ATH_MSG_DEBUG("no valid cell links");
    return 0;
  }

  double sumE = 0.;
  xAOD::CaloCluster::const_cell_iterator cell_itr = cluster->begin();
  xAOD::CaloCluster::const_cell_iterator cell_end = cluster->end();	
  for (; cell_itr != cell_end; ++cell_itr) {
    const CaloCell* cell = *cell_itr;

    const CaloCell* truthCell = truthCells->findCell(cell->caloDDE()->calo_hash());
    if (!truthCell) continue; //not found
    // note: supercluster may contain the cell more than once; pick up weight from link to count the energy correctly
    sumE += cell_itr.weight() * truthCell->energy();    
  }
  return sumE;
}
