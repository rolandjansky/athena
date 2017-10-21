/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 20.06.2007, AUTHOR: OLIVER KORTNER
// Modified: 26.06.2007 by O. Kortner, new naming convention for directories:
//                                     station-phi-eta.
//           26.06.2007 by O. Kortner, new histograms added.
//           19.07.2007 by O. Kortner, no t0 refinement without r-t calibration.
//           18.08.2008 by O. Kortner, curved track fit + time-out option added.
//           03.11.2008 by O. Kortner, allow the user to turn off the t0
//                                     refinement histograms.
//           21.02.2009 by O. Kortner, additional residual histograms
//                                     after t0 refinement.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_NtupleControlHistogramsToolH
#define MuonCalib_NtupleControlHistogramsToolH

//:::::::::::::::::::::::::::::::::::::::
//:: CLASS NtupleControlHistogramsTool ::
//:::::::::::::::::::::::::::::::::::::::

/// \class NtupleControlHistogramsTool
///
/// This class makes control histograms for the calibration regions selected
/// by the user. The user can ask for a refit of the segments with the
/// r-t relationship as loaded by the master stand-alone programme via
/// job options.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 20.06.2007

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// STL //
#include <string>
#include <map>

// ROOT //
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
class TF1;
class TTree;
class TProfile;

namespace MuonGM {
class MuonDetectorManager;
class RpcDetectorElement;
}
class RpcIdHelper;

// MuonCalib //
#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MdtCalibFitters/StraightPatRec.h"
#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "MdtCalibFitters/StraightPatRec.h"
#include "MdtCalibFitters/CurvedPatRec.h"
#include "MuonCalibStandAloneBase/T0Refinement.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
class RegionSelectionSvc;
class MdtCalibInputSvc;

namespace MuonCalib {

class T0MTHistos;

class NtupleControlHistogramsTool : public AthAlgTool, 
					virtual public NtupleCalibrationTool {

public:
// Constructors //
	NtupleControlHistogramsTool(const std::string & t,
			const std::string & n, const IInterface * p);
	///< Constructor of the tool.

	~NtupleControlHistogramsTool(void);
	///< Destructor of the tool.

// Methods //
// methods required by the base classes //
	StatusCode initialize(void);
					///< initialization of the tool
	StatusCode finalize(void);
					///< finilization of the tool
	StatusCode handleEvent(const MuonCalibEvent & event, int evnt_nb,
			const std::vector<MuonCalibSegment *> & segments,
			unsigned int position);
					///< analysis of the event,
					///< raw data and segment histograms
					///< are filled
	bool KeepSegments(void) const;
					///< returns true, if segments should
					///< be stored in memory, false
					///< otherwise

private:
// job options //
	bool m_refit_segments; // = true, if the segments should be refitted,
						   // = false, if the segments are not refitted
	bool m_curved_fits; // = true, if curved segment fits should be performed,
	                    // = false, if straight segment fits should be performed
    bool m_refine_t0; // = true, if t0 refinement plots should be made,
                      // = false otherwise
	double m_time_out; // time out for pattern finding
	double m_road_width; // road width for pattern recognition
	std::string m_MDT_ID_helper; // name of the MDT ID helper
	std::string m_RPC_ID_helper; // name of the RPC ID helper
	std::string m_idToFixedIdToolType; // type of the muon fixed id tool
	std::string m_idToFixedIdToolName; // name of the muon fixed id tool
	std::string m_ROOT_file_name; // name of the output ROOT file

// ROOT file //
	TFile *m_tfile; // pointer to the ROOT file in which the data have to
	                // be stored

// calibration region //
	ServiceHandle<RegionSelectionSvc> m_reg_sel_svc;
// calibration parameters //
 	ServiceHandle<MdtCalibInputSvc> m_input_service; // pointer to the calibration input
	                                   // service

// chamber parameters //
	std::map<NtupleStationId, unsigned int> m_nb_ml; // number of
							 // multilayers
	std::map<NtupleStationId, unsigned int> m_nb_ly; // number of layers per
							 // multilayer
	std::map<NtupleStationId, unsigned int> m_nb_tb; // number of tubes per
						         // layer
	std::map<NtupleStationId, unsigned int> m_nb_totalEtaStrips; // number of eta strips 
	std::map<NtupleStationId, unsigned int> m_nb_totalEtaStrips2; // number of eta strips in 
																				//	RDoublet=2 rpcs

// event data //
	std::map<NtupleStationId, unsigned int> m_nb_raw_MDT_hits;
						// number of raw MDT hits in
						// the chambers
	unsigned int m_nb_events;
						// number of events
	int m_nb_etaStrips[2][4]; // number of rpc eta 
									  // strips [rDoublet][zDoublett]
// straight-line fitter //
	StraightPatRec *m_qfitter;

// curved-line fitter //
    CurvedPatRec *m_cfitter;

// t0 refinement //
	T0Refinement *m_t0_refinement;

// histograms //
   // raw-data histograms //
	std::map<NtupleStationId, TH1F *> m_MDT_raw_nb_hits;
				// number of raw hits per MDT chamber
	std::map<NtupleStationId, TH1F *> m_MDT_raw_hit_distribution;
				// distribution of raw hits per MDT chamber
	std::map<NtupleStationId, TH1F *> m_MDT_raw_TDC;
				// raw TDC spectrum per MDT chamber
	std::map<NtupleStationId, TH1F *> m_MDT_raw_ADC;
				// raw ADC spectrum per MDT chamber
   //histograms to check mdt-rpc correlations
	std::map<NtupleStationId, TH1F *> m_RPC_raw_etaStrip;
	std::map<NtupleStationId, TH2F *> m_MDT_RPC_raw_tubeStripCor;
	std::map<NtupleStationId, TH2F *> m_MDT_RPC_raw_tubeStripCor_adcCut;
	std::map<NtupleStationId, TH2F *> m_MDT_RPC_raw_tubeStripCorVsEvent;
	std::map<NtupleStationId, TH2F *> m_MDT_RPC_raw_tubeStripCorVsEvent_adcCut;
	std::map<NtupleStationId, TH1F *> m_RPC_raw_etaStrip2;
	std::map<NtupleStationId, TH2F *> m_MDT_RPC_raw_tubeStripCor2;
	std::map<NtupleStationId, TH2F *> m_MDT_RPC_raw_tubeStripCor_adcCut2;
	std::map<NtupleStationId, TH2F *> m_MDT_RPC_raw_tubeStripCorVsEvent2;
	std::map<NtupleStationId, TH2F *> m_MDT_RPC_raw_tubeStripCorVsEvent_adcCut2;

   //histograms to check db
   	std::map<NtupleStationId, TH1F *> m_t0_diff;
				// t0 from tuple minus t0 from file
	std::map<NtupleStationId, TH1F *> m_r_mintrt;
				// r minus r(t)
	TH1F *m_t0_diff_global, *m_r_mintrt_global;
	TH2F *m_r_minrt_vs_r;

   // segment histograms //
  	std::map<NtupleStationId, TH1F *> m_MDT_segment_hits;
				// number of hits on the MDT segment
	std::map<NtupleStationId, TH1F *> m_MDT_segment_hist_to_raw_hits_ratio;
				// number of hits on the MDT segment divided
				// by the number of raw hits in the MDT chamber
	std::map<NtupleStationId, TH1F *> m_MDT_segment_CL;
				// confidence level distribution of the segment
				// fits
	std::map<NtupleStationId, TH1F *> m_MDT_segment_TDC;
				// TDC spectra of hits in the segments
	std::map<NtupleStationId, T0MTHistos *> m_MDT_segment_tspec_ml[3];
				//fitter for segment spectra - per multilayer;
	std::map<NtupleStationId, TH1F *> m_MDT_segment_t;
				// drift-time spectra of hits in the segments
	std::map<NtupleStationId, TH1F *> m_MDT_segment_r;
				// drift-radius spectra of hits in the segments
	std::map<NtupleStationId, TH1F *> m_MDT_segment_hit_dis;
				// drift-radius spectra of hits in the segments	
	std::map<NtupleStationId, TH1F *> m_MDT_segment_ADC;
				// ADC spectra of hits in the segments
	std::map<NtupleStationId, TH1F *> m_MDT_segment_slope;
				// slope distributions of the segments
	std::map<NtupleStationId, TH2F *> m_MDT_residuals;
	std::map<NtupleStationId, TH2F *> m_MDT_residuals_vs_x;
	std::map<NtupleStationId, TH2F *> m_MDT_r_vs_rtrack;
	std::map<NtupleStationId, TH2F *> m_MDT_t_vs_rtrack;
	
				// residual distribution per MDT chamber
	std::map<NtupleStationId, TH2F *> m_MDT_residuals_ml1;
				// residual distribution in multilayer 1
	std::map<NtupleStationId, TH2F *> m_MDT_residuals_ml2;
				// residual distribution in multilayer 1
	std::map<NtupleStationId, TH1F *> m_MDT_t0_refinement;
				// delta t0 from track fit
	std::map<NtupleStationId, TH1F *> m_MDT_t_refined;
				// t0 corrected drift time spectrum
	std::map<NtupleStationId, TH2F *> m_MDT_residuals_t0_refined;
				// residuals after t0 refinement
	std::map<NtupleStationId, TH2F *> m_MDT_residuals_t0_refined_gt4hits;
				// residuals after t0 refinement for segments with >4 hitss

//for root tree
//pointer to root tree
	TTree *m_chamber_tree;
// helpers //
	const MdtIdHelper *m_MdtIdHelper; // pointer to the MDT ID helper
	const RpcIdHelper *m_RpcIdHelper; // pointer to the RPC ID helper
	
	const MuonGM::MuonDetectorManager *m_detMgr; // pointer to the muon
						     // detector manager
	const MuonCalib::IIdToFixedIdTool *m_id_tool; // identifier converter

// private methods //
	void createMaps(const MuonFixedId & id);
					// create the histograms for the
					// given station id if they do not
					// exist
	inline double getChi2(const TF1 *fun, const TH1F *hist) const;
	inline double getResidualChi2(TProfile *px) const;
};

}

#endif
