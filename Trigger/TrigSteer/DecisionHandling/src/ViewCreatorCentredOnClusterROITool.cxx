
/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthViews/ViewHelper.h"
#include "ViewCreatorCentredOnClusterROITool.h"

using namespace TrigCompositeUtils;

ViewCreatorCentredOnClusterROITool::ViewCreatorCentredOnClusterROITool(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent) 
  {}


StatusCode ViewCreatorCentredOnClusterROITool::initialize()  {
  ATH_CHECK(m_roisWriteHandleKey.initialize());
  return StatusCode::SUCCESS;
}


StatusCode ViewCreatorCentredOnClusterROITool::attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext& ctx) const {
  SG::WriteHandle<TrigRoiDescriptorCollection> roisWriteHandle = createAndStoreNoAux(m_roisWriteHandleKey, ctx);
  
  for ( Decision* outputDecision : decisions ) { 
    const std::vector<LinkInfo<xAOD::TrigEMClusterContainer>> myClusters = findLinks<xAOD::TrigEMClusterContainer>(outputDecision, m_clusterLinkName, TrigDefs::lastFeatureOfType);
    
    if (myClusters.size() == 0) {
      ATH_MSG_ERROR("Found zero xAOD::TrigEMClusters '" << m_clusterLinkName << "' for Decision object index " << outputDecision->index());
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("This merged Decision Object has " << myClusters.size() << " TrigEMClusters in its history from a previous Step."
      << " AllowMultipleClusters=" << (m_allowMultipleClusters ? "TRUE" : "FALSE"));

    // In NOT m_allowMultipleClusters mode, we centre on the highest et
    size_t singleClusterIndex = 0;
    if (!m_allowMultipleClusters && myClusters.size() > 1) {
      float maxEt = 0;
      for (const LinkInfo<xAOD::TrigEMClusterContainer>& myCluster : myClusters) {
        ATH_CHECK(myCluster.isValid());
        ATH_MSG_VERBOSE("- The TrigEMCluster at index " << myCluster.link.index() << " has Et " << (*myCluster.link)->et() << " MeV.");
        if ((*myCluster.link)->et() > maxEt) {
          maxEt = (*myCluster.link)->et();
          singleClusterIndex = myCluster.link.index();
        }
      }
      ATH_MSG_DEBUG("The TrigEMCluster at index " << singleClusterIndex << " will be used as the centre of the FTF ROI, as this one has the highest ET (" << maxEt << " MeV).");
    }

    // In m_allowMultipleClusters mode, they are all collected into a SuperRoI
    if (m_allowMultipleClusters) {
      TrigRoiDescriptor* superRoI = new TrigRoiDescriptor();
      superRoI->setComposite(true);
      superRoI->manageConstituents(true);
      roisWriteHandle->push_back(superRoI);
    }

    for (const LinkInfo<xAOD::TrigEMClusterContainer>& myCluster : myClusters) {
      ATH_CHECK(myCluster.isValid());
      if (!m_allowMultipleClusters && myCluster.link.index() != singleClusterIndex) {
        continue;
      }

      const xAOD::TrigEMCluster* c = *(myCluster.link);

      const double reta  = c->eta();
      const double retap = reta + m_roiEtaWidth;
      const double retam = reta - m_roiEtaWidth;
      const double rphi  = c->phi();
      const double rphip = rphi + m_roiPhiWidth;
      const double rphim = rphi - m_roiPhiWidth;

      ATH_MSG_DEBUG( "  New ROI for xAOD::TrigEMCluster ET="<< c->et() 
        << " eta="<< c->eta() << " +- " << m_roiEtaWidth 
        << " phi="<< c->phi() << " +- " << m_roiPhiWidth );

      std::unique_ptr<TrigRoiDescriptor> roi = std::make_unique<TrigRoiDescriptor>(reta, retam, retap, rphi, rphim, rphip);
      if (m_allowMultipleClusters) {
        roisWriteHandle->back()->push_back( roi.release() ); // Note: manageConstituents=true, superRoI will handle deletion of roi
      } else {
        roisWriteHandle->push_back( roi.release() ); // Note: roi now owned by roisWriteHandle
      }
    }

    // Link this ROI into the navigation. It will be used to spawn the Event View for this Decision Object.
    const ElementLink<TrigRoiDescriptorCollection> roiEL = ElementLink<TrigRoiDescriptorCollection>(*roisWriteHandle, roisWriteHandle->size() - 1, ctx);
    outputDecision->setObjectLink(roiString(), roiEL);

  }

	return StatusCode::SUCCESS;
}
