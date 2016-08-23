/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 13.08.2008, AUTHOR: MAURO IODICE 
//             ALGORITHM TAKEN FROM STEFFEN KAISER slightly modified
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_MdtDqaTubeEfficiencyH
#define MuonCalib_MdtDqaTubeEfficiencyH

//::::::::::::::::::::::::::::::::::::
//:: CLASS MdtDqaTubeEfficiency     ::
//::::::::::::::::::::::::::::::::::::

/// \class MdtDqaTubeEfficiency
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
// #include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"
#include "MdtCalibIOSvc/MdtCalibInputSvc.h"
class RegionSelectionSvc;
class HistogramManager;

//root 
class TFile;
class TNtuple;
class TH1F;
class TH1I;

class StoreGateSvc; 
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


class MdtDqaTubeEfficiency {
  
 public:
  // Constructors //
  MdtDqaTubeEfficiency(float nsigma, float chi2Cut,
		       bool defaultResol, float adcCut, bool GTFitON, bool useNewCalibConstants, bool useTimeCorrections );

  ///< Default constructor.
  inline ~MdtDqaTubeEfficiency(){}
  ///< Destructor

  // Methods //
	
  /** tool initialize */
  StatusCode initialize(const MdtIdHelper*, const MuonGM::MuonDetectorManager*, const MuonCalib::IIdToFixedIdTool *,
			RegionSelectionSvc *, MdtCalibInputSvc *,  HistogramManager *);
  
  /** tool finalize */
  StatusCode finalize();
	
  StatusCode handleEvent(const MuonCalibEvent &event, int evnt_nr, const std::vector<MuonCalibSegment *> &segments, unsigned int position);
  ///< analysis of the given segment of
  ///< event eventnumber, event contains
  ///< the raw hits
  StatusCode analyseSegments(const std::vector<MuonCalibSegment *> & /*segemnts*/);
  ///< analysis at the end of the data
  ///< file
  inline bool KeepSegments() const {
    return false;
  }

  inline double defaultResolution(double radius) { 
    double P0 = 2.887916e-01  ;
    double P1 = -4.391942e-02 ;
    double P2 = 3.207732e-03  ;
    double P3 = -1.593081e-04 ;
    double P4 = 9.127890e-06  ;
    double P5 = -2.565923e-07 ;
    double resol = P0+P1*radius+P2*pow(radius,2)+P3*pow(radius,3)+P4*pow(radius,4)+P5*pow(radius,5);
    return resol ;
  } ;

 private:
  const MdtIdHelper *m_mdtIdHelper;
  const MuonGM::MuonDetectorManager *m_detMgr;
  const MuonCalib::IIdToFixedIdTool *m_id_tool;
  // pointer to region selection service
  RegionSelectionSvc *p_reg_sel_svc;
  MdtCalibInputSvc *p_calib_input_svc;

  HistogramManager *m_histoManager ;
  std::string m_chamberAthenaName;
  std::string m_chamberHardwareName;

  // ROOT file and analysis ntuples/trees //
  //TFile   *m_tfile;       // ROOT file
  //TFile   *m_tfile_debug; // ROOT file for debug purposes    
  //TNtuple *m_hit_ntuple;  // hit ntuple
      
  std::string m_file_name; // root file name  
  float m_nsigma;          // efficiency at nsigma*resol(r)
  float m_chi2Cut;         // segment chi2/ndof  cut;
  bool  m_defaultResol;    // use default resolution instead of that found in the hit   
  float m_adcCut;          // only consider hit above adcCut ;
  bool  m_GTFitON;     	   // refit with GlobalTimeFitter the initial segment
  bool m_useNewCalibConstants; 
  bool m_useTimeCorrections;

  // calibration region//
  //const NtupleStationId	*m_cal_region;	
	
  // line reconstruction //
  QuasianalyticLineReconstruction *m_qfitter;    

  // counter/numbers //
  //int m_nb_trigger;  
  // int m_nb_multilayers;
  // int m_nb_layers;
  // int m_nb_tubes;
  int m_nb_stations ;
  int m_nb_layers_tubes[1500][4] ;

  // histograms //
  //TH1F *m_h_distance;
  //TH1I *m_h_nb_hit_tubes;
	/*
        std::vector< std::vector< std::vector<TH1F*> > > h_efficiency;
	std::vector< std::vector< std::vector<TH1F*> > > h_fakerate;

        std::vector< std::vector<TH1F*> > h_tube_efficiency;  
        std::vector< std::vector<TH1F*> > h_tube_fakerate; 

        std::vector< std::vector<TH1F*> > h_tube_entries_efficiency;  
        std::vector< std::vector<TH1F*> > h_tube_entries_fakerate;
        */ 
  //TH1F *m_h_layer_efficiency;
  //TH1F *m_h_layer_fakerate;
  
  //TH1F *m_h_chamber_efficiency;
  //TH1F *m_h_chamber_fakerate;

  //TH1F *m_h_chi2;
  
};

}  //namespace MuonCalib

#endif
