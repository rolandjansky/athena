/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 20.06.2007, AUTHOR: Zhen YAN
//  Used for ADC data studies and product the ADC parameters for time slewing
//   2009.01.14   new Tool NtupleCalibADCTool 
//   2009.01.21   Empty frame is working in Athena frame
//   2009.01.22   Add ADC plot for ADC data studies
//   2009.02.26   Add new array histogram for radius range analysis
//   2009.03.06   Add new array histogram for studing the ADC varation with differnet segment AngleXZ
//   2009.06.22   Test the current version in new package, working on the producting of timeslewing coefficient
//   2009.07.02   v320 input_svc ->T0 and RT , slewing factor from r_MDT_residual_vs_ADC[15]
//   2009.07.15   v330 TS_factor[15] change range.  Range 0 to 0.5, 0.5~1.5£¬13.5~15
//   2009.07.23   v400 introduce the TS_cor and reset the driftRadius before the refit. new plot Residual_vs_Radius[3]
//   2009.08.27   v440 new plot  radius_vs_distanceTowire  ->> check the bugs on the NTUP filling
//   2009.08.31   v500 running version except r_MDT_residual_vs_ADC[15]
//   2009.09.02   v510 new ADC correction array, compile ok
//
//   Todo list    1) Try to use GTFitter for the segment refit
//                2) Introduce TrRelation() class in CalibData
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_NtupleCalibADCToolH
#define MuonCalib_NtupleCalibADCToolH

//:::::::::::::::::::::::::::::::::::::::
//:: CLASS NtupleCalibADCTool ::
//:::::::::::::::::::::::::::::::::::::::

/// \class NtupleCalibADCTool

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

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// MuonCalib //
#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MdtCalibFitters/QuasianalyticLineReconstruction.h"
#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "MdtCalibFitters/CurvedPatRec.h"
#include "MuonCalibStandAloneBase/T0Refinement.h"
#include "MdtCalibUtils/GlobalTimeFitter.h"
#include "MdtCalibFitters/DCSLFitter.h"
#include "MdtCalibIOSvc/MdtCalibInputSvc.h"
class RegionSelectionSvc;

namespace MuonGM {
class MuonDetectorManager;
}

namespace MuonCalib {

class T0MTHistos;

class NtupleCalibADCTool : public AthAlgTool, 
					virtual public NtupleCalibrationTool {

public:
// Constructors //
	NtupleCalibADCTool(const std::string & t,
			const std::string & n, const IInterface * p);
	///< Constructor of the tool.

	~NtupleCalibADCTool(void);
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
        bool m_Recalc2ndCoordinate;  // use for certifate ADC_vs_distX and ADC_vs_DirXYZ
	bool m_curved_fits; // = true, if curved segment fits should be performed,
	                    // = false, if straight segment fits should be performed
	double m_time_out; // time out for pattern finding
	double m_road_width; // road width for pattern recognition
	std::string m_MDT_ID_helper; // name of the MDT ID helper
	std::string m_RPC_ID_helper; // name of the RPC ID helper
	std::string m_idToFixedIdToolType; // type of the muon fixed id tool
	std::string m_idToFixedIdToolName; // name of the muon fixed id tool
	std::string m_ROOT_file_name; // name of the output ROOT file
	//std::string m_group_by; // name of the output ROOT file
//	double RA_table[30];

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
																	

// event data //
						// number of raw MDT hits in
						// the chambers
	unsigned int m_nb_events;
						// number of events
									  // strips [rDoublet][zDoublett]
// straight-line fitter //
	QuasianalyticLineReconstruction *m_qfitter;

// curved-line fitter //
        CurvedPatRec *m_cfitter;

// t0 refinement //
	T0Refinement *m_t0_refinement;

// histograms //
   // segment histograms //
// ##########  Group 1, the histograms for generating ADC correction factor ###############

        std::map<NtupleStationId, TH2F *> m_MDT_adc_vs_radius;
        std::map<NtupleStationId, TH2F *> m_MDT_adc_vs_pathD;
        std::map<NtupleStationId, TH2F *> m_MDT_adc_vs_residual;
        std::map<NtupleStationId, TH2F *> m_MDT_adc_vs_posX;
        std::map<NtupleStationId, TH2F *> m_MDT_adc_vs_DistanceToSegmentTrack;
        std::map<NtupleStationId, TH2F *> m_MDT_radius_vs_t;
        std::map<NtupleStationId, TH2F *> m_MDT_radius_vs_DistanceToSegmentTrack;


        std::map<NtupleStationId, TH2F*>  m_r_MDT_residual_vs_adc[15]; 
	
	std::map<NtupleStationId, TH1F *> m_MDT_segment_localangle_YZ;	//local incidence angle XZ, YZ distributions of the segments
        std::map<NtupleStationId, TH1F *> m_MDT_segment_localangle_XZ;

// ##########  Group 2, Histograms for compare the refit inprovement  ###############
        
        std::map<NtupleStationId, TH2F *> m_MDT_residual_vs_radius[3]; 
	std::map<NtupleStationId, TH1F *> m_MDT_segment_hits[3];
	std::map<NtupleStationId, TH1F *> m_MDT_segment_chi2[3];
	std::map<NtupleStationId, TH1F *> m_MDT_segment_delta_chi2;
//	std::map<NtupleStationId, TH1F *> m_MDT_segment_delta_residual;
 

// ##########  Group 3, Histograms for ADC specify checking   ###############
// 3.1  double hits -> ADC distribution  v440
        std::map<NtupleStationId, TH2F *> m_MDT_shit_adc;
        std::map<NtupleStationId, TH2F *> m_MDT_fhit_adc;
// 3.2  big residual hits -> working on

// helpers //
//


	const MdtIdHelper *m_MdtIdHelper; // pointer to the MDT ID helper
	const RpcIdHelper *m_RpcIdHelper; // pointer to the RPC ID helper
	
	const MuonGM::MuonDetectorManager *m_detMgr; // pointer to the muon
						     // detector manager
	const MuonGM::RpcDetectorElement *m_rpcDetEl; // pointer to the RPC detector element 
	const MuonCalib::IIdToFixedIdTool *m_id_tool; // identifier converter

// private methods //
	void createMaps(const MuonFixedId & id);
					// create the histograms for the
					// given station id if they do not
					// exist
};

}

#endif
