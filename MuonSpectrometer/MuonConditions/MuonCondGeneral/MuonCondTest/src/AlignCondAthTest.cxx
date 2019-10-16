/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////
#include "MuonCondTest/AlignCondAthTest.h"

AlignCondAthTest::AlignCondAthTest(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator){
}
 
StatusCode AlignCondAthTest::initialize(){
//

  ATH_MSG_INFO( "in initialize()"  );
  return StatusCode::SUCCESS;
//
}
 
StatusCode AlignCondAthTest::execute() {
//

  ATH_MSG_INFO( " AlignCondAthTest in execute()"  );

  return StatusCode::SUCCESS;
//
}
 
StatusCode AlignCondAthTest::finalize() {
//

  ATH_MSG_INFO( "in finalize()"  );
//
  return StatusCode::SUCCESS;
//
}
  
