/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////
#include "MuonCondTest/AlignCondAthTest.h"
#include "MuonCondSvc/MuonAlignmentDbSvc.h"


AlignCondAthTest::AlignCondAthTest(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator){
   p_MuonAlignmentDbSvc              = 0; 

}
 
StatusCode AlignCondAthTest::initialize(){
//

  ATH_MSG_INFO( "in initialize()"  );
  ATH_CHECK( service("MuonCalib::MuonAlignmentDbSvc",p_MuonAlignmentDbSvc ) );
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
  
