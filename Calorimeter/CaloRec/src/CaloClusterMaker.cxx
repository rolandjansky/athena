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

/*
namespace {

std::string correction_suffix (const std::string& name)
{
  std::string::size_type pos = name.find ("/");
  if (pos != std::string::npos)
    ++pos;
  else
    pos = 0;
  return std::string ("-pre") + name.substr (pos, std::string::npos);
}

} // anonymous namespace
*/

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
  declareProperty("KeepCorrectionToolAndContainerNames",  m_keepCorrectionToolAndContainerNames);

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
  //---- check properties -------------------------------
  
  if ( m_keep_each_correction && m_keepCorrectionToolAndContainerNames.size() > 0 ) {
    msg(MSG::FATAL)
	<< " The two properties KeepEachCorrection (which creates a cluster"
	<< " collection for each correction tool) and" 
	<< " KeepCorrectionToolAndContainerNames (which creates a cluster"
	<< " collection for the tools specified in this list with a name also"
	<< " specified here) can not be used together. Please make up your mind"
	<< " and try again ... " << endreq;
    return StatusCode::FAILURE;
  }    
  if (m_keepCorrectionToolAndContainerNames.size()%2!=0) {
    msg(MSG::FATAL) << "The property KeepCorrectionToolAndContainerNames (which creates a cluster"
		    << " collection for the tools specified in this list with a name also specified here)"
		    << " needs an even number of elements. Found " 
		    << m_keepCorrectionToolAndContainerNames.size() << endreq;
    
    return StatusCode::FAILURE;
  } 


  for (size_t i=0;i<m_keepCorrectionToolAndContainerNames.size();i+=2) {
    // If tool name is a full name, i.e. 'A/B', then we need to keep only the 'B' part
    size_t slashPos = m_keepCorrectionToolAndContainerNames[i].find('/');
    std::string toolName = (slashPos==std::string::npos?m_keepCorrectionToolAndContainerNames[i]:m_keepCorrectionToolAndContainerNames[i].substr(slashPos+1));
    m_CorrectionToolAndContainerNamesMap.insert(std::make_pair(toolName,m_keepCorrectionToolAndContainerNames[i+1]));
  }

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

/*
StatusCode CaloClusterMaker::makeContainer() {   

  // Create the xAOD container and its auxiliary store:
  //xAOD::CaloClusterContainer* clusColl = new xAOD::CaloClusterContainer();
  m_clusColl=new xAOD::CaloClusterContainer();
  CHECK( evtStore()->record(m_clusColl, m_clustersOutputName ) );
  xAOD::CaloClusterAuxContainer* aux = new xAOD::CaloClusterAuxContainer();
  CHECK( evtStore()->record(aux, m_clustersOutputName + "Aux." ) );
  m_clusColl->setStore( aux );

  m_cellLinks= new CaloClusterCellLinkContainer();
  CHECK( evtStore()->record(m_cellLinks, m_clustersOutputName + "Links"));
  
  ATH_MSG_DEBUG("Initial record cluster and aux container with name " << m_clustersOutputName);

  return StatusCode::SUCCESS;
}
*/

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
    std::string interimContName;
    bool keepCorrection=false;
    if (m_keep_each_correction) {
      interimContName=m_clustersOutputName + "-pre" +toolname;
      keepCorrection=true;
    }
    else {
      auto mapIt=m_CorrectionToolAndContainerNamesMap.find(toolname);
      if (mapIt!=m_CorrectionToolAndContainerNamesMap.end()) {
	interimContName=mapIt->second;
	keepCorrection=true;
      }
    }//end else
    if (keepCorrection) {
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



  /*
    std::string cluster_name;

    // See if we should store an intermediate cluster container before
    // the next correction.  If so, then we set cluster_name to the
    // SG key we'll use for this intermediate container.  Otherwise,
    // we leave cluster_name blank.

    if (m_keep_each_correction) {
      cluster_name = m_clustersOutputName + 
        correction_suffix (toolname);
    }
    else {
      for (unsigned int j = 0;
           j < m_keepCorrectionToolAndContainerNames.size();
           j += 2)
      {
        if (m_keepCorrectionToolAndContainerNames[j] == toolname)
            m_clusterCorrectionNames[itool])
        {
          cluster_name = m_keepCorrectionToolAndContainerNames[j+1];
          break;
        }
      }
    }

    if ( ! cluster_name.empty() ) {
      
      CHECK( CaloClusterStoreHelper::recordClusters(m_storeGate, 
                                                    clusCollinterm,
                                                    cluster_name,
                                                    msg()) );
      CHECK( CaloClusterStoreHelper::finalizeClusters(m_storeGate, 
						      clusCollinterm,
						      cluster_name,
						      msg()) );
      if(clusCollinterm->size()>0) ATH_MSG_DEBUG( "record & finalize, key = " << cluster_name << ", E = " << (*clusCollinterm->begin())->e());

      clusCollinterm = makeContainer (clusCollinterm);

    }

    CHECK( m_clusterCorrectionPointers[itool]->execute(clusCollinterm) );
    if (clusCollinterm->size()>0) ATH_MSG_DEBUG( "copy + correction: first cluster, E = " << (*clusCollinterm->begin())->e());
  }

  ATH_MSG_DEBUG( "CaloClusterContainer size = "<< clusColl->size());

  clusColl->swap (*clusCollinterm);
  // lock the Clusters Container
  CHECK( CaloClusterStoreHelper::finalizeClusters(m_storeGate,
						  clusColl,
						  m_clustersOutputName,
						  msg()) );
  if (clusColl->size()>0) ATH_MSG_DEBUG("final copy + finalized, key = " << m_clustersOutputName << ", E = " << (*clusColl->begin())->e());
  //DEBUG
  //  checkOut = CaloClusterStoreHelper::checkStores(m_storeGate,
  //						 cluster_name,
  //						 msg());
  //  checkOut = CaloClusterStoreHelper::analyzeClusterLinks(m_storeGate,
  //							 cluster_name,
  //						 msg());
  //  checkOut = CaloClusterStoreHelper::crosscheckContainers(m_storeGate,
  //							  msg());
  //  msg() << m_storeGate->dump() << endmsg;
  //DEBUG

  delete clusCollinterm;
  */
  return StatusCode::SUCCESS;
}

