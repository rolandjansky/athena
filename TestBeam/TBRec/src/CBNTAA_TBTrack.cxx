/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBRec/CBNTAA_TBTrack.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "TBEvent/TBEventInfo.h"
#include "TBEvent/TBTrack.h"
#include <fstream>
#include <string>
#include "PathResolver/PathResolver.h"


CBNTAA_TBTrack::CBNTAA_TBTrack(const std::string & name, ISvcLocator * pSvcLocator) :
  CBNT_TBRecBase(name, pSvcLocator),
  m_readFileforXcryo(true),
  m_nRun(0),
  m_beam_coor_x(0),
  m_beam_coor_y(0),
  m_beam_chi2_x(0),
  m_beam_chi2_y(0),
  m_beam_intercept_x(0),
  m_beam_intercept_y(0),
  m_beam_slope_x(0),
  m_beam_slope_y(0),
  m_eventStore(0)
{
   declareProperty("ReadFileforXcryo", m_readFileforXcryo);
}

StatusCode CBNTAA_TBTrack::CBNT_initialize() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in initialize()"  << endreq;

  StatusCode sc = service("StoreGateSvc", m_eventStore);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to retrieve pointer to StoreGate Service" << endreq;
    return sc;
  }

  IToolSvc* toolSvc;
  sc=service("ToolSvc",toolSvc);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to retrieve ToolSvc" << endreq;
    return StatusCode::FAILURE;
  }

  StoreGateSvc* detStore;
  sc = service("DetectorStore", detStore);
  if (sc.isFailure()) {
    log << MSG::FATAL << " Cannot locate DetectorStore " << std::endl;
    return StatusCode::FAILURE;
  } 

  // Add beam track parameters
  addBranch("bm_x",      m_beam_coor_x,     "bm_x/f");
  addBranch("bm_y",      m_beam_coor_y,     "bm_y/f");
  addBranch("bm_chi2_x", m_beam_chi2_x,     "bm_chi2_x/f");
  addBranch("bm_chi2_y", m_beam_chi2_y,     "bm_chi2_y/f");
  addBranch("bm_x0",     m_beam_intercept_x,"bm_x0/f");
  addBranch("bm_y0",     m_beam_intercept_y,"bm_y0/f");
  addBranch("bm_slope_x",m_beam_slope_x,    "bm_slope_x/f");
  addBranch("bm_slope_y",m_beam_slope_y,    "bm_slope_y/f");

  return StatusCode::SUCCESS; 
}

StatusCode CBNTAA_TBTrack::CBNT_execute() {
  /// Print an informatory message:
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in execute()" << endreq;
  
  StatusCode sc;
  
  // Get xcryo and ytable from a file (by A. Minaenko)
  // Retrieve Event Info
  const TBEventInfo* theEventInfo;
  sc = m_eventStore->retrieve(theEventInfo,"TBEventInfo");
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Cannot retrieve TBEventInfo from StoreGate" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::VERBOSE << "retrieved theEventInfo" << endreq;
  }

  // Fill run header
  m_nRun = theEventInfo->getRunNum();
  float m_beamMom = theEventInfo->getBeamMomentum();
  float m_xCryo   = -9999;
  float m_yTable  = -9999;
  if(m_readFileforXcryo) {
    if (!this->getXcryoYtable(m_xCryo, m_yTable, m_beamMom)) {
      log << MSG::ERROR << "xCryo and yTable are not found" << endreq;
      return StatusCode::FAILURE;
    }
  } else {
     m_xCryo = theEventInfo->getCryoX();
     m_yTable = theEventInfo->getTableY();
  }
  log << MSG::VERBOSE << "m_xCryo   = " << m_xCryo   << endreq;
  log << MSG::VERBOSE << "m_yTable  = " << m_yTable  << endreq;
  log << MSG::VERBOSE << "m_nRun    = " << m_nRun    << endreq;
  log << MSG::VERBOSE << "m_beamMom = " << m_beamMom << endreq;

  // Get beam coordinates (by A. Mineanko)
  TBTrack *track;
  sc = m_eventStore->retrieve(track,"Track");
  if (sc.isFailure()){
    log << MSG::ERROR << "Retrieval of beam track failed" << endreq;
    return StatusCode::FAILURE;
  }

  float m_zCalo = 30000.; // z-coordinate of the calorimeter surface at which beam coordinates calculated
  m_beam_coor_x = track->getUslope()*m_zCalo + track->getUintercept() + m_xCryo;
  m_beam_coor_y = track->getVslope()*m_zCalo + track->getVintercept();
  m_beam_chi2_x = track->getChi2_u();
  m_beam_chi2_y = track->getChi2_v();
  m_beam_intercept_x = track->getUintercept();
  m_beam_intercept_y = track->getVintercept();
  m_beam_slope_x = track->getUslope();
  m_beam_slope_y = track->getVslope();
  log << MSG::VERBOSE << "m_beam_coor_x = "           << m_beam_coor_x          << endreq; // ok
  log << MSG::VERBOSE << "track->getUslope() = "      << track->getUslope()     << endreq; // ok
  log << MSG::VERBOSE << "m_zCalo =  "                << m_zCalo                << endreq; // ok
  log << MSG::VERBOSE << "track->getUintercept() =  " << track->getUintercept() << endreq; // ok
  log << MSG::VERBOSE << "m_xCryo = "                 << m_xCryo                << endreq; // ok
  log << MSG::VERBOSE << "m_beam_coor_y = "           << m_beam_coor_y          << endreq; // ok
  log << MSG::VERBOSE << "track->getVslope() = "      << track->getVslope()     << endreq; // ok
  log << MSG::VERBOSE << "m_beam_intercept_x = "      << m_beam_intercept_x     << endreq; // ok
  log << MSG::VERBOSE << "m_beam_intercept_y = "      << m_beam_intercept_y     << endreq; // ok
  
  return StatusCode::SUCCESS;
}

StatusCode CBNTAA_TBTrack::CBNT_finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in finalize()" << endreq;
  return StatusCode::SUCCESS;
}

std::string CBNTAA_TBTrack::add_name(const char* base, const std::string extension) {
  std::string retval(base);
  for (unsigned i=0;i<extension.size();i++) {
    const char& ch=extension[i];
    if (ch=='=' || ch==':' || ch=='/')
      continue; //Ignore these characters
    else if (ch=='-')
      retval.append("m");//replace by letter m
    else if (ch=='+')
      retval.append("p");//replace by letter p
    else
      retval.append(1,ch);
  }
  return retval;
}

// by A. Minaenko (necessary for TBTrack information)
StatusCode CBNTAA_TBTrack::getXcryoYtable(float &x, float &y, float &e) {

  // not good to put here (just a workaround) - joh
  std::string m_txtFileWithXY = "xcryo_ytable.txt";

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
  while ( getline(xyFile, line, '\n') ) {
    int run;
    std::istringstream buf(line);
    e = 0;
    buf >> run >> x >> y >> e;
    log << MSG::DEBUG << "m_nRun,run,x,y,e= "<<m_nRun<<" "<<run<<" "<<x<<" "<<y<<" "<<e<<endreq;
    if (run==m_nRun && xyFile.good()) return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}
