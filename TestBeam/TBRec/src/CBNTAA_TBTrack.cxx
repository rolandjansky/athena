/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBRec/CBNTAA_TBTrack.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/INTupleSvc.h"

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
  m_beam_slope_y(0)
{
   declareProperty("ReadFileforXcryo", m_readFileforXcryo);
}

StatusCode CBNTAA_TBTrack::CBNT_initialize() {
  ATH_MSG_DEBUG ( "in initialize()"  );
  
  IToolSvc* toolSvc = nullptr;
  ATH_CHECK( service("ToolSvc",toolSvc) );

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
  ATH_MSG_DEBUG ( "in execute()" );
  
  // Get xcryo and ytable from a file (by A. Minaenko)
  // Retrieve Event Info
  const TBEventInfo* theEventInfo = nullptr;
  ATH_CHECK( evtStore()->retrieve(theEventInfo,"TBEventInfo") );

  // Fill run header
  m_nRun = theEventInfo->getRunNum();
  float beamMom = theEventInfo->getBeamMomentum();
  float xCryo   = -9999;
  float yTable  = -9999;
  if(m_readFileforXcryo) {
    if (!this->getXcryoYtable(xCryo, yTable, beamMom)) {
      ATH_MSG_ERROR ( "xCryo and yTable are not found" );
      return StatusCode::FAILURE;
    }
  } else {
     xCryo = theEventInfo->getCryoX();
     yTable = theEventInfo->getTableY();
  }
  ATH_MSG_VERBOSE ( "xCryo   = " << xCryo   );
  ATH_MSG_VERBOSE ( "yTable  = " << yTable  );
  ATH_MSG_VERBOSE ( "m_nRun    = " << m_nRun    );
  ATH_MSG_VERBOSE ( "beamMom = " << beamMom );

  // Get beam coordinates (by A. Mineanko)
  TBTrack *track = nullptr;
  ATH_CHECK( evtStore()->retrieve(track,"Track") );

  float zCalo = 30000.; // z-coordinate of the calorimeter surface at which beam coordinates calculated
  m_beam_coor_x = track->getUslope()*zCalo + track->getUintercept() + xCryo;
  m_beam_coor_y = track->getVslope()*zCalo + track->getVintercept();
  m_beam_chi2_x = track->getChi2_u();
  m_beam_chi2_y = track->getChi2_v();
  m_beam_intercept_x = track->getUintercept();
  m_beam_intercept_y = track->getVintercept();
  m_beam_slope_x = track->getUslope();
  m_beam_slope_y = track->getVslope();
  ATH_MSG_VERBOSE ( "m_beam_coor_x = "           << m_beam_coor_x          );
  ATH_MSG_VERBOSE ( "track->getUslope() = "      << track->getUslope()     );
  ATH_MSG_VERBOSE ( "zCalo =  "                  << zCalo                  );
  ATH_MSG_VERBOSE ( "track->getUintercept() =  " << track->getUintercept() );
  ATH_MSG_VERBOSE ( "xCryo = "                   << xCryo                  );
  ATH_MSG_VERBOSE ( "m_beam_coor_y = "           << m_beam_coor_y          );
  ATH_MSG_VERBOSE ( "track->getVslope() = "      << track->getVslope()     );
  ATH_MSG_VERBOSE ( "m_beam_intercept_x = "      << m_beam_intercept_x     );
  ATH_MSG_VERBOSE ( "m_beam_intercept_y = "      << m_beam_intercept_y     );
  
  return StatusCode::SUCCESS;
}

StatusCode CBNTAA_TBTrack::CBNT_finalize() {
  ATH_MSG_DEBUG ( "in finalize()" );
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
  std::string txtFileWithXY = "xcryo_ytable.txt";

  ATH_MSG_DEBUG ( "in getXcryoYtable(float x, float y)" );
  std::ifstream xyFile;
  std::string line;
  std::string filename = PathResolver::find_file(txtFileWithXY, "DATAPATH");
  xyFile.open(filename.c_str());
  if (!xyFile.is_open()) {
    ATH_MSG_ERROR ( "File " << txtFileWithXY << " fail to open in $DATAPATH");
    return StatusCode::FAILURE;
  }
  while ( getline(xyFile, line, '\n') ) {
    int run;
    std::istringstream buf(line);
    e = 0;
    buf >> run >> x >> y >> e;
    ATH_MSG_DEBUG ( "m_nRun,run,x,y,e= "<<m_nRun<<" "<<run<<" "<<x<<" "<<y<<" "<<e);
    if (run==m_nRun && xyFile.good()) return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}
