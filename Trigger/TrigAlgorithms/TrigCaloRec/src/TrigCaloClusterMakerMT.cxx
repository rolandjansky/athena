/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigCaloClusterMaker.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
//
// AUTHOR:   C. Santamarina 
//           This is an Hlt algorithm that creates a cell container
//           with calorimeter cells within an RoI. Afterwards a cluster
//           container with the clusters made with those cells is
//           created with the standard offline clustering algorithms.
//
// ********************************************************************
//
#include <sstream>

//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"
#include "CaloInterface/ISetCaloCellContainerName.h"
//
#include "AthenaMonitoring/Monitored.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
//
#include "CaloEvent/CaloCellContainer.h"
//
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
//
#include "CaloUtils/CaloCollectionHelper.h"
//
#include "CaloUtils/CaloClusterStoreHelper.h"
//
#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "CaloRec/CaloClusterProcessor.h"

#include "CaloEvent/CaloTowerContainer.h"
//
#include "TrigCaloClusterMakerMT.h"
#include "TrigCaloRec/TrigCaloQuality.h"
//#include "TrigTimeAlgs/TrigTimerSvc.h"

//#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODTrigCalo/CaloClusterTrigAuxContainer.h"
//#include "xAODCaloEvent/CaloClusterChangeSignalState.h"

//
class ISvcLocator;


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigCaloClusterMakerMT::TrigCaloClusterMakerMT(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_pCaloClusterContainer(NULL)
{

  // Eta and Phi size of the RoI window...

  // Name(s) of Cluster Maker Tools
  declareProperty("ClusterMakerTools",m_clusterMakerNames);

  // Name(s) of Cluster Correction Tools
  declareProperty("ClusterCorrectionTools",m_clusterCorrectionNames);


}

/////////////////////////////////////////////////////////////////////
    // DESTRUCTOR:
    /////////////////////////////////////////////////////////////////////
    //
    TrigCaloClusterMakerMT::~TrigCaloClusterMakerMT()
{  }

/////////////////////////////////////////////////////////////////////
    // INITIALIZE:
    // The initialize method will create all the required algorithm objects
    // Note that it is NOT NECESSARY to run the initialize of individual
    // sub-algorithms.  The framework takes care of it.
    /////////////////////////////////////////////////////////////////////
    //

   StatusCode TrigCaloClusterMakerMT::initialize()
{
  ATH_MSG_DEBUG("in TrigCaloClusterMakerMT::initialize()" );

  if (!m_monTool.empty()) {
    ATH_MSG_DEBUG("Retrieving monTool");
    CHECK(m_monTool.retrieve());
  } else {
    ATH_MSG_INFO("No monTool configured => NO MONITOING");
  }
     
  // Cache pointer to ToolSvc
  IToolSvc* toolSvc = 0;// Pointer to Tool Service
  if (service("ToolSvc", toolSvc).isFailure()) {
    ATH_MSG_FATAL (" Tool Service not found " );
    return StatusCode::FAILURE;
  }

  std::vector<std::string>::iterator itrName;
  std::vector<std::string>::iterator endName;

  for (int iC=1;iC<3;++iC){ 
    if (iC==1) {
      itrName = m_clusterMakerNames.begin();
      endName = m_clusterMakerNames.end();
    } else if (iC==2) {
      itrName = m_clusterCorrectionNames.begin();
      endName = m_clusterCorrectionNames.end();
    }

    for (; itrName!=endName; ++itrName) {

      ListItem theItem(*itrName);
      IAlgTool* algtool;

      if( toolSvc->retrieveTool(theItem.type(), theItem.name(), algtool,this).isFailure() ) {
	ATH_MSG_FATAL ("Unable to find tool for " << (*itrName) );
	return StatusCode::FAILURE;
      } else {
	ATH_MSG_DEBUG((*itrName) << " successfully retrieved" );
	if(iC==1) {
	  m_clusterMakerPointers.push_back(dynamic_cast<CaloClusterCollectionProcessor*>(algtool) );
	} else if (iC==2) {
	  m_clusterCorrectionPointers.push_back(dynamic_cast<CaloClusterProcessor*>(algtool) );
	}
      }
    }
  }
 
  // end of helpers...
 
ATH_CHECK( m_inputCaloQualityKey.initialize() );
ATH_CHECK( m_inputCellsKey.initialize() );
ATH_CHECK( m_inputTowersKey.initialize() );
ATH_CHECK( m_outputClustersKey.initialize() );

  ATH_MSG_DEBUG("Initialization of TrigCaloClusterMakerMT completed successfully");

  return StatusCode::SUCCESS;
}


StatusCode TrigCaloClusterMakerMT::finalize()
{
    ATH_MSG_DEBUG("in finalize()" );
return StatusCode::SUCCESS;

}


StatusCode TrigCaloClusterMakerMT::execute() 
{
  // Monitoring initialization...
  auto time_tot = Monitored::Timer("TIME_execute");
  auto time_clusMaker = Monitored::Timer("TIME_ClustMaker");
  auto time_clusCorr = Monitored::Timer("TIME_ClustCorr");




    ATH_MSG_DEBUG("in TrigCaloClusterMakerMT::execute()" );

  bool isSW=false;
  
  auto ctx = getContext();

  // We now take care of the Cluster Making... 
  auto  clusterContainer =   SG::makeHandle (m_outputClustersKey, ctx); 
  ATH_MSG_VERBOSE(" Output Clusters : " <<  clusterContainer.name());

  ATH_CHECK( clusterContainer.record (std::make_unique<xAOD::CaloClusterContainer>(),  std::make_unique<xAOD::CaloClusterTrigAuxContainer> () ));

  xAOD::CaloClusterContainer* pCaloClusterContainer = clusterContainer.ptr();
  ATH_MSG_VERBOSE(" created ClusterContainer at 0x" << std::hex << pCaloClusterContainer<< std::dec);


  // monitored variables 
  auto mon_container_size = Monitored::Scalar("container_size", 0.);
  auto mon_clusEt = Monitored::Collection("Et",   *pCaloClusterContainer, &xAOD::CaloCluster::et );
  auto mon_clusSignalState = Monitored::Collection("signalState",   *pCaloClusterContainer, &xAOD::CaloCluster::signalState );
  auto mon_clusSize = Monitored::Collection("clusterSize",   *pCaloClusterContainer, &xAOD::CaloCluster::clusterSize );
  std::vector<double>       clus_phi;
  std::vector<double>       clus_eta;
  std::vector<double>       N_BAD_CELLS;
  std::vector<double>       ENG_FRAC_MAX;
  std::vector<unsigned int> sizeVec; 
  auto mon_clusPhi = Monitored::Collection("Phi", clus_phi); // phi and eta are virtual methods of CaloCluster
  auto mon_clusEta = Monitored::Collection("Eta", clus_eta);
  auto mon_badCells = Monitored::Collection("N_BAD_CELLS",N_BAD_CELLS );
  auto mon_engFrac = Monitored::Collection("ENG_FRAC_MAX",N_BAD_CELLS );
  auto mon_size = Monitored::Collection("size",sizeVec );
  auto monitorIt = Monitored::Group( m_monTool, time_clusMaker,  time_clusCorr, mon_container_size, mon_clusEt,
					    mon_clusPhi, mon_clusEta, mon_clusSignalState, mon_clusSize, 
					    mon_badCells, mon_engFrac, mon_size);	    


  auto  pTrigCaloQuality =   SG::makeHandle (m_inputCaloQualityKey, ctx); 
  //TrigCaloQuality*  pTrigCaloQuality = trigCaloQuality.ptr();

  ATH_MSG_VERBOSE(" Input CaloQuality : " <<  pTrigCaloQuality.name());


  // Looping over cluster maker tools... 
  
  time_clusMaker.start();

  auto cells = SG::makeHandle(m_inputCellsKey, ctx);
  ATH_MSG_VERBOSE(" Input Cells : " << cells.name() <<" of size " <<cells->size() );

  auto towers = SG::makeHandle(m_inputTowersKey, ctx);
  //  ATH_MSG_DEBUG(" Input Towers : " << towers.name() <<" of size "<< towers->size());

  int index=0;
  for (CaloClusterCollectionProcessor* clproc : m_clusterMakerPointers) {
    
    // JTB: TO DO: The offline tools should be changed to set declare ReadHandles 
    
    // We need to set the properties of the offline tools. this way of doing is ugly...
    // Abusing of harcoding?? Yes...
    
    AlgTool* algtool = dynamic_cast<AlgTool*> (clproc);
    if(m_clusterMakerNames[index].find("CaloTopoClusterMaker") != std::string::npos){
      
      
      if(!algtool || algtool->setProperty( StringProperty("CellsName",cells.name() )).isFailure()) {
        ATH_MSG_ERROR ("ERROR setting the CellsName name in the offline tool" );
	//        return HLT::TOOL_FAILURE;
	return StatusCode::SUCCESS;
      }
      
    } else if(m_clusterMakerNames[index].find("trigslw") != std::string::npos){
      if(!algtool || algtool->setProperty( StringProperty("CaloCellContainer",cells.name()) ).isFailure()) { 
	ATH_MSG_ERROR ("ERROR setting the CaloCellContainer name in the offline tool" ); 
        //return HLT::TOOL_FAILURE; 
	return StatusCode::SUCCESS;
      } 
      if(!algtool || algtool->setProperty( StringProperty("TowerContainer", towers.name() )).isFailure()) {
	ATH_MSG_ERROR ("ERROR setting the Tower Container name in the offline tool" );
        //return HLT::TOOL_FAILURE;
	return StatusCode::SUCCESS;
      }
    }
      

    if ( (clproc->name()).find("trigslw") != std::string::npos ) isSW=true;
    if ( clproc->execute(pCaloClusterContainer).isFailure() ) {
      ATH_MSG_ERROR("Error executing tool " << m_clusterMakerNames[index] );
    } else {
      ATH_MSG_VERBOSE("Executed tool " << m_clusterMakerNames[index] );
    }

    ++index;
  }
  time_clusMaker.stop();
  
  
  //save raw state (uncalibrated)
  for (xAOD::CaloCluster* cl : *pCaloClusterContainer)
    {
      ATH_MSG_VERBOSE("found cluster with state "
		      << cl->signalState() <<  ", calE: " << cl->calE() << ", calEta: " << cl->calEta() << ", calPhi: " << cl->calPhi() << " calM: " <<cl->calM());
      ATH_MSG_VERBOSE(" Cluster Et  = " << cl->et() );
      ATH_MSG_VERBOSE(" Cluster eta = " << cl->eta() );
      ATH_MSG_VERBOSE(" Cluster phi = " << cl->phi() );
      cl->setRawE(cl->calE());
      cl->setRawEta(cl->calEta());
      cl->setRawPhi(cl->calPhi());
      cl->setRawM(cl->calM());
      ATH_MSG_VERBOSE(" before correction=>Cluster Et  = " << cl->et() );
      ATH_MSG_VERBOSE(" before correction=>Cluster eta = " << cl->eta() );
      ATH_MSG_VERBOSE(" before correction=>Cluster phi = " << cl->phi() );
    }
  
  
  
  // Looping over cluster correction tools... 
  
  time_clusCorr.start();
  ATH_MSG_VERBOSE(" Running cluster correction tools");
  std::vector<CaloClusterProcessor*>::const_iterator itrcct = m_clusterCorrectionPointers.begin();
  std::vector<CaloClusterProcessor*>::const_iterator endcct = m_clusterCorrectionPointers.end();
    
  index=0;
  for (; itrcct!=endcct; ++itrcct) {

    ATH_MSG_VERBOSE(" Running " << (*itrcct)->name());
    ISetCaloCellContainerName* setter =
      dynamic_cast<ISetCaloCellContainerName*> (*itrcct);
    if (setter) {
      if(setter->setCaloCellContainerName(cells.name()) .isFailure()) {
        ATH_MSG_ERROR("ERROR setting the CaloCellContainer name in the offline tool" );
	//        return HLT::BAD_JOB_SETUP;
	return StatusCode::SUCCESS;
      }
    }
    
    for (xAOD::CaloCluster* cl : *pCaloClusterContainer) {
      bool exec = false;
      if      ( (fabsf(cl->eta0())<1.45)  && ((*itrcct)->name().find("37") != std::string::npos ) ) exec=true;
      else if ( (fabsf(cl->eta0())>=1.45) && ((*itrcct)->name().find("55") != std::string::npos ) ) exec=true;
      else exec = false;
      if (!isSW) exec=true;
      if ( exec ) {
      if ( (*itrcct)->execute(cl).isFailure() ) {
        ATH_MSG_ERROR("Error executing correction tool " <<  m_clusterCorrectionNames[index] );
	//        return HLT::TOOL_FAILURE;
	return StatusCode::SUCCESS;
      } else {
	ATH_MSG_VERBOSE("Executed correction tool " << m_clusterCorrectionNames[index] );
      }
      } // Check conditions
    }
    ++index;
    
  }
  time_clusCorr.stop();

  // quality flag for clusters
#if 0
  if (pTrigCaloQuality &&  pTrigCaloQuality->getError()) { // conversion errors in this RoI
    for (xAOD::CaloCluster* cl : *pCaloClusterContainer) {
      CaloClusterBadChannelData data(-999.,-999.,CaloSampling::Unknown,CaloBadChannel(4)); // unphysical data
      cl->addBadChannel(data);
    }
  }
#endif

  // fill monitored variables
  for (xAOD::CaloCluster* cl : *pCaloClusterContainer) {
    
    CaloClusterCellLink* num_cell_links = cl->getCellLinks();
    if(! num_cell_links) {
      sizeVec.push_back(0);
    } else {
      sizeVec.push_back(num_cell_links->size()); 
    }
    clus_phi.push_back(cl->phi());
    clus_eta.push_back(cl->eta());
    N_BAD_CELLS.push_back(cl->getMomentValue(xAOD::CaloCluster::N_BAD_CELLS));
    ENG_FRAC_MAX.push_back(cl->getMomentValue(xAOD::CaloCluster::ENG_FRAC_MAX));
  }
  
  
  ATH_MSG_DEBUG(" REGTEST: Produced a Cluster Container of Size= " << pCaloClusterContainer->size() );
  if(!pCaloClusterContainer->empty()) {
    ATH_MSG_DEBUG(" REGTEST: Last Cluster Et  = " << (pCaloClusterContainer->back())->et() );
    ATH_MSG_DEBUG(" REGTEST: Last Cluster eta = " << (pCaloClusterContainer->back())->eta() );
    ATH_MSG_DEBUG(" REGTEST: Last Cluster phi = " << (pCaloClusterContainer->back())->phi() );
    mon_container_size = pCaloClusterContainer->size(); // fill monitored variable
  }
  
  return StatusCode::SUCCESS; 
}



