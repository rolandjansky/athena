/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "MuonCalibStandAloneBase/CalibrationTeeIOTool.h"

namespace MuonCalib {

CalibrationTeeIOTool :: CalibrationTeeIOTool(const std::string& t, const std::string& n, const IInterface* p): AthAlgTool(t, n, p), m_tool1("MuonCalib::CalibrationDummyIOTool"), m_tool2("MuonCalib::CalibrationDummyIOTool")
	{
	declareInterface< CalibrationIOTool >(this);
	declareProperty("IOTool1", m_tool1);
	declareProperty("IOTool2", m_tool2);
	}



StatusCode CalibrationTeeIOTool :: initialize()
	{
	StatusCode sc=m_tool1.retrieve();
	if(!sc.isSuccess())
		{
		return sc;
		}
	sc=m_tool2.retrieve();
	return sc;
	}


StatusCode CalibrationTeeIOTool :: WriteT0(MdtTubeFitContainer * t0_output, const NtupleStationId & station_id, int iov_start, int iov_end)
	{

	ATH_MSG_INFO("Writing to tool 1");
	StatusCode sc=m_tool1->WriteT0(t0_output, station_id, iov_start, iov_end);
	
	if(!sc.isSuccess())
		return sc;
	ATH_MSG_INFO("Writing to tool 2");
	return m_tool2->WriteT0(t0_output, station_id, iov_start, iov_end);
	}


StatusCode CalibrationTeeIOTool :: WriteRt(const RtCalibrationOutput * rt_relation, const IRtResolution * resolution, const NtupleStationId & station_id, int iov_start, int iov_end, bool real_rt, bool real_resolution)
	{
	StatusCode sc=m_tool1->WriteRt(rt_relation, resolution, station_id, iov_start, iov_end, real_rt, real_resolution);
	if(!sc.isSuccess())
		return sc;
	return m_tool2->WriteRt(rt_relation, resolution, station_id, iov_start, iov_end, real_rt, real_resolution);
	}
	
	
StatusCode CalibrationTeeIOTool :: LoadT0(std::map<NtupleStationId, MdtStationT0Container *> &t0s, int iov_id)
	{
	return m_tool1->LoadT0(t0s, iov_id);
	}
	
StatusCode CalibrationTeeIOTool :: LoadRt(std::map<NtupleStationId, IRtRelation *> & rts, std::map<NtupleStationId, IRtResolution *> & res, int iov_id)
	{
	return m_tool1->LoadRt(rts, res, iov_id);
	}
	
}
