/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib__InitialDqFilter_h
#define MuonCalib__InitialDqFilter_h

//Gaudi
#include "GaudiKernel/ServiceHandle.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/CalibSegmentPreparationTool.h"

//c- c++
#include "set"


namespace MuonCalib {

class InitialDqFilter: public AlgTool, virtual public CalibSegmentPreparationTool
	{
	public:
//=========================constructor==========================================
		InitialDqFilter(const std::string & t, const std::string & n, const IInterface *p);
		inline ~InitialDqFilter() {}
//=========================public member functions==============================
	//initialize and finalize
		StatusCode initialize(void);
		inline StatusCode finalize(void)
			{
			return StatusCode :: SUCCESS;
			}
	//load event
		void prepareSegments(const MuonCalibEvent *& /*event*/, std::map<NtupleStationId, MuonCalibSegment *> & segments);
	private:
//=========================private data=========================================
	//location of initial dq file - jo
		std::string m_initial_dq_path;
	//list of suppressed tubes
		std::set<MuonFixedId> m_suppressed_tubes;
	};

} //namespace MuonCalib
#endif
