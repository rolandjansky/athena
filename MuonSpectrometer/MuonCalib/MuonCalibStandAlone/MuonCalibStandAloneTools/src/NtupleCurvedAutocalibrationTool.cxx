/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//gaudi
#include "GaudiKernel/MsgStream.h"

//MdtCalibRt
#include "MdtCalibRt/RtCalibrationCurved.h"
#include "MdtCalibRt/RtCalibrationOutput.h"
//MdtCalibData
#include "MdtCalibData/RtFullInfo.h"

//this
#include "MuonCalibStandAloneTools/NtupleCurvedAutocalibrationTool.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MdtCalibIOSvc/MdtCalibInputSvc.h"

namespace MuonCalib {

NtupleCurvedAutocalibrationTool :: NtupleCurvedAutocalibrationTool(const std::string& t, const std::string& n, const IInterface* p): 
				AlgTool(t, n, p), 
				m_autocalibration(NULL), 
				m_rt_accuracy(0.5), 
				m_func_type("LEGENDRE"), 
				m_order(15),
				m_fix_min(true), 
				m_fix_max(false), 
				max_it(100),
				m_force_mono(false),
				m_control_histograms(false),
				m_failed(false),
				m_parabolic_extrapolation(false),
				m_smoothing(false),
				m_multilayer_rt_difference(false)
	{
	declareInterface< NtupleCalibrationTool >(this);
	declareProperty("Accuracy", m_rt_accuracy);
	declareProperty("FunctionType", m_func_type);
	declareProperty("FunctionOrder", m_order);
	declareProperty("FixMin", m_fix_min);
	declareProperty("FixMax", m_fix_max);
	declareProperty("MaximumIterations", max_it);
	declareProperty("ForceMonotonous", m_force_mono);
	declareProperty("ControlHistograms", m_control_histograms);
	declareProperty("ParabolicExtrapolation", m_parabolic_extrapolation);
	declareProperty("ConventionalSmoothing", m_smoothing);
	declareProperty("MultilayerRtScale", m_multilayer_rt_difference);
	}
	
StatusCode NtupleCurvedAutocalibrationTool :: initialize()
	{
	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "initialize()" <<endreq;
//interpret functino type
	if(m_func_type == "LEGENDRE")
		{
		m_func_type_num = 1;
		}
	else if (m_func_type == "CHEBYSHEV")
		{
		m_func_type_num = 2;
		}
	else if (m_func_type == "POLYGON")
		{
		m_func_type_num = 3;
		}
	else
		{
		log << MSG::FATAL << "Invalid function type '" << m_func_type << "'!" << endreq;
		return StatusCode ::FAILURE;
		}
	if(m_force_mono)
		{
		log<<MSG::INFO<<"Forcing monotonous rt-relation"<<endreq;
		}
//get pointer to Calibration input service
	StatusCode sc=service("MdtCalibInputSvc", p_calib_input_svc);
	if(!sc.isSuccess())
		{
		log << MSG::ERROR <<"Cannot retrieve MdtCalibInputSvc!" <<endreq;
		return sc;
		}
	setRegion();	
	return StatusCode :: SUCCESS;
	}

void NtupleCurvedAutocalibrationTool :: setRegion()
	{
	MsgStream log(msgSvc(), name());
//retrieve rt-relationid
	const IRtRelation *rt_relation(p_calib_input_svc->GetRtRelation());
	if(rt_relation == NULL)
		{
		log << MSG::FATAL << "No rt-relation stored for this region!" << endreq;
		return;
		}
//create objects
	log << MSG::INFO << "Creating RtCalibrationCurved!" <<endreq;
	m_autocalibration = new RtCalibrationCurved("RT", m_rt_accuracy,
                                        m_func_type_num, m_order,
                                        m_fix_min, m_fix_max, max_it,
                                        m_parabolic_extrapolation,
                                        m_smoothing,
					m_multilayer_rt_difference);
	log << MSG::INFO << "setInput!" <<endreq;
	m_autocalibration->setInput(new RtCalibrationOutput(rt_relation, new RtFullInfo()));
	if(m_force_mono)
		{
		m_autocalibration->forceMonotony();
		}
	if(m_control_histograms)
		{
		m_autocalibration-> switch_on_control_histograms("CurvedAutocalibration.root");
		}
	}

StatusCode NtupleCurvedAutocalibrationTool :: analyseSegments(const std::vector<MuonCalibSegment *> & segemnts)
	{
	MsgStream log(msgSvc(), name());
	if(m_autocalibration == NULL)
		{
		log << MSG::FATAL << "Algorithm not created!" << endreq;
		m_failed=true;
		return StatusCode::FAILURE;
		}
	if(m_autocalibration->analyseSegments(segemnts)==NULL)
		{
		m_autocalibration->switch_off_control_histograms();
		m_failed=true;
		return StatusCode :: FAILURE;
		}
	m_autocalibration->switch_off_control_histograms();
	return StatusCode::SUCCESS;
	}

const IMdtCalibrationOutput * NtupleCurvedAutocalibrationTool :: getResults() const
	{
	MsgStream log(msgSvc(), name());
	if(m_autocalibration == NULL)
		{
		log << MSG::FATAL << "Algorithm not created!" << endreq;
		return NULL;
		}
	if (m_failed) return NULL;
	return m_autocalibration->getResults();
	}

}
