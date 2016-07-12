/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//MuonCalibEventBase
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"

//MdrCalibRt
#include "MdtCalibRt/RtCalibrationOutput.h"

//this 
#include "CalibNtupleAnalysisAlg/CalibNtupleAnalysisAlg2.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"
#include "MuonCalibStandAloneBase/MdtStationT0Container.h"
#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"

//MdtCalibIOSvc
#include "MdtCalibIOSvc/MdtCalibOutputDbSvc.h"
#include "MdtCalibIOSvc/MdtCalibInputSvc.h"

namespace MuonCalib {

CalibNtupleAnalysisAlg2::CalibNtupleAnalysisAlg2(const std::string &name, ISvcLocator *pSvcLocator) : AthAlgorithm(name, pSvcLocator),  
m_calib_tool_handle("MuonCalib::NtupleRunScanTool"), m_num_segments(-1), m_dbservice("MdtCalibOutputDbSvc", name), 
m_calib_input_svc("MdtCalibInputSvc", name), p_calib_tool(NULL), m_crash_if_no_segments(false), m_eventnumber(0) {
  declareProperty("CalibrationTool", m_calib_tool_handle);
  declareProperty("CalibSegmentPreparationTools", m_seg_prep_tool_handles);
  declareProperty("NumberOfSegments", m_num_segments);
  declareProperty("CrashIfNoSegments", m_crash_if_no_segments);
  declareProperty("MdtCalibOutputDbSvc", m_dbservice);
  declareProperty("MdtCalibInputSvc", m_calib_input_svc);
}

CalibNtupleAnalysisAlg2::~CalibNtupleAnalysisAlg2() {
}

StatusCode CalibNtupleAnalysisAlg2::initialize() {
//greet user
//  ATH_MSG_INFO( "Thank you for using CalibNtupleAnalysisAlg2!" );
//get MdtCalibOutptSvc
  ATH_CHECK( m_dbservice.retrieve() );
  ATH_MSG_INFO( "Retrieved MdtCalibOutputDbSvc");
//get MdtCalibInputSvc
  ATH_CHECK( m_calib_input_svc.retrieve() );
  ATH_MSG_INFO( "Retrieved MdtCalibInputSvc");
//create the segment preparation tools
  ATH_CHECK( m_seg_prep_tool_handles.retrieve() );
  for (unsigned int k=0; k<m_seg_prep_tool_handles.size(); k++) {
    ATH_MSG_INFO( m_seg_prep_tool_handles[k].type() << " retrieved" );
  }
  ATH_CHECK( m_calib_tool_handle.retrieve(p_calib_tool) );
  ATH_MSG_INFO( "Retrieved Tool " << m_calib_tool_handle.type() );
  
  return StatusCode::SUCCESS;
}   //end CalibNtupleAnalysisAlg2::initialize

StatusCode CalibNtupleAnalysisAlg2::execute() {
//////////////////////////////////////////////
//run calibration segment preparation tools //
//////////////////////////////////////////////
  const MuonCalibEvent *event(NULL);
  std::map<NtupleStationId, MuonCalibSegment *> segments;
  m_eventnumber = 0;
  while(1) {
// Print number of segments every 1k events for first 20k events, then every 10k to avoid maxing out number of allowed messages
    if( (m_eventnumber < 20000 && m_eventnumber%1000 == 0) || m_eventnumber%10000 == 0 ) {
      ATH_MSG_INFO( m_eventnumber << " events read, " << m_stored_segment.size() << " segments collected" );
    }
    if (static_cast<int>(m_stored_segment.size()) >= m_num_segments && m_num_segments>=0) {
      ATH_MSG_INFO( "Enough segments collected!" );
      return StatusCode::SUCCESS;
    }
    segments.clear();
    for (unsigned int k=0; k<m_seg_prep_tool_handles.size(); k++) {
      m_seg_prep_tool_handles[k]->prepareSegments(event, segments);
      if(event==NULL) {
	return StatusCode::SUCCESS;
      }
    }
    int current_pos = m_stored_segment.size();
    for(std::map<NtupleStationId, MuonCalibSegment *>::const_iterator it=segments.begin(); it!= segments.end(); it++) {
      if(p_calib_tool->KeepSegments()) {
	m_stored_segment.push_back(new MuonCalibSegment(*(it->second)));
      } else {
	m_stored_segment.push_back(it->second);
      }
    }
    ATH_CHECK( p_calib_tool->handleEvent(*event, m_eventnumber, m_stored_segment, current_pos) );
    m_eventnumber++;
  }
  return StatusCode::SUCCESS;
}  //end  CalibNtupleAnalysisAlg2::execute

StatusCode CalibNtupleAnalysisAlg2::finalize() {
  if(m_stored_segment.size()==0) {
    if(m_crash_if_no_segments) {
      ATH_MSG_FATAL("No Segments found!");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_WARNING("No Segments found!");
      return StatusCode::SUCCESS;
    }			
  }
// Final tally of events/segments
  ATH_MSG_INFO( m_eventnumber << " events read, " << m_stored_segment.size() << " segments collected" );

  ATH_CHECK( p_calib_tool->analyseSegments(m_stored_segment) );
//special sausage for resolution fitters
  const IRtResolution *resolution(p_calib_tool->getResolution());
  if(resolution != NULL) {
    ATH_MSG_INFO( "Storing resolution calibration!" );
    const IRtRelation *rt_rel(m_calib_input_svc->GetRtRelation());
    if(rt_rel == NULL) {
      ATH_MSG_FATAL( "Cannot store a resolution without an rt-relation!" );
      return StatusCode::FAILURE;
    }
    RtCalibrationOutput *output(new RtCalibrationOutput(rt_rel, new RtFullInfo(std::string("Resolution"), 1, m_stored_segment.size(), 0.0, 0.0, 0.0, 0.0)));
    m_dbservice->memorize(output, resolution);
  } else {
    ATH_MSG_INFO( "Storing calibration!" );
    const IMdtCalibrationOutput *calib_res(p_calib_tool->getResults());
    if(calib_res == NULL) {
      ATH_MSG_WARNING( "Calibration Tool returned NULL-Pointer to Calibration Results!" );
      return StatusCode::SUCCESS;
    }
    if(! m_dbservice.empty() ) {
      m_dbservice->memorize(p_calib_tool->getResults());
    }
  }
//  ATH_MSG_INFO( "Bye!" );
//	m_calibration_programme->endOfData();
  return StatusCode::SUCCESS;
}  //end CalibNtupleAnalysisAlg2::finalize

} //namespace MuonCalib
