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


CalibNtupleAnalysisAlg2 :: CalibNtupleAnalysisAlg2(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator),  m_calib_tool_handle("MuonCalib::NtupleRunScanTool"), m_num_segments(-1), p_dbservice(NULL), p_calib_input_svc(NULL), p_calib_tool(NULL), m_crash_if_no_segments(false), m_eventnumber(0)
	{
	declareProperty("CalibrationTool", m_calib_tool_handle);
	declareProperty("CalibSegmentPreparationTools", m_seg_prep_tool_handles);
	declareProperty("NumberOfSegments", m_num_segments);
	declareProperty("CrashIfNoSegments", m_crash_if_no_segments);
	}
	

CalibNtupleAnalysisAlg2 :: ~CalibNtupleAnalysisAlg2()
	{
	}

StatusCode CalibNtupleAnalysisAlg2 ::  initialize()
	{
//greet user
	MsgStream log(messageService(), name());
	log<< MSG::INFO << "Thank you for using CalibNtupleAnalysisAlg2!" <<endreq;
//get MdtCalibOutptSvc
	StatusCode sc;
	sc=service("MdtCalibOutputDbSvc", p_dbservice);
	if(!sc.isSuccess())
		{
		log << MSG::ERROR <<"Cannot retrieve MdtCalibOutputDbSvc!" <<endreq;
		return sc;
		}
	log<< MSG::INFO << "Retrieved MdtCalibOutputDbSvc"<<endreq;
//get MdtCalibInputSvc
	sc=service("MdtCalibInputSvc", p_calib_input_svc);
	if(!sc.isSuccess())
		{
		log << MSG::ERROR <<"Cannot retrieve MdtCalibInputSvc!" <<endreq;
		return sc;
		}
	log<< MSG::INFO << "Retrieved MdtCalibInputSvc"<<endreq;
//create the segment preparation tools
    sc = m_seg_prep_tool_handles.retrieve();
    if (sc.isFailure())
        {
        log << MSG::WARNING
            << "Could not find calibration segment preparation tools!"
            << " No segment preparation tools available."
            << endreq;
        }
    else
        {
        for (unsigned int k=0; k<m_seg_prep_tool_handles.size(); k++) {
            log << MSG::INFO << m_seg_prep_tool_handles[k].type()
                << " retrieved"
                << endreq;
        }
        }
	sc = m_calib_tool_handle.retrieve(p_calib_tool);
 	if (sc.isFailure() || p_calib_tool==NULL) 
		{
		log << MSG::FATAL << "Could not find tool " << m_calib_tool_handle.type() << ". Exiting."
	  << endreq;
	  	}
	else
		{
		log << MSG::INFO << "Retrieved Tool " << m_calib_tool_handle.type() << endreq;
		}
	
	
	return StatusCode::SUCCESS;
	}






StatusCode CalibNtupleAnalysisAlg2 :: execute()
	{
	MsgStream log(messageService(), name());
//////////////////////////////////////////////
//run calibration segment preparation tools //
//////////////////////////////////////////////
	const MuonCalibEvent *event(NULL);
	std::map<NtupleStationId, MuonCalibSegment *> segments;
	m_eventnumber = 0;
	while(1) 
		{
		
		if(m_eventnumber % 1000 == 0)
			{
			log << MSG::INFO << m_eventnumber << " events read, " << m_stored_segment.size()<< " segments collected" <<endreq;
			}
		if (static_cast<int>(m_stored_segment.size()) >= m_num_segments && m_num_segments>=0)
			{
			log << MSG::INFO << "Enough segemnts collected!" <<endreq;
			return StatusCode :: SUCCESS;
			}
		segments.clear();
	    	for (unsigned int k=0; k<m_seg_prep_tool_handles.size(); k++) 
			{
	       	 	m_seg_prep_tool_handles[k]->prepareSegments(event, segments);
			if(event==NULL)
				{
				return StatusCode :: SUCCESS;
				}
			}
		int current_pos = m_stored_segment.size();
		for(std::map<NtupleStationId, MuonCalibSegment *>::const_iterator it=segments.begin(); it!= segments.end(); it++)
			{
			if(p_calib_tool->KeepSegments())
				{
				m_stored_segment.push_back(new MuonCalibSegment(*(it->second)));
				}
			else
				{
				m_stored_segment.push_back(it->second);
				}
			}
		StatusCode sc = p_calib_tool->handleEvent(*event, m_eventnumber, m_stored_segment, current_pos);
		if(!sc.isSuccess())
			{
			log << MSG::FATAL << p_calib_tool->name()<<"Failed!"<<std::endl;
			return sc;
			}
		m_eventnumber++;
    		}
	return StatusCode :: SUCCESS;
	}


StatusCode  CalibNtupleAnalysisAlg2 :: finalize()
	{
	MsgStream log(messageService(), name());
	if(m_stored_segment.size()==0)
		{
		if(m_crash_if_no_segments)
			{
			log<<MSG::FATAL<<"No Segments found!"<<endreq;
			return StatusCode::FAILURE;
			}
		else
			{
			log<<MSG::WARNING<<"No Segments found!"<<endreq;
			return StatusCode::SUCCESS;
			}
			
		}
	StatusCode sc=p_calib_tool->analyseSegments(m_stored_segment);
	if(!sc.isSuccess())
		{
		return sc;
		}
//special sausage for resolution fitters
	const IRtResolution * resolution(p_calib_tool->getResolution());
	if(resolution != NULL)
		{
		log<< MSG::INFO <<"Storing resolution!"<<endreq;
		const IRtRelation *rt_rel(p_calib_input_svc->GetRtRelation());
		if(rt_rel == NULL)
			{
			log<< MSG::FATAL << "Cannot store a resolutino without an rt-relation!" <<endreq;
			return StatusCode::FAILURE;
			}
		RtCalibrationOutput *output(new RtCalibrationOutput(rt_rel, new RtFullInfo(std::string("Resolution"), 1, m_stored_segment.size(), 0.0, 0.0, 0.0, 0.0)));
		p_dbservice->memorize(output, resolution);
		}
	else {
		log<< MSG::INFO <<"Storing other calibration!"<<endreq;
		const IMdtCalibrationOutput *calib_res(p_calib_tool->getResults());
		if(calib_res == NULL)
			{
			log << MSG::WARNING << "Calibration Tool returned NULL-Pointer to Calibration Results!" <<endreq;
			return StatusCode :: SUCCESS;
			}
		if(p_dbservice != NULL) {
		p_dbservice->memorize(p_calib_tool->getResults());
		}
		}
	log<< MSG::INFO << "Bye!"<< endreq;
//	m_calibration_programme->endOfData();
	return StatusCode::SUCCESS;
	}


} //namespace MuonCalib
