/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaRec/egammaForwardBuilder.h"
#include "egammaInterfaces/IegammaBaseTool.h"
#include "egammaInterfaces/IegammaBaseTool.h"
#include "egammaInterfaces/IEMFourMomBuilder.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "PathResolver/PathResolver.h"
#include "StoreGate/StoreGateSvc.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/Electron.h" 

#include "EgammaAnalysisInterfaces/IAsgForwardElectronIsEMSelector.h"

#include <algorithm> 
#include <math.h>


using CLHEP::GeV;


//  END OF HEADER FILES INCLUDE

/////////////////////////////////////////////////////////////////

//  CONSTRUCTOR:
    
egammaForwardBuilder::egammaForwardBuilder(const std::string& name, ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_timingProfile(0)
{ 
  // Name of Electron Container to be created
  declareProperty("ElectronOutputName",
		  m_electronOutputName="",
		  "Name of Electron Container to be created");

  // Name of the input cluster collection
  declareProperty("TopoClusterName",
		  m_topoClusterName="",
		  "Name of the input cluster collection");

  // Value of the ET cut
  declareProperty("EtCut",
		  m_ETcut=5.*GeV,
		  "Value of the ET cut");

  // Value of the eta cut
  declareProperty("EtaCut",
		  m_etacut=2.5,
		  "Value of the eta cut");

 
  // Name of the output EM cluster container
  declareProperty("ClusterContainerName",
		  m_outClusterContainerName="",
		  "Name of the output EM cluster container");

  // Selector definition
  declareProperty("forwardelectronIsEMselectors", m_forwardelectronIsEMselectors,
    "The selectors that we need to apply to the FwdElectron object");
  declareProperty("forwardelectronIsEMselectorResultNames", m_forwardelectronIsEMselectorResultNames,
    "The selector result names");

  // Name of the object quality tool
  declareProperty("ObjectQualityToolName", 
		  m_ObjectQualityToolName="",
		  "Name of the object quality Tool");


  // Handle of 4-mom Builder
  declareProperty("FourMomBuilderTool",m_fourMomBuilder,
		  "Handle of 4-mom Builder");

} 


// ================================================================
egammaForwardBuilder::~egammaForwardBuilder()
{ 
  //
  // destructor
  //
}

// =================================================================
StatusCode egammaForwardBuilder::initialize()
{
  //
  // initialize method
  //

  ATH_MSG_DEBUG(" Initializing egammaForwardBuilder ");
  
  // retrieve object quality tool 
  RetrieveObjectQualityTool();

  m_timingProfile = 0;
  StatusCode sc = service("ChronoStatSvc",m_timingProfile);
  if(sc.isFailure() || m_timingProfile == 0) {
    ATH_MSG_ERROR("Cannot find the ChronoStatSvc " << m_timingProfile);
  }

  // retrieve 4-mom builder:
  if ((sc = RetrieveEMFourMomBuilder()).isFailure()) {
    return sc;
  }


  for (const auto& selector : m_forwardelectronIsEMselectors) {
    CHECK(selector.retrieve());
  }
  
  if (m_forwardelectronIsEMselectors.size() != m_forwardelectronIsEMselectorResultNames.size()) {
    ATH_MSG_ERROR("The number of selectors does not match the number of given fwd-electron selector names");
    return StatusCode::FAILURE;
  }
  

  ATH_MSG_DEBUG("Initialization completed successfully");
  return StatusCode::SUCCESS;
}

// ====================================================================
void egammaForwardBuilder::RetrieveObjectQualityTool()
{
  //
  // retrieve object quality tool
  //

  if (m_ObjectQualityToolName=="") {
    ATH_MSG_INFO("egammaOQFlagsBuilder is disabled  " 
		 << m_ObjectQualityToolName);
    return;
  } 

  m_objectqualityTool = ToolHandle<IegammaBaseTool>(m_ObjectQualityToolName);
  if(m_objectqualityTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve " << m_objectqualityTool
		  << ", turning it off");
  }
  else ATH_MSG_DEBUG("Retrieved Tool " << m_objectqualityTool);

  return;
}

StatusCode egammaForwardBuilder::RetrieveEMFourMomBuilder()
{
  //
 // retrieve EMFourMomBuilder tool
 //
  
  if (m_fourMomBuilder.empty()) {
    ATH_MSG_ERROR("EMFourMomBuilder is empty");
    return StatusCode::FAILURE;
  }
  
  
  if(m_fourMomBuilder.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_fourMomBuilder);
    return StatusCode::FAILURE;
  }
  else ATH_MSG_DEBUG("Retrieved Tool "<<m_fourMomBuilder);
  
  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode egammaForwardBuilder::finalize()
{
  //
  // finalize method
  //

  return StatusCode::SUCCESS;
}

// ======================================================================
StatusCode egammaForwardBuilder::execute()
{
  //
  // athena execute method
  //

  ATH_MSG_DEBUG("Executing egammaForwardBuilder ");

  // create an egamma container and register it
  xAOD::ElectronContainer* xaodFrwd = new xAOD::ElectronContainer();
  CHECK( evtStore()->record( xaodFrwd, m_electronOutputName ) );
  xAOD::ElectronAuxContainer* auxFrwd = new xAOD::ElectronAuxContainer();
  CHECK( evtStore()->record( auxFrwd, m_electronOutputName + "Aux." ) );
  xaodFrwd->setStore( auxFrwd );
  ATH_MSG_DEBUG( "Recorded Electrons with key: " << m_electronOutputName );


  //cluster
  xAOD::CaloClusterContainer* outClusterContainer = new xAOD::CaloClusterContainer();
  CHECK( evtStore()->record( outClusterContainer, m_outClusterContainerName ) );
  xAOD::CaloClusterAuxContainer* auxCaloCluster = new xAOD::CaloClusterAuxContainer();
  CHECK( evtStore()->record( auxCaloCluster, m_outClusterContainerName + "Aux." ) );
  outClusterContainer->setStore( auxCaloCluster );



  //Topo cluster Container
  const xAOD::CaloClusterContainer* cluster;
  StatusCode sc = evtStore()->retrieve(cluster, m_topoClusterName);
  if(sc.isFailure()) {
    ATH_MSG_WARNING("egammaForwardBuilder::Could not retrieve Cluster container"); 
    return StatusCode::SUCCESS;
  }



  //loop over cluster container
  xAOD::CaloClusterContainer::const_iterator  clus_begin = cluster->begin();
  xAOD::CaloClusterContainer::const_iterator  clus_end   = cluster->end();

  for (; clus_begin!=clus_end; clus_begin++) {
   
    //Preselectcion cuts
    if((*clus_begin)->et() < m_ETcut||
       fabs((*clus_begin)->eta())<m_etacut)
      continue;
 
    //Save it in the electroncontainer 
    xAOD::Electron* el = new xAOD::Electron();
    xaodFrwd->push_back(el);

    el->setAuthor( xAOD::EgammaParameters::AuthorFwdElectron );

     
    xAOD::CaloCluster *newcluster = new xAOD::CaloCluster(**clus_begin);
    outClusterContainer->push_back(newcluster);
    
    int index = outClusterContainer->size() - 1;
    
    ElementLink<xAOD::CaloClusterContainer> newclusterElementLink;
    newclusterElementLink.toIndexedElement( *outClusterContainer, index  );

    std::vector< ElementLink< xAOD::CaloClusterContainer > > linksToClusters;
    linksToClusters.push_back(newclusterElementLink);
    el->setCaloClusterLinks(linksToClusters);    

    //do  Four Momentum
    CHECK(m_fourMomBuilder->execute(el));
    
    // do object quality 
    CHECK( ExecObjectQualityTool(el) );
    
  
    // FwdSelectors:
    size_t size = m_forwardelectronIsEMselectors.size();
    
    for (size_t i = 0; i<size;++i) {
      const Root::TAccept& accept = m_forwardelectronIsEMselectors[i]->accept(el);
      //save the bool result
      el->setPassSelection(static_cast<bool>(accept), m_forwardelectronIsEMselectorResultNames[i]);
      //save the isem
      el->setSelectionisEM(m_forwardelectronIsEMselectors[i]->IsemValue(), "isEM"+m_forwardelectronIsEMselectorResultNames[i]);
      
    }
  }
  
  ATH_MSG_VERBOSE("egammaForward execute completed successfully");
  
  return StatusCode::SUCCESS;
}  
  
// ===========================================================
StatusCode egammaForwardBuilder::ExecObjectQualityTool(xAOD::Egamma *eg)
{
  //
  // execution of the object quality tools
  //
  // protection in case tool is not available
  // return success as algorithm may be able to run without it 
  // in degraded mode

  if (m_ObjectQualityToolName=="") return StatusCode::SUCCESS;

  // setup chrono for this tool
  std::string chronoName=this->name()+"_"+m_objectqualityTool->name() ;
  if(m_timingProfile) m_timingProfile->chronoStart(chronoName);

  // execute the tool
  StatusCode sc = m_objectqualityTool->execute(eg);
  if ( sc.isFailure() ) {
    ATH_MSG_DEBUG("failure returned by object quality tool"); 
  }

  if(m_timingProfile) m_timingProfile->chronoStop(chronoName);

  return sc;
}
