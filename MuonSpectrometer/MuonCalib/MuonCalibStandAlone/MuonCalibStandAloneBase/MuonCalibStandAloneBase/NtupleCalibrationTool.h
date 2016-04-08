/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _NtupleCalibrationTool_H
#define _NtupleCalibrationTool_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

//c - c++
#include <string>
#include <vector>

namespace MuonCalib {

//calib

class MuonCalibEvent;
class MuonCalibSegment;
class IMdtCalibrationOutput;
class NtupleStationId;
class IRtResolution;
/** @class NtupleCalibrationTool 
	Interface class for an interrface trool for calibration algorithms
	@author rauscher@cern.ch
	@date 20070508
	*/

static const InterfaceID IID_NtupleCalibrationTool("MuonCalib::NtupleCalibrationTool", 1, 0);

class NtupleCalibrationTool: virtual public IAlgTool
	{
	public:
	/** destructor */
		inline virtual ~NtupleCalibrationTool()
			{
			}
	/** interface method */
		static const InterfaceID& interfaceID() { return IID_NtupleCalibrationTool;}	
	/** is called one per event, when the event is read*/
		inline virtual StatusCode handleEvent(const MuonCalibEvent &/*event*/, int /*evnt_nr*/, const std::vector<MuonCalibSegment *> &/*segments*/, unsigned int /*position*/)
			{
			return StatusCode :: SUCCESS;
			}
	/** is called at the end of the analysis */
		inline virtual StatusCode analyseSegments(const std::vector<MuonCalibSegment *> & /*segemnts*/)
			{
			return StatusCode :: SUCCESS;
			}
	/** get calibration results */
		inline virtual const IMdtCalibrationOutput * getResults() const
			{
			return NULL;
			}
	/** get resolution */
		inline virtual const IRtResolution * getResolution() const
			{
			return NULL;
			}
	/** return true if ther segment vector in analyseSegments is needed. If
	this function returns false, the segments are not stored in the segment
	vector */
		inline virtual bool KeepSegments() const
			{
			return true;
			}
	};

}

#endif 
