/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 23.04.2007, AUTHOR: I. POTRAP AND O. KORTNER
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef NtupleSimpleResolutionToolH
#define NtupleSimpleResolutionToolH

//////////////////
// HEADER FILES //
//////////////////

//this
#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
//c - c++
#include "string"
#include "vector"

#define RBIN 30

/* ROOT */
class TFile;
class TH1F;
class TH2F;
class TF1;
class TGraphErrors;
class TTree;


class MdtCalibInputSvc;
namespace MuonCalib {

/** MuonCalib **/
class MuonCalibSegment;
class IRtRelation;

class NtupleStationId;


class NtupleSimpleResolutionTool :  public AthAlgTool, virtual public NtupleCalibrationTool
	{
public:
/** Constructors **/
	NtupleSimpleResolutionTool(const std::string& t, const std::string& n, const IInterface* p);

	inline virtual ~NtupleSimpleResolutionTool(void) {
		destruct();
		}

/** Tool initialize **/
	StatusCode initialize();
/** Tool finalize **/
	inline StatusCode finalize()
		{
		return StatusCode :: SUCCESS;
		}

/** Is called at the end of the analysis - The argumet is only there to overload the interface function*/
	StatusCode analyseSegments(const std::vector<MuonCalibSegment *> & segemnts);

        void handle_segment(MuonCalibSegment & seg, double *par_lim_low, double
	   *par_lim_upp);

/** get the final resolution after the algorithm has been run **/
	inline const IRtResolution * getResolution() const
		{
		return m_final_resolution;
		}

/** set calibratino region */
	void setRegion();

protected:
	static const unsigned int MAXNUMHITS;

private:

	void end_of_data(const IRtRelation * rt_rel);
/* resolution curves */
	IRtResolution *m_final_resolution; // final resolution curve

/* segments */
	unsigned int m_number_of_segments; // number of selected segments
	std::vector<MuonCalibSegment *> m_refitted_segment;
					/* vector with the refitted segments
	                   (used to store the segments for the autocalibration iteration) */

	unsigned int m_MINNUMHITS;
/* bining and ranges for Histograms*/
	static const unsigned int SBIN = 500;
        static const unsigned int RESBIN = 60;
	static constexpr float SRANGE = 2.0;
        static constexpr float RRANGE = 15.0;
        static constexpr float RESRANGE = 6.0;


/* JOB OPTION PARAMETERS */
/* do curved track fit - job option */
	bool m_curved;
/* track error entries limit - job optins */
	float m_REJ_LOW, m_REJ_TOP;
/* Relative difference betwen iterations for convergence. - job optins */
	float m_DELTA_CONV, m_DIFF_MAX;
/* maximimum number of iteration - job options */
	int m_MAX_NUM_IT;
	bool m_hist_fit_method;
	bool m_replace_with_flat;
/* Name of output file with resolution curve */
	std::string m_spr_out_name;
/* Outlier removal */
	bool m_suppress_outliers;
	float m_SUPPRESS_LOW, m_SUPPRESS_TOP;
	float m_low[RBIN], m_top[RBIN];

/* Calibration input service */
	ServiceHandle<MdtCalibInputSvc> m_calib_input_svc;
/* calibration region */
	ServiceHandle<RegionSelectionSvc> m_reg_sel_svc;

/* rt relation */
	const IRtRelation *m_rt_relation;

/* is true if initialize is called - needed for genvonf */
	bool m_is_initialized;

/*	TFile *m_sfile; // ROOT file */
	TFile *m_outfile;

	TH2F *m_residual;
	TH2F *m_residual0;
	TH2F *m_residual2;
	TH2F *m_residual3;
	
	TH2F *m_sigma_track;
	TH2F *m_sigma_track_lim;

	TF1 *m_sprfun; //
	TF1 *m_sprfun_r; //
	TGraphErrors *m_sprgr;
	TGraphErrors *m_sprgr_r;
/*
	TTree *m_ntuple; // ntuple for debug purposes
// Declaration of Ntuple variables
	int hit_st_name, hit_eta, hit_phi, hit_layer, hit_number_hits;
	double hit_drift_time, hit_drift_radius, hit_sigma_drift_radius, hit_sigma_distance_to_track;
	double hit_radial_residual, hit_track_direction, hit_signed_distance;	
*/

/* private methods */
	void destruct(void); /* destruction routine */

	int trackFit(int num_hit, double *dist_hit, double *y,
		double *sigma2_hit, double &shift, double &sigma_track);

	int trkErrorFix(double *par_l, double *par_u);

	int spResCalc(bool SfromR);

	double t_from_r(const double & r, const IRtRelation * rt_rel);
	                       /* //get t(r) for the r-t relationship rt_rel,
												// the method is auxiliary and not optimized;
												// it will disappear when the t(r) will be
												// available in the MuonCalib framework; */

/* Functions for Root Fitting */
	static double ttt(double *x, double *par);
	static double ttt0(double *x, double *par);
};

}

#endif
