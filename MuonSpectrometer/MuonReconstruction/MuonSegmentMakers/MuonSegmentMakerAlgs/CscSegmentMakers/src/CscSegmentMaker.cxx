/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CscSegmentMaker.h"
#include <sstream>
#include "xAODEventInfo/EventInfo.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"

using Muon::CscPrepData;
using Muon::CscPrepDataCollection;
using Muon::CscPrepDataContainer;

/*namespace {

  std::string station_name(int station) {
    if ( station == 1 ) return "CSS";
    if ( station == 2 ) return "CSL";
    return "UNKNOWN_STATION";
  }
  
  std::string measphi_name(bool measphi) {
    if ( measphi ) return "phi";
    return "eta";
  }
  
  // Convert chamber identifier to string.
  std::string chamber(int istation, int zsec, int phi) {
    std::ostringstream ssout;
    if ( istation == 1 ) ssout << "CSS";
    else if ( istation ==2 ) ssout << "CSL";
    else ssout << "???";
    if ( zsec == -1 ) ssout << "-";
    else if ( zsec == 1 ) ssout << "+";
    else ssout << "?";
    ssout << phi;
    return ssout.str();
  }
  
}*/

//******************************************************************************

CscSegmentMaker::
CscSegmentMaker(const std::string& aname, ISvcLocator* pSvcLocator)
  : AthAlgorithm(aname, pSvcLocator),
    m_dumped(0), m_dump(false), 
    m_2dseg_finder(""),
    m_4dseg_finder(""),
    m_psegs(std::unique_ptr<MuonSegmentCombinationCollection>()),
    m_psegs4d(std::unique_ptr<MuonSegmentCombinationCollection>())
{
  declareProperty("dump_count", m_dumpcount =5);
  declareProperty("sg_inkey", m_sg_inkey = "");
  declareProperty("sg_2d_outkey", m_sg_outkey = "");
  declareProperty("sg_4d_outkey", m_sg_4d_outkey = "");
  declareProperty("segfinder_2d", m_2dseg_finder);
  declareProperty("segfinder_4d", m_4dseg_finder);
}

//******************************************************************************

// Destructor.

CscSegmentMaker::~CscSegmentMaker() {
}

//******************************************************************************

StatusCode CscSegmentMaker::initialize(){

  ATH_MSG_INFO ( "Initializing " << name() );

  ATH_MSG_INFO ( "  Dump count: " << m_dumpcount );
  // Show keys.
  ATH_MSG_INFO ( "  Input SG key: " << m_sg_inkey );
  ATH_MSG_INFO ( "  Output SG key for 2D Segment: " << m_sg_outkey );
  ATH_MSG_INFO ( "  Output SG key for 4D Segment: " << m_sg_4d_outkey );

  ATH_MSG_INFO ( "  2D Segment Finder is " << m_2dseg_finder.typeAndName() );
  ATH_MSG_INFO ( "  4D Segment Finder is " << m_4dseg_finder.typeAndName() );
  
  
  // Retrieve the Segment Finder tool.
  if ( m_2dseg_finder.retrieve().isFailure() ) {
    ATH_MSG_FATAL ( "Unable to retrieve 2dseg_finder tool "
                    << m_2dseg_finder );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "Retrieved 2dseg finder tool "
           << m_2dseg_finder );
  }
  
  // Retrieve the Segment Finder tool.
  if ( m_4dseg_finder.retrieve().isFailure() ) {
    ATH_MSG_FATAL ( "Unable to retrieve 4dseg_finder tool "
                    << m_4dseg_finder );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "Retrieved 4dseg finder tool "
           << m_4dseg_finder );
  }
  
  
  return StatusCode::SUCCESS;
}

//******************************************************************************

StatusCode CscSegmentMaker::execute(){

  m_dump = m_dumped < m_dumpcount || m_dumpcount < 0;
  if ( m_dump ) ++m_dumped;

  StatusCode sc = StatusCode::SUCCESS;
  // Get event info.
  const xAOD::EventInfo* pevt = 0;
  StatusCode sc_ev = evtStore()->retrieve(pevt, "");
  int evt = pevt->eventNumber();
  int run = pevt->runNumber();
  if ( m_dump )
    ATH_MSG_DEBUG ( "Processing run " << run
           << ", event " << evt );
     
  ///////////
  // Build Segment
  sc = build_segments();
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( " Failed to build segments " );
  }

  sc = evtStore()->record(m_psegs.release(), m_sg_outkey);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( " Cannot record CSC segment collection " << m_sg_outkey );
  } else {
    if ( m_dump )
      ATH_MSG_DEBUG ( "    Created " << m_psegs->size() << " 2D segments and " );
  }
  
  sc = evtStore()->record(m_psegs4d.release(), m_sg_4d_outkey);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( " Cannot record CSC segment collection " << m_sg_4d_outkey );
  } else {
    if ( m_dump )
      ATH_MSG_DEBUG ( m_psegs4d->size() << " 4D segments."  );
  }
  
  return StatusCode::SUCCESS;

}

//******************************************************************************
StatusCode CscSegmentMaker::build_segments() {
  // Retrieve the container.
  const DataHandle<CscPrepDataContainer> pcols;
  StatusCode status = evtStore()->retrieve(pcols, m_sg_inkey);

  if ( m_dump )
    ATH_MSG_DEBUG ( "Retrieved " << pcols->size()
        << " CSC cluster collections.");

  //  Chambers incols;
  if ( status.isSuccess() ) {
    std::vector<const CscPrepDataCollection*> myclus;
    for ( CscPrepDataContainer::const_iterator icol=pcols->begin();
          icol!=pcols->end(); ++icol ) 
      myclus.push_back(*icol);

    ATH_MSG_DEBUG ("CscSegmentMaker called find!!");
    m_psegs = m_2dseg_finder->find(myclus);
    // Construct output segment collection.
    ATH_MSG_DEBUG ("CscSegmentMaker called find!!");
    if (m_psegs) {
      if (m_psegs->size() > 1) {
        m_psegs4d = m_4dseg_finder->find(*m_psegs);
      }
    }
  }
  return StatusCode::SUCCESS;
}

//******************************************************************************

StatusCode CscSegmentMaker::finalize() {
  ATH_MSG_DEBUG ( "Goodbye" );
  return StatusCode::SUCCESS;
}
 
//******************************************************************************
