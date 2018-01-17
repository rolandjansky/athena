/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaForwardBuilder.h"
#include "egammaInterfaces/IegammaBaseTool.h"

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


//  END OF HEADER FILES INCLUDE

/////////////////////////////////////////////////////////////////

//  CONSTRUCTOR:
    
egammaForwardBuilder::egammaForwardBuilder(const std::string& name, ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_timingProfile(0)
{ 
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
  
  // the data handle keys
  ATH_CHECK(m_topoClusterKey.initialize());
  ATH_CHECK(m_electronOutputKey.initialize());
  ATH_CHECK(m_outClusterContainerKey.initialize());
  m_outClusterContainerCellLinkKey = m_outClusterContainerKey.key() + "_links";
  ATH_CHECK(m_outClusterContainerCellLinkKey.initialize());

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

  if (m_objectqualityTool.name()=="") {
    ATH_MSG_INFO("egammaOQFlagsBuilder is disabled"); 
    return;
  } 

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
  SG::WriteHandle<xAOD::ElectronContainer> xaodFrwd(m_electronOutputKey);
  ATH_CHECK(xaodFrwd.record(std::make_unique<xAOD::ElectronContainer>(),
			    std::make_unique<xAOD::ElectronAuxContainer>()));

  ATH_MSG_DEBUG( "Recorded Electrons with key: " << m_electronOutputKey.key() );


  //cluster
  SG::WriteHandle<xAOD::CaloClusterContainer> outClusterContainer(m_outClusterContainerKey);
  ATH_CHECK(outClusterContainer.record(std::make_unique<xAOD::CaloClusterContainer>(),
					  std::make_unique<xAOD::CaloClusterAuxContainer>()));
  SG::WriteHandle<CaloClusterCellLinkContainer> outClusterContainerCellLink(m_outClusterContainerCellLinkKey);
  ATH_CHECK(outClusterContainerCellLink.record(std::make_unique<CaloClusterCellLinkContainer>()));

  //Topo cluster Container
  SG::ReadHandle<xAOD::CaloClusterContainer> cluster(m_topoClusterKey);

  // check is only used for serial running; remove when MT scheduler used 
  // --used to be a warning with SUCCESS, but that won't work in MT
  if(!cluster.isValid()) {
    ATH_MSG_FATAL("egammaForwardBuilder::Could not retrieve Cluster container"); 
    return StatusCode::FAILURE;
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
  

  // Now finalize the cluster: based on code in CaloClusterStoreHelper::finalizeClusters
  // Note: I don't specifically set the IProxyDict, since I also don't set it when I create
  //    data handles, either. 
  auto sg = outClusterContainer.storeHandle().get();
  for (xAOD::CaloCluster* cl : *outClusterContainer) {
    cl->setLink(outClusterContainerCellLink.ptr(), sg);
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

  if (m_objectqualityTool.name()=="") return StatusCode::SUCCESS;

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
