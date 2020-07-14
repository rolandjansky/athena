
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
    const std::vector<LinkInfo<xAOD::TrigEMClusterContainer>> myCluster = findLinks<xAOD::TrigEMClusterContainer>(outputDecision, m_clusterLinkName, TrigDefs::lastFeatureOfType);
    
    if (myCluster.size() != 1) {
      ATH_MSG_ERROR("Did not find exactly one most-recent xAOD::TrigEMCluster '" << m_clusterLinkName << "' for Decision object index " << outputDecision->index()
        << ", found " << myCluster.size());
      return StatusCode::FAILURE;
    }
    ATH_CHECK(myCluster.at(0).isValid());

    const ElementLink<xAOD::TrigEMClusterContainer> cEL = myCluster.at(0).link;

    const double reta  = (*cEL)->eta();
    const double retap = reta + m_roiEtaWidth;
    const double retam = reta - m_roiEtaWidth;
    const double rphi  = (*cEL)->phi();
    const double rphip = rphi + m_roiPhiWidth;
    const double rphim = rphi - m_roiPhiWidth;

    ATH_MSG_DEBUG( "New ROI for xAOD::TrigEMCluster ET="<< (*cEL)->et() 
      << " eta="<< (*cEL)->eta() << " +- " << m_roiEtaWidth 
      << " phi="<< (*cEL)->phi() << " +- " << m_roiPhiWidth );

    roisWriteHandle->push_back( new TrigRoiDescriptor(reta, retam, retap, rphi, rphim, rphip) );
    const ElementLink<TrigRoiDescriptorCollection> roiEL = ElementLink<TrigRoiDescriptorCollection>(*roisWriteHandle, roisWriteHandle->size() - 1, ctx);

    outputDecision->setObjectLink(roiString(), roiEL);
  }

	return StatusCode::SUCCESS;
}
