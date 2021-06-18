/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "TrigL0GepPerf/GepClusteringAlg.h"

// Base class 
#include "TrigL0GepPerf/ITopoClusterMaker.h"

// *** Include custom topoclustering classes inherited from base class ***

//
#include "xAODEventInfo/EventInfo.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "xAODJet/JetContainer.h"
#include "JetEDM/PseudoJetVector.h"


GepClusteringAlg::GepClusteringAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAnalysisAlgorithm( name, pSvcLocator ),
  m_topoclAlg ("TopoclAlg")
{

  declareProperty("CaloNoiseTool",m_noiseTool,"Tool Handle for noise tool");
  declareProperty("TopoclAlg", m_topoclAlg="Unknown", "Custom topoclustering algorithm label. New custom clusters will be named m_topoclAlg+\"TopoClusters\".");

}


StatusCode GepClusteringAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  //
  //This is called once, before the start of the event loop
  //Retrieves of tools you have configured in the joboptions go here
  //


  // Noise tool
  if(m_noiseTool.retrieve().isFailure()){
    ATH_MSG_ERROR("Unable to find noise tool");
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_INFO("Noise tool retrieved");
  }
  // pass noise tool to CaloCellsHandler 
  m_cch.passNoiseTool(m_noiseTool);


  // CaloIndentifier
  CHECK( detStore()->retrieve (m_ccIdHelper, "CaloCell_ID") );
  // pass caloIndentifier to CaloCellsHandler
  m_cch.passCaloCellIDTool(m_ccIdHelper);


  return StatusCode::SUCCESS;
}



StatusCode GepClusteringAlg::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");


  if(m_noiseTool.retrieve().isFailure()){
    ATH_MSG_ERROR("Unable to find noise tool");
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG("Noise tool retrieved");
  }



  const xAOD::EventInfo* ei = 0;
  CHECK( evtStore()->retrieve( ei , "EventInfo" ) );
  ATH_MSG_DEBUG("eventNumber=" << ei->eventNumber() );


  //
  // Get cells information

  const CaloCellContainer* cells = 0;
  CHECK( evtStore()->retrieve( cells, "AllCalo") );

  if(!m_cellsFilledOnce){  
    m_cch.fillCellsInformation(cells);
    m_cellsFilledOnce = true;
  }
  else{
    m_cch.updateCellsInformation(cells);
  }


  //
  // Run topoclustering algorithm
  std::unique_ptr<Gep::ITopoClusterMaker> topoMaker{};

  // *** Instantiate custom topoclustering classes ***


  if( !topoMaker ){ 
    ATH_MSG_ERROR( "No topoclustering class assigned to topoMaker." );
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "Running " << topoMaker->getName() << " topoclustering algorithm." );

  std::vector<Gep::CustomTopoCluster> customClusters = topoMaker->makeTopoCluster( m_cch.getCaloCellsMap() );

  ATH_MSG_DEBUG( "Topoclustering completed." );

  
  // create the new container and its auxiliary store.
  auto athenaTopoClusters = std::make_unique<xAOD::CaloClusterContainer>();
  auto athenaTopoClustersAux = std::make_unique<xAOD::CaloClusterAuxContainer>();
  
  athenaTopoClusters->setStore(athenaTopoClustersAux.get()); //< Connect the two

  // store custom clusters in athena format 
  for(const auto& iClust: customClusters){
    std::unique_ptr<xAOD::CaloCluster> athenaCluster{new xAOD::CaloCluster()};

    xAOD::CaloCluster* iclus = athenaCluster.get();
    athenaTopoClusters->push_back(std::move(athenaCluster));

    iclus->setE(iClust.vec.E());
    iclus->setEta(iClust.vec.Eta());
    iclus->setPhi(iClust.vec.Phi());
    iclus->setTime(iClust.time);
  }

  std::string clustersName = m_topoclAlg + "TopoClusters";
  std::string clustersNameAux = clustersName + "Aux.";

  ATH_MSG_DEBUG( "Storing " << clustersName );
  CHECK( evtStore()->record(std::move(athenaTopoClusters),clustersName) ); 
  CHECK( evtStore()->record(std::move(athenaTopoClustersAux),clustersNameAux) );


  return StatusCode::SUCCESS;
}

