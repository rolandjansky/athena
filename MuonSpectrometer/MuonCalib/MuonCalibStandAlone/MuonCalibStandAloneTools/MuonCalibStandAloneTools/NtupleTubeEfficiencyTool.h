/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 10.04.2007, AUTHOR: STEFFEN KAISER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_NtupleTubeEfficiencyToolH
#define MuonCalib_NtupleTubeEfficiencyToolH

//::::::::::::::::::::::::::::::::::::
//:: CLASS NtupleTubeEfficiencyTool ::
//::::::::::::::::::::::::::::::::::::

/// \class NtupleTubeEfficiencyTool
///
/// This class determines the single tube efficiencies and fakerates
/// and writes them to a TubeEfficiencies_<regionId>.txt and .root file.
/// (The fakerate only compares the raw hits to the tubes traversed by 
/// one segment.)
///
/// The combined efficiencies for the layer, multilayer and chamber are 
/// calculated as weighted mean of the single tube efficiencies, according to
///
/// eff   = (Sum_i 1/sigma_i^2 * eff_i)/(Sum_i 1/sigma_i^2)  
/// sigma = Sqrt( 1/(Sum_i 1/sigma_i^2) )   
///
/// \author Steffen.Kaiser@cern.ch
///
/// \date 10.04.2007

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// this
#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
class RegionSelectionSvc;

//root 
class TFile;
class TNtuple;
class TH1F;
class TH1I;

class MdtIdHelper;

namespace MuonGM{
class MuonDetectorManager;
}

namespace MuonCalib {

class QuasianalyticLineReconstruction;
class IIdToFixedIdTool;
class MuonCalibSegment;
class NtupleStationId;
class MuonCalibEvent;


class NtupleTubeEfficiencyTool : public AthAlgTool, virtual public NtupleCalibrationTool
	{

    public:
        // Constructors //
        NtupleTubeEfficiencyTool(const std::string& t, const std::string& n, const IInterface* p);
        ///< Default constructor.

        inline ~NtupleTubeEfficiencyTool(){}
        ///< Destructor

        // Methods //
	
	/** tool initialize */
	StatusCode initialize();
	
	/** tool finalize */
	StatusCode finalize();
	
        StatusCode handleEvent(const MuonCalibEvent & event, int evnt_nr, const std::vector<MuonCalibSegment *> &segments, unsigned int position);
        ///< analysis of the given segment of
        ///< event eventnumber, event contains
        ///< the raw hits
        StatusCode analyseSegments(const std::vector<MuonCalibSegment *> & /*segemnts*/);
        ///< analysis at the end of the data
        ///< file
	inline bool KeepSegments() const
		{
		return false;
		}
    private:

	const MdtIdHelper* m_mdtIdHelper;
	const MuonGM::MuonDetectorManager* m_detMgr;
	const MuonCalib::IIdToFixedIdTool *m_id_tool;

        // ROOT file and analysis ntuples/trees //
        TFile   *m_tfile;       // ROOT file
        TFile   *m_tfile_debug; // ROOT file for debug purposes    
        TNtuple *m_hit_ntuple;  // hit ntuple
      
        std::string m_file_name; //root file name  
        double m_nb_hits;        //minimum number of hits in segment
        double m_road_width;     //road width for pattern recognition
        bool   m_debug;          //write debugging rootfile
        double m_chi2_cut;       //track chi2 cut;
        bool   m_exclude_layer;  //exclude layer and refit for eff determination    

        // calibration region//
	//const NtupleStationId	* m_cal_region;	
	
        // line reconstruction //
        QuasianalyticLineReconstruction *m_qfitter;    
	//! pointe rto region selectin service
	ServiceHandle<RegionSelectionSvc> m_reg_sel_svc;

        // counter/numbers //
        int m_nb_trigger;  
        int m_nb_multilayers;
	int m_nb_layers;
	int m_nb_tubes;
	
        // histograms //
       	TH1F *m_h_distance;
	TH1I *m_h_nb_hit_tubes;
	
        std::vector< std::vector< std::vector<TH1F*> > > m_h_efficiency;
	std::vector< std::vector< std::vector<TH1F*> > > m_h_fakerate;

        std::vector< std::vector<TH1F*> > m_h_tube_efficiency;  
        std::vector< std::vector<TH1F*> > m_h_tube_fakerate; 

        std::vector< std::vector<TH1F*> > m_h_tube_entries_efficiency;  
        std::vector< std::vector<TH1F*> > m_h_tube_entries_fakerate;

        TH1F *m_h_layer_efficiency;

        TH1F *m_h_chamber_efficiency;

	TH1F *m_h_chi2;
	
    };

}

#endif
