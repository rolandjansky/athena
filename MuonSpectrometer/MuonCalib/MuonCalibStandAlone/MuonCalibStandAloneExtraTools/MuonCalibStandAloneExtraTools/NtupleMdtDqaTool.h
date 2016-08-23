/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 13.08.2008, AUTHOR: MAURO IODICE
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_NtupleMdtDqaToolH
#define MuonCalib_NtupleMdtDqaToolH

//::::::::::::::::::::::::::::::::::::
//:: CLASS NtupleMdtDqaTool         ::
//::::::::::::::::::::::::::::::::::::

/// \class NtupleMdtDqaTool
/// 
/// This class performs low level MdtDqa analysis mainly focusing on
/// hits occupancies and segment quality.
///
/// \author Mauro.Iodice@cern.ch
///
/// \date 13.08.2008 

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"

#include "MuonCalibStandAloneExtraTools/StringUtil.h"
#include "MuonCalibStandAloneExtraTools/HistogramManager.h"
#include "MuonCalibStandAloneExtraTools/MdtDqaDb.h"
#include "MuonCalibStandAloneExtraTools/MdtDqaNtupleAnalysis.h"
#include "MuonCalibStandAloneExtraTools/MdtDqaTubeEfficiency.h"
#include "MuonCalibStandAloneExtraTools/MdtDqaGlobalTimeFit.h"
#include "MuonCalibStandAloneExtraTools/MdtDqaTrackAnalysis.h"
#include "MuonCalibStandAloneExtraTools/SegmentAnalysis.h"

#include "AthenaBaseComps/AthAlgTool.h"

class RegionSelectionSvc;
class MdtCalibInputSvc;

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


class NtupleMdtDqaTool : public AthAlgTool, virtual public NtupleCalibrationTool {

 public:
  // Constructors //
  NtupleMdtDqaTool(const std::string& t, const std::string& n, const IInterface* p);
  ///< Default constructor.

  inline ~NtupleMdtDqaTool(){}
  ///< Destructor

  // Methods //
	
  /** tool initialize */
  StatusCode initialize();
	
  /** tool finalize */
  StatusCode finalize();
	
  StatusCode handleEvent(const MuonCalibEvent &event, int evnt_nr, const std::vector<MuonCalibSegment *> &segments, unsigned int position);
  ///< analysis of the given segment of
  ///< event eventnumber, event contains
  ///< the raw hits
  
  // StatusCode analyseSegments(const std::vector<MuonCalibSegment *> & /*segemnts*/);
  StatusCode analyseSegments(const std::vector<MuonCalibSegment *> &segments);
  
  ///< analysis at the end of the data
  ///< file
  inline bool KeepSegments() const {
    return false;
  }

 private:
  const MdtIdHelper *m_mdtIdHelper;
  const MuonGM::MuonDetectorManager *m_detMgr;
  const MuonCalib::IIdToFixedIdTool *m_id_tool;

  // pointer to region selection service
  RegionSelectionSvc *p_reg_sel_svc;

  // pointer to the calibration service
  MdtCalibInputSvc *p_calib_input_svc;
  // Manager tools //
  HistogramManager * m_histoManager;

  // Analysis tools and variables : //
  //
  // MdtDqaNtupleAnalysis
  //
  MdtDqaNtupleAnalysis *m_ntupleAna;
  MdtDqaTrackAnalysis *m_trackAnalyzer;
  SegmentAnalysis *m_segmentAnalyzer;
  bool   m_verbose; 
  bool   m_fillHistos;
  bool   m_doFinalize;
  std::string m_MdtDqa_file_name; // .root and .txt are root and text file names  
  bool   m_doHitResids; // Toggle between calculating the hit(or fit) residuals for SegResidVsRadius instead
 
  float m_ADCCUT;

  //
  // MdtDqaTubeEfficiency
  //
  bool   m_doEfficiency;
  MdtDqaTubeEfficiency * m_tubeEffi;
  float m_EffiNSigma;        	   // efficiency at nsigma*resol(r)
  float m_EffiChi2Cut;       	   // segment chi2/ndof  cut;
  bool  m_EffiUseDefaultResolution; //use default resolution instead of that found in the hit   
  float m_EffiHitADCCut;      	   // only consider hit above adcCut;
  bool  m_EffiGTFitON;    	   // refit with GlobalTimeFitter the initial segment
  bool  m_EffiUseNewCalibConstants;
  bool  m_EffiUseTimeCorrections;
  
  //
  // MdtDqaGlobalTimeFit
  //
  bool   m_doGlobalTimeFit;
  // bool   m_doGTFit_t0Refit;           // TO BE IMPLEMENTED 
  MdtDqaGlobalTimeFit * m_globalTimeFit;
  int m_rtDefaultBfieldON;
  float m_GTFitSeg_chi2Cut;
  int  m_GTFitSeg_minNumHits;
  int  m_GTFitSeg_maxNumHits;
  bool m_GTFitDebug;
  
  // TRACK ANALYSIS
  bool m_doTracks;
  //bool m_quickTrack;
  int  m_TrkAuthor;
  
  //Dead Tube analysis
  int m_DeadElementsAlgorithm;
  
  // calibration region//
  //const NtupleStationId	*m_cal_region;	
  
  // line reconstruction //
  //QuasianalyticLineReconstruction *m_qfitter;    
  
  // counter/numbers //
  //int m_nb_trigger;  
  //int m_nb_multilayers;
  //int m_nb_layers;
  //int m_nb_tubes;
	
  // histograms //
  //TH1F *h_distance;
  //TH1I *h_nb_hit_tubes;
  
/*
        std::vector< std::vector< std::vector<TH1F*> > > h_efficiency;
	std::vector< std::vector< std::vector<TH1F*> > > h_fakerate;

        std::vector< std::vector<TH1F*> > h_tube_efficiency;  
        std::vector< std::vector<TH1F*> > h_tube_fakerate; 

        std::vector< std::vector<TH1F*> > h_tube_entries_efficiency;  
        std::vector< std::vector<TH1F*> > h_tube_entries_fakerate;
*/
  //TH1F *h_layer_efficiency;
  //TH1F *h_layer_fakerate;
  
  //TH1F *h_chamber_efficiency;
  //TH1F *h_chamber_fakerate;
  
  //TH1F *h_chi2;
  
};

}  //namespace MuonCalib

#endif
