/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "egammaSwTool.h"


#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloLayerCalculator.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

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
  

  ATH_CHECK(populateTools(m_clusterCorrectionPointersEle35));
  ATH_CHECK(populateTools(m_clusterCorrectionPointersEle37));
  ATH_CHECK(populateTools(m_clusterCorrectionPointersEle55));
  //
  ATH_CHECK(populateTools(m_clusterCorrectionPointersGam35));
  ATH_CHECK(populateTools(m_clusterCorrectionPointersGam37));
  ATH_CHECK(populateTools(m_clusterCorrectionPointersGam55));
  //
  ATH_CHECK(populateTools(m_clusterCorrectionPointersEconv35));
  ATH_CHECK(populateTools(m_clusterCorrectionPointersEconv37));
  ATH_CHECK(populateTools(m_clusterCorrectionPointersEconv55));
  //SuperCluster
  ATH_CHECK(populateTools(m_clusterCorrectionPointersSuperClusterEle35));
  ATH_CHECK(populateTools(m_clusterCorrectionPointersSuperClusterEle37));
  ATH_CHECK(populateTools(m_clusterCorrectionPointersSuperClusterEle55));
  //
  ATH_CHECK(populateTools(m_clusterCorrectionPointersSuperClusterGam35));
  ATH_CHECK(populateTools(m_clusterCorrectionPointersSuperClusterGam37));
  ATH_CHECK(populateTools(m_clusterCorrectionPointersSuperClusterGam55));
  //
  ATH_CHECK(populateTools(m_clusterCorrectionPointersSuperClusterEconv35));
  ATH_CHECK(populateTools(m_clusterCorrectionPointersSuperClusterEconv37));
  ATH_CHECK(populateTools(m_clusterCorrectionPointersSuperClusterEconv55));
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

StatusCode egammaSwTool::populateTools(ToolHandleArray<CaloClusterProcessor>& tools) 
{
  for (const auto tool : tools){
    if ( tool.retrieve().isFailure() ){
      ATH_MSG_FATAL( "Could not get tool: " << tool);
      return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("Retrieved Tool " << tool); 
  }
  return StatusCode::SUCCESS;
}

StatusCode egammaSwTool::processTools(ToolHandleArray<CaloClusterProcessor>& tools,
				      xAOD::CaloCluster* cluster) const
{
  
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
