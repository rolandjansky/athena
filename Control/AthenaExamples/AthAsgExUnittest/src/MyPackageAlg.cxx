
//
//  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
//

// MyPackage includes
#include "MyPackageAlg.h"

MyPackageAlg::MyPackageAlg( const std::string& name, 
			    ISvcLocator* pSvcLocator ) : 
  AthAnalysisAlgorithm( name, pSvcLocator ),
  m_property( 1 ),
  m_tool( "MyPackageTool/MyTool", this ) {
  addRef(); // workaround until fix in Gaudi
  // example property declarations
  declareProperty( "MyProperty", m_property ); 
  declareProperty( "MyTool", m_tool );
}


MyPackageAlg::~MyPackageAlg() {}


StatusCode MyPackageAlg::initialize() {
  ATH_MSG_INFO( "Initializing " << name() << "..." );
  ATH_MSG_INFO( "MyProperty = " << m_property );
  CHECK(m_tool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode MyPackageAlg::finalize() {
  ATH_MSG_INFO( "Finalizing " << name() << "..." );
  return StatusCode::SUCCESS;
}

StatusCode MyPackageAlg::execute() {  
  ATH_MSG_DEBUG( "Executing " << name() << "..." );
  setFilterPassed(false); //optional: start with algorithm not passed

  // Real algorithm here

  setFilterPassed(true); //if got here, assume that means algorithm passed
  return StatusCode::SUCCESS;
}

