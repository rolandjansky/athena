/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 05.09.2008, AUTHOR: MAURO IODICE 
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_MdtDqaGlobalTimeFitH
#define MuonCalib_MdtDqaGlobalTimeFitH

//::::::::::::::::::::::::::::::::::::
//:: CLASS MdtDqaGlobalTimeFit     ::
//::::::::::::::::::::::::::::::::::::

/// \class MdtDqaGlobalTimeFit
///
/// This class determines 
///
/// \author mauro.iodice@roma3.infn.it
///
/// \date 05.09.2008

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// this
// #include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"

#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibData/RtRelationLookUp.h"
#include "MdtCalibData/MdtCalibrationFactory.h"

#include "MdtCalibUtils/GlobalTimeFitter.h"

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

// NOT MAINTAINED ANYMORE  -- class MuCCaFitter;
class DCSLFitter;

class MdtDqaGlobalTimeFit {

 public:
  // Constructors //
  MdtDqaGlobalTimeFit(int nbHitsMin, int nbHitsMax, float chi2cut, int BfieldON, bool debug);
  ///< Default constructor.

  inline ~MdtDqaGlobalTimeFit(){}
  ///< Destructor

  // Methods //
	
  /** tool initialize */
  StatusCode initialize(const MdtIdHelper*, const MuonGM::MuonDetectorManager*, const MuonCalib::IIdToFixedIdTool *,RegionSelectionSvc *, HistogramManager *);
  
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

  IRtRelation *getDefaultRtRelation(int BfieldON);
  
 private:
  const MdtIdHelper *m_mdtIdHelper;
  const MuonGM::MuonDetectorManager *m_detMgr;
  const MuonCalib::IIdToFixedIdTool *m_id_tool;
  // pointer to region selection service
  RegionSelectionSvc *p_reg_sel_svc;

  HistogramManager *m_histoManager;
  std::string m_chamberAthenaName;
  std::string m_chamberHardwareName;

  // ROOT file and analysis ntuples/trees //
  //TFile   *m_tfile;       // ROOT file
  //TFile   *m_tfile_debug; // ROOT file for debug purposes    
  //TNtuple *m_hit_ntuple;  // hit ntuple
      
  std::string m_file_name; //root file name  
  //float  m_nb_hits;        //minimum number of hits in segment
  int    m_minNumHits;     //minimum number of hits in segment
  int    m_maxNumHits;     //maximum number of hits in segment
  float  m_chi2_cut;       //track chi2 cut;
  int    m_BfieldON;       // BField was ON (1) or OFF (0) used for rtDefault choice;
  bool   m_debug;          //write debugging rootfile

  // calibration region//
  //const NtupleStationId	*m_cal_region;	
	
  // line reconstruction //
  IRtRelation *m_rtRel;
  //QuasianalyticLineReconstruction *m_qfitter;    
  // NOT MAINTAINED ANYMORE    MuCCaFitter * m_muFitter;
  DCSLFitter *m_muFitter;
  GlobalTimeFitter *m_GTFitter;

  // counter/numbers //
  //int m_nb_trigger;  
  // int m_nb_multilayers;
  // int m_nb_layers;
  // int m_nb_tubes;
  //int m_nb_stations;
  //int m_nb_layers_tubes[1500][4];

  // histograms //
  //TH1F *m_htoffset;

  // TH1F *h_distance;
  //TH1I *h_nb_hit_tubes;

	/*
        std::vector< std::vector< std::vector<TH1F*> > > h_efficiency;
	std::vector< std::vector< std::vector<TH1F*> > > h_fakerate;

        std::vector< std::vector<TH1F*> > h_tube_efficiency;  
        std::vector< std::vector<TH1F*> > h_tube_fakerate; 

        std::vector< std::vector<TH1F*> > h_tube_entries_efficiency;  
        std::vector< std::vector<TH1F*> > h_tube_entries_fakerate;
        */ 
	
};

}  //namespace MuonCalib

#endif
