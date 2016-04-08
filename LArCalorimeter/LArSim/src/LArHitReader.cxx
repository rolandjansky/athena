/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSim/LArHitReader.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "LArSimEvent/LArHit.h"
#include "StoreGate/StoreGate.h"

// Constructor
LArHitReader::LArHitReader(const std::string& name, ISvcLocator* pSvcLocator ) :
 AthAlgorithm(name , pSvcLocator)
{
  
}


//Initialize method
StatusCode LArHitReader::initialize(){
  ATH_MSG_DEBUG("LArHitReader "<< this->name()<<" initialized");
  return StatusCode::SUCCESS ;   
}


//Execute method
StatusCode LArHitReader::execute(){
  const LArHitContainer* cont = nullptr;
  ATH_CHECK( StoreGate::instance().retrieve(cont,"LArHitFake") );

  ATH_MSG_DEBUG(" LArHitContainer Dump ----------- " );
  ATH_MSG_DEBUG( (std::string)(*cont)   );

  return StatusCode::SUCCESS ;   
}

//Finalize method
StatusCode LArHitReader::finalize(){
  ATH_MSG_DEBUG("finalize() completed correctly");
  return StatusCode::SUCCESS ;   
}


