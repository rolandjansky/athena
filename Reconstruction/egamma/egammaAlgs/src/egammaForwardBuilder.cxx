/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaForwardBuilder.h"
#include "egammaInterfaces/IegammaBaseTool.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/Electron.h"

#include "EgammaAnalysisInterfaces/IAsgForwardElectronIsEMSelector.h"
#include "PATCore/AcceptData.h"

#include <algorithm>
#include <cmath>

egammaForwardBuilder::egammaForwardBuilder(const std::string& name,
                                           ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{}

egammaForwardBuilder::~egammaForwardBuilder()
{
}

StatusCode egammaForwardBuilder::initialize()
{
  // the data handle keys
  ATH_CHECK(m_topoClusterKey.initialize());
  ATH_CHECK(m_electronOutputKey.initialize());
  ATH_CHECK(m_outClusterContainerKey.initialize());
  m_outClusterContainerCellLinkKey = m_outClusterContainerKey.key() + "_links";
  ATH_CHECK(m_outClusterContainerCellLinkKey.initialize());
  ATH_CHECK(m_caloDetDescrMgrKey.initialize());

  // retrieve object quality tool
  if (!m_objectQualityTool.empty()) {
    ATH_CHECK(m_objectQualityTool.retrieve());
  } else {
    m_objectQualityTool.disable();
  }

  // retrieve 4-mom builder:
  if (!m_fourMomBuilder.empty()) {
    ATH_CHECK(m_fourMomBuilder.retrieve());
  } else {
    m_fourMomBuilder.disable();
  }

  ATH_CHECK(m_forwardElectronIsEMSelectors.retrieve());

  if (m_forwardElectronIsEMSelectors.size() !=
      m_forwardElectronIsEMSelectorResultNames.size()) {
    ATH_MSG_ERROR("The number of selectors does not match the number of given "
                  "fwd-electron selector names");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Initialization completed successfully");

  return StatusCode::SUCCESS;
}

StatusCode egammaForwardBuilder::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode egammaForwardBuilder::execute(const EventContext& ctx) const
{

  // create an egamma container and register it
  SG::WriteHandle<xAOD::ElectronContainer> xaodFrwd(m_electronOutputKey, ctx);
  ATH_CHECK(xaodFrwd.record(std::make_unique<xAOD::ElectronContainer>(),
                            std::make_unique<xAOD::ElectronAuxContainer>()));

  // Create the relevant cluster output and register it
  SG::WriteHandle<xAOD::CaloClusterContainer> outClusterContainer(
    m_outClusterContainerKey, ctx);
  ATH_CHECK(outClusterContainer.record(
    std::make_unique<xAOD::CaloClusterContainer>(),
    std::make_unique<xAOD::CaloClusterAuxContainer>()));

  SG::WriteHandle<CaloClusterCellLinkContainer> outClusterContainerCellLink(
    m_outClusterContainerCellLinkKey, ctx);
  ATH_CHECK(outClusterContainerCellLink.record(
    std::make_unique<CaloClusterCellLinkContainer>()));

  // Topo cluster Container
  SG::ReadHandle<xAOD::CaloClusterContainer> inputClusters(m_topoClusterKey, ctx);

  // check is only used for serial running; remove when MT scheduler used
  if(!inputClusters.isValid()) {
    ATH_MSG_FATAL("egammaForwardBuilder::Could not retrieve Cluster container");
    return StatusCode::FAILURE;
  }
  // retrieve CaloDetDescr
  SG::ReadCondHandle<CaloDetDescrManager> caloDetDescrMgrHandle { m_caloDetDescrMgrKey, ctx };
  ATH_CHECK(caloDetDescrMgrHandle.isValid());

  const CaloDetDescrManager* calodetdescrmgr = *caloDetDescrMgrHandle;

  // loop over input cluster container and create fwd electrons
  xAOD::CaloClusterContainer::const_iterator clus_begin =
    inputClusters->begin();
  xAOD::CaloClusterContainer::const_iterator clus_end = inputClusters->end();
  static const SG::AuxElement::Accessor<
    std::vector<ElementLink<xAOD::CaloClusterContainer>>>
    caloClusterLinks("constituentClusterLinks");
  
  size_t origClusterIndex = 0;
  for (; clus_begin!=clus_end; ++clus_begin,++origClusterIndex) {
 
    //Preselectcion cuts
    if((*clus_begin)->et() < m_ETcut||
       std::abs((*clus_begin)->eta())<m_etacut){
      continue;
    }

    //Create a new electron
    xAOD::Electron* el = new xAOD::Electron();
    xaodFrwd->push_back(el);
    el->setAuthor( xAOD::EgammaParameters::AuthorFwdElectron );

    //Deep copy of topo cluster as might want to own modify it 
    xAOD::CaloCluster *newCluster = new xAOD::CaloCluster(**clus_begin);
    outClusterContainer->push_back(newCluster);  

    //set links back to the original caloCalTopoCluster we copied from
    std::vector<ElementLink<xAOD::CaloClusterContainer>> constituentLinks;
    constituentLinks.emplace_back(*inputClusters, origClusterIndex, ctx);
    caloClusterLinks(*newCluster) = constituentLinks;

    //Now attach the copied cluster to the electron
    int index = outClusterContainer->size() - 1;
    ElementLink<xAOD::CaloClusterContainer> newclusterElementLink(
      *outClusterContainer, index, ctx);
    std::vector< ElementLink< xAOD::CaloClusterContainer > > linksToClusters;
    linksToClusters.push_back(newclusterElementLink);
    el->setCaloClusterLinks(linksToClusters);

    // do  Four Momentum
    ATH_CHECK(m_fourMomBuilder->execute(ctx, el));

    // do object quality
    ATH_CHECK(ExecObjectQualityTool(ctx, calodetdescrmgr, el));

    // Apply the Forward Electron selectors
    size_t size = m_forwardElectronIsEMSelectors.size();

    for (size_t i = 0; i < size; ++i) {
      asg::AcceptData accept =
        m_forwardElectronIsEMSelectors[i]->accept(ctx, el);
      // save the bool result
      el->setPassSelection(static_cast<bool>(accept),
                           m_forwardElectronIsEMSelectorResultNames[i]);
      // save the isem
      el->setSelectionisEM(accept.getCutResultInverted(),
                           "isEM" +
                             m_forwardElectronIsEMSelectorResultNames[i]);
    }
  }

  // Now finalize the cluster: based on code in CaloClusterStoreHelper::finalizeClusters
  for (xAOD::CaloCluster* cl : *outClusterContainer) {
    cl->setLink(outClusterContainerCellLink.ptr(), ctx);
  }

  return StatusCode::SUCCESS;
}

StatusCode
egammaForwardBuilder::ExecObjectQualityTool(
  const EventContext& ctx,
  const CaloDetDescrManager* calodetdescrmgr,
  xAOD::Egamma* eg) const
{
  //
  // execution of the object quality tools
  //
  // protection in case tool is not available
  // return success as algorithm can run without it
  if (m_objectQualityTool.name().empty()) return StatusCode::SUCCESS;
  // execute the tool
  return m_objectQualityTool->execute(ctx, *calodetdescrmgr,*eg);
}
