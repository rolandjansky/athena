/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NtupleIntegrationTool_H
#define NtupleIntegrationTool_H

//this
#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

//root
class TFile;

namespace MuonCalib {
	
class RtCalibrationIntegration;

/** @class NtupleIntegrationTool
	Interface tool for the Integration Method for finding an initial r-t 
	relation.
	@author rauscher@cern.ch
*/


class NtupleIntegrationTool : public AthAlgTool, virtual public NtupleCalibrationTool
	{
	public:
//==============================================================================
	/** Tool Constructor */
		NtupleIntegrationTool(const std::string& t,
				      const std::string& n,
				      const IInterface* p);
	/** Tool initialization */
		StatusCode initialize();
	/** Tool finalization */
		StatusCode finalize();
	/**analyse event - fill hits into specra*/
		StatusCode handleEvent(const MuonCalibEvent &/*event*/, int /*evnt_nr*/, const std::vector<MuonCalibSegment *> &segments, unsigned int position);
	/** end of events analysis */
		StatusCode analyseSegments(const std::vector<MuonCalibSegment *> & /*segemnts*/);
	/** get calibration results */
		const IMdtCalibrationOutput * getResults() const;
	/** we dont need to keep the segments*/
		inline bool KeepSegments() const
			{
			return false;
			}
//==============================================================================
	private:
	//! pointer to algoritm
		RtCalibrationIntegration *m_integration;
	//! tmax for integration - job option
		double m_t_max_int;
	//! is true if close hits are filled - job option
		bool m_has_close_hits;
	//! inner tube radius - job option
		double m_inner_tube_radius;
	//! lower radius for parabolic extrapolation at the upper end of the r-t
	        double m_lower_extrapolation_radius;
	//! higher radius for parabolic extrapolation at the upper end of the r-t
	        double m_upper_extrapolation_radius;
	//! write multilayer t_max-difference
		bool m_multilayer_tmax_diff;	
	//! root file
		TFile *p_file;
	};

#endif
	
}
