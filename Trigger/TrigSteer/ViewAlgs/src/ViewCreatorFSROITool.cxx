
/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "ViewCreatorFSROITool.h"

using namespace TrigCompositeUtils;

ViewCreatorFSROITool::ViewCreatorFSROITool(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent) 
  {}


StatusCode ViewCreatorFSROITool::initialize()  {
  ATH_CHECK( m_roisWriteHandleKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode ViewCreatorFSROITool::attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext& ctx) const {
  SG::WriteHandle<TrigRoiDescriptorCollection> roisWriteHandle = createAndStoreNoAux(m_roisWriteHandleKey, ctx);
  roisWriteHandle->push_back( new TrigRoiDescriptor(true) );
  const ElementLink<TrigRoiDescriptorCollection> roiEL = ElementLink<TrigRoiDescriptorCollection>(*roisWriteHandle, /*index =*/ 0, ctx);

  for ( Decision* outputDecision : decisions ) { 
    outputDecision->setObjectLink(roiString(), roiEL);
  }

	return StatusCode::SUCCESS;
}
