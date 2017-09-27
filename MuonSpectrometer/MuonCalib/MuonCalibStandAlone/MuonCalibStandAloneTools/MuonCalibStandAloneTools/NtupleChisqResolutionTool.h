/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NtupleChisqResolutionTool_H
#define NtupleChisqResolutionTool_H

//c - c++
#include <vector>
#include <map>

// root
class TMinuit;
class TH1F;
class TH2F;
class TFile;

//this
#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
class MdtCalibInputSvc;



namespace MuonCalib {

//muoncalib
class IRtRelation;
class MuonCalibSegment;
class IRtResolution;


class NtupleChisqResolutionTool : public AthAlgTool, virtual public NtupleCalibrationTool
  	{
	public:
//===============================public section=================================
	/** Tool constructor */
		NtupleChisqResolutionTool(const std::string& t, const std::string& n, const IInterface* p);
	/** Destructor */
		~NtupleChisqResolutionTool();
	/** tool initialize */
		StatusCode initialize();
	/** tool finalize */
		StatusCode finalize();
	/** analyse the segments */
		StatusCode analyseSegments(const std::vector<MuonCalibSegment *> & segemnts);
	/** set calibration region */
		void setRegion();
	/** get the resolution */
		const IRtResolution * getResolution() const;
	//!?
		inline void fcn(int &/*npar*/, double */*gin*/, double &f, double *par, int /*iflag*/);
	private:
//============================private section===================================
	//!?
		inline double recalculate(double *par);
	//!?
		const std::vector<MuonCalibSegment*> *m_seg;
//    MuonFixedId *m_name;
    //    void NtupleChisqResolutionTool_fcn_wrapper(int &npar, double *gin, double &f, double *par, int iflag);
    //    static NtupleChisqResolutionTool* static_NtupleChisqResolutionTool_pointer;
		//! some root stuff
		TMinuit *m_minuit;
		TH1F *m_prob_dist;
      		//TH2F *m_prob_debg;
		int m_minuitoutputlevel;
		TFile * m_resolfile;
	//! Calibration input service
		ServiceHandle<MdtCalibInputSvc> m_calib_input_svc;
	//! the rt relation
		const IRtRelation *p_rt_rel;
    
    		IRtResolution *m_final_resolution; // final resolution curve
    		inline double t_from_r(const double & r, const IRtRelation * rt_rel);
	                        // get t(r) for the r-t relationship rt_rel,
				// the method is auxiliary and not optimized;
				// it will disappear when the t(r) will be
				// available in the MuonCalib framework;

  };
}

#endif 
