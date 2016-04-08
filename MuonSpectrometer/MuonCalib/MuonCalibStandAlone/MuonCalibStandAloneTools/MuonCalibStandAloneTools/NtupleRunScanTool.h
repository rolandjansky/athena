/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _NtupleRunScanTool_H
#define _NtupleRunScanTool_H

//c - c++
#include "map"

//this
#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibStandAloneTools/HitCounter.h"
#include "AthenaBaseComps/AthAlgTool.h"
//root
class TFile;


class MdtIdHelper;
namespace MuonGM {
class MuonDetectorManager;
}

namespace MuonCalib {


/** @class NtupleRunScanTool
Scans a run and checks for present chambers and number of hits
@author rauscher@cern
*/

class NtupleRunScanTool:  public AthAlgTool, virtual public NtupleCalibrationTool
	{
	public:
//==============================================================================
	/** Tool Constructor*/
		NtupleRunScanTool(const std::string& t, const std::string& n, const IInterface* p);
	/** tool initialization */
		StatusCode initialize();
	/** tool finalization */
		inline StatusCode finalize()
			{
			return StatusCode::SUCCESS;
			}
	/**analyse event - fill hits into specra*/
		StatusCode handleEvent(const MuonCalibEvent &event, int /*evnt_nr*/, const std::vector<MuonCalibSegment *> &segments, unsigned int position);
	/** end of events analysis - segemnts are not used here*/
		StatusCode analyseSegments(const std::vector<MuonCalibSegment *> & /*segemnts*/);
	/** we dont need to keep the segments*/
		inline bool KeepSegments() const
			{
			return false;
			}
//==============================================================================
	private:
	//! minimum number of hits per drift time spectrum - job option
		int m_min_hits;
	//! maximum rate of bad fits - job option
		double m_max_bad_fits;
	//! suppress output for chambers which should not be fitted
		bool m_suppress_nofit;
	//! noisy tube factor: A tube is considered noisy, if n_hits > <n_hits> * m_noisy_tube_factor * n_tubes
		int m_noisy_tube_factor;
	//! hit counter classes - sortet by station
		std::map<NtupleStationId, HitCounter> m_hit_counters;
	//!access to geomodel
		const MdtIdHelper* m_mdtIdHelper;
		const MuonGM::MuonDetectorManager* m_detMgr;
	//!iov informatino
		unsigned int m_time_min, m_time_max;
		unsigned int m_run_min, m_run_max;
	//! output file
		TFile *p_outfile;
	};

}

#endif
