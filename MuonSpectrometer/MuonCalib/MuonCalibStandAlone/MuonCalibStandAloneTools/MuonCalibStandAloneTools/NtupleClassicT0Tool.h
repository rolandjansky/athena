/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _NtupleClassicT0Tool_H
#define _NtupleClassicT0Tool_H


//this
#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
class RegionSelectionSvc;

//t0 algorithm
#include "MdtCalibT0/T0CalibrationClassic.h"


namespace MuonCalib {

/** @class NtupleClassicT0Tool
	Interface tool for the MT T0-Fitter
	@author rauscher@cern.ch
	*/
	
class NtupleClassicT0Tool: public AthAlgTool, virtual public NtupleCalibrationTool
	{
	public:
//==============================================================================
	/** Tool Constructor*/
		NtupleClassicT0Tool(const std::string& t, const std::string& n, const IInterface* p);
	/** tool initialization */
		StatusCode initialize();
	/** tool finalization */
		inline StatusCode finalize()
			{
			return StatusCode::SUCCESS;
			}
	/**analyse event - fill hits into specra*/
		StatusCode handleEvent(const MuonCalibEvent &/*event*/, int /*evnt_nr*/, const std::vector<MuonCalibSegment *> &segments, unsigned int position);
	/** analyze segments */
		StatusCode analyseSegments(const std::vector<MuonCalibSegment *> & /*segments*/);
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
		T0CalibrationClassic * p_alg;
	//! settings class
		T0ClassicSettings *m_settings;
	//! pointe rto region selectin service
		ServiceHandle<RegionSelectionSvc> m_reg_sel_svc;
	//! number of adc bins
		int m_adcBins;
	//! adc range
		double 	m_minAdc, m_maxAdc;
	//! number of time bins
		int m_timeBins;
	//! time range
		double m_minTime, m_maxTime;
	//! if true fit time
		bool m_fitTime;
	//! minimum number of entries
		int m_minEntries;
	//! init param marameter
		int m_initParam;
	//! maximum value for chi2
		double m_chiMax;
	};

}


#endif
