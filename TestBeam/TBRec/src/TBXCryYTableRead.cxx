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
#include "TBEvent/TBEventInfo.h"
#include "TBRec/TBXCryYTableRead.h"
#include <fstream>

TBXCryYTableRead::TBXCryYTableRead(const std::string& name, 
				   ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_nEvent(0),
  m_nEventRandomTrigger(0),
  m_first(true),
  m_nRun(0),
  m_beamMom(0),
  m_xCryo(0),
  m_yTable(0),
  m_txtFileWithXY("xcryo_ytable.txt"),
  m_eventinfo(0)
{ 
  declareProperty("FileName",m_txtFileWithXY);
}

TBXCryYTableRead::~TBXCryYTableRead()
{ }

StatusCode TBXCryYTableRead::initialize()
{
  return StatusCode::SUCCESS;
}


StatusCode TBXCryYTableRead::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode TBXCryYTableRead::execute()
{
  m_nEvent++;
  ATH_MSG_DEBUG ( "Executing TBXCryYTableRead " );

  StatusCode sc;
  // Retrieve Event Info from file
  const TBEventInfo* theEventInfo = nullptr;
  sc = evtStore()->retrieve(theEventInfo,"TBEventInfo");
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR
      ( "Cannot retrieve TBEventInfo from StoreGate" );
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  // Do first event initialization (run header filling)
  if (m_first) {
    m_first = false;
    // Fill run header
    m_nRun = theEventInfo->getRunNum();
    m_beamMom = theEventInfo->getBeamMomentum();
    ATH_MSG_DEBUG ( "Run, mom. from EventInfo: "<<m_nRun<<","<<m_beamMom);
    // Get xcryo and ytable from a file
    float xFile, yFile, eFile;
    getXcryoYtable(xFile, yFile, eFile);
    if(m_beamMom != eFile) {
       ATH_MSG_WARNING ( "Energy from file: "<<eFile<<" is different than from bytestream: "<<m_beamMom<<"  !!!");
       ATH_MSG_WARNING ( "Using value from file !!!");
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
  ATH_MSG_DEBUG ( "Filling TBEvent info with cryoX,tableY: "<<m_xCryo<<","<<m_yTable);
  // FIXME: const violation!
  *const_cast<TBEventInfo*>(theEventInfo) =
    TBEventInfo (theEventInfo->getEventNum(),
                 theEventInfo->getEventClock(),
                 theEventInfo->getEventType(),
                 theEventInfo->getRunNum(),
                 m_beamMom,
                 theEventInfo->getBeamParticle(),
                 m_xCryo,
                 theEventInfo->getCryoAngle(),
                 m_yTable);
  
  //sc = m_eventStore->record(m_eventinfo,"TBEventInfo");
  //if ( sc.isFailure( ) ) {
  //   ATH_MSG_FATAL( "Cannot record new TBEventInfo " );
  //   setFilterPassed(false);
  //   return StatusCode::SUCCESS;
  //}
  setFilterPassed(true);
  return StatusCode::SUCCESS;

}

StatusCode TBXCryYTableRead::getXcryoYtable(float &x, float &y, float &e) {

  ATH_MSG_DEBUG ( "in getXcryoYtable(float x, float y)" );

  std::ifstream xyFile;
  std::string line;
  std::string filename = PathResolver::find_file(m_txtFileWithXY, "DATAPATH");
  xyFile.open(filename.c_str());
  if (!xyFile.is_open()) {
    ATH_MSG_ERROR ( "File " << m_txtFileWithXY << " fail to open in $DATAPATH");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG ( "Asking for run: "<<m_nRun);
  while ( getline(xyFile, line, '\n') ) {
    int run;
    std::istringstream buf(line);
    e = 0;
    buf >> run >> x >> y >> e;
    ATH_MSG_DEBUG ( "run,x,y,e= "<<run<<" "<<x<<" "<<y<<" "<<e);
    if (run == m_nRun && xyFile.good()) return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}
