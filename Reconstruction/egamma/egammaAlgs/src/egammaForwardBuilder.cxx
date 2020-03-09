/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaForwardBuilder.h"
#include "egammaInterfaces/IegammaBaseTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/Electron.h"
 
#include "EgammaAnalysisInterfaces/IAsgForwardElectronIsEMSelector.h"
#include "PATCore/AcceptData.h"

#include <algorithm> 
#include <math.h>


//  END OF HEADER FILES INCLUDE

/////////////////////////////////////////////////////////////////

//  CONSTRUCTOR:
    
egammaForwardBuilder::egammaForwardBuilder(const std::string& name, ISvcLocator* pSvcLocator): 
  AthReentrantAlgorithm(name, pSvcLocator),
  m_timingProfile("ChronoStatSvc", name)
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
  if (!m_objectQualityTool.empty()) {
    ATH_CHECK(m_objectQualityTool.retrieve());
  } else {
    ATH_MSG_DEBUG("egammaOQFlagsBuilder is disabled");
    m_objectQualityTool.disable();
  }

  // retrieve 4-mom builder:
  if (!m_fourMomBuilder.empty()) {
    ATH_CHECK(m_fourMomBuilder.retrieve());
  } else {
    m_fourMomBuilder.disable();
  }

  ATH_CHECK(m_forwardElectronIsEMSelectors.retrieve());
  
  if (m_forwardElectronIsEMSelectors.size() != m_forwardElectronIsEMSelectorResultNames.size()) {
    ATH_MSG_ERROR("The number of selectors does not match the number of given fwd-electron selector names");
    return StatusCode::FAILURE;
  }
  
  if (m_doChrono) ATH_CHECK( m_timingProfile.retrieve() );

  ATH_MSG_DEBUG("Initialization completed successfully");

  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode egammaForwardBuilder::finalize()
{
  return StatusCode::SUCCESS;
}

// ======================================================================
StatusCode egammaForwardBuilder::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("Executing egammaForwardBuilder ");

  // create an egamma container and register it
  SG::WriteHandle<xAOD::ElectronContainer> xaodFrwd(m_electronOutputKey, ctx);
  ATH_CHECK(xaodFrwd.record(std::make_unique<xAOD::ElectronContainer>(),
			    std::make_unique<xAOD::ElectronAuxContainer>()));

  ATH_MSG_DEBUG( "Recorded Electrons with key: " << m_electronOutputKey.key() );

  //cluster
  SG::WriteHandle<xAOD::CaloClusterContainer> outClusterContainer(m_outClusterContainerKey, ctx);
  ATH_CHECK(outClusterContainer.record(std::make_unique<xAOD::CaloClusterContainer>(),
					  std::make_unique<xAOD::CaloClusterAuxContainer>()));
  SG::WriteHandle<CaloClusterCellLinkContainer> outClusterContainerCellLink(m_outClusterContainerCellLinkKey, ctx);
  ATH_CHECK(outClusterContainerCellLink.record(std::make_unique<CaloClusterCellLinkContainer>()));

  //Topo cluster Container
  SG::ReadHandle<xAOD::CaloClusterContainer> cluster(m_topoClusterKey, ctx);

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
    ATH_CHECK(m_fourMomBuilder->execute(ctx, el));
    
    // do object quality 
    ATH_CHECK( ExecObjectQualityTool(ctx, el) );
    
  
    // FwdSelectors:
    size_t size = m_forwardElectronIsEMSelectors.size();
    
    for (size_t i = 0; i<size;++i) {
      asg::AcceptData accept = m_forwardElectronIsEMSelectors[i]->accept(ctx, el);
      //save the bool result
      el->setPassSelection(static_cast<bool>(accept), m_forwardElectronIsEMSelectorResultNames[i]);
      //save the isem
      el->setSelectionisEM(accept.getCutResultInverted(), "isEM"+m_forwardElectronIsEMSelectorResultNames[i]);
      
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
StatusCode egammaForwardBuilder::ExecObjectQualityTool(const EventContext& ctx, xAOD::Egamma *eg) const
{
  //
  // execution of the object quality tools
  //
  // protection in case tool is not available
  // return success as algorithm may be able to run without it 
  // in degraded mode

  if (m_objectQualityTool.name().empty()) return StatusCode::SUCCESS;

  // setup chrono for this tool
  std::string chronoName=this->name()+"_"+m_objectQualityTool->name() ;
  if (m_doChrono) m_timingProfile->chronoStart(chronoName);

  // execute the tool
  StatusCode sc = m_objectQualityTool->execute(ctx, eg);
  if ( sc.isFailure() ) {
    ATH_MSG_DEBUG("failure returned by object quality tool"); 
  }
  if (m_doChrono) m_timingProfile->chronoStop(chronoName);

  return sc;
}
