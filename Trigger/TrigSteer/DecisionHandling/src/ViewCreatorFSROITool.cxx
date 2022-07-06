
/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "ViewCreatorFSROITool.h"

using namespace TrigCompositeUtils;

ViewCreatorFSROITool::ViewCreatorFSROITool(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent) 
  {}


StatusCode ViewCreatorFSROITool::initialize()  {
  ATH_CHECK( m_roisWriteHandleKey.initialize() );

  if ( !m_roiupdater.empty() ) ATH_CHECK( m_roiupdater.retrieve() );

  return StatusCode::SUCCESS;
}


StatusCode ViewCreatorFSROITool::attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext& ctx) const {

  SG::WriteHandle<TrigRoiDescriptorCollection> roisWriteHandle = createAndStoreNoAux(m_roisWriteHandleKey, ctx);

  if ( m_roiupdater.empty() ) { 
    roisWriteHandle->push_back( new TrigRoiDescriptor( RoiDescriptor::FULLSCAN ) );
  }
  else {
    roisWriteHandle->push_back( m_roiupdater->execute( ctx ) );
  }

  const ElementLink<TrigRoiDescriptorCollection> roiEL = ElementLink<TrigRoiDescriptorCollection>( *roisWriteHandle, /*index =*/ 0, ctx );

  for ( Decision* outputDecision : decisions ) { 
    outputDecision->setObjectLink( roiString(), roiEL );
  }

  return StatusCode::SUCCESS;
}
