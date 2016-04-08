/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ReadTopTrans.cxx - simple example of algorithm reading DetCondKeyTrans
// Richard Hawkings, started 24/6/05
//
// This algorithm accesses the /GLOBAL/GEOM/TOPTRANS object in the TDS
// via three methods
// - algorithm execute method
// - IOV svc call back
// - BeginRun incident


#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "DetDescrConditions/DetCondKeyTrans.h"
#include "DetDescrCondExample/ReadTopTrans.h"
#include "StoreGate/StoreGateSvc.h"

ReadTopTrans::ReadTopTrans(const std::string& name, 
  ISvcLocator* pSvcLocator) :AthAlgorithm(name,pSvcLocator) {}

ReadTopTrans::~ReadTopTrans() {}

StatusCode ReadTopTrans::initialize()
{
  ATH_MSG_INFO("ReadTopTrans::initialize() called");

  // register IOV callback function
  const DataHandle<DetCondKeyTrans> aptr;
  if (StatusCode::SUCCESS==detStore()->regFcn(&ReadTopTrans::testCallBack,
      this,aptr,"/GLOBAL/GEOM/TOPTRANS")) {
    ATH_MSG_INFO("Registered IOV callback from ReadTopTrans");
  } else {
    ATH_MSG_INFO("Registration of IOV callback failed");
  }

  // register incident handler for begin run
  IIncidentSvc* incsvc;
  if (StatusCode::SUCCESS!=service("IncidentSvc",incsvc))
    ATH_MSG_FATAL("Incident service not found");
  long int pri=100;
  incsvc->addListener(this,"BeginRun",pri);

  return StatusCode::SUCCESS;
}

StatusCode ReadTopTrans::execute() {
  ATH_MSG_INFO( "In ReadTopTrans::execute - retrieve TOPTRANS from algorithm");
  const DetCondKeyTrans* dtp;
  if (StatusCode::SUCCESS==detStore()->retrieve(dtp,"/GLOBAL/GEOM/TOPTRANS")) {
    ATH_MSG_INFO("Retrieve of TOPTRANS succeeded");
    dtp->print();
    // get a specific transform
    HepGeom::Transform3D mytrans;
    if (dtp->getTransform("test1",mytrans)) {
      CLHEP::Hep3Vector transl=mytrans.getTranslation();
      ATH_MSG_INFO(" X Y Z of test1 are " << transl.x() << "," <<
		   transl.y() << "," << transl.z());
    } else {
      ATH_MSG_ERROR("Key test1 not found!");
    }
  } else {
    ATH_MSG_ERROR("Retrieve of TOPTRANS failed");
  }
  return StatusCode::SUCCESS;
}

StatusCode ReadTopTrans::finalize() {
  ATH_MSG_INFO("In ReadTopTrans::finalize");
  return StatusCode::SUCCESS;
}

StatusCode ReadTopTrans::testCallBack( IOVSVC_CALLBACK_ARGS_P( /* I */, 
							       /* keys */ ) ) {
  ATH_MSG_INFO("ReadTopTrans callback invoked");
  // find and print the object
  ATH_MSG_INFO("Attempting to retrieve TOPTRANS from IOV callback");
  const DetCondKeyTrans* dtp;
  if (StatusCode::SUCCESS==detStore()->retrieve(dtp,"/GLOBAL/GEOM/TOPTRANS")) {
    dtp->print();
  } else {
    ATH_MSG_ERROR("Retrieve of TOPTRANS in IOV callback failed");
  }

  return StatusCode::SUCCESS;
}

void ReadTopTrans::handle(const Incident& inc) {
  if (inc.type()!="BeginRun") return;
  ATH_MSG_INFO("In beginrun incident");
  // find TOPTRANS object from begin run incident
  const DetCondKeyTrans* dtp;
  if (StatusCode::SUCCESS==detStore()->retrieve(dtp,"/GLOBAL/GEOM/TOPTRANS")) {
    ATH_MSG_INFO("Retrieve of TOPTRANS succeeded");
    dtp->print();
  } else {
    ATH_MSG_ERROR("Retrieve of TOPTRANS failed");
  }
}
