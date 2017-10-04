/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBSTANDALONETOOLS_NTUPLEWIRECENTRICITYTOOL_H
#define MUONCALIBSTANDALONETOOLS_NTUPLEWIRECENTRICITYTOOL_H


//c - c++
#include "map"

//root
class TFile;
class TH1F;
#include "TTree.h"

//MuonCalibIdentifier
#include "MuonCalibIdentifier/MuonFixedId.h"

//this
#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

class MdtCalibInputSvc;

namespace MuonCalib {

class MuonCalibHitBase;

/** @class NtupleWireCentricityTool
	Checks for the centricity of the anode wire
	@author rauscher@cern.ch
	
	*/
	
class NtupleWireCentricityTool : public AthAlgTool, virtual public NtupleCalibrationTool
	{
	public:
//==============================================================================
	/** Tool Constructor*/
		NtupleWireCentricityTool(const std::string& t, const std::string& n, const IInterface* p);
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
	/** we dont need to keep the segments*/
		inline bool KeepSegments() const
			{
			return false;
			}
		void setRegion();
//==============================================================================
	private:
	//! minimum track track slope - job option
		double m_min_track_slope;
	//! width of region in the middle of the tube tobe analysed - job option
		double m_region_width;	
	//! root file
		TFile *p_root_file;
	//! drift time spectra for left and right side of the wire
		TH1F *p_dt_spec_left, *p_dt_spec_right;
	//! drift time spectra for tracks above, and below the wire
		TH1F *p_dt_spec_above, *p_dt_spec_below;
	//! track slope
		TH1F *p_track_slope;
	//! track slope
		TH1F *p_track_slope_cut;
	//! x coordinate
		TH1F *m_x_coordinate;
	//! track radius
		TH1F *m_track_radius;
	//! calibration input service
		ServiceHandle<MdtCalibInputSvc> m_calib_input_svc;
	//! drift velocity for large radii
		Double_t m_drift_velocity;
	//! region id
		Int_t m_station, m_eta, m_phi, m_hw_phi;
	//!tree data
		Double_t m_mean_track_slope;
		Int_t m_n_hits;
			
//==============================================================================
		inline void process_hit(const MdtCalibHitBase * hit, const double & track_slope);
	};
		
		


}



#endif
