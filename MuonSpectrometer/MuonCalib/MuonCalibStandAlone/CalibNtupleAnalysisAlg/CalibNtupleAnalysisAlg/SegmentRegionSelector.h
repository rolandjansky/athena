/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_SegmentRegionSelector_h
#define MuonCalib_SegmentRegionSelector_h

//Gaudi
#include "GaudiKernel/ServiceHandle.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/CalibSegmentPreparationTool.h"

class RegionSelectionSvc;

namespace MuonCalib {

class SegmentRegionSelector : public AlgTool, virtual public CalibSegmentPreparationTool
	{
	public:
//=========================constructor==========================================
		SegmentRegionSelector(const std::string & t, const std::string & n, const IInterface *p);
		inline ~SegmentRegionSelector() {}
//=========================public member functions==============================
	//initialize and finalize
		StatusCode initialize(void);
		inline StatusCode finalize(void)
			{
			return StatusCode :: SUCCESS;
			}
	//load event
		void prepareSegments(const MuonCalibEvent *& event, std::map<NtupleStationId, MuonCalibSegment *> & segments);
	private:
//=========================private data=========================================
		//if set to true, segments wich are not comletely in the selected calibration region will be dropped - job options
			bool m_exclusive_segments;
		//minimum and maximum number of segment hits
			int m_min_hits, m_max_hits;
		//select segment author
			int m_select_seg_author;
		//adc cut
			int m_adc_cut;
	//! pointer to regino selectin service
			RegionSelectionSvc *p_reg_sel_svc;
		
	};

} //namespace MuonCalib

#endif
