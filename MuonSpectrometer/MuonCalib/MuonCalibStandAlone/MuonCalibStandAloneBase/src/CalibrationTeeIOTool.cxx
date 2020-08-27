/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibStandAloneBase/CalibrationTeeIOTool.h"

namespace MuonCalib {

CalibrationTeeIOTool::CalibrationTeeIOTool(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t, n, p) {
    declareInterface< CalibrationIOTool >(this);
}

StatusCode CalibrationTeeIOTool::initialize() {
  ATH_CHECK(m_tool1.retrieve());
  ATH_CHECK(m_tool2.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode CalibrationTeeIOTool::WriteT0(MdtTubeFitContainer * t0_output, const NtupleStationId & station_id, int iov_start, int iov_end)
	{

	ATH_MSG_INFO("Writing to tool 1");
	StatusCode sc=m_tool1->WriteT0(t0_output, station_id, iov_start, iov_end);
	
	if(!sc.isSuccess())
		return sc;
	ATH_MSG_INFO("Writing to tool 2");
	return m_tool2->WriteT0(t0_output, station_id, iov_start, iov_end);
	}


StatusCode CalibrationTeeIOTool::WriteRt(const RtCalibrationOutput * rt_relation, const IRtResolution * resolution, const NtupleStationId & station_id, int iov_start, int iov_end, bool real_rt, bool real_resolution)
	{
	StatusCode sc=m_tool1->WriteRt(rt_relation, resolution, station_id, iov_start, iov_end, real_rt, real_resolution);
	if(!sc.isSuccess())
		return sc;
	return m_tool2->WriteRt(rt_relation, resolution, station_id, iov_start, iov_end, real_rt, real_resolution);
	}
	
	
StatusCode CalibrationTeeIOTool::LoadT0(std::map<NtupleStationId, MdtStationT0Container *> &t0s, int iov_id)
	{
	return m_tool1->LoadT0(t0s, iov_id);
	}
	
StatusCode CalibrationTeeIOTool::LoadRt(std::map<NtupleStationId, IRtRelation *> & rts, std::map<NtupleStationId, IRtResolution *> & res, int iov_id)
	{
	return m_tool1->LoadRt(rts, res, iov_id);
	}
	
}
