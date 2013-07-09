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
  ISvcLocator* pSvcLocator) :Algorithm(name,pSvcLocator),
   m_log(msgSvc(),name),
   p_detstore(0)
{
  // declare properties
}

ReadTopTrans::~ReadTopTrans() {}

StatusCode ReadTopTrans::initialize()
{
  m_log << MSG::INFO << "ReadTopTrans::initialize() called" << endreq;

  if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
    m_log << MSG::FATAL << "Detector store not found" << endreq;
    return StatusCode::FAILURE;
  }
  // register IOV callback function
  const DataHandle<DetCondKeyTrans> aptr;
  if (StatusCode::SUCCESS==p_detstore->regFcn(&ReadTopTrans::testCallBack,
      this,aptr,"/GLOBAL/GEOM/TOPTRANS")) {
    m_log << MSG::INFO << "Registered IOV callback from ReadTopTrans" <<
      endreq;
  } else {
    m_log << MSG::ERROR << "Registration of IOV callback failed" << endreq;
  }

  // register incident handler for begin run
  IIncidentSvc* incsvc;
  if (StatusCode::SUCCESS!=service("IncidentSvc",incsvc))
    m_log << MSG::FATAL << "Incident service not found" << endreq;
  long int pri=100;
  incsvc->addListener(this,"BeginRun",pri);

  return StatusCode::SUCCESS;
}

StatusCode ReadTopTrans::execute() {
  m_log << MSG::INFO << 
    "In ReadTopTrans::execute - retrieve TOPTRANS from algorithm" << endreq;
  const DetCondKeyTrans* dtp;
  if (StatusCode::SUCCESS==p_detstore->retrieve(dtp,"/GLOBAL/GEOM/TOPTRANS")) {
    m_log << MSG::INFO << "Retrieve of TOPTRANS succeeded" << endreq;
    dtp->print();
    // get a specific transform
    HepGeom::Transform3D mytrans;
    if (dtp->getTransform("test1",mytrans)) {
      CLHEP::Hep3Vector transl=mytrans.getTranslation();
      m_log << MSG::INFO <<" X Y Z of test1 are " << transl.x() << "," <<
	transl.y() << "," << transl.z() << endreq;
    } else {
      m_log << MSG::ERROR << "Key test1 not found!" << endreq;
    }
  } else {
    m_log << MSG::ERROR << "Retrieve of TOPTRANS failed" << endreq;
  }
  return StatusCode::SUCCESS;
}

StatusCode ReadTopTrans::finalize() {
  m_log << MSG::INFO << "In ReadTopTrans::finalize" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode ReadTopTrans::testCallBack( IOVSVC_CALLBACK_ARGS_P( /* I */, 
							       /* keys */ ) ) {
  m_log << MSG::INFO << "ReadTopTrans callback invoked for keys:";
  // find and print the object
  m_log << MSG::INFO << "Attempting to retrieve TOPTRANS from IOV callback" <<
    endreq;
  const DetCondKeyTrans* dtp;
  if (StatusCode::SUCCESS==p_detstore->retrieve(dtp,"/GLOBAL/GEOM/TOPTRANS")) {
    dtp->print();
  } else {
    m_log << MSG::ERROR << "Retrieve of TOPTRANS in IOV callback failed" 
	  << endreq;
  }

  return StatusCode::SUCCESS;
}

void ReadTopTrans::handle(const Incident& inc) {
  if (inc.type()!="BeginRun") return;
  m_log << MSG::INFO << "In beginrun incident" << endreq;
  // find TOPTRANS object from begin run incident
  const DetCondKeyTrans* dtp;
  if (StatusCode::SUCCESS==p_detstore->retrieve(dtp,"/GLOBAL/GEOM/TOPTRANS")) {
    m_log << MSG::INFO << "Retrieve of TOPTRANS succeeded" << endreq;
    dtp->print();
  } else {
    m_log << MSG::ERROR << "Retrieve of TOPTRANS failed" << endreq;
  }
}
