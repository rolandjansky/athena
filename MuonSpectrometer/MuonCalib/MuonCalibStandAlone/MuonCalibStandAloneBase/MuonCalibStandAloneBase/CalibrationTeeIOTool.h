/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CalibrationTeeIOTool_H
#define CalibrationTeeIOTool_H

#include "MuonCalibStandAloneBase/CalibrationIOTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace MuonCalib{

/** @class CalibrationTeeIOTool
Wites to two calibtaion IO Tools. Reads from the first one.
*/

class CalibrationTeeIOTool : public AthAlgTool, virtual public CalibrationIOTool
	{
	public:
	/** constructor*/
		CalibrationTeeIOTool(const std::string& t, const std::string& n, const IInterface* p);
	/** initialize function */
		StatusCode initialize();
	/** interface functions */	
		StatusCode WriteT0(MdtTubeFitContainer * t0_output, const NtupleStationId & station_id, int iov_start, int iov_end);
		StatusCode WriteRt(const RtCalibrationOutput * rt_relation, const IRtResolution * resolution, const NtupleStationId & station_id, int iov_start, int iov_end, bool real_rt, bool real_resolution);
		StatusCode LoadT0(std::map<NtupleStationId, MdtStationT0Container *> &t0s, int iov_id);
		StatusCode LoadRt(std::map<NtupleStationId, IRtRelation *> & rts, std::map<NtupleStationId, IRtResolution *> & res, int iov_id);
	private:
	//! two calibration IO Tools
		ToolHandle<CalibrationIOTool> m_tool1{this,"IOTool1","MuonCalib::CalibrationDummyIOTool"};
		ToolHandle<CalibrationIOTool> m_tool2{this,"IOTool2","MuonCalib::CalibrationDummyIOTool"};
					
	};

}
#endif
