/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloClusterMaker.cxx,v 1.26 2009-04-18 02:56:18 ssnyder Exp $
//
// Description: see CaloClusterMaker.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Sven Menke
//
// Modified: Feb 09, 2005 (DLelas)
//          - CaloClusterMaker applies correction type tool on a whole 
//            cluster collection now. Needed for Sliding Window
//            cell weights calculation.
//
// Modified Feb 2014 (W Lampl)
//          - Migrate to xAOD::CaloCluster(Container)
//          - Move to AthAlgorithm, some simplifications
//-----------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "CaloRec/CaloClusterMaker.h"

//---------------
// C++ Headers --
//---------------
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "CaloEvent/CaloClusterCellLinkContainer.h"

#include "CaloUtils/CaloClusterSignalState.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "AthenaKernel/errorcheck.h"

#include "GaudiKernel/IChronoStatSvc.h"

//###############################################################################
CaloClusterMaker::CaloClusterMaker(const std::string& name, 
				   ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator)
  , m_clusterMakerTools(this)
  , m_clusterCorrectionTools(this)
  , m_chrono("ChronoStatSvc", name)
  , m_saveSignalState(true)
  , m_chronoTools(false)
{

  // Name of Cluster Container to be registered in TDS
  declareProperty("ClustersOutputName",m_clustersOutputName);  
  
  // Name(s) of Cluster Maker Tools
  declareProperty("ClusterMakerTools",m_clusterMakerTools);

  // Name(s) of Cluster Correction Tools
  declareProperty("ClusterCorrectionTools",m_clusterCorrectionTools);

  declareProperty("KeepEachCorrection",  m_keep_each_correction=false);

  // Name(s) of Cluster Correction Tools (even field) to trigger the
  // recording of the current cluster container in StoreGate before
  // its execution and the corresponding container name(s) (odd
  // fields). This property and KeepEachCorrection are mutually
  // exclusive
  //declareProperty("KeepCorrectionToolAndContainerNames",  m_keepCorrectionToolAndContainerNames);

  // save uncalibrated cluster signal state 
  declareProperty("SaveUncalibratedSignalState",m_saveSignalState);

  //Make Chrono Auditors for Cluster maker and correction tools
  declareProperty("ChronoTools", m_chronoTools);
}

//###############################################################################

CaloClusterMaker::~CaloClusterMaker()
{ }

//###############################################################################

StatusCode CaloClusterMaker::initialize()
{
  //Retrieve maker tools
  for (auto tool :  m_clusterMakerTools) {
    StatusCode sc=tool.retrieve();
    if (sc.isFailure())
      ATH_MSG_ERROR("Failed to retrieve maker tool " << tool);
    else
      ATH_MSG_DEBUG("Successfully retrieved maker tool " << tool);
  }//end loop over maker tools

  
  for (auto tool :  m_clusterCorrectionTools) {
     StatusCode sc=tool.retrieve();
    if (sc.isFailure())
      ATH_MSG_ERROR("Failed to retrieve correction tool " << tool);
    else
      ATH_MSG_DEBUG("Successfully retrieved correction tool " << tool);
  }//end loop over correction tools

  if (m_chronoTools) {
    msg(MSG::INFO) << "Will use ChronoStatSvc to monitor ClusterMaker and ClusterCorrection tools" << endreq;
    CHECK( m_chrono.retrieve() );
  }

  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode CaloClusterMaker::finalize() {
  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode CaloClusterMaker::execute() {

  // make a Cluster Container 
  xAOD::CaloClusterContainer* clusColl=CaloClusterStoreHelper::makeContainer(&(*evtStore()),m_clustersOutputName,msg());
  if (!clusColl) return StatusCode::FAILURE;
  
  ToolHandleArray<CaloClusterCollectionProcessor>::const_iterator toolIt, toolIt_e; //Iterators over Tool handles
  toolIt=m_clusterMakerTools.begin();
  toolIt_e=m_clusterMakerTools.end();
  
  //Make Clusters: Execute each maker tool
  //for (CaloClusterCollectionProcessor& tool :  m_clusterMakerTools) {  //Doesn't work because CaloClusterCollectionProcessor is a base class
  for(;toolIt!=toolIt_e;++toolIt) {
    const std::string chronoName = this->name() + "_" +toolIt->name();
    if (m_chronoTools) m_chrono->chronoStart(chronoName);
    CHECK((*toolIt)->execute(clusColl));
    if (m_chronoTools) m_chrono->chronoStop(chronoName);
  } //End loop over maker tools

  // PL set calibrated state
  if ( m_saveSignalState ) {
    //Fixme: Maybe this loop would auto-vectorize breaking into four separate loops for each quantity
    for (xAOD::CaloCluster* fClus : *clusColl) {
      ATH_MSG_DEBUG( "found cluster with state "<< fClus->signalState());
      fClus->setRawE(fClus->calE());
      fClus->setRawEta(fClus->calEta());
      fClus->setRawPhi(fClus->calPhi());
      fClus->setRawM(fClus->calM());	
    }//end loop over clusters
  }


  //Apply corrections: Exectue each correction tool
  //for (CaloClusterCollectionProcessor* tool :  m_clusterCorrectionTools) { //Doesn't work because CaloClusterCollectionProcessor is a base class
  toolIt=m_clusterCorrectionTools.begin();
  toolIt_e=m_clusterCorrectionTools.end();
  for(;toolIt!=toolIt_e;++toolIt) {
    const std::string& toolname=(*toolIt).name();
    if (m_keep_each_correction) {
      const std::string interimContName=m_clustersOutputName + "-pre" +toolname;
      xAOD::CaloClusterContainer* interimCont=CaloClusterStoreHelper::makeContainer(&(*evtStore()),interimContName,msg());
      CaloClusterStoreHelper::copyContainer(clusColl,interimCont);
      CHECK(CaloClusterStoreHelper::finalizeClusters(&(*evtStore()),interimCont, interimContName, msg()));
    }
    
    ATH_MSG_DEBUG(" Applying correction = " << toolname);
    const std::string chronoName = this->name() + "_" + toolname;
    if (m_chronoTools) m_chrono->chronoStart(chronoName);
    CHECK((*toolIt)->execute(clusColl));
    if (m_chronoTools) m_chrono->chronoStop(chronoName);
  }//End loop over correction tools

  ATH_MSG_DEBUG("Created cluster container with " << clusColl->size() << " clusters");
  CHECK(CaloClusterStoreHelper::finalizeClusters(&(*evtStore()),clusColl,m_clustersOutputName,msg()));

  return StatusCode::SUCCESS;
}


const std::string& CaloClusterMaker::getOutputContainerName() const {
  return m_clustersOutputName;
}
