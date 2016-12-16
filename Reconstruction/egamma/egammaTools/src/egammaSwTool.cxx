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
  //
  declareProperty("ClusterCorrectionToolsGam35",m_clusterCorrectionNamesGam35);
  declareProperty("ClusterCorrectionToolsGam55",m_clusterCorrectionNamesGam55);
  declareProperty("ClusterCorrectionToolsGam37",m_clusterCorrectionNamesGam37);
  //
  declareProperty("ClusterCorrectionToolsEconv55",m_clusterCorrectionNamesEconv55);
  declareProperty("ClusterCorrectionToolsEconv35",m_clusterCorrectionNamesEconv35);
  declareProperty("ClusterCorrectionToolsEconv37",m_clusterCorrectionNamesEconv37);
  //
  //SuperCluster
  declareProperty("ClusterCorrectionToolsSuperClusterEle35",m_clusterCorrectionNamesSuperClusterEle35);
  declareProperty("ClusterCorrectionToolsSuperClusterEle55",m_clusterCorrectionNamesSuperClusterEle55);
  declareProperty("ClusterCorrectionToolsSuperClusterEle37",m_clusterCorrectionNamesSuperClusterEle37);
  //
  declareProperty("ClusterCorrectionToolsSuperClusterGam35",m_clusterCorrectionNamesSuperClusterGam35);
  declareProperty("ClusterCorrectionToolsSuperClusterGam55",m_clusterCorrectionNamesSuperClusterGam55);
  declareProperty("ClusterCorrectionToolsSuperClusterGam37",m_clusterCorrectionNamesSuperClusterGam37);
  //
  declareProperty("ClusterCorrectionToolsSuperClusterEconv35",m_clusterCorrectionNamesSuperClusterEconv35);
  declareProperty("ClusterCorrectionToolsSuperClusterEconv55",m_clusterCorrectionNamesSuperClusterEconv55);
  declareProperty("ClusterCorrectionToolsSuperClusterEconv37",m_clusterCorrectionNamesSuperClusterEconv37);

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
  

  ATH_CHECK(populateTools(m_clusterCorrectionNamesEle35,m_clusterCorrectionPointersEle35));
  ATH_CHECK(populateTools(m_clusterCorrectionNamesEle37,m_clusterCorrectionPointersEle37));
  ATH_CHECK(populateTools(m_clusterCorrectionNamesEle55,m_clusterCorrectionPointersEle55));
  //
  ATH_CHECK(populateTools(m_clusterCorrectionNamesGam35,m_clusterCorrectionPointersGam35));
  ATH_CHECK(populateTools(m_clusterCorrectionNamesGam37,m_clusterCorrectionPointersGam37));
  ATH_CHECK(populateTools(m_clusterCorrectionNamesGam55,m_clusterCorrectionPointersGam55));
  //
  ATH_CHECK(populateTools(m_clusterCorrectionNamesEconv35,m_clusterCorrectionPointersEconv35));
  ATH_CHECK(populateTools(m_clusterCorrectionNamesEconv37,m_clusterCorrectionPointersEconv37));
  ATH_CHECK(populateTools(m_clusterCorrectionNamesEconv55,m_clusterCorrectionPointersEconv55));
  //SuperCluster
  ATH_CHECK(populateTools(m_clusterCorrectionNamesSuperClusterEle35,m_clusterCorrectionPointersSuperClusterEle35));
  ATH_CHECK(populateTools(m_clusterCorrectionNamesSuperClusterEle37,m_clusterCorrectionPointersSuperClusterEle37));
  ATH_CHECK(populateTools(m_clusterCorrectionNamesSuperClusterEle55,m_clusterCorrectionPointersSuperClusterEle55));
  //
  ATH_CHECK(populateTools(m_clusterCorrectionNamesSuperClusterGam35,m_clusterCorrectionPointersSuperClusterGam35));
  ATH_CHECK(populateTools(m_clusterCorrectionNamesSuperClusterGam37,m_clusterCorrectionPointersSuperClusterGam37));
  ATH_CHECK(populateTools(m_clusterCorrectionNamesSuperClusterGam55,m_clusterCorrectionPointersSuperClusterGam55));
  //
  ATH_CHECK(populateTools(m_clusterCorrectionNamesSuperClusterEconv35,m_clusterCorrectionPointersSuperClusterEconv35));
  ATH_CHECK(populateTools(m_clusterCorrectionNamesSuperClusterEconv37,m_clusterCorrectionPointersSuperClusterEconv37));
  ATH_CHECK(populateTools(m_clusterCorrectionNamesSuperClusterEconv55,m_clusterCorrectionPointersSuperClusterEconv55));
  return StatusCode::SUCCESS;
}

// ==================================================================
// FINALIZE METHOD:
StatusCode egammaSwTool::finalize(){
  return StatusCode::SUCCESS;
}

// ==============================================================
// ATHENA EXECUTE METHOD:
StatusCode egammaSwTool::execute(xAOD::CaloCluster *cluster){ 
  ATH_MSG_DEBUG("Executing egammaSwTool");
  
  // protection against bad clusters
  if (cluster==0) return StatusCode::SUCCESS;

  std::vector<CaloClusterProcessor*>::const_iterator firstTool, lastTool;
  
  xAOD::CaloCluster::ClusterSize requestedSize = cluster->clusterSize();
  switch (requestedSize) {
  case xAOD::CaloCluster::SW_55ele:
    ATH_CHECK(processTools(m_clusterCorrectionPointersEle55,cluster));
    break;
  case xAOD::CaloCluster::SW_35ele:
    ATH_CHECK(processTools(m_clusterCorrectionPointersEle35,cluster));
    break;
  case xAOD::CaloCluster::SW_37ele:
    ATH_CHECK(processTools(m_clusterCorrectionPointersEle37,cluster));
    break;
  case xAOD::CaloCluster::SW_35gam:
    ATH_CHECK(processTools(m_clusterCorrectionPointersGam35,cluster));
    break;
  case xAOD::CaloCluster::SW_55gam:
    ATH_CHECK(processTools(m_clusterCorrectionPointersGam55,cluster));
    break;
  case xAOD::CaloCluster::SW_37gam:
    ATH_CHECK(processTools(m_clusterCorrectionPointersGam37,cluster));
    break;
  case xAOD::CaloCluster::SW_55Econv:
    ATH_CHECK(processTools(m_clusterCorrectionPointersEconv55,cluster));
    break;
  case xAOD::CaloCluster::SW_35Econv:
    ATH_CHECK(processTools(m_clusterCorrectionPointersEconv35,cluster));
    break;
  case xAOD::CaloCluster::SW_37Econv:
    ATH_CHECK(processTools(m_clusterCorrectionPointersEconv37,cluster));
    break;
  default:
    ATH_MSG_DEBUG("Inexisting cluster type and calibration requested: " << requestedSize);
    return StatusCode::FAILURE;
    break;
  }
  return StatusCode::SUCCESS;
}
// ==============================================================
// ATHENA EXECUTE METHOD for superClusters

StatusCode egammaSwTool::execute(xAOD::CaloCluster* cluster, xAOD::EgammaParameters::EgammaType egType , bool isBarrel) {
  ATH_MSG_DEBUG("Executing egammaSwTool");
  
  // protection against bad clusters
  if (cluster==0) return StatusCode::SUCCESS;

  if(isBarrel){

    switch (egType) {
    case xAOD::EgammaParameters::electron:
      ATH_MSG_DEBUG("correction for barrel electron");
      ATH_CHECK(processTools(m_clusterCorrectionPointersSuperClusterEle37,cluster));
      break;
    case xAOD::EgammaParameters::unconvertedPhoton:
      ATH_MSG_DEBUG("correction for barrel unconverted");
      ATH_CHECK(processTools(m_clusterCorrectionPointersSuperClusterGam37,cluster));
      break;
    case xAOD::EgammaParameters::convertedPhoton:
      ATH_MSG_DEBUG("correction for barrel converted");
      ATH_CHECK(processTools(m_clusterCorrectionPointersSuperClusterEconv37,cluster));
      break;
    default:
      ATH_MSG_DEBUG("Inexisting correction requested for egType: " <<egType << " isBarrel: " << isBarrel);
      return StatusCode::FAILURE;
      break;
    }
  } else{
    switch (egType) {
    case xAOD::EgammaParameters::electron:
      ATH_MSG_DEBUG("correction for endcap electron");
      ATH_CHECK(processTools(m_clusterCorrectionPointersSuperClusterEle55,cluster));
      break;
    case xAOD::EgammaParameters::unconvertedPhoton:
      ATH_MSG_DEBUG("correction for endcap unconverted");
      ATH_CHECK(processTools(m_clusterCorrectionPointersSuperClusterGam55,cluster));
      break;
    case xAOD::EgammaParameters::convertedPhoton:
      ATH_MSG_DEBUG("correction for endcap converted");
      ATH_CHECK(processTools(m_clusterCorrectionPointersSuperClusterEconv55,cluster));
      break;
    default:
      ATH_MSG_DEBUG("Inexisting correction requested for egType: " <<egType << " isBarrel: " << isBarrel);
      return StatusCode::FAILURE;
      break;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode egammaSwTool::populateTools(const std::vector<std::string>& names , std::vector<CaloClusterProcessor*>& tools) {

 StatusCode sc;
 IToolSvc* p_toolSvc = 0;
 sc = service("ToolSvc",p_toolSvc);
 if ( sc.isFailure() ){
   ATH_MSG_ERROR("Cannot find ToolSvc! ");
   return StatusCode::FAILURE;
 } 
 // CaloClusterCorrections tools
 std::vector<std::string>::const_iterator firstTool=names.begin();
 std::vector<std::string>::const_iterator lastTool =names.end();    
 for ( ; firstTool != lastTool; ++firstTool) {
   IAlgTool* algToolPtr;
   ListItem  clusAlgoTool(*firstTool);
   StatusCode sCode = p_toolSvc->retrieveTool(clusAlgoTool.type(),
					      clusAlgoTool.name(),
					      algToolPtr,
					      this);
   if ( sCode.isFailure() ) {
     ATH_MSG_ERROR("Cannot find tool for " << *firstTool);
     return StatusCode::FAILURE;
   }
   // check for tool type
   CaloClusterProcessor*  theTool = dynamic_cast<CaloClusterProcessor*>(algToolPtr);
   if (theTool != 0 ) { 
     ATH_MSG_DEBUG("Tool " << *firstTool <<  " added in list ");
     tools.push_back(theTool); 
   }
 }
 return StatusCode::SUCCESS;
}

StatusCode egammaSwTool::processTools(const std::vector<CaloClusterProcessor*>& tools,xAOD::CaloCluster* cluster) const{
  
  auto firstTool = tools.begin();
  auto lastTool  = tools.end();
  // loop tools
  for ( ; firstTool != lastTool; ++firstTool ) {
   StatusCode processCheck = (*firstTool)->execute(cluster);
   ATH_MSG_DEBUG("Tool " << (*firstTool)->name() <<  " executing  ");
   if ( processCheck.isFailure() ) {
     ATH_MSG_ERROR("Cluster corrections failed!");
   }
  }
  return StatusCode::SUCCESS;
}
