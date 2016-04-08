/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _NtupleMTT0Tool_H
#define _NtupleMTT0Tool_H


//this
#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
class RegionSelectionSvc;

//t0 algorithm
#include "MdtCalibT0/T0CalibrationMT.h"
#include "MdtCalibT0/T0MTSettings.h"

namespace MuonCalib {

/** @class NtupleMTT0Tool
	Interface tool for the MT T0-Fitter
	@author rauscher@cern.ch
	*/
	
class NtupleMTT0Tool: public AthAlgTool, virtual public NtupleCalibrationTool
	{
	public:
//==============================================================================
	/** Tool Constructor*/
		NtupleMTT0Tool(const std::string& t, const std::string& n, const IInterface* p);
	/** tool initialization */
		StatusCode initialize();
	/** tool finalization */
		inline StatusCode finalize()
			{
			return StatusCode::SUCCESS;
			}
	/**analyse event - fill hits into specra*/
		StatusCode handleEvent(const MuonCalibEvent &/*event*/, int /*evnt_nr*/, const std::vector<MuonCalibSegment *> &segments, unsigned int position);
	/** end of events analysis */
		StatusCode analyseSegments(const std::vector<MuonCalibSegment *> & /*segemnts*/);
	/** set calibratino region */
		void setRegion();
	/** get calibration results */
		const IMdtCalibrationOutput * getResults() const;
	/** we dont need to keep the segments*/
		inline bool KeepSegments() const
			{
			return false;
			}
//==============================================================================
	private:
	//! pointer to algorithm
		T0CalibrationMT	* p_alg;
	//! settings class
		T0MTSettings m_settings;
	//! pointe rto region selectin service
		ServiceHandle<RegionSelectionSvc> m_reg_sel_svc;
	//! number of negative and positive bins
		int m_n_bins_neg, m_n_bins_pos;
	//! use top chi2 instead of the normal one
		bool m_use_top_chi2;
	//! threshold for the application of the scrambling method
		double m_scramble_thresh, m_sclicing_thresh;
	//! minimum number of entries per fit
		int m_min_entries_time, m_min_entries_adc;
	//! group by
		std::vector<std::string> m_group_by;
		std::vector<int> m_sort_by;
		std::vector<std::string> m_adc_group_by;
		std::vector<int> m_adc_sort_by;
	//! correct for relative t0
		bool m_correct_rel_t0;
		
	inline bool eval_group_by(const std::string & group_by, int & sort_by);	
	};

}


#endif
