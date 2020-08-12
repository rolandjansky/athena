/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavigationCnvTool.h"
xAODMaker::TrigNavigationCnvTool::TrigNavigationCnvTool( const std::string& type, const std::string& name,
							 const IInterface* parent ) 
  : AthAlgTool(type, name, parent) {
  declareInterface< ITrigNavigationCnvTool >( this );
}

xAODMaker::TrigNavigationCnvTool::~TrigNavigationCnvTool() {
}

StatusCode xAODMaker::TrigNavigationCnvTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode xAODMaker::TrigNavigationCnvTool::convert( const HLT::HLTResult* aod,
						      xAOD::TrigNavigation* xaod ) const {
  xaod->setSerialized(aod->getNavigationResult());
  return StatusCode::SUCCESS;
}
