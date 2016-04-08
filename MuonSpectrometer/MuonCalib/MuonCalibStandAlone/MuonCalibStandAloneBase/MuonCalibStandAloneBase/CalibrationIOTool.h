/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _CalibrationIOTool_H
#define _CalibrationIOTool_H

//Gaudi
#include "GaudiKernel/IAlgTool.h"

//c - c++
#include "map"

//this
#include "MuonCalibStandAloneBase/NtupleStationId.h"

namespace MuonCalib {

class T0CalibrationOutput;
class RtCalibrationOutput;
class IRtResolution;
class MdtStationT0Container;
class IRtRelation;
class MdtTubeFitContainer;

static const InterfaceID IID_CalibrationIOTool("MuonCalib::CalibrationIOTool", 1, 0);

/** @class CalibrationIOTool 
Interface for a toool writing out calibration

@author rauscher@cern.ch
*/


class CalibrationIOTool: virtual public IAlgTool
	{
	public:
	/** destructor */
		inline virtual ~CalibrationIOTool()
			{
			}
	/** interface method */
		static const InterfaceID& interfaceID() { return IID_CalibrationIOTool;}
	/** write out t0
	    @param t0_output t0 data
	    @param station_id The station/multilayer identifier
	    @param iov_start start of the interval of validity
	    @param iov_end end of the interval of validity
	*/
		inline virtual StatusCode WriteT0(MdtTubeFitContainer * /*t0_output*/, const NtupleStationId & /*station_id*/, int /*iov_start*/, int /*iov_end*/)
			{
			return StatusCode::SUCCESS;
			}
	/** write out rt relation 
	    @param rt_relation the rt relation
	    @param resolution the rt_resolution
	    @param station_id THe station identifier
	    @param iov_start start of the interval of validity
	    @param iov_end end of the interval of validity
	 */
	 	inline virtual StatusCode WriteRt(const RtCalibrationOutput * /*rt_relation*/, const IRtResolution * /*resolution*/, const NtupleStationId & /*station_id*/, int /*iov_start*/, int /*iov_end*/, bool /*real_rt*/, bool /*real resolution*/)
			{
			return StatusCode::SUCCESS;
			}
	/** load t0s for iov 
	    @param t0s map to be filled with the t0 data
	    @param iov_id the iov identifier, e.g. the run number. Note: This will, at the moment alwas be -1 (invalid). So the tool will have to determine the run number by itself (job option)
	*/
		inline virtual StatusCode LoadT0(std::map<NtupleStationId, MdtStationT0Container *> &t0s, int /*iov_id*/)
			{
			t0s.clear();
			return StatusCode::SUCCESS;
			}
	/** load rts for iov
	    @param rts map to be filled with rts
	    @param res map to be filled with resolution
	    @param iov_id the iov identifier, e.g. the run number. Note: This will, at the moment alwas be -1 (invalid). So the tool will have to determine the run number by itself (job option)
	*/
	    	inline virtual StatusCode LoadRt(std::map<NtupleStationId, IRtRelation *> & rts, std::map<NtupleStationId, IRtResolution *> &res, int /*iov_id*/)
			{
			rts.clear();
			res.clear();
			return StatusCode::SUCCESS;
			}			
	};

}

#endif
