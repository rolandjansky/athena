
//
//  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
//

// MyPackage includes
#include "AthExUnittestAlg.h"

AthExUnittestAlg::AthExUnittestAlg( const std::string& name,
				    ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ),
  m_property( 1 ),
  m_tool( "AthExUnittestTool/MyTool", this ) {
  addRef(); // workaround until fix in Gaudi
  // example property declarations
  declareProperty( "MyProperty", m_property ); 
  declareProperty( "MyTool", m_tool );
}

AthExUnittestAlg::~AthExUnittestAlg() {}

StatusCode AthExUnittestAlg::initialize() {
  ATH_MSG_INFO( "Initializing " << name() << "..." );
  ATH_MSG_INFO( "MyProperty = " << m_property );
  CHECK(m_tool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode AthExUnittestAlg::finalize() {
  ATH_MSG_INFO( "Finalizing " << name() << "..." );
  return StatusCode::SUCCESS;
}

StatusCode AthExUnittestAlg::execute() {  
  ATH_MSG_DEBUG( "Executing " << name() << "..." );
  setFilterPassed(false); //optional: start with algorithm not passed

  // Real algorithm here

  setFilterPassed(true); //if got here, assume that means algorithm passed
  return StatusCode::SUCCESS;
}

StatusCode AthExUnittestAlg::beginInputFile() { 
  //This method is called at the start of each input file, even if
  //the input file contains no events. Accumulate metadata information here
  return StatusCode::SUCCESS;
}

