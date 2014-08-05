/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class TBXCryYTableRead 

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

#include "PathResolver/PathResolver.h"

#include "StoreGate/StoreGate.h" 

#define private public
#include "TBEvent/TBEventInfo.h"
#undef private

#include "TBRec/TBXCryYTableRead.h"

#include <fstream>

TBXCryYTableRead::TBXCryYTableRead(const std::string& name, 
				   ISvcLocator* pSvcLocator) : 
  Algorithm(name, pSvcLocator),
  m_nEvent(0),
  m_nEventRandomTrigger(0),
  m_first(true),
  m_nRun(0),
  m_beamMom(0),
  m_xCryo(0),
  m_yTable(0),
  m_txtFileWithXY("xcryo_ytable.txt"),
  m_eventinfo(0),
  m_eventStore(0)
{ 
  declareProperty("FileName",m_txtFileWithXY);
}

TBXCryYTableRead::~TBXCryYTableRead()
{ }

StatusCode TBXCryYTableRead::initialize()
{
  
  MsgStream log(messageService(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  // Get StoreGateSvc //
  StatusCode sc = service ( "StoreGateSvc" , m_eventStore ) ;  
  if( sc.isFailure() ) {
    log<<MSG::FATAL<<" Cannot locate StoreGateSvc " << endreq ;
    sc = StatusCode::FAILURE ;
    return sc ;
  }

  return StatusCode::SUCCESS;
}


StatusCode TBXCryYTableRead::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode TBXCryYTableRead::execute()
{
  m_nEvent++;
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "Executing TBXCryYTableRead " << endreq;

  StatusCode sc;
  // Retrieve Event Info from file
  const TBEventInfo* theEventInfo;
  sc = m_eventStore->retrieve(theEventInfo,"TBEventInfo");
  if ( sc.isFailure() ) {
    log << MSG::ERROR
	<< "Cannot retrieve TBEventInfo from StoreGate" << endreq;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  // Do first event initialization (run header filling)
  if (m_first) {
    m_first = false;
    // Fill run header
    m_nRun = theEventInfo->getRunNum();
    m_beamMom = theEventInfo->getBeamMomentum();
    log << MSG::DEBUG << "Run, mom. from EventInfo: "<<m_nRun<<","<<m_beamMom<<endreq;
    // Get xcryo and ytable from a file
    float xFile, yFile, eFile;
    getXcryoYtable(xFile, yFile, eFile);
    if(m_beamMom != eFile) {
       log << MSG::WARNING << "Energy from file: "<<eFile<<" is different than from bytestream: "<<m_beamMom<<"  !!!"<<endreq;
       log << MSG::WARNING << "Using value from file !!!"<<endreq;
       m_beamMom = eFile;
    }
    m_xCryo = xFile;
    m_yTable = yFile;
  }  

  // Now change the TBEventInfo
  // unfortunatelly no copy constructor....
//  m_eventinfo = new TBEventInfo(theEventInfo->getEventNum(), theEventInfo->getEventClock(),
//                                theEventInfo->getEventType(), m_nRun, m_beamMom, 
//                                theEventInfo->getBeamParticle(), m_xCryo, theEventInfo->getCryoAngle(),
//                                m_yTable);
  log << MSG::DEBUG << "Filling TBEvent info with cryoX,tableY: "<<m_xCryo<<","<<m_yTable<<endreq;
  (const_cast<TBEventInfo*>(theEventInfo))->m_cryoX = m_xCryo;
  (const_cast<TBEventInfo*>(theEventInfo))->m_tableY = m_yTable;
  // in case if energy was different, change also this one
  (const_cast<TBEventInfo*>(theEventInfo))->m_beam_moment = m_beamMom;
  
  //sc = m_eventStore->record(m_eventinfo,"TBEventInfo");
  //if ( sc.isFailure( ) ) {
  //   log << MSG::FATAL << "Cannot record new TBEventInfo "<< endreq;
  //   setFilterPassed(false);
  //   return StatusCode::SUCCESS;
  //}
  setFilterPassed(true);
  return StatusCode::SUCCESS;

}

StatusCode TBXCryYTableRead::getXcryoYtable(float &x, float &y, float &e) {

  MsgStream log(messageService(),name());
  log << MSG::DEBUG << "in getXcryoYtable(float x, float y)" << endreq;

  std::ifstream xyFile;
  std::string line;
  std::string filename = PathResolver::find_file(m_txtFileWithXY, "DATAPATH");
  xyFile.open(filename.c_str());
  if (!xyFile.is_open()) {
    log << MSG::ERROR << "File " << m_txtFileWithXY << " fail to open in $DATAPATH" 
	<< endreq;
    return StatusCode::FAILURE;
  }

  log << MSG::DEBUG << "Asking for run: "<<m_nRun<<endreq;
  while ( getline(xyFile, line, '\n') ) {
    int run;
    std::istringstream buf(line);
    e = 0;
    buf >> run >> x >> y >> e;
    log << MSG::DEBUG << "run,x,y,e= "<<run<<" "<<x<<" "<<y<<" "<<e<<endreq;
    if (run == m_nRun && xyFile.good()) return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}
