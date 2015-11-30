/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "egammaSwTool.h"


#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloLayerCalculator.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloRec/CaloClusterProcessor.h"

#include "GaudiKernel/ListItem.h"

#include <algorithm> 
#include <math.h>

//  END OF HEADER FILES INCLUDE

/////////////////////////////////////////////////////////////////

//  CONSTRUCTOR:

egammaSwTool::egammaSwTool(const std::string& type,
			   const std::string& name,
			   const IInterface* parent)
  : AthAlgTool(type, name, parent)
{

  // declare interface
  declareInterface<IegammaSwTool>(this);

  // The following properties are specified at run-time
  // (declared in jobOptions file)   
  
  // Name(s) of Cluster Correction Tools
  declareProperty("ClusterCorrectionToolsEle55",m_clusterCorrectionNamesEle55);
  declareProperty("ClusterCorrectionToolsEle35",m_clusterCorrectionNamesEle35);
  declareProperty("ClusterCorrectionToolsEle37",m_clusterCorrectionNamesEle37);

  declareProperty("ClusterCorrectionToolsGam35",m_clusterCorrectionNamesGam35);
  declareProperty("ClusterCorrectionToolsGam55",m_clusterCorrectionNamesGam55);
  declareProperty("ClusterCorrectionToolsGam37",m_clusterCorrectionNamesGam37);

  declareProperty("ClusterCorrectionToolsEconv55",m_clusterCorrectionNamesEconv55);
  declareProperty("ClusterCorrectionToolsEconv35",m_clusterCorrectionNamesEconv35);
  declareProperty("ClusterCorrectionToolsEconv37",m_clusterCorrectionNamesEconv37);
}

// ===============================================================
// DESTRUCTOR:
egammaSwTool::~egammaSwTool()
{ 
}

// ===============================================================
// INITIALIZE METHOD:
StatusCode egammaSwTool::initialize()
{
  ATH_MSG_DEBUG(" Initializing egammaSwTool");
  
  StatusCode sc;

  // the tool to extrapolate
  IToolSvc* p_toolSvc = 0;
  sc = service("ToolSvc",p_toolSvc);
  if ( sc.isFailure() ){
    ATH_MSG_ERROR("Cannot find ToolSvc! ");
    return StatusCode::FAILURE;
  }
  else {
    
    // CaloClusterCorrections tools
    std::vector<std::string>::const_iterator firstTool=m_clusterCorrectionNamesEle55.begin();
    std::vector<std::string>::const_iterator lastTool =m_clusterCorrectionNamesEle55.end();
    
    for ( ; firstTool != lastTool; firstTool++ ) {
      IAlgTool* algToolPtr;
      ListItem  clusAlgoTool(*firstTool);
      StatusCode sCode = p_toolSvc->retrieveTool(clusAlgoTool.type(),
						 clusAlgoTool.name(),
						 algToolPtr,
						 this);
      if ( sCode.isFailure() ) {
	ATH_MSG_ERROR("Cannot find tool for " << *firstTool);
      }
      else {
	ATH_MSG_DEBUG("Found tool for " << *firstTool);
	// check for tool type
	CaloClusterProcessor* 
	  theTool = dynamic_cast<CaloClusterProcessor*>(algToolPtr);
	if ( theTool != 0 ) { 
	  m_clusterCorrectionPointersEle55.push_back(theTool); 
	}
      } 
    }

    firstTool=m_clusterCorrectionNamesEle35.begin();
    lastTool =m_clusterCorrectionNamesEle35.end();
    
    for ( ; firstTool != lastTool; firstTool++ ) {
      IAlgTool* algToolPtr;
      ListItem  clusAlgoTool(*firstTool);
      StatusCode sCode = p_toolSvc->retrieveTool(clusAlgoTool.type(),
						 clusAlgoTool.name(),
						 algToolPtr,
						 this);
      if ( sCode.isFailure() ) {
	ATH_MSG_ERROR("Cannot find tool for " << *firstTool);
      }
      else {
	ATH_MSG_DEBUG("Found tool for " << *firstTool);
	// check for tool type
	CaloClusterProcessor* 
	  theTool = dynamic_cast<CaloClusterProcessor*>(algToolPtr);
	if ( theTool != 0 ) { 
	  m_clusterCorrectionPointersEle35.push_back(theTool); 
	}
      } 
    }
    
    firstTool=m_clusterCorrectionNamesEle37.begin();
    lastTool =m_clusterCorrectionNamesEle37.end();
    
    for ( ; firstTool != lastTool; firstTool++ ) {
      IAlgTool* algToolPtr;
      ListItem  clusAlgoTool(*firstTool);
      StatusCode sCode = p_toolSvc->retrieveTool(clusAlgoTool.type(),
						 clusAlgoTool.name(),
						 algToolPtr,
						 this);
      if ( sCode.isFailure() ) {
	ATH_MSG_ERROR("Cannot find tool for " << *firstTool);
      }
      else {
	ATH_MSG_DEBUG("Found tool for " << *firstTool);
	// check for tool type
	CaloClusterProcessor* 
	  theTool = dynamic_cast<CaloClusterProcessor*>(algToolPtr);
	if ( theTool != 0 ) { 
	  m_clusterCorrectionPointersEle37.push_back(theTool); 
	}
      } 
    }

    firstTool=m_clusterCorrectionNamesGam35.begin();
    lastTool =m_clusterCorrectionNamesGam35.end();
    
    for ( ; firstTool != lastTool; firstTool++ ) {
      IAlgTool* algToolPtr;
      ListItem  clusAlgoTool(*firstTool);
      StatusCode sCode = p_toolSvc->retrieveTool(clusAlgoTool.type(),
						 clusAlgoTool.name(),
						 algToolPtr,
						 this);
      if ( sCode.isFailure() ) {
	ATH_MSG_ERROR("Cannot find tool for " << *firstTool);
      }
      else {
	ATH_MSG_DEBUG("Found tool for " << *firstTool);
	// check for tool type
	CaloClusterProcessor* 
	  theTool = dynamic_cast<CaloClusterProcessor*>(algToolPtr);
	if ( theTool != 0 ) { 
	  m_clusterCorrectionPointersGam35.push_back(theTool); 
	}
      } 
    }
    
    firstTool=m_clusterCorrectionNamesGam55.begin();
    lastTool =m_clusterCorrectionNamesGam55.end();
    
    for ( ; firstTool != lastTool; firstTool++ ) {
      IAlgTool* algToolPtr;
      ListItem  clusAlgoTool(*firstTool);
      StatusCode sCode = p_toolSvc->retrieveTool(clusAlgoTool.type(),
						 clusAlgoTool.name(),
						 algToolPtr,
						 this);
      if ( sCode.isFailure() ) {
	ATH_MSG_ERROR("Cannot find tool for " << *firstTool);
      }
      else {
	ATH_MSG_DEBUG("Found tool for " << *firstTool);
	// check for tool type
	CaloClusterProcessor* 
	  theTool = dynamic_cast<CaloClusterProcessor*>(algToolPtr);
	if ( theTool != 0 ) { 
	  m_clusterCorrectionPointersGam55.push_back(theTool); 
	}
      } 
    }

    firstTool=m_clusterCorrectionNamesGam37.begin();
    lastTool =m_clusterCorrectionNamesGam37.end();
    
    for ( ; firstTool != lastTool; firstTool++ ) {
      IAlgTool* algToolPtr;
      ListItem  clusAlgoTool(*firstTool);
      StatusCode sCode = p_toolSvc->retrieveTool(clusAlgoTool.type(),
						 clusAlgoTool.name(),
						 algToolPtr,
						 this);
      if ( sCode.isFailure() ) {
	ATH_MSG_ERROR("Cannot find tool for " << *firstTool);
      }
      else {
	ATH_MSG_DEBUG("Found tool for " << *firstTool);
	// check for tool type
	CaloClusterProcessor* 
	  theTool = dynamic_cast<CaloClusterProcessor*>(algToolPtr);
	if ( theTool != 0 ) { 
	  m_clusterCorrectionPointersGam37.push_back(theTool); 
	}
      } 
    }

    firstTool=m_clusterCorrectionNamesEconv55.begin();
    lastTool =m_clusterCorrectionNamesEconv55.end();
    
    for ( ; firstTool != lastTool; firstTool++ ) {
      IAlgTool* algToolPtr;
      ListItem  clusAlgoTool(*firstTool);
      StatusCode sCode = p_toolSvc->retrieveTool(clusAlgoTool.type(),
						 clusAlgoTool.name(),
						 algToolPtr,
						 this);
      if ( sCode.isFailure() ) {
	ATH_MSG_ERROR("Cannot find tool for " << *firstTool);
      }
      else {
	ATH_MSG_DEBUG("Found tool for " << *firstTool);
	// check for tool type
	CaloClusterProcessor* 
	  theTool = dynamic_cast<CaloClusterProcessor*>(algToolPtr);
	if ( theTool != 0 ) { 
	  m_clusterCorrectionPointersEconv55.push_back(theTool); 
	}
      } 
    }

    firstTool=m_clusterCorrectionNamesEconv35.begin();
    lastTool =m_clusterCorrectionNamesEconv35.end();
    
    for ( ; firstTool != lastTool; firstTool++ ) {
      IAlgTool* algToolPtr;
      ListItem  clusAlgoTool(*firstTool);
      StatusCode sCode = p_toolSvc->retrieveTool(clusAlgoTool.type(),
						 clusAlgoTool.name(),
						 algToolPtr,
						 this);
      if ( sCode.isFailure() ) {
	ATH_MSG_ERROR("Cannot find tool for " << *firstTool);
      }
      else {
	ATH_MSG_DEBUG("Found tool for " << *firstTool);
	// check for tool type
	CaloClusterProcessor* 
	  theTool = dynamic_cast<CaloClusterProcessor*>(algToolPtr);
	if ( theTool != 0 ) { 
	  m_clusterCorrectionPointersEconv35.push_back(theTool); 
	}
      } 
    }

    firstTool=m_clusterCorrectionNamesEconv37.begin();
    lastTool =m_clusterCorrectionNamesEconv37.end();
    
    for ( ; firstTool != lastTool; firstTool++ ) {
      IAlgTool* algToolPtr;
      ListItem  clusAlgoTool(*firstTool);
      StatusCode sCode = p_toolSvc->retrieveTool(clusAlgoTool.type(),
						 clusAlgoTool.name(),
						 algToolPtr,
						 this);
      if ( sCode.isFailure() ) {
	ATH_MSG_ERROR("Cannot find tool for " << *firstTool);
      }
      else {
	ATH_MSG_DEBUG("Found tool for " << *firstTool);
	// check for tool type
	CaloClusterProcessor* 
	  theTool = dynamic_cast<CaloClusterProcessor*>(algToolPtr);
	if ( theTool != 0 ) { 
	  m_clusterCorrectionPointersEconv37.push_back(theTool); 
	}
      } 
    }

    ATH_MSG_DEBUG("egammaSwTool initialisation completed successfully"); 
  }    
  
  return sc;
}

// ==================================================================
// FINALIZE METHOD:
StatusCode egammaSwTool::finalize()
{
  return StatusCode::SUCCESS;
}

// ==============================================================
// ATHENA EXECUTE METHOD:
StatusCode egammaSwTool::execute(xAOD::CaloCluster *cluster)
{ 
  ATH_MSG_DEBUG("Executing egammaSwTool");
  
  // protection against bad clusters
  if (cluster==0) return StatusCode::SUCCESS;

  std::vector<CaloClusterProcessor*>::const_iterator firstTool, lastTool;
  
  xAOD::CaloCluster::ClusterSize requestedSize = cluster->clusterSize();
  switch (requestedSize) {
  case xAOD::CaloCluster::SW_55ele:
    firstTool = m_clusterCorrectionPointersEle55.begin();
    lastTool  = m_clusterCorrectionPointersEle55.end();
    break;
  case xAOD::CaloCluster::SW_35ele:
    firstTool = m_clusterCorrectionPointersEle35.begin();
    lastTool  = m_clusterCorrectionPointersEle35.end();
    break;
  case xAOD::CaloCluster::SW_37ele:
    firstTool = m_clusterCorrectionPointersEle37.begin();
    lastTool  = m_clusterCorrectionPointersEle37.end();
    break;
  case xAOD::CaloCluster::SW_35gam:
    firstTool = m_clusterCorrectionPointersGam35.begin();
    lastTool  = m_clusterCorrectionPointersGam35.end();
    break;
  case xAOD::CaloCluster::SW_55gam:
    firstTool = m_clusterCorrectionPointersGam55.begin();
    lastTool  = m_clusterCorrectionPointersGam55.end();
    break;
  case xAOD::CaloCluster::SW_37gam:
    firstTool = m_clusterCorrectionPointersGam37.begin();
    lastTool  = m_clusterCorrectionPointersGam37.end();
    break;
  case xAOD::CaloCluster::SW_55Econv:
    firstTool = m_clusterCorrectionPointersEconv55.begin();
    lastTool  = m_clusterCorrectionPointersEconv55.end();
    break;
  case xAOD::CaloCluster::SW_35Econv:
    firstTool = m_clusterCorrectionPointersEconv35.begin();
    lastTool  = m_clusterCorrectionPointersEconv35.end();
    break;
  case xAOD::CaloCluster::SW_37Econv:
    firstTool = m_clusterCorrectionPointersEconv37.begin();
    lastTool  = m_clusterCorrectionPointersEconv37.end();
    break;
  default:
    ATH_MSG_DEBUG("Inexisting cluster type and calibration requested: " << requestedSize);
    return StatusCode::FAILURE;
    break;
  }
    
  // loop tools
  for ( ; firstTool != lastTool; firstTool++ ) {
    StatusCode processCheck = (*firstTool)->execute(cluster);
    if ( processCheck.isFailure() ) {
      ATH_MSG_ERROR("Cluster corrections failed!");
      //  return processCheck;
    }
  }

  return StatusCode::SUCCESS;
}

